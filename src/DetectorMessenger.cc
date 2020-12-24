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
#include "G4UIcmdWith3VectorAndUnit.hh"


DetectorMessenger::DetectorMessenger(DetectorConstruction * Det)
:G4UImessenger(),fDetector(Det)
{
  //Directories
  flightGuideDir = new G4UIdirectory("/lightGuide/");
  flightGuideDir->SetGuidance("Parameters for optical simulation.");

  fSurfaceDir = new G4UIdirectory("/lightGuide/surface/");
  fSurfaceDir->SetGuidance("Surface parameters for optical simulation.");

  fModelDir = new G4UIdirectory("/lightGuide/model/");
  fModelDir->SetGuidance("Model source, translation and rotation");

  fWorldVolumeCmd =
  new G4UIcmdWith3VectorAndUnit("/lightGuide/worldVolume", this);
  fWorldVolumeCmd->SetGuidance("Set the size of the world volume");
  fWorldVolumeCmd->AvailableForStates(G4State_PreInit, G4State_Init, G4State_Idle);
  fWorldVolumeCmd->SetToBeBroadcasted(false);
  fWorldVolumeCmd->SetParameterName("X","Y","Z",true);
  fWorldVolumeCmd->SetDefaultValue(G4ThreeVector(0.25,0.25,0.5));
  fWorldVolumeCmd->SetDefaultUnit("m");

  fEnvelopeCmd =
  new G4UIcmdWith3VectorAndUnit("/lightGuide/envelope", this);
  fEnvelopeCmd->SetGuidance("Set the size light guide envelope");
  fEnvelopeCmd->AvailableForStates(G4State_PreInit, G4State_Init, G4State_Idle);
  fEnvelopeCmd->SetToBeBroadcasted(false);
  fEnvelopeCmd->SetParameterName("X","Y","Z",true);
  fEnvelopeCmd->SetDefaultValue(G4ThreeVector(89.75*CLHEP::mm,113.*CLHEP::mm,339.*CLHEP::mm));
  fEnvelopeCmd->SetDefaultUnit("m");

  //Model commands
  fModelCmd = new G4UIcmdWithAString("/lightGuide/model/CADmodel", this);
  fModelCmd->SetGuidance("CAD model to be used");
  fModelCmd->AvailableForStates(G4State_PreInit, G4State_Init, G4State_Idle);
  fModelCmd->SetToBeBroadcasted(false);
  fModelCmd->SetDefaultValue("models/Winston_cone.stl");

  fModelRotationCmd =
    new G4UIcmdWith3VectorAndUnit("/lightGuide/model/rotate", this);
  fModelRotationCmd->SetGuidance("Set light guide first rotation");
  fModelRotationCmd->AvailableForStates(G4State_PreInit, G4State_Init, G4State_Idle);
  fModelRotationCmd->SetToBeBroadcasted(false);
  fModelRotationCmd->SetParameterName("rotationX","rotationY","rotationZ",true);
  fModelRotationCmd->SetDefaultValue(G4ThreeVector(0.,0.,0.));
  fModelRotationCmd->SetDefaultUnit("deg");

  fModelTranslationCmd =
    new G4UIcmdWith3VectorAndUnit("/lightGuide/model/translate", this);
  fModelTranslationCmd->SetGuidance("Set light guide translation");
  fModelTranslationCmd->AvailableForStates(G4State_PreInit, G4State_Init, G4State_Idle);
  fModelTranslationCmd->SetToBeBroadcasted(false);
  fModelTranslationCmd->SetParameterName("X","Y","Z",true);
  fModelTranslationCmd->SetDefaultValue(G4ThreeVector(0.,0.,0.));
  fModelTranslationCmd->SetDefaultUnit("mm");

  fPMTTranslationCmd =
    new G4UIcmdWith3VectorAndUnit("/lightGuide/model/translatePMT", this);
  fPMTTranslationCmd->SetGuidance("Set PMT translation");
  fPMTTranslationCmd->AvailableForStates(G4State_PreInit, G4State_Init, G4State_Idle);
  fPMTTranslationCmd->SetToBeBroadcasted(false);
  fPMTTranslationCmd->SetParameterName("X","Y","Z",true);
  fPMTTranslationCmd->SetDefaultValue(G4ThreeVector(0.,0.,0.));
  fPMTTranslationCmd->SetDefaultUnit("mm");

  fPMTDiameterCmd =
    new G4UIcmdWithADoubleAndUnit("/lightGuide/model/PMTDiameter",this);
  fPMTDiameterCmd->SetGuidance("Set PMT diameter");
  fPMTDiameterCmd->AvailableForStates(G4State_PreInit, G4State_Init, G4State_Idle);
  fPMTDiameterCmd->SetToBeBroadcasted(false);
  fPMTDiameterCmd->SetParameterName("dia",true);
  fPMTDiameterCmd->SetDefaultValue( 65.0*CLHEP::mm );
  fPMTDiameterCmd->SetDefaultUnit("mm");

  fLGThicknessCmd =
    new G4UIcmdWithADoubleAndUnit("/lightGuide/model/SkinThickness",this);
  fLGThicknessCmd->SetGuidance("Set thickness of the light guide skin");
  fLGThicknessCmd->AvailableForStates(G4State_PreInit, G4State_Init, G4State_Idle);
  fLGThicknessCmd->SetToBeBroadcasted(false);
  fLGThicknessCmd->SetParameterName("thickness",true);
  fLGThicknessCmd->SetDefaultValue( 1.0*CLHEP::mm );
  fLGThicknessCmd->SetDefaultUnit("mm");

  fOutputModelCmd = new G4UIcmdWithAString("/lightGuide/model/OutputModel",this);
  fOutputModelCmd->SetGuidance("Creates a .gdml file of the light guide with the given name");
  fOutputModelCmd->AvailableForStates(G4State_PreInit, G4State_Init, G4State_Idle);
  fOutputModelCmd->SetToBeBroadcasted(false);

  fNsegmentsXCmd =
    new G4UIcmdWithAnInteger("/lightGuide/model/nSegmentsX",this);
  fNsegmentsXCmd->SetGuidance("Set segmentation of the light guide envelope in x");
  fNsegmentsXCmd->AvailableForStates(G4State_PreInit, G4State_Init, G4State_Idle);
  fNsegmentsXCmd->SetToBeBroadcasted(false);
  fNsegmentsXCmd->SetParameterName("nSegmentsX",true);
  fNsegmentsXCmd->SetDefaultValue( 1 );

  fNsegmentsZCmd =
    new G4UIcmdWithAnInteger("/lightGuide/model/nSegmentsZ",this);
  fNsegmentsZCmd->SetGuidance("Set segmentation of the light guide envelope in z");
  fNsegmentsZCmd->AvailableForStates(G4State_PreInit, G4State_Init, G4State_Idle);
  fNsegmentsZCmd->SetToBeBroadcasted(false);
  fNsegmentsZCmd->SetParameterName("nSegmentsZ",true);
  fNsegmentsZCmd->SetDefaultValue( 1 );

  //Surface commands
  fSurfaceTypeCmd = new G4UIcmdWithAString("/lightGuide/surface/Type", this);
  fSurfaceTypeCmd->SetGuidance("Surface type.");
  fSurfaceTypeCmd->AvailableForStates(G4State_PreInit, G4State_Init, G4State_Idle);
  fSurfaceTypeCmd->SetToBeBroadcasted(false);

  fSurfaceFinishCmd = new G4UIcmdWithAString("/lightGuide/surface/Finish", this);
  fSurfaceFinishCmd->SetGuidance("Surface finish.");
  fSurfaceFinishCmd->AvailableForStates(G4State_PreInit, G4State_Init, G4State_Idle);
  fSurfaceFinishCmd->SetToBeBroadcasted(false);

  fSurfaceModelCmd =
    new G4UIcmdWithAString("/lightGuide/surface/Model", this);
  fSurfaceModelCmd->SetGuidance("surface model.");
  fSurfaceModelCmd->AvailableForStates(G4State_PreInit, G4State_Init, G4State_Idle);
  fSurfaceModelCmd->SetToBeBroadcasted(false);

  fSurfaceSigmaAlphaCmd =
    new G4UIcmdWithADouble("/lightGuide/surface/SigmaAlpha", this);
  fSurfaceSigmaAlphaCmd->SetGuidance("surface sigma alpha");
  fSurfaceSigmaAlphaCmd->SetGuidance(" parameter.");
  fSurfaceSigmaAlphaCmd->AvailableForStates(G4State_PreInit, G4State_Init, G4State_Idle);
  fSurfaceSigmaAlphaCmd->SetToBeBroadcasted(false);

  fSurfaceMatPropVectorCmd =
    new G4UIcmdWithAString("/lightGuide/surface/Property", this);
  fSurfaceMatPropVectorCmd->SetGuidance("Set material property vector");
  fSurfaceMatPropVectorCmd->SetGuidance(" for the surface.");
  fSurfaceMatPropVectorCmd->AvailableForStates(G4State_PreInit, G4State_Init, G4State_Idle);
  fSurfaceMatPropVectorCmd->SetToBeBroadcasted(false);

  //Gas commands
  fGasPropVectorCmd =
    new G4UIcmdWithAString("/lightGuide/gasProperty", this);
  fGasPropVectorCmd->SetGuidance("Set fill gas property vector");
  fGasPropVectorCmd->AvailableForStates(G4State_PreInit, G4State_Init, G4State_Idle);
  fGasPropVectorCmd->SetToBeBroadcasted(false);

}

