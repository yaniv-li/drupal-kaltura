<?php
// $Id: kaltura_views_handler_filter_media_type.inc,v 1.1.2.1 2010/08/09 04:33:33 univate Exp $

/*
 * Declaration of new filter handler class that extends the basic operator class of views module
 * We want to supply our set of options on which we will filter results
 */
class kaltura_views_handler_filter_media_type extends views_handler_filter_in_operator {

  /*
   * we use only one function to supply our own set of values
   */
  function get_value_options() {
    $this->value_options = array( 1 => t('Video'), 2 => t('Image'), 5 => t('Audio'), 6 => t('Show') );
  }

}


