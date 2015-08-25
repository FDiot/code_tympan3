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

#ifndef __TY_SOL__
#define __TY_SOL__

#include "TYVegetation.h"

/**
* Classe de definition d'un sol.
*/
class TYSol: public TYElement
{
	OPROTODECL(TYSol)
		TY_EXTENSION_DECL_ONLY(TYSol)

		// Methodes
public:
	/**
	* Constructeur.
	*/
	TYSol();
	/**
	* Constructeur par copie.
	*/
	TYSol(const TYSol& other);
	/**
	* Destructeur.
	*/
	virtual ~TYSol();

	///Operateur =.
	TYSol& operator=(const TYSol& other);
	///Operateur ==.
	bool operator==(const TYSol& other) const;
	///Operateur !=.
	bool operator!=(const TYSol& other) const;

	virtual bool deepCopy(const TYElement* pOther, bool copyId = true);

	virtual std::string toString() const;

	virtual DOM_Element toXML(DOM_Element& domElement);
	virtual int fromXML(DOM_Element domElement);

	/**
	* Set/Get de la resistivite.
	*/
	double getResistivite() const { return _resistivite; }

	/**
	* Set/Get de la resistivite.
	*/
	void setResistivite(double res) { _resistivite = res; }

	/**
	* Set/Get de l'epaisseur.
	*/
	double getEpaisseur() const { return _epaisseur; }

	/**
	* Set/Get de l'epaisseur.
	*/
	void setEpaisseur(double epais) { _epaisseur = epais; }

	/**
	* Set/Get de l'ecart type
	*/
	double getEcartType() { return _ecarttype;}
	void setEcartType(double ecarttype) {_ecarttype = ecarttype;}

	/**
	* Set/Get de la longueur
	*/
	double getLongueur(){return _longueur;}
	void setLongueur(double longueur){_longueur = longueur;}

	// Membres
protected:
	///Resistance a l'ecoulement.
	double _resistivite;

	///Epaisseur du sol.
	double _epaisseur;

	///Ecart-type
	double _ecarttype;

	///Longueur 
	double _longueur;


};

#endif // __TY_SOL__
