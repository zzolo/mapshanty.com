<?php
// $id

/**
 * Builds display block for XMPP Client Login
 */
function theme_xmppclient_login() {
  global $user;

  // check if the password is already stored or not in the system, if not direct user to the enter portion
  if (!$user->xmppclient['password']) {
    $output = '<div id="xmppclient-login" class="xmppclient_login">'. l(t('Configure Client'), 'user/'. $user->uid .'/edit') .'</div>';
  }
  else {
    $output = '<div id="xmppclient-login" class="xmppclient_login">';
    $output .= drupal_get_form('xmppclient_login_form');
    $output .= '</div>';
  }
  return $output;
}
