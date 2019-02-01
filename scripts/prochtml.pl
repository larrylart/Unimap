#!/bin/perl
# by: Larry Lart

open( DATA, "<new_star_names_html.txt" ) or die "cannot open file\n";

$file = "";
while( <DATA> )
{
	#chomp;
	#print $_;
	$file .= $_;	
}


close( DATA );

my( %names );

$file =~ s/<tr.*?><td.*?>.*?<a.*?>(.*?)<\/a>.*?<\/td>.*?<td.*?>(.*?)<\/td>.*?<td.*?>(.*?)<\/td>.*?<td.*?>(.*?)<\/td>.*?<td.*?>(.*?)<\/td>.*?<td.*?>(.*?)<\/td>.*?<td.*?>(.*?)<\/td>.*?<td.*?>(.*?)<\/td>.*?<td.*?>(.*?)<\/td>.*?<td.*?>(.*?)<\/td>.*?<td.*?>(.*?)<\/td>.*?<\/tr>/

	#print "$1 | $2 | $3 | $4 | $5 | $6 | $7 | $8 | $9 | $10 | $11 | $12 \n";
	#print "$1 | $8 | $9 \n";
	$names{$1}{RA} = $8;
	$names{$1}{DEC} = $9;
	$names{$1}{SAO} = $3;

/gexs;


foreach $key ( sort keys %names )
{
	
	$rad = 0.0;
	$ra = $names{$key}{RA};
	if( $ra =~ m/([0-9\+\-]+).*?([0-9\+\-\.]+)/ )
	{
		$h=$1;
		$m=$2;
		$rad = $h*15.0 + $m/4.0;
	}
	
	# declination
	$decd = 0.0;
	$dec = $names{$key}{DEC};
	if( $dec =~ m/([0-9\+\-]+).*?([0-9\+\-\.]+)/ )
	{
		$h=$1;
		$m=$2;
	
		$sign=1;
		if( $h<0 || $dec =~ m/^[\s\t]*\-/ ) { $sign=-1; $h=-$h }
		$decd = $sign*($h + $m/60.0);
	}	
	
	$sao = $names{$key}{SAO};
	
	print "$key=$rad,$decd,$sao\n";
	
	#print $key . "=" . $names{$key}{RA} . " | ". $names{$key}{DEC} ." \n";
}

