<?php
// $id

/**
 * Builds display blocks for the temporary mucs associated with nodes
 */
function theme_xmpp_node_muc_block_temporary($node) {
  // if there is an active conversation going on then display that information instead of the archived conversations
  $result = db_fetch_object(db_query("SELECT * FROM {xmpp_node_muc_groups} WHERE nid = %d", $node->nid));
  // retrieving all of the xmpp groups associated with this organic group
  $output .= _xmpp_node_muc_group_list($node->nid, $result->gid, false);
  // retrieve the number of online users for the group
  $output .= _xmpp_node_muc_online_muc_users($node->nid, $result->gid, false);
  // retrieve the muc log to be displayed on the screen
  $output .= _xmpp_node_muc_group_muc_log($node->nid, $result->gid, false);
  // do not display anything if there are no currently active mucs
  if (!(_xmpp_node_muc_count_mucs_from_nid($node->nid))) {
    $output .= '<script type="text/javascript">$(document).ready(function() { $(".xmpp_node_muc_group_list").slideUp(); $(".xmpp_node_muc_online").slideUp(); $(".xmpp_node_muc_activity").slideUp(); $(".block-xmpp_node_muc").slideUp(); });</script>';
  }
  else {
    $output .= '<script type="text/javascript">$(document).ready(function() { $(".xmpp_node_muc_group_list").slideDown(); $(".xmpp_node_muc_online").slideDown(); $(".xmpp_node_muc_activity").slideDown(); $(".block-xmpp_node_muc").slideDown(); });</script>';
  }
  // check if the ajax refresh is configured in the module
  if (XMPP_NODE_MUC_BLOCK_REFRESH) {
    $output .= "<script type='text/javascript'>Drupal.xmpp_node_muc.refresh_block('". $node->nid ."', '". url('xmpp_node_muc/block_refresh/temporary') ."','". XMPP_NODE_MUC_BLOCK_REFRESH_INTERVAL ."');</script>";
  }
  return $output;
}

/**
 * Builds display blocks for the permanent mucs associated with nodes
 */
function theme_xmpp_node_muc_block_permanent($node) {
  $output = '';
  // check if the muc exists since it might have been created without one
  if (!(_xmpp_node_muc_count_mucs_from_nid($node->nid))) {
    return $output;
  }

  $result = db_query("SELECT * FROM {xmpp_node_muc_groups} WHERE nid = %d ORDER BY sub_group ASC", $node->nid);
  while ($row = db_fetch_object($result)) {
    // retrieving all of the xmpp groups associated with this organic group
    $output .= _xmpp_node_muc_group_list($node->nid, $row->gid, false);

    // retrieve the number of online users for the group
    $output .= _xmpp_node_muc_online_muc_users($node->nid, $row->gid, false);

    // retrieve the muc log to be displayed on the screen
    $output .= _xmpp_node_muc_group_muc_log($node->nid, $row->gid, false);
  }
  // check if the ajax refresh is configured in the module
  if (XMPP_NODE_MUC_BLOCK_REFRESH) {
    $output .= "<script type='text/javascript'>Drupal.xmpp_node_muc.refresh_block('". $node->nid ."', '". url('xmpp_node_muc/block_refresh/permanent') ."','". XMPP_NODE_MUC_BLOCK_REFRESH_INTERVAL ."');</script>";
  }
  // add the javascript so that the blocks will be displayed since we currently have them display none because of temporary nodes
  $output .= "<script type='text/javascript'>$(document).ready(function() { $('.xmpp_node_muc_group_list').show(); $('.xmpp_node_muc_online').show(); $('.xmpp_node_muc_activity').show(); });</script>";

  return $output;
}
