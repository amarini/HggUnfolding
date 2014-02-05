
//------------------H FILE
#include <string>
#include <map>
#include <set>
#include <vector>
#include <iostream>
#include <cstdio>

#include "TH1D.h"
#include "TH1F.h"
#include "TH2D.h"
#include "TH2F.h"
#include "TLorentzVector.h"
#include "TChain.h"
#include "TTree.h"
#include "TROOT.h"

#ifndef BuildResponse_h
#define BuildResponse_h

#define MAXN 2048

#include "interface/GlobalContainer.h"
//#include "src/GlobalContainer.cc" // for the moment
#include "TObject.h"

using namespace std;

class Bins{
public:
	Bins(){isConstBin=1;};
	~Bins(){};
	int nBins;
	float xMin;
	float xMax;
	Float_t *bins;
	int isConstBin;
	void SetNonConst(){ isConstBin=0; bins=new Float_t[100];}
};

class RecoInfo: public TObject{ 
public:
	//costructor
	RecoInfo(){cat=-1;hgg.SetPxPyPzE(0,0,0,0);weight=0;nJets=0;}
	RecoInfo(TLorentzVector hgg_,TLorentzVector pho1_,TLorentzVector pho2_,int cat_,float w_){ hgg=hgg_;pho1=pho1_; pho2=pho2_;cat=cat_;weight=w_;}
	//info I need to keep
	TLorentzVector hgg,pho1,pho2,jet1,jet2;
	int nJets;
	int cat;
	float weight;
	ClassDef(RecoInfo,1);
};


//-------- 
class BuildResponse:public TObject{
public:
	BuildResponse();
	~BuildResponse();
	std::vector<std::string> genFiles; // Production
	std::vector<std::string> recoFiles; // dumpAscii files
	int LoopOverGen(); // 0 = success otherwise error
	int LoopOverReco(); // 0 = success otherwise error
	int Loop() {return LoopOverReco() | LoopOverGen();}
	int InitGen();
	int InitReco();
	int InitRecoOptTree(); // not completely implemented
	int LoopOverRecoOptTree(); // not completely implemented
	int Write(string output);
	int Log(string filename){freopen(filename.c_str(), "a", stdout);};
	void SetDebug(int debug_){debug=debug_;};
	void SetNcat(int ncat){catMap.resize(ncat);for(int i=0;i<ncat;i++)catMap[i]=i;}; //give to each cat a cat
	void SetCat(int iCat,int jCat){if( (iCat >= catMap.size() )||(jCat >=catMap.size()))return ; 
			if(catMap[jCat]==jCat){catMap[iCat]=jCat;return;} else return SetCat(iCat,catMap[jCat]);}; //Set Cat iCat to be the "same" as jCat
	void SetCatsModulo(int M);
	
	map<string,string> xSecMapDirToNam;
	map<string,Bins> HistoBins; //fill this to make it configurable from a dat file
	
	//configurable cuts for gen level selection:

	Float_t JetPtCut=25; //TO MAKE CONFIGURABLE
	Float_t JetPhoDR=0.3;
	Float_t PhoIsoDR=0.3;
	Float_t PhoIsoCut=5;
	Float_t PhoEtaCut=2.5;
	Float_t Pho1PtCut=41.66;
	Float_t Pho2PtCut=31.25;
	
private:
	TChain *tGen;
	TChain *tReco;

	//functions
	float CosThetaStar(TLorentzVector &a,TLorentzVector&b);
	Bins GetBins(string name);
	void Fill(string name, float value,float weight);
	void Fill2D(string name, float value1,float value2,float weight);
	
	//objects
	int debug;
	GlobalContainer Container;
	map<pair<string,unsigned long long>, RecoInfo> recoEvents; // map between EventNum ->  reco info
	vector<int> catMap;
	map<string,float> xSecWeight; 
	set<string> histoToSave;
	//check orphans
	map< pair<string,string> ,pair<double,double> > perFileEff; //numbers are pass/tot
public:
	ClassDef(BuildResponse,1);
};
#endif

// -----------------END OF H FILE
