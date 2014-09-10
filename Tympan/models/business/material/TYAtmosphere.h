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

/*
 *
 */

#ifndef __TY_ATMOSPHERE__
#define __TY_ATMOSPHERE__

#include "Tympan/models/business/TYElement.h"
#include "Tympan/models/business/acoustic/TYSpectre.h"

/**
 * Classe de definition d'une atmosphere.
 */
class TYAtmosphere: public TYElement
{
    OPROTODECL(TYAtmosphere)
    TY_EXTENSION_DECL_ONLY(TYAtmosphere)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYAtmosphere();
    /**
     * Constructeur par copie.
     */
    TYAtmosphere(const TYAtmosphere& other);
    /**
     * Destructeur.
     */
    virtual ~TYAtmosphere();

    ///Operateur =.
    TYAtmosphere& operator=(const TYAtmosphere& other);
    ///Operateur ==.
    bool operator==(const TYAtmosphere& other) const;
    ///Operateur !=.
    bool operator!=(const TYAtmosphere& other) const;

    virtual bool deepCopy(const TYElement* pOther, bool copyId = true);

    virtual std::string toString() const;

    virtual DOM_Element toXML(DOM_Element& domElement);
    virtual int fromXML(DOM_Element domElement);

    /**
     * Set/Get de la pression.
     */
    double getPression() const { return _pression; }
    /**
     * Set/Get de la pression.
     */
    void setPression(double pression) { _pression = pression; }

    /**
     * Set/Get de la temperature.
     */
    double getTemperature() const { return _temperature; }
    /**
     * Set/Get de la temperature.
     */
    void setTemperature(double temp) { _temperature = temp; }

    /**
     * Set/Get de l'hygrometrie.
     */
    double getHygrometrie() const { return _hygrometrie; }
    /**
     * Set/Get de l'hygrometrie.
     */
    void setHygrometrie(double hygro) { _hygrometrie = hygro; }

    /**
     * Get/Set de la vitesse du son
     */
    double getVitSon() const;

    /**
     * Set/Get du spectre d'attenuation.
     */
    TYSpectre getSpectreAtt();

    // Membres
protected:
    ///Pression en Pascal.
    double _pression;
    ///Temperature en degres Celsius.
    double _temperature;
    ///Hygrometrie en pourcentage d'humidite relative.
    double _hygrometrie;
};


#endif // __TY_ATMOSPHERE__
