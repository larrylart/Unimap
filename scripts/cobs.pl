#!/bin/perl

use HTML::Entities;
use Text::Unidecode qw(unidecode);
 
####
# Earth's equatorial radius in metres.
our $EQU_RAD = 6378100;

# Earth's flattening parameter (actually 1-f).
our $E = 0.996647186;

# Related to flattening parameter (sqrt(1-(1-f)^2)).
our $EPS = 0.081819221;

# Pi.
our $PI = 3.1415926535897932384626433832795;

# AU to metre conversion factor.
our $AU2METRE = 149598000000;

# rad to deg
our $RAD2DEG = 57.295779513082320876798154814105;
#deg to rad
our $DEG2RAD = 0.017453292;


##############
# Perl trim function to remove whitespace from the start and end of the string
sub trim($)
{
  my $string = shift;
  $string =~ s/^\s+//;
  $string =~ s/^\t+//;
  $string =~ s/\s+$//;
  $string =~ s/\t+$//;
  return $string;
}

########################
# Convert geocentric latitude and distance from centre of Earth to
# geodetic latitude and altitude.
# Returns latitude in radians and altitude in metres.
sub GeocToGeod 
{
	my( $geoc_lat, $geoc_dist ) = @_;
	#my $geoc_dist = shift;

	my $t_lat = sin( $geoc_lat ) / cos( $geoc_lat );
	my $x_alpha = $E * $EQU_RAD / sqrt( $t_lat * $t_lat + $E * $E );
	my $mu_alpha = atan2( sqrt( $EQU_RAD * $EQU_RAD - $x_alpha * $x_alpha ), $E * $x_alpha );
	if( $geoc_lat < 0 ) 
	{
		$mu_alpha = 0 - $mu_alpha;
	}
	my $sin_mu_a = sin( $mu_alpha );
	my $delt_lambda = $mu_alpha - $geoc_lat;
	my $r_alpha = $x_alpha / cos( $geoc_lat );
	my $l_point = $geoc_dist - $r_alpha;
	my $alt = $l_point * cos( $delt_lambda );
	my $denom = sqrt( 1 - $EPS * $EPS * $sin_mu_a * $sin_mu_a );
	my $rho_alpha = $EQU_RAD * ( 1 - $EPS ) / ( $denom * $denom * $denom );
	my $delt_mu = atan2( $l_point * sin( $delt_lambda ), $rho_alpha + $alt );
	my $geod_lat = $mu_alpha - $delt_mu;
	my $lambda_sl = atan2( $E * $E * sin( $geod_lat ) / cos( $geod_lat ), 1 );
	my $sin_lambda_sl = sin( $lambda_sl );
	my $sea_level_r = sqrt( $EQU_RAD * $EQU_RAD / ( 1 + ( ( 1 / ( $E * $E ) ) - 1 ) * $sin_lambda_sl * $sin_lambda_sl ) );

	return ( $geod_lat, $alt );
}

#########################
# Convert parallax constants to geocentric latitude and distance from
# centre of Earth.
sub ParToGeoc 
{
	# the other way around ??
	my( $par_C, $par_S ) = @_;
	#my $par_C = shift;

	my $geoc_lat = atan2( $par_C, $par_S );
	my $geoc_dist = sqrt( $par_S * $par_S + $par_C * $par_C ) * $EQU_RAD;

	
	# convert to degrees
#	my $geoc_lat_deg = $geoc_lat*$RAD2DEG;

	my( $lat_rad, $alt ) = GeocToGeod( $geoc_lat, $geoc_dist );

	# convert to degrees
	my $lat_deg = $lat_rad*$RAD2DEG;

	return( $lat_deg, $alt );
}

################################
open( OBS, "<bobs.txt" );
while( <OBS> )
{
	chomp;
	$_ = trim($_);
	$_ = unidecode(decode_entities($_));

	my $name = "";
	my $web = "";
  
	$code = substr( $_, 0, 3 );
	$lon  = trim(substr( $_, 4, 9 ));
	# adjust lon from 0...360 to -180 W
	if( $lon > 180 ) { $lon = $lon - 360.0; }

	# cos/sin
	$cos =  substr( $_, 14, 8 );
	$sin =  substr( $_, 23, 9 );
	# get lat/erad
	my( $lat, $alt ) = ParToGeoc( $sin, $cos );
  
	# name
	$fname =  substr( $_, 33 );
	# process names
    if( $fname =~ m/<a href=\"*(.*?)\"*>(.*?)<\/a>/ )
    {
    	$name = $2;
  		$web =$1;
  		
    } else
    {
    	$name = $fname;
    }
  
	# print code block
	print "\n[$name]\n";
 
	print "Code=$code\n";
	print "Lat=$lat\n";
	print "Lon=$lon\n";
	print "Alt=$alt\n"; 
	if( $web ne "" ) { print "Web=$web\n"; }
	#print "Ini=observatory/$code.ini\n"; 
 	
	#debug
	#print "sin=$sin\n";
	#print "cos=$cos\n";  
  
}
close( OBS );