#include "TCanvas.h"
#include "TH2.h"
#include "TFile.h"
#include "TTree.h"
#include "TStyle.h"
#include <iostream>

using namespace std;

int main(int argc, char *argv[]){
  TFile *f = new TFile( Form("%s.root",argv[1]) );
  if(f->IsZombie()){
    cout << Form("%s.root does not exist... exiting",argv[1]) << endl;
    return 0;
  }

  TTree *t = (TTree*)f->Get("lightGuide");
  double x,y;
  t->SetBranchAddress("X",&x);
  t->SetBranchAddress("Y",&y);

  TCanvas *c = new TCanvas("Data","Data",1280,720);
  TH2D *h = new TH2D("eff","Relative Efficiency",40,-82,82,40,-42.875,42.875);

  int nEvents = t->GetEntries();
  for(int ev = 0; ev < nEvents; ev++){
    t->GetEntry(ev);
    h->Fill(y,x);
  }

  gStyle->SetOptStat(0);
  h->Scale(1.0/h->GetMaximum());
  h->Draw("COLZ");
  c->Print( Form("%s.png",argv[1]) );
  delete f;
  return 1;
}
