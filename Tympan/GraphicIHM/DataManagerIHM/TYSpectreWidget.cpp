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
 * \file TYSpectreWidget.cpp
 * \brief outil IHM pour un spectre
 *
 *
 */




#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHIHM.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

#include <qtablewidget.h>
#include <qtabwidget.h>
#include <qpainter.h>
#include <qprinter.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qfiledialog.h>
#include <qradiobutton.h>
#include <qpixmap.h>
#include <qbuttongroup.h>
#include <qmessagebox.h>
#include <qheaderview.h>

#include "TYSpectreWidget.h"

#include "Tympan/Tools/OLocalizator.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantAcoustique/TYSpectre.h"
#include "Tympan/GraphicIHM/DataManagerIHM/TYPrintDialog.h"
#include "TYHistoWidget.h"
#include "TYCourbeWidget.h"

//Added by qt3to4:
#include <QHBoxLayout>
#include <QLabel>
#include <QGridLayout>
#include <QFrame>
#include <QBoxLayout>
#include <QPrintDialog>

using namespace Qt;

#define TR(id) OLocalizator::getString("TYSpectreWidget", (id))


TYSpectreWidget::TYSpectreWidget(TYSpectre* pElement, QWidget* _pParent /*=NULL*/):
    TYWidget(pElement, _pParent)
{
    // Par defaut precision prend la valeur affectee a la classe TYSpectre pour le stockage en XML
    _precision = TYSpectre::getXMLPrecision();

    // On utilise une copie du spectre associe a ce widget

    // Et on conserve ce spectre par ailleurs
    _pTmpSpectre = pElement;

    resize(450, 700);
    setWindowTitle(TR("id_caption"));

    _elmW = new TYElementWidget(getElement(), this);

    _spectreLayout = new QGridLayout();

    _spectreLayout->addWidget(_elmW, 0, 0);

    _groupBox = new QGroupBox(this);
    _groupBox->setTitle(TR(""));
    _groupBoxLayout = new QGridLayout();
    _groupBoxLayout->setAlignment(Qt::AlignTop);
    _groupBox->setLayout(_groupBoxLayout);

    _buttonGroup = new QButtonGroup();
    _buttonGroup->setExclusive(true);

    _radioButtonTiers = new QRadioButton();
    _radioButtonTiers->setText(TR("id_radiobutton_tiers"));

    _radioButtonOctave = new QRadioButton();
    _radioButtonOctave->setText(TR("id_radiobutton_octave"));

    QGridLayout* groupBoxRadioLayout = new QGridLayout();
    groupBoxRadioLayout->addWidget(_radioButtonTiers, 0, 0);
    groupBoxRadioLayout->addWidget(_radioButtonOctave, 0, 1);

    QGroupBox* groupBoxRadio = new QGroupBox();
    groupBoxRadio->setTitle("");
    groupBoxRadio->setLayout(groupBoxRadioLayout);

    _radioButtonTiers->setChecked((getElement()->getForm() == SPECTRE_FORM_TIERS));

    _groupBoxLayout->addWidget(groupBoxRadio, 0, 0, 1, 2);

    _lineEditRq = new QLineEdit(_groupBox);
    _lineEditRq->setText(getElement()->getRemarque());
    _groupBoxLayout->addWidget(_lineEditRq, 2, 1);

    _labeRq = new QLabel(_groupBox);
    _labeRq->setText(TR("id_rq_label"));
    _groupBoxLayout->addWidget(_labeRq, 2, 0);

    _comboBoxType = new QComboBox(_groupBox);
    _comboBoxType->insertItem(0, TR("id_spectre_type_att"));
    _comboBoxType->insertItem(1, TR("id_spectre_type_abso"));
    _comboBoxType->insertItem(2, TR("id_spectre_type_lw"));
    _comboBoxType->insertItem(3, TR("id_spectre_type_lp"));
    _comboBoxType->insertItem(4, TR("id_spectre_type_autre"));
    _comboBoxType->setCurrentIndex(getElement()->getType());
    _groupBoxLayout->addWidget(_comboBoxType, 1, 1);

    _labelType = new QLabel(_groupBox);
    _labelType->setText(TR("id_type_label"));
    _groupBoxLayout->addWidget(_labelType, 1, 0);

    _spectreLayout->addWidget(_groupBox, 1, 0);

    //Tableau
    _tableau = new QTableWidget();
    QStringList stringList;
    stringList.append(TR("id_freq"));
    stringList.append("");
    _tableau->setHorizontalHeaderLabels(stringList);
    _tableau->setColumnCount(2);
    _nbFreq = pElement->getNbValues();

    //histo
    _histoWidget = new TYHistoWidget(this, false);
    //courbe
    _courbeWidget = new TYCourbeWidget(this, false);

    _tabWidget = new QTabWidget(this);
    _tabWidget->insertTab(0, _tableau, TR("id_tableau"));
    _tabWidget->insertTab(1, _histoWidget, TR("id_histogramme"));
    _tabWidget->insertTab(2, _courbeWidget, TR("id_courbe"));

    _spectreLayout->addWidget(_tabWidget, 2, 0);

    QGroupBox* buttonGroupBox = new QGroupBox(this);
    buttonGroupBox->setTitle(TR(""));
    QGridLayout* buttonGroupBoxLayout = new QGridLayout();
    buttonGroupBox->setLayout(buttonGroupBoxLayout);
    buttonGroupBoxLayout->setAlignment(Qt::AlignTop);

    _pushButtonImport = new QPushButton(buttonGroupBox);
    _pushButtonImport->setText(TR("id_import_csv"));
    buttonGroupBoxLayout->addWidget(_pushButtonImport, 0, 0);

    _pushButtonExport = new QPushButton(buttonGroupBox);
    _pushButtonExport->setText(TR("id_export_csv"));
    buttonGroupBoxLayout->addWidget(_pushButtonExport, 0, 1);

    _pushButtonPrint = new QPushButton(buttonGroupBox);
    _pushButtonPrint->setText(TR("id_print"));
    buttonGroupBoxLayout->addWidget(_pushButtonPrint, 0, 2);

    _pushButtonFit = new QPushButton(buttonGroupBox);
    _pushButtonFit->setText(TR("id_fit"));
    _pushButtonFit->setCheckable(true);
    _pushButtonFit->setEnabled(false);
    buttonGroupBoxLayout->addWidget(_pushButtonFit, 0, 5);

    _spectreLayout->addWidget(buttonGroupBox, 3, 0);

    update();

    _printer = new QPrinter();

    connect(_radioButtonTiers, SIGNAL(clicked()), this, SLOT(changeOctave()));
    connect(_radioButtonOctave, SIGNAL(clicked()), this, SLOT(changeOctave()));
    connect(_pushButtonExport, SIGNAL(clicked()), this, SLOT(exportCsv()));
    connect(_pushButtonImport, SIGNAL(clicked()), this, SLOT(importCsv()));
    connect(_pushButtonPrint, SIGNAL(clicked()), this, SLOT(print()));
    connect(_tableau, SIGNAL(cellChanged(int, int)), this, SLOT(tabValueChanged(int, int)));
    connect(_tabWidget, SIGNAL(currentChanged(QWidget*)), this, SLOT(tabChanged(QWidget*)));

    setLayout(_spectreLayout);
    updateContent();
}

