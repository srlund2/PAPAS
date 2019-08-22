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
/// \file optical/OpNovice2/src/DetectorMessenger.cc
/// \brief Implementation of the DetectorMessenger class
//
//

#include "DetectorMessenger.hh"

#include <sstream>
#include <iostream>

#include "G4OpticalSurface.hh"

#include "DetectorConstruction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcommand.hh"
#include "G4UIparameter.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithoutParameter.hh"


DetectorMessenger::DetectorMessenger(DetectorConstruction * Det)
:G4UImessenger(),fDetector(Det)
{
  flightGuideDir = new G4UIdirectory("/lightGuide/");
  flightGuideDir->SetGuidance("Parameters for optical simulation.");

  fSurfaceTypeCmd = new G4UIcmdWithAString("/lightGuide/surfaceType", this);
  fSurfaceTypeCmd->SetGuidance("Surface type.");
  fSurfaceTypeCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
  fSurfaceTypeCmd->SetToBeBroadcasted(false);

  fSurfaceFinishCmd = new G4UIcmdWithAString("/lightGuide/surfaceFinish", this);
  fSurfaceFinishCmd->SetGuidance("Surface finish.");
  fSurfaceFinishCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
  fSurfaceFinishCmd->SetToBeBroadcasted(false);

  fSurfaceModelCmd =
    new G4UIcmdWithAString("/lightGuide/surfaceModel", this);
  fSurfaceModelCmd->SetGuidance("surface model.");
  fSurfaceModelCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
  fSurfaceModelCmd->SetToBeBroadcasted(false);

  fSurfaceSigmaAlphaCmd =
    new G4UIcmdWithADouble("/lightGuide/surfaceSigmaAlpha", this);
  fSurfaceSigmaAlphaCmd->SetGuidance("surface sigma alpha");
  fSurfaceSigmaAlphaCmd->SetGuidance(" parameter.");
  fSurfaceSigmaAlphaCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
  fSurfaceSigmaAlphaCmd->SetToBeBroadcasted(false);

  fSurfaceMatPropVectorCmd =
    new G4UIcmdWithAString("/lightGuide/surfaceProperty", this);
  fSurfaceMatPropVectorCmd->SetGuidance("Set material property vector");
  fSurfaceMatPropVectorCmd->SetGuidance(" for the surface.");
  fSurfaceMatPropVectorCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
  fSurfaceMatPropVectorCmd->SetToBeBroadcasted(false);

  fModelCmd = new G4UIcmdWithAString("/lightGuide/CADmodel", this);
  fModelCmd->SetGuidance("CAD model to be used");
  fModelCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
  fModelCmd->SetToBeBroadcasted(false);

  fModelRotationXCmd =
    new G4UIcmdWithAnInteger("/lightGuide/rotX", this);
  fModelRotationXCmd->SetGuidance("Set light guide first rotation");
  fModelRotationXCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
  fModelRotationXCmd->SetToBeBroadcasted(false);

  fModelRotationYCmd =
    new G4UIcmdWithAnInteger("/lightGuide/rotY", this);
  fModelRotationYCmd->SetGuidance("Set light guide second rotation");
  fModelRotationYCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
  fModelRotationYCmd->SetToBeBroadcasted(false);

  fModelRotationZCmd =
    new G4UIcmdWithAnInteger("/lightGuide/rotZ", this);
  fModelRotationZCmd->SetGuidance("Set light guide second rotation");
  fModelRotationZCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
  fModelRotationZCmd->SetToBeBroadcasted(false);

  fModelOffsetXCmd =
    new G4UIcmdWithADouble("/lightGuide/offsetX", this);
  fModelOffsetXCmd->SetGuidance("Set light guide X rotation");
  fModelOffsetXCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
  fModelOffsetXCmd->SetToBeBroadcasted(false);

  fModelOffsetYCmd =
    new G4UIcmdWithADouble("/lightGuide/offsetY", this);
  fModelOffsetYCmd->SetGuidance("Set light guide Y rotation");
  fModelOffsetYCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
  fModelOffsetYCmd->SetToBeBroadcasted(false);

  fModelOffsetZCmd =
    new G4UIcmdWithADouble("/lightGuide/offsetZ", this);
  fModelOffsetZCmd->SetGuidance("Set light guide Z rotation");
  fModelOffsetZCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
  fModelOffsetZCmd->SetToBeBroadcasted(false);
}

