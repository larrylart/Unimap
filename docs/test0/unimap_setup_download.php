<?php

$file_version = "unimap_setup_0_0_3";
if( isset($_GET['ver']) ) $file_version = $_GET['ver'];

require_once('lib/online_func.php');

//echo getcwd() . "<br>\n";

// main call
download_file( "download/", $file_version . ".exe" );

