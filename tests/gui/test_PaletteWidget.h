/*
 * test_Palette.h
 *
 *  Created on: 23 mai 2013
 *      Author: atr
 */

#ifndef TEST_PALETTE_H_
#define TEST_PALETTE_H_


#include <QtTest/QtTest>
#include <QtGui>
#include <QApplication>

#include "Tympan/models/business/TYPalette.h"
#include "Tympan/GraphicIHM/DataManagerIHM/TYPaletteWidget.h"

class TYPaletteWidgetTests: public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void testShow();


protected:
    static QString ressourcePath;

private:

    void initResources();

    TYPaletteWidget* w;
    LPTYPalette p;

    QMainWindow* main_w;
};


#endif /* TEST_PALETTE_H_ */
