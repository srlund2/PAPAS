/******************************************************************************
 * 
 * This script contains three primary functions. The first, plotFromCSV, will take a
 * .CSV file with data in the format:
 * 
 * x,y,z
 * x,y,z
 * 
 * And create a 2D histogram with the values contained. The histogram will be
 * saved to a root file with the name given by the second argument of the function.
 * 
 * The second function, compareHistos, takes the names of two root files
 * as inputs, retrieves a histogram from within both, and can subtract or divide the two. 
 * REAL?? It then takes the subtracted histogram and fills another 1D
 * histogram with the subtracted values, returning this histogram's RMS as the
 * desired output.
 * 
 * The third function, averageHistos, takes 4 histograms as inputs and averages them, 
 * returning a new histogram called haverage.
 * 
 * TO RUN:
 * Open an interactive ROOT session
 * $root -l
 * 
 * Load the script
 * [0].L plotFromCSV.cpp
 * 
 * Run either of the functions (whatever is necessary at the time)
 * [1]plotFromCSV("rawData.csv","outputRootFileName.root")
 * 
 * or
 *  * TO RUN:
 * Open an interactive ROOT session
 * $root -l
 * 
 * Load the script
 * [0].L plotFromCSV.cpp
 * [1]compareHistos("file1.root","file2.root")
 * 
 * As written, file1 should be the real data, while file2 is the simulated data.
 * 
 * or
 * * TO RUN:
 * Open an interactive ROOT session
 * $root -l
 * 
 * Load the script
 * [0].L plotFromCSV.cpp
 * [1]averageHistos("file1.root","file2.root","file3.root","file4.root")
 * 
*******************************************************************************/


#include <fstream>
#include <string>
#include "TVectorD.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TFile.h"


using namespace std;

string getDataPoint( string &input, size_t spaces = 1, string delim = "," );

void plotFromCSV(TString fileName, TString outFileName){

  //Open the csv
  ifstream file(fileName);
  //Check if the file opened properly. Exit if not
  if( !file.is_open() ){
    cout << "File didn't open" << endl;
    return;
  }

 //make sure the size of each histogram is the same && the bins are in the middle
  float range = 54;
  TH2F *h = new TH2F("lightguideintensity","Relative Intensity (Box experiment);X-plane (mm);Z-plane (mm)",18,-range/2.,range/2.,18,-range/2.,range/2.);

  //While we still haven't reached the end of the file, get the next line, parse it, and fill the histogram
  //with the value on that line
  string buffer;
  while( !file.eof() ){
    getline(file,buffer);

    float x = atof(getDataPoint(buffer,1).c_str() );
    float y = atof(getDataPoint(buffer,1).c_str() );
    float counts = atof(getDataPoint(buffer,1).c_str() );

    h->Fill(x-range/2.,y-range/2.,counts);

  }

  //Create a new root file to save the histogram to. "recreate" will overwrite any file with the same name
  TFile f(outFileName.Data(),"recreate");

    // Scale it to make it relative like plothisto does
    h->Scale(1.0/h->GetMaximum());
  h->Write(); //Write the histogram to the file

  //Create a new TCanvas to draw the histogram onto
  TCanvas *c = new TCanvas("canv","canv",800,600);
  //gStyle->SetPalette(55);
  h->Draw("colz"); //Draw it. colz is "Colored Z axis"

  f.Close(); //Close the file

}

