#!/usr/bin/perl -w
use strict;
use strict;

use IO::Handle;


my $GNUPLOT="gnuplot";

my $TO_FILE = 0;

open(PLOT, "| $GNUPLOT -persist -") || die ("Can't start $GNUPLOT: $!");
PLOT->autoflush(1);

print PLOT 'set title "RadioSignal"'."\n";
print PLOT "set mouse;\n";
print PLOT "set key outside below\nset autoscale;\n";

while(1)
{
  
  print PLOT "set term x11;";
  if ($TO_FILE)
  {
    print PLOT "set term png;";
    print PLOT "set output 'tmp.png';";
  }
  print PLOT "plot 'data/radio_sun.log' using 1:2 t 'oven_temp' lt -1 with lines\n;";

  exit 1
    if ($TO_FILE);

  sleep 1
}

close(PLOT);
