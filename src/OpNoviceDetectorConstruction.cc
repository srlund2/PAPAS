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
/// \file OpNovice/src/OpNoviceDetectorConstruction.cc
/// \brief Implementation of the OpNoviceDetectorConstruction class
//
//
#include "OpNoviceDetectorConstruction.hh"

#include "G4Element.hh"
#include "G4SDManager.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4OpticalSurface.hh"
#include "G4ThreeVector.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4Tubs.hh"
#include "G4Trd.hh"
#include "G4SubtractionSolid.hh"
#include "G4NistManager.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include <math.h>

#include "Materials.hh"
#include "AirSD.hh"

/*
 *
 */
OpNoviceDetectorConstruction::OpNoviceDetectorConstruction()
 : G4VUserDetectorConstruction(){
   materials = Materials::getInstance();
}


/*
 *
 */
OpNoviceDetectorConstruction::~OpNoviceDetectorConstruction(){

}


/* Make the detector
 * Ideally this will become the quartz rod, winston cone and PMT
 */
G4VPhysicalVolume* OpNoviceDetectorConstruction::Construct(){
  //Set up the materials
  materials->UseOpticalMaterials(true);
  materials->DefineOpticalProperties();
  G4Material* Air = materials->Air;
  G4Material* Al = materials->Al;
  G4Material* Silica = materials->pQuartz;

  G4double lgHeight = 339.*mm/2;

  bool checkOverlaps = false;

  //----------------- Define the world volume -----------------//
  m_solidWorld =
    new G4Box("World", //name
              0.25*m,  //sizeX
              0.25*m,  //sizeY
              0.5*m);  //sizeZ

  m_logicWorld =
    new G4LogicalVolume(m_solidWorld, //solid
                        Air,          //material
                        "World");     //name

  m_physWorld =
    new G4PVPlacement(0,                  //no rotation
                      G4ThreeVector(),    //at (0,0,0)
                      m_logicWorld,       //logical volume
                      "World",            //name
                      0,                  //mother  volume
                      false,              //no boolean operation
                      0,                  //copy number
                      checkOverlaps);     //overlaps checking

  G4VisAttributes* boxVisAtt_world= new G4VisAttributes(G4Colour(0.5,0.5,0.5));

	m_logicWorld ->SetVisAttributes(boxVisAtt_world);


  //----------------- Make the light guide -----------------//
  bool useSTL = false;

  if(useSTL){
    //STL support to be added later
  }else{
    //Create a trapezoidal air light guide made of aluminum sheet
    G4double thickness = 1.0*mm;
    G4double LengthX = 89.75*mm/2;
    G4double LengthY = 164.*mm/2;
    G4double PMTwindow = 45.969*mm/2;
    G4double HeightZ = lgHeight;

    //Aluminum outter
    G4Trd* outter =
      new G4Trd("BasicLightGuide",
                LengthX+thickness,
                PMTwindow+thickness,
                LengthY+thickness,
                PMTwindow+thickness,
                HeightZ+thickness);
    //Air inner
    G4Trd* inner =
      new G4Trd("AirVolume",
                LengthX,
                PMTwindow,
                LengthY,
                PMTwindow,
                HeightZ+2.0*mm);

    //Subtract material to hollow out the light guide
    G4SubtractionSolid* LightGuide =
      new G4SubtractionSolid("BasicLightGuide",
                             outter,
                             inner);

    G4LogicalVolume* logicLightGuide =
      new G4LogicalVolume(LightGuide,
                          Al,
                          "BasicLightGuide");

    G4VPhysicalVolume*  physLightGuide =
      new G4PVPlacement(0,
                        G4ThreeVector(0, 0, HeightZ),
                        logicLightGuide,
                        "BasicLightGuide",
                        m_logicWorld,
                        false,
                        0);

    G4LogicalSkinSurface* alumLSS = new G4LogicalSkinSurface("quartzSkinSurface", logicLightGuide, materials->AlSurface );

  }





  //----------------- Define PMT  -----------------// This will need a sensitive detector
  double PMTradius = 65.0/2*mm;
  G4SDManager* SDman = G4SDManager::GetSDMpointer();
  AirSD* PMT = new AirSD("MyPMT");
  SDman->AddNewDetector( PMT );

  G4Tubs* solidPMT =
    new G4Tubs("PMT",     //name
              0.0*mm,     //Inner radius
              PMTradius,  //Outter radius
              0.5*mm,     //Height
              0.0*deg,    //Rotation start
              360.0*deg); //Sweep

  G4LogicalVolume* logicPMT =
    new G4LogicalVolume(solidPMT,   //solid
                        Air,        //material
                        "PMT");     //name

  G4VPhysicalVolume*  physPMT =
    new G4PVPlacement(0,
                      G4ThreeVector(0, 0, 2*lgHeight + 1.5*mm),
                      logicPMT,
                      "PMT",
                      m_logicWorld,
                      false,
                      0);

  logicPMT->SetSensitiveDetector( PMT );

  return m_physWorld;
}
