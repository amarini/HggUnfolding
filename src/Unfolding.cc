
#include "TFile.h"

#define Unfolding_cxx
#include "interface/Unfolding.h"
#include <assert.h>




// --- use std as namespace
using namespace std;

int Unfolding::InitGen(){
	if (tGen !=NULL ) delete tGen;
	tGen=new TChain("event");
	if(debug>0) cout<<"Adding"<<genFiles.size()<<" files to Gen Chain"<<endl;
	for(vector<string>::iterator iFile=genFiles.begin();iFile!=genFiles.end();iFile++)
		{
		if(debug>0)cout<< "Adding file: "<<*iFile<<" to the list of gen Chains: ";
		int R=tGen->Add( iFile->c_str() );
		if(debug>0) cout << R<<endl;
		assert ( R==1);
		}
	//Set Branches
	int default_int=-999;
	long default_long=-999;
	float default_float=-999.;	
	TLorentzVector default_p4();
	int  *default_v_int;
	float *default_v_float;
	TLorentzVector *default_v_p4;

	Container.SetEntry("eventNum",&default_long,"long");
	tGen->SetBranchAddress("event",Container.GetEntryPtr<long>("eventNum"));

	Container.SetEntry("gp_n",&default_int,"int");
	default_v_int =new int[MAXN]; Container.SetEntry("gp_status",&default_v_int,"v_int");
	default_v_int =new int[MAXN]; Container.SetEntry("gp_pdgid",&default_v_int,"v_int");
	default_v_int =new int[MAXN]; Container.SetEntry("gp_mother",&default_v_int,"v_int");
	default_v_p4 =new TLorentzVector[MAXN]; Container.SetEntry("gp_p4",&default_v_p4,"v_p4");
	tGen->SetBranchAddress("gp_n",Container.GetEntryPtr<int>("gp_n"));
	tGen->SetBranchAddress("gp_status",Container.GetEntryPtr<int*>("gp_status"));
	tGen->SetBranchAddress("gp_pdgid",Container.GetEntryPtr<int*>("gp_pdgid"));
	tGen->SetBranchAddress("gp_mother",Container.GetEntryPtr<int*>("gp_mother"));
	tGen->SetBranchAddress("gp_p4",Container.GetEntryPtr<TLorentzVector*>("gp_p4"));

	return 0;
	};
int Unfolding::LoopOverGen(){
	InitGen();
	for (unsigned long long int iEntry=0;iEntry<tGen->GetEntries();iEntry++)
		{
		tGen->GetEntry(iEntry);
		int pho1=-1,pho2=-1;
		for(int igp=0;igp< Container.GetEntry<int>("gp_n") ;igp++)
				{
				if ( Container.GetEntry<int*>("gp_status")[igp] != 1) continue;
				if ( Container.GetEntry<int*>("gp_pdgid")[igp] != 22 ) continue;
				int mother= Container.GetEntry<int*>("gp_mother")[igp];
				if( Container.GetEntry<int*>("gp_pdgid")[mother] !=23 ) continue; //check Higgs PdgID
				if (pho1 <0 ) pho1=igp;
				if (pho1 >=0 && pho1 != igp && pho2 <0 ) pho2=igp;

				}
		if (pho1 <0 || pho2 < 0) continue; 
		TLorentzVector hgg = Container.GetEntry<TLorentzVector>("gp_p4")[pho1] + Container.GetEntry<TLorentzVector>("gp_p4")[pho2];

		//get xsWeight: fileName & xsWeight
		string fName=tGen->GetCurrentFile()->GetName();	
		//remove /*
		size_t n=fName.rfind('/');
		assert(n != string::npos);
		fName=fName.erase(n);
		assert( xSecWeight.find(fName) != xSecWeight.end() );	
		float xsweight=xSecWeight[fName];

		bool isGen=true;
		// ph. sp. selection on photons 
		if (isGen){
	 	   string name="gen_hgg_pt";
		   if( Container.GetEntryPtr<TH1D>(name) != NULL ) // if it does not exists create it 
			{
			TH1D *default_th1d=new TH1D(name.c_str(),name.c_str(),100,0.,100.);
		       	Container.SetEntry(name,default_th1d,"TH1D");
			}
		   Container.GetEntryPtr<TH1D>(name)->Fill(hgg.Pt(),xsweight); //weight  -> xsWeight
		//fill matrix and reco(no corrections for bkg);
		//do the matching to the photons 
		} //end ph. sp. selection on gen photons
		map<unsigned long long int, RecoInfo>::iterator reco=recoEvents.find(Container.GetEntry<long>("eventNum"));

		if  (reco!=recoEvents.end() &&
			(
			 ( reco->second.pho1.DeltaR( Container.GetEntry<TLorentzVector>("gp_p4")[pho1] ) <0.3 && reco->second.pho2.DeltaR( Container.GetEntry<TLorentzVector>("gp_p4")[pho2] ) <0.3) ||  //match pho1->pho1 pho2->pho2
			 ( reco->second.pho1.DeltaR( Container.GetEntry<TLorentzVector>("gp_p4")[pho2] ) <0.3 && reco->second.pho2.DeltaR( Container.GetEntry<TLorentzVector>("gp_p4")[pho1] ) <0.3) //match pho1->pho2 pho2->pho1
			)
		    ){
			int iCat=-1;
			if(catMap.size()>0){iCat=catMap[reco->second.cat]; }
			assert(iCat<catMap.size());
		//fill maitrix
			if( isGen){
			   string name="response_hgg_pt";
			   name+=Form("_cat%d",iCat);
			   if( Container.GetEntryPtr<TH2D>(name) != NULL ) // if it does not exists create it 
				{
				TH2D *default_th2d=new TH2D(name.c_str(),name.c_str(),100,0.,100.,100,0.,100.);
		       		Container.SetEntry(name,default_th2d,"TH2D");
				}
			   Container.GetEntryPtr<TH2D>(name)->Fill( hgg.Pt() , reco->second.hgg.Pt() , reco->second.weight); 
			}
		//fill reco histo
			string name="reco_hgg_pt";
			name+=Form("_cat%d",iCat);
			if( Container.GetEntryPtr<TH1D>(name) != NULL ) // if it does not exists create it 
				{
				TH1D *default_th1d=new TH1D(name.c_str(),name.c_str(),100,0.,100.);
				Container.SetEntry(name,default_th1d,"TH1D");
				}
			Container.GetEntryPtr<TH1D>(name)->Fill(reco->second.hgg.Pt(),reco->second.weight); 
			} //exists reco and matched to gen level

		}//loop over gen entries

};

