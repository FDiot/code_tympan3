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
 * \file TYCalculWidget.cpp
 * \brief outil IHM pour un calcul
 *
 *
 */



#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHIHM.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER
#include <qtablewidget.h>
#include <qtabwidget.h>
#include <qradiobutton.h>
#include <qbuttongroup.h>
#include <qmenu.h>
#include <qstringlist.h>
#include <qcombobox.h>
//Added by qt3to4:
#include <QLabel>
#include <QGridLayout>
#include <QFrame>
#include <QDateEdit>
#include <QTextEdit>
#include <QTreeWidgetItem>


#include "Tympan/Tools/OLocalizator.h"
#include "Tympan/MetierSolver/DataManagerMetier/Commun/TYProjet.h"
#include "Tympan/MetierSolver/DataManagerMetier/Commun/TYCalcul.h"
#include "Tympan/MetierSolver/DataManagerMetier/Commun/TYProjet.h"
#include "Tympan/MetierSolver/DataManagerMetier/Commun/TYLinearMaillage.h"
#include "Tympan/MetierSolver/DataManagerMetier/Commun/TYRectangularMaillage.h"
#include "Tympan/MetierSolver/DataManagerCore/TYPluginManager.h"
#include "Tympan/GraphicIHM/DataManagerIHM/TYEtatsWidget.h"

#if defined(WIN32)
#include <crtdbg.h>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#endif
#endif


#define TR(id) OLocalizator::getString("TYCalculWidget", (id))


