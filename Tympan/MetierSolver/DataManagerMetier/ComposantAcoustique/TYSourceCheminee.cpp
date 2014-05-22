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
#include "Tympan/MetierSolver/DataManagerMetier/TYPHMetier.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

#include "Tympan/MetierSolver/CommonTools/OSegment3D.h"

#include "Tympan/Tools/OMessageManager.h"

// Nombre de valeurs de Ka dans le tableau
#ifdef NB_KA
#   undef NB_KA
#   define NB_KA 38
#else
#   define NB_KA 38
#endif

// Nombre de valeurs de theta dans le tableau
#ifdef NB_THETA
#   undef NB_THETA
#   define NB_THETA 21
#else
#   define NB_THETA 21
#endif

OPROTOINST(TYSourceCheminee);

const double TYSourceCheminee::_tabQ[NB_KA][NB_THETA] =
{
    {
        /*ka1*/  3.9213E-02, 3.8299E-02, 3.5626E-02, 3.1399E-02, 2.5933E-02, 1.9633E-02, 1.2955E-02,
        6.3631E-03, 2.9321E-04, -4.8880E-03, -8.9124E-03, -1.1631E-02, -1.3023E-02, -1.3192E-02,
        -1.2351E-02, -1.0793E-02, -8.8587E-03, -6.8993E-03, -5.2361E-03, -4.1269E-03, -3.7380E-03
    },
    {
        /*ka2*/  1.5295E-01, 1.4940E-01, 1.3903E-01, 1.2260E-01, 1.0132E-01, 7.6753E-02, 5.0654E-02,
        2.4837E-02, 1.0112E-03, -1.9370E-02, -3.5229E-02, -4.5960E-02, -5.1463E-02, -5.2134E-02,
        -4.8805E-02, -4.2637E-02, -3.4980E-02, -2.7223E-02, -2.0640E-02, -1.6250E-02, -1.4711E-02
    },
    {
        /*ka3*/  3.3341E-01, 3.2573E-01, 3.0323E-01, 2.6755E-01, 2.2126E-01, 1.6765E-01, 1.1054E-01,
        5.3866E-02, 1.4017E-03, -4.3595E-02, -7.8677E-02, -1.0242E-01, -1.1455E-01, -1.1590E-01,
        -1.0831E-01, -9.4383E-02, -7.7136E-02, -5.9688E-02, -4.4893E-02, -3.5032E-02, -3.1576E-02
    },
    {
        /*ka4*/  5.7143E-01, 5.5835E-01, 5.2003E-01, 4.5914E-01, 3.7992E-01, 2.8790E-01, 1.8948E-01,
        9.1440E-02, 3.3045E-04, -7.8073E-02, -1.3934E-01, -1.8081E-01, -2.0183E-01, -2.0382E-01,
        -1.8997E-01, -1.6488E-01, -1.3394E-01, -1.0271E-01, -7.6267E-02, -5.8657E-02, -5.2487E-02
    },
    {
        /*ka5*/  8.5759E-01, 8.3811E-01, 7.8096E-01, 6.8997E-01, 5.7125E-01, 4.3285E-01, 2.8419E-01,
        1.3542E-01, -3.4683E-03, -1.2348E-01, -2.1752E-01, -2.8120E-01, -3.1323E-01, -3.1559E-01,
        -2.9322E-01, -2.5329E-01, -2.0429E-01, -1.5494E-01, -1.1321E-01, -8.5459E-02, -7.5741E-02
    },
    {
        /*ka6*/  1.1831E+00, 1.1564E+00, 1.0781E+00, 9.5307E-01, 7.8950E-01, 5.9808E-01, 3.9154E-01,
        1.8377E-01, -1.1197E-02, -1.8049E-01, -3.1367E-01, -4.0394E-01, -4.4902E-01, -4.5143E-01,
        -4.1811E-01, -3.5943E-01, -2.8771E-01, -2.1567E-01, -1.5487E-01, -1.1446E-01, -1.0033E-01
    },
    {
        /*ka7*/  1.5403E+00, 1.5058E+00, 1.4044E+00, 1.2424E+00, 1.0296E+00, 7.7974E-01, 5.0881E-01,
        2.3480E-01, -2.3835E-02, -2.4970E-01, -4.2827E-01, -5.4964E-01, -6.0999E-01, -6.1221E-01,
        -5.6552E-01, -4.8406E-01, -3.8483E-01, -2.8537E-01, -2.0156E-01, -1.4594E-01, -1.2649E-01
    },
    {
        /*ka8*/  1.9232E+00, 1.8805E+00, 1.7546E+00, 1.5530E+00, 1.2876E+00, 9.7474E-01, 6.3391E-01,
        2.8725E-01, -4.2025E-02, -3.3151E-01, -5.6184E-01, -7.1921E-01, -7.9754E-01, -7.9976E-01,
        -7.3751E-01, -6.2935E-01, -4.9783E-01, -3.6618E-01, -2.5541E-01, -1.8197E-01, -1.5631E-01
    },
    {
        /*ka9*/  2.3273E+00, 2.2758E+00, 2.1243E+00, 1.8812E+00, 1.5605E+00, 1.1808E+00, 7.6540E-01,
        3.4042E-01, -6.6025E-02, -4.2610E-01, -7.1493E-01, -9.1390E-01, -1.0138E+00, -1.0170E+00,
        -9.3769E-01, -7.9931E-01, -6.3089E-01, -4.6239E-01, -3.2076E-01, -2.2695E-01, -1.9418E-01
    },
    {
        /*ka10*/ 2.7492E+00, 2.6887E+00, 2.5106E+00, 2.2244E+00, 1.8459E+00, 1.3965E+00, 9.0236E-01,
        3.9399E-01, -9.5749E-02, -5.3340E-01, -8.8806E-01, -1.1353E+00, -1.2618E+00, -1.2684E+00,
        -1.1715E+00, -1.0001E+00, -7.9063E-01, -5.8088E-01, -4.0461E-01, -2.8794E-01, -2.4721E-01
    },
    {
        /*ka11*/ 3.1894E+00, 3.1197E+00, 2.9141E+00, 2.5834E+00, 2.1451E+00, 1.6229E+00, 1.0464E+00,
        4.4972E-01, -1.2959E-01, -6.5256E-01, -1.0819E+00, -1.3865E+00, -1.5474E+00, -1.5627E+00,
        -1.4504E+00, -1.2450E+00, -9.9164E-01, -7.3698E-01, -5.2275E-01, -3.8095E-01, -3.3145E-01
    },
    {
        /*ka12*/ 3.6467E+00, 3.5674E+00, 3.3337E+00, 2.9572E+00, 2.4572E+00, 1.8599E+00, 1.1978E+00,
        5.0838E-01, -1.6650E-01, -7.8278E-01, -1.2969E+00, -1.6703E+00, -1.8769E+00, -1.9100E+00,
        -1.7875E+00, -1.5497E+00, -1.2509E+00, -9.4848E-01, -6.9332E-01, -5.2427E-01, -4.6524E-01
    },
    {
        /*ka13*/ 4.0966E+00, 4.0077E+00, 3.7455E+00, 3.3227E+00, 2.7605E+00, 2.0874E+00, 1.3388E+00,
        5.5573E-01, -2.1615E-01, -9.2821E-01, -1.5310E+00, -1.9786E+00, -2.2366E+00, -2.2918E+00,
        -2.1605E+00, -1.8882E+00, -1.5398E+00, -1.1845E+00, -8.8398E-01, -6.8475E-01, -6.1517E-01
    },
    {
        /*ka14*/ 4.5369E+00, 4.4382E+00, 4.1471E+00, 3.6774E+00, 3.0522E+00, 2.3026E+00, 1.4670E+00,
        5.8981E-01, -2.7973E-01, -1.0889E+00, -1.7833E+00, -2.3100E+00, -2.6256E+00, -2.7083E+00,
        -2.5709E+00, -2.2632E+00, -1.8614E+00, -1.4484E+00, -1.0982E+00, -8.6591E-01, -7.8478E-01
    },
    {
        /*ka15*/ 4.9652E+00, 4.8566E+00, 4.5361E+00, 4.0188E+00, 3.3298E+00, 2.5030E+00, 1.5798E+00,
        6.0833E-01, -3.5883E-01, -1.2655E+00, -2.0530E+00, -2.6626E+00, -3.0418E+00, -3.1586E+00,
        -3.0192E+00, -2.6764E+00, -2.2180E+00, -1.7428E+00, -1.3386E+00, -1.0703E+00, -9.7666E-01
    },
    {
        /*ka16*/ 5.3802E+00, 5.2615E+00, 4.9111E+00, 4.3454E+00, 3.5916E+00, 2.6865E+00, 1.6752E+00,
        6.0927E-01, -4.5493E-01, -1.4582E+00, -2.3388E+00, -3.0336E+00, -3.4819E+00, -3.6400E+00,
        -3.5043E+00, -3.1278E+00, -2.6105E+00, -2.0689E+00, -1.6065E+00, -1.2994E+00, -1.1921E+00
    },
    {
        /*ka17*/ 5.7809E+00, 5.6519E+00, 5.2709E+00, 4.6558E+00, 3.8360E+00, 2.8514E+00, 1.7510E+00,
        5.9048E-01, -5.6989E-01, -1.6679E+00, -2.6400E+00, -3.4203E+00, -3.9418E+00, -4.1484E+00,
        -4.0235E+00, -3.6166E+00, -3.0391E+00, -2.4271E+00, -1.9025E+00, -1.5536E+00, -1.4318E+00
    },
    {
        /*ka18*/ 6.1672E+00, 6.0276E+00, 5.6153E+00, 4.9496E+00, 4.0623E+00, 2.9966E+00, 1.8057E+00,
        5.5010E-01, -7.0558E-01, -1.8960E+00, -2.9562E+00, -3.8199E+00, -4.4165E+00, -4.6786E+00,
        -4.5731E+00, -4.1408E+00, -3.5029E+00, -2.8170E+00, -2.2262E+00, -1.8328E+00, -1.6954E+00
    },
    {
        /*ka19*/ 6.5395E+00, 6.3890E+00, 5.9446E+00, 5.2269E+00, 4.2703E+00, 3.1214E+00, 1.8382E+00,
        4.8645E-01, -8.6398E-01, -2.1439E+00, -3.2877E+00, -4.2304E+00, -4.9013E+00, -5.2244E+00,
        -5.1475E+00, -4.6969E+00, -4.0000E+00, -3.2376E+00, -2.5767E+00, -2.1360E+00, -1.9820E+00
    },
    {
        /*ka20*/ 6.8987E+00, 6.7370E+00, 6.2595E+00, 5.4883E+00, 4.4602E+00, 3.2257E+00, 1.8476E+00,
        3.9797E-01, -1.0472E+00, -2.4141E+00, -3.6356E+00, -4.6504E+00, -5.3917E+00, -5.7788E+00,
        -5.7401E+00, -5.2805E+00, -4.5278E+00, -3.6868E+00, -2.9524E+00, -2.4616E+00, -2.2901E+00
    },
    {
        /*ka21*/ 7.2462E+00, 7.0730E+00, 6.5614E+00, 5.7349E+00, 4.6328E+00, 3.3096E+00, 1.8335E+00,
        2.8322E-01, -1.2576E+00, -2.7091E+00, -4.0024E+00, -5.0802E+00, -5.8841E+00, -6.3348E+00,
        -6.3433E+00, -5.8859E+00, -5.0828E+00, -4.1626E+00, -3.3514E+00, -2.8078E+00, -2.6178E+00
    },
    {
        /*ka22*/ 7.5836E+00, 7.3985E+00, 6.8516E+00, 5.9679E+00, 4.7890E+00, 3.3735E+00, 1.7953E+00,
        1.4071E-01, -1.4979E+00, -3.0326E+00, -4.3913E+00, -5.5215E+00, -6.3766E+00, -6.8863E+00,
        -6.9488E+00, -6.5063E+00, -5.6609E+00, -4.6621E+00, -3.7715E+00, -3.1726E+00, -2.9630E+00
    },
    {
        /*ka23*/ 7.9125E+00, 7.7152E+00, 7.1317E+00, 6.1886E+00, 4.9299E+00, 3.4179E+00, 1.7326E+00,
        -3.1215E-02, -1.7709E+00, -3.3886E+00, -4.8069E+00, -5.9777E+00, -6.8694E+00, -7.4281E+00,
        -7.5480E+00, -7.1343E+00, -6.2574E+00, -5.1825E+00, -4.2103E+00, -3.5537E+00, -3.3237E+00
    },
    {
        /*ka24*/ 8.2345E+00, 8.0244E+00, 7.4032E+00, 6.3983E+00, 5.0563E+00, 3.4431E+00, 1.6449E+00,
        -2.3448E-01, -2.0804E+00, -3.7824E+00, -5.2552E+00, -6.4540E+00, -7.3646E+00, -7.9573E+00,
        -8.1330E+00, -7.7614E+00, -6.8670E+00, -5.7206E+00, -4.6657E+00, -3.9491E+00, -3.6977E+00
    },
    {
        /*ka25*/ 8.5510E+00, 8.3278E+00, 7.6673E+00, 6.5982E+00, 5.1690E+00, 3.4493E+00, 1.5312E+00,
        -4.7147E-01, -2.4305E+00, -4.2200E+00, -5.7434E+00, -6.9574E+00, -7.8668E+00, -8.4733E+00,
        -8.6970E+00, -8.3789E+00, -7.4837E+00, -6.2733E+00, -5.1355E+00, -4.3570E+00, -4.0835E+00
    },
    {
        /*ka26*/ 8.8632E+00, 8.6262E+00, 7.9252E+00, 6.7892E+00, 5.2687E+00, 3.4364E+00, 1.3906E+00,
        -7.4501E-01, -2.8265E+00, -4.7089E+00, -6.2803E+00, -7.4966E+00, -8.3829E+00, -8.9783E+00,
        -9.2352E+00, -8.9780E+00, -8.1010E+00, -6.8372E+00, -5.6177E+00, -4.7758E+00, -4.4793E+00
    },
    {
        /*ka27*/ 9.1717E+00, 8.9207E+00, 8.1775E+00, 6.9719E+00, 5.3555E+00, 3.4041E+00, 1.2212E+00,
        -1.0587E+00, -3.2745E+00, -5.2580E+00, -6.8767E+00, -8.0824E+00, -8.9219E+00, -9.4773E+00,
        -9.7454E+00, -9.5504E+00, -8.7116E+00, -7.4086E+00, -6.1105E+00, -5.2042E+00, -4.8840E+00
    },
    {
        /*ka28*/ 9.4773E+00, 9.2117E+00, 8.4247E+00, 7.1465E+00, 5.4296E+00, 3.3517E+00, 1.0212E+00,
        -1.4170E+00, -3.7824E+00, -5.8785E+00, -7.5458E+00, -8.7278E+00, -9.4948E+00, -9.9776E+00,
        -1.0228E+01, -1.0089E+01, -9.3081E+00, -7.9839E+00, -6.6123E+00, -5.6413E+00, -5.2969E+00
    },
    {
        /*ka29*/ 9.7800E+00, 9.4993E+00, 8.6669E+00, 7.3131E+00, 5.4905E+00, 3.2779E+00, 7.8776E-01,
        -1.8253E+00, -4.3595E+00, -6.5844E+00, -8.3040E+00, -9.4487E+00, -1.0115E+01, -1.0489E+01,
        -1.0688E+01, -1.0589E+01, -9.8825E+00, -8.5589E+00, -7.1217E+00, -6.0864E+00, -5.7173E+00
    },
    {
        /*ka30*/ 1.0080E+01, 9.7834E+00, 8.9039E+00, 7.4713E+00, 5.5375E+00, 3.1815E+00, 5.1771E-01,
        -2.2903E+00, -5.0181E+00, -7.3937E+00, -9.1727E+00, -1.0265E+01, -1.0798E+01, -1.1024E+01,
        -1.1130E+01, -1.1049E+01, -1.0427E+01, -9.1291E+00, -7.6373E+00, -6.5392E+00, -6.1452E+00
    },
    {
        /*ka31*/ 1.0376E+01, 1.0064E+01, 9.1354E+00, 7.6206E+00, 5.5698E+00, 3.0605E+00, 2.0709E-01,
        -2.8202E+00, -5.7735E+00, -8.3303E+00, -1.0180E+01, -1.1202E+01, -1.1563E+01, -1.1594E+01,
        -1.1565E+01, -1.1468E+01, -1.0935E+01, -9.6893E+00, -8.1576E+00, -6.9995E+00, -6.5807E+00
    },
    {
        /*ka32*/ 1.0668E+01, 1.0339E+01, 9.3604E+00, 7.7600E+00, 5.5861E+00, 2.9127E+00, -1.4879E-01,
        -3.4253E+00, -6.6463E+00, -9.4274E+00, -1.1367E+01, -1.2295E+01, -1.2433E+01, -1.2217E+01,
        -1.2003E+01, -1.1852E+01, -1.1400E+01, -1.0234E+01, -8.6812E+00, -7.4676E+00, -7.0243E+00
    },
    {
        /*ka33*/ 1.0956E+01, 1.0609E+01, 9.5782E+00, 7.8886E+00, 5.5849E+00, 2.7355E+00, -5.5553E-01,
        -4.1185E+00, -7.6641E+00, -1.0733E+01, -1.2791E+01, -1.3593E+01, -1.3439E+01, -1.2909E+01,
        -1.2457E+01, -1.2206E+01, -1.1820E+01, -1.0758E+01, -9.2063E+00, -7.9436E+00, -7.4766E+00
    },
    {
        /*ka34*/ 1.1237E+01, 1.0873E+01, 9.7877E+00, 8.0051E+00, 5.5648E+00, 2.5261E+00, -1.0198E+00,
        -4.9166E+00, -8.8666E+00, -1.2322E+01, -1.4549E+01, -1.5171E+01, -1.4623E+01, -1.3692E+01,
        -1.2939E+01, -1.2540E+01, -1.2193E+01, -1.1254E+01, -9.7309E+00, -8.4279E+00, -7.9385E+00
    },
    {
        /*ka35*/ 1.1512E+01, 1.1129E+01, 9.9878E+00, 8.1084E+00, 5.5239E+00, 2.2812E+00, -1.5494E+00,
        -5.8416E+00, -1.0313E+01, -1.4320E+01, -1.6807E+01, -1.7156E+01, -1.6046E+01, -1.4592E+01,
        -1.3464E+01, -1.2864E+01, -1.2521E+01, -1.1718E+01, -1.0253E+01, -8.9208E+00, -8.4107E+00
    },
    {
        /*ka36*/ 1.1778E+01, 1.1376E+01, 1.0177E+01, 8.1972E+00, 5.4605E+00, 1.9974E+00, -2.1540E+00,
        -6.9238E+00, -1.2098E+01, -1.6967E+01, -1.9918E+01, -1.9789E+01, -1.7805E+01, -1.5641E+01,
        -1.4046E+01, -1.3190E+01, -1.2810E+01, -1.2145E+01, -1.0769E+01, -9.4225E+00, -8.8943E+00
    },
    {
        /*ka37*/ 1.2035E+01, 1.1614E+01, 1.0355E+01, 8.2703E+00, 5.3729E+00, 1.6709E+00, -2.8454E+00,
        -8.2070E+00, -1.4393E+01, -2.0830E+01, -2.4847E+01, -2.3638E+01, -2.0073E+01, -1.6886E+01,
        -1.4703E+01, -1.3528E+01, -1.3066E+01, -1.2530E+01, -1.1275E+01, -9.9329E+00, -9.3900E+00
    },
    {
        /*ka38*/ 1.2282E+01, 1.1841E+01, 1.0521E+01, 8.3267E+00, 5.2594E+00, 1.2974E+00, -3.6384E+00,
        -9.7578E+00, -1.7557E+01, -2.7951E+01, -3.7266E+01, -3.0744E+01, -2.3208E+01, -1.8396E+01,
        -1.5453E+01, -1.3891E+01, -1.3297E+01, -1.2873E+01, -1.1769E+01, -1.0452E+01, -9.8987E+00
    }
};



