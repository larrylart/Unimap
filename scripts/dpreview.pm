#!/usr/bin/perl -w
###################################################################
# Package:      DPreview
# Purpose:      download digital cameras data
#
# Created by:   Larry Lart on 20/07/2009
# Updated by:   ...
####################################################################
package DPreview;

use LWP::UserAgent;
use HTTP::Cookies;
use HTTP::Request::Common;
use HTTP::Headers;
use HTML::Entities();
use CGI qw/:standard/;

# global handler
my $ua;

# www definition
my( $urlCamTypeList ) = 'http://www.dpreview.com/reviews/specs/Canon/';
my( $strURL ) = 'http://www.dpreview.com/';

####################################################################
# Method: 	Constructor
####################################################################
sub new
{
	shift;
	my $self = {};
	bless $self;
	$self->initialize(@_);
	return $self;
}

####################################################################
# Method: initialize
####################################################################
sub initialize
{
	$ua = new LWP::UserAgent;
	$my_agent = user_agent();
	$ua->agent($my_agent);
}

####################################################################
# Method:	GetCamerasByBrand
# Package:	DPreview
# Purpose:	Get camera page
# Input:	Nothing
# Output:	Nothing
####################################################################
sub GetCamerasByBrand
{
	my( $this, $brand, $bname, $vectCameras ) = @_; 

	$res = $ua->request(GET 'http://www.dpreview.com/reviews/specs/'.$brand.'/');
	if ($res->is_success)
	{
		# print $res->content . "\n";

		# PROCESS PAGE HERE
		# href="/reviews/specs/Canon/

		$_ = $res->content;
		s/\n//g;
		s/\r//g;
		s/\t/ /g;

		$strContent = $_;

		#print "$_\n";
		# Found the camera names
		s/\<tr\>\<td\ valign\=top\ class\=tdcontentsm\ align\=left\>\<a\ href\=\"\/reviews\/specs\/$brand\/(.*?)\.asp\"\>\<b\>([^\<]*?)\</
			$cname = &ProcCameraName( $2, $bname );	
			$vectCameras->{$cname}{FILENAME} = $1;
								/gex;

		# Discontinued Models
		s/\<li\>\<a\ href\=\"\/reviews\/specs\/$brand\/(.*?)\.asp\"\>([^\<]*?)\</
			$cname = &ProcCameraName( $2, $bname );
			$vectCameras->{$cname}{FILENAME} = $1;
								/gex;


	} else
	{
		print "ERROR :: unable to get camera brand=$brand\n"
	}

}

####################################################################
sub ProcCameraName
{
	my( $camera, $bname ) = @_; 
	$camera =~ s/\s*$bname\s*//g;
	return( $camera );

} 

