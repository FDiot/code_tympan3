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
 * \file TYResultatWidget.cpp
 * \brief outil IHM pour un resultat
 */


#include <cstdlib>

#include <qprinter.h>
#include <qradiobutton.h>
#include <qbuttongroup.h>
#include <qtablewidget.h>
#include <qcombobox.h>
#include <qmenu.h>
#include <qfiledialog.h>
#include <qpainter.h>
#include <qprinter.h>
//Added by qt3to4:
#include <QHBoxLayout>
#include <QBoxLayout>
#include <QGridLayout>
#include <QFrame>
#include <QPixmap>
#include <QHeaderView>
#include <QPrintDialog>

#include "Tympan/core/logging.h"
#include "Tympan/models/business/OLocalizator.h"
#include "Tympan/models/business/TYResultat.h"
#include "Tympan/models/business/acoustic/TYUserSourcePonctuelle.h"
#include "Tympan/models/business/geoacoustic/TYAcousticVolumeNode.h"
#include "Tympan/models/business/geoacoustic/TYAcousticLine.h"
#include "Tympan/models/business/TYProjet.h"
#include "Tympan/models/business/TYCalcul.h"
#include "Tympan/gui/widgets/TYPrintDialog.h"
#include "TYResultatTreeDialog.h"
#include "TYResultatWidget.h"

#define TR(id) OLocalizator::getString("TYResultatWidget", (id))

// Declaration de la fonction utilisee par qsort pour le tri des valeurs numerique
static int compareRes(const void* elem1, const void* elem2);

// Declaration de la fonction utilisee par qsort pour le tri par nom des sources
static int compareName(const void* elem1, const void* elem2);

