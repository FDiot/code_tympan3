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




#include "Tympan/core/defines.h"


#include <qtextstream.h>
#include <qfile.h>
#include <qstring.h>
#include <qdom.h>
#include "OXMLTreeManager.h"

#ifndef SAFE_DELETE
/**
 * Delete propre sur un objet.
 */
#define SAFE_DELETE(_p)         { if ((_p) != NULL) { delete(_p); (_p) = NULL; } }
#endif



//----------------------------------------------------------------------------
// public methods
//----------------------------------------------------------------------------

OXMLTreeManager::OXMLTreeManager()
{
    m_pDomDocument = NULL;
    m_CurrentDomNode.clear(); // NULL node
}

OXMLTreeManager::~OXMLTreeManager()
{
    if (m_pDomDocument != NULL)
    {
        delete m_pDomDocument;
        m_pDomDocument = NULL;
        m_CurrentDomNode.clear(); // NULL node
    }
}

bool OXMLTreeManager::create(const QString& docname)
{
    if (m_pDomDocument != NULL)
    {
        delete m_pDomDocument;
        m_pDomDocument = NULL;
        m_CurrentDomNode.clear(); // NULL node
    }

    m_pDomDocument = new QDomDocument(docname);
    if (m_pDomDocument == NULL)
    {
        return FALSE;
    }

    QDomElement elem = m_pDomDocument->createElement(docname);
    QDomNode resultnode;

    resultnode = m_pDomDocument->appendChild(elem);

    m_CurrentDomNode = getRootDomNode();
    return (!resultnode.isNull());

}


bool OXMLTreeManager::load(const QString& filename)
{
    if (m_pDomDocument != NULL)
    {
        SAFE_DELETE(m_pDomDocument);
        m_CurrentDomNode.clear(); // NULL node
    }

    m_pDomDocument = new QDomDocument(filename);
    if (m_pDomDocument == NULL)
    {
        return FALSE;
    }

    QFile filehandle(filename);

    if (filehandle.open(QIODevice::ReadOnly) == FALSE)
    {
        SAFE_DELETE(m_pDomDocument);
        return FALSE;
    }

    if (m_pDomDocument->setContent(&filehandle) == FALSE)
    {
        SAFE_DELETE(m_pDomDocument);
        filehandle.close();
        return FALSE;
    }

    filehandle.close();


    m_CurrentDomNode = getRootDomNode();

    return TRUE;
}

bool OXMLTreeManager::load(const QByteArray& bytearray)
{
    if (m_pDomDocument != NULL)
    {
        SAFE_DELETE(m_pDomDocument);
        m_CurrentDomNode.clear(); // NULL node
    }

    m_pDomDocument = new QDomDocument("");
    if (m_pDomDocument == NULL)
    {
        return FALSE;
    }

    if (m_pDomDocument->setContent(bytearray) == FALSE)
    {
        SAFE_DELETE(m_pDomDocument);
        return FALSE;
    }

    m_CurrentDomNode = getRootDomNode();
    return TRUE;
}


bool OXMLTreeManager::save(const QString& filename)
{
    if (m_pDomDocument == NULL)
    {
        return FALSE;
    }

    QDomNode* rootnode = (QDomNode*)m_pDomDocument;
    if (rootnode->isNull() == TRUE)
    {
        return FALSE;
    }


    QFile filehandle(filename);

    if (filehandle.open(QIODevice::WriteOnly) == FALSE)
    {

        return FALSE;
    }


    QTextStream textstream(&filehandle);

    rootnode->save(textstream, 0);

    filehandle.close();

    return TRUE;
}

bool OXMLTreeManager::isFieldPresent(const QString& field)
{
    QDomAttr resultattribute;

    return findField(field, getCurrentDomNode(), resultattribute);
}

bool OXMLTreeManager::getFieldType(const QString& field, FIELDTYPE& type)
{
    bool bConv;
    bool bFound;
    QDomAttr resultattribute;
    QString value;

    bFound = findField(field, getCurrentDomNode(), resultattribute);
    value = resultattribute.value();
    value.toFloat(&bConv);
    if (bConv == TRUE)
    {
        type = FLOAT;
    }
    else
    {
        value.toLong(&bConv);
        if (bConv == TRUE)
        {
            type = INTEGER;
        }
        else
        {
            type = STRING;
        }
    }
    return bFound;
}

