#!/usr/bin/env perl
#*****************************************************************************************
# build-error-table.pl
#
# Build the error lookup table from the errors.txt
#
# Author   :  Gary Ash <gary.ash@icloud.com>
# Created  :   2-Feb-2026  4:30pm
# Modified :
#
# Copyright © 2026 By Gary Ash All rights reserved.
#*****************************************************************************************
use strict;
use File::Basename;

my @errorNames;
my @errorNumbers;
my $longestErrorName  = 0;
my $errorTextFileName = dirname($0) . "/errors.txt";
open(my $errorTextFile, "<", $errorTextFileName)
  || die "Unable to open the errors file: $!";

while ((my $text = <$errorTextFile>)) {
    my @array = $text =~ /([A-Z|0-9]*)\s*([0-9]*)/;
    push @errorNames,   $array[0];
    push @errorNumbers, $array[1];
    my $len = length($array[0]);
    if ($len > $longestErrorName) {
        $longestErrorName = $len;
    }
}
$longestErrorName += 2;
my $index  = 0;
my $entry  = 0;
my $format = "\{ %3d, \%-${longestErrorName}s \},";
for my $text (@errorNames) {
    $text = '"' . $text . '"';
    printf($format, $errorNumbers[$index], $text);
    ++$index;
    ++$entry;
    if ($entry < 4) {
        print "\t";
    }
    else {
        $entry = 0;
        print "\n";
    }
}
close($errorTextFile);
print "\n";
