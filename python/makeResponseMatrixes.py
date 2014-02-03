#!/usr/bin/python

import os, sys, array
import ROOT 
ROOT.gROOT.SetBatch(1)

from optparse import OptionParser
from glob import glob

DEBUG=1

if(DEBUG>0):print "----- BEGIN -----"

if(DEBUG>0):print "-PARSING OPTIONS-"
usage = "usage: %prog [options] arg1 arg2"
parser=OptionParser(usage=usage)
parser.add_option("-i","--datfile" ,dest='datfile',type='string',help="datfiles",default="data/variables.dat")

(options,args)=parser.parse_args()

# change to options


ROOT.gSystem.Load("libUnfolding.so")


Unfolder = ROOT.BuildResponse();

Unfolder.SetDebug(DEBUG);


Unfolder.SetNcat(config['nCat']);


# merge relevant categories -- use modulus and loops
Unfolder.SetCatsModulo(config['CatMod']); #~n. of cat per bin
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

print "inserting in path cwd"
sys.path.insert(0,os.getcwd())
print "inserting in path cwd/python"
sys.path.insert(0,os.getcwd()+'/python')
from readDat import *

config = ReadDat( options.datfile );

if DEBUG>0:
	PrintDat(config)

print "Adding map information -> ggh_8TeV"

iFile=0;
for f in config['GenFiles']:
	if DEBUG>0:print "adding file '"+f+"' to GEN"
	Unfolder.genFiles.push_back('root://eoscms//'+f);
	#get Directory matching
	n=f.rfind('/')
	d=f[0:n]
	n=d.rfind('/')
	d=d[n+1:]
	if DEBUG>0:print 'Going to add to map dir="' + d + '" Nam="'+config['Map'][iFile] + '"'
	Unfolder.xSecMapDirToNam[d]=config['Map'][iFile];
	iFile+=1
for f in config['RecoFiles']:
	if DEBUG>0:print "adding file '"+f+"' to RECO"
	Unfolder.recoFiles.push_back('root://eoscms//'+f);

# should match with treeNames for opttree
#Unfolder.xSecMapDirToNam["GluGluToHToGG_M-125_StdMaterial"]="ggh_m125_8TeV"
#Unfolder.xSecMapDirToNam["VBF_HToGG_M-125_8TeV-powheg-pythia6_Summer12_DR53X-PU_RD1_START53_V7N-v1"]="vbf_m125_8TeV"
#Unfolder.xSecMapDirToNam["TTH_HToGG_M-125_8TeV-pythia6_Summer12_DR53X-PU_RD1_START53_V7N-v1"]="tth_m125_8TeV"
#Unfolder.xSecMapDirToNam["WH_ZH_HToGG_M-125_8TeV-pythia6_Summer12_DR53X-PU_RD1_START53_V7N-v2"]="wzh_m125_8TeV"
print " >---<"

#EOS
#Unfolder.recoFiles.push_back('root://eoscms///store/user/chanon/HGG/differentialnalysis/histograms_CMS-HGG_DiffAna_v1.root')
#High Stat
#Unfolder.genFiles.push_back('root://eoscms///store/group/phys_higgs/cmshgg/V15_00_11/ExtraMaterial/GluGluToHToGG_M-125_StdMaterial/*.root')
#Unfolder.genFiles.push_back('root://eoscms///store/group/phys_higgs/cmshgg//processed/V15_00_08/mc/Summer12_RD1/VBF_HToGG_M-125_8TeV-powheg-pythia6_Summer12_DR53X-PU_RD1_START53_V7N-v1/*.root')
#Unfolder.genFiles.push_back('root://eoscms///store/group/phys_higgs/cmshgg//processed/V15_00_08/mc/Summer12_RD1/TTH_HToGG_M-125_8TeV-pythia6_Summer12_DR53X-PU_RD1_START53_V7N-v1/*.root')
#Unfolder.genFiles.push_back('root://eoscms///store/group/phys_higgs/cmshgg//processed/V15_00_08/mc/Summer12_RD1/WH_ZH_HToGG_M-125_8TeV-pythia6_Summer12_DR53X-PU_RD1_START53_V7N-v2/*.root')


#Read Bins from dat
if DEBUG > 0: print "Going to set Bins"
Unfolder.HistoBins['hgg_pt'].SetNonConst(); #also call malloc for *bins with 100 spaces
iBin=0
for pt in config['Pt']:
	Unfolder.HistoBins['hgg_pt'].bins[iBin]=pt
	iBin+=1;
	Unfolder.HistoBins['hgg_pt'].nBins=iBin;

Unfolder.HistoBins['hgg_coststar'].SetNonConst(); 
iBin=0
for cost in config['CosThetaStar']:
	Unfolder.HistoBins['hgg_coststar'].bins[iBin]=cost
	iBin+=1;
	Unfolder.HistoBins['hgg_coststar'].nBins=iBin

Unfolder.HistoBins['hgg_y'].SetNonConst(); 
iBin=0
for y in config['Y']:
	Unfolder.HistoBins['hgg_y'].bins[iBin]=y
	iBin+=1;
	Unfolder.HistoBins['hgg_y'].nBins=iBin;

if DEBUG > 0: print " -- "
######################

print "Going to Loop"
#Unfolder.LoopOverReco();
Unfolder.LoopOverRecoOptTree(); 
Unfolder.LoopOverGen();

print "Going to Write"
Unfolder.Write("UnfoldMatrixes.root");


		
