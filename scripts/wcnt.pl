#!/bin/perl
use Getopt::Std;

my( $total_count ) = 0; 
my %opt=();
getopts( "mwI:", \%opt ) or usage();

####################
# help usage routine
####################
sub usage
{
  print STDERR "wcnt usage: arg -I <input file> -m -w\n";
  exit;
}

####################
# process count words or chars
sub do_count
{
	$line = shift;

	# check for empty lines 
	if( length($line) == 0 ){ return; }

  	# check by switch type - let say default is word count -w, -m char count
  	if( $opt{m} )
  	{
  		$total_count += length($_);
  		
  	} else
  	{
  		# do chomp to clear eol - not to be considered in this version - you could ignore
  		chomp( $line );
  		# split line by space or tabs and set to array
  		my @words = split(/[ \t]+/, $line);
  		# now set the number of elements
  		$total_count += @words;

  	}	
}

########
# check type of input - if -I the use file
if( $opt{I} )
{
  print "use input:".$opt{I}."\n";
  $file = $opt{I};
  # read file
  open( DATA, "<$file" ) or die "ERROR :: cannot open file=$file\n";
  while( <DATA> )
  {
  	&do_count($_);
  }
  close( DATA );

} else
{
  print "use standard input\n";
  
  # read lines in array
  @std_input = <STDIN>;
  foreach( @std_input )
  {
    &do_count( $_ );
  }

}

if( $opt{m} )
{
	print "Result count chars = $total_count\n"
	
} else
{
	print "Result count words = $total_count\n"
}



