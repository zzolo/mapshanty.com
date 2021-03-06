// $id Javascript portion for the XMPP Client
var xcpass = '';
/**
 * Creating our namespace for the module so we do not conflict
 * with any other modules that will utilize the same function names
 */
Drupal.xmppclient = {
  ROSTERWINOPTS: 'top=25, width=375, height=450, location=0, scrollbars=1, resizable=1, status=0',
  timeout: 0,
  curl: '',
  presence: ['available', 'unavailable', 'chat', 'busy', 'dnd', 'away', 'xa', 'online', 'offline']
};

if (Drupal.jsEnabled) {
  $(document).ready(function() {
    $('ul li a').each(function() {
      if ($(this).html() == 'Start Client') {
        var url = $(this).attr('href');
        $(this).attr('href', 'javascript:Drupal.xmppclient.menu_login("' + url + '")');
        return false;
      };
    });
  });
}

/**
 * Log person into the Thin XMPP Client on the website
 *
 */
Drupal.xmppclient.login = function() {
  var url = Drupal.settings.xmppclient.login['url'];
  // parsing through the object building the url we will send
  $.each(Drupal.settings.xmppclient.login, function(k, v) {
    if (k == 'url') { return true; }
    url += '&' + k + '=' + escape(v);
  });

  // retrieving the users password then logging them in
  $.get(Drupal.settings.xmppclient.login['srvUrl'] + '/password', function(result) {
    xcpass = result;
    var w = window.open(url, 'XMPPCLIENTROSTERWIN', Drupal.xmppclient.ROSTERWINOPTS);
    w.focus();
    result = '';
  });
  return false;
}

/**
 * @param el {String}
 *      The DOM element we will manipulate if there
 * @param status {String}
 *      The status of the user
 */
Drupal.xmppclient.presence_update = function(el, status) {
  if ($(el).size() > 0) {
    if (status == 'offline' || status == 'unavailable') {
      $(el).each(function() {
        $(this).css({display: 'none'});
        var e = $(this).parent().children('div.xmppclient');
        $.each(xmppclient_presence, function(k, v) {
          e.removeClass(v);
        });
        e.addClass(status);
      });
    } else {
      $(el).each(function() {
        $(this).css({display: 'inline'});
        var e = $(this).parent().children('div.xmppclient');
        $.each(xmppclient_presence, function(k, v) {
          e.removeClass(v);
        });
        e.addClass(status);
      });
    };
  };
};

/**
 * Log the url into the client using the menu link
 *
 * @param url {String}
 *      The url to call in order to start the client
 */
Drupal.xmppclient.menu_login = function(url) {
  $.get(url, function(result) {
    eval(result);
  });
};

/**
 * Log person into the thin client and then open a one on one chat with the jid that is their buddy
 *
 * @param buddy {String}
 *      The jid of the person you wish to initiate the one on one chat with.
 */
Drupal.xmppclient.message_chat = function(buddy) {
  w = window.open('', 'XMPPCLIENTROSTERWIN', Drupal.xmppclient.ROSTERWINOPTS);
  w.blur();
  window.focus();
  if (w.con && w.con.connected()) {
    w.focus();
    w.xcMsgUser(buddy);
  } else {
    w.close();
    // setting the url then removing the key from the object so we do not reparse it
    var url = Drupal.settings.xmppclient.login['url'];
    // parsing the settings object so we can build the url the client will need
    $.each(Drupal.settings.xmppclient.login, function(k, v) {
      if (k == 'url') { return true; }
      url += '&' + k + '=' + escape(v);
    });
    url += '&buddy=' + escape(buddy);

    // retrieving the users password from the system for the client then logging them in
    $.get(Drupal.settings.xmppclient.login['srvUrl'] + '/password', function(result) {
      xcpass = result;
      var w = window.open(url, 'XMPPCLIENTROSTERWIN', Drupal.xmppclient.ROSTERWINOPTS);
      w.focus();
      result = '';
    });
  }
}
