#ifndef H4treeReco_h
#define H4treeReco_h

#include "interface/H4tree.h"

class H4treeReco : public H4tree
{

 public:
  H4treeReco(TTree *);
  void Loop(); 
  ~H4treeReco();
  
 private:
  
  TTree *recoT_;
};

#endif
