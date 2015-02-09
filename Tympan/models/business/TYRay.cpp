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

#include <deque>
#include "TYRay.h"
#if TY_USE_IHM
  #include "Tympan/gui/gl/TYRayGraphic.h"
#endif

TY_EXT_GRAPHIC_INST(TYRay);

#include "TYRay.h"

TYRay::TYRay() : acoustic_path()
{
    _source = NULL;
    _recepteur = NULL;
}

TYRay::TYRay(const TYRay& ray)
{
    *this = ray;
}

TYRay::~TYRay()
{
}

TYRay& TYRay::operator=(const TYRay& other)
{
    TYElement::operator= (other);
    acoustic_path::operator=(other);
    _source = other._source;
    _recepteur = other._recepteur;

    return *this;
}

bool TYRay::deepCopy(TYRay* pOther, bool copyId)
{
    assert(pOther);

    TYElement::deepCopy(pOther, copyId);
    acoustic_path::deepCopy(pOther);

    _source = pOther->_source;
    _recepteur = pOther->_recepteur;

    return true;
}

DOM_Element TYRay::toXML(DOM_Element& domElement)
{
    DOM_Element domNewElem = TYElement::toXML(domElement);
    
    // Sauvegarde du nombre d'evènemenents
    TYXMLTools::addElementUIntValue(domNewElem, "nb_events", _events.size());

    // Sauvegarde du type des evenements
    // Cree un nouveau noeud pour l'ensemble des evenements
    DOM_Document domDoc = domElement.ownerDocument();
    DOM_Element events_type_list = domDoc.createElement("events_type");
    domNewElem.appendChild(events_type_list);

    // Formatage des donnees
    std::ostringstream oss;
    for (size_t i = 0; i < _events.size() - 1; ++i)
    {
        oss << intToStr(_events[i]->type) << ' ';
    }
    oss << intToStr(_events[_events.size() - 1]->type);
    DOMString tab(oss.str().c_str());

    // Ajout des donnees
    QDomText data = domDoc.createTextNode(tab);

    events_type_list.appendChild(data);

    // Sauvegarde de la position des evenements
    // Cree un nouveau noeud pour l'ensemble des evenements
    DOM_Element listPosNode = domDoc.createElement("ListPosEvents");
    domNewElem.appendChild(listPosNode);
    TYPoint p;
    for (size_t i = 0; i<_events.size(); i++)
    {
        p = _events[i]->pos;
        p.toXML(listPosNode);
    }

    return domNewElem;
}

int TYRay::fromXML(DOM_Element domElement)
{
    TYElement::fromXML(domElement);

    DOM_Element elemCur;
    bool nbEventsOk = false;
    std::deque<int> event_kind_tab;
    std::deque<TYPoint> _listPoints;
    size_t nb_events = 0;
    int retVal = 0;
    _events.clear();

    QDomNodeList childs = domElement.childNodes();

    for (unsigned int i = 0; i < childs.length(); i++)
    {
        elemCur = childs.item(i).toElement();

        TYXMLTools::getElementUIntValue(elemCur, "nb_events", nb_events, nbEventsOk);

        if (elemCur.nodeName() == "events_type" && nbEventsOk)
        {
            // On recupere les valeurs
            QString tab;
            TYXMLTools::getElementStringValue(elemCur, "events_type", tab);
            std::istringstream iss(tab.toAscii().data());

            int type_event;
            for (unsigned int i = 0; i < nb_events; ++i)
            {
                iss >> type_event;
                event_kind_tab.push_back(type_event);
            }
        }
        else if (elemCur.nodeName() == "ListPosEvents")
        {
            DOM_Element elemCur2;
            QDomNodeList childs2 = elemCur.childNodes();

            for (size_t j = 0; j < childs2.length(); j++)
            {
                TYPoint pt;
                elemCur2 = childs2.item(j).toElement();
                if (pt.callFromXMLIfEqual(elemCur, &retVal))
                {
                    if (retVal == 1) { _listPoints.push_back(pt); }
                }            
            }
        }
    }

    // Reconstitution des evenements
    assert (event_kind_tab.size() == _listPoints.size() == nb_events);
    for (size_t i=0; i<nb_events; i++)
    {
        acoustic_event *ev = new acoustic_event(_listPoints.at(i));
        ev->type = static_cast<ACOUSTIC_EVENT_TYPES>(event_kind_tab.at(i));

        _events.push_back(ev);
    }

    return 1;
}