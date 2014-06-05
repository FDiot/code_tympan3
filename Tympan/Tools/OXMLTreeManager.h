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
 *
 *
 *
 */

#ifndef __O_XML_TREE_MANAGER__
#define __O_XML_TREE_MANAGER__


/**
 * This class is used to read an write miscellanous information in XML files.
 * It can sort information in a hierarchy like files on a hard drive.
 * It can store 3 types of data : text strings (encoded using the encoding of
 * the xml file, integer decimal values.
 * A "directory" corresponds to an element in a XML file.
 * A "field" corresponds to an attribute in a XML file.
 * Other objects than elements and attributes in the XML norm are correctly loaded and saved with this
 * class but they are not accessible.
 */
class OXMLTreeManager
{
public:
    /**
     * Default constructor.
     */
    OXMLTreeManager();
    /**
     * Destructor.
     */
    virtual ~OXMLTreeManager();

    /**
     * Reset all internal information and creates a doctype (for saving all the information stored in a OXMLTreeManager
     * object in a XML file
     */
    bool create(const QString& docname);

    /**
     * Loads a XML file from a disk using the filename parameter to access the file.
     * Returns FALSE, if the loading of the file fails or if the XML file is badly formed.
     */
    virtual bool load(const QString& filename);
    /**
     * Loads a XML file from memory using the QByteArray object the access the file previously loaded.
     * Returns FALSE, if the XML file is badly formed.
     */
    bool load(const QByteArray& bytearray);

    /**
     * Save in an XML file all the information stored in this OXMLTreeManager object.
     * Returns FALSE if the saving fails.
     */
    virtual bool save(const QString& filename);

    /**
     * Type of data stored in a filed.
     */
    enum FIELDTYPE
    {
        INTEGER,
        FLOAT,
        STRING
    };

    /**
      * returns TRUE if the field whose name is stored in the QString
      * object is present in the current directory.
      */
    bool isFieldPresent(const QString& field);

    /**
      * returns the FIEDTYPE of the field whose name is stored in the QString
      * object is present in the current directory.
      * @see FIELDTYPE
      */
    bool getFieldType(const QString& field, FIELDTYPE& type);

    /**
      * Adds a field in the current directory to store an integer value.
      * Retuns FALSE if it fails.
      */
    bool addField(const QString& field, long value);
    /**
      * Adds a field in the current directory to store an floating point value.
      * Retuns FALSE if it fails.
      */
    bool addField(const QString& field, float value);
    /**
      * Adds a field in the current directory to store a string.
      * Retuns FALSE if it fails.
      */
    bool addField(const QString& field, const QString& value);

    /**
      * Gets the integer value stored in the field in the current directory.
      * @param field Field name
      * @param value Reference to return a value
      * Retuns FALSE if it fails.
      */
    bool getFieldLongValue(const QString& field, long& value);
    /**
      * Gets the floating point value stored in the field in the current directory.
      * @param field Field name
      * @param value Reference to return a value
      * Retuns FALSE if it fails.
      */
    bool getFieldFloatValue(const QString& field, float& value);
    /**
      * Gets the string stored in the field in the current directory.
      * @param field Field name
      * @param value Reference to return a value
      * Retuns FALSE if it fails.
      */
    bool getFieldStringValue(const QString& field, QString& value);

    /**
      * Sets the integer value stored in the field in the current directory.
      * @param field Field name
      * @param value New integer value for the field
      * Retuns FALSE if it fails.
      */
    bool setFieldLongValue(const QString& field, long value);
    /**
      * Sets the floating point value stored in the field in the current directory.
      * @param field Field name
      * @param value New floating point value for the field
      * Retuns FALSE if it fails.
      */
    bool setFieldFloatValue(const QString& field, float value);
    /**
      * Sets the string stored in the field in the current directory.
      * @param field Field name
      * @param value New string to be store in the field
      * Retuns FALSE if it fails.
      */
    bool setFieldStringValue(const QString& field, const QString& value);

    /**
      * Returns TRUE if the directory whose name is store in the directory parameter is present.
      * This method behaves exactly like the SetCurrentDirectory() except it just check for the presence
      * of a directory object.
      * @see IsDirectoryPresent().
      */
    bool isDirectoryPresent(const QString& directory, bool absolute = FALSE, int index = 0);

    /**
      * Sets the current directory to a sub directory using a name an index.
      * Once found, the index is used. As several directories can have the same name (in relative access mode)
      * the index can be used to select more precisely a directory.
      * @param directory Directory name to select (can be relative or absolute).
      */
    bool setCurrentDirectory(const QString& directory, bool absolute = FALSE, int index = 0);
    /**
      * Sets the current directory to a sub directory using an index.
      */
    bool setCurrentSubDirectory(int index);
    /**
      * Sets the current directory to the upper directory.
      */
    bool setCurrentDirectoryUp();

    /**
      * Returns the "absolute path" name of the current directory.
      */
    QString getCurrentDirectory();
    /**
      * Returns the name of the current directory.
      */
    QString getCurrentSubDirectory();
    /**
      * Returns the number of directories whose name are stored in the directory parameter.
      * if a QString::null is passed, than this method returns the whole number of directories.
      */
    unsigned long getSubDirectoryCount(const QString& directory = QString::null);

    /**
      * Returns the number of fields in the current directory.
      */
    unsigned long getFieldCount();

    /**
      * Adds a directory in the current directory.
      * Retuns FALSE if it fails.
      */
    bool addDirectory(const QString& directory);
    /**
      * Remove a directory in the current directory.
      * Retuns FALSE if it fails.
      */
    bool remDirectory(const QString& directory);


private:
    inline void     setCurrentDomNode(const QDomNode& node);
    inline QDomNode getCurrentDomNode();
    inline QDomNode getRootDomNode();
    bool            findDirectory(const QString& directory, QDomNode& foundnode, bool absolute, int index = 0);
    bool            findField(const QString& field, const QDomNode& currentnode, QDomAttr& attribute);
    static bool         isRoot(const QDomNode& node) { return (node.parentNode().toElement().isNull()); }


private:
    QDomDocument*    m_pDomDocument;
    QDomNode        m_CurrentDomNode;

};

inline void OXMLTreeManager::setCurrentDomNode(const QDomNode& node)
{
    m_CurrentDomNode = node;
}

inline QDomNode OXMLTreeManager::getCurrentDomNode()
{
    return m_CurrentDomNode;
}

inline QDomNode OXMLTreeManager::getRootDomNode()
{
    QDomNode node;

    if (m_pDomDocument != NULL)
    {
        QDomElement docElem = m_pDomDocument->documentElement();
        node = docElem;
    }
    else
    {
        node.clear();
    }
    return node;
}


#endif // __O_XML_TREE_MANAGER__