void compareHistos( string file1, string file2){

//----------------------RETIREVE THE HISTOGRAMS---------------------------------
  //Open file 1 and retrieve the histogram from within
  TFile f1(file1.c_str(),"read");
  TH2F *hreal = (TH2F*)f1.Get("lightguideintensity");
  // //normalize 
  // hreal->Scale(1./hreal->Integral());
  //hreal->Scale(1.0/hreal->GetMaximum());

    //Open file 2 and retrieve the histogram from within
  TFile f2(file2.c_str(),"read");
  TH2F *hsim = (TH2F*)f2.Get("eff");
  // //normalize
  // h2->Scale(1./h2->Integral());

  //make some same-size hists to work with later, based on hreal
  TH2F *hdiff = (TH2F*)hreal->Clone();
  TH2F *hrat = (TH2F*)hreal->Clone();
  TH2F *hcopy = (TH2F*)hreal->Clone();
  hcopy->SetNameTitle("copy", "Difference;X-plane (mm);Z-plane (mm)");
  hdiff->SetNameTitle("difference", "Difference;X-plane (mm);Z-plane (mm)");
  hrat->SetNameTitle("divide", "Ratio;X-plane (mm);Z-plane (mm)");

  //We want to subtract sim from real (add -1)
  hdiff->Add(hsim, -1); 
  
  //Create a new histogram to hold the differences for each bin
 // TH1F *hDev = new TH1F("stdDev","stdDev",100,-10,10);

  // //For each bin in the original histogram, fill the new histogram
  // //with the difference of the two
  // int nBins = hcopy->GetNbinsX() * hdiff->GetNbinsY();
  // for(int bin = 0; bin < nBins; bin++){
  //   float val = hdiff->GetBinContent(bin);
  //   hdiff->Fill(val);
  //   //h3->SetBinContent(bin,val);
  
  //}

  //make ratio histogram real/sim
  hrat->Divide(hsim);
  
  // make percent error histogram
  TH2F *hpercer = (TH2F*)hdiff->Clone();
  hpercer->SetNameTitle("percerror", "Percent Error;X-plane (mm);Z-plane (mm)");
  hpercer->Divide(hreal);
  hpercer->Scale(100.);
  hpercer->Write();

  // //Retrieve the RMS from this new histogram
  // float RMS = hDev->GetRMS();
  // cout << "RMS " << RMS << endl;
  // //cout << "Percent error " << RMS << endl;

  //   //Create a canvas to view the histograms on
  TCanvas *c = new TCanvas("canvas","canvas",4000,1600);
  TFile f("comparison.root","recreate");
  gStyle->SetOptStat(0);
  c->Divide(3,2);
  c->cd(1);
  hreal->Draw("colz");
  hreal->Write();

  c->cd(2);
  hsim->Draw("colz");
  hsim->Write();

  c->cd(3);
  hdiff->Draw("colz");
  hdiff->Write();

  c->cd(4);
  hrat->Draw("colz");
  hrat->Write();

  c->cd(5); 
  hpercer->Draw("cloz");
  hpercer->Write();

  c->Print("Comparison-plots.png");
  // hDev->Write();

  f.Close();
  delete c;

}

///gernerate a similar series of histograms. 
//functionalize it. difference, ratio, relative error, 
// all for histograms normalized in different ways

// outer fucntion --open each file, retrieve histograms, 
// loop --- for each normalization method
/// ---- noramlize both histograms
/// call anotehr function
/// other function will make all the analysis whatevers (subtract, dev, divide, per erreor, etc)
// write to file 
// give things good names
//it would be nice to have a canvas for each normalization with all of the comparisons


string getDataPoint( string &input, size_t spaces, string delim ){
  size_t firstComma = input.find_first_of(delim.c_str());
  string buffer = input.substr(0, firstComma );
  input.erase(0,firstComma+spaces);
  return buffer;
}


void averageHistos( string file1, string file2, string file3, string file4){

// //----------------------RETIREVE THE HISTOGRAMS---------------------------------
  //Open file 1 and retrieve the histogram from within
  TFile f1(file1.c_str(),"read");
  TH2F *h1 = (TH2F*)f1.Get("lightguideintensity");


    //Open file 2 and retrieve the histogram from within
  TFile f2(file2.c_str(),"read");
  TH2F *h2 = (TH2F*)f2.Get("lightguideintensity");

      //Open file 3 and retrieve the histogram from within
  TFile f3(file3.c_str(),"read");
  TH2F *h3 = (TH2F*)f3.Get("lightguideintensity");

        //Open file 4 and retrieve the histogram from within
  TFile f4(file4.c_str(),"read");
  TH2F *h4 = (TH2F*)f4.Get("lightguideintensity");



  //make some same-size hists to work with later, clone of h1
  TH2F *htotal = (TH2F*)h1->Clone();

  //add all the other histograms to it
  htotal->Add(h2); 
  htotal->Add(h3);
  htotal->Add(h4);
  
  //divide total by 4?
  TH2F *haverage = (TH2F*)htotal->Clone();
  haverage->Scale(1./4); // this should take the average of the four files
  haverage->SetNameTitle("average", "Average;X-plane (mm);Z-plane (mm)");
  haverage->Write();


}

