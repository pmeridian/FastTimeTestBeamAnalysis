#include "interface/H4treeReco.h"
#include "interface/WaveformUtils.hpp"
#include "interface/WaveformFit.hpp"

H4treeReco::H4treeReco(TTree *tree) : H4tree(tree)
{
  recoT_ = new TTree("H4treeReco","H4treeReco");
  recoT_->Branch("runNumber",    &runNumber,    "runNumber/i");
  recoT_->Branch("spillNumber",  &spillNumber,  "spillNumber/i");
  recoT_->Branch("evtNumber",    &evtNumber,    "evtNumber/i");
  recoT_->Branch("evtTimeDist",  &evtTimeDist,  "evtTimeDist/i");
  recoT_->Branch("evtTimeStart", &evtTimeStart, "evtTimeStart/i");
  recoT_->Branch("nEvtTimes",    &nEvtTimes,    "nEvtTimes/i");

  //add more variables relevant for the study

}

void H4treeReco::Loop()
{
  if (fChain == 0) return;
  
  Long64_t nentries = fChain->GetEntriesFast();
  for (Long64_t jentry=0; jentry<nentries;jentry++) {
    Long64_t ientry = LoadTree(jentry);
    if (ientry < 0) break;
    fChain->GetEntry(jentry); 

    //save x/y coordinates from the wire chambers
    //https://github.com/cmsromadaq/H4DQM/blob/master/src/plotterTools.cpp#L1296

    //loop over the relevant channels and reconstruct the waveforms
    //https://github.com/cmsromadaq/H4DQM/blob/master/src/plotterTools.cpp#L1785
    //store the relevant information: pedestal, amplitude, time, etc.
    //optional:
    //save pulse, pedestal subtracted and aligned using trigger time?

    recoT_->Write();
  }
}


H4treeReco::~H4treeReco()
{
}
