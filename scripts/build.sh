#!/bin/bash

cd "`dirname \"$0\"`/.."

emmake make clean
emconfigure autoreconf -fiv
ac_cv_exeext=".html" emconfigure ./configure --host=none-none-none

emmake make
