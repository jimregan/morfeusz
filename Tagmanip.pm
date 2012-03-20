#
# (c) Copyright by Marcin Woli\'nski, 2002
#
# Funkcje rozklejające i sklejające kropkowe alternatywy wartości
# w znacznikach.
# 
package Tagmanip;

use strict;
#use warnings;

use Exporter;
our (@ISA, @EXPORT, @EXPORT_OK,
     %normcache);
@ISA         = qw(Exporter);
@EXPORT      = qw( &explode_tags &implode_tags &norm_tags);
@EXPORT_OK   = qw( &sortuniq &tagsortuniq);


sub explode_tags {
    my @tagi = map {split '\|', $_} @_;
    my @ntagi;
    while (@tagi) {
	my $tag = shift @tagi;
	if ($tag =~ m/^((?:.+:)?)([^:]+\.[^:]+)((?::.+)?)$/) {
	    my ($prealt, $alt, $postalt) = ($1,$2,$3);
	    unshift @tagi, map "$prealt$_$postalt", split('\.', $alt);
	} else {
	    push @ntagi, $tag;
	}
    }
    return @ntagi;
}


sub implode_tags {
    my @tagi = tagsortuniq(@_);
    my $max = 0;
    for my $tag (@tagi){
	my $num=split /:/,$tag;
	$max=$num
	    if $num>$max;
    }
    # dla wszystkich kategorii z wyjątkiem klasy fleksyjnej:
    for(my $num=$max; $num>0; $num--){
	my %preposttagi=();
	my @ntagi = ();
	while (@tagi) {
	    my $tag = shift @tagi;
	    if ($tag =~ m/^((?:[^:]+:){$num})([^:]+)((?::.+)?)$/) {
		my ($prealt, $proalt, $postalt) = ($1,$2,$3);
		my $key = "$prealt $postalt";
		if (!defined $preposttagi{$key}) {
		    my $tagstruct = [$prealt, $postalt, [$proalt]];
		    $preposttagi{$key} = $tagstruct;
		    push @ntagi, $tagstruct;
		} else {
		    push @{$preposttagi{$key}->[2]}, $proalt;
		}
	    } else {
		push @ntagi, [$tag, undef, []];
	    }
	}
	@tagi= map {
	    my ($prealt, $postalt, $alt) = @$_;
	    $prealt . join('.', @$alt) . $postalt
	    } @ntagi;
    } 
    return @tagi;
}

sub norm_tags {
    my $joined = join('|',@_);
#    print STDERR (defined($normcache{$joined})?"N+: ":"N-: "),"$joined\n";
    $normcache{$joined} = [implode_tags(explode_tags(@_))]
  	unless defined($normcache{$joined});
#    print STDERR "N: $joined -> $normcache{$joined}\n";
    return @{$normcache{$joined}};
    implode_tags(explode_tags(@_))
}

sub sortuniq {
    if(@_==1){
	return @_;
    }else{
	my ($prev, $cur);
	return grep { $prev=$cur; $cur=$_; !($_ eq $prev) }
	sort(@_);
    }
}

our %morfindorder = (
		     '_' => 1,
		     subst => 3,
		     adj => 5,
#		     adj0 => 6,
		     adja => 6,
#		     adv0 => 7,
		     adjp => 7,
		     adv => 8,
		     num => 9,
		     ppron12 => 12,
		     ppron3 => 13,
		     siebie => 14,
		     fin => 15,
		     bedzie => 16,
		     aglt => 17,
		     praet => 18,
		     impt => 19,
		     imps => 20,
		     inf => 21,
		     pcon => 22,
		     pant => 23,
		     ger => 24,
		     pact => 25,
		     ppas => 26,
		     winien => 27,
		     pred => 28,
		     prep => 35,
		     conj => 36,
		     qub => 37,
		     xxx => 38,
		     xxs => 39,
    comp => 41,
    burk => 40,
    advndm => 43,
    intrj => 42,
		     sg => 50,
		     pl => 51,
		     nom => 60,
		     gen => 61,
		     dat => 62,
		     acc => 63,
		     inst => 64,
		     loc => 65,
		     voc => 66,
		     m1 => 69,
		     m2 => 70,
		     m3 => 71,
		     f => 72,
		     n1 => 73,
		     n2 => 74,
		     p1 => 75,
		     p2 => 76,
		     p3 => 77,
		     pri => 78,
		     sec => 79,
		     ter => 80,
		     pos => 81,
		     com => 82,
		     sup => 83,
		     imperf => 84,
		     perf => 85,
		     aff => 86,
		     neg => 87,
		     ndepr => 88,
		     depr => 89,
		     akc => 90,
		     nakc => 91,
		     praep => 92,
		     npraep => 93,
		     congr => 94,
		     rec => 95,
		     agl => 96,
		     nagl => 97,
		     wok => 98,
		     nwok => 99,
		     uni => 200,
		     m => 202,
		     '-m1' => 203,
		     n => 204,
		     '' => 1000
		     );
our %tagcache = ();

sub morfindcmp {
#    print STDERR "'$a' morfindcmp '$b'\n";
    die "Nieznana wartość kategorii gramatycznej $a"
	unless defined($morfindorder{$a});
    die "Nieznana wartość kategorii gramatycznej $b"
	unless defined($morfindorder{$b});
    return $morfindorder{$a} <=> $morfindorder{$b};
};

sub computetagval {
    my $tag = shift;
    my $mult = 100**7;
    my $val  = 0;
    for my $cat (split ':', $tag) {
	die "Nieznana wartość kategorii gramatycznej '$cat'"
	    unless defined($morfindorder{$cat});
	$mult /= 100;
	$val += $mult*$morfindorder{$cat};
#	print STDERR "$cat $mult $morfindorder{$cat}\n";
    }
    $tagcache{$tag} = $val;
#    print STDERR "$tag $val\n";
}

sub tagcmp {
    computetagval($a) unless defined($tagcache{$a});
    computetagval($b) unless defined($tagcache{$b});
    return $tagcache{$a} <=> $tagcache{$b};
    }

sub tagsortuniq {
    if(@_==1){
	return @_;
    } else {
	my ($prev, $cur);
	return grep { $prev=$cur; $cur=$_; !($_ eq $prev) }
	sort tagcmp @_;
    }
}


1;
