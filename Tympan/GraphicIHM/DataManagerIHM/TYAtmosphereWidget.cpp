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
 * \file TYAtmosphereWidget.cpp
 * \brief outil IHM pour l'objet atmosphere
 *
 *
 */



#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHIHM.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER
#include <qmessagebox.h>
#include "Tympan/MetierSolver/DataManagerMetier/ComposantAcoustique/TYSpectre.h"
#include "Tympan/MetierSolver/DataManagerMetier/Commun/TYCalcul.h"
#include "Tympan/Tools/OLocalizator.h"
#include "Tympan/MetierSolver/DataManagerMetier/EltMateriaux/TYAtmosphere.h"
#include <qcombobox.h>
//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>


#define TR(id) OLocalizator::getString("TYAtmosphereWidget", (id))


TYAtmosphereWidget::TYAtmosphereWidget(TYAtmosphere* pElement, QWidget* _pParent /*=NULL*/):
    TYWidget(pElement, _pParent)
{
    QString num;


    _elmW = new TYElementWidget(pElement, this);

    resize(300, 300);
    setWindowTitle(TR("id_caption"));
    _atmosphereLayout = new QGridLayout();
    setLayout(_atmosphereLayout);
    _atmosphereLayout->addWidget(_elmW, 0, 0);

    _groupBox = new QGroupBox(this);
    _groupBox->setTitle(TR(""));
    _groupBoxLayout = new QGridLayout();
    _groupBox->setLayout(_groupBoxLayout);

    // Description des conditions atmospheriques (pression, temperature, hygrometrie)
    _labelPression = new QLabel(_groupBox);
    _labelPression->setText(TR("id_pression_label"));
    _groupBoxLayout->addWidget(_labelPression, 0, 0);
    _lineEditPression = new QLineEdit(_groupBox);
    _groupBoxLayout->addWidget(_lineEditPression, 0, 1);
    QLabel* pUnitPression = new QLabel(_groupBox);
    pUnitPression->setText(TR("id_unite_pression"));
    _groupBoxLayout->addWidget(pUnitPression, 0, 2);

    _labelTemperature = new QLabel(_groupBox);
    _labelTemperature->setText(TR("id_temperature_label"));
    _groupBoxLayout->addWidget(_labelTemperature, 1, 0);

    _lineEditTemperature = new QLineEdit(_groupBox);
    _groupBoxLayout->addWidget(_lineEditTemperature, 1, 1);
    QLabel* pUnitTemp = new QLabel(_groupBox);
    pUnitTemp->setText(TR("id_unite_temp"));
    _groupBoxLayout->addWidget(pUnitTemp, 1, 2);

    connect(_lineEditTemperature, SIGNAL(lostFocus()), this, SLOT(changeCelerite()));

    _labelHygrometrie = new QLabel(_groupBox);
    _labelHygrometrie->setText(TR("id_hygrometrie_label"));
    _groupBoxLayout->addWidget(_labelHygrometrie, 2, 0);

    _lineEditHygrometrie = new QLineEdit(_groupBox);
    _lineEditHygrometrie->setText(num.setNum(getElement()->getHygrometrie(), 'f', 2));
    _groupBoxLayout->addWidget(_lineEditHygrometrie, 2, 1);
    QLabel* pUnitHygro = new QLabel(_groupBox);
    pUnitHygro->setText(TR("id_unite_hygro"));
    _groupBoxLayout->addWidget(pUnitHygro, 2, 2);

    // Affichage de la vitesse du son
    _labelVitSon = new QLabel(_groupBox);
    _labelVitSon->setText(TR("id_celerite_label"));
    _groupBoxLayout->addWidget(_labelVitSon, 3, 0);

    _lineEditVitSon = new QLineEdit(_groupBox);
    _groupBoxLayout->addWidget(_lineEditVitSon, 3, 1);
    QLabel* pUnitCelerite = new QLabel(_groupBox);
    pUnitCelerite->setText(TR("id_unite_celerite"));
    _groupBoxLayout->addWidget(pUnitCelerite, 3, 2);

    _atmosphereLayout->addWidget(_groupBox, 1, 0);

    // Ajout du bouton pour visualiser le spectre d'attenuation
    _groupBoxSpectreAtt = new QGroupBox(this);
    _groupBoxSpectreAtt->setTitle(TR("id_spectreatt"));
    _groupBoxSpectreAttLayout = new QGridLayout();
    _groupBoxSpectreAtt->setLayout(_groupBoxSpectreAttLayout);

    _lineEditNomSpectre = new QLineEdit(_groupBoxSpectreAtt);
    _lineEditNomSpectre->setText(getElement()->getSpectreAtt().getName());
    _lineEditNomSpectre->setEnabled(FALSE);
    _groupBoxSpectreAttLayout->addWidget(_lineEditNomSpectre, 0, 0);

    _pushButtonSpectre = new QPushButton(_groupBoxSpectreAtt);
    _pushButtonSpectre->setText(TR("id_proprietes_button"));
    _groupBoxSpectreAttLayout->addWidget(_pushButtonSpectre, 0, 1);

    _atmosphereLayout->addWidget(_groupBoxSpectreAtt, 2, 0);
    connect(_pushButtonSpectre, SIGNAL(clicked()), this, SLOT(editSpectre()));

    // Description de la meteo

    // 1. Outils de saisie sous la forme classe meteo
    _groupBoxMeteo1 = new QGroupBox(this);
    _groupBoxMeteo1->setTitle(TR(""));
    QGridLayout* groupBoxMeteo1Layout = new QGridLayout();
    _groupBoxMeteo1->setLayout(groupBoxMeteo1Layout);

    // Vitesse du vent
    _labelVitVent = new QLabel(_groupBoxMeteo1);
    _labelVitVent->setText(TR("id_vitvent_label"));
    groupBoxMeteo1Layout->addWidget(_labelVitVent, 0, 0);
    _lineEditVitVent = new QLineEdit(_groupBoxMeteo1);
    groupBoxMeteo1Layout->addWidget(_lineEditVitVent, 0, 1);
    QLabel* pUnitVitVent = new QLabel(_groupBoxMeteo1);
    pUnitVitVent->setText(TR("id_unite_vitvent"));
    groupBoxMeteo1Layout->addWidget(pUnitVitVent, 0, 2);

    // direction du vent
    _labelDirVent = new QLabel(_groupBoxMeteo1);
    _labelDirVent->setText(TR("id_dirvent_label"));
    groupBoxMeteo1Layout->addWidget(_labelDirVent, 1, 0);
    _lineEditDirVent = new QLineEdit(_groupBoxMeteo1);
    groupBoxMeteo1Layout->addWidget(_lineEditDirVent, 1, 1);
    QLabel* pUnitDirVent = new QLabel(_groupBoxMeteo1);
    pUnitDirVent->setText(TR("id_unite_dirvent"));
    groupBoxMeteo1Layout->addWidget(pUnitDirVent, 1, 2);

    // Classe meteo
    QLabel* labelClasseMeteo = new QLabel(_groupBoxMeteo1);
    labelClasseMeteo->setText(TR("id_classe_meteo_label"));
    groupBoxMeteo1Layout->addWidget(labelClasseMeteo, 2, 0);
    _comboBoxClasseMeteo = new QComboBox(_groupBoxMeteo1);
    groupBoxMeteo1Layout->addWidget(_comboBoxClasseMeteo, 2, 1);
    updateBoxClasseMeteo();

    _atmosphereLayout->addWidget(_groupBoxMeteo1, 3, 0);
    _groupBoxMeteo1->hide();

    // 2. Outil de saisie sous la forme de parametres de turbulence
    _groupBoxMeteo2 = new QGroupBox(this);
    _groupBoxMeteo2->setTitle(TR(""));
    QGridLayout* groupBoxMeteo2Layout = new QGridLayout();
    _groupBoxMeteo2->setLayout(groupBoxMeteo2Layout);

    _labelA = new QLabel(_groupBoxMeteo2);
    _labelA->setText(TR("id_meteo_A_label"));
    groupBoxMeteo2Layout->addWidget(_labelA, 0, 0);
    _lineEditA = new QLineEdit(_groupBoxMeteo2);
    groupBoxMeteo2Layout->addWidget(_lineEditA, 0, 1);

    _labelB = new QLabel(_groupBoxMeteo2);
    _labelB->setText(TR("id_meteo_B_label"));
    groupBoxMeteo2Layout->addWidget(_labelB, 1, 0);
    _lineEditB = new QLineEdit(_groupBoxMeteo2);
    groupBoxMeteo2Layout->addWidget(_lineEditB, 1, 1);

    _labelC = new QLabel(_groupBoxMeteo2);
    _labelC->setText(TR("id_meteo_C_label"));
    groupBoxMeteo2Layout->addWidget(_labelC, 2, 0);
    _lineEditC = new QLineEdit(_groupBoxMeteo2);
    groupBoxMeteo2Layout->addWidget(_lineEditC, 2, 1);

    _labelD = new QLabel(_groupBoxMeteo2);
    _labelD->setText(TR("id_meteo_D_label"));
    groupBoxMeteo2Layout->addWidget(_labelD, 3, 0);
    _lineEditD = new QLineEdit(_groupBoxMeteo2);
    groupBoxMeteo2Layout->addWidget(_lineEditD, 3, 1);

    _atmosphereLayout->addWidget(_groupBoxMeteo2, 3, 0);
    _groupBoxMeteo2->hide();

    updateContent();
}

