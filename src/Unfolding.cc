
#include "TFile.h"

#define Unfolding_cxx
#include "interface/Unfolding.h"
#include <assert.h>
#include "TClonesArray.h"



// --- use std as namespace
using namespace std;

// --- Constructor 
Unfolding::Unfolding(){
	tGen=NULL;
	tReco=NULL;
	debug=0;
	catMap.clear();
};
// --- Destructor 
Unfolding::~Unfolding(){
	if ( tGen != NULL ) { tGen->Delete(); tGen=NULL;}
	if ( tReco != NULL ) { tReco->Delete(); tReco=NULL;}
};


int Unfolding::InitGen(){
	if (tGen !=NULL ) delete tGen;
	tGen=new TChain("event");
	if(debug>0) cout<<"[1]"<<" Adding"<<genFiles.size()<<" files to Gen Chain"<<endl;
	for(vector<string>::iterator iFile=genFiles.begin();iFile!=genFiles.end();iFile++)
		{
		if(debug>0)cout<<"[1]"<< " Adding file: "<<*iFile<<" to the list of gen Chains: ";
		int R=tGen->Add( iFile->c_str() );
		if(debug>0) cout << R<<endl;
		//assert ( R==1);
		}

	if(debug>0)
	for(map<string,float>::iterator it=xSecWeight.begin();it!=xSecWeight.end();it++)
	 	cout<<"[1] xSecWeight "<<it->first<<" = "<<it->second<<endl;

	if(debug>1) cout<<"[2]"<<" GenEntries " << tGen->GetEntries()<<endl;


	tGen->SetBranchStatus("*",0);
	tGen->SetBranchStatus("*gp*",1);
	tGen->SetBranchStatus("event",1);
	tGen->SetBranchStatus("lumi",1);
	tGen->SetBranchStatus("run",1);
	tGen->SetBranchStatus("*genjet_algo1*",1);
	

	if(debug>1) cout<<"[2]"<<" End Init GEN"<<endl;
	return 0;
	};
