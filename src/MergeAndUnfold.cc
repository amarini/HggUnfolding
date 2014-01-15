#include "interface/MergeAndUnfold.h"
#include <assert.h>



// ---------------------- MACRO THAT DO UNFOLDING --------------
TVectorD MergeAndUnfold::Unfold(TMatrixD *E){

//convert histos in VectorD
FillVectors();

//construct super matrixes by chaining all the other ones
TMatrixD S,K;
TVectorD l,y; 

//Get Unfold distribution
//l=S.(kS)+ .y
/* 
 * |   |   | K1 |+   | y1 |
 * | l | = | K2 |  . | y2 |
 * |   |   | K3 |    | y3 |
 *         | aL |    |  0 |  a=sqrt(Delta) Tickonov Regularization
 */
l.ResizeTo(v_g[0].GetNrows());
int nCat=v_h.size();
K.ResizeTo( v_g[0].GetNrows() ,nCat * v_r[0].GetNrows() );
S.ResizeTo( v_r[0].GetNrows() * nCat,  v_r[0].GetNrows() * nCat );
y.ResizeTo( nCat*v_r[0].GetNrows() );
//fill K S y

for(int iCat=0;iCat<nCat;iCat++){
	//K 
	for(int gBin=0;gBin<v_g[0].GetNrows();gBin++)
	for(int rBin=0;rBin<v_r[0].GetNrows();rBin++)
		{
		K(gBin,rBin + v_r[0].GetNrows() * iCat) = v_m[iCat](gBin,rBin);
		}
	//S
	for(int rBin=0;rBin<v_r[0].GetNrows();rBin++)
	for(int rBin2=0;rBin2<v_r[0].GetNrows();rBin2++)
		{
		S(rBin + v_r[0].GetNrows() * iCat,rBin2+ v_r[0].GetNrows() * iCat) = v_s[iCat](rBin,rBin2);
		}
	//y 
	for(int rBin=0;rBin<v_r[0].GetNrows();rBin++)
		{
		y(rBin + v_r[0].GetNrows() * iCat) = v_h[iCat](rBin);
		}
} //loop over categories

//is defined * and + .Invert();
TMatrixD Kt(K.GetNcols(),K.GetNrows()); Kt.Transpose(K);
TMatrixD A=Kt*S*K; A.Invert();
//l=A*Kt*S*y;
TMatrixD B=A*Kt*S;
l=B*y;

TMatrixD Bt(B.GetNcols(),B.GetNrows());Bt.Transpose(B);
TMatrixD cov=B*S*Bt;  //covariance matrix after linear transformation

//multiply back for v_g
for(int i=0;i<v_g[0].GetNrows();i++)
	l(i)*=v_g[0](i);
for(int iBin=0;iBin<v_g[0].GetNrows();iBin++)
for(int jBin=0;jBin<v_g[0].GetNrows();jBin++)
	cov(iBin,jBin)*=v_g[0](iBin)*v_g[0](jBin);

if(E!=NULL)(*E)=cov;
//get back a histo
return l;
};

//------------------ Tools --------------------
TVectorD MergeAndUnfold::getVector(TH1 *h){
	TVectorD r;
	int n=h->GetNbinsX();
	if (useOverFlow)n+=2;
	r.ResizeTo(n);
	int i=0;
	if(useOverFlow) { r(i) = h->GetBinContent(i);i++;}
	for(int iBin=1;iBin<=h->GetNbinsX();iBin++)
		{r(i)=h->GetBinContent(iBin);i++;}
	if(useOverFlow) { r(i) = h->GetBinContent(h->GetNbinsX()+1);i++;}
	return r;
}

TMatrixD MergeAndUnfold::getMatrix(TH2 *h){
	TMatrixD r;
	int n=h->GetNbinsX();
	int m=h->GetNbinsY();
	if (useOverFlow)n+=2;
	if (useOverFlow)m+=2;
	r.ResizeTo(n,m);
	r.Zero(); //make sure is set to 0
	if(useOverFlow) 
	{
		for(int iBin=1;iBin<=h->GetNbinsX();iBin++)
		for(int jBin=1;jBin<=h->GetNbinsY();jBin++)
			r(iBin,jBin)=h->GetBinContent(iBin,jBin);
	
	}
	else{
		for(int iBin=0;iBin<=h->GetNbinsX()+1;iBin++)
		for(int jBin=0;jBin<=h->GetNbinsY()+1;jBin++)
			r(iBin,jBin)=h->GetBinContent(iBin,jBin);
	}
	return r;
}
TMatrixD MergeAndUnfold::getCovMatrix(TH1 *h){
	TMatrixD r;
	int n=h->GetNbinsX();
	int m=h->GetNbinsY();
	if (useOverFlow)n+=2;
	if (useOverFlow)m+=2;
	r.ResizeTo(n,m);
	r.Zero(); // make sure is set to 0
	if(useOverFlow) 
	{
		for(int iBin=1;iBin<=h->GetNbinsX();iBin++)
			r(iBin,iBin)=TMath::Power(h->GetBinError(iBin),2);
	
	}
	else{
		for(int iBin=0;iBin<=h->GetNbinsX()+1;iBin++)
			r(iBin,iBin)=TMath::Power(h->GetBinError(iBin),2);
	}
	return r;
}

