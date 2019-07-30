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
// @Author Chad Lantz

#include "Hit.hh"
#include "G4UnitsTable.hh"
#include "G4VVisManager.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"

G4Allocator<Hit> HitAllocator;

/*
 *
 */
Hit::Hit() {}

/*
 *
 */
Hit::~Hit() {}

/*
 *
 */
Hit::Hit(const Hit& right)
  : G4VHit()
{
  trackID   = right.trackID;
  pos       = right.pos;
  energy    = right.energy;
  momentum  = right.momentum;
}

/*
 *
 */
const Hit& Hit::operator=(const Hit& right)
{
  trackID   = right.trackID;
  pos       = right.pos;
  energy    = right.energy;
  momentum  = right.momentum;
  return *this;
}

/*
 *
 */
G4int Hit::operator==(const Hit& right) const
{
  return (this==&right) ? 1 : 0;
}

/*
 *
 */
void Hit::Draw()
{
  G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
  if(pVVisManager)
  {
    G4Circle circle(pos);
    circle.SetScreenSize(2.);
    circle.SetFillStyle(G4Circle::filled);
    G4Colour colour(1.,0.,0.);
    G4VisAttributes attribs(colour);
    circle.SetVisAttributes(attribs);
    pVVisManager->Draw(circle);
  }
}

/*
 *
 */
void Hit::Print()
{
  G4cout << "  trackID: " << trackID
	 << "  mod: " << modNb << "  rad: " << radNb
	 << "  energy deposit: " << G4BestUnit(edep,"Energy")
	 << "  position: " << G4BestUnit(pos,"Length") << G4endl;
}
