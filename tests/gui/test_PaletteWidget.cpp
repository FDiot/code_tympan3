// -*- coding: utf-8 -*-
//! \test test_Palette.cpp

#include <QtTest/QtTest>

#include "test_PaletteWidget.h"

#include "Tympan/Tools/OLocalizator.h"

QTEST_MAIN(TYPaletteWidgetTests)
#include "test_PaletteWidget.moc"

void TYPaletteWidgetTests::testShow()
{
    /*
    QLineEdit lineEdit;
    QTest::keyClicks(&lineEdit, "hello world");
    QCOMPARE(lineEdit.text(), QString("hello world"));
    */
    QTest::qWait(10000);
}

void TYPaletteWidgetTests::initResources()
{
    // Resources path
    QString ressourcePath;
    ressourcePath = QDir::convertSeparators(QDir::currentPath() + "/../resources/");

    if (!((OLocalizator::setRessourceFile(ressourcePath + "Language.xml") == true)
          || (OLocalizator::setRessourceFile(ressourcePath + "Language_FR.xml") == true)
          || (OLocalizator::setRessourceFile(ressourcePath + "Language_EN.xml") == true)))
    {
        QMessageBox::critical(NULL, "Tympan fatal error", "Resources file not found : Language.xml");
    }
    else
    {
        OLocalizator::setRessourcePath(ressourcePath);
    }
}

void TYPaletteWidgetTests::initTestCase()
{
    //qDebug("called before everything else");

    initResources();
    //main_w = new QMainWindow();
    //qobject_cast<QApplication*>(QApplication::instance())->setActiveWindow(main_w);
    //main_w->show();
}

void TYPaletteWidgetTests::cleanupTestCase()
{
    // qDebug("called after myFirstTest and mySecondTest");
    //main_w->close();
    //delete main_w;
}

void TYPaletteWidgetTests::init()
{
    // qDebug("called before each test");
    p = new TYPalette(); // XXX or call a simple palette builder
    w = new TYPaletteWidget(p /*, main_w*/);
    w->show();
    QTest::qWaitForWindowShown(w);
}

void TYPaletteWidgetTests::cleanup()
{
    //qDebug("called after each test");
    w->close();
    delete w;
}
