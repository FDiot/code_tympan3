/* WARNING: I am an auto-generated header file, don't modify me !! */
/* Modify Tympan/models/common/3d.h instead */
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

#ifndef TY_MC_3D
#define TY_MC_3D

/**
 * \file 3d.h
 * \brief All base classes related to 3D manipulation
 */

#include <cmath>
#include <math.h>
#include <vector>
#include <ostream>

// Defines pour les calculs d'intersection
///Les elements sont confondus.
#define INTERS_CONFONDU 2
///L'intersection existe.
#define INTERS_OUI      1
///Pas d'intersection.
#define INTERS_NULLE    0

#define EPSILON_3  0.001            // 10e-3
#define EPSILON_5  0.00001          // 10e-5
#define EPSILON_6  0.000001         // 10e-6
#define EPSILON_7  0.0000001        // 10e-7
#define EPSILON_13 0.0000000000001  // 10e-13
#define EPSILON_50 1.e-50

/* From this threshold (in meters), two elements are considered as at the same
 * position
 * */
#define TYSEUILCONFONDUS EPSILON_3  // 1 mm

#ifndef M_PI
///Pi.
#define M_PI        3.1415926535897932384626433832795
#endif

///2Pi.
#define M_2PI       6.283185307179586476925287766559


////////////////////////////////////////////////////////////////////////////////
// Macros
////////////////////////////////////////////////////////////////////////////////

/**
 * \fn inline double ABS(double a)
 * \brief Retourne la valeur absolue.
 * \param a Un nombre.
 * \return Le valeur absolue.
 */
inline double ABS(double a)
{
    return (a >= 0.0 ? a : -a);
}

/**
 * \fn inline double SIGNE(double a)
 * \brief Retourne le signe d'un nombre
 * \param a Un nombre.
 * \return Le signe (1 ou -1).
 */
inline double SIGNE(double a)
{
    return (a >= 0.0 ? 1 : -1);
}

/**
 * \fn inline double MAX(double a, double b)
 * \brief Retourne le plus grand nombre parmi 2
 * \param a Le 1er nombre a comparer.
 * \param b Le 2eme nombre a comparer.
 * \return Le plus grand nombre entre a et b.
 */
inline double MAX(double a, double b)
{
    return (a > b ? a : b);
}

/**
 * \fn inline double MAX3(double a, double b, double c)
 * \brief Retourne le plus grand nombre.
 * \param a Le 1er nombre a comparer.
 * \param b Le 2eme nombre a comparer.
 * \param c Le 3eme nombre a comparer.
 * \return Le plus grand nombre entre a, b et c.
 */
inline double MAX3(double a, double b, double c)
{
    return MAX(MAX(a, b), c);
}

/**
 * \fn inline double MIN(double a, double b)
 * \brief Retourne le plus petit nombre parmi 2.
 * \param a Le 1er nombre a comparer.
 * \param b Le 2eme nombre a comparer.
 * \return Le plus petit nombre entre a et b.
 */
inline double MIN(double a, double b)
{
    return (a < b ? a : b);
}

/**
 * \fn inline double DEGTORAD(double a)
 * \brief Convertit un angle en degres en un angle en radians.
 * \param a Un angle en degres.
 * \return Un angle en radians.
 */
inline double DEGTORAD(double a)
{
    return (a / 180.0 * M_PI);
}

/**
 * \fn inline double RADTODEG(double a)
 * \brief Convertit un angle en radians en un angle en degres.
 * \param a Un angle en radians.
 * \return Un angle en degres.
 */
inline double RADTODEG(double a)
{
    return (a / M_PI * 180.0);
}

/**
 * \fn inline double GRATORAD(double a)
 * \brief Convertit un angle en gradians en un angle en radians.
 * \param a Un angle en gradians.
 * \return Un angle en radians.
 */
inline double GRATORAD(double a)
{
    return (a / 200.0 * M_PI);
}

/**
 * \fn inline double RADTOGRA(double a)
 * \brief Convertit un angle en radians en un angle en gradians.
 * \param a Un angle en radians.
 * \return Un angle en gradians.
 */
inline double RADTOGRA(double a)
{
    return (a / M_PI * 200.0);
}

/**
 * \fn inline double GRATODEG(double a)
 * \brief Convertit un angle en gradians en un angle en degres.
 * \param a Un angle en gradians.
 * \return Un angle en degres.
 */
inline double GRATODEG(double a)
{
    return (a / 200.0 * 180.0);
}

/**
 * \fn inline double DEGTOGRA(double a)
 * \brief Convertit un angle en degres en un angle en gradians.
 * \param a Un angle en degres.
 * \return Un angle en gradians.
 */
inline double DEGTOGRA(double a)
{
    return (a / 180.0 * 200.0);
}

/**
 * \fn inline int ROUND(double a)
 * \brief calcul de la valeur arrondie d'un nombre.
 * \param a Un nombre.
 * \return La valeur arrondie.
 */
inline int ROUND(double a)
{
    return (a >= 0.0 ? (int)(a + 0.49999) : (int)(a - 0.49999));
}

/**
 * \fn inline double BORNE(double a, double b, double c)
 * \brief Borne un nombre.
 * \param a Un nombre.
 * \param b La borne inferieur.
 * \param c La borne superieur.
 * \return Le nombre ou une des bornes.
 */
inline double BORNE(double a, double b, double c)
{
    return (a < b ? b : (a > c ? c : a));
}

/**
 * \fn inline int BORNE(int a)
 * \brief Borne un nombre entre 0 et 255.
 * \param a Un nombre.
 * \return Le nombre ou une des bornes.
 */
inline int BORNE(int a)
{
    return (int) BORNE((double) a, 0, 255);
}


/**
 * \class OCoord3D
 * \brief The 3D coordinate class.
 */
class OCoord3D
{
public:
    OCoord3D();
    OCoord3D(const OCoord3D& coord);
    OCoord3D(double x, double y, double z);

    virtual ~OCoord3D();

    OCoord3D& operator=(const OCoord3D& coord);
    bool operator==(const OCoord3D& coord) const;
    bool operator!=(const OCoord3D& coord) const;

