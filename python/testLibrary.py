import sys,os,array
import ROOT
import math
from optparse import OptionParser

DEBUG=1

if DEBUG>0: print "-> PARSING"
#####################
usage = "usage: %prog [options] arg1 arg2"
parser = OptionParser(usage=usage)
parser.add_option("-f","--fast" ,dest='fast',help="Run only on 50.000 entries",default=False,action='store_true')

(options,args)=parser.parse_args()

if DEBUG>0: print "-> Loading Library"
ROOT.gSystem.Load("libUnfolding.so");

#try to construct each object to check library Dictionaries
Contstructor = ROOT.Unfolding()
RecoInfo = ROOT.RecoInfo()
GC= ROOT.GlobalContainer()
Unfolder=ROOT.MergeAndUnfold()

##### CONFIG ########
nCat=5

MinReco=0
MaxReco=200
nReco=20

nGen=10
MinGen=0
MaxGen=200

TruthXSec=100

#####################

h_sig_truth=ROOT.TH1D("h_truth","h_truth",nGen,MinGen,MaxGen) #~Pt xSec @Gen
### SIG SHAPE DEFINITION
def f1(x,z):
	return x
def f2(x,z):
	return math.exp(-(x - z)**2/100)
def f(x,z):
	if(x<z): return f1(x,z)
	else : return f2(x,z)*f1(z,z)/f2(z,z)
##

for iBin in range(0,nGen+2):
	h_sig_truth.SetBinContent(iBin, f( h_sig_truth.GetBinCenter(iBin),30 ) );
	h_sig_truth.SetBinError(iBin, 0.1 );
h_sig_truth.Scale(TruthXSec/h_sig_truth.Integral())

# GEN IS DONE

def g(x,z):
	return math.exp( -x/30 );

Matrix=[]
Sig=[]
Bias=[]
Bkg=[]
for iCat in range(0,nCat):
	if DEBUG>0:print "Executing cat", iCat

	h_bkg=ROOT.TH1D("h_bkg_%d"%iCat,"h_bkg_%d"%iCat,nReco,MinReco,MaxReco) #~Pt xSec @RECO
	h_sig=ROOT.TH1D("h_sig_%d"%iCat,"h_sig_%d"%iCat,nReco,MinReco,MaxReco) #~Pt xSec @RECO
	h_matrix=ROOT.TH2D("h_matrix_%d"%iCat,"h_matrix_%d"%iCat,nReco,MinReco,MaxReco,nGen,MinGen,MaxGen) #~Pt xSec @RECO

	#Sig Bias
	h_sig_bias=ROOT.TH1D("h_bias_%d"%iCat,"h_bias_%d"%iCat,nGen,MinGen,MaxGen) #~Pt xSec @GEN -> bias of each category in the reconstruction. Includes efficiencies
	for iBin in range(0,nGen+2):
		h_sig_bias.SetBinContent(iBin, f( h_sig_bias.GetBinCenter(iBin),30+5*iCat ));
		h_sig_bias.SetBinError(iBin, 0.1 );
	h_sig_bias.Scale(TruthXSec*math.exp(-1-iCat)/h_sig_bias.Integral())
	
	#Bkg Reco
	for iBin in range(0,nReco+2):
		h_bkg.SetBinContent(iBin, g( h_bkg.GetBinCenter(iBin),30+5*iCat ));
	h_bkg.Scale( TruthXSec * ((nCat-iCat)**2) /h_bkg.Integral())

	#Matrix - should be almost diagonal -- fill probability values (normalization will be fixed in later steps)
	for gBin in range(0,nGen+2):
		for rBin in range(0,nReco+2):
			delta = h_sig.GetBinCenter(rBin) - h_sig_truth.GetBinCenter(gBin)
			h_matrix.SetBinContent(rBin,gBin,  math.exp(-delta*delta*2.0/h_sig_truth.GetBinWidth(gBin)) ); # gauss around delta=0
	#get efficiency - fix normalization
	for gBin in range(0,nGen+2):
		S=0
		for rBin in range(0,nReco+2):
			S+=h_matrix.GetBinContent(rBin,gBin);
		for rBin in range(0,nReco+2):
			#h_matrix.SetBinContent(rBin,gBin, h_matrix.GetBinContent( rBin,gBin ) * h_sig_truth.GetBin(gBin) * h_sig_bias.GetBinContent(gBin)/h_sig_truth.GetBinContent(gBin) / S ) ### ??
			h_matrix.SetBinContent(rBin,gBin, h_matrix.GetBinContent( rBin,gBin ) * h_sig_bias.GetBinContent(gBin) / S ) ### ??
			#delta = h_sig.GetBinCenter(rBin) - h_sig_truth.GetBinCenter(gBin)

	# RECONSTRUCT RECO SIG WITH THE MATRIX
	#Sig Reco
	for rBin in range(0,nReco+2):
		S=0
		for gBin in range(0,nGen+2):
			S+=h_matrix.GetBinContent(rBin,gBin);
		h_sig.SetBinContent(rBin, S);
		h_sig.SetBinError(rBin, 0.1 );

	#there is no bkg (ph.sp) sub so far. Otherwise one has to add it to h_sig - TODO,maybe later
	Sig.append(h_sig)
	Matrix.append(h_matrix)
	Bias.append(h_sig_bias)
	Bkg.append(h_bkg)
	

