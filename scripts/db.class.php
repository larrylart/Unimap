<?php

include_once("settings.php");

class db
{

var $db_type;
var $db_server;
var $db_name;
var $db_user;
var $db_pass;
var $db_persistent;
var $dbh;

////////////////////////////////////////////////////////////////////
function db( )
{
	global $GSETTINGS;

	$this->db_type = 1;
	$this->db_server = $GSETTINGS['db_server'];
	$this->db_name = $GSETTINGS['db_name'];
	$this->db_user = $GSETTINGS['db_user'];
	$this->db_pass = $GSETTINGS['db_pass'];
	$this->db_persistent = 0;
	$this->db_connect();

}

////////////////////////////////////////////////////////////////////
function db_connect ()
{

	// mySQL
	if($this->db_type == 1)
	{
		if ($this->db_persistent)
			$this->dbh = @mysql_pconnect($this->db_server, $this->db_user, $this->db_pass);
		else
			$this->dbh = @mysql_connect($this->db_server, $this->db_user, $this->db_pass);

		if (!$this->dbh)
		{
			printf("Error: Connection to MySQL server '%s' failed.<BR>\n", $this->db_server);
			return;
		}

		if (!@mysql_select_db($this->db_name, $this->dbh))
		{
			printf("Error: Connection to MySQL database '%s' failed.<BR>\n>%s: %s<BR>\n", $this->db_name, @mysql_errno($this->dbh), @mysql_error($this->dbh));
			return;
		}
	}

}

////////////////////////////////////////////////////////////////////
function esc_str( $str )
{
	// mySQL
	if($this->db_type == 1)
	{
		$result = mysql_real_escape_string($str, $this->dbh);
		return $result;
	}

	// if nothing just return the same string
	return( $str );
}


////////////////////////////////////////////////////////////////////
function db_query( $query )
{
	// mySQL
	if($this->db_type == 1)
	{
		$result = mysql_query($query, $this->dbh)
		or die ("Error: A problem was encountered while executing this query=$query");

		return $result;
	}

}

////////////////////////////////////////////////////////////////////
function db_query_id( $query )
{
	$result = mysql_query($query, $this->dbh) or
		die ("Error: A problem was encountered while executing this query=$query");

	return( mysql_insert_id() );
}

////////////////////////////////////////////////////////////////////
function db_numrows ($result)
{

	switch($this->db_type)
	{
		// mysql
		case 1: return mysql_num_rows($result);

	} //end switch

} // end db_numrows()

////////////////////////////////////////////////////////////////////
function db_fetch_array (&$result)
{

	switch($this->db_type)
	{
		//mySQL
		case 1: return mysql_fetch_array($result);
	} //end switch
}

} //end class db

?>