TYSourceCheminee::TYSourceCheminee()
{
    _name = TYNameManager::get()->generateName(getClassName());

    _type = TypeCheminee;
    //    _pAcousticRectangle = new TYAcousticRectangle();
    _pAcousticRectangle = new TYRectangle();
}

TYSourceCheminee::TYSourceCheminee(const TYSourceCheminee& other)
{
    *this = other;
}

TYSourceCheminee::~TYSourceCheminee()
{
}

TYSourceCheminee& TYSourceCheminee::operator=(const TYSourceCheminee& other)
{
    if (this != &other)
    {
        TYSourcePonctuelle::operator =(other);
        _pAcousticRectangle = other._pAcousticRectangle;
    }
    return *this;
}

bool TYSourceCheminee::operator==(const TYSourceCheminee& other) const
{
    if (this != &other)
    {
        if (TYSourcePonctuelle::operator !=(other)) { return false; }
        if (_pAcousticRectangle != other._pAcousticRectangle) { return false; }
    }
    return true;
}

bool TYSourceCheminee::operator!=(const TYSourceCheminee& other) const
{
    return !operator==(other);
}

bool TYSourceCheminee::deepCopy(const TYElement* pOther, bool copyId /*=true*/)
{
    if (!TYSource::deepCopy(pOther, copyId)) { return false; }

    TYSourceCheminee* pOtherSrc = (TYSourceCheminee*) pOther;

    if (_pAcousticRectangle && pOtherSrc->_pAcousticRectangle)
    {
        _pAcousticRectangle->deepCopy(pOtherSrc->_pAcousticRectangle, copyId);
    }
    else
    {
        _pAcousticRectangle = NULL;
    }

    return true;
}

