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

#ifndef __O_MATRIX_3D__
#define __O_MATRIX_3D__

#include "OVector3D.h"
#include "OPoint3D.h"

#define EPSILON_50    1.e-50

/**
 * \file  OMatrix.h
 * \class  OMatrix
 * \brief The 4x4 matrix class.
 * \author Projet_Tympan
 */
class OMatrix
{
    // Methods
public:
    /**
     * Default constructor.
     */
    OMatrix();

    /**
     * Copy constructor.
     *
     * \param matrix The object to be copied.
     */
    OMatrix(const OMatrix& matrix);

    /**
     * Constructs a new object from a 4x4 double array.
     */
    OMatrix(double matrix[4][4]);

    /**
     * Destructor.
     */
    virtual ~OMatrix();

    /**
     * Assignment operator.
     *
     * \param matrix The source to be assigned.
     */
    OMatrix& operator=(const OMatrix& matrix);

    /**
     * The equality operator.
     *
     * \param matrix The object reference with which this object is compared.
     */
    bool operator==(const OMatrix& matrix) const;

    /**
     * The inequality operator.
     *
     * \param matrix The object reference with which this object is compared.
     */
    bool operator!=(const OMatrix& matrix) const;

    /**
     * \fn OMatrix operator+(const OMatrix& matrix) const;
     * \brief Addition with another matrix.
     *
     * \param matrix The object reference with which this object is added.
     */
    OMatrix operator+(const OMatrix& matrix) const;

    /**
     * \fn  OMatrix operator-(const OMatrix& matrix) const;
     * \brief Substraction with another matrix.
     *
     * \param matrix The object reference with which this object is substracted.
     */
    OMatrix operator-(const OMatrix& matrix) const;

    /**
     * \fn OMatrix operator*(const OMatrix& matrix) const;
     * \brief Multiplication with another matrix.
     *
     * \param matrix The object reference with which this object is multiplicated.
     */
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

#endif // __O_MATRIX_3D__
