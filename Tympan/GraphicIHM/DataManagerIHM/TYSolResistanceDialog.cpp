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
 * \file TYSolResistanceDialog.cpp
 * \brief Outil IHM pour la saisie de la resisitivite du sol
 *
 *
 */





#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHIHM.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER
#include "TYSolResistanceDialog.h"


#include <qvariant.h>
#include <qframe.h>
#include <qslider.h>
#include <qtoolbutton.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qbuttongroup.h>
//Added by qt3to4:
#include <QLabel>
#include <QVBoxLayout>
#include <QWindowsStyle>
#include <QLineEdit>

#include "QPushButton"

#include "Tympan/models/business/OLocalizator.h"
#define TR(id) OLocalizator::getString("TYSolResistanceDialog", (id))

#include "Tympan/GraphicIHM/DataManagerIHM/TYColorInterfaceWidget.h"

///Liste des resistances pour les labels.
static double resistance[] =
{
    5, 10, 20, 50, 100, 200, 500, 1000, 2000, 5000, 10000, 20000
};


TYSolResistanceDialog::TYSolResistanceDialog(QWidget* parent,  double res /*=0*/, const char* name, bool modal, Qt::WFlags fl)
    : QDialog(parent, fl)
{
    setObjectName(name);
    setModal(modal);
    _res = res;
    if (!name)
    {
        resize(263, 536);
    }
    setWindowTitle(TR("id_resistance"));
    setSizePolicy(QSizePolicy(QSizePolicy::Fixed , QSizePolicy::Fixed));

    _frame6 = new QFrame(this);
    _frame6->setGeometry(QRect(0, 0, 280, 570));
    _frame6->setFrameShape(QFrame::StyledPanel);
    _frame6->setFrameShadow(QFrame::Raised);

    _line1_2_4 = new QFrame(_frame6);
    _line1_2_4->setGeometry(QRect(130, 220, 20, 20));
    _line1_2_4->setProperty("frameShape", (int)QFrame::HLine);
    _line1_2_4->setFrameShadow(QFrame::Sunken);
    _line1_2_4->setFrameShape(QFrame::HLine);

    _line1_2_11 = new QFrame(_frame6);
    _line1_2_11->setGeometry(QRect(130, 370, 20, 20));
    _line1_2_11->setProperty("frameShape", (int)QFrame::HLine);
    _line1_2_11->setFrameShadow(QFrame::Sunken);
    _line1_2_11->setFrameShape(QFrame::HLine);

    _line1_2_13 = new QFrame(_frame6);
    _line1_2_13->setGeometry(QRect(130, 410, 20, 20));
    _line1_2_13->setProperty("frameShape", (int)QFrame::HLine);
    _line1_2_13->setFrameShadow(QFrame::Sunken);
    _line1_2_13->setFrameShape(QFrame::HLine);

    _line1_2_6 = new QFrame(_frame6);
    _line1_2_6->setGeometry(QRect(130, 280, 20, 20));
    _line1_2_6->setProperty("frameShape", (int)QFrame::HLine);
    _line1_2_6->setFrameShadow(QFrame::Sunken);
    _line1_2_6->setFrameShape(QFrame::HLine);

    _line1_2 = new QFrame(_frame6);
    _line1_2->setGeometry(QRect(130, 190, 20, 20));
    _line1_2->setProperty("frameShape", (int)QFrame::HLine);
    _line1_2->setFrameShadow(QFrame::Sunken);
    _line1_2->setFrameShape(QFrame::HLine);

    _line1_2_5 = new QFrame(_frame6);
    _line1_2_5->setGeometry(QRect(130, 250, 20, 20));
    _line1_2_5->setProperty("frameShape", (int)QFrame::HLine);
    _line1_2_5->setFrameShadow(QFrame::Sunken);
    _line1_2_5->setFrameShape(QFrame::HLine);

    _line1_2_2 = new QFrame(_frame6);
    _line1_2_2->setGeometry(QRect(130, 130, 20, 20));
    _line1_2_2->setProperty("frameShape", (int)QFrame::HLine);
    _line1_2_2->setFrameShadow(QFrame::Sunken);
    _line1_2_2->setFrameShape(QFrame::HLine);

    _line1_2_7 = new QFrame(_frame6);
    _line1_2_7->setGeometry(QRect(130, 350, 20, 16));
    _line1_2_7->setProperty("frameShape", (int)QFrame::HLine);
    _line1_2_7->setFrameShadow(QFrame::Sunken);
    _line1_2_7->setFrameShape(QFrame::HLine);

    _line1_2_3 = new QFrame(_frame6);
    _line1_2_3->setGeometry(QRect(130, 110, 20, 20));
    _line1_2_3->setProperty("frameShape", (int)QFrame::HLine);
    _line1_2_3->setFrameShadow(QFrame::Sunken);
    _line1_2_3->setFrameShape(QFrame::HLine);

    _textLabel5 = new QLabel(_frame6);
    _textLabel5->setGeometry(QRect(40, 410, 66, 18));
    _textLabel5->setText(TR("id_neigefraiche"));

    _textLabel5_8_3 = new QLabel(_frame6);
    _textLabel5_8_3->setGeometry(QRect(40, 340, 55, 18));
    _textLabel5_8_3->setText(TR("id_neigedure"));

    _textLabel5_8_2 = new QLabel(_frame6);
    _textLabel5_8_2->setGeometry(QRect(40, 320, 72, 18));
    _textLabel5_8_2->setText(TR("id_terrevegetale"));

    _textLabel5_8 = new QLabel(_frame6);
    _textLabel5_8->setGeometry(QRect(40, 259, 59, 18));
    _textLabel5_8->setText(TR("id_solherbeux"));

    _textLabel5_8_4 = new QLabel(_frame6);
    _textLabel5_8_4->setGeometry(QRect(40, 230, 36, 18));
    _textLabel5_8_4->setText(TR("id_gazon"));

    _textLabel5_8_5 = new QLabel(_frame6);
    _textLabel5_8_5->setGeometry(QRect(40, 200, 49, 18));
    _textLabel5_8_5->setText(TR("id_guarrigue"));

    _textLabel5_8_6 = new QLabel(_frame6);
    _textLabel5_8_6->setGeometry(QRect(40, 170, 80, 18));
    _textLabel5_8_6->setText(TR("id_sablecompacte"));

    _textLabel5_8_7 = new QLabel(_frame6);
    _textLabel5_8_7->setGeometry(QRect(40, 100, 84, 18));
    _textLabel5_8_7->setText(TR("id_terrecompactee"));

    _textLabel5_8_11 = new QLabel(_frame6);
    _textLabel5_8_11->setGeometry(QRect(40, 80, 45, 18));
    _textLabel5_8_11->setText(TR("id_solpave"));

    _textLabel5_8_13 = new QLabel(_frame6);
    _textLabel5_8_13->setGeometry(QRect(40, 40, 44, 18));
    _textLabel5_8_13->setText(TR("id_asphalte"));

    _textLabel3 = new QLabel(_frame6);
    _textLabel3->setGeometry(QRect(10, 460, 133, 19));
    _textLabel3->setText(TR("id_resistance"));

    _line1 = new QFrame(_frame6);
    _line1->setGeometry(QRect(130, 40, 20, 20));
    _line1->setProperty("frameShape", (int)QFrame::HLine);
    _line1->setFrameShadow(QFrame::Sunken);
    _line1->setFrameShape(QFrame::HLine);

    _lineEdit1 = new QLineEdit(_frame6);
    _lineEdit1->setEnabled(TRUE);
    _lineEdit1->setGeometry(QRect(150, 460, 70, 20));

    _textLabel6 = new QLabel(_frame6);
    _textLabel6->setGeometry(QRect(230, 460, 30, 21));
    _textLabel6->setText("krayls");

    _pushButton1 = new QPushButton(_frame6);
    _pushButton1->setGeometry(QRect(90, 500, 80, 30));
    _pushButton1->setText(TR("id_ok_btn"));

    _pushButton2 = new QPushButton(_frame6);
    _pushButton2->setGeometry(QRect(180, 500, 80, 30));
    _pushButton2->setText(TR("id_cancel_btn"));

    _slider1 = new QSlider(_frame6);
    _slider1->setGeometry(QRect(160, 40, 30, 381));
    _slider1->setOrientation(Qt::Vertical);
    _slider1->setTickPosition(QSlider::TicksRight);
    _slider1->setTickInterval(9);
    _slider1->setTracking(false);

    QWidget* privateLayoutWidget = new QWidget(_frame6);
    privateLayoutWidget->setGeometry(QRect(200, 25, 50, 410));
    _layout1 = new QVBoxLayout();
    privateLayoutWidget->setLayout(_layout1);

    _textLabel4 = new QLabel(privateLayoutWidget);
    _textLabel4->setText("20000");
    _layout1->addWidget(_textLabel4);

    _textLabel4_2 = new QLabel(privateLayoutWidget);
    _textLabel4_2->setText("10000");
    _layout1->addWidget(_textLabel4_2);

    _textLabel4_3 = new QLabel(privateLayoutWidget);
    _textLabel4_3->setText("5000");
    _layout1->addWidget(_textLabel4_3);

    _textLabel4_4 = new QLabel(privateLayoutWidget);
    _textLabel4_4->setText("2000");
    _layout1->addWidget(_textLabel4_4);

    _textLabel4_5 = new QLabel(privateLayoutWidget);
    _textLabel4_5->setText("1000");
    _layout1->addWidget(_textLabel4_5);

    _textLabel4_6 = new QLabel(privateLayoutWidget);
    _textLabel4_6->setText("500");
    _layout1->addWidget(_textLabel4_6);

    _textLabel4_7 = new QLabel(privateLayoutWidget);
    _textLabel4_7->setText("200");
    _layout1->addWidget(_textLabel4_7);

    _textLabel4_8 = new QLabel(privateLayoutWidget);
    _textLabel4_8->setText("100");
    _layout1->addWidget(_textLabel4_8);

    _textLabel4_9 = new QLabel(privateLayoutWidget);
    _textLabel4_9->setText("50");
    _layout1->addWidget(_textLabel4_9);

    _textLabel4_10 = new QLabel(privateLayoutWidget);
    _textLabel4_10->setText("20");
    _layout1->addWidget(_textLabel4_10);

    _textLabel4_11 = new QLabel(privateLayoutWidget);
    _textLabel4_11->setText("10");
    _layout1->addWidget(_textLabel4_11);

    _textLabel4_12 = new QLabel(privateLayoutWidget);
    _textLabel4_12->setText("5");
    _layout1->addWidget(_textLabel4_12);

    _pButtonGroup = new QButtonGroup(_frame6);

    _toolButton1 = new QToolButton(_frame6);
    _toolButton1->setGeometry(QRect(10, 411, 20, 20));
    QPalette palette = _toolButton1->palette();
    palette.setColor(QPalette::Window, QColor(255, 255, 255));
    palette.setColor(QPalette::Button, QColor(255, 255, 255));
    _toolButton1->setPalette(palette);
    _toolButton1->setToolButtonStyle(Qt::ToolButtonTextOnly);
    _pButtonGroup->addButton(_toolButton1, 0);

    _toolButton1_8_3 = new QToolButton(_frame6);
    _toolButton1_8_3->setGeometry(QRect(10, 341, 20, 20));
    palette.setColor(QPalette::Window, QColor(225, 221, 213));
    palette.setColor(QPalette::Button, QColor(225, 221, 213));
    _toolButton1_8_3->setPalette(palette);
    _toolButton1_8_3->setToolButtonStyle(Qt::ToolButtonTextOnly);
    _pButtonGroup->addButton(_toolButton1_8_3, 4);

    _toolButton1_8_2 = new QToolButton(_frame6);
    _toolButton1_8_2->setGeometry(QRect(10, 321, 20, 20));
    palette.setColor(QPalette::Window, QColor(170, 170, 127));
    palette.setColor(QPalette::Button, QColor(170, 170, 127));
    _toolButton1_8_2->setPalette(palette);
    _toolButton1_8_2->setToolButtonStyle(Qt::ToolButtonTextOnly);
    _pButtonGroup->addButton(_toolButton1_8_2, 8);

    _toolButton1_8 = new QToolButton(_frame6);
    _toolButton1_8->setGeometry(QRect(10, 260, 20, 20));
    palette.setColor(QPalette::Window, QColor(0, 218, 0));
    palette.setColor(QPalette::Button, QColor(0, 218, 0));
    _toolButton1_8->setPalette(palette);
    _toolButton1_8->setToolButtonStyle(Qt::ToolButtonTextOnly);
    _pButtonGroup->addButton(_toolButton1_8, 6);

    _toolButton1_8_4 = new QToolButton(_frame6);
    _toolButton1_8_4->setGeometry(QRect(10, 231, 20, 20));
    palette.setColor(QPalette::Window, QColor(0, 124, 0));
    palette.setColor(QPalette::Button, QColor(0, 124, 0));
    _toolButton1_8_4->setPalette(palette);
    _toolButton1_8_4->setToolButtonStyle(Qt::ToolButtonTextOnly);
    _pButtonGroup->addButton(_toolButton1_8_4, 1);

    _toolButton1_8_5 = new QToolButton(_frame6);
    _toolButton1_8_5->setGeometry(QRect(10, 201, 20, 20));
    palette.setColor(QPalette::Window, QColor(224, 229, 66));
    palette.setColor(QPalette::Button, QColor(224, 229, 66));
    _toolButton1_8_5->setPalette(palette);
    _toolButton1_8_5->setToolButtonStyle(Qt::ToolButtonTextOnly);
    _pButtonGroup->addButton(_toolButton1_8_5, 3);

    _toolButton1_8_6 = new QToolButton(_frame6);
    _toolButton1_8_6->setGeometry(QRect(10, 171, 20, 20));
    palette.setColor(QPalette::Window, QColor(226, 191, 34));
    palette.setColor(QPalette::Button, QColor(226, 191, 34));
    _toolButton1_8_6->setPalette(palette);
    _toolButton1_8_6->setToolButtonStyle(Qt::ToolButtonTextOnly);
    _pButtonGroup->addButton(_toolButton1_8_6, 2);

    _toolButton1_8_7 = new QToolButton(_frame6);
    _toolButton1_8_7->setGeometry(QRect(10, 101, 20, 20));
    palette.setColor(QPalette::Window, QColor(111, 70, 32));
    palette.setColor(QPalette::Button, QColor(111, 70, 32));
    _toolButton1_8_7->setPalette(palette);
    _toolButton1_8_7->setToolButtonStyle(Qt::ToolButtonTextOnly);
    _pButtonGroup->addButton(_toolButton1_8_7, 9);

    _toolButton1_8_11 = new QToolButton(_frame6);
    _toolButton1_8_11->setGeometry(QRect(10, 81, 20, 20));
    palette.setColor(QPalette::Window, QColor(169, 194, 212));
    palette.setColor(QPalette::Button, QColor(169, 194, 212));
    _toolButton1_8_11->setPalette(palette);
    _toolButton1_8_11->setToolButtonStyle(Qt::ToolButtonTextOnly);
    _pButtonGroup->addButton(_toolButton1_8_11, 5);

    _toolButton1_8_13 = new QToolButton(_frame6);
    _toolButton1_8_13->setGeometry(QRect(10, 41, 20, 20));
    palette.setColor(QPalette::Window, QColor(146, 168, 184));
    palette.setColor(QPalette::Button, QColor(146, 168, 184));
    _toolButton1_8_13->setPalette(palette);
    _toolButton1_8_13->setToolButtonStyle(Qt::ToolButtonTextOnly);
    _pButtonGroup->addButton(_toolButton1_8_13, 7);

#if TY_PLATFORM == TY_PLATFORM_WIN32 || TY_PLATFORM == TY_PLATFORM_WIN64
    QWindowsStyle* wStyle = new QWindowsStyle();
    _toolButton1->setStyle(wStyle);
    _toolButton1_8_3->setStyle(wStyle);
    _toolButton1_8_2->setStyle(wStyle);
    _toolButton1_8->setStyle(wStyle);
    _toolButton1_8_4->setStyle(wStyle);
    _toolButton1_8_5->setStyle(wStyle);
    _toolButton1_8_6->setStyle(wStyle);
    _toolButton1_8_7->setStyle(wStyle);
    _toolButton1_8_11->setStyle(wStyle);
    _toolButton1_8_13->setStyle(wStyle);
#endif


    connect(_slider1, SIGNAL(valueChanged(int)), this, SLOT(sliderChanged(int)));
    connect(_lineEdit1, SIGNAL(textChanged(const QString&)), this, SLOT(lineEditChanged(const QString&)));
    connect(_pushButton1, SIGNAL(clicked()), this, SLOT(accept()));
    connect(_pushButton2, SIGNAL(clicked()), this, SLOT(reject()));
    connect(_pButtonGroup, SIGNAL(buttonClicked(int)), this, SLOT(buttonValue(int)));

    _mode = 1;
    QString qString;
    _lineEdit1->setText(qString.setNum(_res));
    _mode = 0;
}

