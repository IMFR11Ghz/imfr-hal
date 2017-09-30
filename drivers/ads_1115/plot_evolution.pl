#!/usr/bin/perl -w
use strict;
use strict;

use IO::Handle;


my $GNUPLOT="gnuplot";

my $TO_FILE = 0;

open(PLOT, "| $GNUPLOT -persist -") || die ("Can't start $GNUPLOT: $!");
PLOT->autoflush(1);

print PLOT 'set title "RADIO SIGNAL!"'."\n";
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
  #print PLOT "plot 'data/oven_program_normal.txt' with linespoints, ";
  print PLOT "plot 'radio_sun.log' using 1:2 t 'voltage_time' lt -1 with lines\n;";

  #print PLOT "'oven1.log' using 1:2 t 'oven_temp' lt -1 with lines\n;";
  #print PLOT "'oven1.log' using 1:2 t 'oven_temp' with lines\n;";

  exit 1
    if ($TO_FILE);

  sleep 0.2
}

close(PLOT);
