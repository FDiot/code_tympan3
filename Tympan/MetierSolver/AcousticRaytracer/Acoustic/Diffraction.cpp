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

#include "Diffraction.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Geometry/Cylindre.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Tools/UnitConverter.h"

bool Diffraction::getResponse(vec3& r, bool force)
{
//#define __RANDOM__
#ifdef __RANDOM__ 
	
	// Tir des rayons aléatoires sur le cone de Keller
	decimal theta = ((decimal)(rand())) * angleOuverture / ((decimal)RAND_MAX);

	if (theta > angleOuverture / 2.)
    {
        theta += (2 * M_PI - angleOuverture);
    }

#else

	// Distribution régulière des rayons entre angleOuverture/2 et -angleOuverture/2
	decimal theta = (nbResponseLeft * delta_theta) - (angleOuverture / 2.);

#endif // __RANDOM__

	if (!force)
    {
        nbResponseLeft--;
        if (nbResponseLeft < 0)
        {
            return false;
        }
    }

    if (!targets.empty())
    {
        r = vec3(targets.back());
        targets.pop_back();
        return true;
    }

    vec3 localResponse;
    Tools::fromRadianToCarthesien2( angleArrive, theta, localResponse );

    vec3 globalResponse = localRepere.vectorFromLocalToGlobal(localResponse);

    r = vec3(globalResponse);

	return true;
}

bool Diffraction::isAcceptableResponse(vec3& test)
{

    vec3 localResponse = localRepere.vectorFromGlobalToLocal(test);

    if (localResponse.z < 0.)
    {
        return false;
    }

    decimal phi = acos(localResponse.z);
    decimal tetha;

    if (localResponse.y >= 0.)
    {
        tetha = acos(localResponse.x / (sqrt(localResponse.x * localResponse.x + localResponse.y * localResponse.y)));
    }
    else
    {
        tetha = 2. * M_PI - acos(localResponse.x / (sqrt(localResponse.x * localResponse.x + localResponse.y * localResponse.y)));
    }

    if (!(tetha < angleOuverture / 2. || tetha > (2. * M_PI - angleOuverture / 2.)))
    {
        return false;
    }

    decimal deltaPhi = fabs(angleArrive - phi);
    if (deltaPhi < M_PI / 18.)
    {
        return true;
    }
    return false;

}

bool Diffraction::generateResponse(std::vector<vec3>& responses, unsigned int nbResponses)
{
    vec3 newDir;
    for (unsigned int i = 0; i < nbResponses; i++)
    {
        getResponse(newDir, true);
        responses.push_back(newDir);
    }

    return true;
}

bool Diffraction::generateTest(std::vector<vec3>& succededTest, std::vector<vec3>& failTest, unsigned int nbResponses)
{
    for (unsigned int i = 0; i < nbResponses; i++)
    {
        vec3 newDir = vec3(((decimal)rand() / (decimal)RAND_MAX) * 2. - 1., ((decimal)rand() / (decimal)RAND_MAX) * 2. - 1., ((decimal)rand() / (decimal)RAND_MAX) * 2. - 1.);
        newDir.normalize();
        if (isAcceptableResponse(newDir))
        {
            succededTest.push_back(newDir);
        }
        else
        {
            failTest.push_back(newDir);
        }
    }
    return true;
}

void Diffraction::buildRepere()
{
    Cylindre* c = (Cylindre*)(shape);
    vec3 O = pos;
    vec3 v1 = c->getVertices()->at(c->getLocalVertices()->at(0));
    vec3 v2 = c->getVertices()->at(c->getLocalVertices()->at(1));

    vec3 Z = v1 - O;
    if (Z.dot(from) < 0)
    {
        Z = v2 - O;
    }
    Z.normalize();

    vec3 X = c->getFirstShape()->getNormal() + c->getSecondShape()->getNormal();
    X.normalize();

    vec3 Y;
    Y.cross(Z, X);
    Y.normalize();

    localRepere = Repere(X, Y, Z, O);

}

void Diffraction::computeAngle()
{
    Cylindre* c = (Cylindre*)(shape);
    angleOuverture = c->getAngleOuverture();

    decimal cosAngleArrive = localRepere.getW().dot(from);
    //std::cout<<"Axe X ("<<localRepere.getU().x<<","<<localRepere.getU().y<<","<<localRepere.getU().z<<")"<<std::endl;
    angleArrive = acos(cosAngleArrive);
}

bool Diffraction::appendTarget(vec3 target, bool force)
{

    if (force)
    {
        vec3 newDir = target - pos;
        newDir.normalize();
        targets.push_back(newDir);
        return true;
    }

    if (targets.size() > static_cast<unsigned int>(nbResponseLeft)) { return false; }
    vec3 newDir = target - pos;
    newDir.normalize();
    if (isAcceptableResponse(newDir))
    {
        targets.push_back(newDir);
        return true;
    }
    return false;
}


