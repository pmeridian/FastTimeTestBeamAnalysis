#ifndef _varplot_h_
#define _varplot_h_

#include "TObject.h"
#include "TProfile.h"
#include "TProfile2D.h"
#include "TGraph.h"
#include "TString.h"

#include "interface/WaveformUtils.hpp"
#include "interface/WaveformFit.hpp"

class ChannelPlot{

public:

  enum PlotType { kNull, kPlot1D, kPlot1DProf, kPlot2D, kPlot2DProf, kPlotGraph, kPlotHistory };

  ChannelPlot();
  ChannelPlot(UInt_t group, UInt_t module, PlotType ptype, Bool_t doPlot, Bool_t doProfile);
 
  UInt_t Size()                                               { return x_.size(); }
  std::vector<float> &GetX()                                  { return x_; }
  float GetX(UInt_t i)                                        { return x_.at(i); }
  std::vector<float> &GetY()                                  { return y_; }
  float GetY(UInt_t i)                                        { return y_.at(i); }
  std::pair<float,float> GetXY(uint i)                        { return std::make_pair<float,float>(x_.at(i),y_.at(i)); }
  std::pair<std::vector<float> *, std::vector<float>*> Get2D(){ return std::make_pair<std::vector<float>*, std::vector<float>*>(&x_,&y_); }
  void ClearVectors()                                         { x_.clear(); y_.clear(); }
  void SetPlot(TObject* plot)                                 { plot_=plot; }
  TObject* GetPlot()                                          { return plot_; }
  void SetName(TString name)                                  { name_=name; }
  TString GetName()                                           { return name_; }
  UInt_t GetGroup()                                           { return group_; }
  UInt_t GetModule()                                          { return module_; }
  void SetGroup(UInt_t group)                                 { group_=group; }
  void SetModule(UInt_t module)                               { module_=module; }
  Waveform *GetWaveform()                                     { return waveform_; }
  void SetWaveform(Waveform *w)                               { waveform_=w; }
  void Fill(float valX, float valY, int i=-1);
  void Fill2D(float valX, float valY, float valZ, int i);
 
  ~ChannelPlot();

 private:
  
  Int_t group_, module_;
  int ptype_;
  bool doPlot_, doProfile_;
  TString name_;
  std::vector<float> x_,y_,z_;
  TObject *plot_;
  Waveform *waveform_;
};

#endif
