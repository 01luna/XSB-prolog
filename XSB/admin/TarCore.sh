#!/bin/sh 

    # Create an XSB tarball; to be untarr'ed in the build directory

    # Run this in ./admin/ directory!

files="./LICENSE ./INSTALL ./INSTALL_PROBLEMS ./INSTALL_DOS ./INSTALL_NT \
	./README ./FAQ 
	./admin \
      ./build/ac* ./build/*.in ./build/config.guess ./build/config.sub \
      ./build/*sh ./build/*.msg ./build/configure ./build/README \
      ./emu/README     ./emu/*.[chi]     ./emu/*.pc    ./emu/*.mk \
      ./emu/nt ./emu/pcdos ./emu/configs ./emu/debugs \
      ./syslib/README  ./syslib/Makefile ./syslib/*.[DHPO] \
      ./cmplib/README  ./cmplib/Makefile ./cmplib/*.[DHPO] \
      ./lib/README     ./lib/Makefile    ./lib/*.[DHPO] \
      ./etc \
      ./packages "

    cd ..

    tar cvf XSB.tar $files

    gzip -f XSB.tar
    chmod 644 XSB.tar.gz
