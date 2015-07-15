#define H4tree_cxx
#include "interface/H4tree.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

void H4tree::Loop()
{
//   In a ROOT session, you can do:
//      Root > .L H4tree.C
//      Root > H4tree t
//      Root > t.GetEntry(12); // Fill t data members with entry number 12
//      Root > t.Show();       // Show values of entry 12
//      Root > t.Show(16);     // Read and show values of entry 16
//      Root > t.Loop();       // Loop on all entries
//

//     This is the loop skeleton where:
//    jentry is the global entry number in the chain
//    ientry is the entry number in the current Tree
//  Note that the argument to GetEntry must be:
//    jentry for TChain::GetEntry
//    ientry for TTree::GetEntry and TBranch::GetEntry
//
//       To read only selected branches, Insert statements like:
// METHOD1:
//    fChain->SetBranchStatus("*",0);  // disable all branches
//    fChain->SetBranchStatus("branchname",1);  // activate branchname
// METHOD2: replace line
//    fChain->GetEntry(jentry);       //read all branches
//by  b_branchname->GetEntry(ientry); //read only this branch
   if (fChain == 0) return;

   Long64_t nentries = fChain->GetEntriesFast();

   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      // if (Cut(ientry) < 0) continue;
   }
}

H4tree::H4tree(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("root://eoscms//eos/cms/store/group/dpg_ecal/alca_ecalcalib/TimingTB_H2_Jul2015/raw/DataTree/3374/14.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("root://eoscms//eos/cms/store/group/dpg_ecal/alca_ecalcalib/TimingTB_H2_Jul2015/raw/DataTree/3374/14.root");
      }
      f->GetObject("H4tree",tree);

   }
   Init(tree);
}

H4tree::~H4tree()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t H4tree::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t H4tree::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void H4tree::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("runNumber", &runNumber, &b_runNumber);
   fChain->SetBranchAddress("spillNumber", &spillNumber, &b_spillNumber);
   fChain->SetBranchAddress("evtNumber", &evtNumber, &b_evtNumber);
   fChain->SetBranchAddress("evtTimeDist", &evtTimeDist, &b_evtTimeDist);
   fChain->SetBranchAddress("evtTimeStart", &evtTimeStart, &b_evtTimeStart);
   fChain->SetBranchAddress("nEvtTimes", &nEvtTimes, &b_nEvtTimes);
   fChain->SetBranchAddress("evtTime", evtTime, &b_evtTime);
   fChain->SetBranchAddress("evtTimeBoard", evtTimeBoard, &b_evtTimeBoard);
   fChain->SetBranchAddress("nAdcChannels", &nAdcChannels, &b_nAdcChannels);
   fChain->SetBranchAddress("adcBoard", adcBoard, &b_adcBoard);
   fChain->SetBranchAddress("adcChannel", adcChannel, &b_adcChannel);
   fChain->SetBranchAddress("adcData", adcData, &b_adcData);
   fChain->SetBranchAddress("nTdcChannels", &nTdcChannels, &b_nTdcChannels);
   fChain->SetBranchAddress("tdcBoard", tdcBoard, &b_tdcBoard);
   fChain->SetBranchAddress("tdcChannel", tdcChannel, &b_tdcChannel);
   fChain->SetBranchAddress("tdcData", tdcData, &b_tdcData);
   fChain->SetBranchAddress("nDigiSamples", &nDigiSamples, &b_nDigiSamples);
   fChain->SetBranchAddress("digiFrequency", digiFrequency, &b_digiFrequency);
   fChain->SetBranchAddress("digiGroup", digiGroup, &b_digiGroup);
   fChain->SetBranchAddress("digiChannel", digiChannel, &b_digiChannel);
   fChain->SetBranchAddress("digiSampleIndex", digiSampleIndex, &b_digiSampleIndex);
   fChain->SetBranchAddress("digiSampleValue", digiSampleValue, &b_digiSampleValue);
   fChain->SetBranchAddress("digiBoard", digiBoard, &b_digiBoard);
   fChain->SetBranchAddress("nScalerWords", &nScalerWords, &b_nScalerWords);
   fChain->SetBranchAddress("scalerWord", scalerWord, &b_scalerWord);
   fChain->SetBranchAddress("scalerBoard", scalerBoard, &b_scalerBoard);
   fChain->SetBranchAddress("nPatterns", &nPatterns, &b_nPatterns);
   fChain->SetBranchAddress("pattern", &pattern, &b_pattern);
   fChain->SetBranchAddress("patternBoard", &patternBoard, &b_patternBoard);
   fChain->SetBranchAddress("patternChannel", &patternChannel, &b_patternChannel);
   fChain->SetBranchAddress("nTriggerWords", &nTriggerWords, &b_nTriggerWords);
   fChain->SetBranchAddress("triggerWords", triggerWords, &b_triggerWords);
   fChain->SetBranchAddress("triggerWordsBoard", triggerWordsBoard, &b_triggerWordsBoard);
   Notify();
}

Bool_t H4tree::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void H4tree::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t H4tree::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}

