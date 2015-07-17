#ifndef H4treeReco_h
#define H4treeReco_h

#include "interface/H4tree.h"

#include "TFile.h"
#include "TString.h"

class H4treeReco : public H4tree
{

 public:
  H4treeReco(TChain *,TString outUrl="H4treeRecoOut.root");
  void Loop(); 
  ~H4treeReco();
  
 private:
  
  TTree *recoT_;
  TFile *fOut_;
};

#endif