std::string TYSourceCheminee::toString() const
{
    return "TYSourceCheminee";
}

DOM_Element TYSourceCheminee::toXML(DOM_Element& domElement)
{
    DOM_Element domNewElem = TYSourcePonctuelle::toXML(domElement);

    if (_pAcousticRectangle)
    {
        // Mise en commentaire DT20061228 - car cette ligne devrait etre inutile
        //      TYXMLTools::addElementStringValue(domNewElem, "accRectId", _pAcousticRectangle->getID().toString().c_str());
    }

    return domNewElem;
}

int TYSourceCheminee::fromXML(DOM_Element domElement)
{
    TYSourcePonctuelle::fromXML(domElement);

    _pAcousticRectangle = NULL;

    QString id;
    DOM_Element elemCur;

    QDomNodeList childs = domElement.childNodes();

    for (unsigned int i = 0; i < childs.length(); i++)
    {
        elemCur = childs.item(i).toElement();

        if (TYXMLTools::getElementStringValue(elemCur, "accRectId", id))
        {
            _pAcousticRectangle = TYRectangle::safeDownCast(TYElement::getInstance(id));
        }
    }


    return 1;
}

//void TYSourceCheminee::setAcousticRectangle(LPTYAcousticRectangle pAcousticRectangle)
void TYSourceCheminee::setAcousticRectangle(LPTYRectangle pAcousticRectangle)
{
    _pAcousticRectangle = pAcousticRectangle;
    _pAcousticRectangle->setParent(this);
    setIsAcousticModified(true);
}

