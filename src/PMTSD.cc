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

#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
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
}


/* Mandatory method
 *
 */
G4bool PMTSD::ProcessHits(G4Step* aStep,G4TouchableHistory*){

  G4Track* theTrack = aStep->GetTrack();
  PMTHit* newHit = new PMTHit();

  newHit->setTrackID  (theTrack->GetTrackID() );
  newHit->setPos      (theTrack->GetVertexPosition() );
  newHit->setHit      (theTrack->GetPosition() );
  newHit->setEnergy   (aStep->GetPreStepPoint()->GetTotalEnergy());
  newHit->setMomentum (aStep->GetPreStepPoint()->GetMomentum());
  newHit->setTime     (theTrack->GetGlobalTime());

  hitCollection->insert( newHit );

  return true;
}

/* Mandatory method
 *
 */
void PMTSD::EndOfEvent(G4HCofThisEvent*){


}
