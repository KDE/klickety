#!/usr/bin/perl -w

# Copyright (c) 2010 Ni Hui <shuizhuyuanluo@126.com>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

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