TYAtmosphereWidget::~TYAtmosphereWidget()
{
}

void TYAtmosphereWidget::updateContent()
{
    QString num;

    // DTn : Desactive 20110905
    //if (getElement()->getParent()) // Desactivation des elements si le calcul est bloque
    //{
    //  TYCalcul* pCalc = (TYCalcul*) getElement()->getParent(); // L'atmosphere ne peut avoir d'autre parent qu'un calcul

    //  if (pCalc->getState()==TYCalcul::Locked)
    //  {
    //      _elmW->setEnabled(false);
    //      _groupBox->setEnabled(false);
    //  }
    //}

    _elmW->updateContent();

    _lineEditPression->setText(num.setNum(getElement()->getPression(), 'f', 2));
    _lineEditVitSon->setText(num.setNum(getElement()->getVitSon(), 'f', 2));
    _lineEditVitSon->setEnabled(false);

    _lineEditVitVent->setText(num.setNum(getElement()->getVitVent(), 'f', 2));
    _lineEditDirVent->setText(num.setNum(getElement()->getDirVent(), 'f', 2));
    _lineEditTemperature->setText(num.setNum(getElement()->getTemperature(), 'f', 2));
    _lineEditHygrometrie->setText(num.setNum(getElement()->getHygrometrie(), 'f', 2));
    _lineEditNomSpectre->setText(getElement()->getSpectreAtt().getName());


#ifdef TY_RECHERCHE
    double A, B, C, D;

    getElement()->getProfilMeteo(A, B, C, D);

    _lineEditA->setText(num.setNum(A, 'f', 2));
    _lineEditB->setText(num.setNum(B, 'f', 2));
    _lineEditC->setText(num.setNum(C, 'G', 2));
    _lineEditD->setText(num.setNum(D, 'f', 2));

    _comboBoxClasseMeteo->setCurrentIndex(getElement()->getClasseMeteo());

    // Montre soit l'interface "saisie turbulence" soit "Classe meteo" selon choix utilisateur
    int typeSaisieMeteo = 1;
#if TY_USE_IHM
    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "TypeSaisieMeteo"))
    {
        typeSaisieMeteo = TYPreferenceManager::getInt(TYDIRPREFERENCEMANAGER, "TypeSaisieMeteo");
    }
    else
    {
        TYPreferenceManager::setInt(TYDIRPREFERENCEMANAGER, "TypeSaisieMeteo", typeSaisieMeteo);
    }

