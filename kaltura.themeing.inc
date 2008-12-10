<?php
// $Id$

/*
 * @file
 * defines all themeing functions for kaltura core module
 *
 */

/*
 * helper function that is called in nodeAPI:alter
 *
 * this function gets the content to be displayed,
 * and returns the content with an embed tag instead of a "kaltura tag"
 *
 * some of the return content is a javascript with handler functions for the buttons on the player
 */
function _kaltura_replace_tags($content, $is_comment = FALSE, $replace_to_thumb = FALSE) {
  global $user, $multibyte;
  $length = drupal_strlen($content);
  
  $found = FALSE;
  preg_match_all('/\[kaltura-widget(.*)\/\]/', $content, $matches);
  $kaltura_tags = array();
  foreach ($matches[0] as $key => $match) {
    if (strpos($match, '/][kal', $split_point) === FALSE) {
      $kaltura_tags[] = $matches[0][$key];
      continue;
    }
    $tags = explode('][', $match);
    $splitted = FALSE;
    if (count($tags)) {
      foreach ($tags as $tag_num => $tag) {
        $splitted = TRUE;
        if ($tag[0] != '[') $tags[$tag_num] = '['. $tag;
        if ($tag[drupal_strlen($tag)-1] != ']') $tags[$tag_num] .= ']';
        $kaltura_tags[] = $tags[$tag_num];
      }
    }
  }
  
  foreach ($kaltura_tags as $kaltura_tag) {
    $found = TRUE;
    // parse the parameters from the tag
    $params = _kaltura_get_params_from_tag($kaltura_tag);
  
    // get the embed options from the params
    $embed_options = _kaltura_get_embed_options($params);
    
    $wid = $embed_options["wid"];
    $width = $embed_options["width"];
    $height = $embed_options["height"];
    $div_id = "kaltura_wrapper_". $wid;
    $thumbnail_div_id = "kaltura_thumbnail_". $wid;
    $player_id = "kaltura_player_". $wid;
    $partner_config = KalturaHelpers::getServiceConfiguration();
    $kaltura_server = (variable_get('kaltura_server_url', ''))? variable_get('kaltura_server_url', ''): KalturaSettings::SERVER_URL;

    if ($replace_to_thumb === TRUE) {
      $html = '<img src="'. $kaltura_server .'/p/'. $partner_config->partnerId .'/sp/'. $partner_config->subPartnerId .'/thumbnail/entry_id/'. $embed_options["media_id"] .'" />';
      $content = str_replace($kaltura_tag, $html, $content);
      continue;
    }
    
    $div_id = "kaltura_wrapper_". $embed_options["media_id"];
    $player_id = "kaltura_player_". $embed_options["media_id"];
    $align = ($embed_options["align"])? 'text-align:'. $embed_options["align"] .'; ': '';
    if ($is_comment) {
      $thumb_div_id = 'kaltura_thumbnail_'. $embed_options["media_id"];
      $onclick_value = 'kaltura_activate_player(\''. $thumb_div_id .'\',\''. $div_id .'\');';
      $img_tag = '<img src="'. $kaltura_server .'/p/'. $partner_config->partnerId .'/sp/'. $partner_config->subPartnerId .'/thumbnail/entry_id/'. $embed_options["media_id"] .'/width/'. $width .'/height/'. $height .'/type/2/bgcolor/000000/crop_provider/wordpress_comment_placeholder" />';
      $comment_div = '<div id="'. $thumb_div_id .'" class="kaltura_hand" onclick="'. $onclick_value .'">';
      $comment_div .= $img_tag .'</div>';
    }
    else {
      $comment_div = '';
    }
    $html = $comment_div .'
      <div id="'. $div_id .'" class="kaltura_wrapper" style="'. (($comment_div != '')? 'display:none;':'') . $align . $embed_options['custom_style'] .'"'. $embed_options['js_events'] .'></div>
      <script type="text/javascript">
              var kaltura_swf = new SWFObject("'. $embed_options["swfUrl"] .'", "'. $player_id .'", "'. $embed_options["width"] .'", "'. $embed_options["height"] .'", "9", "#000000");
              kaltura_swf.addParam("wmode", "opaque");
              kaltura_swf.addParam("flashVars", "'. $embed_options["flashVars"] . (($comment_div != '')? '&autoPlay=true': '') .'");
              kaltura_swf.addParam("allowScriptAccess", "always");
              kaltura_swf.addParam("allowFullScreen", "TRUE");
              kaltura_swf.addParam("allowNetworking", "all");
              kaltura_swf.write("'. $div_id .'");
      </script>
    ';
    
    // rebuild the html with our new code tag 
    $content = str_replace($kaltura_tag, $html, $content);
  }
  
  if ($found && $replace_to_thumb === FALSE) {
    $plugin_url = KalturaHelpers::getKalturaServerUrl();
    $js = '
      <script type="text/javascript">
        function onPlayerAddClick (kshowId,entryId,pd_extraData) {
          if (kshowId && kshowId != -1) 
            kalturaInitModalBox("'. url("kaltura/contribution_wizard/") .'" + kshowId);
          if (entryId && entryId != -1 && "true" == "'. (($embed_options['roughcut'])? 'true': '') .'")
            kalturaInitModalBox("'. url("kaltura/contribution_wizard/") . (($embed_options['roughcut'])? 'entry-': '') .'" + entryId );
        }
  
        function onPlayerEditClick (kshowId,entryId,pd_extraData) {
          if (kshowId && kshowId != -1 && "true" == "'. (($embed_options['kshow'])? 'true': '') .'") 
            kalturaInitModalBox("'. url("kaltura/simple_editor/") .'" + kshowId + "/kshow/user_id@'. $user->uid .'", { width: 890, height: 546 } );
          if (entryId && entryId != -1 && "true" == "'. (($embed_options['roughcut'])? 'true': '') .'")
            kalturaInitModalBox("'. url("kaltura/simple_editor/") .'" + entryId + "/entry/user_id@'. $user->uid .'", { width: 890, height: 546 } );
        }
  
        function handleGotoContribWizard (kshowId, pd_extraData) {
          kalturaInitModalBox("'. url("kaltura/contribution_wizard/") .'" + kshowId);
        }
  
        function handleGotoEditorWindow (kshowId, pd_extraData) {
          kalturaInitModalBox("'. url("kaltura/simple_editor/") .'" + kshowId, { width: 890, height: 546 } );
        }
      </script>';
  
    $content .= $js;
  }
  
  return $content;
}

