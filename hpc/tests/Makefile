ROOTDIR=../..
include $(ROOTDIR)/common.txt

INCLUDE= $(CNINE_INCLUDES) 

TESTS=$(patsubst %.cpp,%,$(wildcard *.cpp))

DEPS=$(COMBIDIR)/*.hpp 

EXECS= 

CUDA_OBJECTS=  
CUDA_EXTERNS=

ifdef WITH_CUDA
INCLUDE+=-I$(CUDA_HOME)/include
#CUDA_OBJECTS+=$(CETENSORDIR)/CtensorBpack.o 
CUDA_EXTERNS+=$(INCLUDEDIR)/Cnine_base.cu 
CUDA_OBJECTS+=$(CUDADIR)/TensorView_add.o  
CUDA_OBJECTS+=$(CUDADIR)/TensorView_assign.o  
CUDA_OBJECTS+=$(CUDADIR)/TensorView_inc.o  
CUDA_OBJECTS+=$(CUDADIR)/TensorView_gather.o  
endif 


$(TESTS): %: %.cpp
ifdef WITH_CUDA
	$(NVCC) $(NVCCFLAGS) -o $@ $@.cpp $(CUDA_EXTERNS) $(CUDA_OBJECTS) $(CFLAGS) $(MACROS) $(INCLUDE) $(LIBS) 
else
	$(CC) -o $@ $@.cpp $(CFLAGS) $(INCLUDE) $(LIBS) 
endif


objects: $(OBJECTS) $(CUDA_OBJECTS)
tests: $(TESTS)
execs: $(EXECS)
derived: $(DERIVED) 

all: objects execs derived tests 

clean: 
	rm -f $(TESTS) $(EXECS) $(OBJECTS) $(CUDA_OBJECTS) 

anew: clean all
