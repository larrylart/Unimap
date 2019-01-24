<?php

// read a file in chuncks is big
////////////////////////////////////////////////////////////////////
function DReadFile( $pathfile )
{
	$size = intval(sprintf("%u", filesize($pathfile)));

	// If it's a large file, readfile might not be able to do it in one go, so:
	$chunksize = 1 * (1024 * 1024); // how many bytes per chunk
	if($size > $chunksize)
	{
		$handle = fopen( $pathfile, 'rb' );
		$buffer = '';
		while( !feof($handle) )
		{
			$buffer = fread( $handle, $chunksize );
			echo $buffer;
			ob_flush();
			flush();
		}
		fclose( $handle );

	} else
	{
		readfile( $pathfile );
	}
}

// download a file
////////////////////////////////////////////////////////////////////
function download_file($path, $dfile)
{
	$pathfile = getcwd() . '/' . $path . basename($dfile);

	//Check for download request:
	//if(isset($_GET['file']))
	if( $dfile != "" )
	{
		//Make sure there is a file before doing anything
		if( is_file($pathfile) )
		{
			//Below required for IE:
			if( ini_get('zlib.output_compression') )
			{
				ini_set('zlib.output_compression', 'Off');
			}

			//Set Headers:
			header('Pragma: public');
			header('Expires: 0');
			header('Cache-Control: must-revalidate, post-check=0, pre-check=0');
			header('Last-Modified: ' . gmdate('D, d M Y H:i:s', filemtime($pathfile)) . ' GMT');
			header('Content-Type: application/force-download');
			header('Content-Type: application/octet-stream');

//			if( strstr($_SERVER["HTTP_USER_AGENT"], "MSIE") != false )
//				header("Content-Disposition: attachment; filename=" . urlencode(basename($F['FILE']['file_path'])) . '; modification-date="' . date('r', $mtime) . '";');
//			else
//				header("Content-Disposition: attachment; filename=\"" . basename($F['FILE']['file_path']) . '"; modification-date="' . date('r', $mtime) . '";');

			header('Content-Disposition: inline; filename="' . basename($dfile) . '"');
			header('Content-Transfer-Encoding: binary');
			header('Content-Length: ' . filesize($pathfile));
			header('Connection: close');

			set_time_limit(3600);

			DReadFile($pathfile);

			exit();

		} else
		{
			//$this->show_notification('File not found!', 'error');
			print "ERROR :: file not found - $pathfile <br>\n";
		}
	}
}

?>
