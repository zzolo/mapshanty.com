<?php
// $Id

/**
 * @file
 * Admin page callback file for the xmppclient module.
 */

//////////////////////////////////////////////////////////////////////////////
// XMPPClient admin settings

/**
 * Settings form display.
 */
function xmppclient_admin_settings() {
  $form['client_settings'] = array('#type' => 'fieldset', '#title' => t('XWChat Client Settings'), '#collapsible' => TRUE, '#collapsed' => FALSE);
  $form['client_settings']['xmppclient_httpbase'] = array(
    '#type'          => 'select',
    '#title'         => t('XWChat HTTP Mode'),
    '#default_value' => XMPPCLIENT_HTTPBASE,
    '#options'       => array('/http-bind/' => t('HTTP Bind'), '/http-poll/' => t('HTTP Poll')),
    '#description'   => t('Choose which http mode the client will utilize polling or binding'),
  );
  $form['client_settings']['xmppclient_authtype'] = array(
    '#type'          => 'select',
    '#title'         => t('XWChat Authentication Type'),
    '#default_value' => XMPPCLIENT_AUTHTYPE,
    '#options'       => array('nonsasl' => t('NONSASL'), 'sasl' => t('SASL')),
    '#description'   => t('Choose whether to use SASL authentication or not when logging in with the client'),
  );
  $form['client_settings']['xmppclient_muc_conference'] = array(
    '#type'          => 'textfield',
    '#title'         => t('XWChat Mutli-User Chat Conference Server'),
    '#default_value' => XMPPCLIENT_MUC_CONFERENCE,
    '#size'          => 50,
    '#maxlength'     => 255,
    '#description'   => t('Server were all of the multi-user chat rooms are stored'),
  );
  $form['client_settings']['xmppclient_jud'] = array(
    '#type'          => 'textfield',
    '#title'         => t('XWChat Jabber User Directory'),
    '#default_value' => XMPPCLIENT_JUD,
    '#size'          => 50,
    '#maxlength'     => 255,
    '#description'   => t('Server where your Jabber user directory service is running'),
  );
  $form['client_settings']['xmppclient_launch'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Launch XWChat Client on user Login'),
    '#default_value' => XMPPCLIENT_LAUNCH,
    '#description'   => t('Will by default launch the XWChat client on user login. Users can stop in their settings'),
  );
  $form['client_settings']['xmppclient_locale'] = array(
    '#type'          => 'select',
    '#title'         => t('XWChat Locale Setting'),
    '#default_value' => XMPPCLIENT_LOCALE,
    '#options'       => array(
      'arabic' => t('Arabic'),
      'ch'     => t('Chinese'),
      'nl'     => t('Dutch'),
      'en'     => t('English'),
      'fi'     => t('Finnish'),
      'fr'     => t('French'),
      'de'     => t('German'),
      'hu'     => t('Hungarian'),
      'it'     => t('Italian'),
      'ko'     => t('Korean'),
      'lt'     => t('Lithuanian'),
      'ro'     => t('Romanian'),
      'ru'     => t('Russian'),
      'es'     => t('Spannish'),
      'tk'     => t('Turkish'),
    ),
    '#description'   => t('Choose the default language that the client will utilize'),
  );
  $form['client_provisioning'] = array('#type' => 'fieldset', '#title' => t('Provision XMPP Accounts'), '#collapsible' => TRUE, '#collapsed' => FALSE);
  $form['client_provisioning']['xmppclient_provisioning_server'] = array(
    '#type'          => 'textfield',
    '#title'         => t('XMPP Server Domain'),
    '#default_value' => XMPPCLIENT_PROVISIONING_SERVER,
    '#size'          => 50,
    '#maxlength'     => 255,
    '#description'   => t('The xmpp server domain we will utilize and hence append the users name too (user @ domain)'),
  );
  $form['client_provisioning']['xmppclient_provisioning_on'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Create an xmpp entry for the user'),
    '#default_value' => XMPPCLIENT_PROVISIONING_ON,
    '#description'   => t('If checked the system will create a xmpp entry for the user so they can utilize xmpp'),
  );

  return system_settings_form($form);
}
