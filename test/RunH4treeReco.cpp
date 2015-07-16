#include "interface/H4treeReco.h"

#include<iostream>

#include "TString.h"
#include "TFile.h"
#include "TChain.h"
#include "TObjArray.h"

TString EOSPREFIX="root://eoscms//eos/cms";

using namespace std;

int main(int argc, char* argv[])
{
  //check arguments
  if(argc<2)
    {
      cout << argv[0] << " input [output=H4treeRecoOutput.root]" << endl
	   << "\t input - csv list of files (EOS or local)" << endl
	   << "\t output - output file name (optional)" << endl;
      return 0;
    }

  //init chain
  TChain *t=new TChain("H4tree");
  TObjArray *tkns=TString(argv[1]).Tokenize(",");
  for(Int_t i=0; i<tkns->GetEntriesFast(); i++)
    {
      TString url=tkns->At(i)->GetName();
      if(!url.Contains(".root")) continue;
      if(url.Contains("/store/") && !url.Contains("root://eoscms//")) url = EOSPREFIX+"/"+url;
      t->Add(url);
    }
  cout << "Chain created with " << t->GetEntries() << " events in "
       << t->GetListOfFiles()->GetEntriesFast() << " files" << endl;

  //prepare output
  TString output="H4treeRecoOutput.root";
  if(argc>2) output=argv[2];
  cout << "Output will be stored in " << output << endl;

  //run reconstruction
  H4treeReco reco(t,output);
  reco.Loop();
  cout << "All done" << endl;
  
  return 0;
}
