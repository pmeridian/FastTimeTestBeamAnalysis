{
  gStyle->SetOptFit(111111);
  TFile *_file0 = TFile::Open("/tmp/meridian/RECO_3346.root");
  H4treeReco->Draw("charge_integ_smallw_mcp[3]>>pedestal(250,-250,250)","wc_xl_hits[0]==0 && wc_xr_hits[0]==0 && wc_xl_hits[1]==0 && wc_xr_hits[1]==0 && wc_yu_hits[1]==0 && wc_yd_hits[1]==0 && abs(t_max[1]-60)<10");
  TH1F* pedestal=(TH1F*)gROOT->FindObject("pedestal");
  pedestal->Fit("gaus");
  c1->SaveAs("noise.png");
  H4treeReco->Draw(Form("charge_integ_smallw_mcp[3]-%4.1f>>noise_corrected(250,-250,250)",pedestal->GetFunction("gaus")->GetParameter(1)),"wc_xl_hits[0]==0 && wc_xr_hits[0]==0 && wc_xl_hits[1]==0 && wc_xr_hits[1]==0 && wc_yu_hits[1]==0 && wc_yd_hits[1]==0 && abs(t_max[1]-60)<10");
  TH1F* noise_corrected=(TH1F*)gROOT->FindObject("noise_corrected");
  noise_corrected->Fit("gaus");
  c1->SaveAs("noise_corrected.png");
  H4treeReco->Draw(Form("charge_integ_smallw_mcp[2]-%4.1f>>signal1(100,-250,1000)",pedestal->GetFunction("gaus")->GetParameter(1)),"wave_max[1]>200 && abs(t_max[3]-t_max[1]+5.7)<.5 && wave_max[3]>30 && wave_max[3]<70"); 
  H4treeReco->Draw(Form("charge_integ_smallw_mcp[3]-%4.1f>>signal2(100,-250,1000)",pedestal->GetFunction("gaus")->GetParameter(1)),"wave_max[1]>200 && abs(t_max[2]-t_max[1]+5.7)<.5 && wave_max[2]>30 && wave_max[2]<70"); 
  // H4treeReco->Draw(Form("charge_integ_smallw_mcp[2]-%4.1f>>signal1(80,-200,1000)",pedestal->GetFunction("gaus")->GetParameter(1)),"wave_max[1]>200 && abs(t_max[3]-t_max[1]+5.7)<.5 && wave_max[3]>10 && wave_max[3]<200 && abs(wc_recox[1]-wc_recox[0]+24.2)<4 && abs(((wc_recox[1]+(wc_recox[0]-24.2)/2)-12.5)-17)<10 && abs((wc_recoy[1]+35)+1)<10"); 
  // H4treeReco->Draw(Form("charge_integ_smallw_mcp[3]-%4.1f>>signal2(80,-200,1000)",pedestal->GetFunction("gaus")->GetParameter(1)),"wave_max[1]>200 && abs(t_max[2]-t_max[1]+5.7)<.5 && wave_max[2]>10 && wave_max[2]<200 && abs(wc_recox[1]-wc_recox[0]+24.2)<4 && abs(((wc_recox[1]+(wc_recox[0]-24.2)/2)-12.5)-17)<10 && abs((wc_recoy[1]+35)+1)<10"); 
  // H4treeReco->Draw(Form("charge_integ_smallw_mcp[2]-%4.1f>>signal1(250,-1000,5000)",pedestal->GetFunction("gaus")->GetParameter(1)),"wave_max[1]<100 && abs(t_max[3]-t_max[1]+5.7)>4 && wave_max[3]<100"); 
  // H4treeReco->Draw(Form("charge_integ_smallw_mcp[3]-%4.1f>>signal2(250,-1000,5000)",pedestal->GetFunction("gaus")->GetParameter(1)),"wave_max[1]<100 && abs(t_max[2]-t_max[1]+5.7)>4 && wave_max[2]<100"); 

  TH1F* signal1=(TH1F*)gROOT->FindObject("signal1");
  TH1F* signal2=(TH1F*)gROOT->FindObject("signal2");
  TH1F* signal=signal2->Clone("signal");
  signal->Sumw2();
  signal->Add(signal1);
  //H4treeReco->Draw(Form("charge_integ_smallw_mcp[3]-%4.1f>>signal(250,-500,5000)",pedestal->GetFunction("gaus")->GetParameter(1)),"wave_max[1]>200"); 
  gROOT->ProcessLine(".L ~/work/H2_2015/FastTimeTestBeamAnalysis/macros/fitFunctions.C++");
  TF1* fitFunc=new TF1("fitFunc",sigFunc,-250,1000,7);
  fitFunc->FixParameter(1,0.);
  fitFunc->FixParameter(2,pedestal->GetFunction("gaus")->GetParameter(2));
  fitFunc->SetParLimits(0,0,100000);
  fitFunc->SetParLimits(3,1,200);
  fitFunc->SetParName(0,"NoiseNorm");
  fitFunc->SetParName(3,"LandauWidth");
  fitFunc->SetParName(4,"LandauMPV");
  fitFunc->SetParName(5,"LandauNorm");
  fitFunc->SetParLimits(4,80,3000);
  //  fitFunc->SetParLimits(6,50,3000);
  fitFunc->SetParameter(3,20);
  fitFunc->SetParameter(4,200);
  fitFunc->FixParameter(6,pedestal->GetFunction("gaus")->GetParameter(2));
  // fitFunc->FixParameter(3,100.);
  //  fitFunc->FixParameter(4,230.);
  // fitFunc->FixParameter(5,0.);
  signal->Fit("fitFunc","BR");
  signal->SetMarkerStyle(20);
  signal->SetMarkerSize(0.8);
  signal->GetXaxis()->SetTitle("Charge Integrated [ADC]");
  signal->Draw("PE");
  fitFunc->SetLineWidth(3);
  fitFunc->Draw("SAME");
  Double_t fitparams[7];
  fitFunc->GetParameters(fitparams);
  TF1 *landau= new TF1("landau",lanconvgau,-250,1000,4);
  landau->SetParameters(&fitparams[3]);
  landau->SetLineColor(kBlue);
  landau->SetLineStyle(2);
  landau->Draw("SAME");

  TF1 *noi=new TF1("noi","gaus",-250,1000);
  noi->SetParameter(0,fitparams[0]);
  noi->SetParameter(1,fitparams[1]);
  noi->SetParameter(2,fitparams[2]);
  noi->SetLineColor(kMagenta);
  noi->SetLineStyle(2);
  noi->Draw("SAME");

  c1->SaveAs("MIP_320mu_3346_Total_Ele50_Smallw.png");
}
