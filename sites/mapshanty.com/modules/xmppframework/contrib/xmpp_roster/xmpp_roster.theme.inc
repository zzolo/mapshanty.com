<?php
// $id

/**
 * Builds the display page for the xmpp roster associations
 *
 * @param $associations
 *      Array of pre-built associations
 * @param $view
 *      View we wish to see
 */
function theme_xmpp_roster_associations($associations = array(), $view = 'both') {
  global $user;
  $output = '';

  $header[] = array('data' => t('Name'), 'field' => 'name');
  $header[] = array('data' => t('Created'), 'field' => 'created');
  $header[] = array('data' => t('Relationship Types'), 'field' => 'type', 'sort' => 'desc');
  $header[] = array('data' => t('Action'), 'field' => 'action');

  if ($view == 'both') {
    // retrieving all associations that are in xmpp and user relationships
    $both = $associations['both'];
    if ($both && count($both)) {
      $output .= t('<h2>Associates you have in your XMPP Roster and have a User Relationship with</h2>');
      foreach ($both as $key => $values) {
        $uid = $values['uid'];
        $entry[$uid]['name'] = $values['name'];
        $entry[$uid]['created'] = format_date($values['created'], XMPP_ROSTER_DATE_FORMAT);
        $entry[$uid]['type'] = implode(', ', $values['type']);
        $entry[$uid]['action'] = l(t('Remove'), 'xmpp_roster/delete/'. $uid);
      }
      $output .= theme('table', $header, $entry);
    }
    else {
      $output .= '<div style="text-align: center;">'. t('No relationships were found that we in your XMPP and User Relationships') .'</div>';
    }
  }

  if ($view == 'relationships') {
    // retrieving all associations that are in user relationships only
    $relationship = $associations['relationship'];
    if ($relationship && count($relationship)) {
      $output .= t('<h2>Associates with User Relationships only</h2>');
      foreach ($relationship as $key => $values) {
        $uid = $values['uid'];
        $entry1[$uid]['name'] = $values['name'];
        $entry1[$uid]['created'] = format_date($values['created'], XMPP_ROSTER_DATE_FORMAT);
        $entry1[$uid]['type'] = implode(', ', $values['type']);
        $entry1[$uid]['action'] = l(t('Add'), 'xmpp_roster/add/'. $uid);
      }
      $output .= theme('table', $header, $entry1);
    }
    else {
      $output .= '<div style="text-align: center;">'. t('No relationships were found') .'</div>';
    }
  }
  // resetting two of the header fields for the xmpp view
  $header[1] = array('data' => t('JID'), 'field' => 'jid');
  $header[2] = array('data' => t('In System'), 'field' => 'system', 'sort' => 'desc');

  if ($view == 'xmpp') {
    // retrieving all associations that are in xmpp only
    $xmpp = $associations['xmpp'];
    if ($xmpp && count($xmpp)) {
      $output .= t('<h2>Associates with XMPP Roster relationships only</h2>');
      foreach ($xmpp as $key => $values) {
        if ($xmpp[$key]['uid']) {
          $xmppuser = user_load(array('uid' => $xmpp[$key]['uid']));
        }
        $entry2[$key]['name'] = ($xmppuser) ? $xmppuser->name : t('Unknown');
        $entry2[$key]['jid'] = $key;
        $entry2[$key]['system'] = ($xmppuser) ? t('Yes') : t('No');
        // adding the link for removal of the user from the roster
        if ($xmppuser) {
          $entry2[$key]['action'] = l(t('Remove'), 'xmpp_roster/delete/'. $xmppuser->uid);
        }
        else {
          $entry2[$key]['action'] = l(t('Remove'), 'xmpp_roster/delete/'. $key);
        }
        $xmppuser = NULL;
      }
      $output .= theme('table', $header, $entry2);
    }
    else {
      $output .= '<div style="text-align: center;">'. t('No xmpp relationships were found') .'</div>';
    }
  }
  return $output;
}
