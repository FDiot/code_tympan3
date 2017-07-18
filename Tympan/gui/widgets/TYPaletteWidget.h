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
 * \file TYPaletteWidget.h
 * \brief Outil IHM pour une palette (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_PALETTE_WIDGET__
#define __TY_PALETTE_WIDGET__

#include <vector>
#include <memory>
#include <set>

#include "Tympan/core/color.h"
#include "Tympan/models/business/TYDefines.h"

class TYElementWidget;
class TYLabeledLookupTableWidget;
class PaletteEditor;

#include "TYWidget.h"

#include <QtGui>
#include <QFrame>

class TYPalette;

class TYPaletteLimitsWidget : public QDialog
{
    Q_OBJECT

public:
    TYPaletteLimitsWidget(TYPalette* pElement, QWidget* _pParent = nullptr);

public slots:
    virtual void updateContent(); // Override
    virtual void apply(); // Override
protected:
    TYPalette* _pElement;
    QVBoxLayout* _layout;   
    QGroupBox* _groupBox;
    QLabel* _label_lower_bound;
    QLabel* _label_upper_bound;
    QLabel* _label_nb_colors;
    QLineEdit* _lineEdit_lower_bound;
    QLineEdit* _lineEdit_upper_bound;
    QLineEdit* _lineEdit_nb_colors;
};

/**
 * \class TYPaletteWidget
 * \brief classe de l'objet IHM pour une palette
 */
class TYPaletteWidget: public TYWidget
{
    Q_OBJECT

    TY_DECL_METIER_WIDGET(TYPalette)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYPaletteWidget(TYPalette* pElement, QWidget* _pParent = NULL);

    /**
     * Destructeur.
     */
    virtual ~TYPaletteWidget();

public slots:
    virtual void updateContent(); // Override
    virtual void apply(); // Override

    /**
     * Reset une palette
     */
    void resetPalette();

    /**
     * Charge une palette
     */
    void loadPalette();

    /**
     * Sauvegarde une palette
     */
    void savePalette();

    /**
     * Make a linear Palette
     */
    void makeLinearPalette();

protected:
    QVBoxLayout* _paletteLayout;

    // Element
    TYElementWidget* _elmW;
    PaletteEditor* _editor;
    TYLabeledLookupTableWidget* p_previewWidget;

    // Boutons Save/Load
    QPushButton* _pushButtonLoadPalette;
    QPushButton* _pushButtonSavePalette;
    QPushButton* _pushButtonResetPalette;
    QPushButton* _pushButtonLinearPalette;

private:
    void updateLegacyFromPalette();
};

// Auxilliary classes used to build the widget using Qt Model/View architecture

/**
 * \brief (Qt) Data model for a TYPalette
 */
class PaletteModel: public QAbstractTableModel
{
    Q_OBJECT

public:
    PaletteModel(TYPalette* p_palette, QObject* parent = NULL);
    virtual ~PaletteModel();

    void getBoundsForValue(unsigned i, float& inf, float& sup) const;

public slots:
    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    int columnCount(const QModelIndex& parent = QModelIndex()) const;

    QVariant data(const QModelIndex& index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    Qt::ItemFlags flags(const QModelIndex& index) const;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);

    void deleteSelectedRows();
    void addRow(float noiseLevel);

    void notifyTYPaletteExternalyChanged();

protected:
    bool isSpecialInfiniteCell(const QModelIndex& index) const
    {
        return index.row() == rowCount(QModelIndex()) - 1;
    }

protected:
    TYPalette* p_palette;
    std::set<unsigned> checked_rows;

};
// class PaletteModel

/**
 * \brief (Qt) Delegate for displaying and editing colors
 */
class PaletteColorDelegate: public QItemDelegate
{
    Q_OBJECT

public:
    PaletteColorDelegate(QObject* parent = NULL) :
        QItemDelegate(parent)
    {
    }
    ;

    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
                          const QModelIndex& index) const;

    void setEditorData(QWidget* editor, const QModelIndex& index) const;
    void setModelData(QWidget* editor, QAbstractItemModel* model,
                      const QModelIndex& index) const;
protected slots:
    void validateColor();

};
// class PaletteColorDelegate

/**
 * \brief (Qt) Delegate for displaying and editing colors
 */
class PaletteValueDelegate: public QItemDelegate
{
    Q_OBJECT

public:
    PaletteValueDelegate(QObject* parent = NULL) :
        QItemDelegate(parent)
    {
    }
    ;

    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
                          const QModelIndex& index) const;

    void setEditorData(QWidget* editor, const QModelIndex& index) const;
    void setModelData(QWidget* editor, QAbstractItemModel* model,
                      const QModelIndex& index) const;
};
// class PaletteColorDelegate

class PaletteEditor: public QFrame
{
    Q_OBJECT

public:
    PaletteEditor(TYPalette* palette, QWidget* parent = NULL);
    virtual ~PaletteEditor();
    void notifyTYPaletteExternalyChanged();

signals:
    void paletteChanged(const TYPalette* palette);

protected slots:
    void addNoiseLevel();
    void deleteNoiseLevels();
    void propagateModelChanges(const QModelIndex& topLeft,
                               const QModelIndex& bottomRight);

protected:
    QDoubleSpinBox* newNoiseLevelSpin;
    QPushButton* newNoiseLevelButton;
    QPushButton* deleteNoiseLevelButton;
    TYPalette* p_palette;
    QTableView* p_table;
    PaletteColorDelegate color_delegate;
    PaletteValueDelegate value_delegate;
    PaletteModel* p_model;

};
// class PaletteEditor

#endif // __TY_PALETTE_WIDGET__
