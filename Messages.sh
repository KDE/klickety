#! /bin/sh
$EXTRACTRC `find . -name \*.ui -o -name \*.rc` > rc.cpp
$XGETTEXT *.cpp -o $podir/klickety.pot
