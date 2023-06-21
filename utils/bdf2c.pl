#!/usr/bin/perl
# bdf2c: convert bdf font to C file 
# Copyright 2019 Bernhard Schneider
# license: MIT
# usage: bdf2c 0-9 A-Z a-z font.bdf
#        no args: use all chars

use strict;
use utf8;

my $font; # root ptr
my $properties; # root ptr
my $char; # root ptr
my $chr;
my $chrname;
my $fontname;
my $basename;
my @ranges;
my $wbyte;
my %ord;
local *FH, *OC, *OH;

# parse args ###########################################

for (@ARGV) {
    my ($from, $to);
    
    if (-r $_) {
        ($basename = $fontname = $_) =~ s/\.bdf$//i;
         $basename =~ s/-/_/g;
        next;
    }
    die "unkown parameter: '$_'\n" unless (/(\S+)-(\S+)/);
    utf8::decode($from = $1);
    utf8::decode($to = $2);
    die "unknown range: ($1-$2)\n" if (ord($from) > ord($to));
    
    for (my $i = ord($from); $i <= ord($to); $i++) {
        $ord{$i} = 1 
    }
}

my $ord1 = undef;
for (sort { $a <=> $b } keys %ord) {
    if ($ord1 == undef) {
        if ( $ord{$_+1} == undef) {
            push @ranges, [ $_, $_ ];
            next;
        }
        $ord1 = $_;
        next;
    } else {
        if ($ord{1+$_} == undef) {
            push @ranges, [ $ord1, $_ ];
            $ord1 = undef;
        }
    }
}

# parse bdf  #########################################

open (FH, $fontname) or die "$@:$!";
while (my $n = <FH>) {

    $n =~ s/[\r\n]$//g;

    next if ( $n eq "" );

    if ($n =~ /^(STARTFONT)\s+(\d+(\.\d+)?)\s*$/) {
        $font->{$1} = $2;
        next;
    }
    if ($n =~ /^(FONT)\s+(\S+)\s*$/) {
        $font->{$1} = $2;
        next;
    }
    if ($n =~ /^(SIZE)\s+(\d+)\s+(\d+)\s+(\d+)\s*$/) {
        $font->{$1} = [ $2, $3, $4 ];
        next;
    }
    if ($n =~ /^(COMMENT)\s+(.*?)\s*$/) {
        $font->{$1} = $2;
        next;
    }
    if ($n =~ /^(FONTBOUNDINGBOX)\s+(-?\d+)\s+(-?\d+)\s+(-?\d+)\s+(-?\d+)\s*$/) {
        $font->{$1} = [ $2, $3, $4, $5 ];
        next;
    }
    if ($n =~ /^(STARTPROPERTIES)\s+(\d+)\s*$/) {
        $font->{$1} = $2;
        next;
    }
    if ($font->{'STARTPROPERTIES'} > 0 && $n =~ /^([A-Z0-9_]+)\s+(.*)$/) {
        eval "\$properties->{$1} = $2;";
        $font->{'STARTPROPERTIES'}--;
        next;
    }
    if ($n =~ /^ENDPROPERTIES\s*$/) {
        next;
    }

    if ($n =~ /^(CHARS)\s+(\d+)\s*$/) {
        $font->{$1} = $2;
        next;
    }
    if ($n =~ /^STARTCHAR\s+(\S+)\s*$/) {
        $chrname = $1;
        next;
    }
    if ($n =~ /^ENDCHAR\s*$/) {
        next;
    }
    if ($n =~ /^ENCODING\s+(\S+)\s*$/) {
        $chr = $1;
        $char->{$chr}{'NAME'} = $chrname;
        next;
    }
    if ($n =~ /^(SWIDTH|DWIDTH)\s+(\d+)\s+(\d+)\s*$/) {
        $char->{$chr}->{$1} = [ $2, $3 ];
        next;
    }
    if ($n =~ /^(BBX)\s+(\-?\d+)\s+(\-?\d+)\s+(\-?\d+)\s+(\-?\d+)\s*$/) {
        $char->{$chr}->{$1} = [ $2, $3, $4, $5 ];
        next;
    }
    if ($n =~ /^BITMAP\s*$/) {
        $char->{$chr}->{'BITMAP'} = '';
        next;
    }
    if ($n =~ /^([0-9A-F]+)\s*$/) {
        $wbyte = ($n =~ s/([0-9A-F]{2})/0x\1,/g);
        $char->{$chr}->{'BITMAP'} .= $n;
        next;
    }
    if ($n =~ /^ENDFONT\s*$/) {
        next;
    }
    die "unknown line: ($n)";
}
close(FH);

