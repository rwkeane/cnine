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


#ifndef _CnineTensor
#define _CnineTensor

#include "Cnine_base.hpp"
#include "TensorView.hpp"

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
  class Tensor: public TensorView<TYPE>{
  public:

    using TensorView<TYPE>::TensorView;
    using TensorView<TYPE>::arr;
    using TensorView<TYPE>::dims;
    using TensorView<TYPE>::strides;


  public: // ---- Constructors ------------------------------------------------------------------------------


    Tensor(const Gdims& _dims, const int _dev=0): 
      TensorView<TYPE>(MemArr<TYPE>(_dims.total(),_dev),_dims,GstridesB(_dims)){}

    Tensor(const Gdims& _dims, const fill_zero& dummy, const int _dev=0): 
      TensorView<TYPE>(MemArr<TYPE>(_dims.total(),dummy,_dev),_dims,GstridesB(_dims)){}

    Tensor(const Gdims& _dims, const fill_sequential& dummy, const int _dev=0):
      Tensor(_dims,_dev){
      int N=dims.total();
      for(int i=0; i<N; i++)
	arr[i]=i;
    }


  public: // ---- Named constructors ------------------------------------------------------------------------


    static Tensor<TYPE> zero(const Gdims& _dims, const int _dev=0){
      return Tensor<TYPE>(_dims,fill_zero(),_dev);
    }

    static Tensor<TYPE> sequential(const Gdims& _dims, const int _dev=0){
      return Tensor<TYPE>(_dims,fill_sequential(),_dev);
    }



  public: // ---- Copying -----------------------------------------------------------------------------------


    Tensor(const Tensor<TYPE>& x):
      Tensor(x.dims,x.dev){
      CNINE_COPY_WARNING();
      view()=x.view();
    }
        
    Tensor(const Tensor<TYPE>& x, const nowarn_flag& dummy):
      Tensor(x.dims,x.dev){
      view()=x.view();
    }
        
    Tensor(const Tensor<TYPE>&& x):
      TensorView<TYPE>(x.arr,x.dims,x.strides,x.dev){
      CNINE_MOVE_WARNING();
    }
        

  public: // ---- Transport -----------------------------------------------------------------------------------


    Tensor(const TensorView<TYPE>& x, const int _dev):
      Tensor(x.dims,_dev){
      CNINE_COPY_WARNING();
      view()=x.view();
    }


  public: // ---- Views -------------------------------------------------------------------------------------


    Tensor(const TensorView<TYPE>& x):
      Tensor(x.dims,x.dev){
      CNINE_CONVERT_WARNING();
      view()=x;
    }

    TensorView<TYPE> view(){
      return TensorView<TYPE>(*this);
    }

    const TensorView<TYPE> view() const{
      return TensorView<TYPE>(*this);
    }


  public: // ---- I/O ---------------------------------------------------------------------------------------


    string classname() const{
      return "Tensor";
    }

    string describe() const{
      ostringstream oss;
      oss<<"Tensor"<<dims<<" ["<<strides<<"]"<<endl;
      return oss.str();
    }

    

    friend ostream& operator<<(ostream& stream, const Tensor<TYPE>& x){
      stream<<x.str(); return stream;
    }

  };


}

#endif