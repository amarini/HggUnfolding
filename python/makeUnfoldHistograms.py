#!/usr/bin/python

import os, sys, array
import ROOT 
from optparse import OptionParser
from glob import glob

DEBUG=2

if(DEBUG>0):print "----- BEGIN -----"

if(DEBUG>0):print "-PARSING OPTIONS-"
usage = "usage: %prog [options] arg1 arg2"
parser=OptionParser(usage=usage)
parser.add_option("","--gendir" ,dest='gendir',type='string',help="Directory of gen files",default="/Users/andreamarini/Documents/HggDifferential/samples/gen")
parser.add_option("","--recodir" ,dest='recodir',type='string',help="Directory with reco txt files",default="/Users/andreamarini/Documents/HggDifferential/samples/reco")

(options,args)=parser.parse_args()

# change to options
GenDirectory="/Users/andreamarini/Documents/HggDifferential/samples/gen"
RecoDirectory="/Users/andreamarini/Documents/HggDifferential/samples/reco"

ROOT.gSystem.Load("libUnfolding.so")


Unfolder = ROOT.Unfolding();

Unfolder.SetDebug(DEBUG);
Unfolder.SetNcat(14);


# merge relevant categories -- use modulus and loops
Unfolder.SetCat(0 ,0);
Unfolder.SetCat(4 ,0);
Unfolder.SetCat(8 ,0);
Unfolder.SetCat(12,0);

Unfolder.SetCat(1 ,1);
Unfolder.SetCat(5 ,1);
Unfolder.SetCat(9 ,1);
Unfolder.SetCat(13,1);

Unfolder.SetCat(2 ,2);
Unfolder.SetCat(6 ,2);
Unfolder.SetCat(10 ,2);

Unfolder.SetCat(3 ,3);
Unfolder.SetCat(7 ,3);
Unfolder.SetCat(11 ,3);

# TODO implement eos - can directly pass * because is read trough a TChain Add
filesGen=glob(GenDirectory+"/*.root")
filesReco=glob(RecoDirectory+"/*.txt")

print "Add gen -> ggH_8TeV"
Unfolder.xSecMapDirToNam["gen"]="ggH_8TeV"
print " >---<"

for gF in filesGen:
	Unfolder.genFiles.push_back(gF);
for rF in filesReco:
	Unfolder.recoFiles.push_back(rF);

print "Going to Loop"
Unfolder.LoopOverReco();
Unfolder.LoopOverGen();

print "Going to Write"
Unfolder.Write("UnfoldMatrixes.root");
		