/*
 *
 */
DetectorMessenger::~DetectorMessenger(){
  delete fSurfaceFinishCmd;
  delete fSurfaceTypeCmd;
  delete fSurfaceModelCmd;
  delete fSurfaceSigmaAlphaCmd;
  delete fSurfaceMatPropVectorCmd;
  delete fModelCmd;
  delete fModelRotationXCmd;
  delete fModelRotationYCmd;
  delete fModelRotationZCmd;
  delete fModelOffsetXCmd;
  delete fModelOffsetYCmd;
  delete fModelOffsetZCmd;
}

/*
 *
 */
void DetectorMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{
  //    FINISH
  if (command == fSurfaceFinishCmd) {
    if (newValue == "polished") {
      fDetector->SetSurfaceFinish(polished);
    }
    else if (newValue == "polishedfrontpainted") {
      fDetector->SetSurfaceFinish(polishedfrontpainted);
    }
    else if (newValue == "polishedbackpainted") {
      fDetector->SetSurfaceFinish(polishedbackpainted);
    }
    else if (newValue == "ground") {
      fDetector->SetSurfaceFinish(ground);
    }
    else if (newValue == "groundfrontpainted") {
      fDetector->SetSurfaceFinish(groundfrontpainted);
    }
    else if (newValue == "groundbackpainted") {
      fDetector->SetSurfaceFinish(groundbackpainted);
    }
    else if (newValue == "polishedlumirrorair") {
      fDetector->SetSurfaceFinish(polishedlumirrorair);
    }
    else if (newValue == "polishedlumirrorglue") {
      fDetector->SetSurfaceFinish(polishedlumirrorglue);
    }
    else if (newValue == "polishedair") {
      fDetector->SetSurfaceFinish(polishedair);
    }
    else if (newValue == "polishedteflonair") {
      fDetector->SetSurfaceFinish(polishedteflonair);
    }
    else if (newValue == "polishedtioair") {
      fDetector->SetSurfaceFinish(polishedtioair);
    }
    else if (newValue == "polishedtyvekair") {
      fDetector->SetSurfaceFinish(polishedtyvekair);
    }
    else if (newValue == "polishedvm2000air") {
      fDetector->SetSurfaceFinish(polishedvm2000air);
    }
    else if (newValue == "polishedvm2000glue") {
      fDetector->SetSurfaceFinish(polishedvm2000glue);
    }
    else if (newValue == "etchedlumirrorair") {
      fDetector->SetSurfaceFinish(etchedlumirrorair);
    }
    else if (newValue == "etchedlumirrorglue") {
      fDetector->SetSurfaceFinish(etchedlumirrorglue);
    }
    else if (newValue == "etchedair") {
      fDetector->SetSurfaceFinish(etchedair);
    }
    else if (newValue == "etchedteflonair") {
      fDetector->SetSurfaceFinish(etchedteflonair);
    }
    else if (newValue == "etchedtioair") {
      fDetector->SetSurfaceFinish(etchedtioair);
    }
    else if (newValue == "etchedtyvekair") {
      fDetector->SetSurfaceFinish(etchedtyvekair);
    }
    else if (newValue == "etchedvm2000air") {
      fDetector->SetSurfaceFinish(etchedvm2000air);
    }
    else if (newValue == "etchedvm2000glue") {
      fDetector->SetSurfaceFinish(etchedvm2000glue);
    }
    else if (newValue == "groundlumirrorair") {
      fDetector->SetSurfaceFinish(groundlumirrorair);
    }
    else if (newValue == "groundlumirrorglue") {
      fDetector->SetSurfaceFinish(groundlumirrorglue);
    }
    else if (newValue == "groundair") {
      fDetector->SetSurfaceFinish(groundair);
    }
    else if (newValue == "groundteflonair") {
      fDetector->SetSurfaceFinish(groundteflonair);
    }
    else if (newValue == "groundtioair") {
      fDetector->SetSurfaceFinish(groundtioair);
    }
    else if (newValue == "groundtyvekair") {
      fDetector->SetSurfaceFinish(groundtyvekair);
    }
    else if (newValue == "groundvm2000air") {
      fDetector->SetSurfaceFinish(groundvm2000air);
    }
    else if (newValue == "groundvm2000glue") {
      fDetector->SetSurfaceFinish(groundvm2000glue);
    }
    //         for Davis model
    else if (newValue == "Rough_LUT") {
      fDetector->SetSurfaceFinish(Rough_LUT);
    }
    else if (newValue == "RoughTeflon_LUT") {
      fDetector->SetSurfaceFinish(RoughTeflon_LUT);
    }
    else if (newValue == "RoughESR_LUT") {
      fDetector->SetSurfaceFinish(RoughESR_LUT);
    }
    else if (newValue == "RoughESRGrease_LUT") {
      fDetector->SetSurfaceFinish(RoughESRGrease_LUT);
    }
    else if (newValue == "Polished_LUT") {
      fDetector->SetSurfaceFinish(Polished_LUT);
    }
    else if (newValue == "PolishedTeflon_LUT") {
      fDetector->SetSurfaceFinish(PolishedTeflon_LUT);
    }
    else if (newValue == "PolishedESR_LUT") {
      fDetector->SetSurfaceFinish(PolishedESR_LUT);
    }
    else if (newValue == "PolishedESRGrease_LUT") {
      fDetector->SetSurfaceFinish(PolishedESRGrease_LUT);
    }
    else if (newValue == "Detector_LUT") {
      fDetector->SetSurfaceFinish(Detector_LUT);
    }
    else {
      G4ExceptionDescription ed;
      ed << "Invalid surface finish: " << newValue;
      G4Exception("DetectorMessenger", "OpNovice2_003", FatalException,ed);
    }
  }

  //  MODEL
  else if (command == fSurfaceModelCmd) {
    if (newValue == "glisur") {
      fDetector->SetSurfaceModel(glisur);
    }
    else if (newValue == "unified") {
      fDetector->SetSurfaceModel(unified);
    }
    else if (newValue == "LUT") {
      fDetector->SetSurfaceModel(LUT);
    }
    else if (newValue == "DAVIS") {
      fDetector->SetSurfaceModel(DAVIS);
    }
    else if (newValue == "dichroic") {
      fDetector->SetSurfaceModel(dichroic);
    }
    else {
      G4ExceptionDescription ed;
      ed << "Invalid surface model: " << newValue;
      G4Exception("DetectorMessenger", "ONovice2_001",
                  FatalException,ed);
    }
  }

  // TYPE
  else if (command == fSurfaceTypeCmd) {
    if (newValue == "dielectric_metal") {
      fDetector->SetSurfaceType(dielectric_metal);
    }
    else if (newValue == "dielectric_dielectric") {
      fDetector->SetSurfaceType(dielectric_dielectric);
    }
    else if (newValue == "dielectric_LUT") {
      fDetector->SetSurfaceType(dielectric_LUT);
    }
    else if (newValue == "dielectric_LUTDAVIS") {
      fDetector->SetSurfaceType(dielectric_LUTDAVIS);
    }
    else {
      G4ExceptionDescription ed;
      ed << "Invalid surface type: " << newValue;
      G4Exception("DetectorMessenger", "OpNovice2_002", FatalException,ed);
    }
  }
  else if (command == fSurfaceSigmaAlphaCmd) {
    fDetector->SetSurfaceSigmaAlpha(
      G4UIcmdWithADouble::GetNewDoubleValue(newValue));
  }
  // MODEL LOCATION
  else if(command == fModelCmd){
    fDetector->SetCADFilename(newValue);
  }
  // MODEL ROTATION
  else if(command == fModelRotationXCmd){
    fDetector->SetRotationX(G4UIcmdWithAnInteger::GetNewIntValue(newValue));
  }
  else if(command == fModelRotationYCmd){
    fDetector->SetRotationY(G4UIcmdWithAnInteger::GetNewIntValue(newValue));
  }
  else if(command == fModelRotationZCmd){
    fDetector->SetRotationZ(G4UIcmdWithAnInteger::GetNewIntValue(newValue));
  }
  // MODEL TRANSLATION
  else if(command == fModelOffsetXCmd){
    fDetector->SetOffsetX(G4UIcmdWithADouble::GetNewDoubleValue(newValue));
  }
  else if(command == fModelOffsetYCmd){
    fDetector->SetOffsetY(G4UIcmdWithADouble::GetNewDoubleValue(newValue));
  }
  else if(command == fModelOffsetZCmd){
    fDetector->SetOffsetZ(G4UIcmdWithADouble::GetNewDoubleValue(newValue));
  }
}
