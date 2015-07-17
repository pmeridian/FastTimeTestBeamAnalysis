#ifndef _varplot_h_
#define _varplot_h_

#include "TObject.h"
#include "TProfile.h"
#include "TProfile2D.h"
#include "TGraph.h"
#include "TString.h"

#include "interface/WaveformUtils.hpp"
#include "interface/WaveformFit.hpp"

typedef enum PlotType {
  kPlot1D,
  kPlot1DProf,
  kPlot2D,
  kPlot2DProf,
  kPlotGraph,
  kPlotHistory
} PlotType;

class VarPlot{

public:

  float* Get(uint i);
  std::pair<float*,float*> Get2D(uint i);
  uint Size();
  TObject* Plot();
  void Fill(float valX, float valY, int i=-1);
  void Fill2float(float valX, float valY, float valZ, int i=-1);
  void ClearVectors();
  VarPlot();
  VarPlot(int *iThisEntry, int *iHistEntry_, PlotType type_, bool profile_=false, uint size_=0);
  ~VarPlot();

  TString name;
  int *iThisEntry;
  int *iHistEntry;

  std::vector<float>* Get();
  std::pair<std::vector<float>*, std::vector<float>*> Get2D();

  void SetPlot(TObject* plot_);
  TObject* GetPlot();
  void SetName(TString name_);
  void Fill2D(float valX, float valY, float valZ, int i);
  void SetGM(TString group_, TString module_);
  bool doPlot;
  TString group;
  TString module;

  bool doProfile;
  int type;
  std::vector<float> x;
  std::vector<float> y;
  std::vector<float> z;
  TObject *plot;

  std::vector<float> *xptr;
  std::vector<float> *yptr;

  Waveform *waveform;
};

#endif
