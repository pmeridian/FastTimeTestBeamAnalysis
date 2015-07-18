#include "interface/ChannelPlot.h"

//
ChannelPlot::ChannelPlot() :
  group_(0),
  module_(0),
  ptype_(kNull),
  doPlot_(true),
  doProfile_(false),
  plot_(0),
  waveform_(0)
{
}

//
ChannelPlot::ChannelPlot(UInt_t group, UInt_t module, PlotType ptype, Bool_t doPlot, Bool_t doProfile) :
  group_(group),
  module_(module),
  ptype_(ptype),
  doPlot_(doPlot),
  doProfile_(doProfile),
  plot_(0),
  waveform_(0)
{
}

//
void ChannelPlot::Fill(float valX, float valY, int i) 
{
  if(plot_==0 || !doPlot_)
    {
      return;
    }
  if (ptype_!=kPlot1D && ptype_!=kPlotGraph && ptype_!=kPlot1DProf) 
    {
      std::cout << "[ChannelPlot::Fill] caught wrong pptype_ in " << name_.Data() << std::endl; 
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
void ChannelPlot::Fill2D(float valX, float valY, float valZ, int i) 
{
  if(plot_==0 || !doPlot_)
    {
      return;
    }
  if (ptype_!=kPlot2D && ptype_!=kPlot2DProf) 
    {
      std::cout << "[ChannelPlot::Fill2D] caught wrong pptype_ in " << name_.Data() << std::endl; 
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
ChannelPlot::~ChannelPlot()
{
  if (plot_)     delete plot_;
  if (waveform_) delete waveform_;
}