TYResultatWidget::TYResultatWidget(TYResultat* pElement, QWidget* _pParent /*=NULL*/):
    TYWidget(pElement, _pParent)
{
    _freq = 100;
    _choixOperation = 0;
    _max = NULL; // Tableau des maxima par colonne (initialise plus tard)
    _nbSources = 0;
    _nbRecepteurs = 0;
    _nbLignes = 0;
    _nbColonnes = 0;

    resize(300, 50);
    setWindowTitle(TR("id_caption"));
    _resultatLayout = new QGridLayout();
    setLayout(_resultatLayout);

    // Placement de l'element (evite de renumeroter les lignes chaque fois qu'on deplace un widget)
    int iln = 0 ;

    QGroupBox* groupBoxFlag = new QGroupBox(this);
    groupBoxFlag->setTitle(TR(""));
    QGridLayout* groupBoxFlagLayout = new QGridLayout();
    groupBoxFlag->setLayout(groupBoxFlagLayout);

    // Bouton radio dB(A)/dB(Lin)/dB(freq)
    QButtonGroup* buttonGroupALIN = new QButtonGroup();
    _radioButtonDBA = new QRadioButton(TR("id_radiobutton_db_a"));
    buttonGroupALIN->addButton(_radioButtonDBA, 0);
    _radioButtonDBLIN = new QRadioButton(TR("id_radiobutton_db_lin"));
    buttonGroupALIN->addButton(_radioButtonDBLIN, 1);
    _radioButtonDBFRQ = new QRadioButton(TR("id_radiobutton_db_freq"));
    buttonGroupALIN->addButton(_radioButtonDBFRQ, 2);

    _choixAffichage = DBA; // Affichage par defaut dBA
    _radioButtonDBA->setChecked(true);
    _radioButtonDBLIN->setChecked(false);
    _radioButtonDBFRQ->setChecked(false);

    //  ComboBox de choix des frequences
    _comboBoxFreq = new QComboBox();
    updateFreqList();
    _comboBoxFreq->setEnabled(false);

    // Gestion des seuils de contribution (affichage colore)
    QLabel* pLabelContribution = new QLabel(this);
    pLabelContribution->setText(TR("id_label_contribution"));

    _pContributionLineEdit = new QLineEdit(this);
    _pContributionLineEdit->setFixedSize(50, 20);
    _pContributionLineEdit->setText("20");

    // Ajout au layout
    QGridLayout* groupBoALINLayout = new QGridLayout();
    groupBoALINLayout->addWidget(pLabelContribution, 0, 0, Qt::AlignRight);
    groupBoALINLayout->addWidget(_pContributionLineEdit, 0, 1, Qt::AlignLeft);

    groupBoALINLayout->addWidget(_radioButtonDBA, 1, 0);
    groupBoALINLayout->addWidget(_radioButtonDBLIN, 1, 1);
    groupBoALINLayout->addWidget(_radioButtonDBFRQ, 1, 2);

    groupBoALINLayout->addWidget(_comboBoxFreq, 1, 3);

    QGroupBox* groupBoxALIN = new QGroupBox();
    groupBoxALIN->setTitle(TR(""));
    groupBoxALIN->setLayout(groupBoALINLayout);

    groupBoxFlagLayout->addWidget(groupBoxALIN, 0, 0);

    // Choix du type d'operation a effectuer (Aucun / Difference / Emergence / Bruit ambiant)
    _buttonGroupOperation = new QButtonGroup();
    _radioButtonNoOp = new QRadioButton((TR("id_radiobutton_noop")));
    _buttonGroupOperation->addButton(_radioButtonNoOp, 0);
    _radioButtonDelta = new QRadioButton(TR("id_radiobutton_delta"));
    _buttonGroupOperation->addButton(_radioButtonDelta, 1);
    _radioButtonEmergence = new QRadioButton(TR("id_radiobutton_emergence"));
    _buttonGroupOperation->addButton(_radioButtonEmergence, 2);
    _radioButtonAmbiant = new QRadioButton(TR("id_radiobutton_ambiant"));
    _buttonGroupOperation->addButton(_radioButtonAmbiant, 3);

    _radioButtonNoOp->setChecked(true);
    _radioButtonDelta->setChecked(false);
    _radioButtonEmergence->setChecked(false);
    _radioButtonAmbiant->setChecked(false);

    // Choix du calcul associe au resultat
    QLabel* labelSubstCalcul = new QLabel(this);
    labelSubstCalcul->setText(TR("id_subst_calcul"));
    _comboBoxSubstCalcul = new QComboBox(this);
    connect(_comboBoxSubstCalcul, SIGNAL(activated(int)), this, SLOT(changeSubstCalcul(int)));

    QGridLayout* groupBoxCalculLayout = new QGridLayout();
    groupBoxCalculLayout->addWidget(_radioButtonNoOp, 0, 0, Qt::AlignLeft);
    groupBoxCalculLayout->addWidget(_radioButtonDelta, 1, 0, Qt::AlignLeft);
    groupBoxCalculLayout->addWidget(_radioButtonEmergence, 2, 0, Qt::AlignLeft);
    groupBoxCalculLayout->addWidget(_radioButtonAmbiant, 3, 0, Qt::AlignLeft);
    groupBoxCalculLayout->addWidget(labelSubstCalcul, 0, 1, Qt::AlignRight);
    groupBoxCalculLayout->addWidget(_comboBoxSubstCalcul, 0, 2, Qt::AlignLeft);

    QGroupBox* groupBoxCalcul = new QGroupBox();
    groupBoxCalcul->setTitle(TR(""));
    groupBoxCalcul->setLayout(groupBoxCalculLayout);

    groupBoxFlagLayout->addWidget(groupBoxCalcul, 0, 1);

    _resultatLayout->addWidget(groupBoxFlag, iln, 0);

    // Table des resultats
    _pTable = new QTableWidget();
    _pTable->setSelectionMode(QAbstractItemView::NoSelection);
    _pTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _resultatLayout->addWidget(_pTable, ++iln, 0);

    QGroupBox* pGroupBox = new QGroupBox(this);
    QHBoxLayout* pGroupBoxLayout = new QHBoxLayout();
    pGroupBox->setLayout(pGroupBoxLayout);

    _pButtonExport = new QPushButton(TR("id_export_global"));
    pGroupBoxLayout->addWidget(_pButtonExport);
    _pButtonExport->setEnabled(true);

    _pButtonExportSpectre = new QPushButton(TR("id_export_spectres"));
    pGroupBoxLayout->addWidget(_pButtonExportSpectre);
    _pButtonExportSpectre->setEnabled(true);

    QPushButton* pButtonPrint = new QPushButton(TR("id_print"));
    pGroupBoxLayout->addWidget(pButtonPrint);

    _resultatLayout->addWidget(pGroupBox, ++iln, 0);

    // Permettre le tri en cliquant sur le haut d'une colonne
    QHeaderView* entetesRecepteurs = _pTable->horizontalHeader(); // On recupere un pointeur sur les entetes horizontales
    QObject::connect(entetesRecepteurs, SIGNAL(sectionClicked(int)), this, SLOT(sortCol(int)));

    QObject::connect(_pContributionLineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(slotContributionLineEditChanged()));
    connect(buttonGroupALIN, SIGNAL(buttonClicked(int)), this, SLOT(changeAffichage(int)));
    connect(_comboBoxFreq, SIGNAL(activated(int)), this, SLOT(setFrequency(int)));
    connect(_buttonGroupOperation, SIGNAL(buttonClicked(int)), this, SLOT(changeOperation(int)));
    QObject::connect(_pButtonExport, SIGNAL(clicked()), this, SLOT(exportExcel()));
    QObject::connect(_pButtonExportSpectre, SIGNAL(clicked()), this, SLOT(exportSpectre()));
    QObject::connect(pButtonPrint, SIGNAL(clicked()), this, SLOT(print()));

    // Tableau pour le tri des resultats
    _tabSortedRes = new TYStructIndiceResultat[getElement()->getNbOfSources()];

    _seuilContribution = 20;

    updateContent();

    _printer = new QPrinter();
}