TYSpectreWidget::~TYSpectreWidget()
{
    delete _printer;
}

void TYSpectreWidget::updateContent()
{
    _elmW->updateContent();

    _lineEditRq->setText(_pTmpSpectre->getRemarque());
    _comboBoxType->setCurrentIndex(_pTmpSpectre->getType());

    _comboBoxType->setEnabled(false);

    // Affichage de l'unite
    QString unite = "dB";
    if (_pTmpSpectre->getType() == SPECTRE_TYPE_ABSO) { unite = ""; }

    TYElement* pParent = _pTmpSpectre->getParent();

    if (pParent && pParent->isA("TYAtmosphere")) { unite = "dB/m"; }

    QStringList stringList;
    stringList.append(TR("id_freq"));
    stringList.append(unite);
    _tableau->setHorizontalHeaderLabels(stringList);

    // Mise a jour du tableau
    spectreToTableau(_pTmpSpectre);

    // Inactivation de la colonne des frequences (toujours !)
    for (int i = 0; i < _tableau->rowCount(); i++)
    {
        _tableau->item(i, 0)->setFlags(0);
    }

    // Spectre en read only si le mode de calcul est "Calcule"
    if (_pTmpSpectre->getIsReadOnly()) { setContentEnabled(false); }
}

void TYSpectreWidget::apply()
{
    _elmW->apply();

    getElement()->setRemarque(_lineEditRq->text());
    getElement()->setType((TYSpectreType) _comboBoxType->currentIndex());

    // Mis a jour du spectre a partir du tableau
    tableauToSpectre(_pTmpSpectre);

    // On repasse le spectre en Tiers d'octave si necessaire
    if (_radioButtonOctave->isChecked())
    {
        _pTmpSpectre->setForm(SPECTRE_FORM_OCT);
        *_pTmpSpectre = _pTmpSpectre->toTOct();
    }

    emit modified();
}

