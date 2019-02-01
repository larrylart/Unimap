#!/bin/perl

#first load cnames
my( %scnames );


open( DATA, "<new_star_cnames.txt" ) or die "cannot open file\n";

while( <DATA> )
{
	#chomp;
	#print $_;
	if( m/^(.*?)=([^,]+),([^,]+),([0-9]+)$/ )
	{
		#print " $1 $2 $3\n";
		if( $3 != 0 ){ $scnames{$4} = $1; }
	}
}

close( DATA );


open( DATA, "<sao2tyc_names.txt" ) or die "cannot open file\n";

while( <DATA> )
{
	#chomp;
	#print $_;
	
	if( m/^SAO *([0-9]+)\|TYC *([0-9]+)\-([0-9]+)\-([0-9]+)/ )
	{
		print "TYC $2-$3-$4=" . $scnames{$1} . "\n";
	}
}

close( DATA );