TYResultatWidget::~TYResultatWidget()
{
    if (_max)
    {
        delete [] _max;
    }

    if (_tabSortedRes)
    {
        delete [] _tabSortedRes;
    }

    if (_printer)
    {
        delete _printer;
    }

    _max = NULL;
    _tabSortedRes = NULL;
    _printer = NULL;
}

void TYResultatWidget::updateContent()
{
    // ACtualisation du combo des calculs
    updateComboSubstCalculs();

    // Par defaut le combo des frequences est positionne a 100 Hz
    int indFreq = TYSpectre::getIndice(100);
    _comboBoxFreq->setCurrentIndex(indFreq);


    _nbSources = static_cast<int>(getElement()->getNbOfSources());
    _nbRecepteurs = static_cast<int>(getElement()->getNbOfRecepteurs());
    _nbLignes = _nbSources + 1; // Ajouter la ligne "synthese"
    _nbColonnes = _nbRecepteurs + 2; // Ajouter les colonnes "Nom sources", et "Puissance source"

    // Nb lignes = Nb sources + 1 car on ajoute la synthese ligne 1
    _pTable->setRowCount(_nbLignes);

    // Nb colonnes = Nb recepteurs + 2 car : nom des sources en colonne 1 et puissance sources en colonne 2
    _pTable->setColumnCount(_nbColonnes);

    // une boucle pour mettre les entetes horizontales (numeros des lignes)
    for (unsigned int row = 0 ; row < _nbLignes ; row++)
    {
        _pTable->setVerticalHeaderItem(row, new QTableWidgetItem(QString().setNum(row)));
    }

    updateHeaderLabel();

    // L'entete de la colonne 1 indique que cette colonne donne acces a la puissance
    _pTable->setHorizontalHeaderItem(1, new QTableWidgetItem(QString("LW")));

    // une boucle pour mettre les nom des recepteurs
    for (unsigned int col = 0; col < _nbRecepteurs ; col++)
    {
        if (getElement()->getRecepteur(col) == NULL) { continue; }

        QString titre = getElement()->getRecepteur(col)->getName();
        _pTable->setHorizontalHeaderItem(col + 2, new QTableWidgetItem(titre));
    }

    // On trie par ordre decroissant des niveaux du premier point
    sortCol(1) ;

    _pTable->setColumnHidden( 1, getElement()->isLWHidden() );
}

void TYResultatWidget::updateHeaderLabel()
{
    QString strVal; // Cree ici car pb avec switch/case
    // L'entete de la colonne 0 rappelle le choix d'affichage
    switch (_choixAffichage)
    {
        case DBLIN:
            _pTable->setHorizontalHeaderItem(0, new QTableWidgetItem(QString("dBZ")));
            break;
        case DBFREQ:
            strVal = QString("dB(") + QString("%1").arg(_freq) + QString(" Hz)");
            _pTable->setHorizontalHeaderItem(0, new QTableWidgetItem(strVal));
            break;
        case DBA :
        default:
            _pTable->setHorizontalHeaderItem(0, new QTableWidgetItem(QString("dBA")));
            break;
    }
}

void TYResultatWidget::apply()
{
}

void TYResultatWidget::changeSubstCalcul(int calcul)
{
    updateTable();
}
void TYResultatWidget::updateComboSubstCalculs()
{
    _comboBoxSubstCalcul->clear();

    // Remplissage du comboBox des calculs
    if (_pElement->getParent()->getParent()->isA("TYProjet"))
    {
        TYProjet* pProjet = TYProjet::safeDownCast(getElement()->getParent()->getParent());

        TYTabLPCalcul listCalcul = pProjet->getListCalcul();
        for (int i = 0; i < listCalcul.size(); i++)
        {
            _comboBoxSubstCalcul->insertItem(i, listCalcul[i]->getName());
        }
    }

    // On affiche la forme d'objet courante
    _comboBoxSubstCalcul->setCurrentIndex(0);

    // Si mode de calcul = none alors le combo est bloque
    if (_choixOperation == 0)
    {
        _comboBoxSubstCalcul->setEnabled(false);
    }
    else
    {
        _comboBoxSubstCalcul->setEnabled(true);
    }
}


TYCalcul* TYResultatWidget::getSelectedSubstCalcul()
{
    if (_pElement->getParent()->getParent()->isA("TYProjet"))
    {
        if (!_radioButtonNoOp->isChecked())
        {
            int calcul = _comboBoxSubstCalcul->currentIndex();
            TYProjet* pProjet = TYProjet::safeDownCast(getElement()->getParent()->getParent());
            return pProjet->getListCalcul()[calcul];
        }
    }
    return NULL;
}

