<?php

try
{
	include_once( "TWSparqlHTML.inc" ) ;

	$query = "project.rq" ;
	$xslt = "generate/project-embed.xsl" ;
	$uri = "http://tw.rpi.edu/instances/RDESC" ;
	$sparql = "<sparql query=\"$query\" xslt=\"$xslt\" uri=\"$uri\"/>" ;

	$contents = TWSparql::getEngine()->render( $sparql ) ;

	print( "$contents" ) ;
}
catch( Exception $e )
{
	$msg = $e->getMessage() ;
	print( "$msg\n" ) ;
}

?>