void TYSpectreWidget::tabValueChanged(int row, int col)
{
    if (col == 1)
    {
        // Translate input value to double to see if it's a correct number 
        bool ok; 
        _tableau->item(row, col)->text().toDouble(&ok);

        // If value is not a valid number, get the original value in the spectrum and return
        if (!ok)
        {
            _tableau->item(row, 1)->setText(QString().setNum(_pTmpSpectre->getTabValReel()[row], 'f', _precision));
            return;
        }

        // Else, we copy value to all the selected cells
        for (int i = 0; i < _nbFreq; i++)
        {
            if ( ( _tableau->item(i, col) ) &&
                 ( _tableau->isItemSelected( _tableau->item(i, col) ) ) )
            {
                    _tableau->item(i, col)->setText(_tableau->item(row, col)->text());
            }
        }

        _isModified = true;
    }
}

void TYSpectreWidget::tabChanged(QWidget* w)
{
    if (_tabWidget->currentWidget() == _courbeWidget)
    {
        _pushButtonFit->setEnabled(false);
    }
    else if (_tabWidget->currentWidget() == _histoWidget)
    {
        _pushButtonFit->setEnabled(false);
    }
    else
    {
        _pushButtonFit->setEnabled(false);
    }
}

void TYSpectreWidget::setContentEnabled(bool state)
{
    for (int i = 0; i < _nbFreq; i++)
    {
        if (state)
        {
            _tableau->item(i, 1)->setFlags(Qt::ItemIsEditable);
        }
        else
        {
            _tableau->item(i, 1)->setFlags(0);
        }
    }

    _pushButtonImport->setEnabled(state);
}