void TYResultatWidget::exportSpectre()
{
    TYResultat* pResultat = TYResultat::safeDownCast(getElement());
    if (!pResultat) { return; } // Securite

    QString qFileName = QFileDialog::getSaveFileName(this, "Choose a file", "", "CSVFile (*.csv)");

    if (!qFileName.isNull())
    {
        if (qFileName.right(3) != "csv")
        {
            qFileName += ".csv";
        }

        pResultat->saveSpectre(qFileName.toStdString(), getSelectedSubstCalcul());
    }
}

void TYResultatWidget::exportExcel()
{
    TYResultat* pResultat = TYResultat::safeDownCast(getElement());

    if (!pResultat) { return; } // Securite

    QString qFileName = QFileDialog::getSaveFileName(this, "Choose a file",
                                                     "", "CSVFile (*.csv)");

    if (!qFileName.isNull())
    {
        if (qFileName.right(3) != "csv")
        {
            qFileName += ".csv";
        }

        pResultat->saveValue(qFileName.toStdString(), _choixAffichage, _freq);
    }
}

void TYResultatWidget::contextMenuEvent(QContextMenuEvent* e)
{
    TYCalcul* pCalcul = TYCalcul::safeDownCast(getElement()->getParent());
    TYCalcul* pSubstCalcul = getSelectedSubstCalcul();

    QPoint point = _pTable->mapFrom(this, e->pos());
    if ((point.x() >= 0) && (point.y() >= 0) && (point.x() <= _pTable->width()) && (point.y() <= _pTable->height()))
    {
        QPoint resPoint = QPoint(point.x(), point.y() - _pTable->horizontalHeader()->height());
        QTableWidgetItem* item = _pTable->itemAt(resPoint);
        if (item)
        {
            int row = _pTable->indexAt(resPoint).row();
            row = row >= 0 ? row : 0; // Securite
            int col = _pTable->indexAt(resPoint).column();
            col = col >= 0 ? col : 0; // Securite
            int ligne = row > 0 ? _tabSortedRes[row - 1].indice : 0;

            OSpectre spectre1 = getSpectre(row, col, pCalcul);
            OSpectre spectre2 = getSpectre(row, col, pSubstCalcul);

            TYSpectre spectre = resuSpectre(spectre1, spectre2, 1);

            spectre.setType(SPECTRE_TYPE_LP);
            spectre = spectre.toDB();

            QAction* editSource = NULL, *editRecepteur = NULL, *editSpectre = NULL, *editContrib = NULL;

            QMenu* pPopup = new QMenu(this);
            if ((row == 0) && (col > 1)) // Ligne de synthese
            {
                if (_choixOperation == 0) // pas d'operation
                {
                    editRecepteur = pPopup->addAction(TR("id_popup_see_recepteur"));
                    pPopup->addSeparator();
                }
                else
                {
                    editSpectre = pPopup->addAction(TR("id_popup_see_spectre"));
                }

                // Si on a conserve la matrice des resultats bruts
                if (getElement()->getPartialState())
                {
                    editContrib = pPopup->addAction(TR("id_popup_see_contrib"));
                }
            }
            else if ((row > 0) && (col > 1)) // Si on selectionne une contribution par source (!= synthese)
            {
                editSpectre = pPopup->addAction(TR("id_popup_see_spectre"));
                pPopup->addSeparator();
                editSource = pPopup->addAction(TR("id_popup_see_source"));
                pPopup->addSeparator();
                editRecepteur = pPopup->addAction(TR("id_popup_see_recepteur"));
            }
            else if ((row > 0) && (col == 0))
            {
                editSource = pPopup->addAction(TR("id_popup_see_source"));
            }
            else if ((row > 0) && (col == 1))
            {
                editSource = pPopup->addAction(TR("id_popup_see_source"));
            }

            QAction* ret = pPopup->exec(_pTable->mapToGlobal(point));

            if (ret)
            {
                if (ret == editSource)
                {
                    getElement()->getSource(ligne)->edit(this);
                }
                else if (ret == editRecepteur)
                {
                    TYCalcul* pCalcul = static_cast<TYCalcul*>(getElement()->getParent());
                    TYPointCalcul* pPoint = getElement()->getRecepteur(col - 2);
                    TYSpectre* pSpectre = NULL;
                    if (pPoint) { pSpectre = pCalcul->getSpectre(pPoint->getID()); }
                    if (!pSpectre) { return; }

                    // Si c'est un resultat calcule, on ne peut pas changer les valeurs
                    if (pCalcul && (pCalcul->getState() == TYCalcul::Actif))
                    {
                        pSpectre->setIsReadOnly(true);
                    }
                    else
                    {
                        pSpectre->setIsReadOnly(false);
                    }

                    pSpectre->edit(this);

                    // Si c'est une mesure, on actualise le tableau resultat
                    if (pCalcul && (pCalcul->getState() == TYCalcul::Locked)) { updateTable(); }
                }
                else if (ret == editSpectre)
                {
                    spectre.setIsReadOnly(true);
                    spectre.edit(this);
                    spectre.setIsReadOnly(false);
                }
                else if (ret == editContrib)
                {
                    showContribTreeDialog(col);
                }
            }

        }
    }
}

