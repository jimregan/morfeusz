#! /usr/bin/perl -n

next if /^\#/;
next if /^0x[0-9A-F]{2}\s+\#\s*UNDEFINED$/;

die unless
    /^0x[0-9a-fA-F]{2}\t0x([0-9a-fA-F]{4})\s+\#\s*(.+)$/;

print "0x\U$1\E\t$2\n";