# if no ranges given, use whole font
if ( %ord == undef) {
    $ord1 = undef;
    for (sort { $a <=> $b } keys %$char ) {
        $ord{$_} = 1;
        if ($ord1 == undef) {
            if ($char->{$_+1} == undef) {
                push @ranges, [ $_, $_ ];
                next;
            }
            $ord1 = $_;
            next;
        } else {
            if ($char->{1+$_} == undef) {
                push @ranges, [ $ord1, $_ ];
                $ord1 = undef;
            }
        }
    }
}

# create header and c file
open (OC, ">" . $basename . ".c") or die "$@:$!";
open (OH, ">" . $basename . ".h") or die "$@:$!";

sub dprint($) {
    print OC @_[0];
    print OH @_[0];
}

dprint "/*\n";
dprint sprintf "            Font: %s\n", $font->{'FONT'};
dprint sprintf "       Copyright: %s\n", $properties->{'COPYRIGHT'}     if (defined $properties->{'COPYRIGHT'});
dprint sprintf "         Foundry: %s\n", $properties->{'FOUNDRY'}       if (defined $properties->{'FOUNDRY'});
dprint sprintf "          Notice: %s\n", $properties->{'NOTICE'}        if (defined $properties->{'NOTICE'});
dprint sprintf "         Comment: %s\n", $font->{'COMMENT'}             if (defined $font->{'COMMENT'});
dprint sprintf "     Family Name: %s\n", $properties->{'FAMILY_NAME'}   if (defined $properties->{'FAMILY_NAME'});
dprint sprintf "     Weight Name: %s\n", $properties->{'WEIGHT_NAME'}   if (defined $properties->{'WEIGHT_NAME'});
dprint sprintf "        SetWidth: %s\n", $properties->{'SETWIDTH_NAME'} if (defined $properties->{'SETWIDTH_NAME'});
dprint sprintf "      Pixel Size: %s\n", $properties->{'PIXEL_SIZE'};
dprint  " */\n\n";
dprint  "#include <stdint.h>\n\n";

my $arraysize;

$arraysize  = $wbyte * $properties->{'PIXEL_SIZE'} * scalar keys %ord; # chars
$arraysize += 4 * (scalar @ranges) + 1; # ranges + trailing 0
$arraysize += 5; # pixel_size bbox

# print OH  "#ifdef __cplusplus\n";
# print OH  "#extern \"C\" {\n";
print OH  "extern uint8_t $basename", "[",  $arraysize, "];\n";
# print OH  "}\n";
# print OH  "#endif\n\n";
print OC  "uint8_t $basename", "[",  $arraysize, "] = { \n";
print OC  "    ",$properties->{'PIXEL_SIZE'},",";
for (@{$font->{'FONTBOUNDINGBOX'}}) { print OC $_,"," };
print OC "\n    ";

my $linebreak = 0;
for (@ranges) { printf OC "0x%02x,0x%02x,0x%02x,0x%02x,", 
                    $_->[0] & 0xff, $_->[0] >> 8, $_->[1] & 0xff, $_->[1] >> 8; 
                print OC "\n    " if ( (++$linebreak % 3) == 0);
}
print OC "0,\n";

for (@ranges) {
    my ($from, $to) = @$_;
    for (my $i = $from; $i <= $to; $i++) {
        printf OC "    %s\n", $char->{$i}->{'BITMAP'};
    }
}
                   
print OC "};\n\n";
 
close(FH);
close(OC);
close(OH);

