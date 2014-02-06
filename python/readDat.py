import sys, os, array


def ReadDat(inputFileName):
	R={}
	inputFile=open(inputFileName,"r")
	for line in inputFile:
		line = line.split('#')[0];
		line = line.replace(' ','') #get rid of spaces
		parts=line.split('=');
		if len(parts)<2 : continue;
		#list of string
		if parts[0]=='DataFiles' or parts[0]=='RecoFiles' or parts[0]=='GenFiles' or parts[0]=='Map':
			R[parts[0]]=[]
			for f in parts[1].split(','):
				R[parts[0]].append( f.split()[0] )  # make read of \n
		#list of float
		if parts[0]=='Pt' or parts[0]=='Eta' or parts[0]=='CosThetaStar' or parts[0]=='Y':
			R[ parts[0] ] = []
			for pt in parts[1].split(','):
				R[ parts[0] ].append( float(pt) )
		#int
		if parts[0]=='nCat' or parts[0] == 'CatMod':
			R[ parts[0] ] = int(parts[1])
		#float
		if parts[0]=='Pho1Pt' or parts[0] == 'Pho2Pt' or parts[0]=='PhoEta' or parts[0]=='PhoIso' or parts[0]=='PhoIsoDR' or parts[0]=='JetPtCut' or parts[0] == 'JetPhoDR':
			R[ parts[0] ] = float(parts[1])
		#string
		if parts[0]=='OutFile' :
			R[ parts[0] ] = parts[1]
	return R

def PrintDat(dat):
        print "--------------------------------------------------------"
        for name in dat:
                print "Dat Key '" +str(name) + "'   value: " + str(dat[name])
        print "--------------------------------------------------------"
        print 
