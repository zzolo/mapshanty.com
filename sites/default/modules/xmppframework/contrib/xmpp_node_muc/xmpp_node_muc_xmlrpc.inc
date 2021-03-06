<?php
// $Id: xmpp_node_muc_xmlrpc.inc,v 1.1 2008/10/02 20:57:52 darrenferguson Exp $

/**
 * XMLRPC function called for entering a room
 *
 * @param $room
 *      Room on the XMPP server the user has entered
 * @param $server
 *      Server the room is situated on
 * @param $nickname
 *      Nickname the user is using in the chat room
 * @param $from
 *      Person who sent the message including resource
 * @param $presence
 *      The users presence whether leaving or entering
 */
function xmpp_node_muc_xmlrpc_room_presence_update($room, $server, $nickname, $from, $presence) {
  $muc = $room .'@'.$server;
  if (!($gid = _xmpp_node_muc_group_gid($muc))) {
    return FALSE;
  }
  $group = _xmpp_node_muc_group_load($gid);
  // if this returns null either the from does not exist or the xmppclient module is not enabled
  if (!($uid = _xmpp_node_muc_uid_from_fulljid($from))) {
    return FALSE;
  }
  $jid = trim(substr($from, 0, strpos($from, '/')));

  // check if they are in the room, and if so just update the nickname since it will be a nickname change
  if (db_result(db_query("SELECT COUNT(*) FROM {xmpp_node_muc_users} WHERE gid = %d AND jid = '%s'", $gid, $jid))) {
    if ($presence == 'available') {
      db_query("UPDATE {xmpp_node_muc_users} SET nickname = '%s', join_time = %d WHERE jid = '%s' AND gid = %d", $nickname, time(), $jid, $gid);
      if (variable_get('xmpp_node_muc_xmlrpc_debug', false)) {
        watchdog('xmpp_node_muc', 'JID: '. $jid .' changed nickname to: '. $nickname .' in room: '. $room, WATCHDOG_NOTICE);
      }
    }
    else {
      db_query("DELETE FROM {xmpp_node_muc_users} WHERE jid = '%s' AND gid = %d", $jid, $gid);
      // loading the node so we can check the type to determine if it is a temporary muc or not
      $node = node_load($group->nid);
      // determine the count of users still in the temporary muc, if any
      $count = db_result(db_query("SELECT COUNT(*) FROM {xmpp_node_muc_users} WHERE gid = %d", $gid));
      if (xmpp_node_muc_type($node->type) == XMPP_NODE_MUC_TEMPORARY && !$count) {
        // deleting the temporary muc from the server, we do however keep the history for the muc
        // that way is someone wishes to utilize it at a later date then they can access it this
        // will only occur if there are no users left in the room and it is a temporary type room
        xmpp_node_muc_delete_muc($node, $gid);
      }
    }
  }
  else {
    // we only care about available since at this point the person was never in the room hence ignore unavailable at this point
    if ($presence == 'available') {
      db_query("INSERT INTO {xmpp_node_muc_users} (gid, nid, uid, jid, nickname, join_time) VALUES (%d, %d, %d, '%s', '%s', %d)", $gid, $group->nid, $uid, $jid, $nickname, time());
      if (variable_get('xmpp_node_muc_xmlrpc_debug', false)) {
        watchdog('xmpp_node_muc', 'JID: '. $jid .' with nickname: '. $nickname .' has entered room: '. $room, WATCHDOG_NOTICE);
      }
    }
  }
  return TRUE;
}

/**
 * XMLRPC function called for getting the MUC log
 *
 * @param $jid
 *      Full JID of the person who sent the message
 * @param $room
 *      Full Muc Room name including the server
 * @param $subject
 *      Subject of the message (can be empty)
 * @param $body
 *      Body of the message (can be empty, but is usually not)
 */
function xmpp_node_muc_xmlrpc_room_log($jid, $room, $subject = '', $body = '') {
  $nickname = substr($jid, 0, strpos($jid, '/')); // retrieving the nickname from the jid

  if (variable_get('xmpp_node_muc_xmlrpc_debug', false)) {
    watchdog('xmpp_node_muc', 'ROOM LOG: '. $room .' From: '. $nickname .' Subject: '. $subject .' Body: '. $body, WATCHDOG_NOTICE);
  }
  // not one of our configured rooms hence we do not want to do anything with this record
  if (!($gid = _xmpp_node_muc_group_gid($room))) {
    return FALSE;
  }

  // load the group object so we have some of the pertinent information regarding it
  $group = _xmpp_node_muc_group_load($gid);
  // saving the message in the system so we can retrieve it later if we require it
  db_query("INSERT INTO {xmpp_node_muc_log} (gid, nid, nickname, mdate, subject, body) VALUES (%d, %d, '%s', %d, '%s', '%s')", $gid, $group->nid, $nickname, time(), $subject, $body);

  // if we receive a subject then we should update the subject in the xmpp_node_muc_groups table for display in the block
  if ($subject != '' && strlen($subject) > 0) {
    db_query("UPDATE {xmpp_node_muc_groups} SET subject = '%s' WHERE gid = %d", $subject, $gid);
  }
  return TRUE;
}

?>
