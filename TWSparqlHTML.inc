<?php

/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

include_once('common/TWSparql.inc');

/**
 * Defines a logger object for TW Sparql module to provide feedback to
 * users specific to Drupal.
 * @author pattoe 
 * @author pwest
 */
class TWSparqlHTMLLogger implements TWSparqlLogger {
  public $debugEnabled = FALSE ;
  private $logHandle = NULL ;
  private $errorHandle = NULL ;

  public static function init() {
    TWSparql::$logger = new TWSparqlHTMLLogger();
  }
  
  /**
   * Return whether debugging is turned on or not
   */
  public function shouldDebug() {
    return $this->debugEnabled ;
  }

  /**
   * Set whether or not we should debug
   */
  public function enableDebug($val) {
	// if already turned on and turning on, nothing to do
	if( $this->debugEnabled && $val )
	{
		return ;
	}
	// if already turned off and turning off, nothing to do
	if( !$this->debugEnabled && !$val )
	{
		return ;
	}
	// if already turned on and turning off, then close the log file
	if( $this->debugEnabled && !$val )
	{
		if( $this->logHandle )
		{
			fclose( $this->logHandle ) ;
			$this->logHandle = NULL ;
		}
		if( $this->errorHandle )
		{
			fclose( $this->errorHandle ) ;
			$this->errorHandle = NULL ;
		}
	}
	else
	{
		// if already turned off and turning on, then open the log file
		if( $this->logHandle )
		{
			fclose( $this->logHandle ) ;
		}
		$this->logHandle = fopen( "/tmp/twsparql.log", "a+" ) ;
		if( $this->errorHandle )
		{
			fclose( $this->errorHandle ) ;
		}
		$this->errorHandle = fopen( "/tmp/twsparql_error.log", "a+" ) ;
	}

	$this->debugEnabled = $val ;
  }

  /**
   * Logs a string as an error.
   * @param string $str String to log to the error console. 
   */
  public function logError($str) {
	if( $this->shouldDebug() ) {
	  fwrite( $this->errorHandle, $str . "\n" ) ;
	}
  }
  
  /**
   * Logs a string as a warning
   * @param string $str String to log to the warning console. 
   */
  public function logWarning($str) {
	if( $this->shouldDebug() ) {
	  fwrite( $this->logHandle, $str . "\n" ) ;
	}
  }
  
  /**
   * Logs a string as a status
   * @param string $str String to log to the status console. 
   */
  public function logStatus($str) {
	if( $this->shouldDebug() ) {
	  fwrite( $this->logHandle, $str . "\n" ) ;
	}
  }
}

/**
 * Defines Drupal specific cache object for the TWSparql module to cache intermediate and final results of a query.
 * @author pattoe
 * @author pwest
 */
class TWSparqlHTMLCache implements TWSparqlCache {
  public static function init() {
    TWSparql::$cache = new TWSparqlHTMLCache();
  }
  
  /**
   * Caches a query document for a particular URI
   * @param string $queryUri A URI that 'executes' the query file to get
   * the query text
   * @param string $results The query text returned by dereferencing $queryUri 
   */
  public function cacheQuery($queryUri, $results) {
  }
  
  /**
   * Caches a result for a particular query URI.
   * @param string $queryUri A URI that queries a SPARQL endpoint and returns
   * an XML document.
   * @param string $results The XML content returned by dereferencing $queryUri 
   */
  public function cacheRDF($queryUri, $results) {
  }
  
  /**
   * Caches an XSLT document for a particular URI.
   * @param string $xsltUri The URI for the XSL stylesheet.
   * @param string $xslt The XSL stylesheet content dereferenced by $xsltUri
   * @param string $expires An optional duration to wait before expiring the 
   * document; defaults to TWSparql::getEngine()->getCacheTTL()
   */
  public function cacheXSLT($xsltUri, $xslt, $expires=NULL) {
  }
  
  /**
   * Caches the final transformation of a query using an XSL template.
   * @param string $queryUri The URI used to generate the SPARQL results
   * @param string $xsltUri The URI used to dereference the XSL stylesheet
   * @param string $result The final result applying the transformation named by
   * $xsltUri to the document returned by deferencing $queryUri.
   * @param string $expires An optional duration to wait before expiring the
   * result; defaults to TWSparql::getEngine()->getCacheTTL()
   */
  public function cacheFinal($queryUri, $xsltUri, $result, $expires=NULL) {
  }
  
