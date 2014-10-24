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
 * \file TYPaletteWidget.cpp
 * \brief Outil IHM pour une palette
 *
 *
 */





#include <limits>

#include <boost/foreach.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/math/special_functions/fpclassify.hpp>

#include "Tympan/models/business/OLocalizator.h"
#include "Tympan/core/logging.h"
#include "Tympan/core/color.h"
#include "TYLookupTableWidget.h"
#include "Tympan/models/business/TYPalette.h"
#include "Tympan/gui/widgets/TYLookupTableWidget.h"


#include <qcolordialog.h>
#include <qfiledialog.h>
#include <qpainter.h>
#include <qmessagebox.h>
#include <qtablewidget.h>
//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>


#include "TYPaletteWidget.h"

#define TR(id) OLocalizator::getString("TYPaletteWidget", (id))

TYPaletteWidget::TYPaletteWidget(TYPalette* pElement, QWidget* _pParent /*=NULL*/)
    : TYWidget(pElement, _pParent, NULL, Qt::Dialog)
{

    bool ok;

    resize(500, 800);
    setWindowTitle(TR("id_caption"));
    _paletteLayout = new QVBoxLayout();
    setLayout(_paletteLayout);

    // ELEMENT
    _elmW = new TYElementWidget(pElement, this);
    _paletteLayout->addWidget(_elmW);

    // ColorMap table
    QGroupBox* pGroupBoxColorMap = new QGroupBox(this);
    pGroupBoxColorMap->setTitle(TR("id_editarray"));
    QVBoxLayout* internalLayout = new QVBoxLayout();
    pGroupBoxColorMap->setLayout(internalLayout);
    _editor = new PaletteEditor(getElement());
    internalLayout->addWidget(_editor);
    _paletteLayout->addWidget(pGroupBoxColorMap, 1);

    // ColorMap preview
    QGroupBox* pGroupBoxPreview = new QGroupBox(this);
    pGroupBoxPreview->setTitle(TR("id_lookuptableview"));
    // NB The internal layout of the pGroupBoxPreview is set by TYLabeledLookupTableWidget()
    p_previewWidget =  new TYLabeledLookupTableWidget(getElement(), pGroupBoxPreview);
    _paletteLayout->addWidget(pGroupBoxPreview);

    ok = QObject::connect(_editor, SIGNAL(paletteChanged(const TYPalette*)), p_previewWidget, SLOT(update(const TYPalette*)));
    assert(ok);

    // SAVE/LOAD/RESET
    QGroupBox* pGroupBoxSaveLoad = new QGroupBox(this);
    pGroupBoxSaveLoad->setTitle(TR("id_saveload"));
    QGridLayout* pGroupBoxSaveLoadLayout = new QGridLayout();
    pGroupBoxSaveLoad->setLayout(pGroupBoxSaveLoadLayout);
    // Load button
    _pushButtonLoadPalette = new QPushButton(pGroupBoxSaveLoad);
    _pushButtonLoadPalette->setText(TR("id_load"));
    pGroupBoxSaveLoadLayout->addWidget(_pushButtonLoadPalette, 0, 0);
    QObject::connect(_pushButtonLoadPalette, SIGNAL(clicked()), this, SLOT(loadPalette()));
    // Save button
    _pushButtonSavePalette = new QPushButton(pGroupBoxSaveLoad);
    _pushButtonSavePalette->setText(TR("id_save"));
    pGroupBoxSaveLoadLayout->addWidget(_pushButtonSavePalette, 0, 1);
    QObject::connect(_pushButtonSavePalette, SIGNAL(clicked()), this, SLOT(savePalette()));
    // Reset button
    _pushButtonResetPalette = new QPushButton(pGroupBoxSaveLoad);
    _pushButtonResetPalette->setText(TR("id_reset"));
    pGroupBoxSaveLoadLayout->addWidget(_pushButtonResetPalette, 0, 2);
    QObject::connect(_pushButtonResetPalette, SIGNAL(clicked()), this, SLOT(resetPalette()));
    // Linear button
    _pushButtonLinearPalette = new QPushButton(pGroupBoxSaveLoad);
    _pushButtonLinearPalette->setText(TR("id_linear"));
    pGroupBoxSaveLoadLayout->addWidget(_pushButtonLinearPalette, 0, 3);
    QObject::connect(_pushButtonLinearPalette, SIGNAL(clicked()), this, SLOT(makeLinearPalette()));

    _paletteLayout->addWidget(pGroupBoxSaveLoad);

    updateContent();

}

