/*
 * Copyright (C) <2012-2014> <EDF-R&D> <FRANCE>
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

#include "AcousticRaytracerConfiguration.h"

AcousticRaytracerConfiguration* AcousticRaytracerConfiguration::_pInstance = 0;
AcousticRaytracerConfiguration::AcousticRaytracerConfiguration()
    {
    // Ray tracing
    NbRaysPerSource = 400000;
    Discretization = 2;
    Accelerator = 3;
    MaxTreeDepth = 12;
    NbRayWithDiffraction = 0;
    MaxProfondeur = 10;
    MaxReflexion = 6;
    MaxDiffraction = 4;
    MaxLength = 5000;
    SizeReceiver = 2.;
    AngleDiffMin = 5.;
    CylindreThick = 0.5;
    MaxPathDifference = 25;

    // Sampler
    InitialAnglePhi = 0.;
    FinalAnglePhi = 360.;
    InitialAngleTheta = 0.;
    FinalAngleTheta = 360.;

    // Filters
    UseSol = false;
    UsePathDifValidation = false;
    DiffractionFilterRayAtCreation = true;
    DiffractionUseDistanceAsFilter = true;
    DiffractionUseRandomSampler = false;
    DiffractionDropDownNbRays = false;

    // Post Filters
    UsePostFilter = false;
    DebugUseCloseEventPostFilter = false;
    DebugUseDiffractionAngleSelector = false;
    DebugUseDiffractionPathSelector = false;
    DebugUseFermatSelector = false;
    DebugUseFaceSelector = false;

    // Miscellaneous
    KeepDebugRay = false;
}
AcousticRaytracerConfiguration* AcousticRaytracerConfiguration::get()
{
    if (!_pInstance)
    {
        _pInstance = new AcousticRaytracerConfiguration();
    }
    return _pInstance;
}
