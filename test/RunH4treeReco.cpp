#include "interface/H4treeReco.h"

#include<iostream>

#include "TString.h"
#include "TFile.h"

using namespace std;

int main()
{
  //open input file
  TString url="root://eoscms//eos/cms/store/group/dpg_ecal/alca_ecalcalib/TimingTB_H2_Jul2015/raw/DataTree/3351/1.root";
  TFile *inF=TFile::Open(url);
  TTree *t=(TTree *)inF->Get("H4tree");
  cout << "Retrieved tree from " << inF << " with " << t->GetEntriesFast() << " events" << endl;
  
  //run reconstruction
  H4treeReco reco(t);
  reco.Loop();

  return 0;
}
