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
 * \file TYResultatWidget.h
 * \brief outil IHM pour un resultat (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_RESULTAT_WIDGET__
#define __TY_RESULTAT_WIDGET__


#include <qwidget.h>
//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>
#include "TYWidget.h"

class TYResultat;
class QLineEdit;
class QGridLayout;
class QLabel;
class QGroupBox;
class QPushButton;
class QTableWidget;
class QTableWidgetItem;
class QComboBox;
class TYElementWidget;
class QRadioButton;
class QButtonGroup;
class QPrinter;
class TYSpectre;
class OSpectre;
class TYCalcul;

#include <string>

typedef struct
{
    int indice;
    double resultat;
    std::string name;
} TYStructIndiceResultat; // Utilise pour trier les resultats


/**
 * \class TYResultatWidget
 * \brief classe de l'objet IHM pour un resultat
 */

class TYResultatWidget : public TYWidget
{
    Q_OBJECT

    TY_DECL_METIER_WIDGET(TYResultat)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYResultatWidget(TYResultat* pElement, QWidget* _pParent = NULL);
    /**
     * Destructeur.
     */
    virtual ~TYResultatWidget();

    /**
     * Tri des numerique des resultats (remplace le tri de QTable non satisfaisant
     */
    void sortRes(int col);

    /**
     * Tri des resultats par nom
     */
    void sortName();

    /**
     * Initialisation du tableau de resultats
     */
    void initTabSort(int col = 1);

    /**
     * Visualise l'arborescence des contributions des sources
     */
    void showContribTreeDialog(int col);

private:
    void updateComboSubstCalculs();
    TYCalcul* getSelectedSubstCalcul();
    void updateHeaderLabel();

public slots:
    virtual void updateContent();
    virtual void apply();

    /**
     * Changement du calcul affiche
     */
    void changeSubstCalcul(int calcul);

    /**
     * Mise a jour de l'affichage de la table
     */
    void updateTable();

    /**
     * Fonction d'export des spectres pour chaque couple source recepteur
     */
    void exportSpectre();

    /**
     * Fonction d'export vers un fichier excel.
     *
     * @param select 0 pour exporter les valeurs dBLin, 1 pour dBA.
     */
    void exportExcel();

    /**
     * Imprime le tableau de resultats
     */
    void print();

    /**
     * Passe l'affichage de dBA a dBLin et inversement
     */
    void changeAffichage(int select);

    /// Changement du type d'operation
    void changeOperation(int select);

    /**
     * La valeur du seuil de contribution en dB a change
     */
    void slotContributionLineEditChanged();

protected slots:
    /**
     * Affiche un menu contextuel.
     */
    virtual void contextMenuEvent(QContextMenuEvent* e);

    /// CHangement de la frequence de travail
    virtual void setFrequency(int freq);

    void sortCol(int col);

private :
    /// Coloration de la cellule en fonction du contexte pour la synthese, pour le reste du tableau
    void decorsSynthese(QTableWidgetItem* pItem, const int& row, const int& col, const bool& valid, OSpectre& spectre1, OSpectre& spectre2);
    void decorsContributions(QTableWidgetItem* pItem, const int& row, const int& col, const bool& valid, OSpectre& spectre1, OSpectre& spectre2);


    TYSpectre getPuissanceElem(const unsigned int& i);
    TYSpectre getPuissanceElem(TYElement* pElement);

    /// Affichage du contenu de la cellule
    void affichageCellule(const int& row, const int& col, TYCalcul* pCalcOp = NULL);

    /// Definition des attributs d'une cellule (couleur de fond, police)
    void setCellAttributs(const int& row, const int& col, const OSpectre& spectre);


    //methode a remplir par Denis Thomasson
    bool isPresenceTonaliteMarquee(const OSpectre& spectre);

    /// Differents etats possibles de l'affichage (pourra etre complete ulterieurement)
    enum optionsAffichage {DBA, DBLIN, DBFREQ};

    /// Different type de cellule
    enum typeCellule {CELL_LW, CELL_SYNTH, CELL_CONTRIB};

    /// Remplissage du combo des frequences
    void updateFreqList();

    /// Recherche des maxima de contribution pour chaque recepteur
    void findMaxima();

    /// Calcul du contenu de la cellule en fonction du contexte (option de calcul)
    double resuAffichage(OSpectre& spectre1, OSpectre& spectre2, bool& valid, const int& typeCase);

    /// Calcul du spectre en fonction du contexte (option de calcul)
    OSpectre resuSpectre(OSpectre& spectre1, OSpectre& spectre2, const int& typeCase);

    /// Recherche du spectre associe a une ligne et une colonne pour un calcul
    OSpectre getSpectre(const int& row, const int& col, TYCalcul* pCalcul);



    // Membres
protected:
    QGridLayout* _resultatLayout;
    QTableWidget* _pTable;

    // Choix de l'affichage dBA, dBLin ou par frequence
    QRadioButton* _radioButtonDBA;
    QRadioButton* _radioButtonDBLIN;
    QRadioButton* _radioButtonDBFRQ;

    /// Combo pour le choix de la frequence de travail
    QComboBox* _comboBoxFreq;

    QLineEdit* _pContributionLineEdit;
    double  _seuilContribution;

    /// Choix du type de calcul
    QButtonGroup* _buttonGroupOperation;
    QRadioButton* _radioButtonNoOp;         // Pas d'operation
    QRadioButton* _radioButtonDelta;        // Difference entre les deux resultats
    QRadioButton* _radioButtonEmergence;    // Emergence par rapport a bruit de fond
    QRadioButton* _radioButtonAmbiant;      // Bruit ambiant

    // Choix du calcul associe au resultat
    QComboBox* _comboBoxSubstCalcul;

    // Capacite d'impression
    QPrinter* _printer;

    // Export EXCEL
    QPushButton* _pButtonExport;
    QPushButton* _pButtonExportSpectre;

    /// Mode d'affichage (dBA, dBLin, dB(f))
    int _choixAffichage;

    /// Type d'operation choisie
    int _choixOperation;

    /// Frequence de travail
    double _freq;

    /// Nombre de sources, de recepteurs
    unsigned int _nbSources;
    unsigned int _nbRecepteurs;

    /// Nombre de ligne, de colonnes
    unsigned int _nbLignes;
    unsigned int _nbColonnes;

    /// Tableau des maxima par colonne
    double* _max;

    /**
     * tableau des indices tries
     */
    TYStructIndiceResultat* _tabSortedRes;

};


#endif // __TY_RESULTAT_WIDGET__
