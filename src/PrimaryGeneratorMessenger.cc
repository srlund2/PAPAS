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
/// \file /include/PrimaryGeneratorMessenger.cc
/// \brief Implementation of the PrimaryGeneratorMessenger class
//
//

#include "PrimaryGeneratorMessenger.hh"

#include <sstream>
#include <iostream>

#include "G4OpticalSurface.hh"

#include "PrimaryGeneratorAction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcommand.hh"
#include "G4UIparameter.hh"
#include "G4UIcmdWithAString.hh"


PrimaryGeneratorMessenger::PrimaryGeneratorMessenger(PrimaryGeneratorAction * Gen)
:G4UImessenger(),fGenerator(Gen),fGeneratorDir(0),fInputFileCmd(0)
{
  fGeneratorDir = new G4UIdirectory("/Input/");
  fGeneratorDir->SetGuidance("");

  fInputFileCmd = new G4UIcmdWithAString("/Input/FileName", this);
  fInputFileCmd->SetGuidance("Filepath of the previously generated events");
  fInputFileCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
  fInputFileCmd->SetToBeBroadcasted(false);

}

/*
 *
 */
PrimaryGeneratorMessenger::~PrimaryGeneratorMessenger(){
  delete fInputFileCmd;
}

/*
 *
 */
void PrimaryGeneratorMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{
  if(command == fInputFileCmd){
    fGenerator->SetInputFile(newValue);
  }
}
