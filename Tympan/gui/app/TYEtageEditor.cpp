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
 * \file TYEtageEditor.cpp
 * \brief Construit un etage a partir des points saisis
 */


#include <qmessagebox.h>
#include <qdialog.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qbuttongroup.h>
//Added by qt3to4:
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>

#include "Tympan/models/business/TYPreferenceManager.h"
#include "Tympan/models/business/OLocalizator.h"
#include "Tympan/models/business/TYPointControl.h"
#include "Tympan/models/business/TYMaillage.h"
#include "Tympan/models/business/TYCalcul.h"
#include "Tympan/models/business/infrastructure/TYInfrastructure.h"
#include "Tympan/models/business/infrastructure/TYBatiment.h"
#include "Tympan/models/business/infrastructure/TYSiteNode.h"
#include "Tympan/models/business/infrastructure/TYEtage.h"
#include "Tympan/models/business/infrastructure/TYEcran.h"
#include "Tympan/models/common/3d.h"
#include "Tympan/gui/app/TYSiteModelerFrame.h"
#include "Tympan/gui/app/TYBatimentModelerFrame.h"
#include "Tympan/gui/app/TYActions.h"
#include "Tympan/gui/app/TYApplication.h"
#include "Tympan/gui/app/TYMainWindow.h"
#include "Tympan/gui/widgets/TabPointsWidget.h"
#include "TYEtageEditor.h"


#define TR(id) OLocalizator::getString("TYEtageEditor", (id))


TYEtageEditor::TYEtageEditor(TYModelerFrame* pModeler) :
    TYPolyLineEditor(pModeler)
{
    _useAngleStep = true;
    _dispDist = true;

    QObject::connect(this, SIGNAL(endedSavingPoints()), this, SLOT(endEtage()));
}

TYEtageEditor::~TYEtageEditor()
{
}

void TYEtageEditor::endEtage()
{
    bool invalid = false;
    bool onlyEcran = false;
    TYTabPoint tabPts = this->getSavedPoints();

    if ( !(getSavedPoints().size() > 1) || (!_pModeler->askForResetResultat()) )
    {
        return;
    }

    // Test validity of polyline
    if (tabPts.size() >= 3)
    {
       // On teste si la polyligne ne se coupe pas elle-meme...
       invalid = testCrossSegment(tabPts, onlyEcran);
    }
    else
    {
        // Pas assez de points pour faire un etage
        onlyEcran = true;
    }

    if (invalid)
    {
        writeDebugMsg("Etage invalide !!!");
        QMessageBox::warning(_pModeler, TR("id_caption"), TR("id_msg_etage_invalid"));
        return;
    }
    else
    {
        forceNormals(tabPts);
        writeDebugMsg("Etage valide !!!");
    }

    TYEtageEditorPropertiesDlg* pDlg = new TYEtageEditorPropertiesDlg(_pModeler, tabPts, onlyEcran);

    // Affiche la boite de dialogue
    int ret = pDlg->exec();

    if (ret == QDialog::Accepted)
    {
        double hauteur = pDlg->getHauteur();
        double epaisseur = pDlg->getEpaisseur();
        bool closed = pDlg->etageSelected();

        if (hauteur <= 0) { return; }

        // Compute volume's center position
        TYRepere rep;
        computeCenter(pDlg->getHauteurSol(), tabPts, rep);

        if (closed) // Build an etage...
        {
            buildFloor(tabPts, rep, hauteur, closed);
        }
        else // Build an "Ecran"
        {
            buildScreen(tabPts, rep,  hauteur, epaisseur);
        }

        // repasse en mode camera selection
        // XXX The bug goes here
        getTYMainWnd()->setDefaultCameraMode();
    }

}

