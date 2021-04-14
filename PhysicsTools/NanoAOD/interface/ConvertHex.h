#include <iostream>
#include <string>
#include "TString.h"

//long int StringToInt(std::string t);
int StringToInt(std::string t);
std::vector<int> StringToIntMaxSize(std::string s);
std::vector<int> VectorStringToInt(std::vector<std::string> svector);

int StringToInt(std::string s){
  TString t = TString(s);
  TString totalHex = "";
  for(int i = 0; i < t.Sizeof()-1; i++){
    int n = (int)t[i];
    std::stringstream ss;
    ss << std::hex << n;
    TString h = TString(ss.str());
    if(h.Sizeof() < 3) h = TString("0")+h;
    totalHex += h;
  }
  long int x;
  std::stringstream ss2;
  ss2 << std::hex << totalHex;
  ss2 >> x;
  return x;
}

std::vector<int> StringToIntMaxSize(std::string s){
  TString t = TString(s);
  std::vector<int> ints;
  if(t.Length() <= 4) {
    ints.push_back(StringToInt(t.Data()));
    return ints;
  }
  else{
    TString t0 = TString(t(0,4));
    t = TString(t(4, t.Length()));
    ints.push_back(StringToInt(t0.Data()));
    while(t.Length() != 0){
      TString t0 = TString('-')+TString(t(0,3));
      ints.push_back(StringToInt(t0.Data()));
      t = TString(t(3, t.Length()));
    }
  }
  return ints;
}

std::vector<int> VectorStringToInt(std::vector<std::string> svector){
  std::vector<int> ints, tempv; 
  std::vector<int> tempc; 
  for (auto & s : svector) {
    tempv = StringToIntMaxSize(s);
    ints.insert(ints.end(), tempv.begin(), tempv.end());
  }
  return ints;
}

/*
 
TString HexToChar(TString hex);
TString IntToString(long int x);

TString HexToChar(TString hex){
  if(hex.Sizeof() > 3){
    TString final = "";
    for(int i = 0; i < hex.Sizeof()-1; i+=2){
      final += HexToChar(TString(hex(i,2)));
    }
    return final;
  }
  else if(hex.Sizeof() == 2){
    TString s2 = TString("0")+hex;
    return HexToChar(s2);
  }
  else if(hex.Sizeof() <= 1){
    return TString("");
  }
  std::stringstream ss;
  ss << std::hex << hex;
  int x;
  ss >> x;
  return TString((char)x);
}

TString IntToString(long int x){
  std::stringstream ss;
  ss << std::hex << x;
  TString hexString = TString(ss.str());
  return HexToChar(hexString);
}


void ConvertHex(){
  TString a = TString("Vamos a ver si codificamos esta frase");
  for(int i = 0; i < StringToIntMaxSize(a).size(); i++){
    std::cout << StringToIntMaxSize(a)[i] << std::endl;
  }
}
*/
