#!/usr/bin/python

import os, sys, array
import ROOT 
from optparse import OptionParser
from glob import glob

DEBUG=1

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
Unfolder.SetNcat(18);


# merge relevant categories -- use modulus and loops
Unfolder.SetCatsModulo(6); #~n. of cat per bin
#Unfolder.SetCat(0 ,0);
#Unfolder.SetCat(4 ,0);
#Unfolder.SetCat(8 ,0);
#Unfolder.SetCat(12,0);
#
#Unfolder.SetCat(1 ,1);
#Unfolder.SetCat(5 ,1);
#Unfolder.SetCat(9 ,1);
#Unfolder.SetCat(13,1);
#
#Unfolder.SetCat(2 ,2);
#Unfolder.SetCat(6 ,2);
#Unfolder.SetCat(10 ,2);
#
#Unfolder.SetCat(3 ,3);
#Unfolder.SetCat(7 ,3);
#Unfolder.SetCat(11 ,3);


#BOOK FILES
#GenDirectory=[]
#GenDirectory+="/eos/cms/store/group/phys_higgs/cmshgg//processed/V15_00_08/mc/Summer12_RD1/GluGluToHToGG_M-125_8TeV-powheg-pythia6_Summer12_DR53X-PU_RD1_START53_V7N-v1"
#GenDirectory+="/eos/cms/store/group/phys_higgs/cmshgg//processed/V15_00_08/mc/Summer12_RD1/VBF_HToGG_M-125_8TeV-powheg-pythia6_Summer12_DR53X-PU_RD1_START53_V7N-v1"
#GenDirectory+="/eos/cms/store/group/phys_higgs/cmshgg//processed/V15_00_08/mc/Summer12_RD1/TTH_HToGG_M-125_8TeV-pythia6_Summer12_DR53X-PU_RD1_START53_V7N-v1"
#GenDirectory+="/eos/cms/store/group/phys_higgs/cmshgg//processed/V15_00_08/mc/Summer12_RD1/WH_ZH_HToGG_M-125_8TeV-pythia6_Summer12_DR53X-PU_RD1_START53_V7N-v2"

#filesGen=glob(GenDirectory+"/*.root")
#filesReco=glob(RecoDirectory+"/*.txt")
#for gF in filesGen:
#	Unfolder.genFiles.push_back(gF);
#for rF in filesReco:
#	Unfolder.recoFiles.push_back(rF);
#

print "Adding map information -> ggh_8TeV"
#Unfolder.xSecMapDirToNam["gen"]="ggH_8TeV"

# should match with treeNames for opttree
Unfolder.xSecMapDirToNam["GluGluToHToGG_M-125_8TeV-powheg-pythia6_Summer12_DR53X-PU_RD1_START53_V7N-v1"]="ggh_m125_8TeV"
Unfolder.xSecMapDirToNam["VBF_HToGG_M-125_8TeV-powheg-pythia6_Summer12_DR53X-PU_RD1_START53_V7N-v1"]="vbf_m125_8TeV"
Unfolder.xSecMapDirToNam["TTH_HToGG_M-125_8TeV-pythia6_Summer12_DR53X-PU_RD1_START53_V7N-v1"]="tth_m125_8TeV"
Unfolder.xSecMapDirToNam["WH_ZH_HToGG_M-125_8TeV-pythia6_Summer12_DR53X-PU_RD1_START53_V7N-v2"]="wzh_m125_8TeV"
print " >---<"

#EOS
Unfolder.recoFiles.push_back('root://eoscms///store/user/chanon/HGG/differentialnalysis/histograms_CMS-HGG_DiffAna_v1.root')
Unfolder.genFiles.push_back('root://eoscms///store/group/phys_higgs/cmshgg//processed/V15_00_08/mc/Summer12_RD1/GluGluToHToGG_M-125_8TeV-powheg-pythia6_Summer12_DR53X-PU_RD1_START53_V7N-v1/*.root')
Unfolder.genFiles.push_back('root://eoscms///store/group/phys_higgs/cmshgg//processed/V15_00_08/mc/Summer12_RD1/VBF_HToGG_M-125_8TeV-powheg-pythia6_Summer12_DR53X-PU_RD1_START53_V7N-v1/*.root')
Unfolder.genFiles.push_back('root://eoscms///store/group/phys_higgs/cmshgg//processed/V15_00_08/mc/Summer12_RD1/TTH_HToGG_M-125_8TeV-pythia6_Summer12_DR53X-PU_RD1_START53_V7N-v1/*.root')
Unfolder.genFiles.push_back('root://eoscms///store/group/phys_higgs/cmshgg//processed/V15_00_08/mc/Summer12_RD1/WH_ZH_HToGG_M-125_8TeV-pythia6_Summer12_DR53X-PU_RD1_START53_V7N-v2/*.root')

print "Going to Loop"
#Unfolder.LoopOverReco();
Unfolder.LoopOverRecoOptTree(); 
Unfolder.LoopOverGen();

print "Going to Write"
Unfolder.Write("UnfoldMatrixes.root");


		