bool TYEtageEditor::testCrossSegment(TYTabPoint& tabPts, bool &onlyEcran)
{
    TYPoint pt;
    bool invalid(false);
    unsigned int nbPts = tabPts.size();

    for (unsigned int i = 0; (i < nbPts) && !invalid; i++)
    {
        TYSegment seg1(tabPts[i], tabPts[(i + 1) % nbPts]);

        for (unsigned int j = 0; (j < nbPts) && !invalid; j++)
        {
            TYSegment seg2(tabPts[j], tabPts[(j + 1) % nbPts]);
            // Test if at least one vertex is in common
            if ((seg1._ptA != seg2._ptA) && (seg1._ptB != seg2._ptB) && (seg1._ptA != seg2._ptB) && (seg1._ptB != seg2._ptA))
            {
                // Test si il y a intersection
                if (seg1.intersects(seg2, pt, TYSEUILCONFONDUS) != INTERS_NULLE)
                {
                    // Si l'intersection intervient entre les points extremes du tab
                    if ((i == nbPts - 1) || (j == nbPts - 1))
                    {
                        onlyEcran = true;
                    }
                    else
                    {
                        invalid = true;
                    }
                }
            }
        }
    }

    return invalid;
}

void TYEtageEditor::forceNormals(TYTabPoint& tabPts)
{
    if (tabPts.size() > 2)
    {
        // Si l'etage est valide, on teste le sens de construction (pour les etages uniquement):
        // ==> si la somme des produits croises des segments est negative, le sens de construction est correct
        // Sinon, on inverse le sens des points du dernier au premier
        double somme = 0;
        for (unsigned int i=1; i < tabPts.size() - 1; i++)
        {
            OVector3D v1(tabPts[i - 1], tabPts[i]);
            OVector3D v2(tabPts[i], tabPts[i + 1]);
            v1.normalize();
            v2.normalize();

            somme = somme + v1.cross(v2)._z;
        }

        if (somme > 0) // Inversion du sens de construction
        {
            TYTabPoint tabPtsTemp = tabPts;
            tabPts.clear();
            vector<TYPoint>::reverse_iterator it;
            for (it = tabPtsTemp.rbegin(); it != tabPtsTemp.rend(); it++)
            {
                tabPts.push_back((*it));
            }
        }
    }
}

void TYEtageEditor::initZ(TYTabPoint& tabPts)
{
    // L'etage, dans son repere, est en z=0
    for (size_t i = 0; i < tabPts.size(); i++)
    {
        tabPts[i]._z = 0.0;
    }
}

void TYEtageEditor::computeCenter(const double initialOffset, TYTabPoint& tabPts, TYRepere &rep)
{
        size_t nbPts = tabPts.size();
    
        // On calcul le centre de gravite de l'etage pour l'utiliser comme repere dans le bâtiment
        OVector3D centre(0.0, 0.0, 0.0);

        // On effectue la moyenne en (X,Y) des points au sol
        for (size_t i = 0; i < nbPts; i++)
        {
            centre._x += tabPts[i]._x;
            centre._y += tabPts[i]._y;
        }

        // Moyenne
        centre._x /= nbPts;
        centre._y /= nbPts;

        // On translate les points saisies en tenant compte du centre
        for (size_t i = 0; i < nbPts; i++)
        {
            tabPts[i] = OVector3D(tabPts[i]) - centre;
        }

        // Hauteur au sol saisie
        centre._z = initialOffset;

        // Repere de l'etage dans le bâtiment
        rep._origin = centre;
}

