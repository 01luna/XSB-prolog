#!/bin/sh 

    # Create an XSB tarball sans testsuite; untar in the XSB install directory

    # RUN this in ./admin/ directory!

files="./LICENSE ./INSTALL ./INSTALL_PROBLEMS ./INSTALL_WINDOWS \
	./README ./FAQ ./Makefile \
	./admin \
      ./build/ac* ./build/*.in ./build/config.guess ./build/config.sub \
      ./build/*sh ./build/*.msg ./build/configure ./build/README \
      ./emu ./syslib ./cmplib  ./lib \
      ./etc \
      ./packages \
      ./docs \
      ./examples "

    cd ..

    tar cvf XSB.tar $files

    gzip -f XSB.tar
