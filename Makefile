SrcSuf        = cc
HeadSuf       = h
ObjSuf        = o
DepSuf        = d
DllSuf        = so

.SUFFIXES: .$(SrcSuf) .$(ObjSuf) .$(DllSuf)

##
## Flags and external dependecies
## 
LDFLAGS       = -O
SOFLAGS       = -fPIC -shared
LD            = g++
CXX           = g++
ROOFIT_BASE=$(ROOFITSYS)
LDFLAGS+=-L$(ROOFIT_BASE)/lib $(ROOTLIBS) -lRooFitCore -lRooFit -lz
LDFLAGS+= -lTMVA
CXXFLAGS+=-I$(ROOFIT_BASE)/include
CXXFLAGS+= `root-config --cflags`
CXXFLAGS+=-I$(shell pwd) -g
CXXFLAGS+= -O -fPIC
ROOTLIBS=`root-config --libs`
LDFLAGS+=$(ROOTLIBS)

SHELL=bash

BASEDIR = $(shell pwd)

$(shell mkdir -p bin)

BINDIR = $(BASEDIR)/bin
SRCDIR = $(BASEDIR)/src
HEADDIR = $(BASEDIR)/interface

#Packages=GlobalContainer Unfolding
## Sort needed for building libraries: LAST <- FIRST
Packages=Unfolding GlobalContainer
#ObjFiles=$(foreach Pack, $(Packgaes),   $(BINDIR)/$(Pack).$(ObjSuf) )
ObjFiles=$(patsubst %, $(BINDIR)/%.$(ObjSuf),$(Packages) )

.PHONY: all
all: info libUnfolding.so

.PHONY: info
info:
	@echo "--INFO--"
	@echo Packages: $(Packages)
	@echo Obj: $(ObjFiles)
	@echo "--------"


##explicit rules
libUnfolding.$(DllSuf): $(ObjFiles)
	@$(LD) $(SOFLAGS) $(LDFLAGS) $(ObjFiles) -o $@

#$(BINDIR)/Unfolding.o: src/Unfolding.cc interface/Unfoding.h
#	g++ `root-config --libs --cflags` -c -o Unfolding.o src/Unfolding.cc
#$(BINDIR)/GlobalContainer.o: src/GlobalContainer.cc interface/GlobalContainer.h
#	g++ `root-config --libs --cflags` -c -o GlobalContainer.o src/GlobalContainer.cc


%.$(ObjSuf): %.cc
%.$(ObjSuf): %.C
%.$(ObjSuf): %.o
%.$(ObjSuf): %.d

#implicit rules
$(BINDIR)/%.$(ObjSuf): 
	@echo compiling $@
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -c -o $@ $(SRCDIR)/$*.$(SrcSuf)

.PHONY: clean
clean:
	rm -v $(ObjFiles) || true
	rm -v libUnfolding.so || true
