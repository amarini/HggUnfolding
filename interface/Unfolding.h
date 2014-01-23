
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

#ifndef Unfolding_h
#define Unfolding_h

#define MAXN 2048

#include "interface/GlobalContainer.h"
//#include "src/GlobalContainer.cc" // for the moment
#include "TObject.h"

using namespace std;

class RecoInfo: public TObject{ 
public:
	//costructor
	RecoInfo(){cat=-1;hgg.SetPxPyPzE(0,0,0,0);weight=0;}
	RecoInfo(TLorentzVector hgg_,TLorentzVector pho1_,TLorentzVector pho2_,int cat_,float w_){ hgg=hgg_;pho1=pho1_; pho2=pho2_;cat=cat_;weight=w_;}
	//info I need to keep
	TLorentzVector hgg,pho1,pho2;
	int cat;
	float weight;
	ClassDef(RecoInfo,1);
};


//-------- 
class Unfolding:public TObject{
public:
	Unfolding();
	~Unfolding();
	std::vector<std::string> genFiles; // Production
	std::vector<std::string> recoFiles; // dumpAscii files
	int LoopOverGen(); // 0 = success otherwise error
	int LoopOverReco(); // 0 = success otherwise error
	int Loop() {return LoopOverReco() | LoopOverGen();}
	int InitGen();
	int InitReco();
	int InitRecoOptTree(); // not completely implemented
	int Write(string output);
	void SetDebug(int debug_){debug=debug_;};
	void SetNcat(int ncat){catMap.resize(ncat);for(int i=0;i<ncat;i++)catMap[i]=i;}; //give to each cat a cat
	void SetCat(int iCat,int jCat){if( (iCat >= catMap.size() )||(jCat >=catMap.size()))return ; 
			if(catMap[jCat]==jCat){catMap[iCat]=jCat;return;} else return SetCat(iCat,catMap[jCat]);}; //Set Cat iCat to be the "same" as jCat
	
	map<string,string> xSecMapDirToNam;
private:
	TChain *tGen;
	TChain *tReco;
	TH1D *gen;
	TH1D *reco;
	TH2D *resp;
	int debug;
	GlobalContainer Container;
	map<pair<string,unsigned long long>, RecoInfo> recoEvents; // map between EventNum ->  reco info
	vector<int> catMap;
	map<string,float> xSecWeight; 
	set<string> histoToSave;
public:
	ClassDef(Unfolding,1);
};
#endif

// -----------------END OF H FILE