/*
 * helper function that breaks the "kaltura tag" into parameters
 */
function _kaltura_get_params_from_tag($tag) {
  $params = array();
  $attributes_array = explode(' ', $tag);
  for ($i = 1, $len = count($attributes_array); $i < $len; $i++) {
    $attr = $attributes_array[$i];
    if (!strpos($attr, "=")) {
      continue;
    }
    $attr = str_replace('"', "", $attr);
    $attr = str_replace("'", "", $attr);
    $keyvalue = explode('=', $attr);
    $key = $keyvalue[0];
    $value = $keyvalue[1];
    $params[$key] = $value;
  }
  return $params;
}

/*
 * callback function for kaltura/contribution_wizard
 *
 * Args is url should have the following structure
 * key1@value1|key2@value2|key3@value3 ...
 *
 * Example:
 * kaltura/contribution_wizard/kshow_id@-2|partner_data@some partner data|type@entry|context@field|field_id@edit-field-remix[0]-value|title@potato soup recipe
 *
 * valid list of params: (required params marked with *, values in brackets)
 *   kshow_id* [-2, -1, 0, real_id]
 *   partner_data [free text]
 *   type [entry/null]
 *   context [field/field_mix/comment/null]
 *   field_id
 *   title
 */
function kaltura_contribution_wizard() {
  $args = func_get_args();
  /* Old arguments handling...
  $kshow_id = $args[0];
  $partner_data = ($args[1] != 'no_partner_data')? $args[1]: '';
  $type = $args[2];
  $is_field = (isset($args[3]) && $args[3] == 'field')? TRUE: FALSE;
  $remix_field = (isset($args[3]) && $args[3] == 'field_mix')? TRUE: FALSE;
  $is_comment = (isset($args[3]) && $args[3] == 'comment')? TRUE: FALSE;
  */
  // argument handling
  $temp_args = explode('^', $args[0]);
  foreach ($temp_args as $pair) {
    $split = explode('#', $pair);
    ${$split[0]} = $split[1];
  }
  
  $kaltura_client = KalturaHelpers::getKalturaClient();
      
  if (!$kaltura_client) {
    drupal_set_message('Failed to start Kaltura session. Please check your settings.', 'warning');
    echo theme_kaltura_maintenance_page("<br /><a href=\"#\" onclick=\"window.top.kalturaCloseModalBox()\">Close</a>", TRUE);
    exit;
  }
  
  /* TODO: remove the creation of RC */
  if ($context == 'field_mix') {
    // create roughcut at this point by API call,
    $rc = new KalturaEntry;
    $rc->name = $title;
    $session_user = KalturaHelpers::getSessionUser();
    $result = $kaltura_client->addRoughcutEntry($session_user, -2, $rc);
    // pass roughcut ID to CW.
    $kshow_id = $result['result']['entry']['id'];
    _create_node_from_roughcut($result['result']);
  }
    
  $theme_params->height = 360;
  $theme_params->width = 680;
  $theme_params->swfUrl = KalturaHelpers::getContributionWizardUrl();
  $flash_vars = KalturaHelpers::getContributionWizardFlashVars($kaltura_client->getKs(), $kshow_id, $partner_data, $type, (($context == 'comment')? TRUE: FALSE));
  $theme_params->flashVarsStr = KalturaHelpers::flashVarsToString($flash_vars);
  
  if ($partner_data && strpos($partner_data, 'kshow_exist@yes')) {
    $add_new = TRUE;
    $theme_params->mix_id = _get_node_for_mix($kshow_id);
  }
  
  if ($context == 'field' || $context == 'field_mix' || $context == 'comment') {
    $no_collect_entries = ($context == 'field_mix')? TRUE: FALSE;
    echo theme('contribution_wizard_field', $theme_params, $field_id, $no_collect_entries, $kshow_id);
    exit();
  }
  if ($kshow_id != "-2" && !$add_new)
    echo theme('contribution_wizard', $theme_params);
  else
    echo theme('contribution_wizard_add', $theme_params);
  exit;
}