void TYSpectreWidget::importCsv()
{
    std::vector<QString> tmp;

    QString qFileName = QFileDialog::getOpenFileName(this, "Choose a file",
                                                     "", "CSV (*.csv)");

    if (!qFileName.isEmpty())
    {
        // Initialisation du tableau
        resetTab();

        // On charge les donnees
        if (!qFileName.endsWith("csv"))
        {
            qFileName += ".csv";
        }

        QFile f(qFileName);
        if (f.open(QIODevice::ReadOnly))
        {
            QTextStream s(&f);
            QString txt;
            int i = 0;
            _tableau->setRowCount(0);

            while (!s.atEnd())
            {
                s >> txt;

                switch (i)
                {
                    case 0 :
                        break;
                    case 1 : // On saute la ligne des labels
                        break;
                    default :
                        tmp.push_back(txt);
                        break;
                }

                i++;
            }

            f.close();
        }

        // Cas d'un spectre en octave
        bool ok = true;
        int indice1 = -10000 ;
        int indice0 = TYSpectre::getIndice(tmp[0].section(";", 0, 0).toDouble(&ok));
        if (ok)
        {
            indice1 = TYSpectre::getIndice(tmp[1].section(";", 0, 0).toDouble(&ok));
        }

        bool bOctave = false;
        if (ok && ((indice1 - indice0) > 1)) // si spectre en octave l'ecart d'indice de la frequence est de 3
        {
            // On modifie le checkBox indiquant l'etat octave-1/3 d'octave ce qui convertit le spectre en octave
            _radioButtonOctave->setChecked(true);
            bOctave = true;
        }
        else // Spectre en (1/3)
        {
            // On modifie le checkBox indiquant l'etat octave-1/3 d'octave ce qui convertit le spectre en octave
            _radioButtonTiers->setChecked(true);
        }

        if ((bOctave && (tmp.size() < 9)) || (!bOctave && (tmp.size() < 31)))        // nb valeurs incorrect (9 en 1/1, 31 en 1/3)
        {
            int ret = QMessageBox::warning(this, "Tympan", TR("id_warning_file_not_ok"), QMessageBox::Yes, QMessageBox::No);
            if (ret == QMessageBox::No) { return ; }
        }

        // On remplit le tableau avec les valeurs (fait seulement maintenant a cause de "_radioButtonOctave->setChecked(true)" qui ecrase les valeurs
        _tableau->setRowCount(tmp.size() - 1);
        for (unsigned j = 0 ; j < tmp.size() ; j++)
        {
            _tableau->setItem(j, 0, new QTableWidgetItem(tmp[j].section(";", 0, 0)));
            _tableau->setItem(j, 1, new QTableWidgetItem(tmp[j].section(";", 1, 1)));
        }
    }

}

void TYSpectreWidget::exportCsv()
{
    QString qFileName = QFileDialog::getSaveFileName(this, "Choose a file",
                                                     "", "CSV (*.csv)");

    if (!qFileName.isEmpty())
    {
        if (!qFileName.endsWith(".csv"))
        {
            qFileName += ".csv";
        }

        QFile f(qFileName);

        if (f.open(QIODevice::WriteOnly))
        {
            QTextStream s(&f);

            // Ajout du type de spectre 1/1 ou 1/3
            int type = _radioButtonOctave->isChecked() ? 1 : 3;
            s << type << '\n';

            // Ajout entete de colonne
            s << TR("id_freq") << ";" << "dB\n";

            // Ecriture contenu du spectre
            int nbFreq = _tableau->rowCount();
            for (int i = 0; i < nbFreq; i++)
            {
                s << _tableau->item(i, 0)->text() << ";" << _tableau->item(i, 1)->text() << "\n";
            }

            f.close();
        }
    }
}

void TYSpectreWidget::print()
{
    if (_tabWidget->currentWidget() == _courbeWidget)
    {
        _courbeWidget->print();
    }
    else if (_tabWidget->currentWidget() == _histoWidget)
    {
        _histoWidget->print();
    }
    else
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
                paint.drawText(mid, _printer->height() * 14 / 15, pDialog->_lineEditTitre->text());
                paint.setFont(QFont());

                int sizeX = 80, sizeY = 25;
                int ymax = int(double(_printer->height()) * 4 / 5 / (_tableau->rowCount() + 1));
                if (ymax < sizeY) { sizeY = ymax; }
                int x = int (double(_printer->width() - sizeX * 2) / 2);
                int y = int ((double(_printer->height()) - sizeY * (_tableau->rowCount() + 1)) / 2);
                paint.drawText(x, y, TR("id_freq"));
                paint.drawText(x + sizeX + 20, y, "dB");
                //paint.translate(x, y + sizeY);
                paint.translate(-sizeX, sizeY);
                for (int i = 0; i < _tableau->rowCount(); i++)
                {
                    for (int j = 0; j < _tableau->columnCount(); j++)
                    {
                        paint.drawText(x + sizeX + 20, y, _tableau->item(i, j)->text());
                        paint.translate(sizeX, 0);
                    }
                    paint.translate(0, sizeY);
                    paint.translate(-_tableau->columnCount()*sizeX, 0);
                }
            }
        }
    }
}

