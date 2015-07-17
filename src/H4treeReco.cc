#include "interface/H4treeReco.h"
#include "interface/WaveformUtils.hpp"
#include "interface/WaveformFit.hpp"
#include "interface/Waveform.hpp"

#include <iostream>

//
H4treeReco::H4treeReco(TChain *tree,TString outUrl) : H4tree(tree)
{
  fOut_  = TFile::Open(outUrl,"RECREATE");
  recoT_ = new TTree("H4treeReco","H4treeReco");
  recoT_->SetDirectory(fOut_);
  recoT_->Branch("runNumber",    &runNumber,    "runNumber/i");
  recoT_->Branch("spillNumber",  &spillNumber,  "spillNumber/i");
  recoT_->Branch("evtNumber",    &evtNumber,    "evtNumber/i");
  recoT_->Branch("evtTimeDist",  &evtTimeDist,  "evtTimeDist/i");
  recoT_->Branch("evtTimeStart", &evtTimeStart, "evtTimeStart/i");
  recoT_->Branch("nEvtTimes",    &nEvtTimes,    "nEvtTimes/i");
  
  //add more variables relevant for the study
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
      //https://github.com/cmsromadaq/H4DQM/blob/master/src/plotterTools.cpp#L1296
      
      //loop over the relevant channels and reconstruct the waveforms
      //https://github.com/cmsromadaq/H4DQM/blob/master/src/plotterTools.cpp#L1785
      
      // Construct the waveform
	for(int iSample=0; iSample< tree->nDigiSamples; ++iSample){
	      	//waveform->addTimeAndSample(tree->digiSampleIndex[iSample]*timeSampleUnit(tree->digiFrequency[iSample]),tree->digiSampleValue[iSample]);
	      	waveform->addTimeAndSample(tree->digiSampleIndex[iSample]*0.2,tree->digiSampleValue[iSample]);
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

H4treeReco::~H4treeReco()
{
  fOut_->cd();
  recoT_->Write();
  fOut_->Close();
}
