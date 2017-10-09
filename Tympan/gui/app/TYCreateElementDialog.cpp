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
 * \file TYCreateElementDialog.cpp
 * \brief Boite de dialogue pour la creation d'un nouvel element metier. L'element cree peut etre sauvegarde en BDD ou exporter en XML.
 */


#include <qlayout.h>
#include <qmessagebox.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlistwidget.h>

#include "Tympan/core/exceptions.h"
#include "Tympan/core/logging.h"
#include "Tympan/models/business/TYPreferenceManager.h"
#include "Tympan/models/business/OLocalizator.h"
#include "Tympan/gui/app/TYOpenElementDialog.h"
#include "TYCreateElementDialog.h"



#define TR(id) OLocalizator::getString("TYCreateElementDialog", (id))

#ifdef _DEBUG
///Le nombre d'elements selectionnables.
#if WITH_NMPB
static const int nbMainElts = 60;
#else
static const int nbMainElts = 59;
#endif

///Le type des elements selectionnables.
static char const* _listMainElement[] =
{
    "TYAcousticBox",           "TYAcousticCircle",       "TYAcousticCylinder",    "TYAcousticLine",         "TYAcousticPolygon",    /*01*/
    "TYAcousticRectangleNode", "TYAcousticRectangle",    "TYAcousticSemiCircle",  "TYAcousticSemiCylinder", "TYAcousticSurfaceNode",/*02*/
    "TYAcousticSurface",       "TYBoucheSurface",        "TYChemineeSurface",     "TYAcousticVolumeNode",   "TYAcousticVolume",     /*03*/
    "TYAltimetrie",            "TYAttenuateur",         "TYBatiment",             "TYBox",                /*04*/
    "TYCalcul",                "TYCourbeNiveau",         "TYCoursEau",            "TYDalle",                "TYDirectivity",        /*05*/
    "TYElement",               "TYEtage",                "TYInfrastructure",      "TYLinearMaillage",       "TYMachine",            /*06*/
    "TYMaillage",              "TYMateriauConstruction", "TYMurElement",          "TYMur",                  "TYParoi",              /*07*/
    "TYPlanEau",               "TYPointCalcul",          "TYPointControl",        "TYPoint",                "TYPolygon",            /*08*/
    "TYProjet",                "TYRectangle",            "TYRectangularMaillage", "TYRepere",               "TYReseauTransport",    /*09*/
    "TYResultat",
#if WITH_NMPB
    "TYRoute",
#endif
    "TYSegment",             "TYSiteNode",             "TYSol",                /*10*/
    "TYSourceLineic",          "TYSourcePonctuelle",     "TYSourceBafflee",       "TYSourceCheminee",       "TYSourceSurfacic",     /*11*/
    "TYSource",                "TYSpectre",              "TYTerrain",             "TYTopographie",          "TYTrafic",             /*12*/
    "TYVegetation"
};
#else // _DEBUG
///Le nombre d'elements selectionnables.
static const int nbMainElts = 13;

///Le type des elements selectionnables.
static char const* _listMainElement[] =
{
    "TYProjet",      "TYSiteNode",             "TYBatiment",      "TYMachine",       "TYSpectre",   /*1*/
    "TYAttenuateur", "TYMateriauConstruction", "TYSol",           "TYParoi",         /*2*/
    "TYVegetation",  "TYSourcePonctuelle",     "TYSourceBafflee", "TYSourceCheminee"
};
#endif // _DEBUG


TYCreateElementDialog::TYCreateElementDialog(QWidget* parent, const char* name, Qt::WFlags f):
    QDialog(parent, f)
{
    setObjectName(name);
    setWindowTitle(TR("id_caption"));

    // Layout
    QGridLayout* pLayout = new QGridLayout();
    setLayout(pLayout);

    _pElementChoiceListBox = new QListWidget(this);
    _pElementChoiceListBox->setSelectionMode(QAbstractItemView::SingleSelection);
    pLayout->addWidget(_pElementChoiceListBox, 0, 0);

    // On rempli la liste
    for (int i = 0; i < nbMainElts; i++)
    {
        _pElementChoiceListBox->insertItem(i, new QListWidgetItem(OLocalizator::getString("DisplayName", _listMainElement[i])));
    }

    QBoxLayout* pBtnLayout = new QHBoxLayout();
    pLayout->addLayout(pBtnLayout, 1, 0);

    pBtnLayout->addStretch(1);

    QPushButton* pButtonOK = new QPushButton(TR("id_ok_btn"), this);
    pButtonOK->setDefault(true);
    QObject::connect(pButtonOK, SIGNAL(clicked()), this, SLOT(accept()));
    pBtnLayout->addWidget(pButtonOK);

    QPushButton* pButtonCancel = new QPushButton(TR("id_cancel_btn"), this);
    pButtonCancel->setShortcut(Qt::Key_Escape);
    QObject::connect(pButtonCancel, SIGNAL(clicked()), this, SLOT(reject()));
    pBtnLayout->addWidget(pButtonCancel);


    TYPreferenceManager::loadGeometryFromPreferences(metaObject()->className(), this);
}

TYCreateElementDialog::~TYCreateElementDialog()
{
    TYPreferenceManager::saveGeometryToPreferences(metaObject()->className(), this);
}

void TYCreateElementDialog::createElement(QString eltType)
{
    LPTYElement pElt;
    try
    {
        pElt = dynamic_cast<TYElement*>(TYElement::findAndClone(eltType.toLatin1()));
    }
    catch(tympan::invalid_data& exc)
    {
        pElt = nullptr;
    }

    if (pElt != nullptr)
    {
        TYOpenElementDialog* pOpenDlg = new TYOpenElementDialog(this);
        pOpenDlg->openElement(pElt);
    }
}

void TYCreateElementDialog::done(int r)
{
    if (r == Accepted)
    {
        createElement(_listMainElement[_pElementChoiceListBox->currentRow()]);
    }

    QDialog::done(r);
}
