
import os,sys,array

from ROOT import *
from optparse import OptionParser
from subprocess import call

usage = "usage: %prog [options] arg1 arg2"
parser=OptionParser(usage=usage)
parser.add_option("-o","--outputdir" ,dest='outdir',type='string',help="input = %default",default="plots")
parser.add_option("-i","--inputfile" ,dest='inputfile',type='string',help="input = %default",default="UnfoldMatrixes.root")
parser.add_option("-d","--datfile" ,dest='datfile',type='string',help="datfile = %default",default="data/variables.dat")

(options,args)=parser.parse_args()

gStyle.SetOptStat(0)
gStyle.SetOptTitle(0)

f= TFile.Open( options.inputfile )

call( ["mkdir", "-p" ,options.outdir ])


colors=[kGreen-4,kBlue-4,kRed-4,kOrange,kGray+1]
styles=[3001,3001,3001,3001,3001,3001]

colors.reverse()
styles.reverse()

def Plot(name="hgg_pt",cat=[0,5,10,15]):
	C=TCanvas(name,name,800,600)	
	p0 = TPad("pad0","pad0",0,0,1,0.3)
	p1 = TPad("pad1","pad1",0,0.3,0.5,1)
	p2 = TPad("pad1","pad1",0.5,0.3,1,1)

	p0.SetLeftMargin(0.05)
	p0.SetRightMargin(0.05)

	p0.Draw()
	p1.Draw()
	p2.Draw()

	p1.cd()

	S=THStack("s_"+name,"s_"+name)
	g=f.Get( 'gen_'+name )	

	r=g.Clone("tot");
	r.Reset()

	#clone 
	col=colors[:]
	st=styles[:]

	for iCat in cat:
		h=f.Get( 'reco_'+name+'_cat%d'%(iCat) )	
		h.SetLineColor(kBlack)
		h.SetLineWidth(2)
		h.SetFillColor( col.pop() )
		h.SetFillStyle( st.pop() )
		S.Add(h)
		r.Add(h)
	
	g.SetLineColor(kRed+2)
	g.SetLineWidth(2);

	g.Draw("HIST")
	S.Draw("HIST SAME")

	g.Draw("AXIS SAME")
	g.Draw("AXIS X+ Y+ SAME")

	p0.cd()
	r.Divide(g)
	r.Draw("P")
	r.GetYaxis().SetRangeUser(0,1)

	p2.cd()
	dummy=TH2F("dummy","dummy",1,g.GetBinLowEdge(1),g.GetBinLowEdge(g.GetNbinsX()+1),1,g.GetBinLowEdge(1),g.GetBinLowEdge(g.GetNbinsX()+1))
	dummy.SetMaximum(g.GetMaximum())
	dummy.SetMinimum(0)
	dummy.Draw("AXIS ")
	dummy.Draw("AXIS X+ Y+ SAME")
	col=colors[:]
	st=styles[:]
	for iCat in cat:
		h2=f.Get( 'response_'+name+'_cat%d'%(iCat) )	
		h2.SetLineColor( col.pop() )
		h2.Draw("BOX SAME")
	
	#a=raw_input()
	C.SaveAs(options.outdir + "/" + name+".pdf")

Plot("hgg_pt")
Plot("hgg_y")
Plot("hgg_coststar")
Plot("hgg_m")
Plot("hgg_deltaphi")
