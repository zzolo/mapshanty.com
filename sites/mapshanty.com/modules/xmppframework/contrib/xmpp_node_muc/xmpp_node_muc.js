// $id Javascript portion for the XMPP NODE MUC
Drupal.xmpp_node_muc = {
  ROSTERWINOPTS: 'top=25,width=375,height=450,location=0,scrollbars=1,resizable=1,status=0'
};

var xcpass = '';

if (Drupal.jsEnabled) {
  $(document).ready(function() {
    $('.xmpp_node_muc a').click(function(e) {
      var nid = $(this).attr('nid');
      var url = $(this).attr('url');
      var attribute = $(this);
      $.get(url + '/' + nid, function(result) { eval(result); });
      return false;
    });
  });
}

/**
 * @param gid
 */
Drupal.xmpp_node_muc.online_users_toggle = function(gid) {
  var name = '#xmpp_node_muc_online_' + gid + ' div#xmpp_node_muc_online_show_' + gid + ' a';
  var stoggle = '#xmpp_node_muc_online_' + gid + ' div#xmpp_node_muc_online_users_' + gid;
  var text = $(name).html();
  $(stoggle).slideToggle();
  if (text == 'Show') {
    $(name).html('Hide');
  } else {
    $(name).html('Show');
  }
}

/**
 * @param gid
 */
Drupal.xmpp_node_muc.activity_msgs_toggle = function(gid) {
  var name = '#xmpp_node_muc_activity_' + gid + ' div#xmpp_node_muc_activity_show_' + gid + ' a';
  var logname = '#xmpp_node_muc_activity_' + gid + ' div#xmpp_node_muc_activity_log_' + gid;
  var text = $(name).html();
  $(logname).slideToggle();
  if (text == 'Show') {
    $(name).html('Hide');
  } else {
    $(name).html('Show');
  }
}

/**
 * @param nid
 *      Node id that we will retrieve the information from
 * @param url
 *      Url where to go and get the information from
 * @param interval
 *      Time between AJAX block refreshes
 */
Drupal.xmpp_node_muc.refresh_block = function(nid, url, interval) {
  setTimeout(function() {
    $.get(url + '/' + nid, function(result) {
      // only try and do something if the result length we receive is larger than 0
      if (result.length > 0) {
        eval(result);
      };
      // continue block refreshing
      Drupal.xmpp_node_muc.refresh_block(nid, url, interval);
    })
  }, interval);
}

/**
 * @param group
 *      Full jid of the group we wish to join
 */
Drupal.xmpp_node_muc.group_chat = function(group) {
  w = window.open('', 'XMPPCLIENTROSTERWIN', Drupal.xmpp_node_muc.ROSTERWINOPTS);
  w.blur();
  window.focus();
  if (w.con && w.con.connected()) {
    w.focus();
    w.xcMUCInviteLaunch(group, Drupal.settings.xmppclient.login['username']);
  } else {
    w.close();

    var url = Drupal.settings.xmppclient.login['url'];
    // parsing through the object building the url we will send
    $.each(Drupal.settings.xmppclient.login, function(k, v) {
      if (k == 'url') { return true; }
      url += '&' + k + '=' + escape(v);
    });
    url += '&nickname=' + Drupal.settings.xmppclient.login['username'];
    url += '&muc=' + group;

    $.get(Drupal.settings.xmppclient.login['srvUrl'] + '/password', function(result) {
      xcpass = result;
      var w = window.open(url, 'XMPPCLIENTROSTERWIN', Drupal.xmpp_node_muc.ROSTERWINOPTS);
      w.focus();
      result = '';
    });
  }
}

/**
 * @param gid
 *      Group id being utilized
 * @param group_name
 *      Group name for the organic group
 * @param subject
 *      Current subject for the muc
 * @param online_status
 *      Online Status for the person in the group
 */
Drupal.xmpp_node_muc.ajax_group_info_refresh = function(gid, group_name, subject, online_status) {
  // setting the pertinent variables with the x-path for the elements we wish to alter
  var gname = '#xmpp_node_muc_group_list_' + gid + ' .xmpp_node_muc_group_list_item div#xmpp_node_muc_gid_' + gid + '.xmpp_node_muc_group_list_item_name div.name span.message';
  var sub = '#xmpp_node_muc_group_list_' + gid + ' .xmpp_node_muc_group_list_item div#xmpp_node_muc_gid_' + gid + '.xmpp_node_muc_group_list_item_name div.subject span.message';
  var ol = '#xmpp_node_muc_group_list_' + gid + ' .xmpp_node_muc_group_list_item div#xmpp_node_muc_gid_' + gid + '.xmpp_node_muc_group_list_item_name div.online span.message';
  var join_group = '#xmpp_node_muc_group_list_' + gid + ' .xmpp_node_muc_group_list_item div#xmpp_node_muc_gid_' + gid + '.xmpp_node_muc_group_list_item_name div.xmpp_node_muc_group_join';

  // this means that the current html there does not have the group id appended so we need to fix this
  if ($(gname).size() == 0) {
    // need to remove the old ones before putting the new ones in
    if ($('#xmpp_node_muc_group_list_').size() == 0) {
      $('.xmpp_node_muc_group_list_item_name').attr('id', 'xmpp_node_muc_gid_');
      $('.xmpp_node_muc_group_list').attr('id', 'xmpp_node_muc_group_list_');
    };
    $('#xmpp_node_muc_group_list_ .xmpp_node_muc_group_list_item div#xmpp_node_muc_gid_').attr('id', 'xmpp_node_muc_gid_' + gid);
    $('#xmpp_node_muc_group_list_').attr('id', 'xmpp_node_muc_group_list_' + gid);
  }

  $(gname).html(group_name);
  $(sub).html(subject);
  $(ol).html(online_status);
  if (online_status == 'Participating') {
    $(join_group).slideUp('slow');
  } else {
    $(join_group).slideDown('slow');
  }
}

