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
/// \file /src/PrimaryGeneratorAction.cc
/// \brief Implementation of the PrimaryGeneratorAction class
#include "PrimaryGeneratorAction.hh"

#include "G4Event.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include <G4String.hh>



/*
 *
 */
PrimaryGeneratorAction::PrimaryGeneratorAction()
 : G4VUserPrimaryGeneratorAction()
{
  fParticleGun      = new G4GeneralParticleSource();
  fASCIIParticleGun = new ASCIIPrimaryGenerator();
  fMessenger        = new PrimaryGeneratorMessenger( this );

  fUseRootInput = fUseASCIIInput = false;
  x = z = px = py = pz = Energy = time = 0;
  evNo = 0;
}

/*
 *
 */
PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  delete fParticleGun;
  delete fASCIIParticleGun;
  delete fMessenger;
}

/*
 *
 */
void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  if(fUseASCIIInput){
    fASCIIParticleGun->GeneratePrimaryVertex(anEvent);
  }else if(fUseRootInput){
    GeneratePrimariesFromRootFile(anEvent);
  }else{
    fParticleGun->GeneratePrimaryVertex(anEvent);
  }
}

/*
 *
 */
void PrimaryGeneratorAction::GeneratePrimariesFromRootFile(G4Event* anEvent){
  G4ParticleDefinition* particleDefinition=
      G4ParticleTable::GetParticleTable()->FindParticle("opticalphoton");

  fInputTree->GetEntry(evNo);

  G4int nPhotons = x->size();
  for(G4int i = 0; i < nPhotons; i++){
     G4PrimaryParticle* particle = new G4PrimaryParticle(particleDefinition);
     particle->SetMomentum( px->at(i)*MeV, py->at(i)*MeV, pz->at(i)*MeV );

     G4PrimaryVertex* vert = new G4PrimaryVertex( x->at(i)*mm, -0.1*mm, z->at(i)*mm , time->at(i)*s );
     vert->SetPrimary( particle );
     anEvent->AddPrimaryVertex( vert );
  }
  evNo++;
}

void PrimaryGeneratorAction::SetInputFile(G4String _name){
  if(fUseRootInput || fUseASCIIInput){
    G4cerr << "WARNING: Input file defined twice. Using first definition." << G4endl;
    return;
  }
  G4String check = _name;
  check = G4StrUtil::to_lower_copy(check);
  if(G4StrUtil::contains(check,".root")){
    fInputFile = new TFile( _name.c_str(), "READ");

    if(!fInputFile->IsOpen()){
      G4cerr << _name << " could not be opened" << G4endl;
      delete fInputFile;
      return;
    }

    G4cout << "Using " << _name << " as source" << G4endl;

    fUseRootInput = true;

    fInputTree = (TTree*)fInputFile->Get("ZDCtree");
    fnEvents = fInputTree->GetEntries();

    fInputTree->SetBranchAddress("X",&x);
    fInputTree->SetBranchAddress("Z",&z);
    fInputTree->SetBranchAddress("Px",&px);
    fInputTree->SetBranchAddress("Py",&py);
    fInputTree->SetBranchAddress("Pz",&pz);
    fInputTree->SetBranchAddress("Energy",&Energy);
    fInputTree->SetBranchAddress("Time",&time);

  }else if(G4StrUtil::contains(check,".txt")){
    fUseASCIIInput = true;

    G4cout << "Using " << _name << " as source" << G4endl;
    fASCIIParticleGun->SetInputFile(_name);
    fnEvents = fASCIIParticleGun->GetnEvents();
  }
}
