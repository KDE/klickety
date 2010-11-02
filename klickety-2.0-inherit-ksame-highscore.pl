#!/usr/bin/perl -w
use strict;
# this script updates klickety ksame mode highscores according to the
# original ksame game.

# read the whole config file
while ( <> ) {
    chomp; # eat the trailing '\n'
    next if ( /^$/ ); # skip empty lines
    next if ( /^\#/ ); # skip comments
    next if ( /\d+_Board/ ); # skip board numbers
    print "[KHighscore_KSame]\n" if ( /^\[KHighscore\]$/ ); # [KHighscore] -> [KHighscore_KSame]
    print "$_\n" if ( /^\d+_(LastPlayer|Score|Name)=.*$/ ); # print the highscores
}
