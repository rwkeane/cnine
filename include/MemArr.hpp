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


#ifndef _CnineMemArr
#define _CnineMemArr

#include "Cnine_base.hpp"
#include "MemBlob.hpp"

#ifdef _WITH_CUDA
#include <cuda.h>
#include <cuda_runtime.h>
#endif 

#ifdef _WITH_CUBLAS
#include <cublas_v2.h>
extern cublasHandle_t cnine_cublas;
#endif 


namespace cnine{


  template<typename TYPE>
  class MemArr{
  public:

    typedef std::size_t size_t;


    std::shared_ptr<MemBlob<TYPE> > blob;
    size_t offset=0;

    MemArr(){
      cout<<"empty"<<endl;
      //CNINE_ERROR("MemArr must always be initialized to a new or existing blob of memory");
    }

    // just for taking views of ATen tensors 
    MemArr(MemBlob<TYPE>* _blob):
      blob(_blob){}

    MemArr(const size_t _memsize, const int _dev=0):
      blob(new MemBlob<TYPE>(_memsize,_dev)){}

    MemArr(const size_t _memsize, const fill_zero& dummy, const int _dev=0):
      MemArr(_memsize,_dev){
      if(device()==0) std::fill(blob->arr,blob->arr+_memsize,0);
      if(device()==1) CUDA_SAFE(cudaMemset(blob->arr,0,_memsize*sizeof(TYPE)));
    }

    MemArr(const MemArr& x, const size_t i):
      MemArr(x){
      offset+=i;
    }

    //MemArr(const int _memsize, const fill_sequential& dummy, const int _dev=0):
    //MemArr(_memsize,_dev){
    //CNINE_ASSRT(_dev==0);
    //for(int i=0; i<_memsize; i++)
    //blob->arr[i]=i;
    //}


    // ---- Transport ----------------------------------------------------------------------------------------

    /*
    void move_to_device(const int _dev){
      if(_dev==blob->dev) return;
      MemBlob<TYPE> newblob(new MemBlob<TYPE>(_memsize,_dev));
      if(_dev==0){
	if(device()==1) CUDA_SAFE(cudaMemcpy(newblob->arr,blob->arr,memsize()*sizeof(TYPE),cudaMemcpyDeviceToHost)); 
      }
      if(_dev==1){
	if(device()==0) CUDA_SAFE(cudaMemcpy(newblob->arr,blob->arr,memsize()*sizeof(float),cudaMemcpyHostToDevice));
      }      
    }
    */

    // ---- Copying ------------------------------------------------------------------------------------------


    MemArr& operator=(const MemArr& x){
      blob=x.blob;
      offset=x.offset;
      return *this;
    }


    // ---- Access -------------------------------------------------------------------------------------------


    int device() const{
      return blob->dev;
    }

    TYPE* ptr() const{
      return blob->arr+offset;
    } 

    template<typename TYPE2>
    TYPE2* ptr_as() const{
      return reinterpret_cast<TYPE2*>(blob->arr+offset);
    } 

    TYPE* get_arr(){
      return blob->arr+offset;
    } 

    const TYPE* get_arr() const{
      return blob->arr+offset;
    } 

    TYPE& operator[](const size_t i) const{
      return blob->arr[i+offset];
    }

    TYPE& operator[](const size_t i){
      return blob->arr[i+offset];
    }


    // ---- Operations ---------------------------------------------------------------------------------------


    // experimental
    MemArr operator+(const size_t i) const{
      return MemArr(*this, i);
    }

  };

}

#endif 