int Unfolding::InitRecoOptTree(){
	TChain *ggH=new TChain("ggh_m125_8TeV");
	TChain *wzH=new TChain("wzh_m125_8TeV");
	TChain *ttH=new TChain("tth_m125_8TeV");
	TChain *qqH=new TChain("vbf_m125_8TeV");

	if (tReco !=NULL ) delete tReco;
	tReco=new TChain("cmshgg");
	if(debug>0) cout<<"Adding"<<recoFiles.size()<<" files to Reco Chain"<<endl;
	for(vector<string>::iterator iFile=recoFiles.begin();iFile!=recoFiles.end();iFile++)
		{
		if(debug>0)cout<< "Adding file: "<<*iFile<<" to the list of reco Chains: ";
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
	int default_int=-999;
	float default_float=-999.;	
	TLorentzVector default_p4();
	int  *default_v_int;
	float *default_v_float;
	TLorentzVector *default_v_p4;
	Container.SetEntry("weight",&default_float,"float"); tReco->SetBranchAddress("full_weight",Container.GetEntryPtr<float*>("weight"));
	//INCOMPLETE	
	return 0;
};

int Unfolding::InitReco(){

}
int Unfolding::LoopOverReco(){
	InitReco();

	for (vector<string>::iterator iFile=recoFiles.begin(); iFile!=recoFiles.end();iFile++)
		{
		FILE *fr=fopen(iFile->c_str(),"r");
		if (fr == NULL) fprintf(stderr,"Error file %s does not exist\n",iFile->c_str());
		char buf[2048]; //max length of buffer-line
		while(fgets(buf,2048,fr)!=NULL)
			{
			char*line=buf	;
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
			while (sscanf(line,"\t%s:%lf%n",branch,&value,&n) != EOF )
				{
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
				}
			TLorentzVector pho1,pho2,hgg; 
			float t=TMath::Exp(-pho1_eta_);
			pho1.SetPtEtaPhiM(pho1_e_ * 2*t/(1+t*t),pho1_eta_,pho1_phi_,0);
			t=TMath::Exp(-pho2_eta_);
			pho2.SetPtEtaPhiM(pho2_e_ * 2*t/(1+t*t),pho2_eta_,pho2_phi_,0);

			hgg=pho1+pho2;
			RecoInfo A(hgg,pho1,pho2,cat_,weight_);
			recoEvents[event_]=A;
			if(xsweight_>0)
				{
				string fName=*iFile;// remove /* save in the map
				size_t n=fName.rfind('/');
				if( n != string::npos)
				{
					fName=fName.erase(n);
					xSecWeight[fName]=xsweight_;
				}//exists n
				
				}
			assert(weight_>0);
			assert(xsweight_>0);

			} // loop over lines in one file
		fclose(fr);
		}

};

int Unfolding::Write(){};


