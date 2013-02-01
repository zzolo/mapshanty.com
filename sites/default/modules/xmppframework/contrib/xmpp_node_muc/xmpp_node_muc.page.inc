<?php
// $id

/**
 * @param $node
 *      Node for the organic group associated with the MUCs
 */
function xmpp_node_muc_list_mucs($node) {
  global $user;

  $output .= '<div id="xmpp_node_muc_create_muc">';
  $output .= l(t('Create Additional MUC'), 'node/'. $node->nid .'/create_muc', array());
  $output .= '</div>';
  $output .= '<table id="xmpp_node_muc_mucs">';
  $output .= '<tr class="xmpp_node_muc_muc_title">';
  $output .= '<td class="name">Name</td>';
  $output .= '<td class="subject">Subject</td>';
  $output .= '<td class="participants">Participants</td>';
  $output .= '<td class="user_status">Current Status</td>';
  $output .= '<td class="sub_group">Sub Group</td>';
  $output .= '</tr>';

  $result = db_query("SELECT * FROM {xmpp_node_muc_groups} WHERE nid = %d ORDER BY sub_group ASC", $node->nid);
  while ($row = db_fetch_object($result)) {
    $output .= '<tr class="xmpp_node_muc_muc_item">';
    $output .= '<td class="name">'. l($row->name, 'node/'. $node->nid . '/edit_muc/'. $row->gid, array()) .'</td>';
    $output .= '<td class="subject">'. ((strlen($row->subject) > 0) ? $row->subject : 'Not Set') .'</td>';
    $output .= '<td class="participants">0</td>';
    // determinng if the user is currently logged into the group or not
    $online = db_result(db_query("SELECT 1 FROM {xmpp_node_muc_users} WHERE jid = '%s' AND gid = %d", $user->xmppclient['jid'], $row->gid));
    $output .= '<td class="user_status">'. (($online == 1) ? 'Participating' : 'Not Participating') .'</td>';
    $output .= '<td class="sub_group">'. (($row->sub_group == 0) ? 'No' : 'Yes') .'</td>';
    $output .= '</tr>';
  }
  $output .= '</table>';

  return $output;
}

/**
 * Function displays a form so the user can create a sub group muc
 */
function xmpp_node_muc_create() {

  if (is_numeric(arg(1)) && ($node = node_load(arg(1)))) {
    $form['nid'] = array(
      '#type'    => 'hidden',
      '#value'   => $node->nid,
    );
    $form['name'] = array(
      '#type'    => 'textfield',
      '#title'   => t('MUC Name'),
      '#size'    => 30,
    );
    $form['submit'] = array(
      '#type'    => 'submit',
      '#value'   => t('Create'),
    );
  }
  return $form;
}

/**
 * @param $node
 *      Node for the organic group associated with the muc
 * @param $gid
 *      Group id for the muc
 */
function xmpp_node_muc_delete($node, $gid) {
  // removing the node from the system
  $group = _xmpp_node_muc_group_load($gid);
  if ($group) {
    xmpp_node_muc_delete_muc($node, $gid);
    drupal_set_message(t('!name has been removed from the server', array('!name' => $group->name)));
  }
  else {
    drupal_set_message(t('No group was specified for muc removal'), 'error');
  }
  drupal_goto('node/'. $node->nid .'/mucs');
}

/**
 * @param $node
 *      Node for the organic group associated with the muc
 * @param $gid
 *      Group id for the muc
 */