/**
 * @param online
 *      Number of online users in the group
 * @param users
 *      Html to display the online users
 */
Drupal.xmpp_node_muc.ajax_online_muc_users_refresh = function(gid, online, users) {
  // setting the online muc users portions
  var name = '#xmpp_node_muc_online_count_' + gid + ' span.message';
  var title = '#xmpp_node_muc_online_count_' + gid + ' span.title';
  var ol = '#xmpp_node_muc_online_show_' + gid;
  var olu = '#xmpp_node_muc_online_users_' + gid;
  // checking if the element exists or not
  if ($(name).size() == 0) {
    // need to remove the old ones before putting the new ones in
    if ($('#xmpp_node_muc_online_').size() == 0) {
      $('.xmpp_node_muc_online_user').remove();
      $('.xmpp_node_muc_online_users').attr('id', 'xmpp_node_muc_online_users_');
      $('.xmpp_node_muc_online_show a').attr('href', 'javascript:Drupal.xmpp_node_muc.online_users_toggle(\'\');');
      $('.xmpp_node_muc_online_show').attr('id', 'xmpp_node_muc_online_show_');
      $('.xmpp_node_muc_online_count').attr('id', 'xmpp_node_muc_online_count_');
      $('.xmpp_node_muc_online').attr('id', 'xmpp_node_muc_online_');
    };
    $('#xmpp_node_muc_online_show_ a').attr('href', 'javascript:Drupal.xmpp_node_muc.online_users_toggle(\'' + gid + '\');');
    $('#xmpp_node_muc_online_count_').attr('id', 'xmpp_node_muc_online_count_' + gid);
    $('#xmpp_node_muc_online_show_').attr('id', 'xmpp_node_muc_online_show_' + gid);
    $('#xmpp_node_muc_online_users_').attr('id', 'xmpp_node_muc_online_users_' + gid);
    $('#xmpp_node_muc_online_').attr('id', 'xmpp_node_muc_online_' + gid);
  }

  $(name).html(online);
  if (online == 0) {
    $(title).html('Online Users:');
    $(ol).hide();
    $(olu).html('');
  } else {
    $(title).html('Active Participants:');
    $(ol).show();
    $(olu).html(users);
  }
}

/**
 * @param gid
 *      Group id for the group
 * @param mdate
 *      Last message date if there is one
 * @param logs
 *      MUC Logs for the system
 */
Drupal.xmpp_node_muc.log_refresh = function(gid, mdate, logs) {
  var name = '#xmpp_node_muc_activity_' + gid + ' div#xmpp_node_muc_activity_last_' + gid + ' span.message';
  var logname = '#xmpp_node_muc_activity_' + gid + ' div#xmpp_node_muc_activity_log_' + gid;
  var activity = '#xmpp_node_muc_activity_' + gid + ' div#xmpp_node_muc_activity_show_' + gid;
  // checking if the element exists or not
  if ($(name).size() == 0) {
    // need to remove the old ones before putting the new ones in
    if ($('#xmpp_node_muc_activity_').size() == 0) {
      $('.xmpp_node_muc_activity_log').attr('id', 'xmpp_node_muc_activity_log_');
      $('.xmpp_node_muc_activity_show a').attr('href', 'javascript:Drupal.xmpp_node_muc.activity_msgs_toggle(\'\');');
      $('.xmpp_node_muc_activity_show').attr('id', 'xmpp_node_muc_activity_show_');
      $('.xmpp_node_muc_activity_last').attr('id', 'xmpp_node_muc_activity_last_');
      $('.xmpp_node_muc_activity').attr('id', 'xmpp_node_muc_activity_');
    };
    $('#xmpp_node_muc_activity_ div#xmpp_node_muc_activity_last_').attr('id', 'xmpp_node_muc_activity_last_' + gid);
    $('#xmpp_node_muc_activity_ div#xmpp_node_muc_activity_log_').attr('id', 'xmpp_node_muc_activity_log_' + gid);
    $('#xmpp_node_muc_activity_ div#xmpp_node_muc_activity_show_ a').attr('href', 'javascript:Drupal.xmpp_node_muc.activity_msgs_toggle(\'' + gid + '\');');
    $('#xmpp_node_muc_activity_ div#xmpp_node_muc_activity_show_').attr('id', 'xmpp_node_muc_activity_show_' + gid);
    $('#xmpp_node_muc_activity_').attr('id', 'xmpp_node_muc_activity_' + gid);
  }

  $(name).html(mdate);
  $(logname).html(logs);
  if (mdate == 'Never') {
    $(activity).hide();
  } else {
    $(activity).show();
  }
}

