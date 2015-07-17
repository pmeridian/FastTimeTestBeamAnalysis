#include "interface/H4treeReco.h"
#include "interface/WaveformUtils.hpp"
#include "interface/WaveformFit.hpp"

#include <iostream>

//
H4treeReco::H4treeReco(TChain *tree,TString outUrl) : 
  H4tree(tree),
  wcXl_(4),   //TDC Xleft
  wcXr_(5),   //TDC Xright
  wcYd_(6),   //TDC Ydown
  wcYu_(7),   //TDC Yup
  MaxTdcChannels_(16),
  MaxTdcReadings_(20)
{
  fOut_  = TFile::Open(outUrl,"RECREATE");
  recoT_ = new TTree("H4treeReco","H4treeReco");
  recoT_->SetDirectory(fOut_);

  //event header
  recoT_->Branch("runNumber",    &runNumber,    "runNumber/i");
  recoT_->Branch("spillNumber",  &spillNumber,  "spillNumber/i");
  recoT_->Branch("evtNumber",    &evtNumber,    "evtNumber/i");
  recoT_->Branch("evtTimeDist",  &evtTimeDist,  "evtTimeDist/i");
  recoT_->Branch("evtTimeStart", &evtTimeStart, "evtTimeStart/i");
  recoT_->Branch("nEvtTimes",    &nEvtTimes,    "nEvtTimes/i");

  //TDC 
  tdc_readings_.resize(MaxTdcChannels_);
  recoT_->Branch("tdc_recox", &tdc_recox_, "tdc_recox/F");
  recoT_->Branch("tdc_recoy", &tdc_recoy_, "tdc_recoy/F");
  
  //add more variables relevant for the study

}

//
void H4treeReco::Loop()
{
  if (fChain == 0) return;
  
  Long64_t nentries = fChain->GetEntries();
  for (Long64_t jentry=0; jentry<nentries;jentry++) 
    {
      
      //progress bar
      if(jentry%10==0) 
	{
	  printf("\r[H4treeReco] status [ %3d/100 ]",int(100*float(jentry)/float(nentries)));
	  std::cout << std::flush;
	}
      
      //readout the event
      fChain->GetEntry(jentry); 

      //save x/y coordinates from the wire chambers
      FillTDC();      
      
      //loop over the relevant channels and reconstruct the waveforms
      //https://github.com/cmsromadaq/H4DQM/blob/master/src/plotterTools.cpp#L1785
      //store the relevant information: pedestal, amplitude, time, etc.
      //optional:
      //save pulse, pedestal subtracted and aligned using trigger time?
      
      recoT_->Fill();
    }
}

//
void H4treeReco::FillTDC()
{
  tdc_recox_=-999;
  tdc_recoy_=-999;

  for (uint j=0; j<MaxTdcChannels_; j++){
    tdc_readings_[j].clear();
  }
  
  for (uint i=0; i<nTdcChannels; i++){
    if (tdcChannel[i]<MaxTdcChannels_){
      tdc_readings_[tdcChannel[i]].push_back((float)tdcData[i]);
    }
  }
  
  if (tdc_readings_[wcXl_].size()!=0 && tdc_readings_[wcXr_].size()!=0){
    float TXl = *std::min_element(tdc_readings_[wcXl_].begin(),tdc_readings_[wcXl_].begin()+tdc_readings_[wcXl_].size());
    float TXr = *std::min_element(tdc_readings_[wcXr_].begin(),tdc_readings_[wcXr_].begin()+tdc_readings_[wcXr_].size());
    tdc_recox_ = (TXr-TXl)*0.005; // = /40./5./10. //position in cm 0.2mm/ns with 25ps LSB TDC
  }
  if (tdc_readings_[wcYd_].size()!=0 && tdc_readings_[wcYu_].size()!=0){
    float TYd = *std::min_element(tdc_readings_[wcYd_].begin(),tdc_readings_[wcYd_].begin()+tdc_readings_[wcYd_].size());
    float TYu = *std::min_element(tdc_readings_[wcYu_].begin(),tdc_readings_[wcYu_].begin()+tdc_readings_[wcYu_].size());
    tdc_recoy_ = (TYu-TYd)*0.005; // = /40./5./10. //position in cm 0.2mm/ns with 25ps LSB TDC
  }
}


//
H4treeReco::~H4treeReco()
{
  fOut_->cd();
  recoT_->Write();
  fOut_->Close();
}