bool OXMLTreeManager::addField(const QString& field, long value)
{
    bool bFound = TRUE;

    if (m_pDomDocument == NULL)
    {
        return FALSE;
    }

    getCurrentDomNode().toElement().setAttribute(field, (qlonglong)value);

    return bFound;
}

bool OXMLTreeManager::addField(const QString& field, float value)
{
    bool bFound = TRUE;

    if (m_pDomDocument == NULL)
    {
        return FALSE;
    }

    getCurrentDomNode().toElement().setAttribute(field, value);

    return bFound;
}

bool OXMLTreeManager::addField(const QString& field, const QString& value)
{
    bool bFound = TRUE;

    if (m_pDomDocument == NULL)
    {
        return FALSE;
    }

    getCurrentDomNode().toElement().setAttribute(field, value);

    return bFound;

}

bool OXMLTreeManager::getFieldLongValue(const QString& field, long& value)
{
    bool bFound;
    QDomAttr resultattribute;

    bFound = findField(field, getCurrentDomNode(), resultattribute);
    if (bFound == TRUE)
    {
        value = resultattribute.value().toLong();
    }

    return bFound;
}

bool OXMLTreeManager::getFieldFloatValue(const QString& field, float& value)
{
    bool bFound;
    QDomAttr resultattribute;

    bFound = findField(field, getCurrentDomNode(), resultattribute);
    if (bFound == TRUE)
    {
        value = resultattribute.value().toFloat();
    }

    return bFound;
}

bool OXMLTreeManager::getFieldStringValue(const QString& field, QString& value)
{
    bool bFound;
    QDomAttr resultattribute;

    bFound = findField(field, getCurrentDomNode(), resultattribute);
    if (bFound == TRUE)
    {
        value = resultattribute.value();
    }

    return bFound;
}

bool OXMLTreeManager::setFieldLongValue(const QString& field, long value)
{
    bool bFound = TRUE;

    getCurrentDomNode().toElement().setAttribute(field, (qlonglong)value);

    return bFound;
}

bool OXMLTreeManager::setFieldFloatValue(const QString& field, float value)
{
    bool bFound = TRUE;

    getCurrentDomNode().toElement().setAttribute(field, value);

    return bFound;
}

bool OXMLTreeManager::setFieldStringValue(const QString& field, const QString& value)
{
    bool bFound = TRUE;

    getCurrentDomNode().toElement().setAttribute(field, value);

    return bFound;
}

bool OXMLTreeManager::isDirectoryPresent(const QString& directory, bool absolute, int index)
{
    QDomNode startnode;

    return findDirectory(directory, startnode, absolute, index);
}

bool OXMLTreeManager::setCurrentDirectory(const QString& directory, bool absolute, int index)
{
    bool bSuccess;
    QDomNode startnode;

    bSuccess = findDirectory(directory, startnode, absolute, index);
    if (bSuccess == TRUE)
    {
        setCurrentDomNode(startnode);
    }
    return bSuccess;
}

