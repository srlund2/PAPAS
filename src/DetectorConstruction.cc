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

#include "CADMesh.hh"

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
#include "G4VisExtent.hh"
#include "G4Colour.hh"
#include "G4UImanager.hh"
// #include "G4GDMLParser.hh"

/*
 *
 */
DetectorConstruction::DetectorConstruction()
 : G4VUserDetectorConstruction(),
   m_worldDim( new G4ThreeVector(0.25*m,0.5*m,0.25*m) ),
   m_LGenvelope( new G4ThreeVector(89.75*mm,113.*mm,165.*mm) ),
   m_LGpos( new G4ThreeVector() ),
   m_pmtPos( new G4ThreeVector() ),
   m_rotation( new G4RotationMatrix() ),
   m_pmtDia(65.*mm),
   m_PMTthickness(0.25*mm),
   m_thickness(1.0*mm),
   m_nSegmentsX(1),
   m_nSegmentsZ(1),
   m_ConstructionHasBeenDone(false),
   m_UsingCADmodel(false),
   m_logicWorld(0),
   m_DetectorMessenger(nullptr)
{
   materials = Materials::getInstance();
   materials->UseOpticalMaterials(true);
   materials->DefineOpticalProperties();

   m_DetectorMessenger = new DetectorMessenger(this);
   m_runMan = G4RunManager::GetRunManager();

   //Set default values

   G4double PhotonEnergy[2] = {0.5*eV,8.0*eV};
   G4double RefractiveIndexAir[2] = {1.0,1.0};
   m_filler = G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR");
   m_GasMPT = new G4MaterialPropertiesTable();
   m_GasMPT->AddProperty("RINDEX", PhotonEnergy, RefractiveIndexAir, 2);
   m_filler->SetMaterialPropertiesTable(m_GasMPT);

}


/*
 *
 */
DetectorConstruction::~DetectorConstruction(){
  delete m_DetectorMessenger;
}

/*
 * Execute geometry.mac here and construct the world according
 * to what it contains
 */
G4VPhysicalVolume* DetectorConstruction::Construct(){
  if ( m_ConstructionHasBeenDone ) {
    G4GeometryManager::GetInstance()->OpenGeometry();
    G4PhysicalVolumeStore::GetInstance()->Clean();
    G4LogicalVolumeStore::GetInstance()->Clean();
    G4SolidStore::GetInstance()->Clean();
    G4LogicalSkinSurface::CleanSurfaceTable();
    G4LogicalBorderSurface::CleanSurfaceTable();
  } else {
    G4UImanager* UImanager = G4UImanager::GetUIpointer();
    UImanager->ApplyCommand("/control/execute geometry.mac");
    BuildPMT();
  }

  if(!m_logicWorld) BuildWorld();
  if(!m_logicLightGuide) BuildTrapezoidLG();
  PlaceGeometry();

  m_ConstructionHasBeenDone = true;

  return m_physWorld;

}

/*
 *
 */