TYCalculWidget::TYCalculWidget(TYCalcul* pElement, QWidget* _pParent /*=NULL*/):
    TYWidget(pElement, _pParent)
{
    QString num;
    short iln = 0; // Numero de ligne pour l'implantation des elements (evite de tout decaler a la main quand on insere un element


    resize(300, 690);
    setWindowTitle(TR("id_caption"));

    _calculLayout = new QGridLayout();
    setLayout(_calculLayout);

    // Les elements courant du calcul
    _elmW = new TYElementWidget(pElement, this);
    _calculLayout->addWidget(_elmW, iln++, 0);

    // BLOCAGE DU CALCUL
    _buttonGroupState = new QButtonGroup(this);
    _buttonGroupState->setExclusive(true);
    _pRadioButtonActif = new QRadioButton(TR("id_actif"));
    _buttonGroupState->addButton(_pRadioButtonActif, 0);
    _pRadioButtonLocked = new QRadioButton(TR("id_locked"));
    _buttonGroupState->addButton(_pRadioButtonLocked, 1);

    QGridLayout* groupBoxStateLayout = new QGridLayout();
    groupBoxStateLayout->addWidget(_pRadioButtonActif, 0, 0);
    groupBoxStateLayout->addWidget(_pRadioButtonLocked, 0, 1);

    QGroupBox* groupBoxState = new QGroupBox();
    groupBoxState->setTitle(TR("id_etat_calcul"));
    groupBoxState->setLayout(groupBoxStateLayout);

    _calculLayout->addWidget(groupBoxState, iln++, 0);

    // CHOIX DE LA METHODE DE CALCUL
    QGroupBox* groupBoxCalcMethod = new QGroupBox(this);
    groupBoxCalcMethod->setTitle(TR(""));
    QGridLayout* groupBoxCalcMethodLayout = new QGridLayout();
    groupBoxCalcMethod->setLayout(groupBoxCalcMethodLayout);

    QLabel* labelCalcMethod = new QLabel(groupBoxCalcMethod);
    labelCalcMethod->setText(TR("id_calc_method_label"));
    groupBoxCalcMethodLayout->addWidget(labelCalcMethod, 0, 0);

    // 20070913 - suite au plassage aux plugins, changement du combo en une ligne de texte
    _comboSolver = new QComboBox(groupBoxCalcMethod);
    _comboSolver->setEditable(false);

    groupBoxCalcMethodLayout->addWidget(_comboSolver, 0, 1);

    _calculLayout->addWidget(groupBoxCalcMethod, iln++, 0);

    // PARAMETRES DU CALCUL

    _tabWidget = new QTabWidget(this);

    // DEFINITION DE L'ONGLET CALCUL

    _groupBoxFlag = new QGroupBox(_tabWidget);
    _groupBoxFlag->setTitle(TR(""));
    QGridLayout* groupBoxFlagLayout = new QGridLayout();
    _groupBoxFlag->setLayout(groupBoxFlagLayout);


    // Utilisation de l'atmosphere
    _labelUseAtmosphere = new QLabel(_groupBoxFlag);
    _labelUseAtmosphere->setText(TR("id_useatmosphere_label"));
    groupBoxFlagLayout->addWidget(_labelUseAtmosphere, 0, 0);

    _checkBoxUseAtmosphere = new QCheckBox(_groupBoxFlag);
    _checkBoxUseAtmosphere->setText(TR(""));
    groupBoxFlagLayout->addWidget(_checkBoxUseAtmosphere, 0, 1);

    QButtonGroup* buttonGroupSolRefReel = new QButtonGroup();
    _pRadioButtonSolReel = new QRadioButton(TR("id_usesol_reel_label"));
    buttonGroupSolRefReel->addButton(_pRadioButtonSolReel, 0);
    _pRadioButtonSolReflechissant = new QRadioButton(TR("id_usesol_refl_label"));
    buttonGroupSolRefReel->addButton(_pRadioButtonSolReflechissant, 1);

    QGridLayout* groupBoxSolRefReelLayout = new QGridLayout();
    groupBoxSolRefReelLayout->addWidget(_pRadioButtonSolReel, 0, 0);
    groupBoxSolRefReelLayout->addWidget(_pRadioButtonSolReflechissant, 0, 1);

    QGroupBox* groupBoxSolRefReel = new QGroupBox();
    groupBoxSolRefReel->setTitle(TR(""));
    groupBoxSolRefReel->setLayout(groupBoxSolRefReelLayout);

    groupBoxFlagLayout->addWidget(groupBoxSolRefReel, 1, 0, 1, 2);

    // Choix du type de calcul avec sol
    QLabel* labelTypeCalcul = new QLabel(_groupBoxFlag);
    labelTypeCalcul->setText(TR("id_label_sol_calc"));
    groupBoxFlagLayout->addWidget(labelTypeCalcul, 1, 2);
    _comboBoxTypeSol = new QComboBox(_groupBoxFlag);
    groupBoxFlagLayout->addWidget(_comboBoxTypeSol, 1, 3);
    updateBoxSol();

    // Calcul avec vegetation
    _labelUseVegetation = new QLabel(_groupBoxFlag);
    _labelUseVegetation->setText(TR("id_usevegetation_label"));
    groupBoxFlagLayout->addWidget(_labelUseVegetation, 2, 0);

    _checkBoxUseVegetation = new QCheckBox(_groupBoxFlag);
    _checkBoxUseVegetation->setText(TR(""));
    groupBoxFlagLayout->addWidget(_checkBoxUseVegetation, 2, 1);

    // Utilisation d'ecrans
    _labelUseEcran = new QLabel(_groupBoxFlag);
    _labelUseEcran->setText(TR("id_useecran_label"));
    groupBoxFlagLayout->addWidget(_labelUseEcran, 3, 0);

    _checkBoxUseEcran = new QCheckBox(_groupBoxFlag);
    _checkBoxUseEcran->setText(TR(""));
    groupBoxFlagLayout->addWidget(_checkBoxUseEcran, 3, 1);

    _labelParcoursLateraux = new QLabel(_groupBoxFlag);
    _labelParcoursLateraux->setText(TR("id_parcours_lateraux"));
    groupBoxFlagLayout->addWidget(_labelParcoursLateraux, 3, 2);

    _checkBoxParcoursLateraux = new QCheckBox(_groupBoxFlag);
    _checkBoxParcoursLateraux->setText(TR(""));
    groupBoxFlagLayout->addWidget(_checkBoxParcoursLateraux, 3, 3);

    // Calcul avec reflexion
    _labelUseReflexion = new QLabel(_groupBoxFlag);
    _labelUseReflexion->setText(TR("id_usereflexion_label"));
    groupBoxFlagLayout->addWidget(_labelUseReflexion, 4, 0);

    _checkBoxUseReflexion = new QCheckBox(_groupBoxFlag);
    _checkBoxUseReflexion->setText(TR(""));
    groupBoxFlagLayout->addWidget(_checkBoxUseReflexion, 4, 1);

    // Calcul en conditions favorables (1)

    QButtonGroup* buttonGroupCondFavHomo = new QButtonGroup();
    _pRadioButtonCondHomo = new QRadioButton(TR("id_condhomo_label"));
    buttonGroupCondFavHomo->addButton(_pRadioButtonCondHomo, 0);
    _pRadioButtonCondFav = new QRadioButton(TR("id_condfav_label"));
    buttonGroupCondFavHomo->addButton(_pRadioButtonCondFav, 1);

    QGridLayout* groupBoxCondFavHomoLayout = new QGridLayout();
    groupBoxCondFavHomoLayout->addWidget(_pRadioButtonCondHomo, 0, 0);
    groupBoxCondFavHomoLayout->addWidget(_pRadioButtonCondFav, 0, 1);

    QGroupBox* groupBoxCondFavHomo = new QGroupBox();
    groupBoxCondFavHomo->setTitle(TR(""));
    groupBoxCondFavHomo->setLayout(groupBoxCondFavHomoLayout);

    groupBoxFlagLayout->addWidget(groupBoxCondFavHomo, 5, 0, 1, 2);

    // Calcul en conditions favorables (2) : Parametre de distance relative des reflexions supplementaires
    _labelParamH = new QLabel(_groupBoxFlag);
    _labelParamH->setText(TR("id_paramh_label"));
    groupBoxFlagLayout->addWidget(_labelParamH, 5, 2);

    _lineEditParamH = new QLineEdit(_groupBoxFlag);
    _lineEditParamH->setText(num.setNum(getElement()->getParamH(), 'f', 2));
    groupBoxFlagLayout->addWidget(_lineEditParamH, 5, 3);

    QButtonGroup* buttonGroupIE = new QButtonGroup();
    _radioButtonEnergetique = new QRadioButton(TR("id_calcul_energetique"));
    buttonGroupIE->addButton(_radioButtonEnergetique, 0);
    _radioButtonInterference = new QRadioButton(TR("id_calcul_interference"));
    buttonGroupIE->addButton(_radioButtonInterference, 1);

    QGridLayout* groupBoxIELayout = new QGridLayout();
    groupBoxIELayout->addWidget(_radioButtonEnergetique, 0, 0);
    groupBoxIELayout->addWidget(_radioButtonInterference, 0, 1);

    QGroupBox* groupBoxCondIE = new QGroupBox();
    groupBoxCondIE->setTitle(TR(""));
    groupBoxCondIE->setLayout(groupBoxIELayout);

    groupBoxFlagLayout->addWidget(groupBoxCondIE, 6, 0, 1, 2);

    // Distance Source/Recepteur minimale
    QLabel* plabelDistanceSRMin = new QLabel(TR("id_distancesrmin"), _groupBoxFlag);
    groupBoxFlagLayout->addWidget(plabelDistanceSRMin, 7, 0);

    _lineEditDistanceSRMin = new QLineEdit(_groupBoxFlag);
    groupBoxFlagLayout->addWidget(_lineEditDistanceSRMin, 7, 1, 1, 1);

    _tabWidget->insertTab(1, _groupBoxFlag, TR("id_opt_calc"));

    // Onglet meteo
    _meteoWidget = new TYAtmosphereWidget(getElement()->getAtmosphere(), _tabWidget);

    _tabWidget->insertTab(2, _meteoWidget, TR("id_opt_meteo"));

    // Onglet Points de controle
    _tableauPointControle = new QTableWidget();
    _tableauPointControle->setColumnCount(5);
    _tableauPointControle->setHorizontalHeaderItem(0, new QTableWidgetItem(TR("id_nom_pc")));
    _tableauPointControle->setHorizontalHeaderItem(1, new QTableWidgetItem(TR("id_pos_x")));
    _tableauPointControle->setHorizontalHeaderItem(2, new QTableWidgetItem(TR("id_pos_y")));
    _tableauPointControle->setHorizontalHeaderItem(3, new QTableWidgetItem(TR("id_pos_h")));
    _tableauPointControle->setHorizontalHeaderItem(4, new QTableWidgetItem(TR("id_actif")));

    _tabWidget->insertTab(3, _tableauPointControle, TR("id_opt_pc"));

    // DEFINITION DE L'ONGLET MAILLAGES
    _tableauMaillages = new QTableWidget();
    _tableauMaillages->setColumnCount(2);
    _tableauMaillages->setHorizontalHeaderItem(0, new QTableWidgetItem(TR("id_nom_pc")));
    _tableauMaillages->setHorizontalHeaderItem(1, new QTableWidgetItem(TR("id_actif")));

    _tabWidget->insertTab(4, _tableauMaillages, TR("id_opt_maillage"));

    _calculLayout->addWidget(_tabWidget, iln++, 0);

    // DEFINITION DE L'ONGLET INFO (INFORMATIONS GENERALES)

    _groupBox = new QGroupBox(_tabWidget);

    _groupBox->setTitle(TR(""));
    QGridLayout* groupBoxLayout = new QGridLayout();
    _groupBox->setLayout(groupBoxLayout);

    _lineEditAuteur = new QLineEdit(_groupBox);
    groupBoxLayout->addWidget(_lineEditAuteur, 1, 1);

    _labelAuteur = new QLabel(_groupBox);
    _labelAuteur->setText(TR("id_auteur_label"));
    groupBoxLayout->addWidget(_labelAuteur, 1, 0);

    QDate date;

    _editDateCreation = new QDateEdit(_groupBox);
    if (getElement()->getDateCreation() == "2001-10-01")
    {
        getElement()->setDateCreation(date.currentDate().toString(Qt::ISODate));
    }
    _editDateCreation->setDate(date.fromString(getElement()->getDateCreation(), Qt::ISODate));
    groupBoxLayout->addWidget(_editDateCreation, 2, 1);

    _labelDateCreation = new QLabel(_groupBox);
    _labelDateCreation->setText(TR("id_datecreation_label"));
    groupBoxLayout->addWidget(_labelDateCreation, 2, 0);

    _labelDateModif = new QLabel(_groupBox);
    _labelDateModif->setText(TR("id_datemodif_label"));
    groupBoxLayout->addWidget(_labelDateModif, 3, 0);

    _editDateModif = new QDateEdit(_groupBox);
    if (getElement()->getDateModif() == "2001-10-01")
    {
        getElement()->setDateModif(date.currentDate().toString(Qt::ISODate));
    }
    _editDateModif->setDate(date.fromString(getElement()->getDateModif(), Qt::ISODate));
    groupBoxLayout->addWidget(_editDateModif, 3, 1);

    _lineEditComment = new QTextEdit(_groupBox);
    groupBoxLayout->addWidget(_lineEditComment, 5, 0, 1, 2);

    _labelComment = new QLabel(_groupBox);
    _labelComment->setText(TR("id_comment_label"));
    groupBoxLayout->addWidget(_labelComment, 4, 0);

    _calculLayout->addWidget(_groupBox, iln++, 0);

    _tabWidget->insertTab(5, _groupBox, TR("id_info_calc"));


    //  Bouton permettant d'acceder aux etats
    QGroupBox* pGroupBoxTableEtats = new QGroupBox(this);
    pGroupBoxTableEtats->setTitle(TR("id_map_box"));
    QGridLayout* groupBoxTableEtatsLayout = new QGridLayout();
    pGroupBoxTableEtats->setLayout(groupBoxTableEtatsLayout);

    QLineEdit* pLineEditNomTableEtats = new QLineEdit(pGroupBoxTableEtats);
    pLineEditNomTableEtats->setEnabled(FALSE);
    groupBoxTableEtatsLayout->addWidget(pLineEditNomTableEtats, 0, 0);

    QPushButton* pPushButtonTableEtats = new QPushButton(pGroupBoxTableEtats);
    pPushButtonTableEtats->setText(TR("id_proprietes"));
    groupBoxTableEtatsLayout->addWidget(pPushButtonTableEtats, 0, 1);

    _calculLayout->addWidget(pGroupBoxTableEtats, iln++, 0);

    _etatsWidget = new TYEtatsWidget(getElement(), this);

    // Resultats
    _groupBoxResultat = new QGroupBox(this);
    _groupBoxResultat->setTitle(TR("id_resultat"));
    QGridLayout* groupBoxResultatLayout = new QGridLayout();
    _groupBoxResultat->setLayout(groupBoxResultatLayout);

    _labelStoreGlobalMatrix = new QLabel(_groupBoxResultat);
    _labelStoreGlobalMatrix->setText(TR("id_storeglobalmatrix_label"));
    groupBoxResultatLayout->addWidget(_labelStoreGlobalMatrix, 0, 0);

    _checkBoxStoreGlobalMatrix = new QCheckBox(_groupBoxResultat);
    _checkBoxStoreGlobalMatrix->setText(TR(""));
    groupBoxResultatLayout->addWidget(_checkBoxStoreGlobalMatrix, 0, 1);

    _pushButtonResultat = new QPushButton(_groupBoxResultat);
    _pushButtonResultat->setText(TR("id_proprietes"));
    groupBoxResultatLayout->addWidget(_pushButtonResultat, 0, 2);

    _calculLayout->addWidget(_groupBoxResultat, iln++, 0);

    updateContent();

    connect(pPushButtonTableEtats, SIGNAL(clicked()), _etatsWidget, SLOT(show()));
    connect(_pushButtonResultat, SIGNAL(clicked()), this, SLOT(editResultat()));
    connect(_checkBoxUseEcran, SIGNAL(clicked()), this, SLOT(updateUseEcran()));
    connect(_checkBoxUseReflexion, SIGNAL(clicked()), this, SLOT(updateUseReflexion()));
}

