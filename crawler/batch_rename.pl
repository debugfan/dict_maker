#!/usr/bin/perl

sub batch_rename {
    my ($old, $new) = @_;
    foreach my $file (glob "*.html") {
        my $newFile = $file;
        $newFile =~ s/\Q$old\E/\Q$new\E/;
        if (-e $newFile) {
            warn "Can't rename $file to $newFile. The $newFile exists!\n";
        }
        else {
            rename $file, $newFile
            or warn "Rename $file to $newFile failed: $!\n";
        }
    }
}

sub main {
    if(@ARGV >= 2) {
        batch_rename($ARGV[0], $ARGV[1]);
    }
    else {
        warn "Arguments are less than 2.\n";
    }
}

main();