/*
 *  Destroys the object and frees any allocated resources
 */
TYSolResistanceDialog::~TYSolResistanceDialog()
{
    // no need to delete child widgets, Qt does it all for us
}

void TYSolResistanceDialog::sliderChanged(int value)
{
    if (_mode == 0)
    {
        double val = double(value) / 9;
        int i = int(val);
        _res = resistance[i] + (val - double(i)) * (resistance[i + 1] - resistance[i]);
        QString qString;
        _lineEdit1->setText(qString.setNum(_res));
    }

    _mode = 0;
}

void TYSolResistanceDialog::lineEditChanged(const QString& sValue)
{
    if (_mode == 1)
    {
        int i = 0;
        double value = sValue.toDouble();
        _res = value;
        while ((value > resistance[i + 1]) && (i < 12))
        {
            i++;
        }

        if (i == 12)
        {
            _slider1->setValue(100);
        }
        else
        {
            double res = (i + (value - resistance[i]) / (resistance[i + 1] - resistance[i])) * 9;
            _slider1->setValue(int(res));
        }
    }

    _mode = 1;
}

void TYSolResistanceDialog::buttonValue(int button)
{
    switch (button)
    {
        case 0: _res = 5.55;
            _lineEdit1->setText(QString().setNum(_res));
            break;
        case 4: _res = 26.66;
            _lineEdit1->setText(QString().setNum(_res));
            break;
        case 8: _res = 45;
            _lineEdit1->setText(QString().setNum(_res));
            break;
        case 6: _res = 156;
            _lineEdit1->setText(QString().setNum(_res));
            break;
        case 1: _res = 350;
            _lineEdit1->setText(QString().setNum(_res));
            break;
        case 3: _res = 666;
            _lineEdit1->setText(QString().setNum(_res));
            break;
        case 2: _res = 1300;
            _lineEdit1->setText(QString().setNum(_res));
            break;
        case 9: _res = 6666;
            _lineEdit1->setText(QString().setNum(_res));
            break;
        case 5: _res = 9500;
            _lineEdit1->setText(QString().setNum(_res));
            break;
        case 7: _res = 20000;
            _lineEdit1->setText(QString().setNum(_res));
            break;
    }
    lineEditChanged(_lineEdit1->text());
}