TYPaletteWidget::~TYPaletteWidget() {}

void TYPaletteWidget::updateContent() // Override
{
    // Mise a jour de l'element
    _elmW->updateContent();
    _editor->notifyTYPaletteExternalyChanged();
}

void TYPaletteWidget::apply() // Override
{
    // Element
    _elmW->apply();
}

void TYPaletteWidget::resetPalette()
{
    if (!getElement()->resetFromPreferences())
    {
        getElement()->resetToDefault();
        getElement()->saveToPreferences();
    }
    updateContent();
}

void TYPaletteWidget::makeLinearPalette()
{
    getElement()->makeLinearPalette();
    updateContent();
}

void TYPaletteWidget::loadPalette()
{
    QString qFileName = QFileDialog::getOpenFileName(this, "Choose a file", QString::null, "XML (*.xml)");

    if (!qFileName.isEmpty())
    {
        if (!qFileName.endsWith(".xml"))
        {
            qFileName += ".xml";
        }

        bool loaded = false;
        LPTYPalette pPalette = TYPalette::loadPaletteFromFile(qFileName);
        if (pPalette)
        {
            (*getElement()) = *pPalette;
            updateContent();
            loaded = true;
        }
        else
        {
            OMessageManager::get()->error("Invalid Palette file %s : the one element is not a Palette.", qFileName.toUtf8().data());
        }

        if (loaded)
        {
            QMessageBox::information(this, "Tympan", TR("id_import_ok").arg(qFileName));
        }
        else
        {
            QMessageBox::warning(this, "Tympan", TR("id_import_failed").arg(qFileName));
        }
    }
}

void TYPaletteWidget::savePalette()
{
    QString qFileName = QFileDialog::getSaveFileName(this, "Choose a file",
                                                     QString::null, "XML (*.xml)");

    if (qFileName.isEmpty())
    {
        return;
    }

    if (!qFileName.endsWith(".xml"))
    {
        qFileName += ".xml";
    }

    bool save = true;

    // Verification de l'etat du fichier
    QFileInfo fi(qFileName);

    // Si le fichier existe , on demande confirmation a l'utilisateur
    if (fi.exists())
    {
        // On ecrase le fichier, puis on sauve
        QFile* file = new QFile(qFileName);
        switch (QMessageBox::warning(this, "Attention", TR("id_file_exist"), QMessageBox::Yes, QMessageBox::No))
        {
            case QMessageBox::Yes:
                if (!file->remove())
                {
                    QMessageBox::warning(this, "Attention", TR("id_file_cannot_be_clear"));
                    save = false;
                }
                break;
            case QMessageBox::No:
                save = false;
                break;
        }

        delete file;
    }

    if (save)
    {
        LPTYPalette pPalette = new TYPalette(*getElement());

        bool status = pPalette->savePaletteToFile(qFileName);
        if (status)
        {
            QMessageBox::information(this, "Tympan", TR("id_export_ok").arg(qFileName));
        }
        else
        {
            QMessageBox::warning(this, "Tympan", TR("id_export_failed").arg(qFileName));
        }
    }
}

/* --------------- class PaletteModel --------------- */

PaletteModel::PaletteModel(TYPalette* p_palette_, QObject* parent):
    p_palette(p_palette_),
    QAbstractTableModel(parent) {}

PaletteModel::~PaletteModel() {}

