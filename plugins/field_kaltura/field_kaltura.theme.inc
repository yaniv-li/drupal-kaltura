<?php

function theme_kaltura_metadata ($metadata) {
  foreach ($metadata as $key => $val) {
    $key = preg_replace('/_/', ' ', $key);
    $met[] = $key  . ' :' . $val;
  }
  $out = theme('item_list', array('items' => $met, 'title' => t('Kaltura metadata'), 'type' => 'ul'));
  return $out;

}
function theme_kaltura_player_default ($vars) {
  $out = '<div class="kaltura">';
  $out .= '<h3 class="kaltura-title">' . $vars['title'] . '</h3>';
  $out .= '<div class="kaltura-embed">' . $vars['embed'] . '</div>';
  $out .= '<div class="kaltura-desc">' . $vars['desc'] . '</div>';
  $out .= '</div>';
  return $out;

}

function theme_kaltura_player ($vars) {
  $out = '<div class="kaltura">';
  $out .= '<div class="kaltura-embed">' . $vars['embed'] . '</div>';
  $out .= '</div>';
  return $out;

}

function theme_kaltura_player_title ($vars) {
  $out = '<div class="kaltura">';
  $out .= '<h3 class="kaltura-title">' . $vars['title'] . '</h3>';
  $out .= '<div class="kaltura-embed">' . $vars['embed'] . '</div>';
  $out .= '</div>';
  return $out;

}

function theme_kaltura_player_meta ($vars) {
  $out = '<div class="kaltura">';
  $out .= '<h3 class="kaltura-title">' . $vars['title'] . '</h3>';
  $out .= '<div class="kaltura-embed">' . $vars['embed'] . '</div>';
  $out .= '<div class="kaltura-desc">' . $vars['desc'] . '</div>';
  $out .= '<div class="kaltura-metadata">' . theme('kaltura_metadata', $vars['meta']) . '</div>';
  $out .= '</div>';
  return $out;

}
function theme_kaltura_thumbnail_default ($vars) {
  $out = '<div class="kaltura">';
  $out .= '<h3 class="kaltura-title">' . $vars['title'] . '</h3>';
  $out .= '<div class="kaltura-thumb">' . theme('image', array('path' => $vars['thub'], 'title' => $vars['title'])) . '</div>';
  $out .= '</div>';
  return $out;

}

function theme_kaltura_thumbnail ($vars) {
  $out = '<div class="kaltura">';
  $out .= '<div class="kaltura-thumb">' . theme('image', array('path' => $vars['thub'], 'title' => $vars['title'])) . '</div>';
  $out .= '</div>';
  return $out;

}

function theme_kaltura_thumbnail_title_desc ($vars) {
  $out = '<div class="kaltura">';
  $out .= '<h3 class="kaltura-title">' . $vars['title'] . '</h3>';
  $out .= '<div class="kaltura-thumb">' . theme('image', array('path' => $vars['thub'], 'title' => $vars['title'])) . '</div>';
  $out .= '<div class="kaltura-desc">' . $vars['desc'] . '</div>';
  $out .= '</div>';
  return $out;

}

function theme_kaltura_thumbnail_meta ($vars) {
  $out = '<div class="kaltura">';
  $out .= '<h3 class="kaltura-title">' . $vars['title'] . '</h3>';
  $out .= '<div class="kaltura-thumb">' . theme('image', array('path' => $vars['thub'], 'title' => $vars['title'])) . '</div>';
  $out .= '<div class="kaltura-desc">' . $vars['desc'] . '</div>';
  $out .= '<div class="kaltura-metadata">' . theme('kaltura_metadata', $vars['meta']) . '</div>';
  $out .= '</div>';
  return $out;

}