function xmpp_node_muc_edit($node, $gid) {

  if (!($group = _xmpp_node_muc_group_load($gid))) {
    drupal_set_message(t('Invalid Group received by the system'), 'error');
    return '';
  }

  // get the number of participants in this muc
  $count = db_result(db_query("SELECT COUNT(jid) FROM {xmpp_node_muc_users} WHERE nid = %d AND gid = %d", $node->nid, $gid));
  $output = '<div id="xmpp_node_muc_mucs_edit">';
  $output .= '<div>'. l('Delete MUC', 'node/'. $node->nid . '/delete_muc/'. $gid, array('onclick' => 'return confirm("Are you sure?")')) .'</div>';
  $output .= '<div><span class="title">Conference Name: </span> <span class="message">'. $group->name .'</span></div>';
  $output .= '<div><span class="title">Subject: </span> <span class="message">'. ((strlen($group->subject) > 0) ? $group->subject : 'Not Set') .'</span></div>';
  $output .= '<div><span class="title">Participants: </span> <span class="message">'. $count .'</span></div>';
  $output .= '</div>'; // end of xmpp_node_muc_mucs_edit

  // if there was participants then display this on the screen to the user
  if ($count > 0) {
    $output .= '<div id="xmpp_node_muc_mucs_participants">';
    $output .= '<table>';
    $output .= '<tr><td>JID</td><td>NickName</td><td>Join Time</td></tr>';
    $result = db_query("SELECT * FROM {xmpp_node_muc_users} WHERE nid = %d AND gid = %d", $node->nid, $gid);
    while($row = db_fetch_object($result)) {
      $output .= '<tr>';
      $output .= '<td>'. $row->jid .'</td>';
      $output .= '<td>'. $row->nickname .'</td>';
      $output .= '<td>'. format_date($row->join_time, XMPP_NODE_MUC_DATE_FORMAT) .'</td>';
      $output .= '</tr>';
    }
    $output .= '</table>';
    $output .= '</div>';
  }

  // retrieve the logs for the muc if there are any
  $log_count = db_result(db_query("SELECT COUNT(gid) FROM {xmpp_node_muc_log} WHERE nid = %d AND gid = %d AND body <> ''", $node->nid, $gid));
  if ($log_count > 0) {
    $output .= '<div id="xmpp_node_muc_mucs_logs">';
    $output .= '<div>'. $group->name .' MUC Logs</div>';
    $output .= '<table>';
    $output .= '<tr class="header"><td>JID</td><td>TimeStamp</td><td>Message</td></tr>';
    $result = db_query("SELECT * FROM {xmpp_node_muc_log} WHERE nid = %d AND gid = %d AND body <> '' ORDER BY mdate DESC", $node->nid, $gid);
    while($row = db_fetch_object($result)) {
      $output .= '<tr class="data_body">';
      $output .= '<td>'. $row->nickname .'</td>';
      $output .= '<td nowrap>'. format_date($row->mdate, XMPP_NODE_MUC_DATE_FORMAT) .'</td>';
      $output .= '<td>'. $row->body .'</td>';
      $output .= '</tr>';
    }
    $output .= '</table>';
    $output .= '</div>';
  }

  return $output;
}

/**
 * Function is utilized to display the conversations that are associated with the temporary node
 * and these conversations have not been associated with the node yet as comments or sub postings
 *
 * @param $node
 *      The node we are retrieving conversations for
 */
function xmpp_node_muc_temporary_conversation_list($node) {
  // retrieving the archived conversations for this particular temporary node type
  $header[] = array('data' => t('Conversation #'), 'field' => 'gid');
  $header[] = array('data' => t('Subject'), 'field' => 'subject', 'sort' => 'desc');
  $header[] = array('data' => t('Start Date'), 'field' => 'start_date');
  $header[] = array('data' => t('End Date'), 'field' => 'end_date');
  $sql = "SELECT gid, subject, MIN(mdate) AS start_date, MAX(mdate) AS end_date FROM {xmpp_node_muc_log}";
  $sql .= sprintf(" WHERE nid = %d GROUP BY gid". tablesort_sql($header), $node->nid);
  $count_sql = sprintf("SELECT DISTINCT gid FROM {xmpp_node_muc_log} WHERE nid = %d", $node->nid);
  $result = pager_query($sql, 20, NULL, $count_sql);
  while ($row = db_fetch_object($result)) {
    $entry[$row->gid]['conversation'] = l($row->gid, 'node/'. $node->nid .'/conversation/'. $row->gid);
    $entry[$row->gid]['subject'] = $row->subject;
    $entry[$row->gid]['start_date'] = format_date($row->start_date, XMPP_NODE_MUC_DATE_FORMAT);
    $entry[$row->gid]['end_date'] = format_date($row->end_date, XMPP_NODE_MUC_DATE_FORMAT);
  }
  $output = theme('table', $header, $entry);
  $output .= theme('pager', NULL, 20, 0);
  return $output;
}

/**
 * Function is utilized to display the pertinent conversations that have occurred in temporary nodes
 *
 * @param $node
 *      The node we are currently checking
 * @param $gid
 *      The gid / conversation we are going to display to the users
 */
function xmpp_node_muc_temporary_conversation($node, $gid) {
  $header[] = array('data' => t('Date'), 'field' => 'mdate', 'sort' => 'desc');
  $header[] = array('data' => t('User'), 'field' => 'nickname');
  $header[] = array('data' => t('Log Message'), 'field' => 'body');
  $sql = sprintf("SELECT * FROM {xmpp_node_muc_log} WHERE nid = %d AND gid = %d AND body <> ''". tablesort_sql($header), $node->nid, $gid);
  $count = 0;
  $result = pager_query($sql, 10);
  while ($row = db_fetch_object($result)) {
    $entry[$count]['mdate'] = format_date($row->mdate, XMPP_NODE_MUC_DATE_FORMAT);
    $entry[$count]['nickname'] = str_replace("'", "", $row->nickname);
    $entry[$count]['body'] = str_replace("'", "", $row->body);
    $count++;
  }
  $output = '<div class="xmpp_node_muc_conversation_list">'. l(t('Conversation List'), 'node/'. $node->nid .'/conversations') .'</div>';
  $output .= theme('table', $header, $entry);
  $output .= theme('pager', NULL, 10, 0);
  return $output;
}