TYCalculWidget::~TYCalculWidget()
{
    delete _meteoWidget;
    _meteoWidget = NULL;
    delete _etatsWidget;
    _etatsWidget = NULL;
}

void TYCalculWidget::updateContent()
{
    QString num;
    QDate date;

    _elmW->setEnabled(true);
    _groupBox->setEnabled(true);
    _groupBoxFlag->setEnabled(true);

    _elmW->updateContent();// Affichage du nom du calcul

    _lineEditAuteur->setText(getElement()->getAuteur());
    _lineEditComment->setPlainText(getElement()->getComment());

    // Mise a jour de l'etat du calcul
    if (getElement()->getState() == TYCalcul::Actif)
    {
        _pRadioButtonActif->setChecked(true);
        _pRadioButtonLocked->setChecked(false);
    }
    else
    {
        _pRadioButtonActif->setChecked(false);
        _pRadioButtonLocked->setChecked(true);
    }


    // Choix du type de calcul
    OGenID currentId = getElement()->getSolverId(); // Id du solveur courant

    LPTYPluginManager pPlug = TYPluginManager::get();
    TYPluginManager::TYPluginList& plugList = pPlug->getPluginList();
    TYPluginManager::TYPluginList::iterator iter;

    QString solverName;
    unsigned short i = 0, currentSolverIndex = 0;
    OGenID id;
    for (iter = plugList.begin(), i = 0; iter != plugList.end(); iter++, i++)
    {
        solverName = (*iter)->getPlugin()->getName();//->filename;
        _comboSolver->insertItem(i, solverName);

        if ((*iter)->getPlugin()->getUUID() == currentId) { currentSolverIndex = i; }
    }

    // On affiche le regime courant
    _comboSolver->setCurrentIndex(currentSolverIndex);

    // Choix concernant le sol
    _pRadioButtonSolReflechissant->setChecked(!getElement()->getUseSol());
    _pRadioButtonSolReel->setChecked(getElement()->getUseSol());
    _comboBoxTypeSol->setCurrentIndex(getElement()->getTypeCalculSol());
    _comboBoxTypeSol->setEnabled(false);

    _pRadioButtonCondHomo->setChecked(!getElement()->getCondFav());
    _pRadioButtonCondFav->setChecked(getElement()->getCondFav());

    _lineEditParamH->setText(num.setNum(getElement()->getParamH(), 'f', 2));
    _lineEditParamH->setEnabled(_pRadioButtonCondFav->isChecked());

    _lineEditDistanceSRMin->setText(num.setNum(getElement()->getDistanceSRMin(), 'f', 2));
    _checkBoxUseVegetation->setChecked(getElement()->getUseVegetation());
    _checkBoxUseAtmosphere->setChecked(getElement()->getUseAtmosphere());

    _checkBoxUseEcran->setChecked(getElement()->getUseEcran());
    _checkBoxParcoursLateraux->setChecked(getElement()->getCalculTrajetsHorizontaux());
    _checkBoxParcoursLateraux->setEnabled(_checkBoxUseEcran->isChecked());

    // Le checkBox de reflexion n'est active que si on fait un calcul d'ecran
    _checkBoxUseReflexion->setEnabled(_checkBoxUseEcran->isChecked());
    _checkBoxUseReflexion->setChecked(getElement()->getUseReflexion());

    _radioButtonEnergetique->setChecked(!getElement()->getInterference());
    _radioButtonInterference->setChecked(getElement()->getInterference());

    _checkBoxStoreGlobalMatrix->setChecked(getElement()->getStatusPartialResult());


    _editDateModif->setDate(date.currentDate());
    _editDateCreation->setDate(date.fromString(getElement()->getDateCreation(), Qt::ISODate));


    // Remplissage du tableau des points de controle
    _tableauPointControle->setEnabled(true);

    TYProjet* pProjet = getElement()->getProjet();
    if (pProjet != NULL)
    {
        TYTabLPPointControl& tabPoints = pProjet->getPointsControl();
        unsigned int nbPoints = static_cast<uint32>(tabPoints.size());
        _tableauPointControle->setRowCount(nbPoints);

        QString msg;
        unsigned int row = 0;
        for (row = 0; row < nbPoints; row++)
        {
            _tableauPointControle->setItem(row, 0, new QTableWidgetItem(tabPoints[row]->getName()));

            msg = QString(TR("id_cell_posx")).arg(tabPoints[row]->_x, 7, 'f', 1);
            _tableauPointControle->setItem(row, 1, new QTableWidgetItem(msg));

            msg = QString(TR("id_cell_posy")).arg(tabPoints[row]->_y, 7, 'f', 1);
            _tableauPointControle->setItem(row, 2, new QTableWidgetItem(msg));

            msg = QString(TR("id_cell_posh")).arg(tabPoints[row]->getHauteur(), 7, 'f', 1);
            _tableauPointControle->setItem(row, 3, new QTableWidgetItem(msg));

            QTableWidgetItem* pCheckItemActif = new QTableWidgetItem("");

            if (tabPoints[row]->getEtat(getElement()))
            {
                pCheckItemActif->setCheckState(Qt::Checked);
            }
            else
            {
                pCheckItemActif->setCheckState(Qt::Unchecked);
            }

            _tableauPointControle->setItem(row, 4, pCheckItemActif);

            _tableauPointControle->setRowHeight(row, 30);
        }
    }

    // Remplissage du tableau des maillages
    _tableauMaillages->setEnabled(true);

    int nbPoints =  static_cast<uint32>(getElement()->getMaillages().size());
    _tableauMaillages->setRowCount(nbPoints);

    LPTYMaillage pMaillage = NULL;

    for (int row = 0; row < nbPoints; row++)
    {
        pMaillage = getElement()->getMaillage(row);
        _tableauMaillages->setItem(row, 0, new QTableWidgetItem(pMaillage->getName()));

        QTableWidgetItem* pCheckItemActif = new QTableWidgetItem("");
        if (pMaillage->getState() == TYMaillage::Actif)
        {
            pCheckItemActif->setCheckState(Qt::Checked);
        }
        else
        {
            pCheckItemActif->setCheckState(Qt::Unchecked);
        }
        _tableauMaillages->setItem(row, 1, pCheckItemActif);

        _tableauMaillages->setRowHeight(row, 30);
    }
}

