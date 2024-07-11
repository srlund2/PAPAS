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
/// \file ASCIIPrimaryGenerator.cc
/// \brief Implementation of the PrimaryGenerator1 class
//
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "ASCIIPrimaryGenerator.hh"

#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4PrimaryParticle.hh"
#include "G4PrimaryVertex.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"

/*
 *
 */
ASCIIPrimaryGenerator::ASCIIPrimaryGenerator()
: G4VPrimaryGenerator(),fEventNo(0)
{
  fPositionVec = new std::vector< G4ThreeVector >;
  fMomentumVec = new std::vector< G4ThreeVector >;
  fEnergyVec   = new std::vector< G4double >;
}


/*
 *
 */
ASCIIPrimaryGenerator::~ASCIIPrimaryGenerator()
{ }


/*
 *
 */
void ASCIIPrimaryGenerator::GeneratePrimaryVertex(G4Event* event)
{
  G4ParticleDefinition* particleDefinition=
      G4ParticleTable::GetParticleTable()->FindParticle("opticalphoton");

  std::vector< G4PrimaryVertex* > vertexVec;
  std::vector< G4PrimaryParticle* > particleVec;

  fPositionVec->clear();
  fMomentumVec->clear();
  fEnergyVec->clear();

  GetNextEvent();

  //For each photon read in, add a new particle and vertex to the event
  int nPhotons = fPositionVec->size();
  for(int i = 0; i < nPhotons; i++){
    particleVec.push_back( new G4PrimaryParticle(particleDefinition) );
    particleVec.back()->SetMomentumDirection( fMomentumVec->at(i) );
    particleVec.back()->SetKineticEnergy( fEnergyVec->at(i)*eV );

    vertexVec.push_back( new G4PrimaryVertex( fPositionVec->at(i) , 0*s ) );
    vertexVec.back()->SetPrimary( particleVec.back() );
    event->AddPrimaryVertex( vertexVec.back() );
  }
}


/*
 * Fill member vectors with position, momentum and energy of all
 * photons in the next event stored in the input ascii file
 */
void ASCIIPrimaryGenerator::GetNextEvent(){
  std::string str;
  double x,y,z,px,py,pz,energy;

  // The first line of each event should be "Event #"
  // If it's not, there's an error in the file formatting
  // or we've reached the end of the file
  getline(fInputFile,str);
  if(str[0] == 'E'){
    fEventNo = std::atoi( str.substr( 6, str.size() ).c_str() );
  }else{
    G4cout << "Error reading input event. Searching for next event..." << G4endl;
    while( !fInputFile.eof() ){
      getline(fInputFile,str);
      if(str[0] == 'E'){
        fEventNo = std::atoi( str.substr( 6, str.size() ).c_str() );
        break;
      }
    }
    if( fInputFile.eof() ){
      G4cout << "Reached end of input file" << G4endl;
      fEventNo = -1;
      return;
    }
  }

  // Loop through the event and read all photons.
  // Format is:
  // (Vertex)   V, x,   y,  z
  // (Momentum) P, px, py, pz, energy
  // Then push all of those to their respective member vectors
  getline(fInputFile,str);
  while( !fInputFile.eof() ){
    //
    //Get Vertex
    if(str[0] != 'V'){
      G4cout << "Error reading input vertex at position " << fInputFile.tellg() << G4endl;
      return;
    }
    x = std::atof( str.substr(2,19).c_str()  );
    y = std::atof( str.substr(20,37).c_str() );
    z = std::atof( str.substr(38,55).c_str() );
    fPositionVec->push_back( G4ThreeVector(x*mm,y*mm,z*mm) );

    //
    //Get momentum and energy
    getline(fInputFile,str);
    if(str[0] != 'P'){
      G4cout << "Error reading input momentum at position " << fInputFile.tellg() << G4endl;
      return;
    }
    px = std::atof( str.substr(2,19).c_str()  );
    py = std::atof( str.substr(20,37).c_str() );
    pz = std::atof( str.substr(38,55).c_str() );
    fMomentumVec->push_back( G4ThreeVector(px,py,pz) );
    energy = std::atof( str.substr(56,73).c_str() );
    fEnergyVec->push_back( energy );

    //
    //The next line is either another Vertex, or the end of the event
    getline(fInputFile,str);
    if( str[0] == 'E' ) return;
  }
}

/*
 * Open the input file and get the total number of events from it
 */
void ASCIIPrimaryGenerator::SetInputFile(G4String _name){
  std::string str;
  fInputFile.open( _name.c_str() );

  getline(fInputFile,str);
  fnEvents = std::atoi( str.substr(15,str.size() ).c_str() );
}
