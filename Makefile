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
LDFLAGS+=-L$(ROOFIT_BASE)/lib $(ROOTLIBS) -lRooFitCore -lRooFit -lz -lRooUnfold
LDFLAGS+= -lTMVA
CXXFLAGS+=-I$(ROOFIT_BASE)/include
CXXFLAGS+= `root-config --cflags`
CXXFLAGS+=-I$(shell pwd) -g
CXXFLAGS+= -O -fPIC
ROOTLIBS=`root-config --libs`
LDFLAGS+=$(ROOTLIBS)

SHELL=bash

BASEDIR = $(shell pwd)


BINDIR = $(BASEDIR)/bin
SRCDIR = $(BASEDIR)/src
HEADDIR = $(BASEDIR)/interface

$(shell mkdir -p $(BINDIR) )

#Packages=GlobalContainer Unfolding
## Sort needed for building libraries: LAST <- FIRST
Packages=Unfolding GlobalContainer MergeAndUnfold
ObjFiles=$(patsubst %, $(BINDIR)/%.$(ObjSuf),$(Packages) )
DictObjFiles=$(patsubst %, $(BINDIR)/%Dict.$(ObjSuf),$(Packages) )
DictSrcFiles=$(patsubst %, $(BINDIR)/%Dict.$(SrcSuf),$(Packages) )
DictHeadFiles=$(patsubst %, $(BINDIR)/%Dict.$(HeadSuf),$(Packages) )
DictLinkDefFiles=$(patsubst %, $(HEADDIR)/%LinkDef.$(HeadSuf),$(Packages) )

#make intermediate files persistent
.PRECIOUS: %.$(SrcSuf) %.$(HeadSuf) %.$(ObjSuf) $(BINDIR)/%Dict.$(SrcSuf)

.PHONY: all
all: info libUnfolding.so

.PHONY: info
info:
	@echo "--- INFO ---"
	@echo Packages: $(Packages)
	@echo Obj: $(ObjFiles)
	@echo Dict: $(DictObjFiles)
	@echo "------------"


#InfoLine = \033[01\;31m compiling $(1) \033[00m
InfoLine = compiling $(1) 


##explicit rules
libUnfolding.$(DllSuf): $(ObjFiles) $(DictObjFiles)
	@echo $(call InfoLine , $@ )
	$(LD) $(SOFLAGS) $(LDFLAGS) $(ObjFiles) $(DictObjFiles) -o $@

#implicit rules

#.o
$(BINDIR)/%.$(ObjSuf): $(SRCDIR)/%.$(SrcSuf) $(HEADDIR)/%.$(HeadSuf)
	@echo $(call InfoLine , $@ )
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -c -o $@ $(SRCDIR)/$*.$(SrcSuf)

$(BINDIR)/%.$(ObjSuf): $(BINDIR)/%.$(SrcSuf) $(BINDIR)/%.$(HeadSuf)
	@echo $(call InfoLine , $@ )
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -c -o $@ -I$(BINDIR)/ $(BINDIR)/$*.$(SrcSuf)
#.d
$(BINDIR)/%.$(DepSuf): $(SRCDIR)/%.$(SrcSuf) $(HEADDIR)/%.$(HeadSuf)
	@echo $(call InfoLine , $@ )
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -M -o $@ $(SRCDIR)/$*.$(SrcSuf)

#LinkDef.h -- unused
$(BINDIR)/%LinkDef.$(HeadSuf): $(HEADDIR)/%.$(HeadSuf)
	@echo $(call InfoLine , $@ )
	$(shell echo "#ifdef __CINT__" > $(BINDIR)/$*LinkDef.$(HeadSuf)  )
	$(shell echo "#pragma link C++ class $*+ ;" >> $(BINDIR)/$*LinkDef.$(HeadSuf)  )
	$(shell echo "#endif" >> $(BINDIR)/$*LinkDef.$(HeadSuf)   )
	touch  $(BINDIR)/$*LinkDef.$(HeadSuf)

#Dict
$(BINDIR)/%Dict.$(SrcSuf): $(HEADDIR)/%.$(HeadSuf) $(HEADDIR)/%LinkDef.$(HeadSuf)
	@echo $(call InfoLine , $@ )
	rootcint -v4 -f $(BINDIR)/$*Dict.cc -c -I$(ROOFIT_BASE)/include -I$(CMSSW_BASE)/src  -I$(CMSSW_RELEASE_BASE)/src $(HEADDIR)/$*.$(HeadSuf) $(HEADDIR)/$*LinkDef.$(HeadSuf)

.PHONY: clean
clean:
	-rm $(ObjFiles) 
	-rm $(DictObjFiles)
	-rm $(DictSrcFiles)
	-rm $(DictHeadFiles)
	-rm  libUnfolding.so 

### END ####


#$(BINDIR)/Unfolding.o: src/Unfolding.cc interface/Unfoding.h
#	g++ `root-config --libs --cflags` -c -o Unfolding.o src/Unfolding.cc
#$(BINDIR)/GlobalContainer.o: src/GlobalContainer.cc interface/GlobalContainer.h
#	g++ `root-config --libs --cflags` -c -o GlobalContainer.o src/GlobalContainer.cc