void TYCalculWidget::apply()
{
    _elmW->apply();

    getElement()->setComment(_lineEditComment->toPlainText());
    getElement()->setAuteur(_lineEditAuteur->text());

    // CHOIX DU SOLVEUR
    LPTYPluginManager pPlug = TYPluginManager::get();
    TYPluginManager::TYPluginList& plugList = pPlug->getPluginList();
    TYPluginManager::TYPluginList::iterator iter;

    unsigned short i = 0;
    unsigned short currentIndex = _comboSolver->currentIndex();
    OGenID id;
    for (i = 0, iter = plugList.begin(); i <= currentIndex; i++, iter++)
    {
        id = (*iter)->getPlugin()->getUUID();
    }

    getElement()->setSolverId(id);

    // ==================

    if (_pRadioButtonActif->isChecked())
    {
        getElement()->setState(TYCalcul::Actif);
    }
    else
    {
        getElement()->setState(TYCalcul::Locked);
        if (getElement()->getResultat()) { getElement()->getResultat()->purge(); }
    }

    getElement()->setUseSol(_pRadioButtonSolReel->isChecked());

    getElement()->setTypeCalculSol(_comboBoxTypeSol->currentIndex());

    getElement()->setCondFav(_pRadioButtonCondFav->isChecked());

    getElement()->setParamH(_lineEditParamH->text().toDouble());
    getElement()->setUseVegetation(_checkBoxUseVegetation->isChecked());
    getElement()->setUseAtmosphere(_checkBoxUseAtmosphere->isChecked());

    getElement()->setUseEcran(_checkBoxUseEcran->isChecked());
    getElement()->setCalculTrajetsHorizontaux(_checkBoxParcoursLateraux->isChecked());

    getElement()->setUseReflexion(_checkBoxUseReflexion->isChecked());

    getElement()->setInterference(_radioButtonInterference->isChecked());

    getElement()->setDateModif(_editDateModif->date().currentDate().toString(Qt::ISODate));
    getElement()->setDateCreation(_editDateCreation->date().toString(Qt::ISODate));
    getElement()->setDistanceSRMin(_lineEditDistanceSRMin->text().toDouble());

    // Actualisation des parametres de l'atmosphere
    _meteoWidget->apply();

    // Mise a jour des points de controles
    TYTabLPPointControl& tabPoints = getElement()->getProjet()->getPointsControl();
    int row = 0;
    for (row = 0; row < _tableauPointControle->rowCount(); row++)
    {
        QTableWidgetItem* pCheck = (QTableWidgetItem*) _tableauPointControle->item(row, 4);
        if (pCheck->checkState() == Qt::Checked)
        {
            tabPoints[row]->setEtat(true, getElement());
            getElement()->addPtCtrlToResult(tabPoints[row]);
        }
        else
        {
            tabPoints[row]->setEtat(false, getElement());
            getElement()->remPtCtrlFromResult(tabPoints[row]);
        }
    }

    // Mise a jour des maillages
    for (row = 0; row < _tableauMaillages->rowCount(); row++)
    {
        QTableWidgetItem* pCheck = (QTableWidgetItem*) _tableauMaillages->item(row, 1);
        if (pCheck->checkState() == Qt::Checked)
        {
            getElement()->getMaillage(row)->setState(TYMaillage::Actif);
        }
        else
        {
            getElement()->getMaillage(row)->setState(TYMaillage::Inactif);
        }
    }

    // Mise a jour du statut de sauvegarde des resultats partiels
    bool bEtat1 = _checkBoxStoreGlobalMatrix->isChecked();
    getElement()->setStatusPartialResult(bEtat1) ; //(_checkBoxStoreGlobalMatrix->isChecked());

    emit modified();
}

