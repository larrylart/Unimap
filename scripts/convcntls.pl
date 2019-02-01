#!/bin/perl

$strNamesFile = shift;
$strDataFile = shift;
$strArtFile = shift;

if( $strNamesFile eq "" || $strDataFile eq "" ){ exit; }

#print " arg names=$strNamesFile, data=$strDataFile\n";
#exit;

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


####################################
# LOAD NAMES
my( %vectNames );
open( NAMES, "<$strNamesFile" );
while( <NAMES> )
{
  chomp;
  $_ = trim($_);
  
   if( !m/^#/ && length($_) > 3 )
  {  
  
    s/^\s*(.+?)[\s\t]+\"([^\"]*)\"[\s\t]+\_\(\"([^\"]+)\"\)/
      $vectNames{"$1"}{"NAME"} = $3;
      if( $2 ne "" ){ $vectNames{"$1"}{"LANGNAME"} = $2; };
      /gex;
  } 
}
close( NAMES );


####################################
# LOAD ART
my( %vectArt );
open( ART, "<$strArtFile" );
while( <ART> )
{
  chomp;
  $_ = trim($_);
  
   if( !m/^#/ && length($_) > 3 )
  {  
    s/^(.+?)[\s\t]+([^\s]*)[\s\t]+([0-9]+)[\s\t]+([0-9]+)[\s\t]+([0-9]+)[\s\t]+([0-9]+)[\s\t]+([0-9]+)[\s\t]+([0-9]+)[\s\t]+([0-9]+)[\s\t]+([0-9]+)[\s\t]+([0-9]+)/
      $vectArt{$1}{"FILE"} = $2;
      $vectArt{$1}{"X1"} = $3;
      $vectArt{$1}{"Y1"} = $4;
      $vectArt{$1}{"HIP1"} = $5;
      
      $vectArt{$1}{"X2"} = $6;
      $vectArt{$1}{"Y2"} = $7;
      $vectArt{$1}{"HIP2"} = $8;
      
      $vectArt{$1}{"X3"} = $9;
      $vectArt{$1}{"Y3"} = $10;
      $vectArt{$1}{"HIP3"} = $11;
    /gex;
  }
}

close( ART );

##################################
# READ HIP CAT
my( %hip );
open( HIP, "</cygdrive/c/home/work/prj_me/unimap/star_catalog/hip_cat.txt" );
while( <HIP> )
{
  chomp;
  s/\s*([0-9]+)\s+([0-9\.\+\-]+)\s+([0-9\.\+\-]+)\s*$/
    $hip{$1}{"ra"}=$2;
    $hip{$1}{"dec"}=$3;
    /gex;
    
   # debug
#   print "no='".$hip{"no"}."' ra='".$hip{"ra"}."' dec='".$hip{"dec"}."'\n"; 
    
}
close( HIP );


#################################
# PROC :: const file
open( CONST, "<$strDataFile" );
while( <CONST> )
{
  chomp;
  $_ = trim($_);

 if( !m/^#/ && length($_) > 3 )
 {

  s/^\s*(.+?)\s+([0-9]+)\s+(.*?)$/
    $cid = $1;
    $no=$2;
    $hip_coord=$3;
    /gex;
  
#    print "obj=$name - coord=$hip_coord\n";

    ########################
    # convert coord from hip to ra/dec

    $newCoord = "";
    $cnt=1;
    @vectCoord = split( /\s|\t/, $hip_coord );
    foreach( @vectCoord )
    {
      if( defined( $hip{$_}{"ra"} ) && defined( $hip{$_}{"dec"} ) )
      {
        $ra = $hip{$_}{"ra"};
        $dec = $hip{$_}{"dec"};
        #print "hip=".$_ . " ra='$ra' dec='$dec' \n";
        $newCoord .= "$ra $dec";
        
        if( $cnt%2 )
        {
          $newCoord .= " ";
        } else
        {
          $newCoord .= ":";
        }
        $cnt++;
      }
    }
    $newCoord =~ s/\:$//;
 
    ##############################
    # convert art coord
    $strArtMap = $vectArt{$cid}{"X1"}." ".$vectArt{$cid}{"Y1"}." (".$hip{$vectArt{$cid}{"HIP1"}}{"ra"}." ".$hip{$vectArt{$cid}{"HIP1"}}{"dec"}.") ".
                $vectArt{$cid}{"X2"}." ".$vectArt{$cid}{"Y2"}." (".$hip{$vectArt{$cid}{"HIP2"}}{"ra"}." ".$hip{$vectArt{$cid}{"HIP2"}}{"dec"}.") ".
                $vectArt{$cid}{"X3"}." ".$vectArt{$cid}{"Y3"}." (".$hip{$vectArt{$cid}{"HIP3"}}{"ra"}." ".$hip{$vectArt{$cid}{"HIP3"}}{"dec"}.")";
    
    
    print "[".$vectNames{"$cid"}{"NAME"}."]\n";
    if( defined( $vectNames{"$cid"}{"LANGNAME"} ) ){ print "LangName=".$vectNames{"$cid"}{"LANGNAME"}."\n"; }
    if( defined( $vectArt{$cid} ) )
    {
      print "ArtworkFile=".$vectArt{$cid}{"FILE"}."\n"; 
      print "ArtworkMap=$strArtMap\n"; 
    }
    print "LINES[0][$no]=$newCoord\n";
    print "\n";
 }
}

close( CONST );