    /**
     * \fn  void setCoords(double, double, double)
     * \brief Sets the coordinates as an array of double.
     */
    void setCoords(double x, double y, double z);
    /**
     * \fn  void setCoords(double coords[3]);
     * \brief Sets the coordinates as an array of double.
     */
    void setCoords(double coords[3]);
    /**
     * \fn void getCoords(double coords[3]);
     * \brief Gets the coordinates as an array of double.
     */
    void getCoords(double coords[3]);
    /**
     * \fn double* getCoords();
     * \brief Gets the coordinates as an array of double.
     */
    double* getCoords();

    /**
     * cast operator to get coordinates via a table.
     */
    operator const double* () const { return &_value[0]; }
    /**
     * cast operator to get coordinates via a table.
     */
    operator double* () { return &_value[0]; }

public:
    union
    {
        struct
        {
            double _x;
            double _y;
            double _z;
        };
        double _value[3];
    };
};

::std::ostream& operator<<(::std::ostream& os, const OCoord3D& c);


/**
 * \class OVector3D
 * \brief The 3D vector class.
 */

class OVector3D: public OCoord3D
{
public:
    OVector3D();
    OVector3D(const OVector3D& vector);
    /**
     * \fn OVector3D(const OCoord3D& coord);
     * \brief Constructs a new vector from a 3D coordinate. Works like the copy constructor.
     * \param coord A 3D coordinate.
     */
    OVector3D(const OCoord3D& coord);
    /**
     * \fn OVector3D(const OCoord3D& coordFrom, const OCoord3D& coordTo);
     * \brief Constructs a new vector from 2 coordinates.
     *
     * \param coordFrom The 1st coordinate.
     * \param coordTo The 2nd coordinate.
     */
    OVector3D(const OCoord3D& coordFrom, const OCoord3D& coordTo);
    /**
     * \fn OVector3D(double x, double y, double z);
     * \brief Constructs a new vector from 3 doubles.
     *
     * \param x X coordinate.
     * \param y Y coordinate.
     * \param z Z coordinate.
     */
    OVector3D(double x, double y, double z);

    virtual ~OVector3D();

    /**
     * \fn void reset();
     * \brief Reset this vector.
     */
    void reset();

    OVector3D& operator=(const OVector3D& vector);
    bool operator==(const OVector3D& vector) const;
    bool operator!=(const OVector3D& vector) const;
    OVector3D operator+(const OVector3D& vector) const;
    OVector3D operator-(const OVector3D& vector) const;
    // XXX meaningless componentwise multiplication whereas a dot product would have been expected.
    OVector3D operator*(const OVector3D& vector) const;
    /**
     * \fn OVector3D operator*(const double a) const;
     * \brief Multiplication with a scalar (commutative).
     *
     * \param a A scalar value.
     */
    OVector3D operator*(const double a) const;
    /**
     * \fn friend OVector3D operator*(const double a, const OVector3D& vector);
     * \brief Multiplication with a scalar (commutative).
     *
     * \param a A scalar value.
     */
    friend OVector3D operator*(const double a, const OVector3D& vector);

    /**
     * \brief dot product (assuming an orthonormal reference frame)
     */
    double dot(const OVector3D& v)
    { return _x * v._x + _y * v._y + _z * v._z; }

    /**
     * \fn OVector3D cross(const OVector3D& vector) const;
     * \brief Cross product.
     *
     * \param vector The object reference with which this object is multiplicated.
     *
     * \return The result of the cross product.
     */
    OVector3D cross(const OVector3D& vector) const;

    /**
     * \fn void balance(double c1, const OVector3D& vector2, double c2);
     * \brief Balances this vector.
     *
     * \param c1 The balance value for this vector.
     * \param vector2 The other member for the balance.
     * \param c2 The balance value for the second vector.
     */
    void balance(double c1, const OVector3D& vector2, double c2);

    /**
     * \fn double scalar(const OVector3D& vector) const;
     * \brief Performs the scalar product between this object and another vector.
     *
     * \param vector The other member for the scalar product.
     *
     * \return The result of the scalar product.
     */
    double scalar(const OVector3D& vector) const;

    /**
     * \fn double norme() const;
     * \brief Computes the length of this vector.
     *
     * \return The length of this vector.
     */
    double norme() const;

    /**
     * \fn OVector3D normal(const OVector3D& vector2, const OVector3D& vector3) const;
     * \brief Computes the normal with this vector and 2 others.
     *
     * @return The normal.
     */
    OVector3D normal(const OVector3D& vector2, const OVector3D& vector3) const;

    /**
     * \fn void normalize();
     * \brief Normalizes this vector.
     */
    void normalize();

    /**
     * \fn void invert();
     * \brief Inverts this vector.
     */
    void invert();

    /**
     * \fn double angle(const OVector3D& vector) const;
     * \brief Computes the angle between this vector and another vector.
     *
     * \param vector The vector to mesure the angle with this vector.
     */
    double angle(const OVector3D& vector) const;

    /**
    * \fn OVector3D getRotationOz(double alpha, OVector3D V)
    * \brief Returns the vector after a rotation around z axis
    * \ x -> xprime. Both of these vectors will be given in the
    * \ original base.
    */
    OVector3D getRotationOz(double alpha);

    /**
    * \fn OVector3D getRotationOzBase2(double alpha, OVector3D V)
    * \brief Returns the vector after a rotation around z axis
    * \ and gives back the coordinates of xprime or yprime in the
    *\ new basis Bprime.
    */
    OVector3D getRotationOzBase2(double alpha);

    /**
    * \fn OVector3D getRotationOy(double alpha, OVector3D V)
    * \brief Returns the vector after a rotation around z axis
    */
    OVector3D getRotationOy(double alpha);

    /**
    * \fn OVector3D getRotationOyBase2(double alpha, OVector3D V)
    * \brief Returns the vector after a rotation around y axis
    * \ and gives back the coordinates of xprime or zprime in the
    *\ new basis Bprime.
    */
    OVector3D getRotationOyBase2(double alpha);

    /**
    * \fn OVector3D getRotationOzOy(double alpha, double theta, OVector3D V)
    * \brief Returns the vector after 2 rotations around z axis and
    * \y axis. It gives the coordinates of xsecond, ysecond and zsecond in the
    *\ first basis B.
    */
    OVector3D getRotationOzOy(double alpha, double theta);
};

inline OCoord3D operator + (const OCoord3D& coord, const OVector3D& vect)
{
    return OCoord3D(coord._x + vect._x, coord._y + vect._y, coord._z + vect._z);
}

