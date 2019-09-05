#include "TCanvas.h"
#include "TH2.h"
#include "TFile.h"
#include "TTree.h"
#include "TStyle.h"
#include <iostream>
#include <string>

using namespace std;

int main(int argc, char *argv[]){
  if(argc == 0) return 0;
  TFile *f = new TFile( argv[1] );
  if(f->IsZombie()){
    cout << Form("%s.root does not exist... exiting",argv[1]) << endl;
    return 0;
  }

  string name = argv[1];
  if(name.find("/") != string::npos) name.erase( 0, name.find_last_of("/") + 1 );
  if(name.find(".") != string::npos) name.erase( name.find_last_of(".") );

  TTree *t = (TTree*)f->Get("lightGuide");
  double x,z;
  t->SetBranchAddress("X",&x);
  t->SetBranchAddress("Z",&z);

  TCanvas *c = new TCanvas("Data","Data",1280,720);
  TH2D *h = new TH2D("eff","Relative Efficiency",40,-82,82,40,-42.875,42.875);

  int nEvents = t->GetEntries();
  for(int ev = 0; ev < nEvents; ev++){
    t->GetEntry(ev);
    h->Fill(z,x);
  }

  gStyle->SetOptStat(0);
  h->Scale(1.0/h->GetMaximum());
  h->Draw("COLZ");
  c->Print( Form("%s.png",name.c_str()) );
  delete f;
  return 1;
}
