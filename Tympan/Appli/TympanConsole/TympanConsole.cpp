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

/*
 *
 */

#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHTympanConsole.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

///Chemin d'origine de l'application.
QString* _originalCurrentDirPath = NULL;

const QString& getOriginalCurrentDirPath()
{
    if (_originalCurrentDirPath)
    {
        return *_originalCurrentDirPath;
    }
    else
    {
#ifdef TYMPAN_DATA_DIR
        _originalCurrentDirPath = new QString(QString(TYMPAN_DATA_DIR) + "/tympan");
#else
        _originalCurrentDirPath = new QString(QDir::currentPath());
#endif
        return *_originalCurrentDirPath;
    }
}

void registerMetierObjet()
{
    TYLinearMaillage linearMaillage;
    TYMaillage maillage;
    TYPalette palette;
    TYPanel panel;
    TYRectangularMaillage rectangularMaillage;
    TYSourceBafflee sourceBafflee;
    TYSourceCheminee sourceCheminee;
    TYSourceLineic sourceLineic;
    TYTrafic trafic;
    TYAcousticBox acousticBox;
    TYAcousticFaceSet acousticFaceSet;
    TYAcousticLine acousticLine;
    TYBoucheSurface boucheSurface;
    TYChemineeSurface chemineeSurface;
    TYFaceSet faceSet;
    TYBatiment batiment;
    TYEcran ecran;
    TYReseauTransport reseauTransport;
    TYRoute route;
    TYCoursEau coursEau;
    TYPlanEau planEau;
}

int main(int argc, char** argv)
{
    // register Metier Objects
    registerMetierObjet();

    // Resources path
    QString ressourcePath(getOriginalCurrentDirPath() + "/resources/");
    ressourcePath = QDir::convertSeparators(ressourcePath);

    if (OLocalizator::setRessourceFile(ressourcePath + "Language.xml") == false)
    {
        std::cout << "Tympan fatal error : Resources file not found" << std::endl;
        exit(1);
    }
    OLocalizator::setRessourcePath(ressourcePath);

    //lancement de la console
    TYConsole console(getOriginalCurrentDirPath());
    console.run(argc, argv);

    return 0;
}

