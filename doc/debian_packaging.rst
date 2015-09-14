.. _packaging-sec:

=======================
Debian jessie packaging
=======================

This section explains how to prepare and compile a Debian package of `Code_TYMPAN` for a Debian jessie
distribution.

The files needed to generate a package are located in **Code_TYMPAN/debian** directory.

Build upstream archive
======================

The archive must only contain the files that are necessary to compile `Code_TYMPAN`. One can use
`hg archive` command, and then add the 3rd parties to the archive (which are not versioned).

::

    cd Code_TYMPAN
    find . -name *.pyc  -delete
    hg archive ../code-tympan_4.7.0.orig.tar.gz
    cd ..
    tar xf code-tympan_4.7.0.orig.tar.gz
    cp -r Code_TYMPAN/3rdparty/* code-tympan_4.7.0.orig/3rdparty
    tar pczf code-tympan_4.7.0.orig.tar.gz code-tympan_4.7.0.orig/
    rm -r code-tympan_4.7.0.orig/

Don't forget to adapt `Code_TYMPAN` version (here *4.7.0*).


Install packaging tools
=======================

::

    sudo apt-get install pbuilder dpkg-dev

Create a jessie chroot
======================

::

    sudo pbuilder --create --distribution jessie

The generated files (.dsc, .deb, .tar.gz) resulting of the commands bellow will be generated in
*/var/cache/pbuilder/result*.

Generate .dsc file
==================

::

    cd Code_TYMPAN
    sudo dpkg-buildpackage -uc -S


Build package
=============

This command must be run from *Code_TYMPAN* parent directory.

::

    cd ..
    sudo pbuilder --build code-tympan_4.7.0-1.dsc

Install package
===============

Retrieve *code-tympan_4.7.0-1_amd64.deb* from */var/cache/pbuilder/result*.

::

    sudo apt-get update
    sudo dpkg -i code-tympan_4.7.0-1_amd64.deb
    sudo apt-get install -f

The last command installs the missing dependencies listed in the package.

More information
================

For more information, read `Debian documentation`_.

.. _`Debian documentation`: https://www.debian.org/doc/manuals/maint-guide/