void TYEtageEditor::buildFloor(TYTabPoint& tabPts, TYRepere& rep, double hauteur, bool closed)
{
        LPTYEtage pEtage = new TYEtage();

        // Construction des murs
        pEtage->setMurs(tabPts, hauteur, closed);

        // Recuperation ou construction du bâtiment
        TYBatiment* pBat = NULL;
        bool added = false;

        if (QString(_pModeler->metaObject()->className()).compare("TYBatimentModelerFrame") == 0)
        {
            pBat = ((TYBatimentModelerFrame*) _pModeler)->getBatiment();

            // Ajout de l'etage au bâtiment
            if (pBat && (added = pBat->addAcousticVol((LPTYAcousticVolume&) pEtage, rep)))
            {
                // Action
                TYAction* pAction = new TYAddAccVolToAccVolNodeAction((LPTYAcousticVolume&) pEtage, pBat, _pModeler, TR("id_action_addetage"));
                _pModeler->getActionManager()->addAction(pAction);
            }
        }
        else if (QString(_pModeler->metaObject()->className()).compare("TYSiteModelerFrame") == 0)
        {
            pBat = new TYBatiment();
            LPTYInfrastructure pInfra = ((TYSiteModelerFrame*) _pModeler)->getSite()->getInfrastructure();
            pInfra->addBatiment(pBat, rep);

            // Ajout de l'etage au bâtiment
            if (pBat && (added = pBat->addAcousticVol((LPTYAcousticVolume&) pEtage, TYRepere())))
            {
                // Action
                TYAction* pAction = new TYAddElementToInfraAction(pBat, pInfra, _pModeler, TR("id_action_addetage"));
                _pModeler->getActionManager()->addAction(pAction);
            }
            else
            {
                pInfra->remBatiment(pBat);
            }
        }

        if (added && pBat)
        {
            // On ajoute ce bat a la selection du calcul courant
            if (getTYApp()->getCurProjet() && _pModeler->isElementInCurrentProjet())
            {
                LPTYCalcul pCalcul = getTYApp()->getCurProjet()->getCurrentCalcul();

                if (pCalcul)
                {
                    pCalcul->addToSelection(pBat);
                }
            }
            refreshSiteFrame();

            pBat->updateGraphicTree();
            _pModeler->getView()->getRenderer()->updateDisplayList();

            // Update
            _pModeler->updateView();
        }
}

void TYEtageEditor::buildScreen(TYTabPoint& tabPts, TYRepere& rep,  double height, double thickness)
{
        LPTYEcran pEcran = new TYEcran();

        // Construction des murs
        pEcran->setElements(tabPts, height, thickness);

        // Recuperation ou construction du bâtiment
        TYBatiment* pBat = NULL;
        bool added = false;

        if (QString(_pModeler->metaObject()->className()).compare("TYBatimentModelerFrame") == 0)
        {
            pBat = ((TYBatimentModelerFrame*) _pModeler)->getBatiment();

            // Ajout de l'ecran au bâtiment
            if (pBat && (added = pBat->addAcousticVol((LPTYAcousticVolume&) pEcran, rep)))
            {
                // Action
                TYAction* pAction = new TYAddAccVolToAccVolNodeAction((LPTYAcousticVolume&) pEcran, pBat, _pModeler, TR("id_action_addecran"));
                _pModeler->getActionManager()->addAction(pAction);
            }
        }
        else if (QString(_pModeler->metaObject()->className()).compare("TYSiteModelerFrame") == 0)
        {
            pBat = new TYBatiment();
            LPTYInfrastructure pInfra = ((TYSiteModelerFrame*) _pModeler)->getSite()->getInfrastructure();
            pInfra->addBatiment(pBat, rep);

            // Ajout de l'ecran a l'infrastructure...
            if (pBat && (added = pBat->addAcousticVol((LPTYAcousticVolume&) pEcran, TYRepere())))
            {
                // Action
                TYAction* pAction = new TYAddElementToInfraAction(pBat, pInfra, _pModeler, TR("id_action_addecran"));
                _pModeler->getActionManager()->addAction(pAction);
            }
            else
            {
                pInfra->remBatiment(pBat);
            }
        }

        if (added && pBat)
        {
            // On ajoute ce bat a la selection du calcul courant
            if (getTYApp()->getCurProjet() && _pModeler->isElementInCurrentProjet())
            {
                LPTYCalcul pCalcul = getTYApp()->getCurProjet()->getCurrentCalcul();

                if (pCalcul)
                {
                    pCalcul->addToSelection(pBat);
                }
            }
            refreshSiteFrame();

            _pModeler->getView()->getRenderer()->updateDisplayList();

            // Update
            pBat->updateGraphicTree();
            _pModeler->updateView();
        }
}