function theme_kaltura_maintenance_page($text, $var) {
  return $text;
}

/*
 * theme for kaltura/contribution_wizard
 * in case we are adding entries to existing kshow
 */
function theme_contribution_wizard($theme_params) {
  $goto_url = kaltura_cw_destination();
  $javascript = '
    var goto_url = "'. $goto_url .'";
    function onContributionWizardAfterAddEntry() {
    }
    
    function onContributionWizardClose(modified) {
      if (modified == "0") { 
        //setTimeout("window.top.history.back();", 0);
        setTimeout("window.top.kalturaCloseModalBox();", 0);
      } else { 
        setTimeout("window.top.kalturaRefreshTop();", 0);
        // setTimeout("SendTopToEntriesPage();",0);
      }
      return;
    }
  ';
    
  $flash_embed = '
    <div id="divKalturaCw"></div>
    <script type="text/javascript">
      var kso = new SWFObject("'. $theme_params->swfUrl .'", "kalturaCw", "'. $theme_params->width .'", "'. $theme_params->height .'", "9", "#000000");
      kso.addParam("wmode", "opaque");
      kso.addParam("flashVars", "'. $theme_params->flashVarsStr .'");
      kso.addParam("allowScriptAccess", "always");
      kso.addParam("allowFullScreen", "TRUE");
      kso.addParam("allowNetworking", "all");
      kso.write("divKalturaCw");
    </script>
  ';
  
  return theme("kaltura_modal", array("javascript" => $javascript, "flashEmbed" => $flash_embed));
}

/*
 * theme for kaltura/contribution_wizard_add
 * in case we add entries as a standalone nodes or a new roughcut
 */
