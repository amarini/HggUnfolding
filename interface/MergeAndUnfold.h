#ifndef MaU_H
#define MaU_H
#include "TH1.h"
#include "TH2.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TH1F.h"
#include "TMatrix.h"
#include "TMatrixD.h"
#include "TVector.h"
#include "TVectorD.h"
#include "TDecompSVD.h"
#include "TMath.h"
#include "TObject.h"

#include "Math/Functor.h"

#include <string>
#include <vector>
#include <map>
using namespace std;


class MergeAndUnfold:public TObject{
public:
	MergeAndUnfold(){useOverFlow=0;regIndex=-1;covMatrix.clear();}
	int AddCat(TH1D *h,TH1D *g,TH1D *r,TH2D *resp); //return catNum on success
	int AddCat(TH1D *h,TH1D *g,TH1D *r,TH2D *resp,TH2D* cov); //return catNum on success
	int AddReg(double delta); 
	void Reset(); 
	TVectorD Unfold(TMatrixD *E=NULL);
	TH1D* UnfoldMinimum(); 
	TH1D* UnfoldLogLikelihoodMinimum( ROOT::Math::Functor &f );
	//TH1D* UnfoldLogLikelihoodMinimum( int nPar , double likelihood(const double*));
	TH1D* getHisto(TVectorD &v, TMatrixD &e); 
	TH1D* UnfoldSvd(vector<int> v_kreg);	
	TH1D* UnfoldBayes(int niter=3);// use RooUnfold and merge
	void SetOverFlow(){useOverFlow=1;};
	void UnsetOverFlow(){useOverFlow=0;};
	int FillVectors();

	vector<TH1D> u;

private:	
	vector<TH1D> histos;

	vector<TH2D> covMatrix;
	vector<TH2D> matrix;
	vector<TH1D> gen; // only 1
	vector<TH1D> reco;

	bool useOverFlow;
	TVectorD getVector(TH1 *h);
	TMatrixD getMatrix(TH2 *h);
	TMatrixD getCovMatrix(TH1 *h);
	TH2D*	 getCovMatrixH(TH1 *h);
	TVectorD integrateRow(TMatrixD &a);
	TVectorD integrateCol(TMatrixD &a);
	void ConstructSuperMatrixes();

	vector<TVectorD> v_h;
	vector<TVectorD> v_g;
	vector<TVectorD> v_r;
	vector<TMatrixD> v_m;
	vector<TMatrixD> v_s; //covariance matrix

	int regIndex;

	TMatrixD S,K;
	TVectorD l,y; 
public:
	ClassDef(MergeAndUnfold,1);
};

#endif