/*
 *
 */
DetectorMessenger::~DetectorMessenger(){
  delete fModelCmd;
  delete fWorldVolumeCmd;
  delete fEnvelopeCmd;
  delete fModelRotationCmd;
  delete fModelTranslationCmd;
  delete fPMTTranslationCmd;
  delete fPMTDiameterCmd;
  delete fLGThicknessCmd;
  delete fOutputModelCmd;
  delete fNsegmentsXCmd;
  delete fNsegmentsZCmd;
  delete fSurfaceModelCmd;
  delete fSurfaceFinishCmd;
  delete fSurfaceTypeCmd;
  delete fSurfaceSigmaAlphaCmd;
  delete fSurfaceMatPropVectorCmd;
  delete fGasPropVectorCmd;
}

/*
 *
 */
void DetectorMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{
  // MODEL FILE
  if(command == fModelCmd){
    fDetector->UseCADModel(newValue);
  }
  // WORLD VOLUME
  else if(command == fWorldVolumeCmd){
    fDetector->SetWorldVolume(fWorldVolumeCmd->GetNew3VectorValue(newValue));
  }
  // LIGHT GUIDE ENVELOPE
  else if(command == fEnvelopeCmd){
    fDetector->SetEnvelope(fEnvelopeCmd->GetNew3VectorValue(newValue));
  }
  // MODEL ROTATION
  else if(command == fModelRotationCmd){
    fDetector->SetRotation(fModelRotationCmd->GetNew3VectorValue(newValue));
  }
  // MODEL TRANSLATION
  else if(command == fModelTranslationCmd){
    fDetector->SetTranslation(fModelTranslationCmd->GetNew3VectorValue(newValue));
  }
  // PMT TRANSLATION
  else if(command == fPMTTranslationCmd){
    fDetector->SetPMTTranslation(fPMTTranslationCmd->GetNew3VectorValue(newValue));
  }
  // PMT DIAMETER
  else if(command == fPMTDiameterCmd){
    fDetector->SetPMTDiameter(fPMTDiameterCmd->GetNewDoubleValue(newValue));
  }
  // PMT DIAMETER
  else if(command == fLGThicknessCmd){
    fDetector->SetLGthickness(fLGThicknessCmd->GetNewDoubleValue(newValue));
  }
  // OUTPUT GEOMETRY TO GDML
  else if(command == fOutputModelCmd){
    fDetector->OutputToGDML(newValue);
  }
  // X SEGMENTATION
  else if(command == fNsegmentsXCmd){
    fDetector->SetNSegmentsX(fNsegmentsXCmd->GetNewIntValue(newValue));
  }
  // Z SEGMENTATION
  else if(command == fNsegmentsZCmd){
    fDetector->SetNSegmentsZ(fNsegmentsZCmd->GetNewIntValue(newValue));
  }
  //    FINISH
  else if (command == fSurfaceFinishCmd) {
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
  // SURFACE PROPERTY
  else if (command == fSurfaceMatPropVectorCmd) {
    // Convert string to physics vector
    // string format is property name, then pairs of energy, value
    // space delimited
    G4MaterialPropertyVector* mpv = new G4MaterialPropertyVector();
    G4cout << newValue << G4endl;
    std::istringstream instring(newValue);
    G4String prop;
    instring >> prop;
    while (instring) {
      G4String tmp;
      instring >> tmp;
      if (tmp == "") { break; }
      G4double en = G4UIcommand::ConvertToDouble(tmp);
      instring >> tmp;
      G4double val;
      val = G4UIcommand::ConvertToDouble(tmp);
      mpv->InsertValues(en, val);
    }
    const char* c = prop.c_str();
    fDetector->AddSurfaceMPV(c, mpv);
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
  // Gas property
  else if (command == fGasPropVectorCmd) {
    // Convert string to physics vector
    // string format is property name, then pairs of energy, value
    // space delimited
    G4MaterialPropertyVector* mpv = new G4MaterialPropertyVector();
    G4cout << newValue << G4endl;
    std::istringstream instring(newValue);
    G4String prop;
    instring >> prop;
    while (instring) {
      G4String tmp;
      instring >> tmp;
      if (tmp == "") { break; }
      G4double en = G4UIcommand::ConvertToDouble(tmp);
      instring >> tmp;
      G4double val;
      val = G4UIcommand::ConvertToDouble(tmp);
      mpv->InsertValues(en, val);
    }
    const char* c = prop.c_str();
    fDetector->AddGasMPV(c, mpv);
  }
}
