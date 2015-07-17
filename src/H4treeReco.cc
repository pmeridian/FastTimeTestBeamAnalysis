#include "interface/H4treeReco.h"
#include "interface/WaveformUtils.hpp"
#include "interface/WaveformFit.hpp"
#include "interface/Waveform.hpp"

#include <iostream>

//
H4treeReco::H4treeReco(TChain *tree,TString outUrl) : 
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
  
  //add more variables relevant for the study
}

//
void H4treeReco::InitDigi()
{
  //init channels of interest
  groupsAndChannels_.insert( std::pair<Int_t,Int_t>(0,0) ); //MPC
  groupsAndChannels_.insert( std::pair<Int_t,Int_t>(0,3) ); //Si Pad #1
  groupsAndChannels_.insert( std::pair<Int_t,Int_t>(0,4) ); //Si Pad #2
  groupsAndChannels_.insert( std::pair<Int_t,Int_t>(1,8) ); //Trigger

  for(std::set< std::pair<Int_t,Int_t> >::iterator key=groupsAndChannels_.begin();
      key!=groupsAndChannels_.end();
      ++key)
    {
      Int_t iGroup(key->first),iChannel(key->second);
      if(iChannel>=nActiveDigitizerChannels_) continue;
	  
      char name[100];
      sprintf(name,"digi_ch%02d",iGroup*8+iChannel);
      varplots_[name]->waveform = new Waveform();
    }
}

//
void H4treeReco::FillWaveforms()
{
  /*
  //Add reconstruction of waveforms
  for (std::map<TString,varPlot<float>*>::iterator it=varplots.begin();it!=varplots.end();++it)
    {
      if (!(it->second->waveform)) continue;
      
      Waveform::baseline_informations wave_pedestal;
      Waveform::max_amplitude_informations wave_max;
      
      wave_pedestal=it->second->waveform->baseline(5,44); //use 40 samples between 5-44 to get pedestal and RMS
      it->second->waveform->offset(wave_pedestal.pedestal);
      
      it->second->waveform->rescale(-1); 
      wave_max=it->second->waveform->max_amplitude(50,900,5); //find max amplitude between 50 and 900 samples
      if (wave_max.max_amplitude<20)
	{
	  //try not inverting if signal is positive
	  it->second->waveform->rescale(-1); 
	  Waveform::max_amplitude_informations wave_max_inv=it->second->waveform->max_amplitude(50,900,5); //find max amplitude between 50 and 900 samples
	  if (wave_max_inv.max_amplitude>wave_max.max_amplitude)
	    wave_max=wave_max_inv;
	  else //stay with negative signals
	    it->second->waveform->rescale(-1); 
	}
      
      TString thisname = it->second->name.ReplaceAll("_pulse","");
      varplots[Form("%s_pedestal",thisname.Data())]->Fill(wave_pedestal.pedestal,1.);
      varplots[Form("%s_pedestal_rms",thisname.Data())]->Fill(wave_pedestal.rms,1.);
      varplots[Form("%s_max_amplitude",thisname.Data())]->Fill(wave_max.max_amplitude,1.);
      varplots[Form("%s_charge_integrated",thisname.Data())]->Fill(it->second->waveform->charge_integrated(0,900),1.); // pedestal already subtracted
      varplots[Form("%s_time_at_max",thisname.Data())]->Fill(wave_max.time_at_max*1.e9,1.);
      varplots[Form("%s_time_at_frac30",thisname.Data())]->Fill(it->second->waveform->time_at_frac(wave_max.time_at_max-1.3e-8,wave_max.time_at_max,0.3,wave_max,7)*1.e9,1.);
      varplots[Form("%s_time_at_frac50",thisname.Data())]->Fill(it->second->waveform->time_at_frac(wave_max.time_at_max-1.3e-8,wave_max.time_at_max,0.5,wave_max,7)*1.e9,1.);
      
      int x1 = -1;
      for(int i=0;i<64;i++){
	if(fibersOn_[hodoX1][i]==1 && x1==-1) x1 = i;
	if(fibersOn_[hodoX1][i]==1 && x1!=-1) { x1 = -1; break; }
      }
      int y1 = -1;
      for(int i=0;i<64;i++){
	if(fibersOn_[hodoY1][i]==1 && y1==-1) y1 = i;
	if(fibersOn_[hodoY1][i]==1 && y1!=-1) { y1 = -1; break; }
      }
      int x2 = -1;
      for(int i=0;i<64;i++){
	if(fibersOn_[hodoX2][i]==1 && x2==-1) x2 = i;
	if(fibersOn_[hodoX2][i]==1 && x2!=-1) { x2 = -1; break; }
      }
      int y2 = -1;
      for(int i=0;i<64;i++){
	if(fibersOn_[hodoY2][i]==1 && y2==-1) y2 = i;
	if(fibersOn_[hodoY2][i]==1 && y2!=-1) { y2 = -1; break; }
      }
      
      varplots[Form("%s_charge_integrated_vs_hodoX1",thisname.Data())]->Fill(x1,it->second->waveform->charge_integrated(0,900)); // pedestal already subtracted
      varplots[Form("%s_charge_integrated_vs_hodoY1",thisname.Data())]->Fill(y1,it->second->waveform->charge_integrated(0,900)); // pedestal already subtracted
      varplots[Form("%s_charge_integrated_vs_hodoX2",thisname.Data())]->Fill(x2,it->second->waveform->charge_integrated(0,900)); // pedestal already subtracted
      varplots[Form("%s_charge_integrated_vs_hodoY2",thisname.Data())]->Fill(y2,it->second->waveform->charge_integrated(0,900)); // pedestal already subtracted
      
      
      int x = getDigiChannelX(it->second->name);
      int y = getDigiChannelY(it->second->name);
      varplots["allCh_charge_integrated_map"]->Fill2D(x,y,it->second->waveform->charge_integrated(0,900)); // pedestal already subtracted	
      varplots["allCh_max_amplitude_map"]->Fill2D(x,y,wave_max.max_amplitude); // pedestal already subtracted
      varplots["allCh_pedestal_map"]->Fill2D(x,y,wave_pedestal.pedestal); // pedestal already subtracted
      varplots["allCh_pedestal_rms_map"]->Fill2D(x,y,wave_pedestal.rms); // pedestal already subtracted
      sum_amplitudes+=wave_max.max_amplitude;
    }
  */
}


