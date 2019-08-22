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


// Make this appear first!
#include "G4Timer.hh"

#include "G4RunManager.hh"
#include "G4Run.hh"

#include "RunAction.hh"
#include "G4VAnalysisManager.hh"
#include "G4SystemOfUnits.hh"
#include "lgAnalysis.hh"


/*
 *
 */
RunAction::RunAction(G4String fileName)
 : G4UserRunAction(),ffileName(fileName),
   fTimer(0){
  fTimer = new G4Timer;

  G4RunManager::GetRunManager()->SetPrintProgress(1);

  // Create analysis manager. The choice of analysis
  // technology is done via selectin of a namespace
  // in B4Analysis.hh
  auto analysisManager = G4AnalysisManager::Instance();
  G4cout << "Using " << analysisManager->GetType() << G4endl;

  // Create directories
  analysisManager->SetVerboseLevel(1);
  analysisManager->SetNtupleMerging(true);

  // Creating ntuple
  analysisManager->CreateNtuple("lightGuide", "pos and momentum");
  analysisManager->CreateNtupleDColumn("X");
  analysisManager->CreateNtupleDColumn("Y");
  analysisManager->CreateNtupleDColumn("hitX");
  analysisManager->CreateNtupleDColumn("hitY");
  analysisManager->CreateNtupleDColumn("time");
  analysisManager->FinishNtuple();
  analysisManager->SetFirstHistoId(1);
  analysisManager->CreateH2("1","Relative Efficiency",
                            150, -82.0*mm, 82.0*mm,
                            150, -44.875*mm, 44.875*mm);
}

/*
 *
 */
RunAction::~RunAction(){
  delete fTimer;
  delete G4AnalysisManager::Instance();
}

/*
 *
 */
void RunAction::BeginOfRunAction(const G4Run* aRun){
  G4cout << "### Run " << aRun->GetRunID() << " start." << G4endl;
  fTimer->Start();

  // Get analysis manager
  auto analysisManager = G4AnalysisManager::Instance();

  // Open an output file
  if(ffileName == "") ffileName = "output";
  analysisManager->OpenFile(ffileName);
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
  analysisManager->CloseFile();
}
