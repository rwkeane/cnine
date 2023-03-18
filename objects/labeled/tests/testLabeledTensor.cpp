/*
 * This file is part of cnine, a lightweight C++ tensor library. 
 *  
 * Copyright (c) 2023, Imre Risi Kondor
 *
 * This source code file is subject to the terms of the noncommercial 
 * license distributed with cnine in the file LICENSE.TXT. Commercial 
 * use is prohibited. All redistributed versions of this file (in 
 * original or modified form) must retain this copyright notice and 
 * must be accompanied by a verbatim copy of the license. 
 *
 */


#include "Cnine_base.cpp"
#include "Ltensor.hpp"
#include "CnineSession.hpp"
#include "Lgrid.hpp"

using namespace cnine;


int main(int argc, char** argv){

  cnine_session session;

  cout<<endl;

  Lgrid grid0({3,3});
  //cout<<grid0<<endl;

  //Ltensor<float> A({grid0});
  Ltensor<float> A=Ltensor<float>::sequential({grid0});
  cout<<A<<endl;

}

