/*
 * Copyright (C) <2012> <EDF-R&D> <FRANCE>
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

/**
 * \file TYPHTympanApp.h
 *
 *
 *
 *
 */

#ifndef __TY_PHTYMPANAPP__
#define __TY_PHTYMPANAPP__

#include "Tympan/common_precompiled.h"

/*
 * Qt preloading for linux hazardous errors.
 */
// #include <qmetatype.h>
// #include <qdir.h>
// #include <qurl.h>
// #include <qabstractprintdialog.h>
// #include <qtextstream.h>
// #include <qslider.h>
// #include <qprinter.h>
// #include <qstyleoption.h>
// #include <qcoreevent.h>

//#ifdef WIN32 // Desactive pour test avec CALIBRE 7
#define USE_QMDIAREA
//#endif

/* Headers which will be used for precompilation if the compiler
 * supports it.
 */
#include "TYAppDefines.h"
#include "TYMainWindow.h"

#include "OSplashScreen.h"
#include "TYAbstractSceneEditor.h"
#include "TYAction.h"
#include "TYActionManager.h"
#include "TYActions.h"
#include "TYAddLibraryDialog.h"
#include "TYApplication.h"
#include "TYBatimentModelerFrame.h"
#include "TYBatimentToolbar.h"
#include "TYBoundaryNoiseMapEditor.h"
#include "TYBoxEditor.h"
#include "TYCalculManager.h"
#include "TYCalculToolbar.h"
#include "TYCameraEditor.h"
#include "TYCameraStepEditor.h"
#include "TYCameraZoneEditor.h"
#include "TYCourbeNiveauEditor.h"
#include "TYCoursEauEditor.h"
#include "TYCreateElementDialog.h"
#include "TYCylinderEditor.h"
#include "TYDistanceEditor.h"
#include "TYDOMErrorHandler.h"
#include "TYEchelleEditor.h"
#include "TYElementCheckListItem.h"
#include "TYElementListItem.h"
#include "TYElementPicker.h"
#include "TYEmpriseEditor.h"
#include "TYEtageEditor.h"
#include "TYFaceModelerFrame.h"
#include "TYFaceModelerPickEditor.h"
#include "TYFaceToolbar.h"
#include "TYFlyCameraEditor.h"
#include "TYInfraToolbar.h"
#include "TYGetLibraryDialog.h"
#include "TYLibraryWidget.h"
#include "TYLinearMaillageEditor.h"
#include "TYMachineModelerFrame.h"
#include "TYMachineToolbar.h"
#include "TYMessageManager.h"
#include "TYModelerFrame.h"
#include "TYModelerToolbar.h"
#include "TYOpenElementDialog.h"
#include "TYOpenGLRenderer.h"
#include "TYOrientationEditor.h"
#include "TYOutputFrame.h"
#include "TYPickEditor.h"
#include "TYPlanEauEditor.h"
#include "TYPluginDialog.h"
#include "TYPointCalculEditor.h"
#include "TYPointRefEditor.h"
#include "TYPolyLineEditor.h"
#include "TYPositionDialog.h"
#include "TYPositionEditor.h"
#include "TYPreferenceDialog.h"
#include "TYProjetFrame.h"
#include "TYRectangleEditor.h"
#include "TYRectangularMaillageEditor.h"
#include "TYRenderWindow.h"
#include "TYRenderWindowInteractor.h"
#include "TYReseauTransportEditor.h"
#include "TYRotationDialog.h"
#include "TYRouteEditor.h"
#include "TYSemiCylinderEditor.h"
#include "TYSilosEditor.h"
#include "TYSiteFrame.h"
#include "TYSiteModelerFrame.h"
#include "TYSolEditor.h"
#include "TYSourceEditor.h"
#include "TYTopoToolbar.h"
#include "TYVegetationEditor.h"
#include "TYVerticalMaillageEditor.h"
#include "TYBoundaryNoiseMapEditor.h"

#endif // __TY_PHTYMPANAPP__
