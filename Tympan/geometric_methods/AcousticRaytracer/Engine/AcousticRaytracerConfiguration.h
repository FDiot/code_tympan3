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

/**
 * @brief Class defining the configuration of the acoustic ray tracer.
 */
class AcousticRaytracerConfiguration
{
public:
    /// Constructor
    AcousticRaytracerConfiguration();
    /// Get access to the configuration
    static AcousticRaytracerConfiguration* get();

    // Attributes
    // Ray tracing
    unsigned int NbRaysPerSource; //!< Number of rays per source for the Sampler
    unsigned int Discretization; //!< Sampler choice with 0: RandomSphericSampler, 1: UniformSphericSampler, 2: UniformSphericSampler2, 3: Latitude2DSampler
    unsigned int Accelerator; //!< Accelerator choice with 0: BruteForceAccelerator, 1: GridAccelerator, 2: BvhAccelerator, 3: KdtreeAccelerator
    unsigned int MaxTreeDepth; //!< BvhAccelerator Accelerator option (Maximal tree depth)
    unsigned int NbRayWithDiffraction; //!< Number of rays to throw during diffraction
    unsigned int MaxProfondeur; //!< Maximal number of events for ray validation in ANIME3D solver
    unsigned int MaxReflexion; //!< Maximal reflection events
    unsigned int MaxDiffraction; //!< Maximal diffraction events
    double MaxLength; //!< LengthSelector Selector option (maximal length)
    double SizeReceiver;
    double AngleDiffMin; //!< Minimal angle (other PI) between two face to allow building of a diffraction cylinder
    double CylindreThick; //!< Diffraction cylinder diameter
    double MaxPathDifference; //!< Parameter (by default 24) for path validations during diffraction

    // Sampler
    double InitialAnglePhi; //!< Starting angle phi for the curve ray sampler
    double FinalAnglePhi; //!< Ending angle phi for the curve ray sampler
    double InitialAngleTheta; //!< Starting angle theta for the curve ray sampler
    double FinalAngleTheta; //!< Ending angle theta for the curve ray sampler

    // Filters
    bool UseSol;
    bool UsePathDifValidation; //!< Flag to activate path validations during diffraction
    bool DiffractionFilterRayAtCreation; //!< Flag to filter the created rays during diffraction
    bool DiffractionUseDistanceAsFilter; //!< Flag to use distance based filter during diffraction (distance from the ridge)
    bool DiffractionUseRandomSampler; //!< Flag to enable random (and not regular) sampling for diffraction
    bool DiffractionDropDownNbRays; //!< Flag to reduce the number of rays thrown depending on the diffraction order

    // Post Filters
    bool UsePostFilter; //!< Flag to use some specifics Selector
    bool DebugUseCloseEventPostFilter; //!< Flag to add the the CloseEventSelector filter
    bool DebugUseDiffractionAngleSelector; //!< Flag to add the DiffractionAngleSelector filter
    bool DebugUseDiffractionPathSelector; //!< Flag to add the DiffractionPathSelector filter
    bool DebugUseFermatSelector; //!< Flag to add the FermatSelector filter
    bool DebugUseFaceSelector; //!< Flag to add the FaceSelector filter

    // Miscellaneous
    bool KeepDebugRay; //!< Flag to store rays into a debug_rays array after being invalidated
private:
    // singleton
    static AcousticRaytracerConfiguration* _pInstance;
};

#endif
