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

#ifndef __TY_ACOUSTICRECTANGLENODE__
#define __TY_ACOUSTICRECTANGLENODE__

#include "TYAcousticSurfaceNode.h"


typedef std::vector<LPTYRectangle> TYTabLPRectangle;

/**
 * Composee d'un ou plusieurs rectangles acoustiques.
 */
class TYAcousticRectangleNode: public TYAcousticSurfaceNode
{
    OPROTODECL(TYAcousticRectangleNode)
    TY_EXTENSION_DECL_ONLY(TYAcousticRectangleNode)
    TY_EXT_GRAPHIC_DECL_ONLY(TYAcousticRectangleNode)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYAcousticRectangleNode();
    /**
     * Constructeur par copie.
     */
    TYAcousticRectangleNode(const TYAcousticRectangleNode& other);
    /**
     * Destructeur.
     */
    virtual ~TYAcousticRectangleNode();

    ///Operateur =.
    TYAcousticRectangleNode& operator=(const TYAcousticRectangleNode& other);
    ///Operateur ==.
    bool operator==(const TYAcousticRectangleNode& other) const;
    ///Operateur !=.
    bool operator!=(const TYAcousticRectangleNode& other) const;

    virtual bool deepCopy(const TYElement* pOther, bool copyId = true);

    virtual std::string toString() const;

    virtual DOM_Element toXML(DOM_Element& domElement);
    virtual int fromXML(DOM_Element domElement);

    virtual TYTabSourcePonctuelleGeoNode getSrcs() const;
    virtual TYSourcePonctuelle srcPonctEquiv() const;
    virtual void distriSrcs();
    virtual bool setSrcsLw();
    virtual double activeSurface();
    virtual bool remAcousticSurf(const LPTYAcousticSurfaceGeoNode pAccSurfGeoNode);
    virtual bool remAcousticSurf(const LPTYAcousticSurface pAccSurf);
    virtual bool remAcousticSurf(QString idAccSurf);
    virtual void remAllSrcs();

    /**
     * Redimensionne le rectangle node en X en appliquant le meme facteur
     * d'echelle aux sous rectangles.
     *
     * @param sizeX La nouvelle taille en X.
     */
    virtual void setSizeX(float sizeX);
    /**
     * Redimensionne le rectangle node en Y en appliquant le meme facteur
     * d'echelle aux sous rectangles.
     *
     * @param sizeY La nouvelle taille en Y.
     */
    virtual void setSizeY(float sizeY);
    /**
     * Redimensionne le rectangle node en appliquant le meme facteur
     * d'echelle aux sous rectangles.
     *
     * @param sizeX La nouvelle taille en X.
     * @param sizeY La nouvelle taille en Y.
     */
    virtual void setSize(float sizeX, float sizeY);
    /**
     * Retourne les dimensions de ce rectangle node.
     *
     * @param sizeX La taille en X.
     * @param sizeY La taille en Y.
     */
    void getSize(float& sizeX, float& sizeY);
    /**
     * Retourne la taille en X de ce rectangle node.
     *
     * @return La taille en X.
     */
    virtual float getSizeX();
    /**
     * Retourne la taille en Y de ce rectangle node.
     *
     * @return La taille en Y.
     */
    virtual float getSizeY();

    /**
     * Applique un facteur d'echelle au rectangle.
     *
     * @param factorX Le facteur d'echelle en X.
     * @param factorY Le facteur d'echelle en Y.
     */
    void scale(float factorX, float factorY);
    /**
     * Applique un facteur d'echelle au rectangle.
     *
     * @param factor Le facteur d'echelle.
     */
    void scale(float factor) { scale(factor, factor); }

    /**
     * Decoupe cet objet en plusieurs elements de type AcousticRectangle
     * sous forme de grille.
     *
     * @param nbCol Nombre de colonnes de la grille de decoupage.
     * @param nbRow Nombre de lignes de la grille de decoupage.
     *
     * @return <code>true</code> si le decoupage a ete effectue;
     *         <code>false</code> sinon.
     */
    virtual bool makeGrid(int nbCol, int nbRow);

    /**
     * Ajoute un AcousticRectangleGeoNode a la grille.
     *
     * @param pAccRectGeoNode L'AcousticRectangleGeoNode a ajouter.
     *
     * @return <code>true</code> si le decoupage a ete effectue;
     *         <code>false</code> sinon.
     */
    bool addSubRect(LPTYAcousticRectangleGeoNode pAccRectGeoNode);
    /**
     * Ajoute un AcousticRectangle a la grille.
     *
     * @param pAccRect L'AcousticRectangle a ajouter.
     * @param repRect Le repere associe a l'AcousticRectangle.
     *
     * @return <code>true</code> si le decoupage a ete effectue;
     *         <code>false</code> sinon.
     */
    bool addSubRect(LPTYAcousticRectangle pAccRect, TYRepere repRect);

    /**
     * Decoupe cet objet en plusieurs elements de type AcousticRectangle
     * ou d'un type derive sous forme de grille.
     */
    virtual void updateGrid();


protected:
    /**
     * Decoupe cet objet en plusieurs elements de type AcousticRectangle
     * ou d'un type derive sous forme de grille.
     *
     * @param nbCol Nombre de colonnes de la grille de decoupage.
     * @param nbRow Nombre de lignes de la grille de decoupage.
     * @param childType Cette chaine de caractere permet de specifier le type des childs,
     *                  ceux-ci heritant de TYAcousticRectangle.
     *
     * @return <code>true</code> si le decoupage a ete effectue;
     *         <code>false</code> sinon.
     */
    bool makeGrid(int nbCol, int nbRow, const char* childType);

    /**
     * Decoupe cet objet en plusieurs elements de type AcousticRectangle
     * ou d'un type derive sous forme de grille.
     *
     * @param childType Cette chaine de caractere permet de specifier le type des childs,
     *                  ceux-ci heritant de TYAcousticRectangle.
     */
    void updateGrid(const char* childType);

private:
    /**
     * Recherche le rectangle le plus proche utilise dans la creation de la grille de decoupage
     * @param currentRect rectangle a etendre
     * @param tabGlobal tableau des rectangles
     * @param signe (+1/-1) pour indiquer la direction de la recherche
     * @param sens (0/1) pour indiquer si la recherche s'effectue sur l'axe horizontal (0) ou vertical (1)
     * @param nearestRect rectangle le plus proche dans la direction proposee
     * @return bool true si un rectangle a ete trouve
     */
    bool findNearestRect(const LPTYRectangle currentRect, const TYTabLPRectangle tabGlobal, const short signe, const short dir, TYRectangle& rect);

    // Membres
protected:

};


///Noeud geometrique de type TYAcousticRectangleNode.
typedef TYGeometryNode TYAcousticRectangleNodeGeoNode;
///Smart Pointer sur TYAcousticRectangleNodeGeoNode.
typedef SmartPtr<TYAcousticRectangleNodeGeoNode> LPTYAcousticRectangleNodeGeoNode;
///Collection de noeuds geometriques de type TYAcousticRectangleNode.
typedef std::vector<LPTYAcousticRectangleNodeGeoNode> TYTabAcousticRectangleNodeGeoNode;


#endif // __TY_ACOUSTICRECTANGLENODE__