void TYCalculWidget::editAtmosphere()
{
    int ret = getElement()->getAtmosphere()->edit(this);

    if (ret == QDialog::Accepted)
    {
        _lineEditNomAtmosphere->setText(getElement()->getAtmosphere()->getName());
    }
}

void TYCalculWidget::editMaillage(QTreeWidgetItem* item)
{
    if (item->parent())
    {
        getElement()->getProjet()->getPointControl(item->text(0).toInt())->edit(_maillagesWidget);
    }
    else
    {
        getElement()->getMaillages()[item->text(0).toInt()]->getElement()->edit(_maillagesWidget);
    }
}

void TYCalculWidget::editResultat()
{
    getElement()->getResultat()->edit(this);
}

void TYCalculWidget::updateUseEcran()
{
    _checkBoxParcoursLateraux->setEnabled(_checkBoxUseEcran->isChecked());
    _checkBoxUseReflexion->setEnabled(_checkBoxUseEcran->isChecked());

    // Si le calcul d'ecran est desactive, la checkbox de reflexion est decoche
    if (! _checkBoxUseEcran->isChecked()) { _checkBoxUseReflexion->setChecked(false); }
}

void TYCalculWidget::updateUseReflexion()
{
    _checkBoxParcoursLateraux->setChecked(true);
}