::std::ostream& operator<<(::std::ostream& os, const OVector3D& v);


/**
 * \class OPoint3D
 * \brief The 3D point class.
 */
class OPoint3D;
typedef std::vector<OPoint3D> TabPoint3D;

class OPoint3D: public OCoord3D
{
    // Methods
public:
    /**
     * \brief Default constructor.
     */
    OPoint3D();

    /**
     * \brief Copy constructor.
     *
     * \param pt The object to be copied.
     */
    OPoint3D(const OPoint3D& pt);

    /**
     * \brief Constructor from a 3D coord.
     *
     * \param coord The object to be copied.
     */
    OPoint3D(const OCoord3D& coord);

    /**
     * \fn OPoint3D(double x, double y, double z);
     * \brief Constructs a new object from 3 doubles.
     *
     * \param x X coordinate.
     * \param y Y coordinate.
     * \param z Z coordinate.
     */
    OPoint3D(double x, double y, double z);

    /**
     * \fn OPoint3D(double v[]);
     * \brief Constructs a new object from 3 doubles.
     *
     * \param v table of coordinates.
     */
    OPoint3D(double v[]);

    /**
     * \brief Destructor.
     */
    virtual ~OPoint3D();

     /**
     * Assigne des valeurs a l'objet.
     *
     * @param x X coordinate.
     * @param y Y coordinate.
     * @param z Z coordinate.
     */
    virtual void set(double x, double y, double z);

    /**
     * Compatibilite avec OGL.
     * Effectue le changement de repere OGL vers Tympan.
     */
    virtual void setFromOGL(float x, float y, float z);
    /**
     * Compatibilite avec OGL.
     * Effectue le changement de repere OGL vers Tympan.
     */
    virtual void setFromOGL(float coords[3]);
    /**
     * Compatibilite avec OGL.
     * Effectue le changement de repere OGL vers Tympan.
     */
    virtual void setFromOGL(double coords[3])
    {
        setFromOGL(coords[0], coords[1], coords[2]);
    }
    /**
     * Compatibilite avec OGL
     * Effectue le changement de repere Tympan vers OGL.
     */
    virtual void getToOGL(float& x, float& y, float& z);
    /**
     * Compatibilite avec OGL.
     * Effectue le changement de repere Tympan vers OGL.
     */
    virtual void getToOGL(float coords[3]);

   /**
     * \fn double distFrom(const OPoint3D& pt) const;
     * \brief Computes the distance from this point to another.
     *
     * \param pt The point to mesure the distance from.
     */
    double distFrom(const OPoint3D& pt) const;

    /**
     * \fn double dist2DFrom(const OPoint3D& pt) const;
     * \brief Computes the distance from this point to another in 2D plan
     */
    double dist2DFrom(const OPoint3D& pt) const;

    /// Compatibility alias for operator==
    bool isEqual(const OPoint3D& oPoint) const
    {
        return *this == oPoint;
    }

    /**
     * Methode utilitaire qui s'assure que 2 points consecutifs sont
     * espaces d'une distance maximale en ajoutant de nouveaux points
     * quand cela est necessaire.
     *
     * @param points Le tableau de points a tester.
     * @param distanceMax La distance maximale entre 2 points.
     *
     * @return Un nouveau tableau de points espaces d'au plus
     *         la distance maximale.
     */
    static TabPoint3D checkPointsMaxDistance(const TabPoint3D& points,
            const double& distanceMax);

    /**
     * \brief Same as previous function taking two points instead of a tab of points
     */
    static TabPoint3D checkPointsMaxDistance(const OPoint3D& point1,
            const OPoint3D& point2, const double& distanceMax);

};

::std::ostream& operator<<(::std::ostream& os, const OPoint3D& v);


/**
 * \class  OMatrix
 * \brief The 4x4 matrix class.
 */
class OMatrix
{
public:
    OMatrix();
    OMatrix(const OMatrix& matrix);
    OMatrix(double matrix[4][4]);

    virtual ~OMatrix();

    OMatrix& operator=(const OMatrix& matrix);
    bool operator==(const OMatrix& matrix) const;
    bool operator!=(const OMatrix& matrix) const;
    OMatrix operator+(const OMatrix& matrix) const;
    OMatrix operator-(const OMatrix& matrix) const;
    OMatrix operator*(const OMatrix& matrix) const;

    /**
     * \fn OVector3D multNormal(const OVector3D& normal) const;
     * \brief Multiplication with a normal (the translation is removed).
     *
     * \param normal The normal vector with which this object is multiplicated.
     */
    OVector3D multNormal(const OVector3D& normal) const;

    /**
     * \fn void show();
     * \brief Affichage du contenu d'une matrice (debug).
     */
    void show();

    /**
     * \fn void reset();
     * \brief Mise a zero des elements de la matrice.
     *
     *      0   0   0   0
     *      0   0   0   0
     *      0   0   0   0
     *      0   0   0   0
     */
    void reset();

    /**
     * \fn void unite();
     * \brief Initialisation d'une matrice unite.
     *
     *      1   0   0   0
     *      0   1   0   0
     *      0   0   1   0
     *      0   0   0   1
     */
    void unite();

    /**
     * \fn int setTranslation(double x, double y, double z);
     * \brief Mise a jour d'une matrice de translation.
     *
     * Translation T(x, y, z) :
     *
     *      1   0   0   x
     *      0   1   0   y
     *      0   0   1   z
     *      0   0   0   1
     */
    int setTranslation(double x, double y, double z);

    /**
     * \fn int setScale(double x, double y, double z);
     * \brief Mise a jour d'une matrice de zoom.
     *
     * Zoom Z(x, y, z) :
     *
     *      x   0   0   0
     *      0   y   0   0
     *      0   0   z   0
     *      0   0   0   1
     */
    int setScale(double x, double y, double z);

    /**
     * \fn int  setRotationOx(double a);
     * \brief Mise a jour d'une matrice de rotation autour de l'axe Ox.
     *
     *      1   0   0    0
     *      0   cos -sin 0
     *      0   sin cos  0
     *      0   0   0    1
     */
    int setRotationOx(double a);

    /**
     * \fn int  setRotationOy(double a);
     * \brief Mise a jour d'une matrice de rotation autour de l'axe Oy.
     *
     *      cos  0  sin 0
     *      0    1  0   0
     *      -sin 0  cos 0
     *      0    0  0   1
     */
    int setRotationOy(double a);

