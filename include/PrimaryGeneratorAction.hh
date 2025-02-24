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
/// \file /include/PrimaryGeneratorAction.hh
/// \brief Definition of the PrimaryGeneratorAction class
//
#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h 1

#include "TFile.h"
#include "TTree.h"

#include "G4VUserPrimaryGeneratorAction.hh"
#include "PrimaryGeneratorMessenger.hh"
#include "ASCIIPrimaryGenerator.hh"

#include "G4GeneralParticleSource.hh"

#include <vector>

class G4GeneralParticleSource;
class G4Event;


class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:
    PrimaryGeneratorAction();
    virtual ~PrimaryGeneratorAction();

  public:
    virtual void  GeneratePrimaries(G4Event*);
    virtual void  GeneratePrimariesFromRootFile(G4Event*);
    virtual void  SetInputFile(G4String _name);
    inline  G4int GetnEvents(){return fnEvents;}

  private:
    G4GeneralParticleSource*        fParticleGun;
    ASCIIPrimaryGenerator*          fASCIIParticleGun;
    PrimaryGeneratorMessenger*      fMessenger;
    G4int                           fnEvents;
    G4bool                          fUseASCIIInput;
    G4bool                          fUseRootInput;
    TFile*                          fInputFile;
    TTree*                          fInputTree;
    std::vector<G4double>           *x, *z, *px, *py, *pz, *Energy, *time;
    G4int                           evNo;

};

#endif /*PrimaryGeneratorAction_h*/
