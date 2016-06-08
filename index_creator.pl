#!/usr/bin/perl

use strict;

my $FILE1;
open $FILE1, "resources/raw/french_eng_words" or die $!;
my ($buf, $data, $n);
my $characterCounter = 0;
my $beginWord = 0;
my $endWord = 0;
my $beginTranslation = 0;
my $endTranslation = 0;
open(my $wordsIndex, '>:raw', 'resources/raw/fe_idx') or die "Unable to open: $!";
while (($n = read $FILE1, $data, 1) != 0){
    #print "$n bytes read\n";
    $buf .= $data;
    if ($data eq "\n") {
        #print "$data $characterCounter\n";
        $endTranslation = $characterCounter - 1;
        
        print $wordsIndex pack('s<',$beginWord) ;
        print $wordsIndex pack('s<',$endWord) ;
        print $wordsIndex pack('s<',$beginTranslation) ;
        print $wordsIndex pack('s<',$endTranslation) ;
        print "$beginWord $endWord $beginTranslation $endTranslation\n";
        $beginWord = $characterCounter+1;
    }
    if ($data eq "|") {
        #print "$data $characterCounter\n";
        $endWord = $characterCounter - 1;
        $beginTranslation = $characterCounter + 1;
    }
    $characterCounter++;
    #print "$data\n";
}
close($FILE1);
close($wordsIndex);