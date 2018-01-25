# Build Code_TYMPAN Debian package
# Automate TYMPAN_ROOT/doc/debian_packaging.rst instructions
abort()
{
   echo "Error ! See $1"
   cat $1
   exit -1
}
cd ..
[ ! -d debian ] && echo "Not run in TYMPAN_ROOT/debian !" && exit -1
TYMPAN_ROOT=`pwd`
version=code-tympan_4.7.0
package=$version.orig
archive=$package.tar.gz
sudo=sudo && [ "`id -u`" = 0 ] && sudo=""

echo "Install packaging tools..."
log=/tmp/apt-get.log
$sudo apt-get install -y --force-yes pbuilder dpkg-dev debian-archive-keyring debhelper 1>$log 2>&1 || abort $log

if [ ! -f ../$archive ] || [ "`find * -newer ../$archive`" != "" ]
then
   echo "Rebuilding $archive..."
   [ -d /var/cache/pbuilder/build ] && $sudo pbuilder --clean
   # Construit l'archive
   find . -name *.pyc -delete
   hg archive ../$archive
   cd ..
   tar xf $archive
   cp -r $TYMPAN_ROOT/3rdparty/* $package/3rdparty
   tar pczf $archive $package/
   rm -r $package/
   echo "$archive created!"
   ls -lart $archive
   cd -
fi

if [ ! -f /var/cache/pbuilder/base.tgz ]
then 
   echo "Creating a jessie chroot..." 
   log=/tmp/pbuilder_create.log
   $sudo pbuilder --create --debug \
--mirror "http://debian.intergenia.de/debian" \
--distribution jessie --debootstrapopts \
--keyring=/usr/share/keyrings/debian-archive-keyring.gpg 1>$log 2>&1 || abort $log
   # Add PPA for cmake 3.2.2
   echo "apt-get install -y software-properties-common
echo deb http://ppa.launchpad.net/george-edison55/cmake-3.x/ubuntu trusty main > /etc/apt/sources.list.d/george-edison55-cmake-3_x-trusty.list
echo deb-src http://ppa.launchpad.net/george-edison55/cmake-3.x/ubuntu trusty main >> /etc/apt/sources.list.d/george-edison55-cmake-3_x-trusty.list
apt-key adv --recv-keys --keyserver keyserver.ubuntu.com 084ECFC5828AB726
apt-get update" > ./add_ppa
   log=/tmp/execute.log
   sudo pbuilder --execute --save-after-exec ./add_ppa 1>$log 2>&1 || abort $log
   rm -f add_ppa
fi

echo "Generate $version-1.dsc file ..."
cd $TYMPAN_ROOT
log=/tmp/dpkg-buildpackage.log
$sudo dpkg-buildpackage -uc -S 1>$log 2>&1 || abort $log

echo "Build package $version-1_amd64.deb ..."
cd ..
log=/tmp/pbuilder_build.log
$sudo pbuilder --build $version-1.dsc 1>$log 2>&1 || abort $log
deb=/var/cache/pbuilder/result/$version-1_amd64.deb
ls -lart $deb

echo "Install package $version-1_amd64.deb ..."
$sudo apt-get update
$sudo dpkg -i $deb
$sudo apt-get install -f
