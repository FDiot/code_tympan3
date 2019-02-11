# Get packages needed for building Code_TYMPAN
cd ..
[ ! -d debian ] && echo "Not run in TYMPAN_ROOT/debian !" && exit -1         
sudo=sudo && [ "`id -u`" = 0 ] && sudo=""
# Install needed packages:
$sudo apt-get install -y software-properties-common
$sudo add-apt-repository -y ppa:george-edison55/cmake-3.x
$sudo apt-get -y update
$sudo apt-get install -y cmake swig cython
$sudo apt-get install -y libqt4-dev libqt4-opengl libqt4-opengl-dev
$sudo apt-get install -y libboost-dev libboost-doc  
$sudo apt-get install -y libcgal-dev libcgal-qt4-dev 
$sudo apt-get install -y python3 python3-dev python3-numpy python3-scipy python3-shapely python3-matplotlib python3-tk
$sudo apt-get install -y python-sphinx doxygen doxygen-latex doxygen-doc doxygen-gui graphviz libjs-mathjax
# Third party install:
cd 3rdparty
package=Code_TYMPAN-4.7_third-party-05.zip
[ ! -f $package ] && wget https://bitbucket.org/TYMPAN/code_tympan/downloads/$package && unzip $package
cd ..