  /**
   * Checks the cache to determine whether the results of dereferencing 
   * $queryUri are available.
   * @param string $queryUri The URI to check for cached content. 
   */
  public function isQueryCached($queryUri) {
    return false ;
  }
  public function isRDFCached($queryUri) {
	return false ;
  }

  public function isXSLTCached($xsltUri) {
	return false ;
  }

  public function isFinalCached($queryUri, $xsltUri) {
	return false ;
  }
  
  /**
   * Clears the cached copy of $queryUri, if any.
   * @param string $queryUri 
   */
  public function clearQueryCache($queryUri) {
  }

  public function clearRDFCache($queryUri) {
  }

  public function clearXSLTCache($xsltUri) {
  }

  public function clearFinalCache($queryUri, $xsltUri) {
  }
  
  /**
   * Gets the cached content for $queryUri, or NULL if no content is available.
   * @param string $queryUri 
   */
  public function getCachedQuery($queryUri) {
    return NULL ;
  }

  public function getCachedRDF($queryUri) {
	return NULL ;
  }

  public function getCachedXSLT($xlstUri) {
	return NULL ;
  }

  public function getCachedFinal($queryUri, $xsltUri) {
	return NULL ;
  }
}

class TWSparqlHTML extends TWSparql {
  private $phpExtensions ;
  private $iBase ;
  private $sBase ;
  private $queryPath ;
  private $xsltPath ;
  private $endpoint ;

  public static function init( $conf_file ) {
    TWSparql::$engine = new TWSparqlHTML();
	if( $conf_file == null || !file_exists( $conf_file ) )
	{
		throw new Exception( "TWSparql specified configuration file does not exist" );
	}
	$ini_array = parse_ini_file( $conf_file, true ) ;
	$missing = TWSparqlHTML::verify_config( $ini_array ) ;
	if( $missing != "" )
	{
	  throw new Exception( "TWSparql configuration file contains errors" ) ;
	}
	TWSparql::getEngine()->setIbase( $ini_array["iBase"] ) ;
	TWSparql::getEngine()->setSbase( $ini_array["sBase"] ) ;
	TWSparql::getEngine()->setXsltPath( $ini_array["xsltPath"] ) ;
	TWSparql::getEngine()->setQueryPath( $ini_array["queryPath"] ) ;
	TWSparql::getEngine()->setEndpoint( $ini_array["endpoint"] ) ;
  }

  public static function verify_config( $ini_array )
  {
	$expected = array( "iBase",
					   "sBase",
					   "xsltPath",
					   "queryPath",
					   "endpoint",
	) ;

	$bad = false ;
	$missing = "" ;

	foreach( $expected as $var )
	{
      if( !isset( $ini_array[$var] ) )
      {
        if( $bad ) $missing .= ", " ;
        $missing .= $var ;
        $bad = true ;
      }
	}

	return $missing ;
  }

  
  public function shouldDebug() {
    return TWSparql::getLogger()->shouldDebug() ;
  }
  
  public function enableDebug($val) {
    TWSparql::getLogger()->enableDebug($val) ;
  }
  
  public function shouldEnablePHPExtensions() {
    return $this->phpExtensions ;
  }
  
  public function enablePHPExtensions($val) {
    $this->phpExtensions = $val ;
  }
  
  public function getIbase() {
    return $this->iBase ;
  }
  
  public function setIbase($val) {
    $this->iBase = $val ;
  }
  
  public function getSbase() {
    return $this->sBase ;
  }
  
  public function setSbase($val) {
    $this->sBase = $val ;
  }
  
  public function getXsltPath() {
    return $this->xsltPath ;
  }
  
  public function setXsltPath($val) {
    $this->xsltPath = $val ;
  }
  
  public function getQueryPath() {
    return $this->queryPath ;
  }
  
  public function setQueryPath($val) {
    $this->queryPath = $val ;
  }
  
  public function getDefaultXsl() {
    return "" ;
  }
  
  public function setDefaultXsl($val) {
  }
  
  public function getEndpoint() {
    return $this->endpoint ;
  }
  
  public function setEndpoint($val) {
	$this->endpoint = $val ;
  }
  
  public function getCacheTTL() {
    return "" ;
  }
  
  public function setCacheTTL($val) {
  }
  
  public function getCacheEnabled() {
    return FALSE ;
  }
  
  public function setCacheEnabled($val) {
  }
}

// force the caller to initialize TWSparqlHTML
//TWSparqlHTML::init();
TWSparqlHTMLLogger::init();
TWSparqlHTMLCache::init();

