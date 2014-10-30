#!/bin/sh

## ====================================================================
## This script builds a MacOS X Installer packages using MacPorts
##
## The installer is built from the latest CVS sources; you may modify 
## the cvs command below to use instead a tag or a date.
##
## The script must be run from a MacOS X computer with the MacPorts
## installed. Run the script by typing "./build_packages.sh" and enter
## your admin password when asked. The packages will be copied to the
## to the same directory containing the script. Use the MacOS X Finder
## "Create archive of..." command to zip each installer package before
## uploading.
##
## ====================================================================

dir=`PWD`

echo "Removing old files..."
## rm -rf xsb
rm -rf xsb-3.5.0-*.pkg*
## rm -f xsb-3.5.0.tar.gz

echo "Retrieving current XSB Subversion version..."
## svn checkout svn://svn.code.sf.net/p/xsb/src/trunk/XSB xsb

echo "Cleaning up exported XSB Subversion version..."
cd xsb
chmod a+x admin/cleandist.sh
admin/cleandist.sh

echo "Creating XSB sources archive..."
cd ..
## tar -czf xsb-3.5.0.tar.gz xsb

echo "Updating MacPorts XSB portfile..."
sha256="`openssl sha256 -r xsb-3.5.0.tar.gz`"
rmd160="`openssl rmd160 -r xsb-3.5.0.tar.gz`"
sudo mkdir -p /opt/local/var/macports/distfiles/xsb
sudo cp -f xsb-3.5.0.tar.gz /opt/local/var/macports/distfiles/xsb/xsb-3.5.0.tar.gz
sudo mkdir -p /opt/local/var/macports/sources/rsync.macports.org/release/tarballs/ports/lang/xsb
cd /opt/local/var/macports/sources/rsync.macports.org/release/tarballs/ports/lang/xsb
sudo cp -f Portfile Portfile.old
sudo cp $dir/Portfile .
sudo sed -e 's/^version.*/version 3.5.0/' -i '' Portfile
sudo sed -e "s/sha256.*/sha256 $sha256 \\\/" -i '' Portfile
sudo sed -e "s/rmd160.*/rmd160 $rmd160/" -i '' Portfile

echo "Creating XSB single-threaded installer..."
sudo port -d destroot +st
sudo port -d pkg
cp -R work/xsb-3.5.0.pkg $dir/xsb-3.5.0.pkg
zip -r $dir/xsb-3.5.0.pkg.zip $dir/xsb-3.5.0.pkg
sudo port clean

echo "Creating XSB multi-threaded installer..."
sudo port -d destroot +mt
sudo port -d pkg +mt
cp -R work/xsb-3.5.0.pkg $dir/xsb-3.5.0-mt.pkg
zip -r $dir/xsb-3.5.0-mt.pkg.zip $dir/xsb-3.5.0-mt.pkg
sudo port clean
