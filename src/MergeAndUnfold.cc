
#ifndef MaU_H
#define MaU_H
#include "TH1D.h"
#include "TH2D.h"
#include "TH1F.h"
#include <string>
using namespace std;


class MergeAndUnfold{
public:

private:	
	vector<TH1D> histos;

	vector<TH2D> response;
	vector<TH1D> gen;
	vector<TH1D> reco;

};

#endif
