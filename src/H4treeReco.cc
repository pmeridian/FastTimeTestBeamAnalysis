#include "interface/H4treeReco.h"
#include "interface/WaveformUtils.hpp"
#include "interface/WaveformFit.hpp"
#include "interface/Waveform.hpp"

#include <iostream>

//
H4treeReco::H4treeReco(TChain *tree,JSONWrapper::Object *cfg,TString outUrl) : 
  H4tree(tree),
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
  recoT_->Branch("charge_integ",         charge_integ_,        "charge_integ[maxch]/F");
  recoT_->Branch("charge_integ_max",     charge_integ_max_,    "charge_integ_max[maxch]/F");
  recoT_->Branch("t_max",                t_max_,     	       "t_max[maxch]/F");
  recoT_->Branch("t_max_frac30",         t_max_frac30_,        "t_max_frac30[maxch]/F");
  recoT_->Branch("t_max_frac50",         t_max_frac50_,        "t_max_frac50[maxch]/F");

  InitDigi(cfg);
}

//
void H4treeReco::InitDigi(JSONWrapper::Object *cfg)
{
  //init channels of interest
  std::vector<JSONWrapper::Object> digis=(*cfg)["digis"].daughters();
  std::cout << "[H4treeReco::InitDigi] preparing analysis for " << digis.size() << " channels" << std::endl;
  groupNamesH_   = new TH1F("Groups",";;Group;"  ,digis.size(),0,digis.size());
  channelNamesH_ = new TH1F("Channels",";;Channel;",digis.size(),0,digis.size());
  for(size_t i=0; i<digis.size(); i++)
    {
      TString iname = digis[i]["name"].toString();
      UInt_t igr    = digis[i]["group"].toInt();
      UInt_t ich    = digis[i]["channel"].toInt();
      std::cout << "\t " << iname << " will be reconstructed from group=" << igr << " channel=" << ich << std::endl;

      //add a ChannelPlot class for the reconstruction
      GroupChannelKey_t key(igr,ich);
      chPlots_[key] = new ChannelPlot(igr,ich,ChannelPlot::kNull, false,false);
      chPlots_[key]->SetWaveform( new Waveform() );
      chPlots_[key]->SetName(iname);
      
      groupNamesH_->SetBinContent(i+1,igr);
      groupNamesH_->GetXaxis()->SetBinLabel(i+1,iname);
      channelNamesH_->SetBinContent(i+1,ich);
      channelNamesH_->GetXaxis()->SetBinLabel(i+1,iname);
    }
}

//
void H4treeReco::FillWaveforms()
{
	
  //fill waveforms
  for (uint iSample = 0 ; iSample < nDigiSamples ; ++iSample)
    {
      GroupChannelKey_t key(digiGroup[iSample],digiChannel[iSample]);
      if(chPlots_.find(key)==chPlots_.end()) continue;
      if(digiChannel[iSample]>=nActiveDigitizerChannels_) continue;
      chPlots_[key]->GetWaveform()->addTimeAndSample(digiSampleIndex[iSample]*0.2,digiSampleValue[iSample]);
    }

  //reconstruct waveforms
  Waveform * waveform;
  maxch_=0;
  for (std::map<GroupChannelKey_t,ChannelPlot*>::iterator it=chPlots_.begin();it!=chPlots_.end();++it,++maxch_)
    {
      // Extract waveform information:
      waveform = it->second->GetWaveform() ;
       
      //use 40 samples between 5-44 to get pedestal and RMS
      Waveform::baseline_informations wave_pedestal= waveform->baseline(5,44); 
      
      //substract the pedestal from the samples
      waveform->offset(wave_pedestal.pedestal);
      
      //if pedestal is very high, the signal is negative -> invert it
      if(wave_pedestal.pedestal>2100) waveform->rescale(-1);	
      
      //find max amplitude between 50 and 900 samples
      Waveform::max_amplitude_informations wave_max=waveform->max_amplitude(50,900,5); 
 
      //fill information for the reco tree
      group_[maxch_]              = it->first.first;
      ch_[maxch_]                 = it->first.second;
      pedestal_[maxch_]           = wave_pedestal.pedestal;
      pedestalRMS_[maxch_]        = wave_pedestal.rms;
      wave_max_[maxch_]           = wave_max.max_amplitude;
      charge_integ_[maxch_]       = waveform->charge_integrated(0,900);
      charge_integ_max_[maxch_]   = waveform->charge_integrated(wave_max.time_at_max-1.3e-8,wave_max.time_at_max);
      t_max_[maxch_]              = wave_max.time_at_max*1.e9;
      t_max_frac30_[maxch_]       = waveform->time_at_frac(wave_max.time_at_max-1.3e-8,wave_max.time_at_max,0.3,wave_max,7)*1.e9;
      t_max_frac50_[maxch_]       = waveform->time_at_frac(wave_max.time_at_max-1.3e-8,wave_max.time_at_max,0.5,wave_max,7)*1.e9;
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
      //https://github.com/cmsromadaq/H4DQM/blob/master/src/plotterTools.cpp#L1785
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
  groupNamesH_->Write();
  channelNamesH_->Write();
  fOut_->Close();
}
