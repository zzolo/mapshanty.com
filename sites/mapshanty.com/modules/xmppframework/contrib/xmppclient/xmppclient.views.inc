        <?php
// $Id: xmppclient.views.inc,v 1.1 2008/10/02 20:57:52 darrenferguson Exp $

/**
 * @file
 * Provide views data and handlers for xmppclient.module
 */

/**
 * Implementation of hook_views_handlers().
 */
function xmppclient_views_handlers() {
  return array(
    'info' => array(
      'path' => drupal_get_path('module', 'xmppclient') .'/includes',
    ),
    'handlers' => array(
      'views_handler_filter_xmppclient_status' => array(
        'parent' => 'views_handler_filter_in_operator',
      ),
    ),
  );
}

/**
 * Implementation of hook_views_data()
 */
function xmppclient_views_data() {
  // field group
  $data['xmppclient_user']['table']['group']  = t('User');

  // table joins
  $data['xmppclient_user']['table']['join'] = array(
    'users' => array(
      'left_field' => 'uid',
      'field' => 'uid'
    ),
    'node' => array(
      'left_table' => 'users',
      'left_field' => 'uid',
      'field' => 'uid',
      'type' => 'INNER'
    ),
    'node_revisions' => array(
      'left_table' => 'node',
      'left_field' => 'uid',
      'field' => 'uid',
      'type' => 'INNER'
    )
  );

  // jid
  $data['xmppclient_user']['jid'] = array(
    'title' => t('Jid'),
    'help' => t('The user Jabber ID'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    )
  );

  // status
  $data['xmppclient_user']['status'] = array(
    'title' => t('Jabber Status'),
    'help' => t("The user's current Jabber status"),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_xmppclient_status',
    ),
  );

  // message
  $data['xmppclient_user']['message'] = array(
    'title' => t('Jabber Message'),
    'help' => t("The user's current Jabber status message"),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    )
  );

  return $data;
}
