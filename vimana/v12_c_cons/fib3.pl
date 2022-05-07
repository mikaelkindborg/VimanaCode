#
# This is perl 5, version 30, subversion 2 (v5.30.2) built for darwin-thread-multi-2level
#
# perl fib3.pl  13.03s user 0.05s system 92% cpu 14.157 total
#

sub fibRec {
    my $n = shift;
    $n < 2 ? $n : fibRec($n - 1) + fibRec($n - 2);
}

print(fibRec(37));