void TYResultatWidget::print()
{
    TYPrintDialog* pDialog = new TYPrintDialog(this);
    pDialog->_groupBoxProjet->setEnabled(false);
    pDialog->_groupBoxCalcul->setEnabled(false);
    if (pDialog->exec() == QDialog::Accepted)
    {
        QPrintDialog dialog(_printer, this);
        if (dialog.exec())
        {
            QPainter paint(_printer);

            QPixmap tmpPix;

            int mid = int(_printer->width() / 2) - int(paint.fontMetrics().width(pDialog->_lineEditTete->text()) / 2);
            paint.drawText(mid, 20, pDialog->_lineEditTete->text());
            mid = int(_printer->width() / 2) - int(paint.fontMetrics().width(pDialog->_lineEditPied->text()) / 2);
            paint.drawText(mid, _printer->height() - 20, pDialog->_lineEditPied->text());
            paint.setFont(QFont("Helvetica" , 15, QFont::Bold));
            mid = int(_printer->width() / 2) - int(paint.fontMetrics().width(pDialog->_lineEditTitre->text()) / 2);
            paint.drawText(mid, 45, pDialog->_lineEditTitre->text());
            paint.setFont(QFont());

            int sizeX = 120, sizeY = 25;
            int ymax = int(double(_printer->height()) * 4 / 5 / (_pTable->rowCount() + 1));
            if (ymax < sizeY) { sizeY = ymax; }
            int x = int (double(_printer->width() - sizeX * (_pTable->columnCount())) / 2);
            int y = int ((double(_printer->height()) - sizeY * (_pTable->rowCount() + 3)));

            int i;
            for (i = 0; i < _pTable->horizontalHeader()->count(); i++)
            {
                paint.drawText(x, y, _pTable->horizontalHeaderItem(i)->text());
                paint.translate(sizeX, 0);
            }

            paint.translate(-_pTable->columnCount()*sizeX, 0);
            paint.translate(0, sizeY);

            for (i = 0; i < _pTable->rowCount(); i++)
            {
                for (int j = 0; j < _pTable->columnCount(); j++)
                {
                    if (_pTable->item(i, j))
                    {
                        paint.drawText(x, y, _pTable->item(i, j)->text());    // paintCell(&paint, i, j, QRect(0, 0, sizeX, sizeY), false);
                    }
                    paint.translate(sizeX, 0);
                }
                paint.translate(0, sizeY);
                paint.translate(-_pTable->columnCount()*sizeX, 0);
            }
        }
    }
}

void TYResultatWidget::changeAffichage(int select)
{
    switch (select)
    {
        case 1 :
            _choixAffichage = DBLIN; // Affichage dBLin
            _comboBoxFreq->setEnabled(false);
            break;
        case 2 :
            _choixAffichage = DBFREQ; // Affichage par frequence
            _comboBoxFreq->setEnabled(true);
            break;
        case 0 :
        default:
            _choixAffichage = DBA; // Affichage dBA
            _comboBoxFreq->setEnabled(false);
            break;
    }

    // Mets a jour le widget
    updateHeaderLabel();
    updateTable();
}

void TYResultatWidget::changeOperation(int select)
{
    _choixOperation = select;

    if (_choixOperation == 0)
    {
        _comboBoxSubstCalcul->setEnabled(false);
        _pButtonExport->setEnabled(true);
        _pButtonExportSpectre->setEnabled(true);
    }
    else
    {
        _comboBoxSubstCalcul->setEnabled(true);
        _pButtonExport->setEnabled(false);
        _pButtonExportSpectre->setEnabled(false);
    }

    // Mets a jour le widget
    updateTable();
}

void TYResultatWidget::updateTable()
{
    // On commence par remplir la premiere colonne avec les noms des sources
    QString titre = TR("id_entete_synthese");
    _pTable->setItem(0, 0, new QTableWidgetItem(titre));
    _pTable->setRowHeight(0, 30);
    _pTable->setColumnWidth(0, 140);

    for (unsigned int row = 1; row < _nbLignes ; row++) // On commence apres la ligne synthese
    {
        TYElement* pSource = getElement()->getSource(_tabSortedRes[row - 1].indice);
        if (pSource == NULL) { continue; }

        QString titre = pSource->getName();
        _pTable->setItem(row, 0, new QTableWidgetItem(titre));
        _pTable->setRowHeight(row, 30);
    }

    // Recherche des maxima de contribution pour chaque recepteur
    findMaxima();

    // Boucle sur les lignes et les colonnes pour afficher le tableau
    TYCalcul* pCalcOp = getSelectedSubstCalcul();
    for (unsigned int row = 0; row < _nbLignes; row++)
    {
        for (unsigned int col = 1; col < _nbColonnes; col++)
        {
            affichageCellule(row, col, pCalcOp);
        }
    }
}

