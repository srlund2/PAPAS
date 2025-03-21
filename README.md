**PAPAS: Package for Air-Light-Guide Photon Acceptance Simulation**

### Project Description
PAPAS is a Geant4-based application that enables monte carlo optical simulation of CAD modeled light guides. This open-source simulation package that CAD model import (via CADMesh) into Geant4, Monte Carlo photon tracing, and performance analysis. PAPAS is designed to be maximally customizable, to provide optical simulation of CAD models to a variety of circumastances. 

The creation of PAPAS was motivated by a need for light guide simulaiton. Light guides are commonly used in particle and nuclear physics to redirect photons within a particle detector. The performance of a detector’s light guides is crucial to meaningful data collection. Thus, it is imperative to be able to evaluate the performance of a potential light guide model prior to implementation. Our group developed PAPAS to test new designs and geometries for our currently-in-development detector.  

### Dependencies 
PAPAS is a Geant4 package, and as such relies of Geant4's simulation and physics capacities. Additionaly, analysis scripts provided in PAPAS rely on CERN-ROOT. At time of writing, PAPAS is compatible with Geant4.11.2, and ROOT 6.28.

When installing Geant4, a user should source geant4make.sh
```
    source /path/to/geant4-install/share/Geant4-[version]/geant4make/geant4make.sh
```

### Some technical details 
[any technical details from original readme that should be kept]

### Installation
Using cmake, compile and link to generate an executable. 
  - download/pull source material into local source folder (PAPAS)
  - create corresponding build and install folders
  - cmake
   
```
  $ mkdir papas-build papas-install
  $ cd papas-build
  $ cmake -DCMAKE_INSTALL_PREFIX=../papas-install /path/to/PAPAS
```

### Usage 
The program is run from terminal. For visulization with the interactive GUI: 
```
$ cd path/to/papas-install/bin 
$ ./lightGuide
```
In the _Session_ bar at the bottom of the GUI, macro commands can be used. For example,
```
/lightGuide/model/PMTDiameter 18 mm
```
(from geometry.mac) can be used to change the diameter of the PMT window to 18 mm. 

To run in batch mode without visulization, it can be run with the following optional arguments:
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

The following will run simulation settings recorded in run1.mac, and return a file "test.root", containing the simulated data.
```
   $ ./lightGuide -m run1.mac -o test
```

### Customization
*
Customization of the simulaiton is done through editing the macro files: 
  - beam.mac (Controls illumination parameters and geometry. Default beam particle is optical photon.)
  - geomtery.mac (Controls placement/sizing of light guide and PMT window.)
  - hist.mac (Optional place for user input light distribution, activated in beam.mac.)
  - run1.mac (Executes simulaiton. Controls verbosity, number of optical photons, etc.)

Further instructions and usage examples can be found in the macro files. 

### Analysis 
The pre-written analysis script is utlized by running ./plotHisto with arguments of the desired file to be analyzed and the desired dimensions of the output histogram, in Xmm and Ymm.
```
$ ./plotHisto test.root 20 40
```
Further instructions for usage and modification can be found in the plotHisto.cc and compareHisto.cc files.

This work is protected under the MIT LISCENSE.





------------------------------------------------------------------------------------------------------------------

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