OSpectre TYSourceCheminee::lwApparenteSrcDest(const OSegment3D& seg, const TYAtmosphere& atmos, const int& expGeo /*=0*/, const int& regime/*=-1*/) const
{
    OSpectre s = OSpectre::getEmptyLinSpectre();

    if (!_pAcousticRectangle)
    {
        return s;
    }

    //    double rayonBouche = _pAcousticRectangle->getBoundingRect()->getCircleEqDiameter() / 2.0; // 1/2 longueur de la diagonale de la bouche
    double rayonBouche = _pAcousticRectangle->getCircleEqDiameter() / 2.0; // 1/2 longueur de la diagonale de la bouche
    OSpectre spectre_Ka = atmos.getKAcoust().mult(rayonBouche);

    OVector3D v3D = seg.toVector3D();
    OVector3D normale = _pAcousticRectangle->normal();
    double theta = v3D.angle(normale); // Angle du segment par rapport a la normale au support de la source

    // angle compris entre 0 et pi
    if (theta < -M_PI) { theta = theta + M_2PI; }
    if (theta >  M_PI) { theta = theta - M_2PI; }
    theta = ABS(theta); // On prend la valeur absolue de theta

    int indice_theta = (int)(20 * theta / M_PI); // Indice de l'angle theta dans le tableau
    //  indice_theta = indice_theta > (NB_THETA-2) ? NB_THETA-2 : indice_theta; // Eviter les depassement de tableau

    for (unsigned int i = 0 ; i < s.getNbValues(); i++)
    {
        double ka = spectre_Ka.getTabValReel()[i];
        ka = ka > 3.8 ? 3.8 : ka ;
        int indice_Ka = (int)(10 * ka);  //-1;

        //      indice_Ka = indice_Ka >= 0 ? indice_Ka : 0;

        //      indice_Ka = indice_Ka > (NB_KA-2) ? NB_KA-2 : indice_Ka; // Eviter les depassement de tableau

        s.getTabValReel()[i] = normeQChem(indice_Ka, indice_theta, ka, theta);
    }

    return s.racine();
}