bool OXMLTreeManager::setCurrentSubDirectory(int index)
{
    unsigned int i;
    signed long currentindex = index;
    QDomNode foundnode;
    QDomNodeList childnodes = getCurrentDomNode().childNodes();

    i = 0;
    while ((i < childnodes.count()) && (currentindex >= 0))
    {
        QDomNode readnode = childnodes.item(i);
        QDomElement e = readnode.toElement(); // try to convert the node to an element.
        if (e.isNull() == FALSE)
        {
            if (currentindex == 0)
            {
                foundnode = readnode;
            }
            currentindex--;
        }
        i++;
    }


    if (currentindex == -1)
    {
        setCurrentDomNode(foundnode);
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

bool OXMLTreeManager::setCurrentDirectoryUp()
{
    bool bSuccess = FALSE;
    QDomNode startnode = getCurrentDomNode();

    if ((startnode.isNull() == FALSE) && (isRoot(startnode) == FALSE))
    {
        setCurrentDomNode(startnode.parentNode());
        bSuccess = TRUE;
    }
    return bSuccess;
}


QString OXMLTreeManager::getCurrentDirectory()
{
    QString path = "";
    QDomNode node = getCurrentDomNode();

    while ((node.isNull() == FALSE) && (isRoot(node) == FALSE))
    {
        QDomElement e = node.toElement(); // try to convert the node to an element.
        if (!e.isNull())
        {
            path = e.tagName() + "." + path;
        }
        node = node.parentNode();
    }
    return path;
}

QString OXMLTreeManager::getCurrentSubDirectory()
{
    QString path = "";
    QDomNode node = getCurrentDomNode();

    if ((node.isNull() == FALSE) && (isRoot(node) == FALSE))
    {
        QDomElement e = node.toElement(); // try to convert the node to an element.
        if (!e.isNull())
        {
            path = e.tagName();
        }
    }
    return path;
}

unsigned long OXMLTreeManager::getSubDirectoryCount(const QString& directory)
{
    int count;
    QDomNode node = getCurrentDomNode();
    QDomNodeList childnodes;


    childnodes = node.childNodes();

    count = 0;

    for (unsigned int i = 0; i < childnodes.count(); i++)
    {
        QDomNode readnode = childnodes.item(i);
        QDomElement e = readnode.toElement(); // try to convert the node to an element.
        if (e.isNull() == FALSE)
        {
            if ((directory == QString::null) || (e.tagName() == directory))
            {
                count++;
            }
        }
    }

    return count;
}


unsigned long OXMLTreeManager::getFieldCount()
{
    QDomNode node = getCurrentDomNode();
    QDomNamedNodeMap namednodemap = node.toElement().attributes();

    return namednodemap.length();
}

bool OXMLTreeManager::addDirectory(const QString& directory)
{
    QDomNode foundnode;
    QDomNode resultnode;

    if (m_pDomDocument == NULL)
    {
        return FALSE;
    }

    if (findDirectory(directory, foundnode, FALSE) == FALSE)
    {
        // code to add directory here
        QDomElement elem = m_pDomDocument->createElement(directory);
        resultnode = getCurrentDomNode().appendChild(elem);
    }

    return (!resultnode.isNull());
}

bool OXMLTreeManager::remDirectory(const QString& directory)
{
    QDomNode foundnode;
    QDomNode resultnode;

    if (findDirectory(directory, foundnode, FALSE))
    {
        // code to delete directory here
        resultnode = getCurrentDomNode().removeChild(foundnode);
    }


    return (resultnode.isNull());
}

//----------------------------------------------------------------------------
// private methods
//----------------------------------------------------------------------------


bool OXMLTreeManager::findDirectory(const QString& directory, QDomNode& foundnode, bool absolute, int index)
{
    bool bError = FALSE;
    QDomNode startnode;
    QString workname = directory;

    if (absolute == TRUE)
    {
        startnode = getRootDomNode();
    }
    else
    {
        startnode = getCurrentDomNode();
    }

    while ((workname != "") && (bError == FALSE))
    {
        // Firstly, we extract the name of the first node to be searched for
        signed long dotindex;
        QString currentsearch;

        dotindex = workname.indexOf(".");
        if (dotindex == -1)
        {
            currentsearch = workname;
            workname = "";
        }
        else
        {
            currentsearch = workname;
            currentsearch.truncate(dotindex);
            workname.remove(0, dotindex + 1);
        }
        // Now we parse the elements to find if one is matching the current name
        QDomNode node = startnode.firstChild();

        bError = TRUE;
        while ((node.isNull() == FALSE) && (bError == TRUE))
        {
            QDomElement e = node.toElement(); // try to convert the node to an element.
            if (e.isNull() == FALSE)
            {
                if (e.tagName() == currentsearch)
                {
                    startnode = node;
                    if ((dotindex == -1) && (index > 0))
                    {
                        index--; // Don't stop the research by keeping bError to TRUE
                    }
                    else
                    {
                        bError = FALSE;
                    }
                }
            }
            node = node.nextSibling();
        }
    }

    if (bError == FALSE)
    {
        foundnode = startnode;
    }
    else
    {
        foundnode.clear();
    }
    return (!bError);
}

bool OXMLTreeManager::findField(const QString& field, const QDomNode& currentnode, QDomAttr& attribute)
{
    QDomNode readnode = currentnode;
    QDomElement currentelement = readnode.toElement();
    bool bFound = currentelement.hasAttribute(field);

    attribute = currentelement.attributeNode(field);

    return bFound;
}

