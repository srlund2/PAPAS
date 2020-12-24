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
#include "G4SubtractionSolid.hh"
#include "G4GDMLParser.hh"
#include "G4RunManager.hh"

#include "Materials.hh"
#include "DetectorMessenger.hh"


class DetectorConstruction : public G4VUserDetectorConstruction
{
  public:
    DetectorConstruction();
    virtual ~DetectorConstruction();

  public:
    virtual G4VPhysicalVolume* Construct();

    void BuildWorld           ();
    void BuildTrapezoidLG     ();
    void BuildPMT             ();
    void PlaceGeometry        ();
    void SetWorldVolume       (G4ThreeVector arg);
    void SetEnvelope          (G4ThreeVector arg);
    void SetRotation          (G4ThreeVector arg);
    void SetTranslation       (G4ThreeVector arg);
    void SetPMTTranslation    (G4ThreeVector arg);
    void SetPMTDiameter       (G4double arg);
    void SetLGthickness       (G4double arg);
    void UseCADModel          (G4String fileName);
    void OutputToGDML         (G4String name);
    void SetNSegmentsX        (G4int arg);
    void SetNSegmentsZ        (G4int arg);

    void SetSurfaceModel      (const G4OpticalSurfaceModel model);
    void SetSurfaceFinish     (const G4OpticalSurfaceFinish finish);
    void SetSurfaceType       (const G4SurfaceType type);
    void SetSurfaceSigmaAlpha (G4double v);
    void AddSurfaceMPV        (const char* c, G4MaterialPropertyVector* mpv);
    void AddGasMPV            (const char* c, G4MaterialPropertyVector* mpv);

  private:
    G4ThreeVector*          m_worldDim;
    G4ThreeVector*          m_LGenvelope;
    G4ThreeVector*          m_LGpos;
    G4ThreeVector*          m_pmtPos;
    G4RotationMatrix*       m_rotation;
    G4double                m_pmtDia;
    G4double                m_PMTthickness;
    G4double                m_thickness;
    G4int                   m_nSegmentsX;
    G4int                   m_nSegmentsZ;
    G4bool                  m_ConstructionHasBeenDone;
    G4bool                  m_UsingCADmodel;

    G4Box*                  m_solidWorld;
    G4LogicalVolume*        m_logicWorld;
    G4VPhysicalVolume*      m_physWorld;

    G4Box*                  m_solidHalfWorld;
    G4LogicalVolume*        m_logicHalfWorld;
    G4VPhysicalVolume*      m_physHalfWorld;

    G4Trd*                                 m_inner;
    G4Trd*                                 m_outter;
    G4SubtractionSolid*                    m_LightGuide;
    G4LogicalVolume*                       m_logicLightGuide;
    std::vector< G4VPhysicalVolume* >      m_physLightGuide;

    G4Tubs*                                m_solidPMT;
    G4LogicalVolume*                       m_logicPMT;
    std::vector< G4VPhysicalVolume* >      m_physPMT;

    std::vector< G4LogicalBorderSurface* > m_Surfvec;

    Materials*                             materials;
    G4Material*                            m_filler;
    G4MaterialPropertiesTable*             m_GasMPT;

    G4GDMLParser                           m_Parser;
    G4RunManager*                          m_runMan;
    DetectorMessenger*                     m_DetectorMessenger;
};

#endif /*DetectorConstruction_h*/
