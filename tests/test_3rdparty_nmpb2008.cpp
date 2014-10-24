/**
 * @file test_cerema.cpp
 *
 * @brief Test the build, link and use of the NMBP2008 library
 *
 * @author Anthony Truchet <anthony.truchet@logilab.fr>
 *
 */

#include <cstdlib>

#include "gtest/gtest.h"

#include "RoadEmissionNMPB08.h"

TEST(NMPB2008, road_emission_use)
{

    RoadTrafficComponent rtf_VL;
    rtf_VL.vehicleType = VehicleType_VL;
    rtf_VL.flowType = FlowType_CONST;
    rtf_VL.trafficFlow = 3000;
    rtf_VL.trafficSpeed = 110;
    RoadTrafficComponent rtf_PL;
    rtf_PL.vehicleType = VehicleType_PL;
    rtf_PL.flowType = FlowType_CONST;
    rtf_PL.trafficFlow = 500;
    rtf_PL.trafficSpeed = 100;
    RoadTraffic roadTraffic;
    roadTraffic.nbComponents = 2;
    roadTraffic.traffic = new
    RoadTrafficComponent[roadTraffic.nbComponents];
    roadTraffic.traffic[0] = rtf_VL;
    roadTraffic.traffic[1] = rtf_PL;
    roadTraffic.surfaceType = RoadSurface_R2;
    roadTraffic.surfaceAge = 8;
    roadTraffic.ramp = 0.5;

    double* Lwm = NMPB08_Lwm(&roadTraffic, Spectrum_3oct_A) ;

    for (int i = 0 ; i < 18 ; i++)
    {
        printf("%.2f, ", Lwm[i]);
    }
}
