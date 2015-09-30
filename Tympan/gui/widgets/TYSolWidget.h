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

/**
* \file TYSolWidget.h
* \brief outil IHM pour un sol (fichier header)
* \author Projet_Tympan
*
*/

#ifndef __TY_SOL_WIDGET__
#define __TY_SOL_WIDGET__

#include "TYWidget.h"
#include <QGridLayout>
#include <QLabel>

class TYSol;
class QLineEdit;
class QGridLayout;
class QLabel;
class QCheckBox;
class QGroupBox;
class QPushButton;
class TYElementWidget;

/**
* \class TYSolWidget
* \brief classe de l'objet IHM pour un sol
*/
class TYSolWidget : public TYWidget
{
	Q_OBJECT

		TY_DECL_METIER_WIDGET(TYSol)

		// Methodes
public:
	/**
	* Constructeur.
	*/
	TYSolWidget(TYSol* pElement, QWidget* _pParent = NULL);
	/**
	* Destructeur.
	*/
	virtual ~TYSolWidget();


	public slots:
		virtual void updateContent();
		virtual void apply();
		void editResistivite();

		// Membres
protected:

	QGroupBox* _groupBox;
	QLineEdit* _lineEditEpaisseur;
	QLineEdit* _lineEditResistivite;
	QLineEdit* _lineEditEcartType;
	QLineEdit* _lineEditLongueur;
	QLabel* _labelEpaisseur;
	QLabel* _labelResistivite;
	QLabel* _labelEcartType;
	QLabel* _labelLongueur;

	QPushButton* _pushButtonResistivite;

protected:
	QGridLayout* _solLayout;
	QGridLayout* _groupBoxLayout;

	TYElementWidget* _elmW;
};


#endif // __TY_SOL_WIDGET__
