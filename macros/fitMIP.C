{
  TFile *_file0 = TFile::Open("/tmp/meridian/RECO_3346.root");
  H4treeReco->Draw("wave_max[3]>>signal(200,0,200)","wave_max[1]>200 && abs(t_max[3]-t_max[1]+5)<1 && wave_max[2]>10 && wave_max[2]<60 && abs(t_max[2]-t_max[1]+5)<1"); 
  gROOT->ProcessLine(".L ~/work/H2_2015/FastTimeTestBeamAnalysis/macros/fitFunctions.C++");
  TF1* fitFunc=new TF1("fitFunc",sigFunc,0,200,7);
  fitFunc->FixParameter(1,12.6);
  fitFunc->FixParameter(2,7.8);
  fitFunc->SetParLimits(3,1,50);
  fitFunc->SetParLimits(4,10,100);
  fitFunc->SetParLimits(6,0,30);
  fitFunc->SetParameter(3,10);
  fitFunc->SetParameter(4,40);
  fitFunc->FixParameter(6,7.8);
  signal->Fit("fitFunc","BR");
  gStyle->SetOptFit(111111);
  signal->SetMarkerStyle(20);
  signal->SetMarkerSize(0.8);
  signal->GetXaxis()->SetTitle("Amplitude [ADC]");
  signal->Draw("PE");
  fitFunc->SetLineWidth(3);
  fitFunc->Draw("SAME");
  Double_t fitparams[7];
  fitFunc->GetParameters(fitparams);
  TF1 *landau= new TF1("landau",lanconvgau,0,200,4);
  landau->SetParameters(&fitparams[3]);
  landau->SetLineColor(kBlue);
  landau->SetLineStyle(2);
  landau->Draw("SAME");
}
