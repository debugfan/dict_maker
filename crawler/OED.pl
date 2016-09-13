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
    my ($page_alpha, $page_no) = @_;
    my $req_url = "";
    if($page_no == 1) {
        $req_url = sprintf("http://www.etymonline.com/index.php?l=%c&allowed_in_frame=0", ord($page_alpha));
    }
    else {
        $req_url = sprintf("http://www.etymonline.com/index.php?l=%c&p=%d&allowed_in_frame=0", ord($page_alpha), $page_no-1);
    }
    my $req  = HTTP::Request->new(GET => $req_url);
    my $ua = LWP::UserAgent->new;
    my $res = $ua->request($req);

    if($res->code == 200) {
        my $filename = sprintf("..\\data\\OED\\OED_%c%d.html", ord($page_alpha), $page_no);
        save_file($filename, $res->content);
    }
    else {
        printf("Bad request for page %d.\n", $page_no);
    }
}

my %pages_table = ("a"=>60,
    "b"=>53,
    "c"=>84,
    "d"=>46,
    "e"=>37,
    "f"=>36,
    "g"=>32,
    "h"=>36,
    "i"=>37,
    "j"=>10,
    "k"=>9,
    "l"=>31,
    "m"=>56,
    "n"=>20,
    "o"=>22,
    "p"=>81,
    "q"=>6,
    "r"=>46,
    "s"=>108,
    "t"=>46,
    "u"=>24,
    "v"=>15,
    "w"=>22,
    "x"=>1,
    "y"=>4,
    "z"=>3);

sub next_chr {
    my $c = shift;
    return chr(ord($c)+1)
}    

sub dump_oed {
    my ($start, $end) = @_;
    for(my $c = $start; ord($c) <= ord($end); $c=next_chr($c)) {
        for(my $i = 1; $i <= $pages_table{$c}; $i++) {
                dump_page($c, $i);
            }
    }
}

sub main {
    if(ord($ARGV[0]) >= ord('a') && ord($ARGV[1]) >= ord($ARGV[0])) {
        dump_oed($ARGV[0], $ARGV[1]);
    }
}

main();

