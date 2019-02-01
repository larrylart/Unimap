<?php

ini_set("default_socket_timeout", 600);

try
{
	$client = new SoapClient("unimap_service.wsdl", array('connection_timeout' => 600, 'trace' => true, 'exceptions' => true, 'cache_wsdl' => 0));
	//$client = new SoapClient("http://192.168.10.193/soaps.php?wsdl", array('trace' => true, /*'exceptions' => true,*/ 'cache_wsdl' => 0));

	//$client->__setSoapHeaders(array($s_header));

	$vectMsg = array();
	$vectMsg['ImagePath'] =  "C:/tmp/img/";
	$vectMsg['ImageName'] =  "uma_a.bmp";
	// SAO, HIPPARCOS, TYCHO2, USNOB1, NOMAD, 2MASS, GSC
	$vectMsg['SearchCatalog'] =  "HIPPARCOS";
	//$vectOut = array();

	//$vectData = $client->ImageSolveRequest($vectMsg);
	$vectData = $client->ImageSolve($vectMsg);
	
	//$vectMsg['ScopeId'] =  "11";
	//$vectData = $client->ScopePositionGet($vectMsg);
	//var_dump($vectOut);
	
}catch (SoapFault $e) {

	print "_____ data :: errors _____\n";
	var_dump($e);
	print "__________\n";
}



echo "REQUEST HEADERS:\n" . $client->__getLastRequestHeaders() . "\n";

print "_____ request _____\n";
print $client->__getLastRequest();
print "__________\n\n";

print "_____ response _____\n";
print $client->__getLastResponse();
print "__________\n\n";

// check if data returned is ok
if( !$vectData )
{
	print "error :: no data returned=$vectData\n";
	//return(NULL);
}


print "_____ data :: credit request _____\n";
var_dump($vectData);
print "__________\n";


?>