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
  fPtrVec.push_back(&x);
  fPtrVec.push_back(&z);
  fPtrVec.push_back(&xHit);
  fPtrVec.push_back(&zHit);
  fPtrVec.push_back(&time);
  fPtrVec.push_back(&theta);

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

  for(uint i = 0; i < fPtrVec.size(); i++){
    fPtrVec.at(i)->clear();
  }
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
      for(int i = 0; i < HC->entries(); i++ ){
        PMTHit* aHit = (*HC)[i];
        // fill vectors //
        x.push_back    ( aHit->getPos().x() );
        z.push_back    ( aHit->getPos().z() );
        xHit.push_back ( aHit->getHit().x() );
        zHit.push_back ( aHit->getHit().z() );
        time.push_back ( aHit->getTime()    );
        G4double pTheta = atan(sqrt(pow(aHit->getMomentum().x(),2) + pow(aHit->getMomentum().z(),2) )/fabs(aHit->getMomentum().y() ));
        theta.push_back( pTheta );
      }
      // fill ntuple  //
      G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
      /* Not necessary if filling with vectors
      analysisManager->FillNtupleDColumn(0, x     );
      analysisManager->FillNtupleDColumn(1, z     );
      analysisManager->FillNtupleDColumn(2, xHit  );
      analysisManager->FillNtupleDColumn(3, zHit  );
      analysisManager->FillNtupleDColumn(4, time  );
      analysisManager->FillNtupleDColumn(5, theta );
      */
      analysisManager->AddNtupleRow();
    }
  }
}