OSpectre TYResultatWidget::getSpectre(const int& row, const int& col, TYCalcul* pCalcul)
{
    OSpectre spectre;
    spectre.setValid(false);

    if ((!pCalcul) || (col == 0)) { return spectre; }   // Cas de la colonne des noms de source

    if (col == 1) // Spectre de puissance d'une source
    {
        if (row > 0)
        {
            spectre = getPuissanceElem(_tabSortedRes[row - 1].indice);
        }
        else
        {
            spectre.setValid(false);
        }
    }
    else
    {
        if (row == 0)  // Ligne synthese
        {
            TYPointCalcul* pPoint = getElement()->getRecepteur(col - 2);
            if (pPoint) { spectre = *pCalcul->getSpectre(pPoint->getID()); }
        }
        else // Contribution d'une source en un point
        {
            // Recuperation du resultat courant
            TYResultat* pThisRes = getElement();
            // Recuperation de pointeurs sur la source et du recepteur
            TYElement* pSource = pThisRes->getSource(_tabSortedRes[row - 1].indice);
            TYPointCalcul* pRecepteur = pThisRes->getRecepteur(col - 2);

            // Puis on recupere le spectre correspondant dans le calcul passe en parametre
            TYResultat* pResultat = pCalcul->getResultat();
            spectre = pResultat->getSpectre(pRecepteur, pSource);
            spectre = spectre.toDB();
        }
    }

    return spectre;
}

void TYResultatWidget::initTabSort(int col)
{
    int row = 1; // Numero de 1ere ligne (ligne 0 = synthese)

    TYCalcul* pCalcul = TYCalcul::safeDownCast(getElement()->getParent());
    TYCalcul* pSubstCalcul = getSelectedSubstCalcul();

    OSpectre spectre1;
    OSpectre spectre2;

    bool valid = true;

    for (unsigned int i = 0; i < getElement()->getNbOfSources(); i++)
    {
        _tabSortedRes[i].indice = i;
        row = i + 1;

        if (getElement() && getElement()->getSource(i))
        {
            _tabSortedRes[i].name = getElement()->getSource(i)->getName().toStdString();
        }

        spectre1 = getSpectre(row, col, pCalcul);
        spectre2 = getSpectre(row, col, pSubstCalcul);

        _tabSortedRes[i].resultat = resuAffichage(spectre1, spectre2, valid, 1);
    }
}

void TYResultatWidget::sortCol(int col)
{
    if (col < 0)
    {
        return;
    }
    else if (col == 0)
    {
        sortName();
    }
    else
    {
        sortRes(col);
        // Dans ce cas, on reaffiche car on a reorganise les resultats
    }

    updateTable();
}

void TYResultatWidget::sortRes(int col)
{
    // On commence par remplir le tableau des resultats
    // (la synthese est supposee etre toujours la plus forte valeur et "surnager" en haut du tableau)
    initTabSort(col);

    // Tri
    qsort(_tabSortedRes, getElement()->getNbOfSources(), sizeof(TYStructIndiceResultat), compareRes);
}

void TYResultatWidget::sortName()
{
    // On commence par remplir le tableau des resultats
    // (la synthese est supposee etre toujours la plus forte valeur et "surnager" en haut du tableau)
    initTabSort(1);

    // Tri
    qsort(_tabSortedRes, getElement()->getNbOfSources(), sizeof(TYStructIndiceResultat), compareName);
}

TYSpectre TYResultatWidget::getPuissanceElem(const unsigned int& i)
{
    TYElement* pElement = getElement()->getSource(i);

    return getPuissanceElem(pElement);
}

TYSpectre TYResultatWidget::getPuissanceElem(TYElement* pElement)
{
    assert(pElement);
    TYSpectre spectre;

    std::map<TYElement*, LPTYSpectre>& mapElementSpectre = getElement()->getMapElementSpectre();
    TYSpectre* puissance = mapElementSpectre[pElement];

    if (puissance) { spectre = *puissance; }

    return spectre;
}

void TYResultatWidget::showContribTreeDialog(int col)
{
    TYResultatTreeDialog* pDlg = new TYResultatTreeDialog(this);

    // Recuperation du point de contrOle
    LPTYPointCalcul pPoint = getElement()->getRecepteur(col - 2);
    pDlg->set(getElement(), pPoint);
    pDlg->exec();
}

