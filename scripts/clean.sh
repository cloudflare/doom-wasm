#!/usr/bin/env bash

cd "`dirname \"$0\"`/.."

rm -fr aclocal.m4 autom4te.cache autotools config.hin configure Makefile.in stamp-h1 config.log config.status config.h a.wasm Makefile clean

cd textscreen
rm -fr Makefile Makefile.in .deps *.o *.a
cd fonts
rm -fr Makefile Makefile.in
cd ..
cd examples
rm -fr Makefile Makefile.in *.o *.a *.wasm *.wasm.map *.worker.js *.html *.html.symbols *.js .deps
cd ../..

cd opl
rm -fr Makefile Makefile.in .deps *.o *.a
cd examples
rm -fr Makefile Makefile.in *.o *.a *.wasm *.wasm.map *.worker.js *.html *.html.symbols *.js .deps
cd ../..

cd data
rm -fr Makefile Makefile.in
cd ..

cd pcsound
rm -fr Makefile Makefile.in .deps *.o *.a .deps
cd ..

cd src
rm -fr Makefile Makefile.in .deps *.o *.a .deps chocolate* org.chocolate_doom* Doom.desktop Doom.metainfo.xml Doom_Screensaver.desktop
cd doom
rm -fr Makefile Makefile.in .deps *.o *.a .deps
cd ../..
