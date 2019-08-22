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
/// \file /include/DetectorConstruction.hh
/// \brief Definition of the DetectorConstruction class
//
//
//
//
#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "globals.hh"
#include "G4Material.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4GDMLParser.hh"

#include "Materials.hh"
#include "DetectorMessenger.hh"


class DetectorConstruction : public G4VUserDetectorConstruction
{
  public:
    DetectorConstruction();
    virtual ~DetectorConstruction();

  public:
    virtual G4VPhysicalVolume* Construct();
    void SetCADFilename     (std::string name);
    void SetCADFiletype     (std::string type){filetype   = type;}
    void SetGDMLoutName     (std::string name){GDMLoutput = name;}

    void SetSurfaceFinish     (const G4OpticalSurfaceFinish finish);
    void SetSurfaceType       (const G4SurfaceType type);
    void SetSurfaceModel      (const G4OpticalSurfaceModel model);
    void SetSurfaceSigmaAlpha (G4double v);
    void SetRotationX         (G4int arg);
    void SetRotationY         (G4int arg);
    void SetRotationZ         (G4int arg);
    void SetOffsetX           (G4double arg);
    void SetOffsetY           (G4double arg);
    void SetOffsetZ           (G4double arg);


  private:
    G4Box*               m_solidWorld;
    G4LogicalVolume*     m_logicWorld;
    G4VPhysicalVolume*   m_physWorld;

    G4LogicalVolume*     cad_logical;
    G4VPhysicalVolume*   cad_physical;

    G4int m_rotX;
    G4int m_rotY;
    G4int m_rotZ;
    G4double m_offsetX;
    G4double m_offsetY;
    G4double m_offsetZ;

    Materials* materials;

    std::string filename = "";
    std::string filetype = "";
    std::string GDMLoutput;

    G4GDMLParser fParser;
    DetectorMessenger* fDetectorMessenger;
};

#endif /*DetectorConstruction_h*/