    /**
     * \fn int  setRotationOz(double a);
     * \brief Mise a jour d'une matrice de rotation autour de l'axe Oz.
     *
     *      cos -sin 0  0
     *      sin cos  0  0
     *      0   0    1  0
     *      0   0    0  1
     */
    int setRotationOz(double a);

    /**
     * \fn int  aligneVecteurSurOx(const OVector3D& vector);
     * \brief Mise a jour d'une matrice d'alignement d'un vecteur quelconque avec l'axe des x.
     *
     * Remarque :
     *  - En raison d'erreur d'arrondi, le fonctionnement de cette
     *    fonction peut etre perturbe dans le cas de vecteur enorme.
     *
     * Principe :
     *  - Rotation de V autour de Oz pour le placer dans le plan xOz
     *      == rotation de -a1 avec a1 = angle entre Ox et la
     *         projection de V dans le plan xOy
     *          sin(a1) = Vy/sqrt(Vx**2+Vy**2)
     *          cos(a1) = Vx/sqrt(Vx**2+Vy**2)
     *      => M1 =
     *          cos1    sin1    0       0
     *          -sin1   cos1    0       0
     *          0       0       1       0
     *          0       0       0       1
     *
     *      => on obtient V1
     *
     *  - Rotation de V1 autour de Oy afin de l'aligner avec Ox
     *      == rotation de -a2 avec a2 = angle entre Ox et V1
     *          sin(a2) = Vz/sqrt(Vx**2+Vy**2+Vz**2)
     *          cos(a2) = sqrt(Vx**2+Vy**2)/sqrt(Vx**2+Vy**2+Vz**2)
     *      => M2 =
     *          cos2    0       sin2    0
     *          0       1       0       0
     *          -sin2   0       cos2    0
     *          0       0       0       1
     *
     *  - Traiter le cas particulier de sqrt(Vx**2+Vy**2) = 0
     *      Cas ou V est // a Oz
     *      == rotation de +-PI/2 autour de Oy
     *          et donc a1 = 0 ety a2 = +-PI/2
     *
     *  - Calcul de la matrice finale :
     *      M = M2 . M1
     *
     */
    int aligneVecteurSurOx(const OVector3D& vector);

    /**
     * \fn int  aligneVecteurSurOy(const OVector3D& vector);
     * \brief Mise a jour d'une matrice d'alignement d'un vecteur quelconque avec l'axe des y.
     *
     * Remarque :
     *  - En raison d'erreur d'arrondi, le fonctionnement de cette
     *    fonction peut etre perturbe dans le cas de vecteur enorme.
     *
     * Principe :
     *  - Rotation de V autour de Oz pour le placer dans le plan yOz
     *      == rotation de a1 avec a1 = angle entre Oy et la
     *         projection de V dans le plan xOy
     *          sin(a1) = Vx/sqrt(Vx**2+Vy**2)
     *          cos(a1) = Vy/sqrt(Vx**2+Vy**2)
     *      => M1 =
     *          cos1    -sin1   0       0
     *          sin1    cos1    0       0
     *          0       0       1       0
     *          0       0       0       1
     *
     *      => on obtient V1
     *
     *  - Rotation de V1 autour de Ox afin de l'aligner avec Oy
     *      == rotation de -a2 avec a2 = angle entre Oy et V1
     *          sin(a2) = Vz/sqrt(Vx**2+Vy**2+Vz**2)
     *          cos(a2) = sqrt(Vx**2+Vy**2)/sqrt(Vx**2+Vy**2+Vz**2)
     *      => M2 =
     *          1       0       0       0
     *          0       cos2    sin2    0
     *          0       -sin2   cos2    0
     *          0       0       0       1
     *
     *  - Traiter le cas particulier de sqrt(Vx**2+Vy**2) = 0
     *      Cas ou V est // a Oz
     *      == rotation de +-PI/2 autour de Ox
     *          et donc a1 = 0 ety a2 = +-PI/2
     *
     *  - Calcul de la matrice finale :
     *      M = M2 . M1
     *
     */
    int aligneVecteurSurOy(const OVector3D& vector);

    /**
     * \fn int invert();
     * \brief Inversion d'une matrice.
     *
     * Remarque :
     *  Algo realise par Richard Carling, extrait de Graphic Gems I.
     *
     * Principe :
     * <pre>
     *       -1
     *      A  = ___1__ adjoint A
     *            det A
     * </pre>
     *
     * \return Indique la reussite de la conversion (0 ou 1).
     */
    int invert();

    /**
     * \fn OMatrix getInvert(int * ok = 0) const;
     * \brief Retourne la matrice inverse de cette matrice.
     *
     * \see invert()
     *
     * \param ok Indique si l'inversion a pu se faire.
     */
    OMatrix getInvert(int* ok = 0) const;

    /**
     * \fn void adjoint();
     * \brief Calcul de la matrice adjointe d'une matrice.
     *
     * Remarque :
     *  Algo realise par Richard Carling, extrait de Graphic Gems I.
     *
     * Principe :
     * <pre>
     *    Calculate the adjoint of a 4x4 matrix:
     *      Let  a   denotes the minor determinant of matrix A obtained by
     *           ij
     *
     *      deleting the ith row and jth column from A.
     *
     *                    i+j
     *     Let  b   = (-1)    a
     *          ij            ji
     *
     *    The matrix B = (b  ) is the adjoint of A.
     *                     ij
     * </pre>
     *
     */
    void adjoint();

    /**
     * \fn OMatrix getAdjoint();
     * \brief Retourne la matrice adjointe de cette matrice.
     *
     * \see adjoint()
     */
    OMatrix getAdjoint();

    /**
     * \fn double determinant();
     * \brief Calcul du determinant d'une matrice.
     *
     * Remarque :
     *  Algo realie par Richard Carling, extrait de Graphic Gems I.
     */
    double determinant();

    /**
     * \fn static double mat2x2Det (double a, double b, double c, double d);
     * \brief Calcul du determinant d'une matrice 2 x 2.
     */
    static double mat2x2Det(double a, double b, double c, double d);

