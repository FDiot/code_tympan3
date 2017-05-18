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

#include <set>
#include <vector>
#include <map>

#include "Engine/AcousticRaytracerConfiguration.h"
#include "Geometry/Triangle.h"
#include "Geometry/Cylindre.h"
#include "Tools/Logger.h"
#include "PostTreatment.h"

typedef std::pair<unsigned int, unsigned int> segment;
typedef std::map<segment , std::vector<Shape*> > mapSegmentShapes;

void registerSegmentsFromShapes(Shape* shape, mapSegmentShapes& currentMap)
{

    Triangle* triangle = dynamic_cast<Triangle*>(shape);
    if (!shape) { return; }

    std::vector<unsigned int>* vertices = triangle->getLocalVertices();
    for (unsigned int i = 0; i < vertices->size(); i++)
    {
        segment seg1 = std::pair<unsigned int, unsigned int>(vertices->at(i), vertices->at((i + 1) % (vertices->size())));
        mapSegmentShapes::iterator it = currentMap.find(seg1);

        if (it == currentMap.end())
        {
            std::vector<Shape*> newList;
            newList.push_back(shape);
            currentMap.insert(std::pair<segment, std::vector<Shape*> >(seg1, newList));
        }
        else
        {
            it->second.push_back(shape);
        }

        segment seg2 = std::pair<unsigned int, unsigned int>(vertices->at((i + 1) % (vertices->size())), vertices->at(i));
        it = currentMap.find(seg2);

        if (it == currentMap.end())
        {
            std::vector<Shape*> newList;
            newList.push_back(shape);
            currentMap.insert(std::pair<segment, std::vector<Shape*> >(seg2, newList));
        }
        else
        {
            it->second.push_back(shape);
        }
    }

}

bool isAcceptableEdge(const segment& seg, Shape* p1, Shape* p2, decimal& angleOuverture)
{
    /*
     we test if p1 vs p2 angle is greater than PI/2+angleMax
     else they are considered as colinear and no diffraction cylinder is built
               seg
                |
         p1     |p2/
                | /
      __________|/
      comp      \
                 \ normal
                  \

    */

    // Minimal angle (other PI) between two face to allow building of a diffraction cylinder
    float angleMax = AcousticRaytracerConfiguration::get()->AngleDiffMin * M_PI / 180;

    // Compute "mean" normal between the two faces
    vec3 normal = p1->getNormal() + p2->getNormal();
    normal.normalize();

    // Search for a segment of the shape different from seg
    std::vector<unsigned int>* vertices = p1->getLocalVertices();
    vec3 otherVertex;
    for (unsigned int i = 0; i < vertices->size(); i++)
    {
        if (vertices->at(i) != seg.first && vertices->at(i) != seg.second)
        {
            otherVertex = p1->getVertices()->at(vertices->at(i));
            break;
        }
    }

    // Create "comp", a vector colinear with a segment found ahead
    vec3 proj = otherVertex.closestPointOnLine(p1->getVertices()->at(seg.first), p1->getVertices()->at(seg.second));

    vec3 comp = otherVertex - proj;
    comp.normalize();

    // Compute angle betwwen comp and tne "mean" normal
    decimal angle = comp.dot(normal);
    angleOuverture = 2. * acos(angle);

    if (angle < cos(M_PI / 2. + angleMax / 2.))
    {
        return true;
    }

    return false;
}

bool PostTreatment::constructEdge(Scene* scene)
{
    // define diffraction cylinder diameter
    float cylinderThick = AcousticRaytracerConfiguration::get()->CylindreThick;

    // Create a list of segments common to two faces
    mapSegmentShapes segmentList;
    std::vector<Shape*> *shapes = scene->getShapes();

    for (unsigned int i = 0; i < shapes->size(); i++)
    {
        registerSegmentsFromShapes(shapes->at(i), segmentList);
    }

    ss << segmentList.size() << " segments ont ete enregistres." << std::endl;

    std::set<segment> validSegment;

    for (mapSegmentShapes::iterator it = segmentList.begin(); it != segmentList.end(); it++)
    {
        // If segment is owned by 2 shapes, we test angle between them
        if (it->second.size() == 2)
        {
            std::set<segment>::iterator itset1 = validSegment.find(it->first);
            std::set<segment>::iterator itset2 = validSegment.find(std::pair<unsigned int, unsigned int>(it->first.second, it->first.first));
            decimal angleOuverture;
            if (itset1 == validSegment.end() && itset2 == validSegment.end() && isAcceptableEdge(it->first, it->second.at(0), it->second.at(1), angleOuverture))
            {
                validSegment.insert(it->first);
                Cylindre* cylindre = new Cylindre(it->second.at(0), it->second.at(1), scene->getVertices(), it->first.first, it->first.second, cylinderThick);
                cylindre->setAngleOuverture(angleOuverture);
                scene->addShape(cylindre);
                ss << "Ajout du segment (" << it->first.first << "," << it->first.second << ")" << std::endl;
            }
        }
    }

    ss << validSegment.size() << " cylindre ont ete ajoutes." << std::endl;

    return true;
}

#ifdef _ALLOW_TARGETING_
bool PostTreatment::findTargetsForNMPB(Scene* scene, std::vector<Recepteur>& recepteurs, TargetManager& targetManager, decimal density)
{

    std::vector<Shape*> *shapes = scene->getShapes();
    for (unsigned int i = 0; i < shapes->size(); i++)
    {
        if (dynamic_cast<Triangle*>(shapes->at(i)))
        {
            /*Triangle* t = dynamic_cast<Triangle*>(shapes->at(i));
            if(!(t->getMaterial()->isNatural)){
                vec3 normale = t->getNormal();
                vec2 normale2D = vec2(sqrt(normale.x * normale.x + normale.y * normale.y),normale.z);
                normale2D.normalize();
                //Si l'angle de la normale par rapport a l'horizontale est plus faible que 15 degres pour etre coherent avec la definition de surface verticale de la NMPB
                if(acos(normale2D.dot(vec2(1.,0.))) < M_PI / 12.){
                    std::vector<vec3> samples;
                    t->sample(density,samples);
                    targetManager.registerTargets(samples);
                }
            }*/
        }
        else if (dynamic_cast<Cylindre*>(shapes->at(i)))
        {
            Cylindre* c = dynamic_cast<Cylindre*>(shapes->at(i));
            std::vector<vec3> samples;
            c->sample(density, samples);
            targetManager.registerTargets(samples);
        }
    }

    for (unsigned int i = 0; i < recepteurs.size(); i++)
    {
        targetManager.registerTarget(recepteurs.at(i).getPosition());
    }

    return true;
}

void PostTreatment::appendDirectionToSources(TargetManager* targets, std::vector<Source>& sources)
{
    /*for(unsigned int i = 0; i < sources.size(); i++){
        vec3 pos = sources.at(i).getPosition();
        for(unsigned int j = 0; j < targets.size(); j++){
            vec3 newDir = targets.at(j) - pos;
            newDir.normalize();
            sources.at(i).addDirection(newDir);
        }
        sources.at(i).setInitialRayCount(targets.size());
    }*/
    for (unsigned int i = 0; i < sources.size(); i++)
    {
        sources.at(i).setTargetManager(targets);
        sources.at(i).setInitialTargetCount(sources.at(i).getInitialRayCount());
    }
}
#endif