int MergeAndUnfold::FillVectors()
{
	if ( gen.size() != 1 ) return 1;	 // only one gen target
	if ( histos.size() != matrix.size() ) return 1;	
	if ( histos.size() != reco.size() ) return 1;	
	v_h.resize(histos.size());
	v_g.resize(histos.size());  //for semplicity - same syntax
	v_r.resize(histos.size());
	v_m.resize(histos.size());
	v_s.resize(histos.size());

	for(size_t i=0;i<histos.size();i++)
	{
		v_h[i]=getVector( &histos[i] );
		v_g[i]=getVector( &gen[0] );
		v_r[i]=getVector( &reco[i] );
		v_m[i]=getMatrix( &matrix[i] );
		v_s[i]=getCovMatrix( &histos[i] );
	}

	//get efficiency and multiply matrix TODO, check this part
	for(size_t iCat=0;iCat<histos.size();iCat++)
	{
		if(iCat==regIndex) continue;
		TVectorD p_g=integrateCol(v_m[iCat]);
		TVectorD p_r=integrateRow(v_m[iCat]);
		assert(p_g.GetNrows() == v_g[iCat].GetNrows() );
	//	//get efficiencies  -- not necessary see SVD pg9 eq (31). One gets wi out of this procedure
	//	for(int i=0;i<p_g.GetNrows();i++)
	//		{
	//		p_g(i)=p_g(i)/v_g[0](i);
	//		}
	//	//p_g now has the efficiencies
	//	for(int iBin=0;iBin<v_m[iCat].GetNrows();iBin++)
	//	for(int jBin=0;jBin<v_m[iCat].GetNcols();jBin++)
	//	{
	//		v_m[iCat](iBin,jBin) *= p_g(jBin); //is it needed or not?
	//	}
		//subtract bkg
		for(int i=0;i<p_r.GetNrows();i++)
			{
			p_r(i)=p_r(i)/v_r[iCat](i);
			}
		for(int iBin=0;iBin< v_s[iCat].GetNrows();iBin++)
		for(int jBin=0;jBin< v_s[iCat].GetNcols();jBin++)
			{
			//v_s[iCat](iBin,jBin) *= p_r(iBin)*p_r(jBin); // actually error should not increase because it is a difference.
			if(jBin==iBin)v_h[iCat](iBin) *= p_r(iBin);
			}

	}
		
	// Remember to multiply back by v_g[] after unfolding.
	//
	return 0;	
}

int MergeAndUnfold::AddCat(TH1D *h,TH1D *g,TH1D *r,TH2D *resp)
{
	histos.push_back( *(TH1D*)(h->Clone(Form("histo_cat%d",int(histos.size())))) );
	if (gen.size()==0)gen.push_back( *(TH1D*)(g->Clone(Form("gen_cat%d", int(gen.size())))) );
	reco.push_back( *(TH1D*)(r->Clone(Form("reco_cat%d",int(reco.size())))) );
	matrix.push_back( *(TH2D*)(resp->Clone(Form("matrix_cat%d",int(matrix.size())))) );
	return int(histos.size())-1;
}


void MergeAndUnfold::Reset()
{
	histos.clear();
	matrix.clear();
	gen.clear();
	reco.clear();
	useOverFlow=0;
	return ;
}

