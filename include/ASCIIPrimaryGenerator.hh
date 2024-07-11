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
/// \file ASCIIPrimaryGenerator.hh
/// \brief Definition of the ASCIIPrimaryGenerator class
//
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef ASCIIPrimaryGenerator_h
#define ASCIIPrimaryGenerator_h 1

#include "G4VPrimaryGenerator.hh"

#include <fstream>
#include <vector>

class G4Event;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class ASCIIPrimaryGenerator : public G4VPrimaryGenerator
{
  public:
    ASCIIPrimaryGenerator();
   ~ASCIIPrimaryGenerator();

   virtual void  SetInputFile(G4String _name);
   virtual void  GetNextEvent( );
   inline  G4int GetnEvents(){return fnEvents;}

  public:
    virtual void GeneratePrimaryVertex(G4Event*);

  private:
    std::vector< G4ThreeVector >*   fPositionVec;
    std::vector< G4ThreeVector >*   fMomentumVec;
    std::vector< G4double >*        fEnergyVec;
    G4int                           fEventNo;
    G4int                           fnEvents = 0;
    std::ifstream                   fInputFile;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