#make a canvas with the histograms
C0=ROOT.TCanvas("c0","c0",800,800)
C0.Divide(nCat,2)
for iCat in range(0,nCat):
	C0.cd(iCat +1 )
	h_sig_truth.SetLineColor(ROOT.kRed)
	h_sig_truth.Draw("HIST")
	Sig[iCat].SetLineColor(ROOT.kBlue+2)
	Sig[iCat].Draw("HIST SAME")
	Bias[iCat].SetLineColor(ROOT.kGreen+2)
	Bias[iCat].Draw("HIST SAME")
	C0.cd(iCat +nCat +1)
	Matrix[iCat].Draw("BOX")

# Generate Bkg+Sig in different categories - or actually set the error correctly + random variation
for iCat in range(0,nCat):
	#produce a hdata
	h=Sig[iCat].Clone("histo_%d"%iCat)
	for iBin in range(0,nReco):
		s=Sig[iCat].GetBinContent(iBin)  
		b=Bkg[iCat].GetBinContent(iBin)  
		h.SetBinContent(iBin, s) ## add random fluctuations - TODO
		h.SetBinError  (iBin, s/math.sqrt(b)) 
	Unfolder.AddCat(h,h_sig_truth,Sig[iCat],Matrix[iCat]);
# Fit Sig 
uMin=Unfolder.UnfoldMinimum();
uBayes=Unfolder.UnfoldBayes(3);

Unfolder.AddReg(0.1)
#uMinReg=Unfolder.UnfoldMinimum(); 

# Unfold Distributions
# Show Results
C1=ROOT.TCanvas("c1","c1",800,800)

uMin.SetLineColor(ROOT.kBlue+2)
uMin.SetMarkerColor(ROOT.kBlue+2)
uMin.SetMarkerStyle(ROOT.kCircle)
uMin.SetMarkerSize(0.8)

uBayes.SetLineColor(ROOT.kGreen+1) 
uBayes.SetMarkerColor(ROOT.kGreen+1) 
uBayes.SetMarkerStyle(ROOT.kOpenCircle)
uBayes.SetMarkerSize(0.8)

#uMinReg.SetLineColor(ROOT.kCyan+1) 
#uMinReg.SetMarkerStyle(ROOT.kDiamond)
#uMinReg.SetMarkerSize(0.8)

h_sig_truth.Draw("HIST")

for iCat in reversed(range(0,nCat)):
	Sig[iCat].Draw("HIST SAME")
	Unfolder.u[iCat].SetLineColor(ROOT.kGray+iCat)
	Unfolder.u[iCat].SetMarkerColor(ROOT.kGray+iCat)
	Unfolder.u[iCat].Draw("P SAME")

uBayes.Draw("P SAME")
uMin.Draw("P SAME")
#uMinReg.Draw("P SAME")

L=ROOT.TLegend(0.75,.75,.89,.89)
L.AddEntry(h_sig_truth,"Truth","L")
L.AddEntry(uMin,"Min","PF");
#L.AddEntry(uMinReg,"Reg 0.1","P");
L.AddEntry(uBayes,"Bayes","PF");
L.Draw("SAME")

print "press Enter to finish"
a=raw_input()
