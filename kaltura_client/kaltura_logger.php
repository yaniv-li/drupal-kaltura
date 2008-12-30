<?php
class KalturaLogger { //implements IKalturaLogger {
	function log($str) {
		watchdog('logger', 'hello');
		$klog = fopen(drupal_get_path('module', 'kaltura') .'/kaltura.log', 'a');
		if (!$klog) watchdog('kaltura client', $str);
		else {
		    fwrite($klog, $str . PHP_EOL);
		    fclose($klog);
		}
	}
}
?>