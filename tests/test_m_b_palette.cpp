// -*- coding: utf-8 -*-
//! \test test_study.cpp

#include "gtest/gtest.h"

#include "Tympan/gui/widgets/TYLookupTableWidget.h"
#include "Tympan/models/business/TYPalette.h"


TEST(TestTYPalette, color_conversions)
{
    QColor b = toQColor(OColor::BLUE);
    const QColor blue = QColor::fromRgb(0, 0, 255);
    EXPECT_EQ(blue, b);
    OColor c = toOColor(b);
    EXPECT_EQ(OColor::BLUE, c);
}

TEST(TestTYPalette, default_construction)
{
    LPTYPalette p_palette(new TYPalette());
    ASSERT_TRUE(p_palette);
    EXPECT_EQ(9, p_palette->getNbColors());
    // Test for hard-coded default values
    const float default_min = 40.0;
    const float default_max = 75.0;
    const OColor deepGreen(76.0f / 255.0f, 230.0f / 255.0f,          0.0f);
    const OColor lightGreen(85.0f / 255.0f,            1.0f,          0.0f);
    const OColor darkRed(168.0f / 255.0f,            0.0f,          0.0f);
    EXPECT_EQ(default_min, p_palette->getValueMin());
    EXPECT_EQ(default_max, p_palette->getValueMax());
    EXPECT_EQ(deepGreen, p_palette->getInfColor());
    EXPECT_EQ(darkRed, p_palette->getSupColor());
    EXPECT_EQ(darkRed, p_palette->getColorFromIndex(8));
    EXPECT_EQ(default_max, p_palette->getValueFromIndex(7));
    // Test getting the color
    OColor color;
    float value;
    value = default_min - 1.0 ;
    color = p_palette->getColorFromValue(value);
    EXPECT_EQ(deepGreen, color);
    EXPECT_EQ(deepGreen, p_palette->getColorFromIndex(0));
    EXPECT_EQ(default_min, p_palette->getValueFromIndex(0));
    value = default_min;
    color = p_palette->getColorFromValue(value);
    EXPECT_EQ(deepGreen, color);
    value = default_min + 1.0 ;
    color = p_palette->getColorFromValue(value);
    EXPECT_EQ(lightGreen, color);
    EXPECT_EQ(default_min + 5, p_palette->getValueFromIndex(1));
}

TEST(TestTYPalette, clear)
{
    LPTYPalette p_palette(new TYPalette());
    ASSERT_TRUE(p_palette);
    p_palette->clear();
    EXPECT_EQ(2, p_palette->getNbColors());
    // Test for hard-coded default values
    EXPECT_EQ(0, p_palette->getValueMin());
    EXPECT_EQ(0, p_palette->getValueMax());
    EXPECT_EQ(OColor::BLACK, p_palette->getInfColor());
    EXPECT_EQ(OColor::WHITE, p_palette->getSupColor());
}

TEST(TestTYPalette, inserters)
{
    LPTYPalette p_palette(new TYPalette());
    ASSERT_TRUE(p_palette);
    EXPECT_EQ(9, p_palette->getNbColors());
    // Use hard-coded default values
    float default_min = 40.0;
    float default_max = 75.0;
    const OColor deepGreen(76.0f / 255.0f, 230.0f / 255.0f,          0.0f);
    const OColor lightGreen(85.0f / 255.0f,            1.0f,          0.0f);
    const OColor darkRed(168.0f / 255.0f,            0.0f,          0.0f);
    OColor color;
    int n_colors = 9;
    float value;
    bool status;

    // Insertion at a non extremal existing value
    value = 60;
    status = p_palette->insertColor(value, OColor::MAGENTA);
    EXPECT_FALSE(status);
    EXPECT_EQ(n_colors, p_palette->getNbColors());
    color = p_palette->getColorFromValue(value);
    EXPECT_EQ(OColor::MAGENTA, color);

    // Insertion in the middle
    value = 53;
    status = p_palette->insertColor(value, OColor::CYAN);
    EXPECT_TRUE(status);
    EXPECT_EQ(++n_colors, p_palette->getNbColors());
    color = p_palette->getColorFromValue(value);
    EXPECT_EQ(OColor::CYAN, color);

    // Insertion after the maximal existing value
    value = default_max + 10;
    status = p_palette->insertColor(value, OColor::MAGENTA);
    EXPECT_TRUE(status);
    EXPECT_EQ(++n_colors, p_palette->getNbColors());
    EXPECT_FLOAT_EQ(value, p_palette->getValueMax());
    EXPECT_EQ(darkRed, p_palette->getSupColor()); // Sup color unchanged
    // Changing the sup color
    p_palette->setSupColor(OColor::BLACK);
    EXPECT_EQ(OColor::BLACK, p_palette->getSupColor()); // Sup color changed
    EXPECT_FLOAT_EQ(default_max + 10, p_palette->getValueMax());

    // Moving within a reasonable interval
    float old_value = 53;
    value = 54;
    status = p_palette->moveValue(old_value, value);
    EXPECT_TRUE(status);
    EXPECT_EQ(n_colors, p_palette->getNbColors());
    color = p_palette->getColorFromValue(value);
    EXPECT_EQ(OColor::CYAN, color);
    color = p_palette->getColorFromValue(old_value);
    EXPECT_EQ(OColor::CYAN, color);

    // Trying to move outside a reasonable interval
    old_value = 54;
    value = 60;
    status = p_palette->moveValue(old_value, value);
    EXPECT_FALSE(status);
    EXPECT_EQ(n_colors, p_palette->getNbColors());
    color = p_palette->getColorFromValue(value);
    EXPECT_EQ(OColor::MAGENTA, color);
    color = p_palette->getColorFromValue(old_value);
    EXPECT_EQ(OColor::CYAN, color);

    // Moving the inf
    old_value = default_min;
    value = 30;
    status = p_palette->moveValue(old_value, value);
    EXPECT_TRUE(status);
    EXPECT_EQ(n_colors, p_palette->getNbColors());
    color = p_palette->getColorFromValue(value);
    EXPECT_EQ(deepGreen, color);
    color = p_palette->getColorFromValue(old_value);
    EXPECT_EQ(lightGreen, color);
    EXPECT_FLOAT_EQ(value, p_palette->getValueMin());

    // Moving the sup
    ASSERT_FLOAT_EQ(default_max + 10, p_palette->getValueMax()); // The current max is at default_max+10
    old_value = default_max + 15;
    value = default_max + 20;
    status = p_palette->moveValue(old_value, value);
    EXPECT_TRUE(status);
    EXPECT_EQ(n_colors, p_palette->getNbColors());
    color = p_palette->getColorFromValue(value + 1);
    EXPECT_EQ(OColor::BLACK, color);
    color = p_palette->getColorFromValue(old_value);
    EXPECT_NE(OColor::BLACK, color);
    EXPECT_EQ(OColor::BLACK, p_palette->getSupColor());
    EXPECT_FLOAT_EQ(default_max + 20, p_palette->getValueMax());
}


