<?php
// $id

function xmpp_relationships_admin_settings() {
  $form['settings'] = array('#type' => 'fieldset', '#title' => t('XMPP Relationships Settings'), '#collapsible' => TRUE, '#collapsed' => FALSE);
  // determine whether to send an xmpp notification that the subscription is complete
  $form['settings']['xmpp_relationships_send_subscription_notification'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Send Notification of Relationship via XMPP'),
    '#default_value' => XMPP_RELATIONSHIPS_SEND_SUBSCRIPTION_NOTIFICATION,
    '#description'   => t('Will determine whether a headline message will be sent to the user regarding the creation or removal of a relationship'),
  );

  // retrieving the user_relationships_types information
  if (function_exists('user_relationships_types_load')) {
    $options = array();
    foreach (user_relationships_types_load() as $rtype) {
      $options[$rtype->rtid] = $rtype->name;
    }
    $form['types'] = array('#type' => 'fieldset', '#title' => t('XMPP Relationships'), '#collapsible' => TRUE, '#collapsed' => FALSE);
    $form['types']['xmpp_relationships_user_relationships_types'] = array(
      '#type'          => 'checkboxes',
      '#title'         => t('User Relationship Types'),
      '#default_value' => variable_get('xmpp_relationships_user_relationships_types', array()),
      '#options'       => $options,
      '#description'   => t('User Relationship Types we will create XMPP relationships for'),
    );
  }

  $form['buttons']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
  );
  $form['buttons']['reset'] = array(
    '#type' => 'submit',
    '#value' => t('Reset to defaults'),
  );

  return $form;
}

/**
 * Submit hook for the settings form.
 */
function xmpp_relationships_admin_settings_submit($form, &$form_state) {
  $op = $form_state['clicked_button']['#value'];
  switch ($op) {
    case "Save configuration":
      variable_set('xmpp_relationships_send_subscription_notification', $form_state['values']['xmpp_relationships_send_subscription_notification']);
      if (isset($form_state['values']['xmpp_relationships_user_relationships_types'])) {
        variable_set('xmpp_relationships_user_relationships_types', $form_state['values']['xmpp_relationships_user_relationships_types']);
      }
      drupal_set_message(t('The configuration options have been saved.'));
      break;
    case "Reset to defaults":
      variable_del('xmpp_relationships_send_subscription_notification');
      variable_del('xmpp_relationships_user_relationships_types');
      drupal_set_message(t('The configuration options have been reset to their default values.'));
      break;
  }
}

