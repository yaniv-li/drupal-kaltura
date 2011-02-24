<?php
// $Id: kaltura.notification.inc,v 1.1.2.5.2.1 2010/11/17 01:14:57 xuriz Exp $

/*
 * @file
 * contains functions for handling all notifications from kaltura
 *
 */

/*
 * This is the calback function for the kaltura/notification_handler URL
 *
 * This function uses the KalturaNotificationClient class to normalize the received notifications
 * The mentioned class also validates the notification signature, to prevent malicious data injection
 *
 * Each notification is save in the notifications table, so in case of duplicate notification only
 * the first notificaion will be handled
 *
 * This function also invokes the hook_notification_handler so other modules that want to act upon
 * notification is received will be able to do so.
 */
function kaltura_notification_handler() {
  $admin_secret = variable_get('kaltura_admin_secret', '');
  //$params = $_REQUEST;
  $params = $_POST;
 /*   foreach($_POST as $key => $val)
  {
	error_log($key . "=>" . $val);
  }*/
  watchdog('kaltura NH', print_r($params, TRUE));
  unset($params['q']);
  $noti = new KalturaNotificationClient($params, $admin_secret);
  if ($noti->valid_signature === NULL) {
    watchdog('kaltura', 'notification params empty');
    return FALSE;
  }
  elseif ($noti->valid_signature === FALSE) {
    watchdog('kaltura', 'notification signature not valid');
    return FALSE;
  }
  else {
    watchdog('kaltura', 'hooray!!! notification signature valid');
  }
  echo 'OK';
  foreach ($noti->data as $notification_data) {
    $times = kaltura_notification_received($notification_data['notification_id']);
    if (!$times || $notification_data['notification_type'] == 'test') {
      kaltura_notification_save($notification_data['notification_id'], $notification_data);
      kaltura_forward_notification($notification_data);
      kaltura_invoke('notification_handler', $notification_data);
    }
    else {
      watchdog('notification', 'This notification ('. $notification_data['notification_id'] .') was already received');
    }
  }
}

/*
 * This function saves the received notification to the DB
 */
function kaltura_notification_save($notification_id, $notification_data) {
  $notify['notification_id'] = $notification_id;
  $notify['data'] = $notification_data;
  $notify['received_at'] = time();
  try {
    db_insert('kaltura_notifications')
      ->fields($notify)
      ->execute();
  }
  catch (Exception $e) {
    watchdog('kaltura', 'problem updateing notifaction table');
  }
}

/*
 * This function checks if the received notification was already handled - to avoid duplicates
 */
function kaltura_notification_received($notification_id) {
  /*
   *$query = 'SELECT COUNT(notification_id) c FROM {kaltura_notifications} WHERE notification_id = %d';
   *$result = db_query($query, $notification_id);
   *$count_obj = db_fetch_object($result);
   */
  $count = db_select('kaltura_notifications', 'n')
    ->fields('n')
    ->condition('notification_id', $notification_id, '=')
    ->countQuery()
    ->execute()
    ->fetchField();
  return $count;
}

/*
 * This function check the notificatio type, and forwards the notification to the appropriate function
 *
 * Some notification types, as described in  kaltura API documentation, are not yet handled here
 */
function kaltura_forward_notification($noti) {
  watchdog('kaltura notification handler', print_r($noti, TRUE));
  switch ($noti['notification_type']) {
    case 'entry_add':
	  if ($noti['media_type'] == 6)
	  {
		kaltura_notify_node_entry_add('mix', $noti);
	  }
	  else
	  {
		kaltura_notify_entry_add('entry', $noti);
	  }
      break;
    case 'kshow_add': kaltura_notify_entry_add('mix', $noti);
      break;
    case 'kshow_update_info': watchdog('KNH', 'notification of type kshow_update_info received<br />'. serialize($noti));
      break;
    case 'entry_update_thumbnail': kaltura_update_entry_thumbnail($noti);
      break;
    case 'entry_update': kaltura_notify_update($noti);
      break;
    case 'test': kaltura_test_notification_received();
      break;
    default:
  }
}

/*
 * helper function to set the notification_status variable to 1
 *
 * should only be called when test notification recieved
 */