void check_3_colors(LPTYPalette p_palette)
{
    EXPECT_EQ(3, p_palette->getNbColors());
    // Test for values in the test data
    const float value_min = 30.0;
    const float value_max = 80.0;
    EXPECT_EQ(value_min, p_palette->getValueMin());
    EXPECT_EQ(value_max, p_palette->getValueMax());
    EXPECT_EQ(OColor::BLUE, p_palette->getInfColor());
    EXPECT_EQ(OColor::RED, p_palette->getSupColor());
    // Test getting the color
    OColor color;
    color = p_palette->getColorFromValue(50.0);
    EXPECT_EQ(OColor::GREEN, color);
}

LPTYPalette build_3_colors()
{
    LPTYPalette p_palette(new TYPalette());
    p_palette->clear();
    p_palette->setInfColor(OColor::BLUE);
    p_palette->moveValue(0, 30);
    p_palette->setSupColor(OColor::RED);
    p_palette->insertColor(80, OColor::GREEN);
    return p_palette;
}

TEST(TestTYPalette, remove)
{
    LPTYPalette p_palette = build_3_colors();
    ASSERT_TRUE(p_palette);
    // Can't remove the infinity bound
    EXPECT_FALSE(p_palette->removeValue(TYPalette::infinity));
    EXPECT_EQ(3, p_palette->getNbColors());
    // Insert and remove a bound
    EXPECT_TRUE(p_palette->insertColor(60, OColor::MAGENTA));
    EXPECT_EQ(4, p_palette->getNbColors());
    EXPECT_EQ(OColor::MAGENTA, p_palette->getColorFromValue(60 - 1));
    EXPECT_FALSE(p_palette->removeValue(50)); // can't remove an inexisting bound
    EXPECT_EQ(4, p_palette->getNbColors());
    EXPECT_TRUE(p_palette->removeValue(60)); // can remove the exact bound
    EXPECT_EQ(3, p_palette->getNbColors());
    EXPECT_EQ(OColor::GREEN, p_palette->getColorFromValue(60));
    // Can remove a lower bound (here 30, the lower bound become 80)
    EXPECT_TRUE(p_palette->removeValue(30));
    EXPECT_EQ(2, p_palette->getNbColors());
    EXPECT_EQ(80, p_palette->getValueMin());
    // Can't remove the last finite (thus lower) bound
    EXPECT_FALSE(p_palette->removeValue(10));
    EXPECT_EQ(2, p_palette->getNbColors());
}

TEST(TestTYPalette, test_3_colors)
{
    LPTYPalette p_palette = build_3_colors();
    check_3_colors(p_palette);
}


TEST(TestTYPalette, load_legacy_from_file)
{
    QString filename = QString::fromUtf8("./data/legacy_3_colors_Palette.xml");
    LPTYPalette p_palette(TYPalette::loadPaletteFromFile(filename));
    ASSERT_TRUE(p_palette);
    check_3_colors(p_palette);
}

TEST(TestTYPalette, save_reload)
{
    QString filename = QString::fromUtf8("./tmp_palette_1.xml");
    LPTYPalette p_palette = build_3_colors();

    p_palette->savePaletteToFile(filename);
    LPTYPalette p_loaded_palette = TYPalette::loadPaletteFromFile(filename);
    EXPECT_EQ(p_palette->getNbColors(), p_loaded_palette->getNbColors());
    EXPECT_EQ(p_palette->getInfColor(), p_loaded_palette->getInfColor());
    EXPECT_EQ(p_palette->getSupColor(), p_loaded_palette->getSupColor());
    EXPECT_EQ(p_palette->getValueMin(), p_loaded_palette->getValueMin());
    EXPECT_EQ(p_palette->getValueMax(), p_loaded_palette->getValueMax());
    EXPECT_EQ(p_palette->getColorFromValue(50.0), p_loaded_palette->getColorFromValue(50.0));
}


