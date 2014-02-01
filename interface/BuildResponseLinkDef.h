#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ nestedclass;
#pragma link C++ nestedtypedef;

#pragma link C++ class Bins+ ;
#pragma link C++ class RecoInfo+ ;
#pragma link C++ class std::pair<std::string,std::string>+;
#pragma link C++ class std::map<std::string,std::string>+;
#pragma link C++ class std::pair<std::string,Bins>+;
#pragma link C++ class std::map<std::string,Bins>+;
#pragma link C++ class std::map<unsigned long long, RecoInfo>+;
#pragma link C++ class std::map<ULong64_t, RecoInfo>+;
#pragma link C++ class BuildResponse+ ;

#endif