double TYSourceCheminee::normeQChem(const int& indice_Ka, const int& indice_theta, const double& ka, const double& theta) const
{
    /* ===========================================================================================
       Recherche par interpolation lineaire dans le tableau _tabQ
       de la valeur de Q la plus proche
       ===========================================================================================*/
    int indiceKaTab = indice_Ka > 0 ? indice_Ka - 1 : 0; // Eviter les depassement de tableau
    indiceKaTab = indiceKaTab > (NB_KA - 2) ? NB_KA - 2 : indiceKaTab; // Eviter les depassement de tableau
    int indiceThetaTab = indice_theta > (NB_THETA - 2) ? NB_THETA - 2 : indice_theta; // Eviter les depassement de tableau
    indiceThetaTab = indiceThetaTab > 0 ? indiceThetaTab - 1 : 0;

    double tabQ1_1 = _tabQ[indiceKaTab]  [indice_theta];//_tabQ[indice_Ka]  [indice_theta]
    double tabQ1_2 = _tabQ[indiceKaTab]  [indice_theta + 1]; //_tabQ[indice_Ka]  [indice_theta+1]
    double tabQ2_1 = _tabQ[indiceKaTab + 1][indice_theta]; //_tabQ[indice_Ka+1][indice_theta]
    double tabQ2_2 = _tabQ[indiceKaTab + 1][indice_theta + 1]; //_tabQ[indice_Ka+1][indice_theta+1]

    double ka1 = indice_Ka / 10.0;
    double ka2 = (indice_Ka + 1) / 10.0;

    double theta1 = indice_theta * M_PI / 20.0;
    double theta2 = (indice_theta + 1) * M_PI / 20.0;
    //  double pos_theta = (theta - theta1) / (theta2 - theta1);

    double val1 = tabQ1_1 + (theta - theta1) * (tabQ1_2 - tabQ1_1) / (theta2 - theta1);
    double val2 = tabQ2_1 + (theta - theta1) * (tabQ2_2 - tabQ2_1) / (theta2 - theta1);

    double val = val1 + (ka2 - ka) * (val2 - val1) / (ka2 - ka1);//val1 + (ka - ka1) * (val2 - val1) / (ka2 - ka1)


    return pow(10.0, val / 10.0);
}
