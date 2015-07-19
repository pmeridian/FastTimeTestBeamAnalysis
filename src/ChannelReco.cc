#include "interface/ChannelReco.h"

//
ChannelReco::ChannelReco(JSONWrapper::Object &cfg, PlotType ptype, Bool_t doPlot, Bool_t doProfile) :
  ptype_(ptype),
  doPlot_(doPlot),
  doProfile_(doProfile),
  plot_(0),
  waveform_(0)
{
  //read configuration
  name_              = cfg["name"].toString();
  group_             = cfg["group"].toInt();
  module_            = cfg["channel"].toInt();
  searchWindowUp_       = cfg["searchWindowUp"].toInt();
  searchWindowLo_       = cfg["searchWindowLo"].toInt();
  spyWindowUp_       = cfg["spyWindowUp"].toInt();
  spyWindowLo_       = cfg["spyWindowLo"].toInt();
  cfdWindowLo_       = cfg["cfdWindowLo"].toDouble();
  fixedThrForTiming_ = cfg["fixedThrForTiming"].toDouble();
  pedestalWindowLo_   = cfg["pedestalWindowLo"].toInt();
  pedestalWindowUp_   = cfg["pedestalWindowUp"].toInt();
  thrForPulseInversion_ = cfg["thrForPulseInversion"].toInt();
 
  //save configuration values in a histogram
  chRecoCfgH_ = new TH1F(name_+"_cfg",";"+name_+";Value",12,0,12);
  chRecoCfgH_->SetDirectory(0);
  chRecoCfgH_->SetBinContent(1,group_);                  chRecoCfgH_->GetXaxis()->SetBinLabel(1,"group");
  chRecoCfgH_->SetBinContent(2,module_);                 chRecoCfgH_->GetXaxis()->SetBinLabel(2,"channel");
  chRecoCfgH_->SetBinContent(3,searchWindowUp_);         chRecoCfgH_->GetXaxis()->SetBinLabel(3,"searchWindowUp");
  chRecoCfgH_->SetBinContent(4,searchWindowLo_);         chRecoCfgH_->GetXaxis()->SetBinLabel(4,"searchWindowLo");
  chRecoCfgH_->SetBinContent(5,cfdWindowLo_);            chRecoCfgH_->GetXaxis()->SetBinLabel(5,"cfdWindowLo");
  chRecoCfgH_->SetBinContent(6,fixedThrForTiming_);      chRecoCfgH_->GetXaxis()->SetBinLabel(6,"fixedThrForTiming");
  chRecoCfgH_->SetBinContent(7,pedestalWindowLo_);       chRecoCfgH_->GetXaxis()->SetBinLabel(7,"pedestalWindowLo");
  chRecoCfgH_->SetBinContent(8,pedestalWindowUp_);       chRecoCfgH_->GetXaxis()->SetBinLabel(8,"pedestalWindowUp");
  chRecoCfgH_->SetBinContent(9,thrForPulseInversion_);   chRecoCfgH_->GetXaxis()->SetBinLabel(9,"thrForPulseInversion");
  chRecoCfgH_->SetBinContent(10,spyWindowUp_);        chRecoCfgH_->GetXaxis()->SetBinLabel(10,"spyWindowUp");
  chRecoCfgH_->SetBinContent(11,spyWindowLo_);        chRecoCfgH_->GetXaxis()->SetBinLabel(11,"spyWindowLo");

  std::cout << "\t " << name_ << " will be reconstructed from group=" << group_ << " channel=" << module_ << std::endl;
}

//
void ChannelReco::Fill(float valX, float valY, int i) 
{
  if(plot_==0 || !doPlot_)
    {
      return;
    }
  if (ptype_!=kPlot1D && ptype_!=kPlotGraph && ptype_!=kPlot1DProf) 
    {
      std::cout << "[ChannelReco::Fill] caught wrong pptype_ in " << name_.Data() << std::endl; 
      return;
    }

  if (ptype_==kPlot1D)
    {
      if (i<0) x_.push_back(valX);
      else 
	{
	  if (x_.size()<=i) x_.resize(i+1);
	  x_.at(i)=valX;
	}
      (dynamic_cast<TH1F*>(plot_))->Fill(valX);
    }
  if (ptype_==kPlot1DProf)
    {
      if (i<0) { x_.push_back(valX); y_.push_back(valY); }
      else
	{
	  if (x_.size()<=i)
	    {
	      x_.resize(i+1);
	      y_.resize(i+1);
	    }
	  x_.at(i)=valX;
	  y_.at(i)=valY;
	}
      (dynamic_cast<TProfile*>(plot_))->Fill(valX,valY);
    }
  if (ptype_==kPlotGraph)
    {
      (dynamic_cast<TGraph*>(plot_))->SetPoint(group_,module_,valX);
    }
}

//
void ChannelReco::Fill2D(float valX, float valY, float valZ, int i) 
{
  if(plot_==0 || !doPlot_)
    {
      return;
    }
  if (ptype_!=kPlot2D && ptype_!=kPlot2DProf) 
    {
      std::cout << "[ChannelReco::Fill2D] caught wrong pptype_ in " << name_.Data() << std::endl; 
      return;
    }

  if (ptype_==kPlot2D)
    {
      if (i<0) {x_.push_back(valX); y_.push_back(valY);}
      else
	{
	  if (x_.size()<=i) 
	    {
	      x_.resize(i+1);
	      y_.resize(i+1);
	    }
	  x_.at(i)=valX;
	  y_.at(i)=valY;
	}
      (dynamic_cast<TH2F*>(plot_))->Fill(valX,valY);
    }
  if (ptype_==kPlot2DProf)
    {
      if (i<0)
	{
	  x_.push_back(valX); 
	  y_.push_back(valY); 
	  z_.push_back(valZ);
	}
      else {
	if (x_.size()<=i) 
	  {
	    x_.resize(i+1);
	    y_.resize(i+1);
	    z_.resize(i+1);
	  }
	x_.at(i)=valX;
	y_.at(i)=valY;
	z_.at(i)=valZ;
      }
      (dynamic_cast<TProfile2D*>(plot_))->Fill(valX,valY,valZ);
    }
}


//
ChannelReco::~ChannelReco()
{
  if (plot_)     delete plot_;
  if (waveform_) delete waveform_;
}
