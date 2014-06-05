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

#ifndef __O_PROTOTYPE__
#define __O_PROTOTYPE__

#include <memory>
#include <unordered_map>
#include <iostream>
#include <boost/exception/error_info.hpp>

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

/**
 * Macro pour la declaration des methodes a surcharger
 * de la classe OPrototype, et pour la registration du type
 * 'classname' dans la Prototype Factory.
 * Cette version permet d'utiliser la methode inherits().
 * Cette macro doit etre appelee dans la declaration de la
 * nouvelle classe derivee (.h).
 */
#define OPROTOSUPERDECL(classname, superclassname) \
    OPROTODECL(classname) \
    typedef superclassname Superclass; \
    public: \
    virtual const char* getSuperClassName() const { return #superclassname; } \
    static bool isTypeOf(const char* className) { \
        if (!strcmp(className, #classname)) { \
            return true; \
        } \
        return superclassname::isTypeOf(className); \
    } \
    virtual bool inherits(const char* className) const { \
        return classname::isTypeOf(className); \
    }

//
/////////////////////////////////////////////////////////////

namespace tympan
{
     typedef boost::error_info < struct tag_classname, std::string >
         oproto_classname_errinfo;
} // namespace tympan


///Nombre maximum de prototypes.
static const int PROTOTYPE_MAX_NB = 256;


/**
 * Classe abstraite Prototype du pattern Prototype Factory.
 * Les classes derivees de Prototype peuvent etre instanciees
 * a partir seulement du nom de la classe (chaine de caracteres).
 *
 * Pour cela, la map statique _factory_map associe a une chaine de caracteres
 * representant le nom de la classe la factory permettant de construire une
 * instance de ce type
 * (ATTENTION: pas de clonage : appel systematique au constructeur par defaut)
 *
 * La methode public static 'findAndClone()' est l'interface principale
 * de ce mecanisme, elle permet d'instancier un nouvel objet a partir
 * de son nom (a condition que le type correspondant existe et soit
 * deja registre dans Prototype au moment de l'appel). Si le nom n'est pas connu
 * au moment de l'appel, elle leve une exception tympan::invalid_data.
 *
 * La methode 'findPrototype()' cherche si une classe est enregistree aupres
 * de OPrototype.
 *
 * Enfin, la methode virtuelle pure 'clone()' permet aux classes derivees de
 * retourner une nouvelle instance du type derive correspondant.
 *
 * De plus, les methodes 'isA()' et 'inherits()' peuvent s'averer
 * tres utiles, la premiere permet de verifier le type d'une instance,
 * tandis que la deuxieme permet de verifier si une instance herite
 * d'un type particulier.
 *
 * L'exemple suivant presente le squelette a reprendre pour creer
 * des classes derivees de Prototype, c'est-a-dire de nouveaux
 * composants. La classe derivee pour cet exemple se nomme
 * 'ConcretePrototype'.
 *
 * <pre>
 * // Declaration (ConcretePrototype.h)
 *
 * class ConcretePrototype : public Prototype {
 *
 *    // La methode suivante doit etre amie pour acceder aux membres prives.
 *    friend registerConcretePrototype();
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
 * La macro OPROTOSUPERDECL est une extension de OPROTODECL, elle permet
 * d'utiliser la methode 'inherits()' grce au nom de la classe heritee :
 *
 * <pre>
 *
 * #define  OPROTOSUPERDECL(classname, superclassname) \
 *  OPROTODECL(classname) \
 *  public: \
 *      virtual const char* getSuperClassName() const { return #superclassname; }
 *
 * </pre>
 *
 * La declaration d'une nouvelle classe s'en trouve alors simplifiee.
 * L'exemple precedent devient donc :
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
 * Cette technique d'enregistrement n'est pas autonome puisqu'il est
 * necessaire de reprendre le modele ci-dessus pour chaque nouvelle
 * classe derivee de Prototype. Par contre les macros offrent un
 * moyen alternatif simple pour la creation de ce type de classe.
 *
 *
 * ATTENTION: la map doit etre remplie avant le lancement de l'application, au
 * moyen de la methode statique 'add_factory()', qui permet d'enregistrer les
 * differentes classes ainsi que leurs fabriques (utiliser la fonction
 * 'build_factory<TYTopographie>()'.
 * Exemple d'utilisation:
 *      OPrototype::add_factory("TYTopographie", move(build_factory<TYTopographie>()));
 */

#include <type_traits>

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
     * Retourne le type de la classe surchargee sous la forme d'une
     * chaine de caractere.
     *
     * @return Le nom de la classe surchargee.
     */
    virtual const char* getSuperClassName() const { return 0; }

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
     * Permet de tester si le type de cette instance est derivee
     * d'un type particulier.
     * Le type de cette classe est consideree comme un type derive.
     *
     * @param   className Le nom de la classe a comparer.
     *
     * @return  Retourne <code>true</code> si le type cet objet est
     *          derivee de la classe specifiee, si non <code>false</code>.
     */
    virtual bool inherits(const char* className) const;


    /**
     * Compare le type de cet classe (et celui de ses classes derivees) avec
     * un nom de classe donne.
     *
     * @param   className Le nom de la classe a comparer.
     *
     * @return  Retourne <code>true</code> si cette classe est du type de la
     *          classe specifiee, si non <code>false</code>.
     */
    static bool isTypeOf(const char* className);

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

    /**
     * Ajoute un type derive de OPrototype au tableau des
     * classes registrees.
     *
     * @param pProto Une instance de la classe a registrer.
     *
     * @return Le nombre de prototype registres ou -1 si une
     *         erreur est survenue.
     */
    static int registerPrototype(OPrototype* pProto);

// Membres
private:

    ///Tableau des prototypes registres.
    static OPrototype*  _prototypes[PROTOTYPE_MAX_NB];

    ///Nombre de prototypes registres.
    static int          _nbPrototypes;

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




#endif // __O_PROTOTYPE__