function theme_contribution_wizard_add($theme_params) {
  $goto_url = kaltura_cw_destination();
  $javascript = '
    var kshowId = -1;
    var goto_url = "'. $goto_url .'";
    var node_url = "'. (($theme_params->mix_id)? url("node/". $theme_params->mix_id): '') .'";
    function onContributionWizardAfterAddEntry(obj) {
      if (obj && obj.length > 0 && obj[0].kshowId)
      kshowId = obj[0].kshowId;
    }
    
    function onContributionWizardClose(modified) {
      if (modified == "0")';
      $javascript .= ($theme_params->mix_id)? 'setTimeout("SendTopToNodePage();", 0);': 'setTimeout("window.top.history.back();", 0);';
      $javascript .= ' else ';
      $javascript .= ($theme_params->mix_id)? 'setTimeout("SendTopToNodePage();", 0);': 'setTimeout("SendTopToEntriesPage();",0);';
      $javascript .='}';
    
  $flash_embed = '
    <div id="divKalturaCw"></div>
    <script type="text/javascript">
      var kso = new SWFObject("'. $theme_params->swfUrl .'", "kalturaCw", "'. $theme_params->width .'", "'. $theme_params->height .'", "9", "#000000");
      kso.addParam("wmode", "opaque");
      kso.addParam("flashVars", "'. $theme_params->flashVarsStr .'");
      kso.addParam("allowScriptAccess", "always");
      kso.addParam("allowFullScreen", "TRUE");
      kso.addParam("allowNetworking", "all");
      kso.write("divKalturaCw");
    </script>
  ';

  return theme("kaltura_modal", array("javascript" => $javascript, "flashEmbed" => $flash_embed));
}

/*
 * theme for kaltura/contribution_wizard_add
 * in case we add entries as a standalone nodes or a new roughcut
 */
function theme_contribution_wizard_field($theme_params, $field_id, $no_collect_entries, $kshow_id) {
  $goto_url = kaltura_cw_destination();
  $javascript = '
    var entry_list = '. (($no_collect_entries === TRUE)? '"'. $kshow_id .'"': '-1') .';
    var goto_url = "'. $goto_url .'";
    var node_url = "'. (($theme_params->mix_id)? url("node/". $theme_params->mix_id): '') .'";
    function onContributionWizardAfterAddEntry(obj) {
      str = "";
      for(i=0;i<obj.length;i++) {
        str += obj[i]["entryId"] + ",";
      }
      '.
      (($no_collect_entries === TRUE)? '': 'entry_list = str;') .'
    }
    
    function onContributionWizardClose(modified) {
      if (modified == "0")
        setTimeout("window.top.kalturaCloseModalBox();", 0);
      else
        window.location.href = "'. url("kaltura/insert_widget/") .'" + entry_list + "/'. $field_id .'";
    }
  ';
    
  $flash_embed = '
    <div id="divKalturaCw"></div>
    <script type="text/javascript">
      var kso = new SWFObject("'. $theme_params->swfUrl .'", "kalturaCw", "'. $theme_params->width .'", "'. $theme_params->height .'", "9", "#000000");
      kso.addParam("wmode", "opaque");
      kso.addParam("flashVars", "'. $theme_params->flashVarsStr .'");
      kso.addParam("allowScriptAccess", "always");
      kso.addParam("allowFullScreen", "TRUE");
      kso.addParam("allowNetworking", "all");
      kso.write("divKalturaCw");
    </script>
  ';

  return theme("kaltura_modal", array("javascript" => $javascript, "flashEmbed" => $flash_embed));
}

/*
 * callback function for kaltura/simple_editor 
 */
function kaltura_simple_editor() {
  $args = func_get_args();
  $kshow_id = $args[0];
  $type = $args[1];
  $partner_data = ($args[2] != 'empty_partner_data')? $args[2]: '';
  $no_refresh = ($args[3] == 'no_refresh')? TRUE: FALSE;
  
  if (WORK_WITH_KSHOW)
    $kaltura_client = KalturaHelpers::getKalturaClient(FALSE, "edit:". $kshow_id);
  else
    $kaltura_client = KalturaHelpers::getKalturaClient(FALSE, "edit:*");

  
  if (!$kaltura_client ) {
    drupal_set_message('Failed to start Kaltura session. Please check your settings.', 'warning');
    echo theme_kaltura_maintenance_page("<br /><a href=\"#\" onclick=\"window.top.kalturaCloseModalBox()\">Close</a>", TRUE);
    exit;
  }
  
  $theme_params->height = 546;
  $theme_params->width = 890;
  $theme_params->swfUrl = KalturaHelpers::getSimpleEditorUrl();
  $flash_vars = KalturaHelpers::getSimpleEditorFlashVars($kaltura_client->getKs(), $kshow_id, $type, $partner_data);
  $theme_params->flashVarsStr = KalturaHelpers::flashVarsToString($flash_vars);
  
  echo theme('simple_editor', $theme_params, $no_refresh);
  exit;
}

/*
 * theme for kaltura/simple_editor 
 */
function theme_simple_editor($theme_params, $no_refresh) {
  $javascript =' 
  function onSimpleEditorBackClick(modified) {
    if (modified == 0) { 
      setTimeout("window.top.kalturaCloseModalBox();", 0);
    } else {
    ';
  if ($no_refresh === TRUE) {
    $javascript .= 'setTimeout("window.top.kalturaCloseModalBox();", 0);';
  }
  else {
    $javascript .= 'setTimeout("window.top.kalturaRefreshTop();", 0);';
  }
  $javascript .= '
    }
    return;
  }
  
  function onSimpleEditorSaveClick(modified) {
  
  }
  ';

  $flash_embed = '
    <div id="divKalturaSe"></div>
    <script type="text/javascript">
      var kso = new SWFObject("'. $theme_params->swfUrl .'", "kalturaSe", "'. $theme_params->width .'", "'. $theme_params->height .'", "9", "#000000");
      kso.addParam("wmode", "opaque");
      kso.addParam("flashVars", "'. $theme_params->flashVarsStr .'");
      kso.addParam("allowScriptAccess", "always");
      kso.addParam("allowFullScreen", "TRUE");
      kso.addParam("allowNetworking", "all");
      kso.write("divKalturaSe");
    </script>
  ';

  return theme("kaltura_modal", array("javascript" => $javascript, "flashEmbed" => $flash_embed));
}

/*
 * main theme for modal windows 
 */
function theme_kaltura_modal($theme_params) {
  return '
    <html>
    <head>
    <link rel="stylesheet" type="text/css" href="'. base_path() . drupal_get_path('module', 'kaltura') .'/kaltura.css"/>
    <style type="text/css">
            html, body { margin:0; padding:0; }
    </style>
    <script type="text/javascript" src="'. base_path() . drupal_get_path('module', 'kaltura') .'/kaltura.js"></script>
    <script type="text/javascript" src="'. base_path() . drupal_get_path('module', 'kaltura') .'/swfobject.js"></script>
    <script type="text/javascript">
      '. $theme_params["javascript"] .'    
    </script>
    </head>
    <body>
      '. $theme_params["flashEmbed"] .'
    </body>
    </html>';
}

/*
 * callback function for kaltura/insert_widget page
 * 
 * currently not in use
 * 
 * used in old version of the module.
 * might be useful for the "kaltura_inline" implementation
 */
function kaltura_insert_widget() {
  $args = func_get_args();
  $entry_list = $args[0];
  $field_name = str_replace('_', '-', $args[1]);
  if ($field_name == 'edit-comment') {
    $entries = explode(',', rtrim($entry_list, ','));
    $entry_list = '';
    foreach ($entries as $entry) {
      $entry_list .= '[kaltura-widget media_type=\"video\" comment=\"'. $entry .'\" size=\"large\" /]';
    }
  }
  
  $javascript =' 
    var bodyTextArea = window.top.document.getElementById("'. $field_name .'");
    if (bodyTextArea)
      bodyTextArea.value += "'. $entry_list .'";';
      $javascript .= ($field_name == 'edit-comment')? 'window.top.document.getElementById("edit-preview").click();': '';
      $javascript .= '
      window.top.kalturaCloseModalBox();
  ';

  $flash_embed = '';
  
  echo theme("kaltura_modal", array("javascript" => $javascript, "flashEmbed" => $flash_embed));
  exit;
}

/*
 * helper function to get embed options according to the "kaltura tag" parameters
 *
 * this function also calls the kaltura_use_widget (file kaltura.module) function which exposes a hook
 *
 * returns an array of embed options later injected into the embed tag
 */
function _kaltura_get_embed_options($params) {
  switch ($params["align"]) {
    case 'r': $align = "right";
      break;
    case 'm': $align = "center";
      break;
    case 'l': $align = "left";
      break;
    default: $align = "";
      break;
  }

  if ($params['custom_style']) {
    $custom_style = $params['custom_style'];
  }
  
  if ($_SERVER["SERVER_PORT"] == 443)
    $protocol = "https://";
  else
    $protocol = "http://"; 


  if ($params['uiconf_id']) {
    $uicid = $params['uiconf_id'];
    $theme_uicid = TRUE;
  }
  $wid = '_'. variable_get('kaltura_partner_id', '');

  if ($params["entry"]) {
    $uicid = kaltura_use_uiconf($uicid, 'entry', $theme_uicid, $params['media_type']);
    $swf_url = KalturaHelpers::getSwfUrlForWidget($wid .'_'. $uicid .'/entry_id/'. $params["entry"] .'/uiconf_id/'. $uicid);
    $media_id = $params["entry"];
    $player_size = _kaltura_calculate_player_size('entry', $params['size'], $params['width'], $params['height']);
  }
  elseif ($params["kid"]) {
    $uicid = kaltura_use_uiconf($uicid, 'mix', $theme_uicid);
    $kshow = TRUE;
    $swf_url = KalturaHelpers::getSwfUrlForWidget($wid .'_'. $uicid .'/kid/'. $params["kid"] .'/uiconf_id/'. $uicid);
    $media_id = $params["kid"];
    $player_size = _kaltura_calculate_player_size('mix', $params['size'], $params['width'], $params['height']);
  }
  elseif ($params["mix"]) {
    $uicid = kaltura_use_uiconf($uicid, 'mix', $theme_uicid);
    $roughcut = TRUE;
    $swf_url = KalturaHelpers::getSwfUrlForWidget($wid .'_'. $uicid .'/entry_id/'. $params["mix"] .'/uiconf_id/'. $uicid);
    $media_id = $params["mix"];
    $player_size = _kaltura_calculate_player_size('mix', $params['size'], $params['width'], $params['height']);
  }
  elseif ($params["comment"]) {
    $uicid = kaltura_use_uiconf($uicid, 'entry', $theme_uicid, 'comment');
    $swf_url = KalturaHelpers::getSwfUrlForWidget($wid .'_'. $uicid .'/entry_id/'. $params["comment"] .'/uiconf_id/'. $uicid);
    $media_id = $params["comment"];
    $player_size = _kaltura_calculate_player_size('comment', $params['size'], $params['width'], $params['height']);
  }

  $flash_vars_str = "layoutId=". $player_size['layout_id'] ."&pd_original_url=". urlencode($protocol . $_SERVER["HTTP_HOST"] . request_uri());

  return array(
    "flashVars" => $flash_vars_str,
    "height" => $player_size["height"],
    "width" => $player_size["width"],
    "custom_style" => $custom_style,
    "align" => $align,
    "media_id" => $media_id,
    "js_events" => $events,
    "wid" => $wid,
    "uiconf" => $uicid,
    "roughcut" => $roughcut,
    "kshow" => $kshow,
    "swfUrl" => $swf_url
  );
}

/*
 * helper function that returns the player width and height according to letter
 * found in the "kaltura tag"
 */
function _kaltura_calculate_player_size($type, $size, $width = 0, $height = 0) {
  if ($width > 0 && $height > 0) return array( 'width' => $width, 'height' => $height, 'layout_id' => 'fullLarge' );
  switch ($type) {
    case 'entry':
      $variable_width = variable_get('kaltura_entry_width', '410');
      $variable_height = variable_get('kaltura_entry_height', '364');
      break;
    case 'mix':
      $variable_width = variable_get('kaltura_mix_width', '410');
      $variable_height = variable_get('kaltura_mix_height', '364');
      break;
    case 'comment':
      $variable_width = variable_get('kaltura_comment_width', '410');
      $variable_height = variable_get('kaltura_comment_height', '364');
      break;
  }
  return array( 'width' => $variable_width, 'height' => $variable_height, 'layout_id' => 'fullLarge' );
}