    /**
     * \fn static double mat3x3Det(double a1, double a2, double a3, double b1, double b2, double b3, double c1, double c2, double c3);
     * \brief Calcul du determinant d'une matrice 3 x 3.
     */
    static double mat3x3Det(double a1, double a2, double a3, double b1, double b2, double b3, double c1, double c2, double c3);

    /**
     * \fn OCoord3D dot(const OCoord3D& coord) const;
     * \brief Multiplication with a 3D coordinate
     *
     * \param coord The reference object with which this matrix is multiplicated.
     */
    OCoord3D dot(const OCoord3D& coord) const;

    /**
     */
    OCoord3D scale(const OCoord3D& coord) const;


    // Members
public:
    ///The 4x4 matrix array.
    double _m[4][4];
};

OVector3D operator*(const OMatrix& mat, const OVector3D& vec);
OPoint3D operator*(const OMatrix& mat, const OPoint3D& pt);


/**
 * \class OGeometrie
 * \brief Class utilitaire de geometrie.
 */
class OGeometrie
{
public:
    /**
     * \fn static bool intersDemiSegmentAvecSegment(const OPoint3D& ptS, const OPoint3D& ptA, const OPoint3D& ptB);
     * \brief Retourne true si l'horizontale a partir du point ptS intersecte le segment definit par ptA et ptB.
     *
     * \param ptS Le Le point du demi segment.
     * \param ptA Le 1er point pour le sgment.
     * \param ptB Le 2eme point pour le sgment.
     *
     * \return true  si l'intersection existe,
     *         false sinon.
     */
    static bool intersDemiSegmentAvecSegment(const OPoint3D& ptS, const OPoint3D& ptA, const OPoint3D& ptB);

    /**
     * \fn static int intersDroitesPoints(const OPoint3D& ptA, const OPoint3D& ptB, const OPoint3D& ptC, const OPoint3D& ptD, OPoint3D& ptI);
     * \brief Calcul de l'intersection entre deux droites definies chacune par 2 points.
     *
     * \param ptA Le 1er point pour la 1ere droite.
     * \param ptB Le 2eme point pour la 1ere droite.
     * \param ptC Le 1er point pour la 2eme droite.
     * \param ptD Le 2eme point pour la 2eme droite.
     * \param ptSym Le point resultant de l'intersection.
     *
     * \return INTERS_OUI (1) si l'intersection existe,
     *         INTERS_NULLE (0) s'il n'y a pas d'intersection.
     */
    static int intersDroitesPoints(const OPoint3D& ptA, const OPoint3D& ptB, const OPoint3D& ptC, const OPoint3D& ptD, OPoint3D& ptI);

    /**
     * \fn static int intersDroitesPointVecteur(const OPoint3D& ptA, const OVector3D& vecA, const OPoint3D& ptB, const OVector3D& vecB, OPoint3D& ptI);
     * \brief Calcul de l'intersection entre deux droites definies chacune par un point et un vecteur.
     *
     * \param ptA Point pour la 1ere droite.
     * \param vecA Vecteur pour la 1ere droite.
     * \param ptB Point pour la 2eme droite.
     * \param vecB Vecteur pour la 2eme droite.
     * \param ptSym Le point resultant de l'intersection.
     *
     * \return INTERS_OUI (1) si l'intersection existe,
     *         INTERS_NULLE (0) s'il n'y a pas d'intersection.
     */
    static int intersDroitesPointVecteur(const OPoint3D& ptA, const OVector3D& vecA, const OPoint3D& ptB, const OVector3D& vecB, OPoint3D& ptI);

    /**
     * \fn static double symPointDroite(const OPoint3D& ptA, const OPoint3D& ptB, const OPoint3D& ptP, OPoint3D& ptI);
     * \brief Calul du point symetrique au point passe par rapport a une droite definie par 2 points.
     *
     * \param ptA Le 1er point pour la 1ere droite.
     * \param ptB Le 2eme point pour la 1ere droite.
     * \param ptP Le point dont on veut le symetrique.
     * \param ptI Le point resultant de la symetrie.
     *
     * \return Le coefficient k (compris entre 0 et 1 si le segment
     *         [Point source / Point image] coupe le segment [ptA ptB]).
     */
    static double symPointDroite(const OPoint3D& ptA, const OPoint3D& ptB, const OPoint3D& ptP, OPoint3D& ptI);

    /**
     * \fn static bool pointInPolygonAngleSum(const OPoint3D& ptP, const OPoint3D* pts, int nbPts);
     * \brief Tests if a point is inside a polygon using angle sum algorithm.
     *
     * This solution was motivated by solution wich consists to compute the sum
     * of the angles made between the test point and each pair of points making
     * up the polygon and correspondence with Reinier van Vliet and Remco Lam.
     * If this sum is 2pi then the point is an interior point, if 0 then the point
     * is an exterior point.
     * To determine whether a point is on the interior of a convex polygon in 3D
     * one might be tempted to first determine whether the point is on the plane,
     * then determine it's interior status. Both of these can be accomplished at
     * once by computing the sum of the angles between the test point (ptP) and
     * every pair of edge points pts[i]->pts[i+1]. This sum will only be 2pi if
     * both the point is on the plane of the polygon AND on the interior.
     * The angle sum will tend to 0 the further away from the polygon point ptP becomes.
     *
     * \param ptP Le point a tester.
     * \param pts Un tableau de points constituant le polygone.
     * \param nbPts Le taille du tableau de points (doit etre >= 3).
     *
     * \return <code>true</code> si le point se trouve a l'interieur du polygone,
     *         <code>false</code> sinon.
     */
    static bool pointInPolygonAngleSum(const OPoint3D& ptP, const OPoint3D* pts, int nbPts);

    /**
     * \fn static bool pointInPolygonRayCasting(const OPoint3D& ptP, const OPoint3D* pts, int nbPts, const OVector3D& normal,const OBox & box);
     * \brief Tests if a point is inside a polygon using ray casting algorithm.
     *
     * \param ptP Le point a tester.
     * \param pts Un tableau de points constituant le polygone.
     * \param nbPts Le taille du tableau de points (doit etre >= 3).
     * \param normal La normale au polygone.
     * \param ptMin point min de la bouding box du polygon
     * \param ptMax point max de la bouding box du polygon
     *
     * \return <code>true</code> si le point se trouve a l'interieur du polygone,
     *         <code>false</code> sinon.
     */
    static bool pointInPolygonRayCasting(const OPoint3D& ptP, const OPoint3D* pts, int nbPts);