// ********* PROPERTY DIALOG DEFINITION *********************

TYEtageEditorPropertiesDlg::TYEtageEditorPropertiesDlg(QWidget* parent, TYTabPoint &tabPts, bool onlyEcran) : QDialog(parent)
{
    _pBatBtn = NULL;
    _pEcranBtn = NULL;
    _pEpaisseurLabelName = NULL;
    _pEpaisseurLineEdit = NULL;
    _pHauteurLineEdit = NULL;
    _pHauteurSolLineEdit = NULL;

    this->setWindowTitle(TR("id_caption"));

    QGridLayout* pLayout = new QGridLayout();
    setLayout(pLayout);

    // Hauteur
    _pHauteurLayout = new QHBoxLayout();
    _pHauteurLayout->setMargin(10);
    pLayout->addLayout(_pHauteurLayout, 0, 0);
    QLabel* pHauteurLabelName = new QLabel(this);
    pHauteurLabelName->setText(TR("id_hauteur_label"));
    _pHauteurLayout->addWidget(pHauteurLabelName);
    _pHauteurLineEdit = new QLineEdit(this);

    double hauteur = 5.0;
    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "DefaultHMur"))
    {
        hauteur = TYPreferenceManager::getDouble(TYDIRPREFERENCEMANAGER, "DefaultHMur");
    }
    else
    {
        TYPreferenceManager::setDouble(TYDIRPREFERENCEMANAGER, "DefaultHMur", hauteur);
    }

    _pHauteurLineEdit->setText(QString().setNum(hauteur, 'f', 2));
    _pHauteurLayout->addWidget(_pHauteurLineEdit);

    // Epaisseur pour les ecrans
    _pEpaisseurLayout = new QHBoxLayout();
    _pEpaisseurLayout->setMargin(10);
    pLayout->addLayout(_pEpaisseurLayout, 1, 0);
    _pEpaisseurLabelName = new QLabel(this);
    _pEpaisseurLabelName->setText(TR("id_epaisseur_ecran_label"));
    _pEpaisseurLayout->addWidget(_pEpaisseurLabelName);
    _pEpaisseurLineEdit = new QLineEdit(this);
    _pEpaisseurLineEdit->setText(QString().setNum(0.0, 'f', 2));

    double epaisseur = 0.3;
    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "DefaultWidthEcran"))
    {
        epaisseur = TYPreferenceManager::getDouble(TYDIRPREFERENCEMANAGER, "DefaultWidthEcran");
    }
    else
    {
        TYPreferenceManager::setDouble(TYDIRPREFERENCEMANAGER, "DefaultWidthEcran", epaisseur);
    }

    _pEpaisseurLineEdit->setText(QString().setNum(epaisseur, 'f', 2));

    _pEpaisseurLayout->addWidget(_pEpaisseurLineEdit);
    _pEpaisseurLabelName->setEnabled(false);
    _pEpaisseurLineEdit->setEnabled(false);

    // Hauteur par rapport au sol
    _pHauteurSolLayout = new QHBoxLayout();
    _pHauteurSolLayout->setMargin(10);
    pLayout->addLayout(_pHauteurSolLayout, 2, 0);
    QLabel* pHauteurSolLabelName = new QLabel(this);
    pHauteurSolLabelName->setText(TR("id_hauteur_sol_label"));
    _pHauteurSolLayout->addWidget(pHauteurSolLabelName);
    _pHauteurSolLineEdit = new QLineEdit(this);
    _pHauteurSolLineEdit->setText(QString().setNum(0.0, 'f', 2));
    _pHauteurSolLayout->addWidget(_pHauteurSolLineEdit);

    // Ecran ou bâtiment
    _pEcranOuBatLayout = new QHBoxLayout();
    _pEcranOuBatLayout->setMargin(10);
    pLayout->addLayout(_pEcranOuBatLayout, 3, 0);

    QButtonGroup* pEcranOuBatBtnGroup = new QButtonGroup();
    pEcranOuBatBtnGroup->setExclusive(true);
    _pBatBtn = new QRadioButton(TR("id_bat_label"));
    pEcranOuBatBtnGroup->addButton(_pBatBtn, 0);
    _pEcranBtn = new QRadioButton(TR("id_ecran_label"));
    pEcranOuBatBtnGroup->addButton(_pEcranBtn, 1);
    _pBatBtn->setChecked(true);

    QGridLayout* groupBoxEcranOuBatLayout = new QGridLayout();
    groupBoxEcranOuBatLayout->addWidget(_pBatBtn, 0, 0);
    groupBoxEcranOuBatLayout->addWidget(_pEcranBtn, 0, 1);

    QGroupBox* groupBoxEcranOuBat = new QGroupBox();
    groupBoxEcranOuBat->setTitle(TR(""));
    groupBoxEcranOuBat->setLayout(groupBoxEcranOuBatLayout);

    _pEcranOuBatLayout->addWidget(groupBoxEcranOuBat);

    if (onlyEcran)
    {
        _pEcranBtn->setChecked(true);
        _pEcranBtn->setEnabled(false);
        _pBatBtn->setEnabled(false);
        _pEpaisseurLabelName->setEnabled(true);
        _pEpaisseurLineEdit->setEnabled(true);
    }

    QObject::connect(pEcranOuBatBtnGroup, SIGNAL(buttonClicked(int)), this, SLOT(toggleButton(int)));

    // Geometrie
    _tabPtsW = new TabPointsWidget(tabPts, this);
    _tabPtsW->update();
    QHBoxLayout *tabPointsLayout = new QHBoxLayout();
    tabPointsLayout->setMargin(10);
    pLayout->addLayout(tabPointsLayout, 4, 0);
    tabPointsLayout->addWidget(_tabPtsW);





    // Btns Ok et Cancel
    _pBtnLayout = new QHBoxLayout();
    pLayout->addLayout(_pBtnLayout, 5, 0);

    _pBtnLayout->addStretch(1);

    // Ok
    QPushButton* pButtonOK = new QPushButton(TR("id_ok_btn"), this);
    pButtonOK->setDefault(true);
    QObject::connect(pButtonOK, SIGNAL(clicked()), this, SLOT(accept()));
    _pBtnLayout->addWidget(pButtonOK);

    // Cancel
    QPushButton* pButtonCancel = new QPushButton(TR("id_cancel_btn"), this);
    pButtonCancel->setShortcut(Qt::Key_Escape);
    QObject::connect(pButtonCancel, SIGNAL(clicked()), this, SLOT(reject()));
    _pBtnLayout->addWidget(pButtonCancel);

}

TYEtageEditorPropertiesDlg::~TYEtageEditorPropertiesDlg()
{
    //  QObject::disconnect(_pBatBtn, SIGNAL(clicked ()), this, SLOT(toggleEtage()));

    delete _pHauteurLayout;
    delete _pHauteurSolLayout;
    delete _pEpaisseurLayout;
    delete _pEcranOuBatLayout;
    delete _pBtnLayout;
    _pBatBtn = NULL;
    _pEcranBtn = NULL;
    _pEpaisseurLabelName = NULL;
    _pEpaisseurLineEdit = NULL;
    _pHauteurLineEdit = NULL;
    _pHauteurSolLineEdit = NULL;
}

void TYEtageEditorPropertiesDlg::toggleButton(int i)
{
    if (_pEcranBtn->isChecked())
    {
        _pEpaisseurLabelName->setEnabled(true);
        _pEpaisseurLineEdit->setEnabled(true);
    }
    else
    {
        _pEpaisseurLabelName->setEnabled(false);
        _pEpaisseurLineEdit->setEnabled(false);
    }
}

void TYEtageEditorPropertiesDlg::accept()
{
    _tabPtsW->apply();
    QDialog::accept();
}
