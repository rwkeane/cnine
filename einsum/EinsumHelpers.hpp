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


#ifndef _CnineEinsumHelpers
#define _CnineEinsumHelpers

#include "TensorView.hpp"


namespace cnine{
  namespace einsum{

    class ix_tuple: public vector<int>{
    public:

      typedef vector<int> BASE;

      using BASE::BASE;

      bool contains(const int x) const{
	return std::find(begin(),end(),x)!=end();
      }

      int find(const int x) const{
	auto it=std::find(begin(),end(),x);
	CNINE_ASSRT(it!=end());
	return it-begin();
      }

      string str(){
	ostringstream oss;
	for(int i=0; i<size() ; i++){
	  oss<<"i"<<(*this)[i];
	  if(i<size()-1) oss<<",";
	}
	return oss.str();
      }

    };

    class ix_set: public set<int>{
    public:

      ix_set(){}

      ix_set(const vector<int>& x){
	for(auto p:x) 
	  insert(p);
      }

      bool contains(const int x) const{
	return find(x)!=end();
      }

      vector<int> order(const vector<int>& loop_order){
	vector<int> r;
	for(auto& p:loop_order)
	  if(contains(p)) r.push_back(p);
	return r;
      }

      string str() const{
	ostringstream oss;
	for(auto& p:*this)
	  oss<<static_cast<char>('a'+p);
	return oss.str();
      }

      friend ostream& operator<<(ostream& stream, const ix_set& x){
	stream<<x.str(); return stream;
      }

    };

  }

}

#endif 