####################################################################
# Method:	GetCamerasByBrand
# Package:	DPreview
# Purpose:	Get camera page
# Input:	Nothing
# Output:	Nothing
####################################################################
sub GetCameraDetails
{
	my( $this, $brand, $camera, $vectCameras ) = @_; 
	# build my pattern 
	my( %vectProps ) = ( 
		CamFormat => "Format", 
		AliasName => "Also known as",
		ReleaseStatus => "Release Status",
		MaxResolution => "Max resolution",
		LowResolution => "Low resolution",
		ImageRatio => "Image ratio w:h",
		EffectivePixels => "Effective pixels",
		SensorPhotoDetectors => "Sensor photo detectors",
		SensorSize => "Sensor size",
		PixelDensity => "Pixel density",
		SensorType => "Sensor type",
		SensorManufacturer => "Sensor manufacturer",
		ISORating => "ISO rating",
		ZoomWide => "Zoom wide",
		ZoomTele => "Zoom tele",
		DigitalZoom => "Digital zoom",
		ImageStabilization => "Image stabilization",
		AutoFocus => "Auto Focus",
		ManualFocus => "Manual Focus",
		NormalFocusRange => "Normal focus range",
		MacroFocusRange => "Macro focus range",
		WhiteBalanceOverride => "White balance override",
		ApertureRange => "Aperture range",
		MinShutter => "Min shutter",
		MaxShutter => "Max shutter",
		BuiltInFlash => "Built-in Flash",
		FlashRange => "Flash range",
		ExternalFlash => "External flash",
		FlashModes => "Flash modes",
		ExposureCompensation => "Exposure compensation",
		Metering => "Metering",
		AperturePriority => "Aperture priority",
		ShutterPriority => "Shutter priority",
		FocalLengthMultiplier => "Focal length multiplier",
		ContinuousDrive => "Continuous Drive",
		MovieClips => "Movie Clips",
		RemoteControl => "Remote control",
		SelfTimer => "Self-timer",
		TimelapseRecording => "Timelapse recording",
		OrientationSensor => "Orientation sensor",
		StorageTypes => "Storage types",
		StorageIncluded => "Storage included",
		UncompressedFormat => "Uncompressed format",
		QualityLevels => "Quality Levels",
		Viewfinder => "Viewfinder",
		LensThread => "Lens thread",
		LCD => "LCD",
		LCDDots => "LCD Dots",
		LiveView => "Live View",
		USB => "USB",
		HDMI => "HDMI",
		Wireless => "Wireless",
		EnvironmentallySealed => "Environmentally sealed",
		Battery => "Battery",
		Weight => "Weight",
		Dimensions => "Dimensions",
		Notes => "Notes"
		
						);

	$res = $ua->request(GET 'http://www.dpreview.com/reviews/specs/'.$brand.'/'. $vectCameras->{$camera}{FILENAME} .'.asp' );
	if( $res->is_success )
	{
		$_ = $res->content;

		#############
		# PROCESS EOF AND OTHER SPEC CHARS
		s/\n//g;
		s/\r//g;
		s/\t/ /g;
	
		###################
		# GET IMAGE
		if( m/\<td\ class\=\"tdlabelsimpsm\"\ nowrap\>.*?Image.*?\<img\ .*?src\=\"([^\"]*?)\"/ )
		{
			$1 =~ m/^(.*)\/(.*?)$/;
			#print "image url=$1 - $2\n";
			$vectCameras->{$camera}{IMGURL} = $1;
			$vectCameras->{$camera}{IMGFILE} = $2;
			
			#exit;
		}	
	
		##################
		# look for my patterns		
		foreach $key ( keys %vectProps )
		{
			$strPattern = $vectProps{$key};
			if( m/\<td\ class\=\"tdlabelsimpsm\"\ nowrap\>.*?$strPattern[^\<]*?\<\/td\>\<td\ class\=\"tdcontentsm\"\>(.*?)\<\/td\>/ )
			{
				$vectCameras->{$camera}{DETAILS}{$key} = $1;
				$vectCameras->{$camera}{DETAILS}{$key} =~ s/\<[^\>]*?\>//g;
				$vectCameras->{$camera}{DETAILS}{$key} =~ s/\&nbsp\;//g;
				$vectCameras->{$camera}{DETAILS}{$key} =~ s/&sup2;/\^2/g;
				# remove white spaces and tabs at the begining and end
				$vectCameras->{$camera}{DETAILS}{$key} =~ s/^[\s|\t]*//gex;
				$vectCameras->{$camera}{DETAILS}{$key} =~ s/[\s|\t]*$//gex;
				#print "look for $strPattern found=$2\n";

			} else
			{
				print "WARNING :: field=$key not found ... \n";
			}
		}
		
	} else
	{
		print "ERROR :: unable to get camera camera deatils camera=$camera\n"
	}

}

####################################################################
# Method:	GetCameraImageIcon
####################################################################
sub GetCameraImageIcon
{
	my( $this, $camera, $vectCameras ) = @_; 

	$res = $ua->request(GET $vectCameras->{$camera}{IMGURL} . '/' . $vectCameras->{$camera}{IMGFILE} );
	if( $res->is_success )
	{
		# set image file prop
		$vectCameras->{$camera}{IMGPATHFILE} = "data/hardware/camera/icons/" . $vectCameras->{$camera}{IMGFILE};
		# write image to file	
		open( IMGFILE, ">../" . $vectCameras->{$camera}{IMGPATHFILE} );
		print IMGFILE $res->content;
		close( IMGFILE );
	}	
}


####################################################################
# Method:	GetCameraPrice
####################################################################
sub GetCameraPrice
{
	my( $this, $camera, $vectCameras ) = @_; 
	my( @vectPrices );
	my( $nTotalPrice ) = 0;
	my( $nNoOfPrices ) = 0;
	my( $nAverage ) = 0;

#	print "camera_price_for=$camera\n";
	$res = $ua->request(GET 'http://www.dpreview.com/products/endpoints/shopdata.ashx?dp=' . $camera );
	if( $res->is_success )
	{
		$_ = $res->content;

		#############
		# PROCESS EOF AND OTHER SPEC CHARS
		s/\n//g;
		s/\r//g;
		s/\t/ /g;
	
		###################
		# GET PRICES :: GBP, USD, 
		s/\"price\"\:\{\"show\"\:true,\"amount\"\:([0-9\.]+)\,\"nation\"\:\"USD\",.*?\"isKit\"\:false\,\"isAcc\"\:false\,\"isRefurb\":false\}/
#			print "prince=$1\n";
#			push( @vectPrices, $1 );
			$nTotalPrice += $1;
			$nNoOfPrices++;

			/gex;
		
#		$nPrices = scalar(@vectPrices);
		if( $nNoOfPrices > 0 )
		{
			$nAverage = $nTotalPrice/$nNoOfPrices;
		}
		
#		print "average=".$nAverage."\n";
		
	} else
	{
		"Error :: failed to request price data\n";
	}
	
#	exit;
	return( $nAverage );
}

1;