bool TYResultatWidget::isPresenceTonaliteMarquee(const OSpectre& spectre)
{
    return spectre.isTonalite();
}

void TYResultatWidget::slotContributionLineEditChanged()
{
    _seuilContribution = _pContributionLineEdit->text().toDouble();
    updateTable();
}

void TYResultatWidget::affichageCellule(const int& row, const int& col, TYCalcul* pCalcOp)
{
    TYCalcul* pCalcul = TYCalcul::safeDownCast(getElement()->getParent());

    OSpectre spectre1 = getSpectre(row, col, pCalcul);
    OSpectre spectre2 = getSpectre(row, col, pCalcOp);

    QTableWidgetItem* pItem = NULL;
    QString msg;
    double valeur = 0.0;
    bool valid = true;

    // Cas 1 : ligne de synthese
    if (row == 0)
    {
        if (col > 1) // La case (0,1) est vide
        {
            valeur = resuAffichage(spectre1, spectre2, valid, CELL_SYNTH);
            msg = QString("%1").arg(valeur, 7, 'f', 1);
            pItem = new QTableWidgetItem(msg);
            decorsSynthese(pItem, row, col, valid, spectre1, spectre2);
            _pTable->setItem(row, col, pItem);
            _pTable->setRowHeight(row, 30);
        }
    }
    else if (col > 1)   // Tout sauf ligne de synthese ou puissance de source
    {
        valeur = resuAffichage(spectre1, spectre2, valid, CELL_CONTRIB);
        msg = QString("%1").arg(valeur, 7, 'f', 1);
        pItem = new QTableWidgetItem(msg);
        decorsContributions(pItem, row, col, valid, spectre1, spectre2);
        _pTable->setItem(row, col, pItem);
        _pTable->setRowHeight(row, 30);
    }
    else // A priori, il ne reste que la puissance de la source
    {
        spectre2 = OSpectre(); // La puissance affichee correspond a la puissance de la source du calcul courant
        valeur = resuAffichage(spectre1, spectre2, valid, CELL_LW);
        msg = QString("%1").arg(valeur, 7, 'f', 1);
        pItem = new QTableWidgetItem(msg);
        decorsContributions(pItem, row, col, valid, spectre1, spectre2);
        _pTable->setItem(row, col, pItem);
        _pTable->setRowHeight(row, 30);
    }
}

double TYResultatWidget::resuAffichage(OSpectre& spectre1, OSpectre& spectre2, bool& valid, const int& typeCase)
{
    double val1 = 0.0, val2 = 0.0;

    switch (_choixAffichage)
    {
        case DBLIN:
            val1 = spectre1.valGlobDBLin();
            val2 = spectre2.valGlobDBLin();
            break;
        case DBFREQ:
            val1 = spectre1.getValueReal(_freq);
            val2 = spectre2.getValueReal(_freq);
            break;
        case DBA :
        default:
            val1 = spectre1.valGlobDBA();
            val2 = spectre2.valGlobDBA();
            break;
    }

    if (_choixOperation != 0)
    {
        if ((!spectre2.isValid()) || (typeCase == CELL_LW)) { val2 = 0.0; }
        if ((!spectre2.isValid()) && ((typeCase == CELL_SYNTH) || (typeCase == CELL_CONTRIB))) { valid = false; }
    }

    switch (_choixOperation)
    {
        case 0 : // Pas d'operation
            return val1;
        case 1 : // difference entre deux resultats
            return val1 - val2;
        case 2 : // emergence (uniquement pour la ligne synthese)
            if (typeCase == CELL_SYNTH)
            {
                return (TYResultat::safeDownCast(_pElement))->getEmergence(val1, val2);
            }
            else
            {
                return val1;
            }
        case 3 : // bruit ambiant (uniquement pour la ligne synthese)
        default :
            if (typeCase == CELL_SYNTH)
            {
                return (TYResultat::safeDownCast(_pElement))->getAmbiant(val1, val2);
            }
            else
            {
                return val1;
            }
    }
}

OSpectre TYResultatWidget::resuSpectre(OSpectre& spectre1, OSpectre& spectre2, const int& typeCase)
{
    switch (_choixOperation)
    {
        case 0 : // Pas d'operation
            return spectre1;
            break;
        case 1 : // difference entre deux resultats
            return spectre1.subst(spectre2);
            break;
        case 2 : // emergence
            return (TYResultat::safeDownCast(_pElement))->getEmergence(spectre1, spectre2);
            break;
        case 3 : // bruit ambiant
        default :
            return (TYResultat::safeDownCast(_pElement))->getAmbiant(spectre1, spectre2);
            break;
    }

    return OSpectre();
}