int MergeAndUnfold::AddReg(double delta){
	if (histos.size()==0) return 1;
	if (regIndex >= 0 ) return 2;  //already exists a regularization factor

	TH1D * h=(TH1D*)histos[0].Clone("reg");
	TH2D * m=(TH2D*)matrix[0].Clone("reg2");
	for(int iBin=0;iBin<=m->GetNbinsX()+1;iBin++)
	for(int jBin=0;jBin<=m->GetNbinsY()+1;jBin++)
		{
		if( iBin == jBin ) m->SetBinContent(iBin,jBin,-2);
		else if( abs(iBin-jBin) == 1 ) m->SetBinContent(iBin,jBin,1);
		else m->SetBinContent(iBin,jBin,0);
		m->SetBinError(iBin,jBin,0);
		}
	if( useOverFlow )
	{
		m->SetBinContent(0,0,-1);
		m->SetBinContent(m->GetNbinsX()+1,m->GetNbinsY()+1,-1);
	}
	else
	{
		m->SetBinContent(0,0,1);
		m->SetBinContent(0,1,0);
		m->SetBinContent(1,0,0);
		m->SetBinContent(1,1,-1);
		m->SetBinContent(m->GetNbinsX()+1,m->GetNbinsY()+1,1);
		m->SetBinContent(m->GetNbinsX()+1,m->GetNbinsY(),0);
		m->SetBinContent(m->GetNbinsX(),m->GetNbinsY()+1,0);
		m->SetBinContent(m->GetNbinsX(),m->GetNbinsY(),-1);
	}
	for(int iBin=0;iBin<=h->GetNbinsX()+1;iBin++)
	{
		h->SetBinContent(iBin,0);
		h->SetBinError(iBin,1);
	}

	m->Scale(TMath::Sqrt(delta));

	regIndex=AddCat(h,m->ProjectionX(),m->ProjectionY(),m);
	return 0;
};


TVectorD MergeAndUnfold::integrateRow(TMatrixD &a)
{
	TVectorD r;
	r.ResizeTo(a.GetNcols());
	r.Zero();
	for (int i=0;i<a.GetNrows();i++)
	for (int j=0;j<a.GetNcols();j++)
	{
		r(j)+=a(i,j);
	}
	return r;
}
TVectorD MergeAndUnfold::integrateCol(TMatrixD &a)
{
	TVectorD r;
	r.ResizeTo(a.GetNrows());
	r.Zero();
	for (int i=0;i<a.GetNrows();i++)
	for (int j=0;j<a.GetNcols();j++)
	{
		r(i)+=a(i,j);
	}
	return r;
}


TH1D* MergeAndUnfold::UnfoldMinimum(){
	TMatrixD e;
	TVectorD v=Unfold(&e);
	return getHisto(v,e);
} 
TH1D* MergeAndUnfold::getHisto(TVectorD &v, TMatrixD &e){
	TH1D *h=(TH1D*)gen[0].Clone("Gen");
	int start=1;
	int end=h->GetNbinsX();
	int i=0;
	if(useOverFlow)start--;
	if(useOverFlow)end++;
	for(int iBin=start;iBin<=end;iBin++,i++)
		{
			h->SetBinContent(iBin, v(i) );
			h->SetBinError(iBin, TMath::Sqrt(e(i,i)) );
		}
}


#include "RooUnfold/RooUnfoldSvd.h"
#include "RooUnfold/RooUnfoldResponse.h"
TH1D* MergeAndUnfold::UnfoldSvd(int kreg){
	vector<TH1D> u;
	for (int iCat=0;iCat<histos.size();iCat++)
		{
		RooUnfoldResponse R( &reco[iCat], &gen[0], &matrix[iCat], Form("Resp_%d",iCat), "resp");
		RooUnfoldSvd U(&R,&histos[iCat],kreg,1000);
		U.SetNToys(1000);
		u.push_back(  *((TH1D*)U.Hreco(RooUnfold::kCovToy)->Clone(Form("Svd_%d",iCat)))  );
		}
	TH1D *r=(TH1D*)u[0].Clone("Svd");
	for(int iBin=0;iBin<r->GetNbinsX();iBin++)
	{
		r->SetBinContent(iBin,0);
		r->SetBinError(iBin,0);
	}
	//sums
	for (int iCat=0;iCat<histos.size();iCat++)
		for(int iBin=0;iBin<r->GetNbinsX();iBin++)
		{
			r->SetBinContent(iBin, r->GetBinContent(iBin) + u[iCat].GetBinContent(iBin)/pow(u[iCat].GetBinError(iBin),2) );
			r->SetBinError(iBin, r->GetBinError(iBin) + 1.0/pow(u[iCat].GetBinError(iBin),2) );

		}
	//
	for(int iBin=0;iBin<r->GetNbinsX();iBin++)
		{
		r->SetBinContent(iBin, r->GetBinContent(iBin)*r->GetBinError(iBin) );
		r->SetBinError(iBin,1./sqrt(r->GetBinError(iBin)));
		}

	return r;
}
