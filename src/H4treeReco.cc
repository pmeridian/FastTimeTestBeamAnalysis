#include "interface/H4treeReco.h"
#include "interface/WaveformUtils.hpp"
#include "interface/WaveformFit.hpp"
#include "interface/Waveform.hpp"

#include <iostream>

//
H4treeReco::H4treeReco(TChain *tree,JSONWrapper::Object *cfg,TString outUrl) : 
  H4tree(tree),
  cfg_(cfg),
  wcXl_(4),   //TDC Xleft
  wcXr_(5),   //TDC Xright
  wcYd_(6),   //TDC Ydown
  wcYu_(7),   //TDC Yup
  MaxTdcChannels_(16),
  MaxTdcReadings_(20),
  nActiveDigitizerChannels_(8)
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
  
  //digitizer channel info
  recoT_->Branch("maxch",               &maxch_,               "maxch/i");
  recoT_->Branch("group",                group_,               "group[maxch]/F");
  recoT_->Branch("ch",                   ch_,                  "ch[maxch]/F");
  recoT_->Branch("pedestal",             pedestal_,            "pedestal[maxch]/F");
  recoT_->Branch("pedestalRMS",          pedestalRMS_,         "pedestalRMS[maxch]/F");
  recoT_->Branch("wave_max",             wave_max_,            "wave_max[maxch]/F");
  recoT_->Branch("waveform_aroundmax",   waveform_aroundmax_,  "waveform_aroundmax[maxch][25]/F");
  recoT_->Branch("charge_integ",         charge_integ_,        "charge_integ[maxch]/F");
  recoT_->Branch("charge_integ_max",     charge_integ_max_,    "charge_integ_max[maxch]/F");
  recoT_->Branch("t_max",                t_max_,     	       "t_max[maxch]/F");
  recoT_->Branch("t_max_frac30",         t_max_frac30_,        "t_max_frac30[maxch]/F");
  recoT_->Branch("t_max_frac50",         t_max_frac50_,        "t_max_frac50[maxch]/F");

  InitDigi();
}

//
void H4treeReco::InitDigi()
{
  //init channels of interest
  std::vector<JSONWrapper::Object> digis=(*cfg_)["digis"].daughters();
  for(size_t i=0; i<digis.size(); i++)
    {
      //add a ChannelPlot class for the reconstruction
      ChannelReco *chRec = new ChannelReco(digis[i],ChannelReco::kNull, false,false);
      GroupChannelKey_t key(chRec->GetGroup(),chRec->GetModule());
      chPlots_[key] = chRec;
      chPlots_[key]->SetWaveform( new Waveform() );
    }
}

//
void H4treeReco::FillWaveforms()
{
  //first reset waveforms
  int ictr(0);
  for (std::map<GroupChannelKey_t,ChannelReco*>::iterator it=chPlots_.begin();it!=chPlots_.end();++it,ictr++)
    {
      it->second->ClearVectors();
      it->second->GetWaveform()->clear();
      for(int k=0; k<25; k++) waveform_aroundmax_[ictr][k]=0;
    }

  //fill waveforms
  for (uint iSample = 0 ; iSample < nDigiSamples ; ++iSample)
    {
      GroupChannelKey_t key(digiGroup[iSample],digiChannel[iSample]);
      if(chPlots_.find(key)==chPlots_.end()) continue;
      if(digiChannel[iSample]>=nActiveDigitizerChannels_) continue;
      chPlots_[key]->GetWaveform()->addTimeAndSample(digiSampleIndex[iSample]*timeSampleUnit(digiFrequency[iSample]),
						     digiSampleValue[iSample]);
    }

  //reconstruct waveforms
  Waveform * waveform;
  maxch_=0;
  for (std::map<GroupChannelKey_t,ChannelReco*>::iterator it=chPlots_.begin();it!=chPlots_.end();++it,++maxch_)
    {
      // Extract waveform information:
      ChannelReco *chRec=it->second;
      waveform = chRec->GetWaveform() ;
       
      //use 40 samples between 5-44 to get pedestal and RMS
      Waveform::baseline_informations wave_pedestal= waveform->baseline(chRec->GetPedestalWindowLo(),
									chRec->GetPedestalWindowUp()); 

      //substract the pedestal from the samples
      waveform->offset(wave_pedestal.pedestal);
      
      //if pedestal is very high, the signal is negative -> invert it
      if(wave_pedestal.pedestal>chRec->GetThrForPulseInversion()) waveform->rescale(-1);	
      
      //find max amplitude in search window
      Waveform::max_amplitude_informations wave_max=waveform->max_amplitude(chRec->GetSearchWindowLo(),
									    chRec->GetSearchWindowUp(),
									    5); 

      //fill information for the reco tree
      group_[maxch_]              = it->first.first;
      ch_[maxch_]                 = it->first.second;
      pedestal_[maxch_]           = wave_pedestal.pedestal;
      pedestalRMS_[maxch_]        = wave_pedestal.rms;
      wave_max_[maxch_]           = wave_max.max_amplitude;
      t_max_[maxch_]              = wave_max.time_at_max*1.e9;
      for(int i=chRec->GetSpyWindowLo(); i<=chRec->GetSpyWindowUp(); i++)
	{
	  int idx2store=i-chRec->GetSpyWindowLo();
	  int idx=wave_max.sample_at_max+i;
	  float val(0);
	  if(idx>=0 && idx<waveform->_samples.size()) val=waveform->_samples[idx];
	  waveform_aroundmax_[maxch_][idx2store]=val;
	}

      charge_integ_[maxch_]       = waveform->charge_integrated(0,900);
      charge_integ_max_[maxch_]   = waveform->charge_integrated(wave_max.time_at_max-chRec->GetCFDWindowLo(),
								wave_max.time_at_max);

      t_max_frac30_[maxch_]       = waveform->time_at_frac(wave_max.time_at_max-chRec->GetCFDWindowLo(),
							   wave_max.time_at_max,0.3,wave_max,7)*1.e9;
      t_max_frac50_[maxch_]       = waveform->time_at_frac(wave_max.time_at_max-chRec->GetCFDWindowLo(),
							   wave_max.time_at_max,0.5,wave_max,7)*1.e9;

    }
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
      FillWaveforms();
	
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


H4treeReco::~H4treeReco()
{
  fOut_->cd();
  recoT_->Write();
  for(std::map<GroupChannelKey_t,ChannelReco*>::iterator it=chPlots_.begin();
      it!=chPlots_.end();
      it++)
    {
      TH1F *h=it->second->GetConfigSummary();
      h->SetDirectory(fOut_);
      h->Write();
    }
  fOut_->Close();
}