void TYCalculWidget::updateBoxSol()
{
    int nbMethod = TR("id_nb_sol_calc").toInt();
    QString str, label;
    for (int i = 0; i < nbMethod; i++)
    {
        label = "id_sol_calc";
        label += QString().setNum(i);
        str = TR(label);
        _comboBoxTypeSol->insertItem(i, str);
    }
}


void TYCalculWidget::contextMenuEvent(QContextMenuEvent* e)
{
    QWidget* currentTab = _tabWidget->currentWidget();
    unsigned short checkCol = 4; // Numero de la colonne a checker
    if (currentTab == _tableauPointControle)
    {
        checkCol = 4;
        QPoint point = _tableauPointControle->mapFrom(this, e->pos());
        if ((point.x() >= 0) && (point.y() >= 0) && (point.x() <= _tableauPointControle->width()) && (point.y() <= _tableauPointControle->height()))
        {
            bool bActif = true;
            QMenu* pMenu = new QMenu(this);

            QAction* selectAll = pMenu->addAction(TR("id_popup_select_all"));
            QAction* unSelectAll = pMenu->addAction(TR("id_popup_unselect_all"));

            QAction* ret = pMenu->exec(_tableauPointControle->mapToGlobal(point));
            if (ret != NULL)
            {
                if (ret == selectAll)
                {
                    bActif = true;
                }
                else if (ret == unSelectAll)
                {
                    bActif = false;
                }
                // Update de la table active
                for (int row = 0; row < _tableauPointControle->rowCount(); row++)
                {
                    QTableWidgetItem* pCheck = (QTableWidgetItem*) _tableauPointControle->item(row, checkCol);
                    if (bActif)
                    {
                        pCheck->setCheckState(Qt::Checked);
                    }
                    else
                    {
                        pCheck->setCheckState(Qt::Unchecked);
                    }
                }
            }
        }
    }
    else if (currentTab == _tableauMaillages)
    {
        checkCol = 1;
        QPoint point = _tableauMaillages->mapFrom(this, e->pos());
        if ((point.x() >= 0) && (point.y() >= 0) && (point.x() <= _tableauMaillages->width()) && (point.y() <= _tableauMaillages->height()))
        {
            bool bActif = true;
            QMenu* pMenu = new QMenu(this);

            QAction* selectAll = pMenu->addAction(TR("id_popup_select_all"));
            QAction* unSelectAll = pMenu->addAction(TR("id_popup_unselect_all"));

            QAction* ret = pMenu->exec(_tableauMaillages->mapToGlobal(point));
            if (ret != NULL)
            {
                if (ret == selectAll)
                {
                    bActif = true;
                }
                else if (ret == unSelectAll)
                {
                    bActif = false;
                }
                // Update de la table active
                for (int row = 0; row < _tableauMaillages->rowCount(); row++)
                {
                    QTableWidgetItem* pCheck = (QTableWidgetItem*) _tableauMaillages->item(row, checkCol);
                    if (bActif)
                    {
                        pCheck->setCheckState(Qt::Checked);
                    }
                    else
                    {
                        pCheck->setCheckState(Qt::Unchecked);
                    }
                }
            }
        }
    }
}
