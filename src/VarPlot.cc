#include "interface/VarPlot.h"

std::vector<float>* VarPlot::Get(){ return &x; }
std::pair<std::vector<float> *, std::vector<float>*> VarPlot::Get2D(){ return std::make_pair<std::vector<float>*, std::vector<float>*>(&x,&y); }
float * VarPlot::Get(uint i){ return &(x.at(i)); }
std::pair<float*,float*> VarPlot::Get2D(uint i){ return std::make_pair<float*,float*>(&(x.at(i)),&(y.at(i))); }
uint VarPlot::Size() { return x.size(); }
TObject* VarPlot::Plot(){ return plot; }
void VarPlot::SetPlot(TObject* plot_){plot=plot_;}
TObject* VarPlot::GetPlot(){return plot;}
void VarPlot::SetName(TString name_){name=name_;}
void VarPlot::ClearVectors(){x.clear(); y.clear();}

void VarPlot::SetGM(TString group_, TString module_){
  group = group_;
  module = module_;
}

void VarPlot::Fill(float valX, float valY, int i) {
  if (type!=kPlot1D && type!=kPlotGraph && type!=kPlot1DProf) {std::cout << "WRONG 1D " << name.Data() << std::endl; return;}
  if (type==kPlot1D)
    {
      if (i<0) x.push_back(valX);
      else {
	if (x.size()<=i) x.resize(i+1);
	x.at(i)=valX;
      }
      (dynamic_cast<TH1F*>(plot))->Fill(valX);
    }
  if (type==kPlot1DProf)
    {
      if (i<0) {x.push_back(valX); y.push_back(valY);}
      else
	{
	  if (x.size()<=i){
	    x.resize(i+1);
	    y.resize(i+1);
	  }
	  x.at(i)=valX;
	  y.at(i)=valY;
	}
      (dynamic_cast<TProfile*>(plot))->Fill(valX,valY);
    }
  if (type==kPlotGraph)
    {
      (dynamic_cast<TGraph*>(plot))->SetPoint(*iHistEntry,*iThisEntry,valX);
    }
}

void VarPlot::Fill2D(float valX, float valY, float valZ, int i) {
  if (type!=kPlot2D && type!=kPlot2DProf) {std::cout << "WRONG 2D " << name.Data() << std::endl; return;}
  if (type==kPlot2D)
    {
      if (i<0) {x.push_back(valX); y.push_back(valY);}
      else
	{
	  if (x.size()<=i) {
	    x.resize(i+1);
	    y.resize(i+1);
	  }
	  x.at(i)=valX;
	  y.at(i)=valY;
	}
      (dynamic_cast<TH2F*>(plot))->Fill(valX,valY);
    }
  if (type==kPlot2DProf)
    {
      if (i<0) {x.push_back(valX); y.push_back(valY); z.push_back(valZ);}
      else {
	if (x.size()<=i) {
	  x.resize(i+1);
	  y.resize(i+1);
	  z.resize(i+1);
	}
	x.at(i)=valX;
	y.at(i)=valY;
	z.at(i)=valZ;
      }
      (dynamic_cast<TProfile2D*>(plot))->Fill(valX,valY,valZ);
    }
}

VarPlot::VarPlot()
{
  xptr = &x;
  yptr = &y;
  plot = NULL;
  waveform = NULL;
  doPlot = true;
  doProfile = false;
}

VarPlot::VarPlot(int *iThisEntry_, int *iHistEntry_, PlotType type_, bool profile_, uint size_):
  iThisEntry(iThisEntry_),
  iHistEntry(iHistEntry_),
  doProfile(profile_),
  type(type_)
{
  x.resize(size_);
  y.resize(size_);
  xptr = &x;
  yptr = &y;
  plot = NULL;
  waveform = NULL;
  doPlot = true;
  doProfile = false;
}

VarPlot::~VarPlot(){
  if (plot) delete plot;
  if (waveform) delete waveform;
}
