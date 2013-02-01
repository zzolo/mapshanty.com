<?php
// $Id: xmpp_relationships_xmlrpc.inc,v 1.1 2008/10/02 20:57:52 darrenferguson Exp $
/**
 * @description: function updates the users presence in the database
 *
 * @param $jid
 *      Full JID including resource of the user who's status was updated
 * @param $status
 *      Users current status i.e. available, away, do not disturb etc
 * @param $message
 *      Message the user might have left when they set their status
 */
function xmpp_relationships_xmlrpc_update_presence($jid, $status, $message) {

  // not one of our users hence we do not need to do anything with the presence
  if (!($uid = __xmppclient_uid_from_fulljid($jid))) {
    return FALSE;
  }

  $last_access = time(); // retrieving the current epoch so we can add it to the database query.
  db_query("UPDATE {xmppclient_user} SET status = '%s', message = '%s', last_access = %d WHERE uid = %d", $status, $message, $last_access, $uid);
  return TRUE;
}

?>
