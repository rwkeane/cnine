/*
 * This file is part of cnine, a lightweight C++ tensor library. 
 *  
 * Copyright (c) 2021, Imre Risi Kondor
 *
 * This source code file is subject to the terms of the noncommercial 
 * license distributed with cnine in the file LICENSE.TXT. Commercial 
 * use is prohibited. All redistributed versions of this file (in 
 * original or modified form) must retain this copyright notice and 
 * must be accompanied by a verbatim copy of the license. 
 *
 */

#ifndef _CSRmatrix
#define _CSRmatrix

#include "Cnine_base.hpp"
#include "RtensorA.hpp"
#include "array_pool.hpp"
#include "CSRvector.hpp"


namespace cnine{


  template<class TYPE>
  class CSRmatrix: public array_pool<TYPE>{
  public:

    using array_pool<TYPE>::arr;
    using array_pool<TYPE>::arrg;
    using array_pool<TYPE>::tail;
    using array_pool<TYPE>::memsize;
    using array_pool<TYPE>::dev;
    using array_pool<TYPE>::is_view;
    using array_pool<TYPE>::dir;

    using array_pool<TYPE>::reserve;
    using array_pool<TYPE>::size;
    using array_pool<TYPE>::offset;
    using array_pool<TYPE>::size_of;

    int n=0;
    int m=0;

    mutable CSRmatrix<TYPE>* transpp=nullptr;

    ~CSRmatrix(){
      if(is_view) return;
      if(transpp) delete transpp;
    }
      

  public: // ---- Constructors ------------------------------------------------------------------------------------


    CSRmatrix():
      array_pool<TYPE>(){}

    CSRmatrix(const int _n, const int _m):
      array_pool<TYPE>(_n), n(_n), m(_m){}


  public: // ---- Copying ------------------------------------------------------------------------------------


    CSRmatrix(const CSRmatrix<TYPE>& x):
      array_pool<TYPE>(x){
      n=x.n;
      m=x.m;
    }

    CSRmatrix(CSRmatrix<TYPE>&& x):
      array_pool<TYPE>(std::move(x)){
      n=x.n;
      m=x.m;
    }

    CSRmatrix& operator=(const CSRmatrix<TYPE>& x){
      array_pool<TYPE>::operator=(x);
      n=x.n;
      m=x.m;
      return *this;
    }

  
  public: // ---- Conversions --------------------------------------------------------------------------------


    CSRmatrix(const RtensorA& x):
      CSRmatrix(x.view2()){}

    CSRmatrix(const Rtensor2_view& x):
      CSRmatrix(x.n0,x.n1){
      dir.resize0(x.n0);

      int t=0;
      for(int i=0; i<n; i++)
	for(int j=0; j<n; j++)
	  if(x(i,j)!=0) t++;
      array_pool<TYPE>::reserve(2*t);

      tail=0;
      for(int i=0; i<n; i++){
	dir.set(i,0,tail);
	int m=0;
	for(int j=0; j<n; j++)
	  if(x(i,j)!=0){
	    *reinterpret_cast<int*>(arr+tail+2*m)=j;
	    arr[tail+2*m+1]=x(i,j);
	    m++;
	  }
	dir.set(i,1,2*m);
	tail+=2*m;
      }
    }


  public: // ---- Access -------------------------------------------------------------------------------------


    int nrows() const{
      return n;
    }

    int size_of(const int i) const{
      CNINE_CHECK_RANGE(if(i>=n) throw std::out_of_range("In CSRmatrix::size_of(): index "+to_string(i)+" out of range (0,"+to_string(n-1)+")."));
      return array_pool<TYPE>::size_of(i)/2;
    }

    //int lenght_of_row(const int i){
    //CNINE_CHECK_RANGE(if(i>=n) throw std::out_of_range("In CSRmatrix::length_of_row(): index "+to_string(i)+" out of range (0,"+to_string(n-1)+")."));
    //return size_of(i)/2;
    //}