//
void H4treeReco::Loop()
{
  Waveform *waveform;

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
      
      // Construct the waveform
	for(int iSample=0; iSample< nDigiSamples; ++iSample){
	      	//waveform->addTimeAndSample(fChain->digiSampleIndex[iSample]*timeSampleUnit(fChain->digiFrequency[iSample]),fChain->digiSampleValue[iSample]);
	      	waveform->addTimeAndSample(digiSampleIndex[iSample]*0.2,digiSampleValue[iSample]);
	}
	// Extract waveform information:
       	Waveform::baseline_informations wave_pedestal;
       	Waveform::max_amplitude_informations wave_max;
       
	wave_pedestal= waveform->baseline(5,44); //use 40 samples between 5-44 to get pedestal and RMS


	//substract a fixed value from the samples
	waveform->offset(wave_pedestal.pedestal);

	//rescale all the samples by a given rescale factor, i.e. invert the signal
	waveform->rescale(-1); 
	wave_max=waveform->max_amplitude(50,900,5); //find max amplitude between 50 and 900 samples
	if(wave_max.max_amplitude<20){
		waveform->rescale(-1);	
		Waveform::max_amplitude_informations wave_max_inv = waveform->max_amplitude(50,900,5);
		if(wave_max_inv.max_amplitude > wave_max.max_amplitude)
			wave_max = wave_max_inv;
		else   // stay with negative signals
			waveform->rescale(-1);
	}

	double charge_integration = waveform->charge_integrated(0,900);// pedestal already subtracted 
	double t_max              = wave_max.time_at_max*1.e9;
	double t_max_frac30       = waveform->time_at_frac(wave_max.time_at_max-1.3e-8,wave_max.time_at_max,0.3,wave_max,7)*1.e9;
	double t_max_frac50       = waveform->time_at_frac(wave_max.time_at_max-1.3e-8,wave_max.time_at_max,0.5,wave_max,7)*1.e9;
	
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


H4treeReco::~H4treeReco()
{
  fOut_->cd();
  recoT_->Write();
  fOut_->Close();
}
