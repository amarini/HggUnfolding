
#ifndef GlobalContainer_h
#define GlobalContainer_h
#include <map>
#include <vector>
#include "TLorentzVector.h"
#include "TH1D.h"
#include "TH2D.h"

using namespace std;


class GlobalContainer{
public:
	GlobalContainer();
	~GlobalContainer();
	template <class T>
	int SetEntry(string name,T value,string type="float"); //interfaced to Set Actual value
	template <class T>
	T GetEntry(string name); // interfaced to GetEntryPtr
	template <class T> // template specification
	T* GetEntryPtr(string name);

	int WriteHistos();

private:
	template <class T> //SetEntry is interfaced to this macro
	int SetActualValue(string name,T value); //template specification
	map<string,float> 		float_container;
	map<string,double> 		double_container;
	map<string,int>   		int_container;
	map<string,long>   		long_container;
	map<string,TLorentzVector>   	p4_container;
	map<string,TH1D>   	histo_container;
	map<string,TH2D>   	histo2_container;
	map<string, float* > 	v_float_container;
	map<string, int * > 	v_int_container;
	map<string, long * > 	v_long_container;
	map<string, double * > 	v_double_container;
	map<string, TLorentzVector* > v_p4_container;

};

// ----------------------------- TEMPLATE CODE --------------------------

template <class T>
int GlobalContainer::SetEntry(string name,T value,string type)
{
	bool exists_float=false,exists_double=false,exists_int=false,exists_long=false,exists_p4=false;
	bool exists_v_float=false,exists_v_double=false,exists_v_int=false,exists_v_p4=false,exists_v_long=false;
	bool exists_TH1D=false,exists_TH2D=false;

	if(float_container.find(name) != float_container.end()) exists_float=true;
	if(int_container.find(name) != int_container.end()) exists_int=true;
	if(long_container.find(name) != long_container.end()) exists_long=true;
	if(double_container.find(name) != double_container.end()) exists_double=true;
	if(p4_container.find(name) != p4_container.end()) exists_p4=true;

	if(v_float_container.find(name) != v_float_container.end()) exists_v_float=true;
	if(v_int_container.find(name) != v_int_container.end()) exists_v_int=true;
	if(v_long_container.find(name) != v_long_container.end()) exists_v_long=true;
	if(v_double_container.find(name) != v_double_container.end()) exists_v_double=true;
	if(v_p4_container.find(name) != v_p4_container.end()) exists_v_p4=true;

	bool exists=exists_float || exists_double || exists_int || exists_p4 || exists_long
		   || exists_v_float || exists_v_double || exists_v_int || exists_v_p4 || exists_v_long
		   || exists_TH1D || exists_TH2D;

	if (type=="float" && ( (!exists) || exists_float) ) return SetActualValue(name,value); 
	if (type=="double" && ( (!exists) || exists_double ) ) return SetActualValue(name,value);
	if (type=="int" && ( (!exists) || exists_int ) ) return SetActualValue(name,value);
	if (type=="long" && ( (!exists) || exists_long ) ) return SetActualValue(name,value);
	if ( (type=="TLorentzVector"|| type=="p4") && ( (!exists) || exists_p4) ) return SetActualValue(name,value);
	if (type=="TH1D" && ( (!exists) || exists_TH1D ) ) return SetActualValue(name,value);
	if (type=="TH2D" && ( (!exists) || exists_TH2D ) ) return SetActualValue(name,value);
	if (type=="v_float" && ( (!exists) || exists_v_float) ) return SetActualValue(name,value);
	if (type=="v_double" && ( (!exists) || exists_v_double ) ) return SetActualValue(name,value);
	if (type=="v_int" && ( (!exists) || exists_v_int )) return SetActualValue(name,value);
	if (type=="v_long" && ( (!exists) || exists_v_long )) return SetActualValue(name,value);
	if ( (type=="v_TLorentzVector"|| type=="v_p4") && ( (!exists) || exists_v_p4) ) return SetActualValue(name,value);

	//either exists of the wrong type or type does not exists	
	if( exists ) cout<< "value "<< name <<" already exists ";
	if( exists_float) cout<< "with type float"<<endl;
	if( exists_int) cout<< "with type int"<<endl;
	if( exists_long) cout<< "with type long"<<endl;
	if( exists_double) cout<< "with type double"<<endl;
	if( exists_p4) cout<< "with type v_p4"<<endl;
	if( exists_v_float) cout<< "with type v_float"<<endl;
	if( exists_v_int) cout<< "with type v_int"<<endl;
	if( exists_v_long) cout<< "with type v_long"<<endl;
	if( exists_v_double) cout<< "with type v_double"<<endl;
	if( exists_v_p4) cout<< "with type v_p4"<<endl;
	if( exists_TH1D) cout<< "with type TH1D"<<endl;
	if( exists_TH2D) cout<< "with type TH2D"<<endl;
	if( !exists )  cout<<" Type '"<<type<< "' is not correct: allowed only: int,long,double,p4,float or v_* or TH1D,TH2D"<<endl;
return 1;
}

// ----------------------------------------- GET ENTRY ----------------------------------------------
template <class T>
T GlobalContainer::GetEntry(string name){
	T *Ptr = GetEntryPtr<T>(name);
	if ( Ptr !=NULL ) return *Ptr ;
}

#endif
