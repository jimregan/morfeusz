# wewnêtrzne kodowanie Morfeusza
#
# modu³ udostêpnia tablicê asocjacyjn± przypisuj±c± nazwom znaków ich
# numery w tym¿e kodowaniu

package MorfEnc;

use strict;
use warnings;

use Exporter;
our (@ISA, @EXPORT,
     %morfenc);
@ISA         = qw(Exporter);
@EXPORT      = qw( %morfenc );



open ENC, "<morfeusz_encoding.txt";

my $code=-1;
while (<ENC>) {
    chomp;
    next if (/^\#/);
    if ($_ eq '*EVEN*') {
	$code+=1 if $code % 2 == 0;
	next;
    } elsif (s/^=//) {
	# synonim dla poprzedniej nazwy
	die "Duplicate name '$_'" if defined($morfenc{$_});
	$morfenc{$_} = $code;
    } else {
	# kolejny znak
	die "Duplicate name '$_'" if defined($morfenc{$_});
	$morfenc{$_} = ++$code;
    }
#    print "$_==$morfenc{$_}\n"; 
}

close ENC;

# sanity checks:

die "This should not happen: Caseable letters start at odd point"
    unless $morfenc{'CASEABLE'} % 2 == 0;

die "Too many code points: $morfenc{'NOTACHARACTER'}"
    unless $morfenc{'NOTACHARACTER'} < 256;

1;
