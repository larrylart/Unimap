#!/usr/bin/perl
# by: Larry Lart 

use lib '/cygdrive/c/home/work/prj_me/unimap/srcipts/';
use DPreview;

my( $strURL ) = 'http://www.dpreview.com/';	

###########################################################
# The Main Routine
###########################################################


my( $pDPreview ) = new DPreview( );
my( %vectBrand ) = 
( 
	"Canon" => "Canon", 
	"Nikon" => "Nikon",
	"Sony"	=> "Sony", 
	"Casio"	=> "Casio",
	"FujiFilm"	=> "FujiFilm",
	"Pentax"	=> "Pentax",
	"Kodak"	=> "Kodak", 
	"Olympus"	=> "Olympus", 
	"Panasonic"	=> "Panasonic",
	"Konica_Minolta"	=> "Konica Minolta", 
	"Samsung"	=> "Samsung", 
	"Toshiba"	=> "Toshiba",
	"Ricoh"	=> "Ricoh",
	"Agfa"	=> "Agfa", 
	"Contax" => "Contax",
	"Epson"	=> "Epson", 
	"HP" => "HP",
	"Kyocera"	=> "Kyocera",
	"Leica"	=> "Leica",
	"Sanyo" => "Sanyo",
	"Sigma"	=> "Sigma"
);

