CC=g++
CXX=g++
CXXLD=g++
CCLD=gcc

CXXFLAGS= -g -Wall -I -I$  -pthread -fPIC -DPIC
CCFLAGS=$(CXXFLAGS)

ROOTSYS = /opt64/root/root_v5.34.05/
#ROOTSYS = /opt64/root/root_v5.26.00/

ROOTCFLAGS   := $(shell ${ROOTSYS}/bin/root-config --cflags)
ROOTGLIBS    := $(shell ${ROOTSYS}/bin/root-config --glibs)
ROOTLDFLAGS  := $(shell ${ROOTSYS}/bin/root-config --ldflags)
CXXFLAGS += $(ROOTCFLAGS)

INCLUDES:= DANCE_Alpha_Calibrator.h 

OBJECTS:= DANCE_Alpha_Calibrator.o

LIBS  = -lm $(ROOTGLIBS) 

SRCS:= DANCE_Alpha_Calibrator.cpp

all: DANCE_Alpha_Calibrator

DANCE_Alpha_Calibrator:  $(SRCS) $(OBJECTS) $(INCLUDES) 
	$(CXX) -o DANCE_Alpha_Calibrator $(OBJECTS) $(CXXFLAGS) $(ROOTGLIBS) $(LIBS)

%.o: %.cpp 
	$(CXX) $(CXXFLAGS) -c $< 
%.o: %.cxx 
	$(CXX) $(CXXFLAGS) -c $< 
%.o: %.c
	$(CXX) $(CXXFLAGS) -c $< 

clean:
	rm -f *.o DANCE_Alpha_Calibrator
# DO NOT DELETE

print_env:
	@echo $(ROOTSYS)
	@echo $(ROOTCFLAGS)
	@echo $(ROOTGLIBS)
	@echo $(ROOTLDFLAGS)
