<?php
// $Id$

/**
 * Declaration of new field handler that extends the basic field handler of views module
 * We want to theme the media date field ourselvs.
 */
class kaltura_views_handler_field_kaltura_media_date extends views_handler_field {

  /**
   * Call out to the theme() function, which probably just calls render() but
   * allows sites to override output fairly easily.
   */
  function theme($data) {
    if ( isset($data->node_kaltura_kaltura_media_date) && $data->node_kaltura_kaltura_media_date != '0000-00-00 00:00:00' ) {
      return $data->node_kaltura_kaltura_media_date ;
    }
  }

}


