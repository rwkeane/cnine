/*
 * This file is part of cnine, a lightweight C++ tensor library. 
 *  
 * Copyright (c) 2024, Imre Risi Kondor
 *
 * This source code file is subject to the terms of the noncommercial 
 * license distributed with cnine in the file LICENSE.TXT. Commercial 
 * use is prohibited. All redistributed versions of this file (in 
 * original or modified form) must retain this copyright notice and 
 * must be accompanied by a verbatim copy of the license. 
 *
 */

#ifndef _GPUbuffer
#define _GPUbuffer

#include "Cnine_base.hpp"


namespace cnine{

  template<typename TYPE>
  class GPUbuffer{
  public:

    typedef std::size_t size_t;

    TYPE* arr=nullptr;
    size_t _size=0;
    int dev=1;

    ~GPUbuffer(){
      if(arr) CUDA_SAFE(cudaFree(arr));
    }


  public: //---- Constructors -------------------------------------


    GPUbuffer(){}

    GPUbuffer(const int n, const int _dev=1){
      _size=std::max(1,n);
      CUDA_SAFE(cudaMalloc((void **)&arr,_size*sizeof(TYPE)));
    }

    void reset(const int n, const int _dev=1){
      if(n<=_size) return;
      if(arr) CUDA_SAFE(cudaFree(arr));
      _size=std::max(1,n);
      CUDA_SAFE(cudaMalloc((void **)&arr,_size*sizeof(TYPE)));
    }


  public: //---- Access -------------------------------------------


    TYPE* operator()(const int i=0){
      return arr+i;
    }

    
    template<typename TENSOR>
    void push(const int i, const TENSOR& x){
      CNINE_ASSRT(x.get_dev()==0);
      CNINE_ASSRT(i+x.asize()<=_size);
      CUDA_SAFE(cudaMemcpy(arr,x.mem(),x.asize()*sizeof(TYPE),cudaMemcpyHostToDevice));
    }

  };

}

#endif 