    /**
     * \fn static bool shortestSegBetween2Lines(const OPoint3D& pt1, const OPoint3D& pt2, const OPoint3D& pt3, const OPoint3D& pt4, OPoint3D& ptA, OPoint3D& ptB, double *mua, double *mub);
     * \brief Calculates the line segment PaPb that is the shortest route between two lines P1P2 and P3P4.
     *
     * Calculates also the values of mua and mub where:
     *    Pa = P1 + mua (P2 - P1)
     *    Pb = P3 + mub (P4 - P3)
     *
     * Remark: The values of mua and mub range from negative to positive infinity.
     * The line segments between P1 P2 and P3 P4 have their corresponding mu
     * between 0 and 1.
     *
     * \param pt1 A 1st point on the 1st line.
     * \param pt2 A 2nd point on the 1st line.
     * \param pt3 A 1st point on the 2nd line.
     * \param pt4 A 2nd point on the 2nd line.
     * \param ptA The 1st point of the resulting segment.
     * \param ptB The 2nd point of the resulting segment.
     * \param mua The mua value result.
     * \param mub The mub value result.
     *
     * \return <code>false</code> if no solution exists,
     *         <code>true</code> otherwise.
     */
    static bool shortestSegBetween2Lines(const OPoint3D& pt1, const OPoint3D& pt2, const OPoint3D& pt3, const OPoint3D& pt4,
                                         OPoint3D& ptA, OPoint3D& ptB, double* mua, double* mub);

    /**
     * \fn  static void boundingBox(OPoint3D* pts, int nbPts, OPoint3D& ptMin, OPoint3D& ptMax);
     * \brief Computes the simple bounding box for a volume using min-max method.
     *
     * \param pts Un tableau de points constituant le volume.
     * \param nbPts Le taille du tableau de points.
     * \param ptMin Le point contenant les valeurs min en X, Y et Z.
     * \param ptMax Le point contenant les valeurs max en X, Y et Z.
     */
    static void boundingBox(OPoint3D* pts, int nbPts, OPoint3D& ptMin, OPoint3D& ptMax);

    /**
     * \fn static void computeNormal(OPoint3D* pts, int nbPts, OVector3D & normal);
     * \brief Computes the normal of the list of points.
     *
     * \param pts Un tableau de points constituant le volume.
     * \param nbPts Le taille du tableau de points.
     * \param resulting normal.
     */
    static void computeNormal(OPoint3D* pts, int nbPts, OVector3D& normal);
};


/**
 * \class OSegment3D
 * \brief Classe de definition d'un segment.
 */
class OSegment3D
{
public:
    OSegment3D();
    OSegment3D(const OSegment3D& other);
    OSegment3D(const OPoint3D& ptA, const OPoint3D& ptB);

    virtual ~OSegment3D();

    virtual OSegment3D& operator=(const OSegment3D& other);
    virtual bool operator==(const OSegment3D& other) const;
    virtual bool operator!=(const OSegment3D& other) const;

    /**
     * \fn OSegment3D operator*(const OMatrix& matrix) const;
     * \brief Multiplication avec une matrice.
     *
     * \param matrix La matrice a appliquer aux points de ce segment.
     *
     * \return Une copie de ce segment transforme.
     */
    virtual OSegment3D operator*(const OMatrix& matrix) const;

    /**
     * \fn double longueur() const;
     * \brief Retourne la longueur du segment.
     */
    virtual double longueur() const;

    /**
     * \fn int symetrieOf(const OPoint3D& pt, OPoint3D& ptSym) const;
     * \brief Retourne le symetrique du point passe.
     *
     * \param pt Le point dont on veut le symetrique.
     * \param ptSym Le point resultant de la symetrie.
     *
     * \return 1 si le calcul a reussi, 0 sinon.
     */
    virtual int symetrieOf(const OPoint3D& pt, OPoint3D& ptSym) const;

    /**
     * \fn int projection(const OPoint3D& pt, OPoint3D& ptProj,double seuilConfondus) const;
     * \brief Retourne le projete du point passe.
     *
     * \param pt Le point dont on veut le projete.
     * \param ptProj Le point resultant de la projection.
     *
     * \return 1 si le calcul a reussi, 0 sinon.
     */
    virtual int projection(const OPoint3D& pt, OPoint3D& ptProj, double seuilConfondus) const;

    /**
     * \fn int intersects(const OSegment3D& seg, OPoint3D & pt,double seuilConfondus) const;
     * \brief Retourne le point d'intersection avec le segment passe.
     *
     * \param seg Segment dont on veut l'intersection.
     * \param ptSym Le point resultant de l'intersection.
     * \param seuilConfondus distance mini entre le point et le segment
     * en dessous de laquelle on considere qu'ils sont confondus
     *
     * \return INTERS_OUI (1) si l'intersection existe,
     *         INTERS_CONFONDU (2) si les elements sont confondus,
     *         INTERS_NULLE (0) s'il n'y a pas d'intersection.
     */
    virtual int intersects(const OSegment3D& seg, OPoint3D& pt, double seuilConfondus) const;

    /**
     * \fn OPoint3D centreOf() const;
     * \brief Retourne la position du milieu du segment
     *
     * \return OPoint3D
     */
    virtual OPoint3D centreOf() const;

    /**
     * \fn OPoint3D centerOfCurvedPath(const double &R) const;
     * \brief Retourne la position du centre de l'arc de cercle de rayon R reliant les deux extremites du segment
     *
     * \return OPoint3D Position du centre du cercle
     */
    virtual OPoint3D centerOfCurvedPath(const double& R) const;

    /**
     * \fn double lengthOfCurvedPath(const double& R);
     * \brief Calcule la longueur du trajet de rayon R passant par les extremites du segment
     * \param R rayon du cercle de rayon R
     * \return un double
     */
    virtual double lengthOfCurvedPath(const double& R);

    /**
     * \fn OVector3D toVector3D() const
     * \brief Construit un OVector3D a partir d'un segment utile pour la directivite des sources
     *
     * \return une reference sur un vecteur3D
     */
    virtual OVector3D toVector3D() const { return OVector3D(_ptA, _ptB); }