void DetectorConstruction::BuildWorld(){
  bool checkOverlaps = false;

  //----------------- Define the world volume -----------------//
  m_solidWorld =
    new G4Box("World",       //name
              m_worldDim->x(),  //sizeX
              m_worldDim->y(),  //sizeY
              m_worldDim->z()); //sizeZ

  m_logicWorld =
    new G4LogicalVolume(m_solidWorld, //solid
                        materials->Air,          //material
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
    new G4Box("HalfWorld",       //name
              m_worldDim->x(),   //sizeX
              m_worldDim->y()/2, //sizeY
              m_worldDim->z());  //sizeZ

  m_logicHalfWorld =
    new G4LogicalVolume(m_solidHalfWorld, //solid
                        m_filler,         //material
                        "HalfWorld");     //name

  m_physHalfWorld =
    new G4PVPlacement(0,                                    //no rotation
                      G4ThreeVector(0,m_worldDim->y()/2,0), //at (0,0,0)
                      m_logicHalfWorld,                     //logical volume
                      "HalfWorld",                          //name
                      m_logicWorld,                         //mother  volume
                      false,                                //no boolean operation
                      0,                                    //copy number
                      checkOverlaps);                       //overlaps checking

  G4VisAttributes* boxVisAtt_half_world = new G4VisAttributes(G4Colour(0.0,0.0,1.0,0.1)); // or G4Colour(0.0,0.0,1.0,0.1)
	m_logicHalfWorld ->SetVisAttributes(boxVisAtt_half_world);

}

/*
 * Make the trapezoidal light guide logical volume
 */
void DetectorConstruction::BuildTrapezoidLG( ){
  //Divide the light guide envelope (cumulative) by the number
  //of light guides that will occupy it
  G4double xSize = (m_LGenvelope->x()/2.)/m_nSegmentsX;
  G4double zSize = (m_LGenvelope->z()/2.)/m_nSegmentsZ;

  //Determine the size of the top of the trapezoid so the
  //square opening will be inscribed in the window of the PMT
  G4double PMTwindow = .707*m_pmtDia/2.;
  m_rotation->rotateX(90*deg);

  //Aluminum outter
  m_outter =
    new G4Trd("BasicLightGuide",
              xSize,
              PMTwindow + m_thickness,
              zSize,
              PMTwindow + m_thickness,
              m_LGenvelope->y()/2.);
  //Air inner
  m_inner =
    new G4Trd("AirVolume",
              xSize - m_thickness,
              PMTwindow,
              zSize - m_thickness,
              PMTwindow,
              m_LGenvelope->y()/2. + 0.1*mm);

  //Subtract material to hollow out the light guide
  m_LightGuide =
    new G4SubtractionSolid("BasicLightGuide",
                           m_outter,
                           m_inner);

  m_logicLightGuide =
    new G4LogicalVolume(m_LightGuide,
                        materials->Al,
                        "BasicLightGuide");

}


/*
 * Make the PMT logical volume and Sensitvie Detector
 */
void DetectorConstruction::BuildPMT(){

  G4SDManager* SDman = G4SDManager::GetSDMpointer();
  PMTSD* PMT = new PMTSD("MyPMT");
  SDman->AddNewDetector( PMT );

  m_solidPMT =
    new G4Tubs("PMT",       //name
              0.0*mm,       //Inner radius
              m_pmtDia/2.0, //Outter radius
              m_PMTthickness, //Height
              0.0*deg,      //Rotation start
              360.0*deg);   //Sweep

  m_logicPMT =
    new G4LogicalVolume(m_solidPMT, //solid
                        m_filler,   //material
                        "PMT");     //name

  G4VisAttributes* VisAtt_PMT = new G4VisAttributes(G4Colour(1.0,1.0,0.6,0.7));
  m_logicPMT->SetVisAttributes(VisAtt_PMT);
  m_logicPMT->SetSensitiveDetector( PMT );

}

/*
*
*/
void DetectorConstruction::PlaceGeometry(){
  G4double xPos = 0.;
  G4double yPos = m_LGenvelope->y()/2. + m_LGpos->y() - m_worldDim->y()/2.;
  G4double zPos = 0.;

  G4double PMTx = 0.;
  G4double PMTy = m_LGenvelope->y() + m_pmtPos->y() + m_PMTthickness - m_worldDim->y()/2.;
  G4double PMTz = 0.;

  G4RotationMatrix * PMTrot = new G4RotationMatrix();
  PMTrot->rotateX(90*deg);


  if(m_nSegmentsX == 1 && m_nSegmentsZ == 1){

          m_physLightGuide.push_back(
            new G4PVPlacement(m_rotation,
                              G4ThreeVector(m_LGpos->x(), m_LGpos->y(), m_LGpos->z()),
                              m_logicLightGuide,
                              "LightGuide",
                              m_logicHalfWorld,
                              false,
                              0) );

          //----------------- Place the PMT -----------------//

          m_physPMT.push_back(
            new G4PVPlacement(PMTrot,
                              G4ThreeVector(m_pmtPos->x(), m_pmtPos->y(), m_pmtPos->z()),
                              m_logicPMT,
                              "PMT",
                              m_logicHalfWorld,
                              false,
                              0) );

          //----------------- Define Optical Borders -----------------//

          m_Surfvec.push_back(
            new G4LogicalBorderSurface("AlSurface",
                                       m_physLightGuide.back(),
                                       m_physHalfWorld,
                                       materials->AlSurface ) );
          m_Surfvec.push_back(
            new G4LogicalBorderSurface("AlSurface",
                                       m_physHalfWorld,
                                       m_physLightGuide.back(),
                                       materials->AlSurface ) );


          return;
  }

  char name[40];

  for(G4int xIndex = 0; xIndex < m_nSegmentsX; xIndex++ ){
    //Start from -x, add user specified offset, add 1/2 the width of one LG, then a full width for each
    xPos = - m_LGenvelope->x()/2. + m_LGpos->x() + (0.5 + xIndex)*m_LGenvelope->x()/m_nSegmentsX;

    //Centered on the light guide plus any user specified offset
    PMTx = xPos + m_pmtPos->x();

    for(G4int zIndex = 0; zIndex < m_nSegmentsZ; zIndex++ ){
      //Start from -z, add user specified offset, add 1/2 the length of one LG, then a full length for each
      zPos = - m_LGenvelope->z()/2. + m_LGpos->z() + (0.5 + zIndex)*m_LGenvelope->z()/m_nSegmentsZ;

      //Centered on the light guide plus any user specified offset
      PMTz = zPos + m_pmtPos->z();
      //----------------- Place the LightGuide -----------------//

      sprintf(name,"LightGuide%d_%d",xIndex,zIndex);
      m_physLightGuide.push_back(
        new G4PVPlacement(m_rotation,
                          G4ThreeVector(xPos, yPos, zPos),
                          m_logicLightGuide,
                          name,
                          m_logicHalfWorld,
                          false,
                          0) );

      //----------------- Place the PMT -----------------//

      sprintf(name,"PMT%d_%d",xIndex,zIndex);
      m_physPMT.push_back(
        new G4PVPlacement(PMTrot,
                          G4ThreeVector(PMTx, PMTy, PMTz),
                          m_logicPMT,
                          name,
                          m_logicHalfWorld,
                          false,
                          0) );

      //----------------- Define Optical Borders -----------------//

      sprintf(name,"AlSurface%d_%d",xIndex,zIndex);
      m_Surfvec.push_back(
        new G4LogicalBorderSurface(name,
                                   m_physLightGuide.back(),
                                   m_physHalfWorld,
                                   materials->AlSurface ) );
      m_Surfvec.push_back(
        new G4LogicalBorderSurface(name,
                                   m_physHalfWorld,
                                   m_physLightGuide.back(),
                                   materials->AlSurface ) );
    }//end y loop
  }//end x loop
}

/*
 * Set the dimensions of the world volume.
 * If the volume exists modify it, otherwise build it.
 */
void DetectorConstruction::SetWorldVolume(G4ThreeVector arg){
  if(m_ConstructionHasBeenDone){
    m_solidWorld->SetXHalfLength( arg.x() );
    m_solidWorld->SetYHalfLength( arg.y() );
    m_solidWorld->SetZHalfLength( arg.z() );
    m_runMan->GeometryHasBeenModified();
    G4UImanager::GetUIpointer()->ApplyCommand("/vis/viewer/rebuild");
  } else {
    delete m_worldDim;
    m_worldDim = new G4ThreeVector(arg);
    BuildWorld();
  }
}

/*
 * Sets the envelope the light guide is constructed in
 * If the geometry has already been constructed, reconstruct
 * it with the new envelope
 */
void DetectorConstruction::SetEnvelope(G4ThreeVector arg){
  if(m_LGenvelope) delete m_LGenvelope;
  m_LGenvelope = new G4ThreeVector(arg);

  if(m_ConstructionHasBeenDone){
    Construct();
    m_runMan->GeometryHasBeenModified();
    G4UImanager::GetUIpointer()->ApplyCommand("/vis/viewer/rebuild");
  }
}

/*
 * If the world already exists, rotate the light guide
 * otherwise set the rotation for the light guide that will be built
 */
void DetectorConstruction::SetRotation(G4ThreeVector arg){
  if(m_rotation) delete m_rotation;
  m_rotation = new G4RotationMatrix();

  m_rotation->rotateX(arg.x());
  m_rotation->rotateY(arg.y());
  m_rotation->rotateZ(arg.z());

  if(m_ConstructionHasBeenDone){
    for(uint i = 0; i < m_physLightGuide.size(); i++){
      m_physLightGuide[i]->SetRotation(m_rotation);
    }
    m_runMan->GeometryHasBeenModified();
    G4UImanager::GetUIpointer()->ApplyCommand("/vis/viewer/rebuild");
  }
}

/*
 * If the world already exists, move the light guide. If there is more
 * than one light guide, set the position and rebuild.
 * otherwise set the location for the light guide that will be built
 */
void DetectorConstruction::SetTranslation(G4ThreeVector arg){
  if(m_ConstructionHasBeenDone){
    if(m_physLightGuide.size() == 1){
      m_physLightGuide.back()->SetTranslation(arg);
    } else {
      m_LGpos = new G4ThreeVector(arg);
      Construct();
    }
    m_runMan->GeometryHasBeenModified();
    G4UImanager::GetUIpointer()->ApplyCommand("/vis/viewer/rebuild");
  } else {
    m_LGpos = new G4ThreeVector(arg);
  }
}


/*
 * If the world already exists, move the PMT. If there is more
 * than one PMT, set the position and rebuild.
 * otherwise set the location for the PMT that will be built
 */
void DetectorConstruction::SetPMTTranslation(G4ThreeVector arg){
  if(m_ConstructionHasBeenDone){
    if(m_physPMT.size() == 1){
      m_physPMT.back()->SetTranslation(arg);
    } else {
      m_LGpos = new G4ThreeVector(arg);
      Construct();
    }
    m_runMan->GeometryHasBeenModified();
    G4UImanager::GetUIpointer()->ApplyCommand("/vis/viewer/rebuild");
  } else {
    m_pmtPos = new G4ThreeVector(arg);
  }
}

/*
 * If the world already exists, modify the PMT
 * otherwise set the diameter for PMT that will be built
 */
void DetectorConstruction::SetPMTDiameter(G4double arg){
  if(m_ConstructionHasBeenDone){
    m_solidPMT->SetOuterRadius(arg/2.0);
    m_runMan->GeometryHasBeenModified();
    G4UImanager::GetUIpointer()->ApplyCommand("/vis/viewer/rebuild");
  } else {
    m_pmtDia = arg;
  }
}

/*
 * Set the thickness of the trapezoidal light guide shell
 * If the geometry has already been constructed, reconstruct
 * it with the new thickness
 */
void DetectorConstruction::SetLGthickness(G4double arg){
  m_thickness = arg;

  if(m_ConstructionHasBeenDone){
    Construct();
    m_runMan->GeometryHasBeenModified();
    G4UImanager::GetUIpointer()->ApplyCommand("/vis/viewer/rebuild");
  }
}


/*
 * Builds a light guide logical volume from CAD model file
 */
void DetectorConstruction::UseCADModel(G4String fileName){
  //Delete anything we are going to redefine
  if(m_logicLightGuide) delete m_logicLightGuide;
  float xSize=0.0, ySize=0.0, zSize=0.0;

  for(uint i = 0; i < m_physLightGuide.size(); i++){
    delete m_physLightGuide[i];
    delete m_physPMT[i];
    delete m_Surfvec[2*i];
    delete m_Surfvec[2*i+1];
  }
  m_physLightGuide.clear();
  m_physPMT.clear();
  m_Surfvec.clear();


  G4String fileType = fileName.substr( fileName.last('.') + 1, fileName.size() - fileName.last('.'));

  if(fileType == "stl"){
    auto mesh = CADMesh::TessellatedMesh::FromSTL((char*) fileName.c_str());
    // auto mesh = CADMesh::TessellatedMesh::From((char*) fileName.c_str());
    mesh->SetScale(mm);
    mesh->SetOffset( G4ThreeVector(-20*cm, 0, 0) );
    mesh->SetReverse(false);

    m_logicLightGuide =
      new G4LogicalVolume(mesh->GetSolid(), //solid
                          materials->Al,           //material
                          "LightGuide");           //name
  }

  // if(fileType == "gdml"){
  //   m_Parser.Read(fileName);
  // }
  // if(fileType == "step"){
  //   m_logicLightGuide = m_Parser.ParseST(fileName,materials->Air,materials->Al);
  // }

  if(m_logicLightGuide !=0 ){

    //Print the extent to the console to help the user position the light guide
    char message[64];
    G4VisExtent extent = m_logicLightGuide->GetSolid()->GetExtent();
    xSize = extent.GetXmax() - extent.GetXmin();
    ySize = extent.GetYmax() - extent.GetYmin();
    zSize = extent.GetZmax() - extent.GetZmin();
    G4cout << "===== Light guide extent =====" << G4endl;
    sprintf(message,"Xmin = %04.2f, Xmax = %04.2f: Width  = %04.2f", extent.GetXmin(), extent.GetXmax(), xSize );
    G4cout << message << G4endl;
    sprintf(message,"Ymin = %04.2f, Ymax = %04.2f: Height = %04.2f", extent.GetYmin(), extent.GetYmax(), ySize );
    G4cout << message << G4endl;
    sprintf(message,"Zmin = %04.2f, Zmax = %04.2f: Depth  = %04.2f", extent.GetZmin(), extent.GetZmax(), zSize );
    G4cout << message << G4endl;

  }

  if(m_pmtPos == 0) m_pmtPos = new G4ThreeVector( );
  SetEnvelope( G4ThreeVector( m_nSegmentsX*xSize, ySize, m_nSegmentsZ*zSize ) );

  if(m_ConstructionHasBeenDone){
    PlaceGeometry();
    m_runMan->GeometryHasBeenModified();
    G4UImanager::GetUIpointer()->ApplyCommand("/vis/viewer/rebuild");
  }
  G4cout << "Using " << fileName << " for CAD model" << G4endl;
}


/*
 * Output the model of the light guide to GDML
 */
void DetectorConstruction::OutputToGDML(G4String fileName){

  G4cout << "Go home, you're drunk" << G4endl;
  // if(m_physLightGuide[0] != 0){
    //For some reason this only works as a pointer
  //   G4GDMLParser* gdml = new G4GDMLParser();
  //   gdml->Write(fileName.c_str(),m_physLightGuide.back());
  //   delete gdml;
  // }else{
  //   G4cout << "No physical light guide defined..." << G4endl;
  //   G4cout << "Can't write model to GDML" << G4endl;
  // }
}

/*
 * Set the segmentation of the light guide envelope in x
 * Reconstruct the geometry if necessary
 */
void DetectorConstruction::SetNSegmentsX(G4int arg){
  m_nSegmentsX = arg;
  if(m_ConstructionHasBeenDone){
    Construct();
  }
}

/*
 * Set the segmentation of the light guide envelope in Z
 * Reconstruct the geometry if necessary
 */
void DetectorConstruction::SetNSegmentsZ(G4int arg){
  m_nSegmentsZ = arg;
  if(m_ConstructionHasBeenDone){
    Construct();
  }
}


/*
 * Set the surface model for the light guide
 */
void DetectorConstruction::SetSurfaceModel(const G4OpticalSurfaceModel model){
  materials->AlSurface->SetModel(model);
  if(m_ConstructionHasBeenDone){
    m_runMan->GeometryHasBeenModified();
  }
}

/*
 *
 */
void DetectorConstruction::SetSurfaceFinish(const G4OpticalSurfaceFinish finish){
  materials->AlSurface->SetFinish(finish);
  if(m_ConstructionHasBeenDone){
    m_runMan->GeometryHasBeenModified();
  }
}

/*
 *
 */
void DetectorConstruction::SetSurfaceType(const G4SurfaceType type){
  materials->AlSurface->SetType(type);
  if(m_ConstructionHasBeenDone){
    m_runMan->GeometryHasBeenModified();
  }
}

/*
 *
 */
void DetectorConstruction::SetSurfaceSigmaAlpha(G4double v){
  materials->AlSurface->SetSigmaAlpha(v);

  if(m_ConstructionHasBeenDone){
    G4RunManager::GetRunManager()->GeometryHasBeenModified();
  }

  G4cout << "Surface sigma alpha set to: " << materials->AlSurface->GetSigmaAlpha() << G4endl;
}

/*
 * Set the material property table of the surface
 * of the light guide
 */
void DetectorConstruction::AddSurfaceMPV(const char* c, G4MaterialPropertyVector* mpv){
  mpv->SetSpline(true);
  materials->GetMPTArray().at(1)->AddProperty(c, mpv);
  materials->AlSurface->SetMaterialPropertiesTable(materials->GetMPTArray().at(1));
  G4cout << "The MPT for the surface is now: " << G4endl;
  materials->GetMPTArray().at(1)->DumpTable();
  G4cout << "............." << G4endl;
}

/*
 * Set the material property table of the volume
 * which contains the light guide
 */
void DetectorConstruction::AddGasMPV(const char* c, G4MaterialPropertyVector* mpv){
  mpv->SetSpline(true);
  m_GasMPT->AddProperty(c, mpv);
  G4cout << "The MPT for the gas is now: " << G4endl;
  m_GasMPT->DumpTable();
  G4cout << "............." << G4endl;
}