int Unfolding::LoopOverGen(){
	InitGen();
	//moved to GlobalContainer and after to Init
	TClonesArray *gp_p4=0;tGen->SetBranchAddress("gp_p4"		, &gp_p4);
	Int_t gp_n; tGen->SetBranchAddress("gp_n",&gp_n);
	Short_t gp_status[MAXN]; tGen->SetBranchAddress("gp_status",gp_status);
	Short_t gp_pdgid[MAXN]; tGen->SetBranchAddress("gp_pdgid",gp_pdgid);
	Short_t gp_mother[MAXN]; tGen->SetBranchAddress("gp_mother",gp_mother);
	ULong64_t eventNum=0;tGen->SetBranchAddress("event",&eventNum);
	//JETS
	Int_t gjets_n;tGen->SetBranchAddress("genjet_algo1_n",&gjets_n);
	TClonesArray *gjets_p4=0; tGen->SetBranchAddress("genjet_algo1_p4",&gjets_p4);
	Float_t JetPtCut=25; //TO MAKE CONFIGURABLE
	Float_t JetPhoDR=0.3;

	if(debug>0) cout<<"[1] Loop Over Gen"<< endl;
	if(debug>1) cout<<"[2] GenEntries " << tGen->GetEntries()<<endl;
	for (long long iEntry=0; iEntry < tGen->GetEntries() ; iEntry++ )
		{
		if(debug>1)cout<<"[2] Loop Over Gen: "<<iEntry<<endl;
		tGen->GetEntry(iEntry);
		int pho1=-1,pho2=-1;
		if(debug>1)cout<<"[2] Got Entry "<<iEntry<<" - event:"<<eventNum<<endl;
		for(int igp=0;igp< gp_n ;igp++)
				{
				if(debug>2 && ((TLorentzVector*)(gp_p4->At(igp)) )->Pt() > 10) printf("[3] igp=%d is state %d particle w/ pdgid %d pt=%.1f eta=%.1f phi=%.1f\n",igp,gp_status[igp],gp_pdgid[igp],((TLorentzVector*)(gp_p4->At(igp)) )->Pt() ,((TLorentzVector*)(gp_p4->At(igp)) )->Eta(), ((TLorentzVector*)(gp_p4->At(igp)) )->Phi());
				if(debug>2)cout<<"[3] gp "<<igp<<" < "<< gp_n <<endl;
				if ( gp_status[igp] != 1) continue;
				if ( gp_pdgid[igp] != 22 ) continue;
				//if ( gp_mother[igp] != 25 ) continue;	// 0 % pass this cut
				//find a 25 in the full mother chain
				bool isHiggsSon=false;
				for( int mother=gp_mother[igp]; mother>=0 && mother != gp_mother[mother]  ;mother=gp_mother[mother])
					{
					if (gp_pdgid[mother] == 25) isHiggsSon=true;
					}
				if( !isHiggsSon) continue;
				if(debug>2) printf("[3] igp=%d is a photon, I already have: %d %d \n",igp,pho1,pho2);
				if(pho1>=0 && pho2>=0 && debug>1) printf("[2] igp=%d is a photon but I already have: %d %d \n",igp,pho1,pho2);
				if (pho1 <0 ) pho1=igp;
				if (pho1 >=0 && pho1 != igp && pho2 <0 ) pho2=igp;

				}
		if(debug>1)cout<<"[2] selection "<<pho1<<" "<<pho2<<endl;
		if (pho1 <0 || pho2 < 0) continue; 
		TLorentzVector g1= *((TLorentzVector*)(gp_p4->At(pho1)));
		TLorentzVector g2= *((TLorentzVector*)(gp_p4->At(pho2)));
		TLorentzVector hgg = g1+g2; 

		//find  jet related quontities
		Int_t nJets=0;
		Float_t Ht=0;
		Int_t jetIdx[10]={-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
		for(int iJet= 0 ;iJet< gjets_n; iJet++)
			{
			TLorentzVector j= * ((TLorentzVector*)gjets_p4->At(iJet));
			if(j.Pt() < JetPtCut) continue;
			if(j.DeltaR(g1)<0.3) continue;
			if(j.DeltaR(g2)<0.3) continue;
			jetIdx[nJets]=iJet;
			Ht+=j.Pt();
			nJets++;	
			}

		//get xsWeight: fileName & xsWeight
		string dName=tGen->GetCurrentFile()->GetName();	
		//remove /*
		size_t n=dName.rfind('/');
		assert(n != string::npos);
		dName=dName.erase(n);
		n=dName.rfind('/');
		if( n != string::npos )dName=dName.erase(0,n+1);

		if(debug>1)cout<<"[2] Dir = "<<dName<<endl;
		assert( xSecMapDirToNam.find(dName) != xSecMapDirToNam.end() );
		string xSecName=xSecMapDirToNam[dName];

		if( xSecWeight.find(xSecName) == xSecWeight.end() )
			{
			cout<<"------ ERROR -------" <<endl;
			cout<<"dir= "<<dName<<endl;
			cout<<"xSecName= "<<xSecName<<endl;
			for(map<string,float>::iterator it= xSecWeight.begin();it!=xSecWeight.end();it++)
				cout<<"xSecWeight "<<it->first <<" --- "<<it->second<<endl;
			}
		assert( xSecWeight.find(xSecName) != xSecWeight.end() );	
		float xsweight=xSecWeight[xSecName];

		if(debug>1)cout<<"[2] xSec "<<xSecName<<" "<< dName<<" "<< xSecWeight[xSecName]<<endl;

		bool isGen=true;
		// ph. sp. selection on photons 
		if (isGen){

	 	   string name="gen_hgg_pt"; Fill(name,hgg.Pt(),xsweight);
	 	   name="gen_hgg_coststar"; Fill(name,CosThetaStar(g1,g2),xsweight);
	 	   name="gen_hgg_deltaphi"; Fill(name,fabs(g1.DeltaPhi(g2)),xsweight);
	 	   name="gen_hgg_y"; Fill(name,hgg.Rapidity(),xsweight);

		//fill matrix and reco(no corrections for bkg);
		//do the matching to the photons 
		} //end ph. sp. selection on gen photons
		map< pair<string,unsigned long long int>, RecoInfo>::iterator reco=recoEvents.find(pair<string,unsigned long long>(xSecName,eventNum ));
		if(debug>2) cout<<"[3] Gen Entry "<<xSecName<<":"<<eventNum<<endl;

		if  (reco!=recoEvents.end() &&
			(
			 ( reco->second.pho1.DeltaR( g1 ) <0.3 && reco->second.pho2.DeltaR( g2 ) <0.3) ||  //match pho1->pho1 pho2->pho2
			 ( reco->second.pho1.DeltaR( g2 ) <0.3 && reco->second.pho2.DeltaR( g1 ) <0.3) //match pho1->pho2 pho2->pho1
			)
		    ){
			int iCat=-1;
			if(catMap.size()>0){iCat=catMap[reco->second.cat]; }
			assert(iCat<catMap.size());
		//fill maitrix
			if( isGen){
			   string name="response_hgg_pt";name+=Form("_cat%d",iCat);
			   Fill2D(name, hgg.Pt() , reco->second.hgg.Pt() , reco->second.weight);
	 	   name="response_hgg_coststar"; name+=Form("_cat%d",iCat); Fill2D(name,CosThetaStar(g1,g2),	CosThetaStar(reco->second.pho1,reco->second.pho2),  reco->second.weight);
	 	   name="response_hgg_deltaphi"; name+=Form("_cat%d",iCat); Fill2D(name,fabs(g1.DeltaPhi(g2)),	fabs(reco->second.pho1.DeltaPhi(reco->second.pho2))	,reco->second.weight);
	 	   name="response_hgg_y";	 name+=Form("_cat%d",iCat); Fill2D(name,hgg.Rapidity(),		reco->second.hgg.Rapidity(),reco->second.weight);
			}
		//fill reco histo
			string name="reco_hgg_pt";name+=Form("_cat%d",iCat);
			Fill(name,reco->second.hgg.Pt(),reco->second.weight);
	 	   	name="reco_hgg_coststar"; name+=Form("_cat%d",iCat); Fill(name,CosThetaStar(reco->second.pho1,reco->second.pho2),  reco->second.weight);
	 	   	name="reco_hgg_deltaphi"; name+=Form("_cat%d",iCat); Fill(name,fabs(reco->second.pho1.DeltaPhi(reco->second.pho2))	,reco->second.weight);
	 	   	name="reco_hgg_y";	 name+=Form("_cat%d",iCat); Fill(name,reco->second.hgg.Rapidity(),reco->second.weight);
			} //exists reco and matched to gen level
		else{
			//not matched  or not exists -- only debug
			if(reco!=recoEvents.end() ) // not matched
				if(debug>1)cout<<"[2] hgg reco PT NOT MATCHED="<<reco->second.hgg.Pt()<<" DR o-o "<<reco->second.pho1.DeltaR( g1 ) <<" "<< reco->second.pho2.DeltaR( g2 ) << " DR x-x "<<reco->second.pho1.DeltaR( g2 )  <<" "<< reco->second.pho2.DeltaR( g1 ) <<endl;
		}

		}//loop over gen entries

};

int Unfolding::InitRecoOptTree(){
	TChain *ggH=new TChain("ggh_m125_8TeV");
	TChain *wzH=new TChain("wzh_m125_8TeV");
	TChain *ttH=new TChain("tth_m125_8TeV");
	TChain *qqH=new TChain("vbf_m125_8TeV");

	if (tReco !=NULL ) delete tReco;
	tReco=new TChain("cmshgg");
	if(debug>0) cout<<"[1] Adding"<<recoFiles.size()<<" files to Reco Chain"<<endl;
	for(vector<string>::iterator iFile=recoFiles.begin();iFile!=recoFiles.end();iFile++)
		{
		if(debug>0)cout<< "[1] Adding file: "<<*iFile<<" to the list of reco Chains: ";
		int R_1=ggH->Add( iFile->c_str() );
		int R_2=qqH->Add( iFile->c_str() );
		int R_3=wzH->Add( iFile->c_str() );
		int R_4=ttH->Add( iFile->c_str() );
		if(debug>0) cout << R_1 <<"_"<<R_2<<"_"<<R_3<<"_"<<R_4<<endl;
		assert ( R_1==1);
		assert ( R_2==1);
		assert ( R_3==1);
		assert ( R_4==1);
		}
	tReco->Add(ggH);
	tReco->Add(qqH);
	tReco->Add(wzH);
	tReco->Add(ttH);
	//delete ggH; // ?? 
	//delete wzH;
	//delete qqH;
	//delete ttH;
	//-------------Set Branch Address ----------
	//INCOMPLETE	
	return 0;
};

int Unfolding::LoopOverRecoOptTree(){
//tReco->GetTree()->GetName()
	if(debug>0) cout<<"Init Opt Tree"<<endl;
	InitRecoOptTree();
	if(debug>0) cout<<"Loop Over Opt Tree"<<endl;
	Float_t run_=-999;
	Float_t lumi_=-999; 
	Double_t event_=-999;
	Float_t pho1_et_=-999;
	Float_t pho1_eta_=-999;
	Float_t pho1_phi_=-999;
	Float_t pho2_et_=-999;
	Float_t pho2_eta_=-999;
	Float_t pho2_phi_=-999;
	Float_t cat_=-999;
	Float_t weight_=1;
	Float_t xsweight_=-999;
	string Nam_;
	//SetBranchAddress
	tReco->SetBranchAddress("xsec_weight",&xsweight_);
	tReco->SetBranchAddress("full_weight",&weight_);
	tReco->SetBranchAddress("run",&run_);
	tReco->SetBranchAddress("lumi",&lumi_);
	tReco->SetBranchAddress("event",&event_);
	tReco->SetBranchAddress("eta1",&pho1_eta_);
	tReco->SetBranchAddress("eta2",&pho2_eta_);
	tReco->SetBranchAddress("et1",&pho1_et_);
	tReco->SetBranchAddress("et2",&pho2_et_);
	tReco->SetBranchAddress("scphi1",&pho1_phi_); //TODO
	tReco->SetBranchAddress("scphi2",&pho2_phi_);
	tReco->SetBranchAddress("full_cat",&cat_);
	//TODO Implement Jets in txt Sync and OptTree. I don't have all info
	Float_t jet1_eta,jet1_pt,jet1_phi;
	Float_t jet2_eta,jet2_pt,jet2_phi;

	Int_t njets20;

	//tReco->SetBranchAddress("");

	for(Int_t iEntry=0;iEntry<tReco->GetEntries();iEntry++)
	{
	tReco->GetEntry(iEntry);
	Nam_=tReco->GetTree()->GetName();
			TLorentzVector pho1,pho2,hgg; 
			pho1.SetPtEtaPhiM(pho1_et_ ,pho1_eta_,pho1_phi_,0);
			pho2.SetPtEtaPhiM(pho2_et_ ,pho2_eta_,pho2_phi_,0);

			hgg=pho1+pho2;
			RecoInfo A(hgg,pho1,pho2,cat_,weight_);
			if(debug>2) cout<<"[3] hgg reco PT="<< hgg.Pt()<<endl;
			recoEvents[pair<string,unsigned long long >(Nam_,event_)]=A;
			if(debug>1) cout<<"[2] recoEvents"<<recoEvents.size()<<endl;
			if(xsweight_>0)
				{
				xSecWeight[Nam_]=xsweight_;
				}
			if(debug > 2) printf("[3] Readed values: %.1f %.1f %.1f %.1f %.1f %.1f %.1f %.1f %.1f %.1f %.1f %.1f %s\n",run_,lumi_,event_,pho1_et_,pho1_eta_,pho1_phi_,pho2_et_,pho2_eta_,pho2_phi_,cat_,weight_,xsweight_,Nam_.c_str());
			assert(weight_>0);
			assert(xsweight_>0);
	}
	return 0 ;

}

int Unfolding::InitReco(){
	if(debug>0)cout<<"[1] Init Reco"<<endl;

}
int Unfolding::LoopOverReco(){
	InitReco();
	if(debug>0)cout<<"[1] Loop Over Reco"<<endl;

	for (vector<string>::iterator iFile=recoFiles.begin(); iFile!=recoFiles.end();iFile++)
		{
		if(debug>1)cout<<"[2] Loop Over Reco: File "<<*iFile<<endl;
		FILE *fr=fopen(iFile->c_str(),"r");
		if (fr == NULL) fprintf(stderr,"Error file %s does not exist\n",iFile->c_str());
		char buf[2048]; //max length of buffer-line
		while(fgets(buf,2048,fr)!=NULL)
			{
			if(debug>2)printf(".");
			int n; double value; char branch[2048];
			//sostituisco i : con ' ' ? 
			long int run_=-999;
			long int lumi_=-999; 
			long long int event_=-999;
			float pho1_e_=-999;
			float pho1_eta_=-999;
			float pho1_phi_=-999;
			float pho2_e_=-999;
			float pho2_eta_=-999;
			float pho2_phi_=-999;
			int cat_=-999;
			float weight_=1;
			float xsweight_=-999;
			char Nam_[1023];
			string buf_s=buf;
			std::replace(buf_s.begin(),buf_s.end(),':',' ');
			std::replace(buf_s.begin(),buf_s.end(),'\t',' ');
			const char*line=buf_s.c_str()	;
			while (sscanf(line,"%s%n",branch,&n) != EOF )
				{
				line+=n;
				if(debug>2) printf("[3] branch=%s\n",branch);

				if( string(branch) == "Nam") 
					{
					sscanf(line,"%s%n",Nam_,&n);
					}
				else
					sscanf(line,"%lf%n",&value,&n);
				if( string(branch) == "run") run_=value ;
				if( string(branch) == "lumi") lumi_=value ;
				if( string(branch) == "event") event_=value;
				if( string(branch) == "pho1_e") pho1_e_=value;
				if( string(branch) == "pho1_eta") pho1_eta_=value;
				if( string(branch) == "pho1_phi") pho1_phi_=value;
				if( string(branch) == "pho2_e") pho2_e_=value;
				if( string(branch) == "pho2_eta") pho2_eta_=value;
				if( string(branch) == "pho2_phi") pho2_phi_=value;
				if( string(branch) == "cat") cat_=value;
				if( string(branch) == "fullweight") weight_=value; //not in txt yet
				if( string(branch) == "xsweight") xsweight_=value; //not in txt yet
				//if( string(branch) == "sigtyp") sigtyp_=int(value); //not in txt yet
				line +=n;
				}
			TLorentzVector pho1,pho2,hgg; 
			float t=TMath::Exp(-pho1_eta_);
			pho1.SetPtEtaPhiM(pho1_e_ * 2*t/(1+t*t),pho1_eta_,pho1_phi_,0);
			t=TMath::Exp(-pho2_eta_);
			pho2.SetPtEtaPhiM(pho2_e_ * 2*t/(1+t*t),pho2_eta_,pho2_phi_,0);

			hgg=pho1+pho2;
			RecoInfo A(hgg,pho1,pho2,cat_,weight_);
			if(debug>2) cout<<"[3] hgg reco PT="<< hgg.Pt()<<endl;
			recoEvents[pair<string,unsigned long long >(Nam_,event_)]=A;
			if(debug>1) cout<<"[2] recoEvents"<<recoEvents.size()<<endl;
			if(xsweight_>0)
				{
				xSecWeight[Nam_]=xsweight_;
				}
			if(debug > 2) printf("[3] Readed values: %ld %ld %lld %.1f %.1f %.1f %.1f %.1f %.1f %d %.1f %.1f %s\n",run_,lumi_,event_,pho1_e_,pho1_eta_,pho1_phi_,pho2_e_,pho2_eta_,pho2_phi_,cat_,weight_,xsweight_,Nam_);
			assert(weight_>0);
			assert(xsweight_>0);

			} // loop over lines in one file
		fclose(fr);
		}

};

int Unfolding::Write(string output){
	TFile *out=TFile::Open(output.c_str(),"RECREATE");
	out->cd();
for (set<string>::iterator hName=histoToSave.begin();hName!=histoToSave.end();hName++)
	{
	if(debug>0)cout<<"[1] Going to Write "<< *hName<<endl;
	if( hName->find("resp") !=string::npos)
	Container.GetEntryPtr<TH2D>( *hName )->Write();
	else
	Container.GetEntryPtr<TH1D>( *hName )->Write();
	}
	out->Close();
};

float Unfolding::CosThetaStar(TLorentzVector &a,TLorentzVector&b)
{
	TLorentzVector h=a+b;
	TLorentzVector a1=a;
	a1.Boost(-h.BoostVector());
	return -a1.CosTheta();
}


Bins Unfolding::GetBins(string name)
{
	Bins R;
	if( name.find("hgg_pt")!=string::npos)		{R.nBins=60; R.xMin=0;R.xMax=120;}
	else if( name.find("hgg_coststar")!=string::npos)	{R.nBins=5; R.xMin=0;R.xMax=1;}
	else if( name.find("hgg_deltaphi")!=string::npos)	{R.nBins=5; R.xMin=0;R.xMax=3.1416;}
	else if( name.find("hgg_y")!=string::npos)		{R.nBins=5; R.xMin=0;R.xMax=5;}
	else {
		if(debug>0) cout<<"[1] Observable "<<name<<" unknown binnig"<<endl;
		R.nBins=100;R.xMin=0;R.xMax=100;
	}

	return R;
}

void Unfolding::Fill(string name, float value, float weight)
{
		   if( histoToSave.insert(name).second && debug>0) cout<<"[1] Booking histo "<<name<<" to be saved"<<endl;

		   if( Container.GetEntryPtr<TH1D>(name) == NULL ) // if it does not exists create it 
			{
		   	if(debug>1)cout<<"[2] Creating histo "<<name<<" int Container"<<endl;
			TH1D *default_th1d = NULL;
			Bins R=GetBins(name);
			if(R.isConstBin)
				default_th1d=new TH1D(name.c_str(),name.c_str(),R.nBins,R.xMin,R.xMax);
			else default_th1d=new TH1D(name.c_str(),name.c_str(),R.nBins,R.bins);
		       	Container.SetEntry(name,*default_th1d,"TH1D");
			}
		   Container.GetEntryPtr<TH1D>(name)->Fill(value,weight); //weight  -> xsWeight
		   return; 
}

void Unfolding::Fill2D(string name, float value1,float value2, float weight)
{
		   if( histoToSave.insert(name).second && debug>0) cout<<"[1] Booking histo "<<name<<" to be saved"<<endl;

		   if( Container.GetEntryPtr<TH2D>(name) == NULL ) // if it does not exists create it 
			{
		   	if(debug>1)cout<<"[2] Creating histo "<<name<<" int Container"<<endl;
			TH2D *default_th2d;//=new TH2D(name.c_str(),name.c_str(),100,0.,100.,100,0.,100);
			Bins R=GetBins(name);
			if(R.isConstBin)
				default_th2d=new TH2D(name.c_str(),name.c_str(),R.nBins,R.xMin,R.xMax,R.nBins,R.xMin,R.xMax);
			else default_th2d=new TH2D(name.c_str(),name.c_str(),R.nBins,R.bins,R.nBins,R.bins);
		       	Container.SetEntry(name,*default_th2d,"TH2D");
			}
		   Container.GetEntryPtr<TH2D>(name)->Fill(value1,value2,weight); //weight  -> xsWeight
		   return; 
}

void Unfolding::SetCatsModulo(int M)
{
	int nCat=catMap.size();
	if( nCat % M != 0 ) 
		{
		cout<<"error nCats" <<nCat<<" not modulo "<<M<<endl;
		return;
		}
	for( int iCat = 0 ;iCat < nCat ;iCat++)
	{
		SetCat(iCat, iCat%M);
	}
	return;
}
