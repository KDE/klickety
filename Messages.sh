#!/usr/bin/env bash
$EXTRACTRC `find . -name \*.rc -o -name \*.ui -o -name \*.kcfg` >> rc.cpp
$XGETTEXT *.cpp  *.h -o $podir/klickety.pot
rm -f rc.cpp