    /**
     * \fn OSegment3D swap() const;
     * \brief Retourne le segment
     */
    virtual OSegment3D swap() const;

public:
    ///Extremite A.
    OPoint3D _ptA;
    ///Extremite B.
    OPoint3D _ptB;
};


/**
 * \class ORepere3D
 * \brief 3D Repere with a point and 3 vectors.
 */
class ORepere3D
{
public:
    ORepere3D();
    ORepere3D(const ORepere3D& repere);
    /**
     * \brief Constructor with a point and 3 vectors.
     *
     * \param origin The origin point.
     * \param vecI Vector I for the X axis.
     * \param vecJ Vector J for the Y axis.
     * \param vecK Vector K for the Z axis.
     */
    ORepere3D(const OPoint3D& origin, const OVector3D& vecI, const OVector3D& vecJ, const OVector3D& vecK);
    /**
     * \brief Constructor with a point and 1 vectors.
     *  build an arbitrary 3D repere from a point and a single vector
     */
    ORepere3D(const OPoint3D& origin, const OVector3D& vec);
    /**
     * Constructor from a matrix
     *
     * \param matrix
     */
    ORepere3D(const OMatrix& matrix);

    virtual ~ORepere3D();

    ORepere3D& operator=(const ORepere3D& repere);
    bool operator==(const ORepere3D& repere) const;
    bool operator!=(const ORepere3D& repere) const;

    /**
     * \fn void set(const OPoint3D& origin, const OVector3D& vecI, const OVector3D& vecJ, const OVector3D& vecK);
     * \brief Sets with a point and 3 vectors.
     *
     * \param origin The origin point.
     * \param vecI Vector I for the X axis.
     * \param vecJ Vector J for the Y axis.
     * \param vecK Vector K for the Z axis.
     */
    void set(const OPoint3D& origin, const OVector3D& vecI, const OVector3D& vecJ, const OVector3D& vecK);

    /**
     * \fn void set(const OMatrix& matrix);
     * \brief Sets from a matrix.
     *
     * \param matrix A matrix to set this repere with.
     */
    void set(const OMatrix& matrix);

    /**
     * \fn void normalize();
     * \brief Normalize each vectors composing this repere.
     */
    void normalize();

    /**
     * \return The homogeneous matrix associated with this pose (aka ORepere)
     *
     * \brief return the transformation matrix from unity to this pose
     * such as this = transform * unity
     */
    OMatrix asMatrix() const;

public:
    ///The origin point.
    OPoint3D _origin;
    ///Vector I for the X axis.
    OVector3D _vecI;
    ///Vector J for the Y axis.
    OVector3D _vecJ;
    ///Vector K for the Z axis.
    OVector3D _vecK;

};


/**
 * \class OBox
 * \brief The box class.
 */

class OBox
{
public:
    OBox();
    OBox(const OBox& box);
    OBox(const OCoord3D& min, const OCoord3D& max);
    OBox(double x1, double y1, double z1, double x2, double y2, double z2);

    virtual ~OBox() {}

    virtual OBox& operator=(const OBox& box);
    virtual bool operator==(const OBox& box) const;
    virtual bool operator!=(const OBox& box) const { return !operator==(box);}

    /**
     * \fn bool isInside(const OPoint3D& pt) const
     * \brief Test whether the point is inside the box or not.
     *
     * \param pt The point to test.
     */
    virtual bool isInside(const OPoint3D& pt) const;

    /**
     * \fn bool isInside2D(const OPoint3D& pt) const
     * \brief Test whether the point is inside the box or not (from upper point of view).
     *
     * \param pt The point to test.
     */
    virtual bool isInside2D(const OPoint3D& pt) const;

    /**
     * \fn bool isInContact(const OBox& box) const
     * \brief Test whether the boxes are in contact or not.
     *
     * \param box The box to test.
     */
    virtual bool isInContact(const OBox& box) const;

    /**
     * \fn void Enlarge(const OPoint3D& pt)
     * \brief Enlarge the box with the point if the point is outside the box.
     *
     * \param pt The point to test/make it larger.
     */
    virtual void Enlarge(const OPoint3D& pt);

    /**
     * \fn void Enlarge(float x, float y, float z)
     * \brief Enlarge the box with the point (x,y, z) if the point is outside the box.
     *
     * \param x coordonnee x du point a tester
     * \param y coordonnee y du point a tester
     * \param z coordonnee z du point a tester
     */
    virtual void Enlarge(float x, float y, float z);

    /**
     * \fn void Enlarge(const OBox& box)
     * \brief Enlarge this box with the box passed if this box does not contain the box passed.
     *
     * \param box The box to test.
     */
    virtual void Enlarge(const OBox& box);

    /**
     * \fn void Translate(const OPoint3D& vectorTranslate)
     * \brief Translate this box
     *
     * \param vector translation.
     */
    virtual void Translate(const OPoint3D& vectorTranslate);

public:
    OPoint3D _min;
    OPoint3D _max;
};


/**
 * \class OBox2
 * \brief classe de definition d'une boite (pas forcement parallele aux axes a la difference de la OBox)
 */
class OBox2 : public OBox
{
    // Methods
public:
    OBox2();
    OBox2(const OBox2& box);
    /**
     * \fn OBox2(const OBox & box, ORepere3D & repere)
     * \brief Constructor from a box and its local coordinate system
     * \param box The box define in the local coordinate system
     */
    OBox2(const OBox& box);
    /**
     * \fn OBox2(const double& length, const double& width, const double& height);
     * \brief build a box centered on [0, 0, 0] from its length, width and height
     */
    OBox2(const double& length, const double& width, const double& height);

private : // Set private for security seems to an "af hoc" adaptation and is used only by an internal member function
    /**
     * \fn OBox2(const OBox & box, ORepere3D & repere)
     * \brief Constructor from a box and its local coordinate system
     * \param box The box define in the local coordinate system
     * \param repere The global coordinate system of the box
     * \param centre Is the box centre, this point is the middle of [S'R]
     */
    OBox2(const OBox2& box, const ORepere3D& repere, const OPoint3D& centre);

public :
    virtual ~OBox2() {}

    virtual OBox2& operator=(const OBox2& box);
    virtual bool operator==(const OBox2& box) const;
    virtual bool operator!=(const OBox2& box) const { return !operator==(box);}

