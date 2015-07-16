#include "interface/H4treeReco.h"
#include "interface/WaveformUtils.hpp"
#include "interface/WaveformFit.hpp"

H4treeReco::H4treeReco()
{
  recoT_ = new TTree("H4treeReco","H4treeReco");
  recoT_->Branch("runNumber",    &runNumber,    "runNumber/i");
  recoT_->Branch("spillNumber",  &spillNumber,  "spillNumber/i");
  recoT_->Branch("evtNumber",    &evtNumber,    "evtNumber/i");
  recoT_->Branch("evtTimeDist",  &evtTimeDist,  "evtTimeDist/i");
  recoT_->Branch("evtTimeStart", &evtTimeStart, "evtTimeStart/i");
  recoT_->Branch("nEvtTimes",    &nEvtTimes,    "nEvtTimes/i");
}

H4treeReco::~H4treeReco()
{
}