#endif // TY_USE_IHM

    if (typeSaisieMeteo == 0)
    {
        _groupBoxMeteo2->show();
    }
    else
    {
        _groupBoxMeteo1->show();
    }
#endif
}

void TYAtmosphereWidget::apply()
{
    _elmW->apply();

    getElement()->setPression(_lineEditPression->text().toDouble());
    getElement()->setTemperature(_lineEditTemperature->text().toDouble());
    getElement()->setHygrometrie(_lineEditHygrometrie->text().toDouble());
    getElement()->setDirVent(_lineEditDirVent->text().toDouble());
    getElement()->setVitVent(_lineEditVitVent->text().toDouble());

    double A, B, C, D;

    A = _lineEditA->text().toDouble();
    B = _lineEditB->text().toDouble();
    C = _lineEditC->text().toDouble();
    D = _lineEditD->text().toDouble();

    getElement()->setProfilMeteo(A, B, C, D);
    getElement()->setClasseMeteo(_comboBoxClasseMeteo->currentIndex());


    emit modified();
}

void TYAtmosphereWidget::changeCelerite()
{
    getElement()->setPression(_lineEditPression->text().toDouble());
    getElement()->setTemperature(_lineEditTemperature->text().toDouble());
    getElement()->setHygrometrie(_lineEditHygrometrie->text().toDouble());

    _lineEditVitSon->setText(QString().setNum(getElement()->getVitSon(), 'f', 2));
}

void TYAtmosphereWidget::editSpectre()
{
    // On augment la precision d'affichage car les valeurs sont faibles
    int oldPrecision = TYSpectre::getXMLPrecision();
    TYSpectre::setXMLPrecision(7);

    int ret = getElement()->getSpectreAtt().edit(this);

    // Affiche le nom du spectre si modifie
    if (ret == QDialog::Accepted)
    {
        _lineEditNomSpectre->setText(getElement()->getSpectreAtt().getName());
    }

    // On remet la precision d'affichage a la valeur anterieure
    TYSpectre::setXMLPrecision(oldPrecision);
}

void TYAtmosphereWidget::updateBoxClasseMeteo()
{
    int nbMethod = TR("id_nb_classe_meteo").toInt();
    QString str, label;
    for (int i = 0; i < nbMethod; i++)
    {
        label = "id_classe_meteo";
        label += QString().setNum(i);
        str = TR(label);
        _comboBoxClasseMeteo->insertItem(i, str);
    }
}
