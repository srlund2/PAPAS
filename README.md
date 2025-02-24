
### ZDC Light Guide Monte Carlo


This simulation is based on OpNovice and OpNovice2

#### Build requirements
  - Geant4 compiled with GDML and additional datasets. Be sure to source geant4make.sh
```
    source /path/to/geant4-install/share/Geant4-[version]/geant4make/geant4make.sh
```

  - CERN ROOT

#### main()

define Random Number Engine, initial seed, CAD input and GDML output

#### G4VUserPhysicsList

 - Define particles; including
  - *** G4OpticalPhoton     ***
 - Define processes; including
   - *** G4Cerenkov          ***
   - *** G4Scintillation     ***
   - *** G4OpAbsorption      ***
   - *** G4OpRayleigh        ***
   - *** G4OpBoundaryProcess ***

#### Materials

defines many materials for potential use
    For our purposes the G4OpticalSurface AlSurface and the G4Material Al and Air
    will be given optical properties and used for construction.

#### G4VUserDetectorConstruction

A light guide is made either by simple G4trd or by importing a model
    via CADMesh.
    define G4LogicalBorderSurface between the light guide and world volume
    defines a PMT window sensitive detector at the top of the light guide
    AlSurface properties can be modified via DetectorMessenger via the following commands
```
    /lightGuide/surface/Model
    /lightGuide/surface/Type
    /lightGuide/surface/Finish
    /lightGuide/surface/SigmaAlpha
    /lightGuide/surface/Property
```

CAD models can be imported and positioned via
```
    /lightGuide/model/CADmodel
    /lightGuide/model/rotate
    /lightGuide/model/translate
    /lightGuide/model/translatePMT
```
The models must be in ASCII STL format in the current implementation.

And the optical properties of a sub volume of G4Air which the light guide sits in can be configured via
```
    /lightGuide/gasProperty
```

examples of all of these can be found in run1.mac

#### G4VUserPrimaryGeneratorAction

Use G4GeneralParticleSource to shoot an optical photon into the light guide
Particle type and distribution is set in run1.mac

#### G4UserRunAction

define G4Timer (start/stop)
define G4AnalysisManager (output .root file)
set verbose levels

#### PMTHit

stores G4int         trackID,
       G4ThreeVector pos;      // Origin position of the photon
       G4ThreeVector hit;      // Location where the photon hit the PMT window
       G4double      energy;   // Energy of the photon
       G4ThreeVector momentum; // Momentum of the photon (direction)
       G4double      time;     // Time of arrival of the photon
for each hit on the PMT sensitive detector

#### PMTSD

Records a PMTHit if the photon strikes the PMT window

#### G4UserEventAction

 Show how to count the number of secondary particles in an event

#### Visualisation

 The Visualization Manager is set in the main().
 The initialisation of the drawing is done via a set of /vis/ commands
 in the macro vis.mac. This macro is automatically read from
 the main in case of interactive running mode.

#### How to start

 - compile and link to generate an executable
```
  $ mkdir zdclg-build zdclg-install
  $ cd zdclg-build
  $ cmake -DCMAKE_INSTALL_PREFIX=../zdclg-install /path/to/zdclg
```

   This example handles the program arguments in a new way.
   It can be run with the following optional arguments:
```
  $ ./lightGuide [-m macro ]
                  [-u UIsession]
                  [-t nThreads]
                  [-r seed]
                  [-o outputFileName]
```

   The -t option is available only in multi-threading mode
   and it allows the user to override the Geant4 default number of
   threads. The number of threads can be also set via G4FORCENUMBEROFTHREADS
   environment variable which has the top priority.

 - execute lightGuide in 'batch' mode from macro files
```
   $ ./lightGuide -m run1.mac
```

 - execute lightGuide in 'interactive mode' with visualization
```
$ lightGuide
....
Idle> type your commands. For instance:
Idle> /control/execute run1.mac
....
Idle> exit
```
