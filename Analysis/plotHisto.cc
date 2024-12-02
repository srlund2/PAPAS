#include "TCanvas.h"
#include "TH2.h"
#include "TFile.h"
#include "TTree.h"
#include "TStyle.h"
#include <iostream>
#include <string>

using namespace std;

int main(int argc, char *argv[]){
  //loads in root file
  float xRange, zRange;
  if(argc == 0) return 0;
  TFile *f = new TFile( argv[1] );
  if(f->IsZombie()){
    cout << Form("%s.root does not exist... exiting",argv[1]) << endl;
    return 0;
  }
  //read root file
  string name = argv[1];
  if(name.find("/") != string::npos) name.erase( 0, name.find_last_of("/") + 1 );
  if(name.find(".") != string::npos) name.erase( name.find_last_of(".") );

  //parse and define root file
  TTree *t = (TTree*)f->Get("lightGuide");
  vector<double> *x=0,*z=0;
  t->SetBranchAddress("X",&x);
  t->SetBranchAddress("Z",&z);
  t->SetBranchAddress("ReflCount",&ref);

  //set size paramaters of histo, defaults to 40. mm in x, 80. mm in z, with a 720 canvas.
  xRange = (argc > 2) ? atof( argv[2] ) : 40.;
  zRange = (argc > 3) ? atof( argv[3] ) : 80.;
  int canvw = (argc > 4) ? atof( argv[4] ) : 720;
  float asrat = zRange/xRange;

  //creates the canvas
  TCanvas *c = new TCanvas("Data","Data",canvw*asrat+100,canvw);
  
  //set binning parameters, preset is 44 bins in x and 21 bins in y
  TH2D *h = new TH2D("eff","Relative Efficiency",44,-1*zRange/2.,zRange/2.,21,-1*xRange/2.,xRange/2.);

  // //optional second histogram that tracks the number of relfections for each collected photon 
  // //if you change the binning on h, you should probably also change the binning on r. otherwise it will break :)
  // TH2D *r = new TH2D("bounces","Number of bounces",44,-1*zRange/2.,zRange/2.,21,-1*xRange/2.,xRange/2.); 
  

  //filling new histogram(s) from provided file
  int nEvents = t->GetEntries();
  for(int ev = 0; ev < nEvents; ev++){
    t->GetEntry(ev);
    int nPoints = x->size();
    for(int i = 0; i < nPoints; i++){
      h->Fill(z->at(i),x->at(i));
      // r->Fill(z->at(i),x->at(i),ref);
    }
  }


  // //A few lines to make the reflection plot look nice and be correct
  // r->Divide(h);
  // // Set all weird zeros to real zeros in reflection plot
  // int nbins = (r->GetNbinsX()+2)*(r->GetNbinsY()+2);
  //   for (Int_t bin = 0; bin < nbins; ++bin) {
  //     if (r->GetBinContent(bin)==0){
  //       r->SetBinContent(bin, 0.00000000001);
  //     }
  //   }

  //   int hbins = (h->GetNbinsX()+2)*(h->GetNbinsY()+2);
  //   for (Int_t bin = 0; bin < hbins; ++bin) {
  //     if (h->GetBinContent(bin)==0){
  //       h->SetBinContent(bin, 0.00000000001);     
  //     }
  //   }


  // // If you'd like a root file with only the effeciency data produced by this script, 
  // // (instead of the full data profile from the simulation), this will create that.
  // TFile k( Form("%s-duplicate.root",name.c_str()), "recreate");
  // h->Write();
  // k.Close();


// //print out the total efficiency to command line, if desired
//   float total = t->GetEntries("EventNo");
//   float hits = nEvents;
//   float efficiency = (hits/total)*100;
//   cout << "total efficiency: " << efficiency << "%" << endl;


//canvas settings
c->SetRightMargin(0.2);
c->SetLeftMargin(0.15);

// make 2D heat map
  gStyle->SetOptStat(0);
  //begin non-essential labeling/decorative stuff
  gStyle->SetPalette(55);
  h->Scale(1.0/h->GetMaximum());
  h->GetXaxis()->SetTitle("X-plane (mm)");
  h->GetYaxis()->SetTitle("Z-plane (mm)");
  h->GetZaxis()->SetTitle("Relative Efficiency Scale");
  h->GetXaxis()->CenterTitle(true);
  h->GetXaxis()->SetTitleOffset(1);
  h->GetYaxis()->CenterTitle(true);
  h->GetZaxis()->CenterTitle(true);
  h->GetZaxis()->SetTitleOffset(1.5);
  h->GetZaxis()->RotateTitle(true);
  //end non-essential labeling/decorative stuff
  h->Draw("Colz"); //Colz for normal heat plot
  h->SetAxisRange(0.,1.0,"Z"); 
  c->Print( Form("%s.png",name.c_str()) );
  


// // make 2D reflection map
//   gStyle->SetOptStat(0);
     //begin non-essential labeling/decorative stuff
//   gStyle->SetPalette(51);  
//   r->GetXaxis()->SetTitle("X-plane (mm)");
//   r->GetYaxis()->SetTitle("Z-plane (mm)");
//   r->GetZaxis()->SetTitle("Average number of bounces");
//   r->GetXaxis()->CenterTitle(true);
//   r->GetXaxis()->SetTitleOffset(1);
//   r->GetYaxis()->CenterTitle(true);
//   r->GetZaxis()->CenterTitle(true);
//   r->GetZaxis()->SetTitleOffset(1.5);
//   r->GetZaxis()->RotateTitle(true);
     //end non-essential labeling/decorative stuff
//   r->Draw("Colz"); //Colz for normal heat plot
//   c->Print( Form("%s-reflections.png",name.c_str()) );



  delete f;
  return 1;
}
