#!/usr/bin/perl
use strict;
use LWP;
use URI::Escape;

sub save_file {
    my ($filename, $file_data) = @_;
    open OUT_FILE, ">$filename";
    print OUT_FILE $file_data;
    close OUT_FILE
}

sub dump_page {
    my ($page_no) = @_;
    my $req_url = sprintf("http://mnemonicdictionary.com/wordlist/GREwordlist?page=%d", $page_no);
    my $req  = HTTP::Request->new(GET => $req_url);
    my $ua = LWP::UserAgent->new;
    my $res = $ua->request($req);

    if($res->code == 200) {
        my $filename = sprintf("..\\data\\MDGRE\\MDGRE_%d.html", $page_no);
        save_file($filename, $res->content);
    }
    else {
        printf("Bad request for page %d.\n", $page_no);
    }
}

sub dump_mdgre {
    my ($start, $end) = @_;
    for(my $i = $start; $i<= $end; $i=$i+1) {
        dump_page($i);
    }
}

sub main {
    if($ARGV[0] > 0 && $ARGV[1] > $ARGV[0]) {
        dump_mdgre($ARGV[0], $ARGV[1]);
    }
}

main();
