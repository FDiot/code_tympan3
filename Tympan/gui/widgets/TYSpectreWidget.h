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
 * \file TYSpectreWidget.h
 * \brief outil IHM pour un spectre (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_SPECTRE_WIDGET__
#define __TY_SPECTRE_WIDGET__


#include "TYWidget.h"
//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>
#include <QString>
class QLineEdit;
class QGridLayout;
class QLabel;
class QTreeWidget;
class QGroupBox;
class TYElementWidget;
class QComboBox;
class QTabWidget;
class QTableWidget;
class QPushButton;
class QPrinter;
class QRadioButton;
class QButtonGroup;
class TYSpectre;
class TYHistoWidget;
class TYCourbeWidget;


/**
 * \class TYSpectreWidget
 * \brief classe de l'objet IHM pour un spectre
 */
class TYSpectreWidget : public TYWidget
{
    Q_OBJECT

    TY_DECL_METIER_WIDGET(TYSpectre)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYSpectreWidget(TYSpectre* pElement, QWidget* _pParent = NULL);
    /**
     * Destructeur.
     */
    virtual ~TYSpectreWidget();

    /**
     * Get/Set de la precision d'affichage
     */
    void setPrecision(const int& prec) { _precision = prec; }
    int getPrecision() { return _precision; }
    const int getPrecision() const { return _precision; }

public :
    /**
        @brief Initialisation du tableau
    */
    void resetTab();

    /**
        @brief Mise a jour du tableau depuis un spectre
        @param pSpectre Pointeur sur un TYSpectre
    */
    void spectreToTableau(TYSpectre* pSpectre);

    /**
        @brief Mise a jour d'un spectre depuis le tableau
        @param pSpectre Pointeur sur un TYSpectre
    */
    void tableauToSpectre(TYSpectre* pSpectre);

    /**
     * @brief Construit un spectre a partir du contenu du widget
     *
     * A la difference difference de la methode tableauToSpectre, cette methode
     * cree un spectre pleinement fonctionnel
     *
     * @param pSpectre Pointeur sur un TYSpectre
     */
    void spectre(TYSpectre* pSpectre);

public slots:
    virtual void updateContent();
    virtual void apply();

    /**
     * Gere le changement de fiche.
     */
    void tabChanged(QWidget*);

    /**
     * Gere l'impression.
     */
    void print();

    /**
     * Fonction d'export vers un fichier csv.
     */
    void exportCsv();

    /**
     * Fonction d'import depuis un fichier csv.
     */
    void importCsv();

    /**
     * Modification du tableau de valeurs.
     */
    void tabValueChanged(int row, int col);

    /**
     * Gere le passage de tiers d'octave a octave et inversement.
     */
    void changeOctave();

    /**
     * Gere le changement de type.
     */
    void changeType();

    /**
     * Permet ou non de modifier le contenu de la widget.
     */
    void setContentEnabled(bool state);


    // Membres
protected:

    QGroupBox* _groupBox;
    QTreeWidget* _listViewTabSp;
    QLineEdit* _lineEditRq;
    QLabel* _labeRq;
    QComboBox* _comboBoxType;
    QLabel* _labelType;
    QTabWidget* _tabWidget;
    QTableWidget* _tableau;
    QPushButton* _pushButtonPrint;
    QPushButton* _pushButtonExport;
    QPushButton* _pushButtonImport;
    QRadioButton* _radioButtonTiers;
    QRadioButton* _radioButtonOctave;
    QPushButton* _pushButtonFit;
    QButtonGroup* _buttonGroup;
    TYHistoWidget* _histoWidget;
    TYCourbeWidget* _courbeWidget;

protected:
    QGridLayout* _spectreLayout;
    QGridLayout* _groupBoxLayout;

    TYElementWidget* _elmW;
    int _nbFreq;
    QPrinter* _printer;

public:
    TYSpectre* _pTmpSpectre;

protected:
    bool _isModified;

    /// Precision d'affichage des spectres
    int _precision;

private:
    bool _isDrawing;
	QString _etatSpectre;
};


#endif // __TY_SPECTRE_WIDGET__
