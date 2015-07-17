#ifndef H4treeReco_h
#define H4treeReco_h

#include "interface/H4tree.h"
#include "interface/VarPlot.h"

#include "TFile.h"
#include "TString.h"

#include <set>

class H4treeReco : public H4tree
{

 public:
  H4treeReco(TChain *,TString outUrl="H4treeRecoOut.root");
  void Loop(); 
  void FillTDC();
  void FillWaveforms();
  ~H4treeReco();
  
 private:

  void InitDigi();

  std::set< std::pair<Int_t,Int_t> > groupsAndChannels_;
  std::map<TString,VarPlot*> varplots_;

  //TDC readings
  UInt_t wcXl_, wcXr_, wcYd_, wcYu_;
  UInt_t MaxTdcChannels_,MaxTdcReadings_;
  std::vector< std::vector<Float_t> > tdc_readings_;
  float tdc_recox_, tdc_recoy_;

  UInt_t nActiveDigitizerChannels_;

  TTree *recoT_;
  TFile *fOut_;
};

#endif