    TYPE operator()(const int i, const int j) const{
      CNINE_CHECK_RANGE(if(i>=n) throw std::out_of_range("In CSRmatrix::operator(): index "+to_string(i)+" out of range (0,"+to_string(n-1)+")."));
      CNINE_CHECK_RANGE(if(i>=m) throw std::out_of_range("In CSRmatrix::operator(): index "+to_string(i)+" out of range (0,"+to_string(m-1)+")."));
      CNINE_ASSRT(i<size());
      int offs=dir(i,0);
      int s=dir(i,1);
      for(int a=0; a<s; a++)
	if(*reinterpret_cast<int*>(arr+offs+2*a)==j)
	  return arr[offs+2*a+1];
      CNINE_ASSERT(false,"element("+to_string(i)+","+to_string(j)+") not found");
    }

    const CSRvector<TYPE> operator()(const int i) const{
      CNINE_CPUONLY();
      CNINE_CHECK_RANGE(if(i>=size()) throw std::out_of_range("In CSRmatrix::operator(): index "+to_string(i)+" out of range (0,"+to_string(size()-1)+")."));
      return CSRvector<TYPE>(arr+dir(i,0),dir(i,1)/2);
    }

    void for_each(std::function<void(const int, const int, const TYPE)> lambda) const{
      for(int i=0; i<size(); i++){
	int len=size_of(i);
	int offs=offset(i);
	for(int j=0; j<len; j++)
	  lambda(i,*reinterpret_cast<int*>(arr+offs+2*j),arr[offs+2*j+1]);
      }
    }

    void for_each(std::function<void(const int, const CSRvector<TYPE>)> lambda) const{
      for(int i=0; i<size(); i++)
	lambda(i,(*this)(i));
    }

    void push_back(const vector<int>& ix, const vector<TYPE>& v){
      int len=ix.size();
      CNINE_ASSRT(v.size()==len);
      if(tail+2*len>memsize)
	reserve(std::max(2*memsize,tail+2*len));
      for(int i=0; i<len; i++){
	arr[tail+2*i]=ix[i];
	arr[tail+2*i+1]=v[i];
      }
      dir.push_back(tail,2*len);
      tail+=2*len;
    }


  public: // ---- GPU access ---------------------------------------------------------------------------------

    
    int* get_dirg(const int _dev=1){
      return dir.get_arrg(_dev);
    }


  public: // ---- Transposes ---------------------------------------------------------------------------------


    const CSRmatrix& transp() const{
      if(!transpp) make_transp();
      return *transpp;
    }

    void make_transp() const{
      if(transpp) delete transpp;
      transpp=new CSRmatrix<TYPE>(m,n);
      CSRmatrix<TYPE>& T=*transpp;
      T.reserve(tail);

      vector<int> len(m,0);
      for_each([&](const int i, const int j, const TYPE v){
	  len[j]++;});

      T.tail=0;
      for(int i=0; i<m; i++){
	T.dir.set(i,0,T.tail);
	T.dir.set(i,1,2*len[i]);
	T.tail+=2*len[i];
      }

      std::fill(len.begin(),len.end(),0);
      for_each([&](const int i, const int j, const TYPE v){
	  int offs=T.offset(j);
	  *reinterpret_cast<int*>(T.arr+offs+2*len[j])=i;
	  T.arr[offs+2*len[j]+1]=v;
	  len[j]++;
	});
    }


  public: // ---- I/O ----------------------------------------------------------------------------------------


    string str(const string indent="") const{
      ostringstream oss;
      for_each([&](const int i, const CSRvector<TYPE> lst){oss<<indent<<i<<": "<<lst<<endl;});
      return oss.str();
    }

    friend ostream& operator<<(ostream& stream, const CSRmatrix& v){
      stream<<v.str(); return stream;}

  };

}


#endif 