static void processEvents();

void processEvents()
{
    while (qApp->hasPendingEvents())
    {
        qApp->processEvents();
    }
}

void TYSpectreWidget::changeOctave()
{
    LPTYSpectre pSpectre = new TYSpectre();
    pSpectre->setType(_pTmpSpectre->getType()); // Recopie du type du spectre edite

    // Effectue la conversion
    if (_radioButtonTiers->isChecked()) // Passage de 1/1 en 1/3
    {
        pSpectre->setForm(SPECTRE_FORM_OCT);

        // Creation des valeurs du spectre depuis le tableau
        tableauToSpectre(pSpectre.getRealPointer());

        // Passage du spectre en 1/3
        *pSpectre = pSpectre->toTOct();

    }
    else // Passage de 1/3 en 1/1
    {
        pSpectre->setForm(SPECTRE_FORM_TIERS);

        // Creation des valeurs du spectre depuis le tableau
        tableauToSpectre(pSpectre.getRealPointer());

        //Passage du spectre en 1/1
        *pSpectre = pSpectre->toOct();
    }

    // Remplissage du tableau
    spectreToTableau(pSpectre);
}

void TYSpectreWidget::changeType()
{
    if (_comboBoxType->currentIndex() == 1)
    {
        _histoWidget->setDbType(true);
        _courbeWidget->setDbType(true);
    }
    else
    {
        _histoWidget->setDbType(false);
        _courbeWidget->setDbType(false);
    }
}

void TYSpectreWidget::resetTab()
{
    // Reset tab
    for (int i = 0; i < _tableau->rowCount(); i++)
    {
        _tableau->takeItem(i, 0);
        _tableau->takeItem(i, 1);
    }

    _tableau->setRowCount(0);
}

void TYSpectreWidget::spectreToTableau(TYSpectre* pSpectre)
{
    assert(pSpectre);

    // Initialisation du tableau
    resetTab();

    // Recuperation du tableau des frequences (octave ou 1/3 d'octaves)
    TYTabFreq tabFreq = TYSpectre::getTabFreqNorm(pSpectre->getForm());

    int nbFreq = pSpectre->getNbValues() ;

    _tableau->setColumnCount(2);
    _tableau->setRowCount(nbFreq);
    for (int i = 0; i < nbFreq; i++)
    {
        _tableau->setItem(i, 0, new QTableWidgetItem((QString().setNum(tabFreq[i], 'f', 2))));
        _tableau->setItem(i, 1, new QTableWidgetItem((QString().setNum(pSpectre->getTabValReel()[i], 'f', _precision))));
    }
}

void TYSpectreWidget::tableauToSpectre(TYSpectre* pSpectre)
{
    assert(pSpectre);

    bool ok;
    double val = -200.0;
    int indexDepart = 0;

    int nbFreq =  _tableau->rowCount();

    // Pour toutes les frequences
    for (int i = indexDepart ; i < nbFreq ; i++)
    {
        val = _tableau->item(i, 1)->text().toDouble(&ok); // Suppose que la valeur a freq donnee stockee en colonne 1
        if (ok)
        {
            pSpectre->getTabValReel()[i] = val;
        }
    }
}

void TYSpectreWidget::spectre(TYSpectre* pSpectre)
{
    assert(pSpectre);

    if (_radioButtonOctave->isChecked())
    {
        *pSpectre = pSpectre->toOct();
    }

    tableauToSpectre(pSpectre);
}
