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
// ********************************************************************
//
// January 22, 2012
// Yakov Kulinich
// Stony Brook & BNL
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef Hit_h
#define Hit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "G4ParticleDefinition.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class Hit : public G4VHit
{
public:

  Hit();
  ~Hit();
  Hit(const Hit&);
  const Hit& operator=(const Hit&);
  G4int operator==(const Hit&) const;

  inline void* operator new(size_t);
  inline void  operator delete(void*);

  void Draw();
  void Print();

public:
  void setTrackID    (G4int track)        { trackID = track; }
  void setModNb      (G4int mod)          { modNb = mod; }
  void setRadNb      (G4int rad)          { radNb = rad; }
  void setRodNb      (G4int rod)          { rodNb = rod; }
  void setEdep       (G4double de)        { edep = de; }
  void setPos        (G4ThreeVector xyz)  { pos = xyz; }
  void setMomentum   (G4ThreeVector mom)  { momentum = mom;}
  void setParticle   (G4ParticleDefinition *part) {particle = part;}
  void setEnergy     (G4double e)         {energy = e;}
  void setCharge     (G4double c)         {charge = c;}
  void setNCherenkovs (G4int n)           {nCherenkovs = n;}
  G4int         getTrackID()    { return trackID; }
  G4int         getModNb()      { return modNb; }
  G4int         getRadNb()      { return radNb; }
  G4int         getRodNb()      { return rodNb; }
  G4int         getNCherenkovs()  { return nCherenkovs; }
  G4double      getEdep()       { return edep; }
  G4ThreeVector getPos()        { return pos; }
  G4ParticleDefinition*  getParticle()      { return particle; }
  G4double      getEnergy()       { return energy; }
  G4ThreeVector getMomentum()        { return momentum; }
  G4double      getCharge()     {return charge;}
  G4double      getVelocity()   {return velocity;}
  G4double      getBeta()   {return beta;}

private:
  G4int         trackID;
  G4int         modNb;
  G4int         radNb;
  G4int         rodNb;
  G4double      velocity;
  G4double      beta;
  G4int         nCherenkovs;
  G4double      edep;
  G4ThreeVector pos;
  G4ParticleDefinition *particle;
  G4ThreeVector momentum;
  G4double      energy;
  G4double      charge;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

typedef G4THitsCollection<Hit> HitsCollection;

extern G4Allocator<Hit> HitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void* Hit::operator new(size_t)
{
  void *aHit;
  aHit = (void *) HitAllocator.MallocSingle();
  return aHit;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void Hit::operator delete(void *aHit)
{
  HitAllocator.FreeSingle((Hit*) aHit);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