function kaltura_test_notification_received() {
  variable_set('kaltura_notification_status', 1);
}

/*
 * helper function to update the thumbnail url of an entry
 */
function kaltura_update_entry_thumbnail($notification) {
  /*
   *$thumburl = $notification['thumbnail_url'];
   *$entry = $notification['entry_id'];
   *$update = 'UPDATE {node_kaltura} SET kaltura_thumbnail_url = \'%s\' WHERE kaltura_entryId = \'%s\'';
   *$result = db_query($update, $thumburl, $entry);
   */
  try {
    db_update('node_kaltura')
      ->fields(array('kaltura_thumbnail_url' => $notification['thumbnail_url']))
      ->condition('kaltura_entryid', $notification['entry_id'])
      ->execute();
  }
  catch (Exception $e) {
    watchdog('kaltura', 'update entry thumbnail failed');
  }
}


function kaltura_notify_update($notification) {
  //dd($notification);
  $entry['kaltura_tags'] =  $notification['tags'];
  $entry['kaltura_admin_tags'] =  $notification['admin_tags'];
  $entry['kstatus'] =  $notification['status'];
  $entry['kaltura_title'] =  $notification['name'];
  try {
    db_insert('node_kaltura')
      ->fields($entry)
      ->execute();
  }
  catch (Exception $e) {
    watchdog('kaltura', 'problem updating recored from notication');
  }

}


/**
 * kaltura_notify_entry_add
 * addes and entry to the kaltura table via notification
 * @param mixed $type
 * @param mixed $notification_data
 * @access public
 * @return void
 */
function kaltura_notify_entry_add($type, $notification_data) {
  $entry = mk_kaltura_array($notification_data);
  $entry['kaltura_created_date'] = REQUEST_TIME; //we have no way when the entry was created on the KMC so the create date is when we received the notifcation
  try {
    db_insert('node_kaltura')
      ->fields($entry)
      ->execute();
  }
  catch (Exception $e) {
    watchdog('kaltura', 'problem adding new recored from notication');
  }
}

/**
 * mk_kaltura_array
 * create  an array of fields to add to kaltura table from the notification array
 * @param mixed $array
 * @access public
 * @return void
 */
function mk_kaltura_array ($array) {
  $ent['kaltura_entryid'] = $array['entry_id'];
  $ent['kaltura_tags'] = $array['tags'];
  $ent['kstatus'] = $array['status'];
  $ent['kaltura_media_type'] = $array['media_type'];
  $ent['kaltura_thumbnail_url'] = $array['thumbnail_url'];
  $ent['kaltura_partner_data'] = $array['partner_data'];
  $ent['kaltura_width'] = $array['width'];
  $ent['kaltura_height'] = $array['height'];
  $ent['kaltura_download_url'] = $array['download_url'];
  $ent['kaltura_title'] = $array['name'];
  return $ent;
}

function kmc_obj_to_drupal_array($obj) {
  $ent['kaltura_entryid'] = $obj->id;
  $ent['kaltura_tags'] = $obj->tags;
  $ent['kaltura_admin_tags'] = $obj->adminTags;
  $ent['kstatus'] = $obj->status;
  $ent['kaltura_media_type'] = $obj->mediaType;
  $ent['kaltura_duration']= $obj->duration;
  $ent['kaltura_thumbnail_url'] = $obj->thumbnailUrl;
  $ent['kaltura_partner_data'] = $obj->partnerData;
  $ent['kaltura_source'] = $obj->sourceType;
  $ent['kaltura_width'] = $obj->width;
  $ent['kaltura_height'] = $obj->height;
  $ent['kaltura_download_url'] = $obj->downloadUrl;
  $ent['kaltura_views'] = $obj->views;
  $ent['kaltura_plays'] = $obj->plays;
  $ent['kaltura_votes'] = $obj->votes;
  $ent['kaltura_rank'] = $obj->rank;
  $ent['kaltura_total_rank'] = $obj->totalRank;
  $ent['kaltura_title'] = $obj->name;
  $ent['kaltura_description'] = $obj->description;
  return $ent;
}


