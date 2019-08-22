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
/// \file /src/DetectorConstruction.cc
/// \brief Implementation of the DetectorConstruction class
//
//

// USER //
#include "DetectorConstruction.hh"
#include "DetectorMessenger.hh"
#include "Materials.hh"
#include "PMTSD.hh"

#include <math.h>

// GEANT4 //
#include "G4Element.hh"
#include "G4SDManager.hh"
#include "G4RunManager.hh"
#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"
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

#include "G4GDMLParser.hh"

#ifdef CADMESH
#include "CADMesh.hh"
#endif

/*
 *
 */
DetectorConstruction::DetectorConstruction()
 : G4VUserDetectorConstruction(),fDetectorMessenger(nullptr){
   materials = Materials::getInstance();
   fDetectorMessenger = new DetectorMessenger(this);
}


/*
 *
 */
DetectorConstruction::~DetectorConstruction(){

}


/* Detector construction is currently all done here
 * Breaking out into modules may be done later
 */
G4VPhysicalVolume* DetectorConstruction::Construct(){
  //Set up the materials
  materials->UseOpticalMaterials(true);
  materials->DefineOpticalProperties();
  G4Material* Air = materials->Air;
  G4Material* Al = materials->Al;


  //Set the lightguide height. Hardcoded for now,
  //should be determined by the model dimensions
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

  G4VisAttributes* boxVisAtt_world= new G4VisAttributes(G4VisAttributes::Invisible);

	m_logicWorld ->SetVisAttributes(boxVisAtt_world);


  //----------------- Make the light guide -----------------//
  //If we have defined a CAD file, use it
  if(filename != ""){

    // CAD model rotation.
    G4RotationMatrix * rot = new G4RotationMatrix();
    rot->rotateX(m_rotX*deg);
    rot->rotateY(m_rotY*deg);
    rot->rotateZ(m_rotZ*deg);


    #ifdef CADMESH
    if(filetype == "stl"){
      CADMesh* mesh = new CADMesh((char*) filename.c_str());
      mesh->SetScale(mm);
      mesh->SetOffset( G4ThreeVector(-20*cm, 0, 0) );
      mesh->SetReverse(false);

      cad_logical =
        new G4LogicalVolume(mesh->TessellatedMesh(), //solid
                            Al,                      //material
                            "LightGuide");           //name
    }
    #endif

    if(filetype == "gdml"){
      fParser.Read(filename);
    }
    if(filetype == "step"){
      cad_logical = fParser.ParseST(filename,Air,Al);
    }

    if(cad_logical !=0 ){
      cad_physical =
        new G4PVPlacement(rot,
                          G4ThreeVector(0,0,lgHeight-(3*cm)),
                          cad_logical,
                          "cad_physical",
                          m_logicWorld,
                          false,
                          0);
    }

      if(GDMLoutput != ""){
        G4GDMLParser* gdml = new G4GDMLParser();
        gdml->Write("GDMLoutput",cad_physical);
      }


    //Make the surface optically reflective
    /*G4LogicalSkinSurface* alumLSS =
      new G4LogicalSkinSurface("AlSkinSurface",
                               cad_logical,
                               materials->AlSurface );*/
    G4LogicalBorderSurface* alumLSS =
      new G4LogicalBorderSurface("AlSurface",
                                 m_physWorld,
                                 cad_physical,
                                 materials->AlSurface );
   G4LogicalBorderSurface* alumLSS1 =
     new G4LogicalBorderSurface("AlSurface",
                                cad_physical,
                                m_physWorld,
                                materials->AlSurface );

  }else{
    //Create a trapezoidal air light guide made of aluminum sheet
    G4double thickness = 1.0*mm;
    G4double LengthX   = 89.75*mm/2;
    G4double LengthY   = 164.*mm/2;
    G4double PMTwindow = 45.969*mm/2;
    G4double HeightZ   = lgHeight;

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
    (void)physLightGuide;

    /*G4LogicalSkinSurface* alumLSS =
      new G4LogicalSkinSurface("AlSkinSurface",
                               logicLightGuide,
                               materials->AlSurface );*/
   G4LogicalBorderSurface* alumLSS1 =
     new G4LogicalBorderSurface("AlSurface",
                                physLightGuide,
                                m_physWorld,
                                materials->AlSurface );
   G4LogicalBorderSurface* alumLSS2 =
     new G4LogicalBorderSurface("AlSurface",
                                m_physWorld,
                                physLightGuide,
                                materials->AlSurface );

  }//end else


  //----------------- Define PMT window -----------------//
  double PMTradius = 65.0/2*mm;
  G4SDManager* SDman = G4SDManager::GetSDMpointer();
  PMTSD* PMT = new PMTSD("MyPMT");
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
  (void)physPMT;

  logicPMT->SetSensitiveDetector( PMT );

  return m_physWorld;
}

