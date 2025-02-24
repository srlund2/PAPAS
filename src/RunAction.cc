// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
/// \file /src/RunAction.cc
/// \brief Implementation of the RunAction class
//
// @Author Chad Lantz


#include "G4Timer.hh"

#include "G4RunManager.hh"
#include "G4Run.hh"

#include "RunAction.hh"
#include "EventAction.hh"

#include "G4AnalysisManager.hh"
#include "G4SystemOfUnits.hh"


/*
 *
 */
RunAction::RunAction(G4String fileName)
 : G4UserRunAction(),fTimer(0){
  fTimer = new G4Timer;
  m_fileName = fileName;

  G4RunManager::GetRunManager()->SetPrintProgress(1);

  // Get analysis manager and set the type
  auto analysisManager = G4AnalysisManager::Instance();
  analysisManager->SetDefaultFileType("root");
  analysisManager->SetVerboseLevel(1);
  // Only merge in MT mode to avoid warning when running in Sequential mode
#ifdef G4MULTITHREADED
  analysisManager->SetNtupleMerging(true);
#endif

  // Open an output file
  if(m_fileName == "") m_fileName = "output.root";
  analysisManager->OpenFile(m_fileName);
  
  analysisManager->SetHistoDirectoryName("histo");
  analysisManager->SetNtupleDirectoryName("ntuple");

  analysisManager->CreateH2("position", "Emission Position of Detected Photons", 100, -39, 30, 100, -30, 30);


  // Creating ntuple
  // Populate the vector with valid pointers and assign them to branches
  // These vectors are filled in PMTSD
  for(int i = 0; i < 7; i ++ ) fPtrVec.push_back( new std::vector< double > );
  analysisManager->CreateNtuple("lightGuide", "pos and momentum");
  analysisManager->CreateNtupleIColumn( "EventNo" );
  analysisManager->CreateNtupleDColumn( "X",       *fPtrVec[0] );
  analysisManager->CreateNtupleDColumn( "Z",       *fPtrVec[1] );
  analysisManager->CreateNtupleDColumn( "hitX",    *fPtrVec[2] );
  analysisManager->CreateNtupleDColumn( "hitZ",    *fPtrVec[3] );
  analysisManager->CreateNtupleDColumn( "time",    *fPtrVec[4] );
  analysisManager->CreateNtupleDColumn( "theta",   *fPtrVec[5] );
  analysisManager->CreateNtupleDColumn( "energy",  *fPtrVec[6] );
  analysisManager->FinishNtuple();

}

/*
 *
 */
RunAction::~RunAction(){
  delete fTimer;
  delete G4AnalysisManager::Instance();
  for(auto vec : fPtrVec ) delete vec;
}

/*
 *
 */
void RunAction::BeginOfRunAction(const G4Run* aRun){
  fTimer->Start();

}

/*
 *
 */
void RunAction::EndOfRunAction(const G4Run* aRun){
  fTimer->Stop();
  G4cout << "number of event = " << aRun->GetNumberOfEvent()
         << " " << *fTimer << G4endl;

  // Get analysis manager
  auto analysisManager = G4AnalysisManager::Instance();
  // save histograms & ntuple
  analysisManager->Write();
  analysisManager->CloseFile(false);
}
