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
		if parts[0]=='DataFiles' or parts[0]=='RecoFiles' or parts[0]=='GenFiles' or parts[0]=='Map' or parts[0] == 'Attribute' or parts[0] == 'Functions':
			if parts[0] not in R:
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
			R[ parts[0] ] = parts[1].split()[0] ## remove e.g. eol
		#include -- TODO check
		if parts[0] == "include":
			filename2=parts[1].replace(" ","")
			k=filename.rfind("/")
			if k>=0:
				dir=filename[:k] + "/"
			else: 
				dir="./"
			try:
				f=open(dir+filename2);
				f.close();
				print "Reading configuration from "+dir+filename2
				Dat2=read_dat(dir+filename2)
				for name in Dat2:
					if (name=="Attribute" or name=="Functions" ) and name in R:
						R[name]+=Dat2[name]
					else:
						R[name]=Dat2[name]
			except IOError:
				try:
					f=open("./"+filename2);
					f.close();
					print "Reading configuration from ./"+filename2
					Dat2=read_dat("./"+filename2)
					for name in Dat2:
						if (name=="Attribute" or name=="Functions" ) and name in R:
							R[name]+=Dat2[name]
						else:
							R[name]=Dat2[name]
				except IOError:
					print "ERROR: "+dir+filename2+" - ./"+filename2+" - No such file or directory"
	return R

def PrintDat(dat):
        print "--------------------------------------------------------"
        for name in dat:
                print "Dat Key '" +str(name) + "'   value: " + str(dat[name])
        print "--------------------------------------------------------"
        print 
def ReadFromDat(dat,what,default,Error):
	try:
		return dat[what]
	except KeyError:
		print Error
		return default
def SetAttribute(Analyzer, attr,value ):
	print "Set Attribute %s to %s"%(attr,value)
	exec("Analyzer.%s=%s"%(attr,value))
	return
def SetFunction(Analyzer, attr ):
	print "Executing Functions '%s'"%attr
	print "Analyzer.%s"%(attr)
	exec("Analyzer.%s"%(attr))
	return
def SetAttributes(Analyzer, dat):
	for (attr,value) in dat["Attribute"]:
		SetAttribute(Analyzer,attr,value)
	for attr in dat["Functions"]:
		SetFunction(Analyzer,attr)
	return
