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
 * \file TYEtageEditor.h
 * \brief Construit un etage a partir des points saisis (fichier header)
 *
 *
 *
 *
 *
 */


#ifndef __TY_ETAGE_EDITOR__
#define __TY_ETAGE_EDITOR__

#include <qdialog.h>
#include <qlineedit.h>
#include <qradiobutton.h>
//Added by qt3to4:
#include <QBoxLayout>
#include <QLabel>

#include "TYPolyLineEditor.h"
#include "Tympan/models/business/TYDefines.h"

class QRadioButton;
class QLabel;
class QLineEdit;
class QBoxLayout;
class TabPointsWidget;

/**
 * \class TYEtageEditor
 * \brief Construit un etage a partir des points saisis
 */

class TYEtageEditor : public TYPolyLineEditor
{
    Q_OBJECT

public:
    TYEtageEditor(TYModelerFrame* pModeler);
    ~TYEtageEditor();

public slots:
    /**
     * \fn void endEtage()
     * \brief Construit un etage a partir des points saisis.
     */
    void endEtage();

protected:
    bool testCrossSegment(TYTabPoint& tabPts, bool &onlyEcran);
    void forceNormals(TYTabPoint& tabPts);
    void initZ(TYTabPoint& tabPts);
    void computeCenter(const double initialOffset, TYTabPoint& tabPts, TYRepere &rep);
    void buildFloor(TYTabPoint& tabPts, TYRepere& rep, double hauteur, bool closed);
    void buildScreen(TYTabPoint& tabPts, TYRepere& rep,  double height, double thickness);
};

class TYEtageEditorPropertiesDlg : public QDialog
{
    Q_OBJECT

public:
    TYEtageEditorPropertiesDlg(QWidget* parent, TYTabPoint &tabPts, bool onlyEcran = false);
    ~TYEtageEditorPropertiesDlg();

    /**
     * \fn double getHauteur()
     * \brief Retourne la hauteur saisie
     */
    double getHauteur() { return _pHauteurLineEdit->text().toDouble(); }
    /**
     * \fn double getHauteurSol()
     * \brief Retourne la hauteur au sol saisie
     */
    double getHauteurSol() { return _pHauteurSolLineEdit->text().toDouble(); }
    /**
     * \fn double getEpaisseur() { return _pEpaisseurLineEdit->text().toDouble()
     * \brief Retourne l'epaisseur saisie
     */
    double getEpaisseur() { return _pEpaisseurLineEdit->text().toDouble(); }

    /**
     * \fn bool etageSelected()
     * \brief Retourne si l'utilisateur a selectionne la construction d'un etage
     */
    bool etageSelected() { return _pBatBtn->isChecked(); }

protected slots:
    /**
     * \fn void toggleButton(int i)
     * \brief Callback qd l'utilisateur selectionne la creation d'un ecran
     */
    void toggleButton(int i);

    virtual void accept();

private:
    QRadioButton*   _pBatBtn;
    QRadioButton*   _pEcranBtn;
    QLabel*         _pEpaisseurLabelName;
    QLineEdit*      _pEpaisseurLineEdit;
    QLineEdit*      _pHauteurLineEdit;
    QLineEdit*      _pHauteurSolLineEdit;
    QBoxLayout*     _pHauteurLayout;
    QBoxLayout*     _pHauteurSolLayout;
    QBoxLayout*     _pEpaisseurLayout;
    QBoxLayout*     _pEcranOuBatLayout;
    QBoxLayout*     _pBtnLayout;

    TabPointsWidget *_tabPtsW;
};

#endif // __TY_ETAGE_EDITOR__
