#include "Tympan/geometric_methods/AcousticRaytracer/Geometry/Shape.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Geometry/Cylindre.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Acoustic/Recepteur.h"
#include "TYANIME3DConversionTools.h"

namespace Tools
{

acoustic_event build_from_RayEvent(const Event *rev)
{
    acoustic_event tyrev;

    Cylindre* cyl = NULL;
    switch (rev->getType())
    {
        case SPECULARREFLEXION :
            if (rev->getShape()->isSol())
            {
                tyrev.type = TYREFLEXIONSOL;
            }
            else
            {
                tyrev.type = TYREFLEXION;
            }

            tyrev.idFace1 = rev->getShape()->getPrimitiveId();

            break;

        case DIFFRACTION:
            tyrev.type = TYDIFFRACTION;
            cyl = dynamic_cast<Cylindre*>(const_cast<Event *>(rev)->getShape());
            if (cyl)
            {
                tyrev.idFace1 = cyl->getFirstShape()->getPrimitiveId();
                tyrev.idFace2 = cyl->getSecondShape()->getPrimitiveId();
            }
            break;

        default:
            tyrev.type = TY_NO_TYPE;
            break;
    }

    tyrev.pos = OPoint3D(rev->getPosition().x, rev->getPosition().y, rev->getPosition().z);
    tyrev.angle = rev->getAngle();

    return tyrev;
}

void build_event_list_from_Ray(int sens, Ray* ray, acoustic_path& ap)
{
    //Definition des Evenements.
    acoustic_event* e = NULL;

    if (sens == 1) // Rays traveled from receptor to source
    {
        //Add source as an event (the receptor is the source)
        e = new acoustic_event();
        e->type = TYSOURCE;
        Recepteur* recep = (static_cast<Recepteur*>(ray->getRecepteur()));
        e->pos = OPoint3D(recep->getPosition().x, recep->getPosition().y, recep->getPosition().z);
        e->angle = 0.0;
        ap.getEvents().push_back(e);

        //Creation des evenements de diffractions et reflexions
        std::vector<std::shared_ptr<Event> >::reverse_iterator rit;

        for (rit = ray->getEvents()->rbegin(); rit != ray->getEvents()->rend(); rit++)
        {
            e = new acoustic_event( build_from_RayEvent( (*rit).get() ) );
            ap.getEvents().push_back(e);
        }

        // Add receptor as an event (the source is the reeptor)
        e = new acoustic_event();
        e->type = TYRECEPTEUR;
        e->pos = OPoint3D(ray->source->getPosition().x, ray->source->getPosition().y, ray->source->getPosition().z);
        e->angle = 0.0;
        ap.getEvents().push_back(e);
    }
    else // Rays traveled from source to receptor (normal
    {
        //Add source as an event
        e = new acoustic_event();
        e->type = TYSOURCE;
        e->pos = OPoint3D(ray->source->getPosition().x, ray->source->getPosition().y, ray->source->getPosition().z);
        e->angle = 0.0;
        ap.getEvents().push_back(e);

        //Creation des evenements de diffractions et reflexions
        std::vector<std::shared_ptr<Event> >::iterator rit;

        for (rit = ray->getEvents()->begin(); rit != ray->getEvents()->end(); rit++)
        {
            e = new acoustic_event( build_from_RayEvent( (*rit).get() ) );
            ap.getEvents().push_back(e);
        }

        // Add receptor as an event
        e = new acoustic_event();
        e->type = TYRECEPTEUR;
        Recepteur* recep = (static_cast<Recepteur*>(ray->getRecepteur()));
        e->pos = OPoint3D(recep->getPosition().x, recep->getPosition().y, recep->getPosition().z);
        e->angle = 0.0;
        ap.getEvents().push_back(e);
    }

}

acoustic_path build_from_Ray(int sens, Ray* ray)
{
    acoustic_path tyRay;
    build_event_list_from_Ray(sens, ray, tyRay);

    return tyRay;
}

}
