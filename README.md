**PAPAS: Package for Air-Light-Guide Photon Acceptance Simulation**

### Project Description
PAPAS is a Geant4-based application that enables monte carlo optical simulation of CAD modeled light guides. This open-source simulation package that CAD model import (via CADMesh) into Geant4, Monte Carlo photon tracing, and performance analysis. PAPAS is designed to be maximally customizable, to provide optical simulation of CAD models to a variety of circumstances. 

The creation of PAPAS was motivated by a need for light guide simulaiton. Light guides are commonly used in particle and nuclear physics to redirect photons within a particle detector. The performance of a detector’s light guides is crucial to meaningful data collection. Thus, it is imperative to be able to evaluate the performance of a potential light guide model prior to implementation. Our group developed PAPAS to test new designs and geometries for our currently-in-development detector.  

### Dependencies 
PAPAS is a Geant4 package, and as such relies of Geant4's simulation and physics capacities. Additionaly, analysis scripts provided in PAPAS rely on CERN-ROOT. At time of writing, PAPAS is compatible with Geant4.11.2, and ROOT 6.28.

When installing Geant4, a user should source geant4make.sh
```
    source /path/to/geant4-install/share/Geant4-[version]/geant4make/geant4make.sh
```

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
$ ./papas
```
In the _Session_ bar at the bottom of the GUI, macro commands can be used. For example,
```
/papas/model/PMTDiameter 18 mm
```
(from geometry.mac) can be used to change the diameter of the PMT window to 18 mm. 

To run in batch mode without visulization, it can be run with the following optional arguments:
```
  $ ./papas [-m macro ]
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
   $ ./papas -m run1.mac -o test
```

### Customization
*
Customization of the simulaiton is done through editing the macro files: 
  - beam.mac (Controls illumination parameters and geometry. Default beam particle is optical photon.)
  - geomtery.mac (Controls placement/sizing of light guide and PMT window.)
  - hist.mac (Optional place for user input light distribution, activated in beam.mac.)
  - run1.mac (Executes simulaiton. Controls verbosity, number of optical photons, etc.)

The surface finish of a light guide can also be modified, relevant commands can be found at https://geant4-userdoc.web.cern.ch/UsersGuides/ForApplicationDeveloper/html/TrackingAndPhysics/physicsProcess.html?highlight=optical#boundary-process. 
For our purposes, the G4OpticalSurface AlSurface and the G4Material Al and Air are given optical properties and used as the default in construction.
Further instructions and usage examples can be found in the macro files. 

### Analysis 
The pre-written analysis script is utlized by running ./plotHisto with arguments of the desired file to be analyzed and the desired dimensions of the output histogram, in Xmm and Ymm.
```
$ ./plotHisto test.root 20 40
```
Further instructions for usage and modification can be found in the plotHisto.cc and compareHisto.cc files.

This work is protected under the MIT LISCENSE.

