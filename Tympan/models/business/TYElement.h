/*
 * Copyright (C) <2012-2014> <EDF-R&D> <FRANCE>
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

#ifndef __TY_ELEMENT__
#define __TY_ELEMENT__


#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif // HAVE_CONFIG_H

#if _MSC_VER > 1000
  #pragma once
#endif // _MSC_VER > 1000
#ifdef _MSC_VER
  #pragma warning( disable : 4275 4800 4355 4273 4786 4018 4503 4081)
#endif

#include <memory>
#include <unordered_map>
#include <iostream>

#include <boost/exception/error_info.hpp>

#ifndef _NDEBUG
  #include <QDebug>
#endif // _NDEBUG

#include "Tympan/core/defines.h"
#include "Tympan/core/macros.h"
#include "Tympan/core/idgen.h"
#include "Tympan/models/business/TYNameManager.h"
#if TY_USE_IHM
  #include "Tympan/gui/widgets/TYElementWidget.h"
  #include "Tympan/gui/gl/TYElementGraphic.h"
  #include "TYPreferenceManager.h"
#endif // TY_USE_IHM
#include "TYDefines.h"
#include "TYXMLTools.h"


// Nom de la categorie pour la sauvegarde des preferences.
#define TYDIRPREFERENCEMANAGER "UserPreferences"

/////////////////////////////////////////////////////////////
// Definition de macros pour la creation de nouveaux types
// derives de OPrototype.

/**
 * Macro pour la declaration des methodes a surcharger
 * de la classe OPrototype, et pour la registration du type
 * 'classname' dans la Prototype Factory.
 * Cette macro doit etre appelee dans la declaration de la
 * nouvelle classe derivee (.h).
 */
