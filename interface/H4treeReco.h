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
  void FillTDC();
  ~H4treeReco();
  
 private:

  //TDC readings
  UInt_t wcXl_, wcXr_, wcYd_, wcYu_;
  UInt_t MaxTdcChannels_,MaxTdcReadings_;
  std::vector< std::vector<Float_t> > tdc_readings_;
  float tdc_recox_, tdc_recoy_;

  TTree *recoT_;
  TFile *fOut_;
};

#endif
