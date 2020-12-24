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

#include "PMTSD.hh"
#include "PMTHit.hh"
#include "RunAction.hh"

#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4ios.hh"
#include "G4Poisson.hh"
#include "G4ParticleTypes.hh"
#include "G4ParticleDefinition.hh"

#include "TString.h"

#include <string>
#include <iostream>
#include <cmath>


/*
 *
 */
PMTSD::PMTSD(G4String sdName)
  :G4VSensitiveDetector(sdName){
  collectionName.insert(sdName);
  HCID = -1;
  VISUALIZE = false;

}


/*
 *
 */
PMTSD::~PMTSD(){

}


/* Mandatory method
 *
 */
void PMTSD::Initialize(G4HCofThisEvent* HCE){

  hitCollection = new HitsCollection(GetName(), collectionName[0]);

  std::string name = GetName();

  if(HCID<0)
    { HCID = G4SDManager::GetSDMpointer()->GetCollectionID( name );}

  HCE->AddHitsCollection( HCID, hitCollection );

  // If the UI window exists, set visualize to true so we record and draw hits
  if( G4UImanager::GetUIpointer()->GetG4UIWindow() ) VISUALIZE = true;

  // Get the vectors from run action
  RunAction* runAction = (RunAction*)G4RunManager::GetRunManager()->GetUserRunAction();
  fPtrVec = runAction->GetVectors();

}


/* Mandatory method
 *
 */
G4bool PMTSD::ProcessHits(G4Step* aStep,G4TouchableHistory*){
  G4Track* theTrack = aStep->GetTrack();

  G4ThreeVector origin = theTrack->GetVertexPosition();
  G4ThreeVector hitPos = theTrack->GetPosition();
  G4ThreeVector momentum = aStep->GetPreStepPoint()->GetMomentum();

  //Display hits if the UI is on
  if(VISUALIZE){
    PMTHit* newHit = new PMTHit();
    newHit->setPos( theTrack->GetPosition() );
    hitCollection->insert ( newHit );
  }

  // Fill ouput vectors
  // These vectors are created in RunAction
  fPtrVec[0]->push_back( origin.x() );  //Origin of the photon in x
  fPtrVec[1]->push_back( origin.z() );  //Origin of the photon in z
  fPtrVec[2]->push_back( hitPos.x() );  //Where the photon landed on the PMT in x
  fPtrVec[3]->push_back( hitPos.z() );  //Where the photon landed on the PMT in z
  fPtrVec[4]->push_back( theTrack->GetGlobalTime() );  //Time of arrival of the photon at the PMT
  G4double pTheta = atan(sqrt(pow(momentum.x(),2) + pow(momentum.z(),2) )/fabs(momentum.y() ));
  fPtrVec[5]->push_back( pTheta );  //Incident angle at the PMT about the y-axis
  fPtrVec[6]->push_back( aStep->GetPreStepPoint()->GetTotalEnergy() ); //Photon energy

  return true;
}

/* Mandatory method
 *
 */
void PMTSD::EndOfEvent(G4HCofThisEvent*){


}
