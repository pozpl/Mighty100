#!/usr/bin/perl

use strict;

sub processFile($$){
    my ($dictFilePath, $indexPath) = @_;
    my $FILE1;
    open $FILE1, $dictFilePath or die $!;
    my ($buf, $data, $n);
    my $characterCounter = 0;
    my $beginWord = 0;
    my $endWord = 0;
    my $beginTranslation = 0;
    my $endTranslation = 0;
    open(my $wordsIndex, '>:raw',  $indexPath) or die "Unable to open: $!";
    my $pipe_found = 0;
    while (($n = read $FILE1, $data, 1) != 0){
        #print "$n bytes read\n";
        $buf .= $data;
        if ($data eq "\n") {
            #print "$data $characterCounter\n";
            $endTranslation = $characterCounter - 1;
            if($pipe_found){
                print $wordsIndex pack('s<',$beginWord) ;
                print $wordsIndex pack('s<',$endWord) ;
                print $wordsIndex pack('s<',$beginTranslation) ;
                print $wordsIndex pack('s<',$endTranslation) ;
            }
            print "$beginWord $endWord $beginTranslation $endTranslation\n";
            $beginWord = $characterCounter+1;
            $pipe_found = 0;
        }
        if ($data eq "|") {
            #print "$data $characterCounter\n";
            $endWord = $characterCounter - 1;
            $beginTranslation = $characterCounter + 1;
            $pipe_found = 1;
        }
        $characterCounter++;
        #print "$data\n";
    }
    close($FILE1);
    close($wordsIndex);
}

processFile("resources/raw/french_eng_words", 'resources/raw/fe_idx');
processFile('resources/raw/spanish_eng_words_0_100','resources/raw/se_100_idx');
processFile('resources/raw/port_eng_words_1_100','resources/raw/pe_100_idx');