int PaletteModel::rowCount(const QModelIndex& parent) const
{
    return p_palette->getNbColors();
}
int PaletteModel::columnCount(const QModelIndex& parent) const
{
    return 2;
}

void PaletteModel::notifyTYPaletteExternalyChanged()
{
    reset();
};
void PaletteModel::getBoundsForValue(unsigned row, float& inf, float& sup) const

{
    inf = -std::numeric_limits<float>::infinity();
    sup = +std::numeric_limits<float>::infinity();

    if (row > 0)
    {
        inf = p_palette->getValueFromIndex(row - 1);
    }
    if (row < rowCount(QModelIndex()) - 1)
    {
        sup =  p_palette->getValueFromIndex(row + 1);
    }
}

QVariant PaletteModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
    {
        return QVariant();
    }
    switch (index.column())
    {
        case 0: // Request noise level (1st column)
            if (isSpecialInfiniteCell(index)) // Last cell can not be edited
            {
                if (role == Qt::DisplayRole)
                {
                    return TR("id_editarrayInfinity");
                }
            }
            switch (role)
            {
                case Qt::DisplayRole:
                    return QString("%1").arg(p_palette->getValueFromIndex(index.row()), 0, 'f', 1);
                    break;
                case Qt::EditRole:
                    return p_palette->getValueFromIndex(index.row());
                    break;
                case Qt::CheckStateRole:
                    return checked_rows.count(index.row()) ? Qt::Checked : Qt::Unchecked;
                    break;
                case Qt::TextAlignmentRole:
                    return Qt::AlignRight;
                    break;
            }
            break;
        case 1:// Request color (2nd column)
            switch (role)
            {
                case Qt::BackgroundColorRole:
                    OColor ocolor(p_palette->getColorFromIndex(index.row()));
                    return toQColor(ocolor);
                    break;
            }
            break;
        default:
            break;
    }
    return QVariant();
}

QVariant PaletteModel::headerData(int section, Qt::Orientation orientation,
                                  int role) const
{
    switch (section)
    {
        case 0:
            return TR("id_editarraycol0");
            break;
        case 1:
            return TR("id_editarraycol1");
            break;
        default:
            return QVariant();
            break;
    }
}

Qt::ItemFlags PaletteModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
    {
        return Qt::ItemIsEnabled;
    }
    Qt::ItemFlags flags = QAbstractTableModel::flags(index);
    flags |= Qt::ItemIsEditable;
    flags &= ~Qt::ItemIsSelectable;
    switch (index.column())
    {
        case 0:
            if (isSpecialInfiniteCell(index))
            {
                // No edition possible for the last row value
                flags &= ~Qt::ItemIsEditable;
                flags &= ~Qt::ItemIsUserCheckable;
            }
            else
            {
                flags |= Qt::ItemIsUserCheckable;
            }
            break;
        case 1:
            // Nothing specific yet
            break;
        default:
            assert(false && "Should never be reachable");
            break;
    }
    return flags;
}

bool PaletteModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (!index.isValid())
    {
        return false;
    }
    TYPalette::values_type real_value;
    QColor qcolor;
    bool ok = false;
    TYPalette::values_type old_value = p_palette->getValueFromIndex(index.row());

    switch (index.column())
    {
        case 0: // Request noise level (1st column)
            if (isSpecialInfiniteCell(index)) // Last cell can not be edited
            {
                return false;
            }
            switch (role)
            {
                case Qt::EditRole:
                    real_value = value.toFloat(&ok);
                    if (ok)
                    {
                        ok = p_palette->moveValue(old_value, real_value);
                        if (ok)
                        {
                            emit dataChanged(index, index);
                        }
                        return ok;
                    }
                    break;
                case Qt::CheckStateRole:
                    if (value == Qt::Checked)
                    {
                        ok = checked_rows.insert(index.row()).second;
                    }
                    else
                    {
                        ok = checked_rows.erase(index.row()) == 1;
                    }
                    break;
            }
            return ok;
            break;
        case 1:// Request color (2nd column)
            switch (role)
            {
                case Qt::BackgroundColorRole:
                case Qt::EditRole:
                    qcolor = value.value<QColor>();
                    p_palette->insertColor(old_value, toOColor(qcolor));
                    emit dataChanged(index, index);
                    return true;
                    break;
                default:
                    assert(false && "This should be an unreachable case");
                    break;
            }
            break;
        default:
            assert(false && "This should be an unreachable case");
            break;
    }
    return false; // to make the compiler happy with all exec path having a return.
}

