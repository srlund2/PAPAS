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

#include "G4VProcess.hh"

#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "G4SDManager.hh"
#include "G4Event.hh"
#include "G4Track.hh"
#include "G4ios.hh"

#include "PMTHit.hh"
#include "lgAnalysis.hh"

/*
 *
 */
EventAction::EventAction()
  : G4UserEventAction(),
    hitsCollID(0){

}

/*
 *
 */
EventAction::~EventAction(){

}

/*
 *
 */
void EventAction::BeginOfEventAction(const G4Event* event){
  fEventNo = event->GetEventID();
  if(fEventNo%100000 == 0) G4cout << "Begin Event " << fEventNo << G4endl;
  hitsCollID = 0;
}

/*
 *
 */
void EventAction::EndOfEventAction(const G4Event* event){
  hitsCollID = G4SDManager::GetSDMpointer()->GetCollectionID("MyPMT");
  G4HCofThisEvent* HCE = event->GetHCofThisEvent();
  if(HCE){
    HitsCollection* HC = (HitsCollection*)(HCE->GetHC(hitsCollID));
    if( HC->entries() ){
      PMTHit* aHit = (*HC)[0];
      // fill ntuple  //
      G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
      analysisManager->FillNtupleDColumn(0, aHit->getPos().x() );
      analysisManager->FillNtupleDColumn(1, aHit->getPos().y() );
      analysisManager->FillNtupleDColumn(2, aHit->getHit().x() );
      analysisManager->FillNtupleDColumn(3, aHit->getHit().y() );
      //analysisManager->FillNtupleDColumn(4, aHit->getEnergy()  );
      analysisManager->FillNtupleDColumn(4, aHit->getTime()  );
      analysisManager->AddNtupleRow();
    }
  }
}
