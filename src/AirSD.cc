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

#include "AirSD.hh"

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
FiberSD::FiberSD(G4String sdName, G4int modNum)
  :G4VSensitiveDetector(sdName) m_modNum(modNum) {
  collectionName.insert(sdName);
  HCID = -1;

}


/*
 *
 */
FiberSD::~FiberSD(){ }


/*
 *
 */
void FiberSD::HistInitialize(){
  std::string name = GetName();
}


/*
 *
 */
void FiberSD::Initialize(G4HCofThisEvent* HCE){

  fiberCollection = new FiberHitsCollection(SensitiveDetectorName,
					      m_modNum);

  std::string name = collectionName[0];

  if(HCID<0)
    { HCID = G4SDManager::GetSDMpointer()->GetCollectionID( name );}

  HCE->AddHitsCollection( HCID, fiberCollection );
  G4cout << " HCID " << HCID << " name " << name << G4endl;
}


/*
 *
 */
G4bool FiberSD::ProcessHits(G4Step* aStep,G4TouchableHistory*){

  G4Track* theTrack = aStep->GetTrack();

  G4double energy = aStep->GetPreStepPoint()->GetTotalEnergy();
  G4ThreeVector momentum = aStep->GetPreStepPoint()->GetMomentum();
  G4ParticleDefinition *particle = aStep->GetTrack()->GetDefinition();
  G4double charge = aStep->GetPreStepPoint()->GetCharge();

  FiberHit* newHit = new FiberHit();

  newHit->setTrackID       (aStep->GetTrack()->GetTrackID() );
  newHit->setPos           (aStep->GetTrack()->GetVertexPosition() );
  newHit->setEnergy        (energy);
  newHit->setMomentum      (momentum);

  fiberCollection->insert (newHit );


  return true;
}

/*
 *
 */
void FiberSD::EndOfEvent(G4HCofThisEvent*){


}