# for each predefined brand
foreach $brand ( sort keys %vectBrand )
{	
	my( %vectCameras );
	my( %vectCamerasTypes );

	$pDPreview->GetCamerasByBrand( $brand, $vectBrand{$brand}, \%vectCameras ); 
		
	# open cameras brands ini file
	open( CAMERAFILE, ">../_data/hardware/camera/camera_def_$brand.ini" );

	print CAMERAFILE "\n############################\n";
	print CAMERAFILE "# $brand\n";
	print CAMERAFILE "############################\n";

	# for all camera in this brand
	foreach $camera ( sort keys %vectCameras )
	{
		print CAMERAFILE "\n[$camera]\n";

		print "$camera = ".$vectCameras{$camera}{FILENAME}."\n";

		##############
		# get camera details
		$pDPreview->GetCameraDetails( $brand, $camera, \%vectCameras );

		###########
		# GET IMAGE
		$pDPreview->GetCameraImageIcon( $camera, \%vectCameras );
		
		#################
		# EXPORT INI FOR EXTERNAL USE
		# :: IMAGE ICON FILE
		print CAMERAFILE "IconImage=".$vectCameras{$camera}{IMGPATHFILE}."\n";
		
		#################
		# :: CAMERA TYPE
		if( $vectCameras{$camera}{DETAILS}{"CamFormat"} =~ m/Interchangeable\ +lens\ +camera/ )
		{
			print CAMERAFILE "Type=DigitalSLR\n";
			$vectCamerasTypes{"DigitalSLR"} = 1;
			
		} elsif( $vectCameras{$camera}{DETAILS}{"CamFormat"} =~ m/Compact/ )
		{
			print CAMERAFILE "Type=Digital\n";
			$vectCamerasTypes{"Digital"} = 1;
			
		} else
		{
			print CAMERAFILE "Type=Digital\n";
			$vectCamerasTypes{"Digital"} = 1;
			#print "Unknown camera format=".$vectCameras{$camera}{DETAILS}{"CamFormat"}."\n";
			#exit;
		}
		
		# :: GET MAX RES
		$vectCameras{$camera}{DETAILS}{"MaxResolution"} =~ m/([0-9]+)\s*?x\s*?([0-9]+)\s*/;
		$MaxResWidth = $1;
		$MaxResHeight = $2;
		print CAMERAFILE "Resolution=$1". "x" . "$2\n";
		
		#################
		# :: SENSOR SIZE
		if( $vectCameras{$camera}{DETAILS}{"SensorSize"} =~ m/([0-9\.]+)\s*?x\s*?([0-9\.]+)\s*?mm/ )
		{
			print CAMERAFILE "SensorWidth=$1\n";
			print CAMERAFILE "SensorHeight=$2\n";
			
			# :: SENSOR PIXEL SIZE
			$nSensPixSize = ($1*1000.0)/$MaxResWidth;
			print CAMERAFILE "SensorPixelSize=$nSensPixSize\n";
		}
		
		################
		# :: SENSOR TYPE
		print CAMERAFILE "SensorType=".$vectCameras{$camera}{DETAILS}{"SensorType"}."\n";
		
		#############
		# :: WEIGTH
		$vectCameras{$camera}{DETAILS}{"Weight"} =~ m/^\s*([0-9\.]+)\s*([gkK]+)/;
		$nWeight = $1;
		$nUnits = $2; 
		$nUnits =~ tr/[A-Z]/[a-z]/;
		if( $nUnits eq "kg" ){ $nWeight = $nWeight*1000.0; }
		print CAMERAFILE "Weight=$nWeight\n";	
		
		##############
		# :: LIVEVIEW
		if( !defined( $vectCameras{$camera}{DETAILS}{"LiveView"} ) ||
			 	$vectCameras{$camera}{DETAILS}{"LiveView"} eq "" ||
			 	$vectCameras{$camera}{DETAILS}{"LiveView"} =~ m/No/ )
		{
			# do nothing
			
		} else
		{
			print CAMERAFILE "LiveView=".$vectCameras{$camera}{DETAILS}{"LiveView"}."\n";
		}


		##############
		# :: LENS THREAD
		if( !defined( $vectCameras{$camera}{DETAILS}{"LensThread"} ) ||
			 	$vectCameras{$camera}{DETAILS}{"LensThread"} eq "" ||
			 	$vectCameras{$camera}{DETAILS}{"LensThread"} =~ m/No/ )
		{
			# do nothing
			
		} else
		{
			print CAMERAFILE "LensThread=".$vectCameras{$camera}{DETAILS}{"LensThread"}."\n";
		}

		##############
		# :: LENS FOCAL
		#print "ZoomWide=".$vectCameras{$camera}{DETAILS}{"ZoomWide"}."\n";
		#print "ZoomTele=".$vectCameras{$camera}{DETAILS}{"ZoomTele"}."\n";
		$nFocalMin = 0; $nFocalMax = 0;
		if( $vectCameras{$camera}{DETAILS}{"ZoomWide"} =~ m/^\s*([0-9\.\+\-]+)\s*?(.*)\s*$/ ){ $nFocalMin = $1; }
		if( $vectCameras{$camera}{DETAILS}{"ZoomTele"} =~ m/^\s*([0-9\.\+\-]+)\s*?(.*)\s*$/ ){ $nFocalMax = $1; }
		if( $nFocalMin != 0 || $nFocalMax != 0 )
		{
			print CAMERAFILE "LensFocal=$nFocalMin:$nFocalMax\n";
			#print "LensFocal=$nFocalMin:$nFocalMax\n";
		}

		##############
		# :: ISO RATINGS 
		print CAMERAFILE "ISO=".$vectCameras{$camera}{DETAILS}{"ISORating"}."\n";

		##############
		# :: SHUTTER
		#print "ZoomWide=".$vectCameras{$camera}{DETAILS}{"ZoomWide"}."\n";
		#print "ZoomTele=".$vectCameras{$camera}{DETAILS}{"ZoomTele"}."\n";
		$nShutterMin = 0; $nShutterMax = 0;
		if( $vectCameras{$camera}{DETAILS}{"MinShutter"} =~ m/^\s*([0-9\/\.]+)\s*?(.*)\s*$/ ){ $nShutterMin = eval( $1 ); }
		if( $vectCameras{$camera}{DETAILS}{"MaxShutter"} =~ m/^\s*([0-9\/\.]+)\s*?(.*)\s*$/ ){ $nShutterMax = eval( $1 ); }
		if( $nShutterMin != 0 || $nShutterMax != 0 )
		{
			print CAMERAFILE "Shutter=$nShutterMin:$nShutterMax\n";
			#print "Shutter=$nShutterMin:$nShutterMax\n";
		}


		##############
		# :: MOVIE MODE 
		if( !defined( $vectCameras{$camera}{DETAILS}{"MovieClips"} ) ||
			 	$vectCameras{$camera}{DETAILS}{"MovieClips"} eq "" ||
			 	$vectCameras{$camera}{DETAILS}{"MovieClips"} =~ m/No/ )
		{
			# do nothing
			
		} else
		{	
			print CAMERAFILE "Movie=".$vectCameras{$camera}{DETAILS}{"MovieClips"}."\n";
		}


		###########
		# :: GET PRICE
		$nPrice = $pDPreview->GetCameraPrice( $vectCameras{$camera}{FILENAME}, \%vectCameras );
		if( $nPrice != 0 ){ print CAMERAFILE "Price=$nPrice(USD)\n"; }

		#########
		# DEBUG
#		foreach $key ( keys %{ $vectCameras{$camera}{DETAILS} } )
#		{
#			print "$key = ".$vectCameras{$camera}{DETAILS}{$key}."\n";
#		}

		# debug - first out
		#exit;
		#close( CAMERAFILE );
		#exit;
	}
	
	close( CAMERAFILE );

	########################
	# WRITE BRAND MAIN FILE
	
	# build types
	$strCamTypes = "";
	foreach $type ( keys %vectCamerasTypes ){ $strCamTypes .= "$type,"; }
	$strCamTypes =~ s/\,$//;

	# open cameras brands ini file
	open( CAMERABRAND, ">>../_data/hardware/camera/camera_brands.ini" );
	print CAMERABRAND "\n[$brand]\n";
	print CAMERABRAND "IniFile=data/hardware/camera/camera_def_$brand.ini\n";
	if( $strCamTypes ne "" ){ print CAMERABRAND "Types=$strCamTypes\n"; }
	close( CAMERABRAND );
	
}


exit;