/*
 *
 */
void DetectorConstruction::SetSurfaceSigmaAlpha(G4double v){
  materials->AlSurface->SetSigmaAlpha(v);
  G4RunManager::GetRunManager()->GeometryHasBeenModified();

  G4cout << "Surface sigma alpha set to: " << materials->AlSurface->GetSigmaAlpha()
         << G4endl;
}

/*
 *
 */
void DetectorConstruction::SetSurfaceFinish(const G4OpticalSurfaceFinish finish){
  materials->AlSurface->SetFinish(finish);
  G4RunManager::GetRunManager()->GeometryHasBeenModified();
}

/*
 *
 */
void DetectorConstruction::SetSurfaceType(const G4SurfaceType type){
  materials->AlSurface->SetType(type);
  G4RunManager::GetRunManager()->GeometryHasBeenModified();
}

/*
 *
 */
void DetectorConstruction::AddSurfaceMPV(const char* c,
                                         G4MaterialPropertyVector* mpv) {
  mpv->SetSpline(true);
  materials->GetMPTArray().at(1)->AddProperty(c, mpv);
  materials->AlSurface->SetMaterialPropertiesTable(materials->GetMPTArray().at(1));
  G4cout << "The MPT for the surface is now: " << G4endl;
  materials->GetMPTArray().at(1)->DumpTable();
  G4cout << "............." << G4endl;
}

/*
 *
 */
void DetectorConstruction::SetSurfaceModel(const G4OpticalSurfaceModel model){
  materials->AlSurface->SetModel(model);
  G4RunManager::GetRunManager()->GeometryHasBeenModified();
}

void DetectorConstruction::SetCADFilename(std::string name){
  filename = name;
  G4cout << "Using " << filename << G4endl;
  delete G4SDManager::GetSDMpointer()->FindSensitiveDetector("MyPMT");
  G4RunManager::GetRunManager()->GeometryHasBeenModified();
  //G4RunManager::GetRunManager()->ResetNavigator();
  G4GeometryManager::GetInstance()->OpenGeometry();
  G4PhysicalVolumeStore::GetInstance()->Clean();
  G4LogicalVolumeStore::GetInstance()->Clean();
  G4SolidStore::GetInstance()->Clean();
  Construct();
  //G4RunManager::GetRunManager()->ReinitializeGeometry();
  // /lightGuide/CADmodel ../zdclg/models/LightGuide2007BigPMT.stl
}

/*
 *
 */
void DetectorConstruction::SetRotationX(G4int arg){
  m_rotX = arg;
  G4RunManager::GetRunManager()->GeometryHasBeenModified();
}

/*
 *
 */
void DetectorConstruction::SetRotationY(G4int arg){
  m_rotY = arg;
  G4RunManager::GetRunManager()->GeometryHasBeenModified();
}

/*
 *
 */
void DetectorConstruction::SetRotationZ(G4int arg){
  m_rotZ = arg;
  G4RunManager::GetRunManager()->GeometryHasBeenModified();
}

/*
 *
 */
void DetectorConstruction::SetOffsetX(G4double arg){
  m_offsetX = arg;
  G4RunManager::GetRunManager()->GeometryHasBeenModified();
}

/*
 *
 */
void DetectorConstruction::SetOffsetY(G4double arg){
  m_offsetY = arg;
  G4RunManager::GetRunManager()->GeometryHasBeenModified();
}

/*
 *
 */
void DetectorConstruction::SetOffsetZ(G4double arg){
  m_offsetZ = arg;
  G4RunManager::GetRunManager()->GeometryHasBeenModified();
}
