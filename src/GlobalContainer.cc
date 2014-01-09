#include <iostream>
#include "interface/GlobalContainer.h"

using namespace std;

//Destructor
GlobalContainer::~GlobalContainer(){
		for(map<string,float*>::iterator iContainer=v_float_container.begin();iContainer!=v_float_container.end();iContainer++)
			delete iContainer->second;
		v_float_container.clear();
		for(map<string,int*>::iterator iContainer=v_int_container.begin();iContainer!=v_int_container.end();iContainer++)
			delete iContainer->second;
		v_int_container.clear();
		for(map<string,double*>::iterator iContainer=v_double_container.begin();iContainer!=v_double_container.end();iContainer++)
			delete iContainer->second;
		v_double_container.clear();
		for(map<string,TLorentzVector*>::iterator iContainer=v_p4_container.begin();iContainer!=v_p4_container.end();iContainer++)
			delete iContainer->second;
		v_p4_container.clear();
		for(map<string,long*>::iterator iContainer=v_long_container.begin();iContainer!=v_long_container.end();iContainer++)
			delete iContainer->second;
		v_long_container.clear();
		};

int GlobalContainer::WriteHistos()
{
	for(map<string,TH1D>::iterator iHisto=histo_container.begin();iHisto!=histo_container.end();iHisto++)
		iHisto->second.Write();	
	for(map<string,TH2D>::iterator iHisto=histo2_container.begin();iHisto!=histo2_container.end();iHisto++)
		iHisto->second.Write();	
}

// ----------------------------------------- SET ACTUL VALUE ----------------------------------------------

template<>
int GlobalContainer::SetActualValue<float>(string name,float value)
{
	float_container[name]=value;
	return 0;
}
template<>
int GlobalContainer::SetActualValue<int>(string name,int value)
{
	int_container[name]=value;
	return 0;
}
template<>
int GlobalContainer::SetActualValue<long>(string name,long value)
{
	long_container[name]=value;
	return 0;
}
template<>
int GlobalContainer::SetActualValue<double>(string name,double value)
{
	double_container[name]=value;
	return 0;
}
template<>
int GlobalContainer::SetActualValue<TLorentzVector>(string name,TLorentzVector value)
{
	p4_container[name]=value;
	return 0;
}
template<>
int GlobalContainer::SetActualValue<float*>(string name,float* value)
{
	v_float_container[name]=value;
	return 0;
}
template<>
int GlobalContainer::SetActualValue<int*>(string name,int* value)
{
	v_int_container[name]=value;
	return 0;
}
template<>
int GlobalContainer::SetActualValue<long*>(string name,long* value)
{
	v_long_container[name]=value;
	return 0;
}
template<>
int GlobalContainer::SetActualValue<double*>(string name,double* value)
{
	v_double_container[name]=value;
	return 0;
}
template<>
int GlobalContainer::SetActualValue<TLorentzVector*>(string name,TLorentzVector* value)
{
	v_p4_container[name]=value;
	return 0;
}
template<>
int GlobalContainer::SetActualValue<TH1D>(string name,TH1D value)
{
	histo_container[name]=value;
	return 0;
}
template<>
int GlobalContainer::SetActualValue<TH2D>(string name,TH2D value)
{
	histo2_container[name]=value;
	return 0;
}
template<>
int GlobalContainer::SetActualValue<TH2D*>(string name,TH2D *value)
{
	//histo2_container[name]=value;
	return 1;
}
template<>
int GlobalContainer::SetActualValue<TH1D*>(string name,TH1D *value)
{
	//histo2_container[name]=value;
	return 1;
}
template<>
int GlobalContainer::SetActualValue<int**>(string name,int** value)
{
	//v_int_container[name]=value;
	return 1;
}
template<>
int GlobalContainer::SetActualValue<TLorentzVector**>(string name,TLorentzVector** value)
{
	//v_int_container[name]=value;
	return 1;
}

//-------------------------- GET ENTRY PTR ---------------------------------
template <>
float* GlobalContainer::GetEntryPtr<float>(string name)
{
	if(float_container.find(name) != float_container.end()) return &(float_container.find(name)->second);
return NULL;
}
template <>
int* GlobalContainer::GetEntryPtr<int>(string name)
{
	if(int_container.find(name) != int_container.end()) return &(int_container.find(name)->second);
return NULL;
}
template <>
long* GlobalContainer::GetEntryPtr<long>(string name)
{
	if(long_container.find(name) != long_container.end()) return &(long_container.find(name)->second);
return NULL;
}
template <>
double* GlobalContainer::GetEntryPtr<double>(string name)
{
	if(double_container.find(name) != double_container.end()) return &(double_container.find(name)->second);
return NULL;
}
template <>
TLorentzVector* GlobalContainer::GetEntryPtr<TLorentzVector>(string name)
{
	if(p4_container.find(name) != p4_container.end()) return &(p4_container.find(name)->second);
return NULL;
}
//--- vector type
template <>
float** GlobalContainer::GetEntryPtr<float*>(string name)
{
	if(v_float_container.find(name) != v_float_container.end()) return &(v_float_container.find(name)->second);
return NULL;
}
template <>
int** GlobalContainer::GetEntryPtr<int*>(string name)
{
	if(v_int_container.find(name) != v_int_container.end()) return &(v_int_container.find(name)->second);
return NULL;
}
template <>
long** GlobalContainer::GetEntryPtr<long*>(string name)
{
	if(v_long_container.find(name) != v_long_container.end()) return &(v_long_container.find(name)->second);
return NULL;
}
template <>
double** GlobalContainer::GetEntryPtr<double*>(string name)
{
	if(v_double_container.find(name) != v_double_container.end()) return &(v_double_container.find(name)->second);
return NULL;
}
template <>
TLorentzVector** GlobalContainer::GetEntryPtr<TLorentzVector*>(string name)
{
	if(v_p4_container.find(name) != v_p4_container.end()) return &(v_p4_container.find(name)->second);
return NULL;
}
template <>
TH1D* GlobalContainer::GetEntryPtr<TH1D>(string name)
{
	if(histo_container.find(name) != histo_container.end()) return &(histo_container.find(name)->second);
return NULL;
}
template <>
TH2D* GlobalContainer::GetEntryPtr<TH2D>(string name)
{
	if(histo2_container.find(name) != histo2_container.end()) return &(histo2_container.find(name)->second);
return NULL;
}
