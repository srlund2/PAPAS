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
// @Author Chad Lantz
#include "EventAction.hh"

#include "G4AnalysisManager.hh"
#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4ios.hh"

#include "RunAction.hh"

/*
 *
 */
EventAction::EventAction()
  : G4UserEventAction()
{
}

/*
 *
 */
EventAction::~EventAction()
{
}

/*
 *
 */
void EventAction::BeginOfEventAction(const G4Event* event){

  if(event->GetEventID()%100000 == 0) G4cout << "Begin Event " << event->GetEventID() << G4endl;

}

/*
 *
 */
void EventAction::EndOfEventAction(const G4Event* event){
  // fill ntuple  //

  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

  RunAction *runAction = ((RunAction*)G4RunManager::GetRunManager()->GetUserRunAction());
  std::vector< std::vector<double>* > ptrVec = runAction->GetVectors();

  for(unsigned int i = 0; i < ptrVec[0]->size(); i++){
    analysisManager->FillH2(0, ptrVec[0]->at(i), ptrVec[1]->at(i));
  }

  analysisManager->FillNtupleIColumn( 0, event->GetEventID());
  analysisManager->AddNtupleRow();
  
  runAction->ClearVectors();
}
