<?php
////////////////////////////////////////////////////////////////////
// class:	CCelestronSite
// purpose:	parse celestron site
// Created by:	Larry Lart
////////////////////////////////////////////////////////////////////

class CCelestronSite
{

// class params
var $strUrl = 'http://www.celestron.com/c3/category.php?CatID=2';
var $r;
var $m_strScopeBrand = 'celestron';
var $m_strImgPath = '';
var $m_strScopeDefFile = '';

// contructor
////////////////////////////////////////////////////////////////////
public function __construct( $strDataPath='telescopes/' )
{
	$this->m_strDataPath = $strDataPath;
	$this->m_strImgPath = $this->m_strDataPath . 'img/' .
			$this->m_strScopeBrand . '/';
	$this->m_strScopeDefFile = $this->m_strDataPath .
			'telescope_def_' . $this->m_strScopeBrand . '.ini';

	// init - get first page to set cookies
	$this->Init();

	return( $this );
}

// Init
////////////////////////////////////////////////////////////////////
function Init()
{
	$this->r = new HttpRequest( 'http://www.celestron.com/', HttpRequest::METH_GET );
	$this->r->setHeaders(array('User-Agent' => 'Mozilla/3.0'));
	$this->r->setOptions(array('redirect' => 10));
	$this->r->enableCookies();
}

// get telescopes
////////////////////////////////////////////////////////////////////
function GetTelescopes()
{
	///////
	// get telescopes categories
	$vectTelCat = $this->GetTelescopesCat();
	// for each categorie
	foreach( $vectTelCat as $cat_id => $cat_name )
	{
		print "---  $cat_name =  $cat_id\n";

		// get scoped in cat
		$vectCatTel = $this->GetCatTelescopes( $cat_id );
		// for each scope in cat
		foreach( $vectCatTel as $tel_id => $tel_name )
		{
			print "[$tel_name :: $tel_id]\n";

			// get scope details
			$vectSpec = $this->GetTelescopeSpec( $cat_id, $tel_id );

			// for each telescope detail - create telescope definition
			$fileSpec = fopen( $this->m_strScopeDefFile, "a+" );
			fwrite( $fileSpec, "[$tel_name]\n" );
			fwrite( $fileSpec, "Class=$cat_name\n" );
			foreach( $vectSpec as $spec_name => $spec_value )
			{
				$strLine = "$spec_name=$spec_value\n";
				fwrite( $fileSpec, $strLine );
				// debug
				print "$spec_name=$spec_value\n";
			}
			fwrite( $fileSpec, "\n" );
			fclose( $fileSpec );

			//exit;
		}

		//exit;
	}

//	print $strPage;

}

//

////////////////////////////////////////////////////////////////////
// get telescopes categories
////////////////////////////////////////////////////////////////////
function GetTelescopeSpec( $nCat, $nProd )
{
	$vectSpec =  array();

	///////
	// get telescopes categories
	$strPage = $this->DoHttpGet( 'http://www.celestron.com/c3/product.php?CatID='.$nCat.'&ProdID=' . $nProd );

//	print $strPage;

	// process html page
	$pat = '/<div class\="lineLabel">([^\:]*?)\:.*?<div class\="lineValue">(.*?)<\/div>/s';
	$nFound = preg_match_all( $pat, $strPage, $matches, PREG_SET_ORDER );

	if( $nFound )
	{
		foreach ($matches as $val)
		{
			//$spec_name = trim(html_entity_decode($val[1]));
			//$spec_value = trim(html_entity_decode($val[2]));
			// remove html tags
			$spec_name = trim( preg_replace( '/<[^<]*?>/s', '', trim(html_entity_decode($val[1])) ) );
			$spec_value = trim( preg_replace( '/<[^<]*?>/s', '', trim(html_entity_decode($val[2])) ) );

			// map fields
			if( $spec_name == 'Aperture' )
				$spec_name  ='ClearAperture';
			elseif( $spec_name == 'Highest Useful Magnification' )
				$spec_name = 'MaxVisualPower';
			else
				$spec_name = preg_replace( '/\s/s', '', $spec_name );

			$vectSpec[$spec_name] = $spec_value;

			// debug
			//print $spec_name . " = " . $spec_value . "\n";

		}

	} else
	{
		// todo: flag here if nothing found
		return( NULL );
	}

	// get image
	// <div class="product_image">
	$pat = '/<div class\="product_image">.*?<img src\="([^"]*?)".*?>/s';
	if( preg_match( $pat, $strPage, $matches ) )
	{
		$strImgFile = $matches[1];
		$strImgUrl = 'http://www.celestron.com/c3/' . $strImgFile;
		$dataImg = $this->DoHttpGet( $strImgUrl );
		// write image to file
		$imgFile = fopen( $this->m_strImgPath . basename($strImgFile), "wb" );
		fwrite( $imgFile, $dataImg );
		fclose( $imgFile );
		// add this to properties
		$vectSpec['ScopePhoto'] = 'img/' . $this->m_strScopeBrand . '/' . basename($strImgFile);
	}

	return( $vectSpec );
}

////////////////////////////////////////////////////////////////////
// get telescopes categories
////////////////////////////////////////////////////////////////////
function GetCatTelescopes( $nCat )
{
	$vectTel =  array();

	///////
	// get telescopes categories
	$strPage = $this->DoHttpGet( 'http://www.celestron.com/c3/category.php?CatID=' . $nCat );

//	print $strPage;

	// process html page
	$pat = '/<a class\="PageItem" id\="pageItem[0-9]*?" href\="product\.php\?CatID\=([0-9]*?)\&amp\;ProdID\=([0-9]*?)">([^<]*?)<\/a>/s';
	$nFound = preg_match_all( $pat, $strPage, $matches, PREG_SET_ORDER );

	if( $nFound )
	{
		foreach ($matches as $val)
		{
			$cat_id = trim($val[1]);

			$tel_id = trim(html_entity_decode($val[2]));
			$tel_name = trim(html_entity_decode($val[3]));

			// debug
			//print $tel_name . " = " . $tel_id . "\n";

			$vectTel[$tel_id] = $tel_name;
		}

	} else
	{
		// todo: flag here if we are not logged in ??
		return( NULL );
	}

	return( $vectTel );
}

// get telescopes categories
////////////////////////////////////////////////////////////////////
function GetTelescopesCat()
{
	$vectCat =  array();

	///////
	// get telescopes categories
	$strPage = $this->DoHttpGet( 'http://www.celestron.com/c3/category.php?CatID=2' );

	// process html page
	$pat = '/' .
				'<a class\="PageItem" id\="pageItem[0-9]*?" href\="category\.php\?CatID\=([0-9]*?)">([^<]*?)<\/a>.*?' .
				'/s';
	$nFound = preg_match_all( $pat, $strPage, $matches, PREG_SET_ORDER );

	if( $nFound )
	{
		foreach ($matches as $val)
		{
			$cat_id = trim(html_entity_decode($val[1]));
			$cat_name = trim(html_entity_decode($val[2]));

			// debug
			//print $cat_name . " = " . $cat_id . "\n";

			$vectCat[$cat_id] = $cat_name;
		}

	} else
	{
		// todo: flag here if we are not logged in ??
		return( NULL );
	}

	return( $vectCat );
}

////////////////////////////////////////////////////////////////////
// http get
////////////////////////////////////////////////////////////////////
function DoHttpGet( $strUrl )
{
	$strPage = '';

	$this->r->setUrl( $strUrl );
	$this->r->setMethod( HttpRequest::METH_GET );

	try
	{
		$strPage = $this->r->send()->getBody();

	} catch (Exception $e)
	{
		echo 'SITE :: Caught exception: ',  $e->getMessage(), "\n";
		exit(0);
	}

//	print $strPage;

	return( $strPage );
}

// end of class
}

?>

