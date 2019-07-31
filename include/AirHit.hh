//
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
//
// @Author Chad Lantz

#ifndef AirHit_h
#define AirHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "G4ParticleDefinition.hh"

class AirHit : public G4VHit{
public:

  AirHit();
  ~AirHit();
  AirHit(const AirHit&);
  const AirHit& operator=(const AirHit&);
  G4int operator==(const AirHit&) const;

  inline void* operator new(size_t);
  inline void  operator delete(void*);

  void Draw();
  void Print();

public:
  void          setTrackID    (G4int track)       { trackID = track; }
  void          setPos        (G4ThreeVector xyz) { pos = xyz; }
  void          setHit        (G4ThreeVector xyz) { hit = xyz; }
  void          setMomentum   (G4ThreeVector mom) { momentum = mom; }
  void          setEnergy     (G4double e)        { energy = e; }
  G4int         getTrackID    ( )                 { return trackID; }
  G4ThreeVector getPos        ( )                 { return pos; }
  G4ThreeVector getHit        ( )                 { return hit; }
  G4double      getEnergy     ( )                 { return energy; }
  G4ThreeVector getMomentum   ( )                 { return momentum; }

private:
  G4int         trackID;
  G4ThreeVector pos;
  G4ThreeVector hit;
  G4double      energy;
  G4ThreeVector momentum;
};


///////////////////////// Hit collection definition /////////////////////////
typedef G4THitsCollection<AirHit> HitsCollection;

extern G4Allocator<AirHit> HitAllocator;

/*
 *
 */
inline void* AirHit::operator new(size_t)
{
  void *aHit;
  aHit = (void *) HitAllocator.MallocSingle();
  return aHit;
}

/*
 *
 */
inline void AirHit::operator delete(void *aHit)
{
  HitAllocator.FreeSingle((AirHit*) aHit);
}

#endif
