#! /usr/bin/perl

use strict;

open SVN, "svn update|";
my @svn_output = <SVN>;
close SVN;

my $current_rev;
foreach my $line (@svn_output)
{
  if ($line =~ /Revision /)
  {
    $current_rev = $line;
    $current_rev =~ s/^Revision (\d+)\.$/$1/;
    chomp($current_rev);
  }
}

my $new_rev = $current_rev + 1;

print("Current revision: $current_rev\n");
print("New revision: $new_rev\n");

my $look_for = ".*GTKEVEMON_VERSION_STR.*";
my $subst = "#define GTKEVEMON_VERSION_STR \"Revision 1.5-$new_rev\"";

open FILE, "src/defines.h" or die("Cannot open defines.h (reading)");

my @file_content = <FILE>;
my $file_output;
foreach my $line (@file_content)
{
  $line =~ s/$look_for/$subst/;
  $file_output .= $line;
}
close FILE;

print("Writing defines.h...\n");
open FILE, "> src/defines.h" or die("Cannot open defines.h (writing)");
print FILE $file_output;
close FILE;

print("Executing SVN...\n");
system("svn ci");

print("Backend needs update: http://gtkevemon.battleclinic.com/updates/\n");
