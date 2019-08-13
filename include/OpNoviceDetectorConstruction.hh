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
/// \file OpNovice/include/OpNoviceDetectorConstruction.hh
/// \brief Definition of the OpNoviceDetectorConstruction class
//
//
//
//
#ifndef OpNoviceDetectorConstruction_h
#define OpNoviceDetectorConstruction_h 1

#include "globals.hh"
#include "G4Material.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4VUserDetectorConstruction.hh"
#include "Materials.hh"


class OpNoviceDetectorConstruction : public G4VUserDetectorConstruction
{
  public:
    OpNoviceDetectorConstruction();
    virtual ~OpNoviceDetectorConstruction();

  public:
    virtual G4VPhysicalVolume* Construct();
    void SetCADFilename     (std::string name){filename   = name;}
    void SetCADFiletype     (std::string type){filetype   = type;}
    void SetGDMLoutName     (std::string name){GDMLoutput = name;}
    void SetSurfaceRoughness(G4double ruff)   {fRoughness = ruff;}

  private:
    G4Box*               m_solidWorld;
    G4LogicalVolume*     m_logicWorld;
    G4VPhysicalVolume*   m_physWorld;

    Materials* materials;

    std::string filename = "";
    std::string filetype = "";
    std::string GDMLoutput;
    G4double    fRoughness = 0;
};

#endif /*OpNoviceDetectorConstruction_h*/