void PaletteModel::deleteSelectedRows()
{
    BOOST_REVERSE_FOREACH(unsigned r, checked_rows)
    {
        beginRemoveRows(QModelIndex(), r, r);
        TYPalette::values_type value = p_palette->getValueFromIndex(r);
        bool ok = p_palette->removeValue(value);
        assert(ok && "This value must be valid");
        endRemoveRows();
    }
    checked_rows.clear();
    emit dataChanged(index(0, 0), index(rowCount() - 1, 1));
}

void PaletteModel::addRow(float noiseLevel)
{
    TYPalette::values_type value;
    OColor color;
    unsigned r = 0;
    // We look for the index r of the row to be created
    BOOST_FOREACH(boost::tie(value, color), p_palette->getColorMap())
    {
        if (value == noiseLevel)
        {
            return;    // We don't try to inser duplicates
        }
        if (value > noiseLevel)
        {
            break;
        }
        ++r;
    }
    // Now r is the index of the new row
    beginInsertRows(QModelIndex(), r, r);
    p_palette->insertColor(noiseLevel, OColor::WHITE);
    endInsertRows();
    emit dataChanged(index(r, 0), index(r, 1));
    checked_rows.clear();
}

/* --------------- class PaletteColorDelegate --------------- */

QWidget* PaletteColorDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option,
                                            const QModelIndex& index) const
{
    return new QWidget(parent);
}

void PaletteColorDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    bool ok;
    QColor qcolor = index.model()->data(index, Qt::BackgroundColorRole).value<QColor>();
    QPalette p = editor->palette();
    p.setColor(QPalette::Window, qcolor);
    p.setColor(QPalette::Base, qcolor);
    QColorDialog* dialog = new QColorDialog(editor);
    dialog->setCurrentColor(qcolor);
    ok = QObject::connect(dialog, SIGNAL(finished(int)), this, SLOT(validateColor()));
    assert(ok && "Connection failed for validateColor()");
    dialog->show();
}

void PaletteColorDelegate::validateColor()
{
    QColorDialog* dialog = qobject_cast<QColorDialog*>(this->sender());
    QWidget* editor = dialog->parentWidget();
    if (dialog->result() == QDialog::Accepted)
    {
        QColor qcolor = dialog->currentColor();
        QPalette p = editor->palette();
        p.setColor(QPalette::Window, qcolor);
        p.setColor(QPalette::Base, qcolor);
        editor->setPalette(p);
        emit commitData(editor);
    }
    emit closeEditor(editor, QAbstractItemDelegate::NoHint);
}

void PaletteColorDelegate::setModelData(QWidget* editor, QAbstractItemModel* model,
                                        const QModelIndex& index) const
{
    QColor color = editor->palette().color(QPalette::Base);
    model->setData(index, color);
}

/* --------------- class PaletteValueDelegate --------------- */

QWidget* PaletteValueDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option,
                                            const QModelIndex& index) const
{
    QDoubleSpinBox* editor = new QDoubleSpinBox(parent);
    editor->setDecimals(1);
    TYPalette::values_type inf, sup;
    const PaletteModel* model = qobject_cast<const PaletteModel*>(index.model());
    assert(model && "This cast shoul always succeed.");
    model->getBoundsForValue(index.row(), inf, sup);
    editor->setMinimum(inf);
    editor->setMaximum(sup);
    return editor;
}

void PaletteValueDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    bool ok;
    TYPalette::values_type value = index.model()->data(index).toFloat(&ok);
    QDoubleSpinBox* spinbox = qobject_cast<QDoubleSpinBox*>(editor);
    assert(spinbox && "THis cast is expecto to always work.");
    if (ok)
    {
        spinbox->setValue(value);
    }
    else
    {
        OMessageManager::get()->debug(QString("Invalid value for noise level : %1").arg(index.model()->data(index).toString()));
    }
}

void PaletteValueDelegate::setModelData(QWidget* editor, QAbstractItemModel* model,
                                        const QModelIndex& index) const
{
    QDoubleSpinBox* spinbox = qobject_cast<QDoubleSpinBox*>(editor);
    spinbox->interpretText();
    TYPalette::values_type value = spinbox->value();
    model->setData(index, value, Qt::EditRole);
}

/* --------------- class PaletteColorDelegate --------------- */

PaletteEditor::PaletteEditor(TYPalette* palette_, QWidget* parent) :
    QFrame(parent),
    p_palette(palette_),
    p_model(new PaletteModel(p_palette, this)),
    newNoiseLevelSpin(NULL),
    newNoiseLevelButton(NULL),
    deleteNoiseLevelButton(NULL),
    p_table(NULL),
    color_delegate(this),
    value_delegate(this)
{
    bool ok;

    QVBoxLayout* main_layout = new QVBoxLayout(this);
    QFrame* hframe = new QFrame();
    main_layout->addWidget(hframe);

    QHBoxLayout* addEntryLayout = new QHBoxLayout(hframe);
    addEntryLayout->addWidget(new QLabel(TR("id_noise_level"), hframe), 1);

    newNoiseLevelSpin = new QDoubleSpinBox();
    newNoiseLevelSpin->setSuffix(" dBA");
    newNoiseLevelSpin->setMaximum(1000);
    newNoiseLevelSpin->setDecimals(1);
    addEntryLayout->addWidget(newNoiseLevelSpin, 1);

    newNoiseLevelButton = new QPushButton();
    newNoiseLevelButton->setText(TR("id_newentry"));
    ok = QObject::connect(newNoiseLevelButton, SIGNAL(pressed()), this, SLOT(addNoiseLevel()));
    assert(ok && "Connection failed for addNoiseLevel");
    addEntryLayout->addWidget(newNoiseLevelButton, 1);


    deleteNoiseLevelButton = new QPushButton();
    deleteNoiseLevelButton->setText(TR("id_delete_sel"));
    ok = QObject::connect(deleteNoiseLevelButton, SIGNAL(pressed()), this, SLOT(deleteNoiseLevels()));
    assert(ok && "Connection failed for addNoiseLevel");
    addEntryLayout->addWidget(deleteNoiseLevelButton, 1);

    p_table = new QTableView(this);
    p_table->setModel(p_model);
    main_layout->addWidget(p_table, 1);
    p_table->setItemDelegateForColumn(0, &value_delegate);
    p_table->setItemDelegateForColumn(1, &color_delegate);

    ok = connect(p_model, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(propagateModelChanges(const QModelIndex&, const QModelIndex&)));
    assert(ok && "Connection failed for slot propagateModelChanges");
}

PaletteEditor::~PaletteEditor() {}

void PaletteEditor::notifyTYPaletteExternalyChanged()
{
    p_model->notifyTYPaletteExternalyChanged();
    emit paletteChanged(p_palette);
};

void PaletteEditor::propagateModelChanges(const QModelIndex& topLeft, const QModelIndex& bottomRight)
{
    emit paletteChanged(p_palette);
}

void PaletteEditor::addNoiseLevel()
{
    qobject_cast<PaletteModel*>(p_table->model())->addRow(newNoiseLevelSpin->value());
}

void PaletteEditor::deleteNoiseLevels()
{
    qobject_cast<PaletteModel*>(p_table->model())->deleteSelectedRows();
}
