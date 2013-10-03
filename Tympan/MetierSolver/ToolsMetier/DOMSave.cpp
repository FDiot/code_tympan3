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



#include "DOMSave.h"

#include <qfile.h>
#include <qtextstream.h>

#include <iostream>
#include <fstream>
using namespace std;

#if defined(WIN32)
#include <crtdbg.h>
#endif

#if defined(WIN32)
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#endif
#endif


//////////////////////////////////////////////////////////////////////////////
// DOMSave
// ---------------------------------------------------------------------------
//  Local const data
//
//  Note: This is the 'safe' way to do these strings. If you compiler supports
//        L"" style strings, and portability is not a concern, you can use
//        those types constants directly.
// ---------------------------------------------------------------------------


int DOMSave::saveNodeToFile(DOM_Node& nodeToSave, const char* fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly))
    {
        return 3;
    }
    QTextStream textStream(&file);

    textStream << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\" standalone=\"no\"?>\n";

    // Is the node to save is already a document node print
    // it. Otherwise, create new document, import the node to save
    // into it and print it.
    if (nodeToSave.isDocument())
    {
        textStream << nodeToSave.toDocument().toString();
    }
    else
    {
        QDomDocument doc;
        doc.importNode(nodeToSave, true);
        textStream << doc.toString();
    }
    file.close();
    return 0;
}


QString DOMSave::saveNodeToString(DOM_Node& nodeToSave)
{
    QString outputString;
    QTextStream textSream(&outputString);

    //XXX Encoding of project files is hardcoded here to be Latin-1
    //TODO The handling of the encodings within XML, for file names and for the GUI is to be cleaned.
    textSream << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\" standalone=\"no\"?>\n";

    // Is the node to save is already a document node print
    // it. Otherwise, create new document, import the node to save
    // into it and print it.
    if (nodeToSave.isDocument())
    {
        textSream << nodeToSave.toDocument().toString();
    }
    else
    {
        QDomDocument doc;
        doc.importNode(nodeToSave, true);
        textSream << doc.toString();
    }
    return outputString;
}