#define OPROTODECL(classname) \
    public: \
    virtual OPrototype* clone() const { \
        return new classname(); \
    } \
    virtual const char* getClassName() const { return #classname; } \
    static classname* safeDownCast(OPrototype * pObject) { \
        if (pObject) { \
            classname * pTypedObject = dynamic_cast<classname *>(pObject); \
            if (pTypedObject != nullptr) { \
                return pTypedObject; \
            } \
        } \
        return nullptr;\
    } \

/////////////////////////////////////////////////////////////

namespace tympan
{
     typedef boost::error_info < struct tag_classname, std::string >
         oproto_classname_errinfo;
} // namespace tympan


/**
 * Classe abstraite Prototype du pattern Prototype Factory.
 * Les classes derivees de Prototype peuvent etre instanciees a partir
 * seulement du nom de la classe (chaine de caracteres).
 *
 * Pour cela, la map statique _factory_map associe a une chaine de caracteres
 * representant le nom de la classe la factory permettant de construire une
 * instance de ce type
 *
 * La methode public static 'findAndClone()' est l'interface principale
 * de ce mecanisme, elle permet d'instancier un nouvel objet a partir
 * de son nom (a condition que le type correspondant existe et soit
 * deja enregistre dans Prototype au moment de l'appel). Si le nom n'est pas
 * connu au moment de l'appel, elle leve une exception 'tympan::invalid_data'.
 *
 * La methode 'findPrototype()' cherche si une classe est enregistree aupres
 * de OPrototype.
 *
 * Enfin, la methode virtuelle pure 'clone()' permet aux classes derivees de
 * retourner une nouvelle instance du type derive correspondant.
 *
 * De plus, la methode 'isA()' permet de verifier le type d'une instance.
 *
 * L'exemple suivant presente le squelette a reprendre pour creer des classe
 * derivees de Prototype, c'est-a-dire de nouveaux composants. La classe
 * derivee pour cet exemple se nomme 'ConcretePrototype'.
 *
 * <pre>
 * // Declaration (ConcretePrototype.h)
 *
 * class ConcretePrototype : public Prototype {
 *
 * public:
 *    // Constructeur par defaut.
 *    ConcretePrototype();
 *
 *    //...//
 *
 *    // Permet de creer une nouvelle instance de type ConcretePrototype.
 *    virtual Prototype* clone() const {
 *      return new ConcretePrototype();
 *    }
 *
 *    // Retourne le nom de la classe.
 *    virtual char* getClassName() { return "ConcretePrototype"; }
 *
 * };
 * </pre>
 *
 * La macro OPROTODECL a ete concue pour reprendre ces declarations comme suit:
 *
 * <pre>
 * #define  OPROTODECL(classname) \
 *    friend register##classname(); \
 *  public: \
 *    virtual OPrototype* clone(bool andCopy = false) const { \
 *      if (andCopy) return new classname(*this); \
 *      else return new classname(); \
 *    } \
 *    virtual char* getClassName() { return #classname; }
 * </pre>
 *
 *
 * La declaration d'une nouvelle classe s'en trouve alors simplifiee, il suffit
 * de faire appel a la macro. L'exemple precedent devient donc :
 *
 * <pre>
 *
 * class ConcretePrototype : public Prototype {
 *    OPROTODECL(ConcretePrototype)
 *
 * public:
 *    // Constructeur par defaut.
 *    ConcretePrototype();
 *
 *    //...//
 * };
 *
 * // Implementation (ConcretePrototype.cpp)
 *
 * #include "ConcretePrototype.h"
 *
 *
 * ConcretePrototype::ConcretePrototype()
 * {
 * }
 *
 * </pre>
 *
 * Cette technique d'enregistrement n'est donc pas autonome puisqu'il est
 * necessaire de reprendre le modele ci-dessus pour chaque nouvelle classe
 * derivee de Prototype.
 *
 *
 * ATTENTION: en complement des declarations des classes via la macro OPROTODECL,
 * il faut egalement enregistrer celles-ci aupres de OProrotype. Cette action doit
 * etre effectuee avant toute utilisation des methodes de creation d'instances
 * et de recherche de classes definies par OPrototype ('findPrototype()', 'clone()'
  et 'findAndClone()').
 * Cet enregistrement s'effectue au moyen de la methode statique 'add_factory()',
 * qui permet d'enregistrer les differentes classes ainsi que leurs fabriques
 * (utiliser la fonction 'build_factory<T>()' pour la fabrique).
 * Exemple d'utilisation:
 *      OPrototype::add_factory("TYTopographie", move(build_factory<TYTopographie>()));
 *
 * (voir Tympan/models/business/init_registry.{h,cpp})
 */


class OPrototype
{
    // Methodes
public:
    /**
     * Destructeur.
     */
    virtual ~OPrototype();


    /**
     * Recherche une classe par son nom dans le tableau des
     * classes registrees, une nouvelle instance est creee et
     * retournee si le type a ete trouvee.
     *
     * @param   className Le nom de la classe a chercher puis cloner.
     *
     * @return  Une nouvelle instance du type recherche ou NULL s'il
     *          n'a pas ete trouve.
     */
    static OPrototype* findAndClone(const char* className);

    /**
     * Recherche une classe par son nom dans le tableau des
     * classes registrees.
     *
     * @param className Le nom de la classe a chercher.
     *
     * @return L'index dans le tableau si la classe a ete registree;
     *         -1 sinon.
     */
    static int findPrototype(const char* className);

    /**
     * Instancie un nouvel objet du meme type que cet objet.
     * Il est aussi possible de retourner une copie.
     *
     * @param andCopy Pour que l'objet clone soit une copie de cet objet.
     *
     * @return Une nouvelle instance du type derive.
     */
    virtual OPrototype* clone() const = 0;

    /**
     * Retourne le type de la classe sous la forme d'une
     * chaine de caractere.
     *
     * @return Le nom de la classe.
     */
    virtual const char* getClassName() const { return "OPrototype"; }

    /**
     * Compare le type de cet objet avec un nom de classe donne.
     *
     * @param   className Le nom de la classe a comparer.
     *
     * @return  Retourne <code>true</code> si cet objet est une instance de la
     *          classe specifiee, si non <code>false</code>.
     */
    bool isA(const char* className) const;

    /**
     * Effectue un cast sur l'objet passe si cela est possible, sinon
     * retourne NULL. Cette methode statique est reimplementee dans toutes
     * les classes derivees a OPrototype par la macro OPROTODECL.
     *
     * @param   object L'instance a caster.
     *
     * @return  L'instance castee dans le bon type ou NULL.
     */
    static OPrototype* safeDownCast(OPrototype* pObject);

    /**
     * Defines an interface for the Oprototype factory
     *
     * make method must return a unique pointer to a OPrototype object
     *
     */
    class IOProtoFactory
    {
    public:
        typedef std::unique_ptr<IOProtoFactory> ptr_type;
        virtual std::unique_ptr<OPrototype> make() = 0;
    };

    /**
     * Template implementation of the IOProtoFactory interface
     *
     * make method creates a T object and returns a unique pointer to it
     *
     * CAUTION: T must inherit from OPrototype
     *
     */
    template<typename T>
    class Factory : public IOProtoFactory
    {
    public:
        typedef std::unique_ptr<T> ptr_type;

        static_assert(std::is_base_of<OPrototype, T>::value,
                      "Factory<T> : T must inherit from OPrototype");

        ptr_type typed_make () { return ptr_type(new T()); }
        virtual std::unique_ptr<OPrototype> make () { return typed_make(); }
    };

    /**
     * Adds the factory "factory" allowing to build the class named "classname"
     */
    static void add_factory(const char*, IOProtoFactory::ptr_type factory);

protected:

    /**
     * Constructeur par defaut.
     */
    OPrototype();

// Membres
private:

    /**
     * maps a class name (key) to the corresponding factory (value) that can
     * build it through its "make()" method
     */
    static std::unordered_map<std::string, IOProtoFactory::ptr_type> _factory_map;

};

/**
 * Template class method allowing to build a factory for a T class
 *
 * @return a unique ptr on the build factory
 *
 * CAUTION: T must inherit from OPrototype
 */
template<typename T>
std::unique_ptr<OPrototype::Factory<T> > build_factory(){return std::unique_ptr<OPrototype::Factory<T> >( new OPrototype::Factory<T>() ); }

typedef std::vector<LPTYElement> LPTYElementArray;

typedef std::map<TYUUID, TYElement*> TYElementContainer;

/**
 * Macro pour la declaration des methodes d'extension
 * pour l'IHM et l'Impression.
 */
#if TY_USE_IHM

#if defined(WIN32)
#define TY_EXTENSION_DECL(classname) \
    friend class classname; \
    public: \
    virtual TYEditWidget* getEditWidget() { \
        return new classname##Widget(this); \
    }
#else
#define TY_EXTENSION_DECL(classname) \
    public: \
    virtual TYEditWidget* getEditWidget() { \
        return new classname##Widget(this); \
    }
#endif

#define TY_EXTENSION_DECL_ONLY(classname) \
    public: \
    virtual TYEditWidget* getEditWidget() ;

#define TY_EXTENSION_INST(classname) \
    TYEditWidget* classname::getEditWidget() {  \
        return new classname##Widget(this); \
    }

#else // TY_USE_IHM

#define TY_EXTENSION_DECL(classname)
#define TY_EXTENSION_DECL_ONLY(classname)
#define TY_EXTENSION_INST(classname)
#endif // TY_USE_IHM


/**
 * Macro pour la declaration des methodes d'extension pour le graphique
 */
#if TY_USE_IHM

#if defined(WIN32)
#define TY_EXT_GRAPHIC_DECL(classname) \
    friend class classname; \
    public: \
    virtual LPTYElementGraphic getGraphicObject() { \
        if (!_pGraphicObject)   _pGraphicObject = new classname##Graphic(this); \
        return _pGraphicObject; \
    }
#else
#define TY_EXT_GRAPHIC_DECL(classname) \
    public: \
    virtual LPTYElementGraphic getGraphicObject() { \
        if (!_pGraphicObject)   _pGraphicObject = new classname##Graphic(this); \
        return _pGraphicObject; \
    }
#endif

#define TY_EXT_GRAPHIC_DECL_ONLY(classname) \
    public: \
    virtual LPTYElementGraphic getGraphicObject();

#define TY_EXT_GRAPHIC_INST(classname) \
    LPTYElementGraphic classname::getGraphicObject() { \
        if (!_pGraphicObject)   _pGraphicObject = new classname##Graphic(this); \
        return _pGraphicObject; \
    }

#else

#define TY_EXT_GRAPHIC_DECL(classname)
#define TY_EXT_GRAPHIC_DECL_ONLY(classname)
#define TY_EXT_GRAPHIC_INST(classname)
typedef void*   LPTYElementGraphic;
#endif // TY_USE_IHM

/**
 * Structure contenant un identifiant unique pour chaque TYElement.
 */
typedef class OGenID TYUUID;


template<typename T>
T *downcast(TYElement * elt)
{
    return T::safeDownCast(elt);
}


/**
 * Classe de base des composants et elements metiers.
 */
class TYElement : public IRefCount, public OPrototype
{
    OPROTODECL(TYElement)

    // Methodes
public:
    /**
     * @name Construction et Destruction
     */
    //@{
    TYElement();

    /**
     * Constructeur par defaut.
     * Initialise un nouvel element metier.
     * L'element parent permet de parcourir l'arborescence dans
     * les elements metiers.
     * @param PutInInstanceList : booleen indiquant si le TYElement cree utilisera la fonctionalite TYElement::getInstance(TYUUID uuid)
     */
    TYElement(TYElement* pParent, bool PutInInstanceList = true);

    /**
     * Constructeur par copie.
     * @param PutInInstanceList : booleen indiquant si le TYElement cree utilisera la fonctionalite TYElement::getInstance(TYUUID uuid)
     */
    TYElement(const TYElement& other, bool PutInInstanceList = true);

    /**
     * Destructeur.
     */
    virtual ~TYElement();

    //@}

    /**
     * @name Interface XML
     */
    //@{

    /**
     * Construit une arborescence DOM a partir du noeud passe
     * afin de sauvegarder les parametres au format XML.
     *
     * @param domElement Le noeud DOM parent a partir duquel l'arborescence doit etre creee.
     *
     * @return  Le noeud DOM nouvellement cree.
     */
    virtual DOM_Element toXML(DOM_Element& domElement) /* const */;
    /**
     * Extrait les informations a partir du noeud DOM passe afin de
     * recuperer les parametres de la classe.
     *
     * @param domElement Le noeud DOM a partir duquel les parametres doivent etre recuperes.
     *
     * @return 1 si la recuperation des parametres a reussie.
     */
    virtual int fromXML(DOM_Element domElement);

    //@}

    /**
     * @name Gestion du Calcul courant
     */
    //@{

    /**
     * Verifie si cet element est present dans la liste d'ID passee.
     * S'il est present cela signifie qu'il est actif dans le
     * Calcul courant, c'est cet etat qui est retourne.
     *
     * @param listID La liste d'ID du Calcul courant.
     * @param recursif Indique si la mise a jour du Calcul courant doit etre recursive.
     *
     * @return <code>true</code> si cet objet est actif dans le Calcul concerne;
     *         <code>false</code> sinon.
     */
    virtual void updateCurrentCalcul(TYListID& listID, bool recursif = true);

    /**
     * Collecte les enfants de cet element de facon recursive ou non.
     */
    virtual void getChilds(LPTYElementArray& childs, bool recursif = true) {}

    /**
     * Set/Get de l'indicateur d'appartenance au Calcul courant.
     */
    virtual void setInCurrentCalcul(bool state, bool recurschild = true,  bool recursparent = true);
    /**
     * Set/Get de l'indicateur d'appartenance au Calcul courant.
     */
    virtual bool isInCurrentCalcul() { return _inCurrentCalcul; }

    /**
     * @brief Increment le compteur de nombre de copies
     */
    void addCopy()  { _copyCount++; }

    /**
     * @brief Modifie la vaeur du compteur
     */
    void setCopyCount(const unsigned int copyCount) { _copyCount = copyCount; }

private:

    /**
     * Called by children to inform parents that they change their "in calcul" state.
     */
    void OnChildInCalculStatusChange();

    //@}

public:

#if TY_USE_IHM

    /**
     * @name Interface Graphique
     */
    //@{

    /**
     * Retourne l'objet graphique metier associe a cet element.
     * Cette fonction est creee pour TYElement, uniquement pas securite
     * Elle ne devrait jamais etre appelee
     */
    virtual LPTYElementGraphic getGraphicObject() { return NULL; }

    /**
     * Affiche ou non l'object graphique et le met a jour.
     */
    void drawGraphic(bool draw = true);

    /**
     * Mets a jour l'objet graphique associe a cet element.
     */
    virtual void updateGraphic();

    /**
     * Mets a jour le graphe de scene.
     */
    virtual void updateGraphicTree();

    //@}

    /**
     * @name Interface IHM
     */
    //@{

    /**
     * Retourne un nouveau widget IHM pour l'edition des parametres de
     * cet element.
     *
     * @return Un widget IHM pour l'edition des parametres de cet element.
     *
     * @see TYElement#edit()
     */
    virtual TYEditWidget* getEditWidget();

    /**
     * Affiche une boite de dialogue pour l'edition des parametres de cet
     * element.
     * Cette methode utilise le widget retourne par getEditWidget().
     *
     * @param pParent Un pointeur sur le widget parent.
     *
     * @see getEditWidget()
     * @see TYMetierWidget#edit()
     */
    int edit(TYEditWidget* pParent = NULL);

    //@}
#endif

    /**
     * @name Accesseurs
     */
    //@{

    /**
     * Set/Get de l'identifiant de l'element.
     */
    void setID(TYUUID id);
    void setID(const QString& str_id);

    /**
     * Set/Get de l'identifiant de l'element.
     */
    const TYUUID& getID() const;
    /**
     * Get de l'identifiant de l'element sous forme de string.
     */
    QString getStringID() const { return getID().toString();}

    bool hasNullID() const { return _uuid.isNull(); }

    /**
     * Genere un nouvel Id pour l'element (utilise lors de l'import d'un element dans un projet)
     */
    void regenerateID();

    /**
     * Set/Get du nom de l'element.
     */
    void setName(QString name) { _name = name; }
    /**
     * Set/Get du nom de l'element.
     */
    QString getName() const { return _name; }

    /**
     * Set/Get de la reference sur le parent de cet element.
     */
    void setParent(TYElement* pParent) { _pParent = pParent; }
    /**
     * Set/Get de la reference sur le parent de cet element.
     */
    TYElement* getParent() const { return _pParent; }

    //@}

    /**
     * @name Indicateurs de modification.
     */
    //@{

    /**
     * Set/Get de l'indicateur de modification acoustique.
     */
    virtual void setIsAcousticModified(bool isModified);
    /**
     * Set/Get de l'indicateur de modification acoustique.
     */
    bool getIsAcousticModified() { return _isAcousticModified; }

    /**
     * Set/Get de l'indicateur de modification de la geometrie.
     */
    virtual void setIsGeometryModified(bool isModified);

    /**
     * Set/Get de l'indicateur de modification de la geometrie.
     */
    bool getIsGeometryModified() { return _isGeometryModified; }

    //@}

    /**
     * @name Operateurs
     */
    //@{

    /**
     * Operateur de copie.
     */
    TYElement& operator=(const TYElement& other);

    /**
     * Operateur de comparaison.
     */
    bool operator==(const TYElement& other) const;

    /**
     * Operateur de comparaison.
     */
    bool operator!=(const TYElement& other) const;

    /**
     * Effectue une copie en allouant de la memoire (deep copy) et
     * non pas en copiant seulement les pointeurs (shallow copy).
     * Cette methode s'utilise comme l'operateur egal.
     * L'ID peut ne pas etre copie, le parent n'est jamais copie.
     *
     * @param pOther L'element a copier.
     * @param copyId Copie l'identifiant unique ou pas.
     *
     * @return <code>true</code> si la copie est possible;
     *         <code>false</code> sinon.
     */
    virtual bool deepCopy(const TYElement* pOther, bool copyId = true);

    //@}

    /**
     * @name Methodes Utilitaires
     */
    //@{



    /**
     * Retourne une chaine de caractere donnant des informations
     * sur cet objet (utile pour le debugage).
     *
     * @return Une chaine de caractere informative sur cet objet.
     */
    virtual std::string toString() const { return "TYElement"; };

    /**
     * Teste si l'element porte le meme nom que celui de la classe (sans le prefixe TY).
     * Si tel est le cas, la methode 'fromXML' est appellee et <code>true</code> est
     * retourne.
     *
     * @param domElement Le noeud XML a tester.
     * @param pRetVal Adresse d'un 'int' pour la valeur de retour de 'fromXML'.
     *
     * @return <code>true</code> si l'element porte le meme nom que la classe.
     */
    bool callFromXMLIfEqual(DOM_Element& domElement, int* pRetVal = NULL);

    /**
     * Retourne le nom de la donnee metier associee a cette classe.
     * Ce nom est utilise pour le nom des tables en BDD ainsi que
     * pour le nom des noeuds XML.
     *
     * @return Le nom de la donnee metier.
     */
    std::string getMetierName();

    //@}

    /**
     * Recherche un type particuler d'element ou un type derive dans un noeud XML
     * et instancie un element du type trouve. Si des elements sont trouves, leur methode
     * 'fromXML' est appelee.
     *
     * @param parentElem    L'element XML parent.
     * @param type          Le nom du type a chercher (ou un type derive).
     *
     * @return Une instance du type trouve, sinon NULL.
     */
    static LPTYElementArray findTypeCollectionAndCallFromXML(DOM_Element parentElem, const char* type);

    /**
     * Active ou desactive l'enregistrement de toutes les instances creees de type
     * TYElement ou derive.
     *
     * @param log Active ou desactive.
     */
    static void setLogInstances(bool log) { _logInstances = log; }
    /**
     * Retourne l'etat de l'indicateur d'enregistrement des instances de type
     * TYElement ou derive.
     *
     * @return L'etat de l'enregistrement.
     */
    static bool getLogInstances() { return _logInstances; }

    /**
     * Recherche une instance particuliere de type TYElement ou derive
     * a partir de son ID.
     *
     * @param id L'ID de l'element a rechercher.
     *
     * @return L'instance ayant l'ID correspondant, sinon NULL.
     */
    static TYElement* getInstance(TYUUID uuid);

    /**
     * Vide le tableau de l'enregistrement des instances.
     */
    static void purgeInstances();

    /**
     * Verification de l'existence d'un doublon dans la liste des instances
     */
    static bool testId(const TYUUID& id, const TYElement* pElem);


    /**
     * Retourne un nouvel identifiant unique.
     *
     * @return Un nouvel identifiant unique.
     */
    static TYUUID newID();

    /**
     * conversion d'une string en identifiant 128 bits.
     *
     */
    static TYUUID fromString(QString id);

    /**
     * conversion d'un identifiant 128 bits en string.
     *
     */
    static QString toString(TYUUID& uuid);

    /**
     * Renvoie le nombre d'appels aux constructeurs d'objets metiers.
     * (utilise pour faire des tests de performance)
     */
    static uint64 getConstructorCount();

    /**
     * Renvoie le nombre d'appels aux destructeurs d'objets metiers.
     * (utilise pour faire des tests de performance)
     */
    static uint64 getDestructorCount();

    /**
     * Renvoie le nombre de (re)generation of UUID
     * (utilise pour faire des tests de performance)
     */
    static uint64 getIdGenerationCount();

    /**
     * Get/Set de l'etat de l'indicateur de sauvegarde
     */
    static bool getIsSavedOk() { return _toSave; }

    /**
     * Get/Set de l'etat de l'indicateur de sauvegarde
     */
    static void setIsSavedOk(const bool& toSave) { _toSave = toSave; }

    /**
     * Get/Set de l'etat de regerneration d'ID
     */
    static void setRegenerateID(const bool& bRegenerate) { _bRegenerateID = bRegenerate; }
    static bool getRegenerateID() { return _bRegenerateID; }



private:
    /**
     * Ajoute l'instance this au tableau d'enregistrement des instances
     *
     */
    void addInstance();

    /**
     * Supprime une instance au tableau d'enregistrement des instances
     *
     */
    void remInstance();

    // Membres
private:
    ///Identifiant unique de l'element.
    mutable TYUUID _uuid;

protected:
    ///Nom courant de l'element.
    QString _name;

    ///Reference sur l'element parent.
    TYElement* _pParent;

    //Pour eviter de grossir la liste d'instance avec des objets temporaires:
    bool _bPutInInstanceList;

    // Decompte du nombre de copies
    unsigned int _copyCount;

    ///Indique si cet element est actif dans le Calcul courant.
    bool _inCurrentCalcul;

    ///Indicateur de modification acoustique.
    bool _isAcousticModified;

    ///Indicateur de modification de la geometrie.
    bool _isGeometryModified;

    ///L'object graphique metier associe a cet element.
    LPTYElementGraphic _pGraphicObject;

private:
    ///Indique si on souhaite registrer toutes les instances de type TYElement et derivees.
    static bool _logInstances;
    ///Collection de toutes les instances de type TYElement et derivees.
    //XXX This should be a map (of weak pointers) not a list !
    static TYElementContainer* _instances;
    static TYElementContainer& getInstances();
    /// Indicateur de modification. Passe a true si un TYElement a ete modifie, Indique la necessite de sauvegarder
    static bool _toSave;
    /// Indicateur de regeneration d'ID true si regeneration d'ID a la lecture d'un fichier XML
    static bool _bRegenerateID;

    static uint64 ty_created_counter;
    static uint64 ty_destroyed_counter;
    static uint64 ty_regen_id_counter;
};

// Some simple auxilliary fonction to help debugging and reporting

inline const char* str_qt2c(const QString& qstr)
{ return qstr.toLocal8Bit().data(); } // TODO Clarifiy consistency wrt Local8Bit vs Utf8

inline QString xml2qstring(const QDomNode& node)
{
    QString msg; QTextStream str(&msg);
    node.save(str, 0);
    return msg;
}

inline  const char* xml2cstring(const QDomNode& node)
{ return str_qt2c(xml2qstring(node)); }

#ifndef _NDEBUG
inline QDebug _debugXml(const char* expr, const QDomNode& node)
{ return qDebug() << expr << xml2qstring((node)); }

#define debugXml(expr) _debugXml(#expr, (expr))
#else // _NDEBUG
#define debugXml(expr) ((void)0)
#endif // _NDEBUG

#endif // __TY_ELEMENT__