    /**
     *\fn OCoord3D BoxCoord(int N);
     *\brief Returns the coordinates of one of the box corner.
     *\ We consider that the first corner is the one on the bottom left side
     *\ and then we go clockwise, upper and the same.
     *\ N must be between 0 and 8, where 0 represents the box center.
     *\ 1(0,0,0) 2(0,1,0) 3(1,1,0) 4(1,0,0) 5(1,0,1) 6(0,0,1) 7(0,1,1) 8(1,1,1)
     *\param N is the corner we want the coordinates of.
     */
    OPoint3D BoxCoord(int N) const;
    /**
     * \fn bool isInside(const OPoint3D& pt) const
     * \brief Test whether the point is inside the box or not.
     * \param pt The point to test.
     */
    virtual bool isInside(const OPoint3D& pt) const;
    /**
     * \fn bool isInside2D(const OPoint3D& pt) const
     * \brief Test whether the point is inside the box or not (from upper point of view).
     * \param pt The point to test.
     */
    virtual bool isInside2D(const OPoint3D& pt) const;
    /**
     * \fn void Translate(const OPoint3D& vectorTranslate)
     * \brief Translate this box
     * \param vector translation.
     */
    virtual void Translate(const OVector3D& vect);
    /**
     * fn OBox2 boxRotation(const OVector3D& v1, const OVector3D& v2)
     * brief return a box rotated by two vectors
     */
    OBox2 boxRotation(const OPoint3D& O, const OPoint3D& P2);
    /**
     *\fn void BoxRotationOzOy(const OBox2& box);
     *\brief Computes the box rotation around Oz and Oy (usual coordinates system).
     *\param box is an OBox2.
     */
    void BoxRotationOzOy(double alpha, double theta);
    /**
     * \fn void moveAndRotate(const OPoint3D& origin, const OVector3D& vec);
     * \brief Move and rotate the box
     */
    void moveAndRotate(const OPoint3D& origin, const OVector3D& vec);

private:
    /**
     * fn OBox2 rotInXOYOnly(const OVector3D& v1, const OVector3D& v2)
     * brief return a box rotated by two vectors
     */
    OBox2 rotInXOYOnly(const OVector3D& v1, const OVector3D& v2, const OPoint3D& O, const OPoint3D& P2);
    /**
     * fn OBox2 rotInXOZOnly(const OVector3D& v1, const OVector3D& v2)
     * brief return a box rotated by two vectors
     */
    OBox2 rotInXOZOnly(const OVector3D& v1, const OVector3D& v2, const OPoint3D& O, const OPoint3D& P2);
    /**
     * fn OBox2 rot3D(const OVector3D& v1, const OVector3D& v2)
     * brief return a box rotated by two vectors
     */
    OBox2 rot3D(const OVector3D& v1, const OVector3D& v2, const OPoint3D& O, const OPoint3D& P2);

public:
    ORepere3D _repere;
    OPoint3D _center;
    OPoint3D _A;
    OPoint3D _B;
    OPoint3D _C;
    OPoint3D _D;
    OPoint3D _E;
    OPoint3D _F;
    OPoint3D _G;
    OPoint3D _H;
    double _length;
    double _height;
    double _width;
};


/**
 * \class OHPlane3D
 * \brief The 3D Plane class using Hessian normal form
 *
 * the general equation of a plane  is: a.x + b.y + c.z = d
 *
 * by defining the components of the unit normal vector , N = (Nx, Ny, Nz)
 *  Nx = a / sqrt(a²+b²+c²)
 *  Ny = b / sqrt(a²+b²+c²)
 *  Nz = c / sqrt(a²+b²+c²)
 *
 * and the constant: p = d / sqrt(a²+b²+c²)
 *
 * Then the Hessian normal form of the plane is: N.X = -p
 */
class OHPlane3D
{
public:
    OHPlane3D();
    OHPlane3D(const OHPlane3D& Plane);
    /**
     * \brief Constructs a new Plane.
     *
     * \param normal The normal to the plane.
     * \param origin A point of the plane
     */
    OHPlane3D(const OVector3D& normal, const OPoint3D& origin);

    virtual ~OHPlane3D();

    /**
     * \brief set a new Plane.
     *
     * \param normal The normal to the plane.
     * \param origin A point of the plane
     */
    void set(const OVector3D& normal, const OPoint3D& origin);

    OHPlane3D& operator=(const OHPlane3D& Plane);
    bool operator==(const OHPlane3D& Plane) const;
    bool operator!=(const OHPlane3D& Plane) const;

    /**
     * \fn int intersects(const OPoint3D& pt1, const OPoint3D& pt2, OPoint3D& ptIntersec, double& t) const;
     * \brief Calcul de l'intersection avec un segment defini par deux points.
     *
     * Remarque :
     *  Dans le cas ou le segment est compris dans la ligne, le resultat
     *  est INTERS_CONFONDU, et les coordonnees de l'intersection ne
     *  sont pas mises a jour.
     *
     * \param pt1 Le 1er point du segment.
     * \param pt2 Le 2eme point du segment.
     * \param ptIntersec Le point correspondant a l'intersection.
     * \param t le coefficient barycentrique du pt d'intersection sur (pt1, pt2).
     *
     * \return INTERS_OUI (1) si l'intersection existe,
     *         INTERS_CONFONDU (2) si les elements sont confondus,
     *         INTERS_NULLE (0) s'il n'y a pas d'intersection.
     */
    int intersects(const OPoint3D& pt1, const OPoint3D& pt2, OPoint3D& ptIntersec, double& t) const;
    /**
     * \fn int intersects(const OSegment3D& seg, OPoint3D& ptIntersec) const;
     * \brief Calcul de l'intersection avec un segment
     *
     * Remarque :
     *  Dans le cas ou le segment est compris dans la ligne, le resultat
     *  est INTERS_CONFONDU, et les coordonnees de l'intersection ne
     *  sont pas mises a jour.
     *
     * \param seg Le segment.
     * \param ptIntersec Le point correspondant a l'intersection.
     *
     * \return INTERS_OUI (1) si l'intersection existe,
     *         INTERS_CONFONDU (2) si les elements sont confondus,
     *         INTERS_NULLE (0) s'il n'y a pas d'intersection.
     */
    int intersects(const OSegment3D& seg, OPoint3D& ptIntersec) const;

private:
    OVector3D   _N;
    OPoint3D    _O;
    double      _p;
};


#endif // TY_MC_3D
