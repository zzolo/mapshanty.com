<?php
// $id

/**
 * Builds the display page for the xmpp vcard synchronize
 *
 * @param $vcard
 *      Array object holding the vcard
 */
function theme_xmpp_vcard_information($vcard = array(), $profile = NULL) {
  global $user;

  $tuser = new StdClass();
  $tuser->uid = 0;

  $header[] = array('data' => t('Name'), 'field' => 'name', 'sort' => 'desc');
  $header[] = array('data' => t('XMPP'), 'field' => 'xmpp');
  $header[] = array('data' => t('User Profile'), 'field' => 'drupal');
  $header[] = array('data' => t('Synchronize'), 'field' => 'synch');
  $output = '<div style="float: right">'. l(t('Synchronize All'), 'xmpp_vcard/synchronize/all') .'</div>';
  $output .= t('<h2>Current XMPP Vcard information</h2>');
  foreach ($vcard as $key => $values) {
    if (is_array($values)) {
      foreach ($values as $sub => $subvalues) {
        // retrieving the field mappings for the particular vcard node
        if ($field = _xmpp_vcard_fields(strtoupper($sub))) {
          $entry[strtoupper($sub)]['name'] = $field['name'];
          $entry[strtoupper($sub)]['xmpp'] = $subvalues;
          if (isset($profile->$field['mapping']) && $profile->$field['mapping'] && is_array($profile->$field['mapping']) ) {
            $value = $profile->$field['mapping'];
            $entry[strtoupper($sub)]['drupal'] = ($value[0]['value']) ? $value[0]['value'] : '';
          }
          else {
            $entry[strtoupper($sub)]['drupal'] = ($field['mapping']) ? $profile->$field['mapping'] : '';
          }
          // determine if the field is marked as publicly available
          if ((_user_relationship_field_perms_allow_access(array('node' => $profile), $field['mapping'], $tuser))) {
            $entry[strtoupper($sub)]['synch'] = l(t('Synchronize'), 'xmpp_vcard/synchronize/'. $sub);
          }
          else {
            $entry[strtoupper($sub)]['synch'] = t('Not Public');
          }
        }
      }
    }
    else {
      // retrieving the field mappings for the particular vcard node
      if ($field = _xmpp_vcard_fields(strtoupper($key))) {
        $entry[strtoupper($key)]['name'] = $field['name'];
        $entry[strtoupper($key)]['xmpp'] = $values;
        if (isset($profile->$field['mapping']) && is_array($profile->$field['mapping'])) {
          $entry[strtoupper($key)]['drupal'] = $profile->$field['mapping'][0]['value'];
        }
        else {
          $entry[strtoupper($key)]['drupal'] = ($field['mapping']) ? $profile->$field['mapping'] : '';
        }
        // determine if synchronization can occur for this field
        if ((_user_relationship_field_perms_allow_access(array('node' => $profile), $field['mapping'], $tuser))) {
          $entry[strtoupper($key)]['synch'] = l(t('Synchronize'), 'xmpp_vcard/synchronize/'. $key);
        }
        else {
          $entry[strtoupper($key)]['synch'] = t('Not Public');
        }
      }
    }
  }
  $output .= theme('table', $header, $entry);
  return $output;
}
