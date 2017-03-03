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

#ifndef ACOUSTICRAYTRACERCONFIGURATION_H
#define ACOUSTICRAYTRACERCONFIGURATION_H

class AcousticRaytracerConfiguration
{
public:
    // Constructor
    AcousticRaytracerConfiguration();
    // Get access to the configuration
    static AcousticRaytracerConfiguration* get();

    /// Attributes
    // Ray tracing
    unsigned int NbRaysPerSource;
    unsigned int Discretization;
    unsigned int Accelerator;
    unsigned int MaxTreeDepth;
    unsigned int NbRayWithDiffraction;
    unsigned int MaxProfondeur;
    unsigned int MaxReflexion;
    unsigned int MaxDiffraction;
    double MaxLength;
    double SizeReceiver;
    double AngleDiffMin;
    double CylindreThick;
    double MaxPathDifference;

    // Sampler
    double InitialAnglePhi;
    double FinalAnglePhi;
    double InitialAngleTheta;
    double FinalAngleTheta;

    // Filters
    bool UseSol;
    bool UsePathDifValidation;
    bool DiffractionFilterRayAtCreation;
    bool DiffractionUseDistanceAsFilter;
    bool DiffractionUseRandomSampler;
    bool DiffractionDropDownNbRays;

    // Post Filters
    bool UsePostFilter;
    bool DebugUseCloseEventPostFilter;
    bool DebugUseDiffractionAngleSelector;
    bool DebugUseDiffractionPathSelector;
    bool DebugUseFermatSelector;
    bool DebugUseFaceSelector;

    // Miscellaneous
    bool KeepDebugRay;
private:
    // singleton
    static AcousticRaytracerConfiguration* _pInstance;
};

#endif