void TYResultatWidget::decorsSynthese(QTableWidgetItem* pItem, const int& row, const int& col, const bool& valid, OSpectre& spectre1, OSpectre& spectre2)
{
    if (!valid)
    {
        pItem->setBackgroundColor(QColor(127, 127, 127)); // Gris uni
        return;
    }

    if ((_choixAffichage == DBFREQ) ||
        (_choixOperation == 1)      ||
        (_choixOperation == 2))
    {
        pItem->setBackgroundColor(QColor(255, 255, 255)); // Blanc uni ?
        return; // Rien si affichage sur une frequence ou difference ou emergence
    }

    OSpectre spectre;
    TYCalcul* pCalcul = TYCalcul::safeDownCast(_pElement->getParent());

    if (_choixOperation == 3)  // Bruit ambiant
    {
        spectre = getElement()->getAmbiant(spectre1, spectre2);
    }
    else if (_choixOperation == 0) // Pas d'operation
    {
        spectre = getSpectre(row, col, pCalcul);
    }

    if (isPresenceTonaliteMarquee(spectre)) // Coloration en jaune
    {
        pItem->setBackgroundColor(QColor(255, 255, 0));
    }
}

void TYResultatWidget::decorsContributions(QTableWidgetItem* pItem, const int& row, const int& col, const bool& valid, OSpectre& spectre1, OSpectre& spectre2)
{
    // Les cellules sont decorees sauf dans le cas ou on affiche l'ecart entre deux resultats
    if (!valid)
    {
        pItem->setBackgroundColor(QColor(127, 127, 127)); // Gris uni
        return;
    }

    if ((_choixOperation == 1) || (col == 1))
    {
        pItem->setBackgroundColor(QColor(255, 255, 255)); // Blanc uni ?
    }
    else
    {
        double valeur = pItem->text().toDouble();
        double global = _pTable->item(0, col)->text().toDouble();
        // Affichage orange pour les cellules qui contribuent a plus que valeur globale - seuil
        // Uniquement dansle cas ou aucune operation n'a ete faite sur le resultat.
        if ((_choixOperation == 0) && (valeur >= (global - _seuilContribution)))
        {
            pItem->setBackgroundColor(QColor(255, 204, 0));
        }

        // Affichage rouge de la cellule correspondant a la contribution la plus forte
        int imaxi = static_cast<int>(_max[col - 2] * 10);
        int ivaleur = static_cast<int>(valeur * 10);
        if (ivaleur >= (imaxi - 1))
        {
            pItem->setBackgroundColor(QColor(255, 0, 0));
        }
    }
}

void TYResultatWidget::updateFreqList()
{
    TYTabFreq tabFreq = TYSpectre::getTabFreqNorm();
    for (unsigned int i = 0 ; i < tabFreq.size() ; i++)
    {
        QString str;
        str.setNum(tabFreq[i], 'f', 1);
        _comboBoxFreq->insertItem(i, str);
    }
}

void TYResultatWidget::setFrequency(int freq)
{
    _freq = _comboBoxFreq->currentText().toFloat();
    updateHeaderLabel();
    updateTable();
}

void TYResultatWidget::findMaxima()
{
    // Initialisation du tableau
    if (_max)
    {
        delete [] _max;
        _max = NULL;
    }

    _max = new double[_nbRecepteurs];

    TYCalcul* pCalcul = TYCalcul::safeDownCast((TYResultat::safeDownCast(_pElement))->getParent());
    TYCalcul* pSubstCalcul = getSelectedSubstCalcul();

    double tmpMax = -10000, valeur = 0.0;
    bool valid = true;

    for (unsigned int col = 2; col < _nbColonnes; col++)
    {
        tmpMax = -10000;

        for (unsigned int row = 1; row < _nbLignes; row++)
        {
            OSpectre spectre1 = getSpectre(row, col, pCalcul);
            OSpectre spectre2 = getSpectre(row, col, pSubstCalcul);
            valeur = resuAffichage(spectre1, spectre2, valid, 1);
            tmpMax = valeur > tmpMax ? valeur : tmpMax ;
        }

        _max[col - 2] = tmpMax;
    }
}

int compareRes(const void* elem1, const void* elem2)
{
    TYStructIndiceResultat* Res1 = (TYStructIndiceResultat*) elem1;
    TYStructIndiceResultat* Res2 = (TYStructIndiceResultat*) elem2;

    double res = Res2->resultat - Res1->resultat;
    int sgn = int(res / fabs(res));
    return (sgn);
}

int compareName(const void* elem1, const void* elem2)
{
    TYStructIndiceResultat* Res1 = (TYStructIndiceResultat*) elem1;
    TYStructIndiceResultat* Res2 = (TYStructIndiceResultat*) elem2;

    if (Res1->name > Res2->name) { return 1; }

    return (-1);
}
