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
 : G4VUserDetectorConstruction(),m_DetectorMessenger(nullptr){
   materials = Materials::getInstance();
   m_DetectorMessenger = new DetectorMessenger(this);

   m_WorldSizeX = m_WorldSizeZ = 0.25*m;
   m_WorldSizeY = 0.5*m;

}


/*
 *
 */
DetectorConstruction::~DetectorConstruction(){

}

/* Default geometry (2018 testbeam) is created here
 * The user is able to modify geometry via messenger commands
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
    new G4Box("World",       //name
              m_WorldSizeX,  //sizeX
              m_WorldSizeY,  //sizeY
              m_WorldSizeZ); //sizeZ

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

  //----------------- Define Half of the world -----------------//
  // This volume will be filled with gas with a different refractive
  // index than the rest of the world and will hold the light guide
  // and PMT

  m_solidHalfWorld =
    new G4Box("HalfWorld",    //name
              m_WorldSizeX,   //sizeX
              m_WorldSizeY/2, //sizeY
              m_WorldSizeZ);  //sizeZ

  m_logicHalfWorld =
    new G4LogicalVolume(m_solidHalfWorld, //solid
                        Air,              //material
                        "HalfWorld");     //name

  m_physHalfWorld =
    new G4PVPlacement(0,                                 //no rotation
                      G4ThreeVector(0,m_WorldSizeY/2,0), //at (0,0,0)
                      m_logicHalfWorld,                  //logical volume
                      "HalfWorld",                       //name
                      m_logicWorld,                      //mother  volume
                      false,                             //no boolean operation
                      0,                                 //copy number
                      checkOverlaps);                    //overlaps checking

  G4VisAttributes* boxVisAtt_half_world = new G4VisAttributes(G4Colour(0.0,0.0,1.0,0.9));

	m_logicHalfWorld ->SetVisAttributes(boxVisAtt_half_world);


  //----------------- Make the light guide -----------------//
  //Make this one by default and allow the user to replace it later
  //Create a trapezoidal air light guide made of aluminum sheet
  G4double thickness = 1.0*mm;
  G4double LengthX   = 89.75*mm/2;
  G4double LengthY   = 164.*mm/2;
  G4double PMTwindow = 45.969*mm/2;
  G4double HeightZ   = lgHeight;
  G4RotationMatrix* rot = new G4RotationMatrix();
  rot->rotateX(90*deg);

  //Aluminum outter
  G4Trd* outter =
    new G4Trd("BasicLightGuide",
              LengthX+thickness,
              PMTwindow+thickness,
              LengthY+thickness,
              PMTwindow+thickness,
              HeightZ);
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

  m_logicLightGuide =
    new G4LogicalVolume(LightGuide,
                        Al,
                        "BasicLightGuide");

  m_physLightGuide =
    new G4PVPlacement(rot,
                      G4ThreeVector(0, HeightZ - m_WorldSizeY/2, 0),
                      m_logicLightGuide,
                      "BasicLightGuide",
                      m_logicHalfWorld,
                      false,
                      0);

  //----------------- Define Optical Borders -----------------//

  m_SurfLGtoWorld =
    new G4LogicalBorderSurface("AlSurface",
                               m_physLightGuide,
                               m_physHalfWorld,
                               materials->AlSurface );
  m_SurfLGtoInner =
    new G4LogicalBorderSurface("AlSurface",
                               m_physHalfWorld,
                               m_physLightGuide,
                               materials->AlSurface );


  //----------------- Define PMT window -----------------//
  double PMTradius = 65.0/2*mm;
  double PMTthickness = 0.5/2*mm;
  G4RotationMatrix * PMTrot = new G4RotationMatrix();
  PMTrot->rotateX(90*deg);

  G4SDManager* SDman = G4SDManager::GetSDMpointer();
  PMTSD* PMT = new PMTSD("MyPMT");
  SDman->AddNewDetector( PMT );

  m_solidPMT =
    new G4Tubs("PMT",       //name
              0.0*mm,       //Inner radius
              PMTradius,    //Outter radius
              PMTthickness, //Height
              0.0*deg,      //Rotation start
              360.0*deg);   //Sweep

  m_logicPMT =
    new G4LogicalVolume(m_solidPMT, //solid
                        Air,        //material
                        "PMT");     //name

  m_physPMT =
    new G4PVPlacement(PMTrot,
                      G4ThreeVector(0, 2*lgHeight + PMTthickness, 0),
                      m_logicPMT,
                      "PMT",
                      m_logicWorld,
                      false,
                      0);

  m_logicPMT->SetSensitiveDetector( PMT );

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
void DetectorConstruction::UseCADModel(G4String fileName){
  //Delete anything we are going to redefine
  if(m_SurfLGtoWorld)   delete m_SurfLGtoWorld;
  if(m_SurfLGtoInner)   delete m_SurfLGtoInner;
  if(m_logicLightGuide) delete m_logicLightGuide;
  if(m_physLightGuide)  delete m_physLightGuide;

  G4double lgHeight = 130*mm;
  G4String fileType = fileName.substr( fileName.last('.') + 1, fileName.size() - fileName.last('.'));
  G4cout << fileType << G4endl;

  #ifdef CADMESH
  if(fileType == "stl"){
    CADMesh* mesh = new CADMesh((char*) fileName.c_str());
    mesh->SetScale(mm);
    mesh->SetOffset( G4ThreeVector(-20*cm, 0, 0) );
    mesh->SetReverse(false);

    m_logicLightGuide =
      new G4LogicalVolume(mesh->TessellatedMesh(), //solid
                          materials->Al,            //material
                          "LightGuide");           //name
  }
  #endif

  if(fileType == "gdml"){
    m_Parser.Read(fileName);
  }
  if(fileType == "step"){
    m_logicLightGuide = m_Parser.ParseST(fileName,materials->Air,materials->Al);
  }

  if(m_logicLightGuide !=0 ){
    m_physLightGuide =
      new G4PVPlacement(0,
                        G4ThreeVector(0,0,lgHeight),
                        m_logicLightGuide,
                        "physLightGuide",
                        m_logicHalfWorld,
                        false,
                        0);
  }

  //----------------- Define Optical Borders -----------------//
  m_SurfLGtoWorld =
    new G4LogicalBorderSurface("AlSurface",
                               m_physLightGuide,
                               m_physHalfWorld,
                               materials->AlSurface );
  m_SurfLGtoInner =
    new G4LogicalBorderSurface("AlSurface",
                               m_physHalfWorld,
                               m_physLightGuide,
                               materials->AlSurface );

  G4RunManager::GetRunManager()->GeometryHasBeenModified();
}

/*
 *
 */
void DetectorConstruction::OutputToGDML(G4String fileName){
  if(m_physLightGuide != 0){
    //For some reason this only works as a pointer
    G4GDMLParser* gdml = new G4GDMLParser();
    gdml->Write(fileName.c_str(),m_physLightGuide);
    delete gdml;
  }else{
    G4cout << "No physical light guide defined..." << G4endl;
    G4cout << "Can't write model to GDML" << G4endl;
  }
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

/*
 *
 */
void DetectorConstruction::SetRotation(G4ThreeVector arg){
  if(!m_rotation) m_rotation = new G4RotationMatrix();
  m_rotation->rotateX(arg.x());
  m_rotation->rotateY(arg.y());
  m_rotation->rotateZ(arg.z());
  m_physLightGuide->SetRotation(m_rotation);
  G4RunManager::GetRunManager()->GeometryHasBeenModified();
}

/*
 *
 */
void DetectorConstruction::SetTranslation(G4ThreeVector arg){
  m_physLightGuide->SetTranslation(arg);
  G4RunManager::GetRunManager()->GeometryHasBeenModified();
}
