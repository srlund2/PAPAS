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
/// \file optical/OpNovice2/include/DetectorMessenger.hh
/// \brief Definition of the DetectorMessenger class
//
//
//

#ifndef DetectorMessenger_h
#define DetectorMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class DetectorConstruction;
class G4UIdirectory;
class G4UIcommand;
class G4UIcmdWithAString;
class G4UIcmdWithAnInteger;
class G4UIcmdWithADouble;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithoutParameter;
class G4UIcmdWith3VectorAndUnit;


class DetectorMessenger: public G4UImessenger{
  public:

    DetectorMessenger(DetectorConstruction* );
   ~DetectorMessenger();

    virtual void SetNewValue(G4UIcommand*, G4String);

  private:

    DetectorConstruction*      fDetector;

    G4UIdirectory*             flightGuideDir;
    G4UIdirectory*             fSurfaceDir;
    G4UIdirectory*             fModelDir;

    // the model
    G4UIcmdWithAString*        fModelCmd;
    G4UIcmdWith3VectorAndUnit* fWorldVolumeCmd;
    G4UIcmdWith3VectorAndUnit* fEnvelopeCmd;
    G4UIcmdWith3VectorAndUnit* fModelRotationCmd;
    G4UIcmdWith3VectorAndUnit* fModelTranslationCmd;
    G4UIcmdWith3VectorAndUnit* fPMTTranslationCmd;
    G4UIcmdWithADoubleAndUnit* fPMTDiameterCmd;
    G4UIcmdWithADoubleAndUnit* fLGThicknessCmd;
    G4UIcmdWithAString*        fOutputModelCmd;
    G4UIcmdWithAnInteger*      fNsegmentsXCmd;
    G4UIcmdWithAnInteger*      fNsegmentsZCmd;

    // the surface
    G4UIcmdWithAString*        fSurfaceModelCmd;
    G4UIcmdWithAString*        fSurfaceFinishCmd;
    G4UIcmdWithAString*        fSurfaceTypeCmd;
    G4UIcmdWithADouble*        fSurfaceSigmaAlphaCmd;
    G4UIcmdWithAString*        fSurfaceMatPropVectorCmd;

    // the gas
    G4UIcmdWithAString*        fGasPropVectorCmd;

};
#endif
