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

#ifndef __TY_DIRECTIVITY__
#define __TY_DIRECTIVITY__


#include "Tympan/models/business/TYElement.h"
#include "Tympan/models/business/acoustic/TYSpectre.h"

struct ConstraintDirectivityElement
{
    double Theta;
    double Phi;
    TYSpectre Coefficients;

    ConstraintDirectivityElement& operator=(const ConstraintDirectivityElement& other)
    {
        if (this != &other)
        {
            Theta = other.Theta;
            Phi = other.Phi;
            Coefficients = other.Coefficients;
        }
        return *this;
    }

    ///Operateur de comparaison.
    bool operator==(const ConstraintDirectivityElement& other) const
    {
        if (this != &other)
        {
            if (Theta != other.Theta) { return false; }
            if (Phi != other.Phi) { return false; }
            if (Coefficients != other.Coefficients) { return false; }
        }
        return true;
    }
};

typedef std::vector<ConstraintDirectivityElement> ConstraintDirectivityDefinition;

/**
 * Classe pour definir la directivite d'une source acoustique ponctuelle.
 */
class TYDirectivity: public TYElement
{
    OPROTODECL(TYDirectivity)
    TY_EXTENSION_DECL_ONLY(TYDirectivity)

    // Methodes
public:
    /**
     * Constructeurs
     */
    TYDirectivity();
    TYDirectivity(OVector3D& vec) : DirectivityVector(vec) {}
    /**
     * Constructeur par copie.
     */
    TYDirectivity(const TYDirectivity& other);
    /**
     * Destructeur.
     */
    virtual ~TYDirectivity();

    ///Operateur =.
    TYDirectivity& operator=(const TYDirectivity& other);
    ///Operateur ==.
    bool operator==(const TYDirectivity& other) const;
    ///Operateur !=.
    bool operator!=(const TYDirectivity& other) const;

    virtual bool deepCopy(const TYElement* pOther, bool copyId = true);

    virtual std::string toString() const;

    virtual DOM_Element toXML(DOM_Element& domElement);
    virtual int fromXML(DOM_Element domElement);

public :
    OVector3D DirectivityVector;

};

class TYUserDefinedDirectivity : public TYDirectivity
{
public:
    TYUserDefinedDirectivity() : TYDirectivity() {}
    TYUserDefinedDirectivity( const TYUserDefinedDirectivity& other );
    TYUserDefinedDirectivity(OVector3D& vec) : TYDirectivity(vec) {}
    ~TYUserDefinedDirectivity() {}

    virtual DOM_Element toXML(DOM_Element& domElement);
    virtual int fromXML(DOM_Element domElement);
    void LoadFromCsv(std::string fileName);
    void setDirectivityTable( const ConstraintDirectivityDefinition& table_ ) { DirectivityTable = table_; };
    ConstraintDirectivityDefinition&  getDirectivityTable() { return  DirectivityTable; }
    void buildSolverTable(int sampleStep, void* tableToBuild);

private :
    ConstraintDirectivityElement readValuesFromCSV(std::deque<double>& line_array);

    ConstraintDirectivityDefinition DirectivityTable;
};

class TYComputedDirectivity : public TYDirectivity
{
public:
    enum
    {
        Surface,    /*!< 0 - Sources situees sur une face. */
        Baffled,    /*!< 1 - Sources bouches de ventilation. */
        Chimney,    /*!< 2 - Sources cheminees. */
    };

    TYComputedDirectivity() :   TYDirectivity(),
                                Type(Surface),
                                SpecificSize(1.) {}
    YComputedDirectivity(OVector3D& vec, int type, double size) :
        TYDirectivity(vec),
        Type(type),
        SpecificSize(size) {}
    TYComputedDirectivity( const TYComputedDirectivity& directivity );
    ~TYComputedDirectivity() {}
    virtual DOM_Element toXML(DOM_Element& domElement);
    virtual int fromXML(DOM_Element domElement);

    int Type;
    double SpecificSize; /*!< Support (face/volume) size */
};


#endif // __TY_DIRECTIVITY__
