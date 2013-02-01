// Variables
var pWin = window.opener || window;
var jid = '', alias = '', presence = '', timer = 0, maxscroll = 0, currentscroll = 0;

// adding the style sheet based off the information we have in the client settings
document.write('<link rel="stylesheet" type="text/css" href="' + pWin.XC.xcfontsize + '" />');

// extending the string object
String.prototype.substitute = function(was, becomes) {
  return this.split(was).join(becomes);
};

/**
 * Set message information and then activate timeout to clear it
 * @param {String} msg The message to set
 * @action sets the message and calls the timeout to clear it
 */
function xcSetMsg(msg, error) {
  msg = '<div>' + msg + '</div>';
  if (error) {
    JQ('#msg').addClass('xcError').append(msg).show();
  } else {
    JQ('#msg').removeClass('xcError').append(msg).show();
  }
  if (timer) { clearTimeout(timer); timer = 0; };
  timer = setTimeout(function() { JQ('#msg').removeClass('xcError').html('').hide(); timer = 0; }, pWin.XC.ERRORTIMEOUT);
};

/**
 * @param {Integer} top The current position of the top of the scrollbar
 */
function xcScrollSave(top) {
  if (top > maxscroll) { maxscroll = top; }
  currentscroll = top;
};

/**
 * Posts the contents of the chat to the server
 */
function xcPostChat() {
  if (!pWin.XC.srvUrl) { return false; };
  var body = new Array();
  var participants = new Array(jid, pWin.XC.fjid);
  var tarray = new Array();
  // retrieve the conversation information from the window
  JQ('#msg_pane.xcMsgPane div.xcChatMessage').each(function() {
    var timestamp = JQ(this).find('.xcChatMessageHeader .xcChatMessageTimestamp').html();
    var name = JQ(this).find('.xcChatMessageHeader .xcChatMessageSender').html();
    if (timestamp) {
      name = name + ' (' + timestamp + ')';
      tarray.push(timestamp);
    };
    var data = name + ': ' + JQ(this).children('p.xcChatMessageBody').html();
    body.push(data);
  });
  // check if the timestamps were accurate if not then leave them empty
  var begin_time = tarray[0]; // format is YYYY-MM-DD HH24:MI:SS
  var end_time = tarray[tarray.length - 1]; // format is YYYY-MM-DD HH24:MI:SS
  pWin.XC.pWin.$('#xcMUCPostForm #chat_type').val('single_chat');
  pWin.XC.pWin.$('#xcMUCPostForm #begin_time').val(begin_time);
  pWin.XC.pWin.$('#xcMUCPostForm #end_time').val(end_time);
  pWin.XC.pWin.$('#xcMUCPostForm #participants').val(participants.join('\r\n'));
  pWin.XC.pWin.$('#xcMUCPostForm #body').val(body.join('\r\n'));
  pWin.XC.pWin.document.forms['xcMUCPostForm'].submit();
  return false;
};

/**
 * Create unique name for the MUC chat and then send the necessary information to the server
 * in order for the chat room to be created
 */
function xcMUCChatInitiate() {
  var chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
  var room = '';
  for (var x = 0; x < 10; x++) {
    i = Math.floor(Math.random() * 52);
    room += chars.charAt(i);
  };
  // create the room on the xmpp server
  try {
    var p = new JSJaCPresence();
    p.setTo(room + '@' + pWin.XC.MUC + '/' + pWin.XC.nickname);
    p.appendNode(p.buildNode('x', {xmlns: pWin.NS_MUC_USER}));
    pWin.con.send(p, window.xcCreateRoomVrfy);
  } catch (e) {
    xcSetMsg(room + pWin.xcT(' creation failed') + ': ' + e.message, true);
  };
};

/**
 * @param {String} room
 *      Chat room to send the messages too
 */
function xcMsgChatHistoryToMUC(room) {
  if (!room) { return false; };
  JQ('div.xcChatMessage p.xcChatMessageBody').each(function() {
    var body = JQ(this).html();
    if (body) {
      pWin.xcMsgSend(body, room, null, 'groupchat');
    };
  });
}

/**
 * @param {JSJaCPresence} p Presence packet letting us know room was successfully created
 */
function xcCreateRoomVrfy(p) {
  try {
    // room successfully created hence unlock the room
    var room = pWin.xcJID(p.getFrom(), false);
    var iq = new JSJaCIQ();
    iq.setTo(room);
    iq.setType('set');
    iq.appendNode(iq.buildNode('query', {xmlns: pWin.NS_MUC_OWNER},
                  [iq.buildNode('x', {xmlns: pWin.NS_XDATA, type: 'submit'})]));
    pWin.con.send(iq);
    pWin.xcMUCInviteLaunch(room, pWin.XC.nickname); // launches the Group Chat
    // invite the current user to the new group chat room
    var imessage = pWin.xcT('Changing to Group Chat room');
    var m = new JSJaCMessage();
    m.setTo(room);
    m.appendNode(m.buildNode('x', {xmlns: pWin.NS_MUC_USER},
                 [m.buildNode('invite', {to: jid},
                 [m.buildNode('reason', imessage),
                  m.buildNode('continue')])]));
    pWin.con.send(m);
    // send chat history to the room so we know we have it
    xcMsgChatHistoryToMUC(room);
    window.close(); // closing this chat window since it is no longer needed
  } catch (e) {
    xcSetMsg(e.message, true);
  };
};

/*
 * Performs message window initialization for the client
 */
function xcMsgInit() {
  if ((jid = JQ(document).getUrlParam('jid'))) {
    JQ('#jid').val(jid);
    if ((contact = pWin.xcContactExists(jid))) {
      JQ('#alias').html(contact.getName());
      xcSetContactPresence(JQ('#xcContactTitle'), contact.getShow(), contact.getStatus());
    } else {
      if (pWin.xcJID(jid, false).match(/conference/)) {
        alias = pWin.xcJID(jid, true);
      } else {
        alias = pWin.xcJID(jid, false);
      }; // either system message or message from a one on one chat via group chat
      JQ('#alias').html(alias);
      xcSetContactPresence(JQ('#xcContactTitle'), 'available');
    };
  } else {
    xcSetMsg(pWin.xcT('No Chat ID information'), true);
    setTimeout('window.close()', pWin.XC.ERRORTIMEOUT);
  };
  // check if the send button should be displayed or not
  if (pWin.XC.xc_sendbutton == 0) {
    JQ('#xcMsgContainer').addClass('no-button');
  };
  // check if the link for saving the chat log should be displayed
  if (!(pWin.XC.srvUrl)) {
    JQ('#xcSaveChatLogLink').hide();
  };
};

/**
 * Puts the message on the screen for the user
 */
function xcMessage(m) {
  // if translation is not turned on then just get the body and display it
  if (!(pWin.XC.xc_translation)) {
    xcMessageShow(m.getBody().htmlEnc(), null); // display the message to the user
  } else {
    marray = new Array(); // array to store all of the incoming messages depending on language
    JQ(m.getDoc()).children().children('body').each(function() {
      var body = JQ(this).text().htmlEnc(); // getting the body text for the message
      if (!(lang = JQ(this).attr('xml:lang'))) { lang = 'en'; }; // determine if a language was specified if not then default to english
      marray[lang] = body; // set the array with the pertinent language and the translation for that language
    });
    // now we check the message array and see if the language we are looking for is there
    // this is if the message packet came in with xml:lang set in the body tag then we get from there
    if (marray[pWin.XC.xc_translation_lge]) {
      xcMessageShow(marray[pWin.XC.xc_translation_lge], m.getBody().htmlEnc()); // display the translation that came in to the user
    } else {
      // step through the array retrieving just the first entry and translating it into the appropriate language
      for (var lang in marray) {
        xcTranslateString(marray[lang], lang); // translate then display the string
        break;
      };
    };
  };
};

/**
 * Actually display the incoming message to the person
 * @param {String} body
 *      The body text of the message that came in
 * @param {String} pretrans
 *      The data before translation
 */
function xcMessageShow(body, pretrans) {
  var alias = JQ('#alias').html();
  var html = '<div class="xcChatMessage received">' +
             '<div class="xcChatMessageHeader">' +
             '<span class="xcChatMessageTimestamp">' + (pWin.XC.xc_showtimestamps == 1 ? pWin.xcDate() : '') + '</span>' +
             '<span class="xcChatMessageSender">' + alias + '</span> ' +
             '</div>';
  html += '<p class="xcChatMessageBody">' + body + '</p>';
  html += '</div>';
  // if this is 0 then the DOM still has not fully loaded hence re-run the function
  if (JQ('#msg_pane').size() == 0) {
    setTimeout(function() { xcMessageShow(body, pretrans); }, 1000);
  } else {
    JQ('#msg_pane').append(html).children(':last-child').each(function() {
      if ((currentscroll / maxscroll) < pWin.XC.scrollthreshold ) { return false; };
      this.scrollIntoView(false);
    });
  };
  // if we are running translation this will show the pre-translation information
  if (pretrans) {
    var html = '<div class="xcChatMessage received">' +
               '<div class="xcChatMessageHeader">' +
               '<span class="xcChatMessageTimestamp">' + (pWin.XC.xc_showtimestamps == 1 ? pWin.xcDate() : '') + '</span>' +
               '<span class="xcChatMessageSender">' + alias + '</span> ' +
               '</div>';
    html += '<p class="xcChatMessageBody">' + pretrans + '</p>';
    html += '</div>';
    if (JQ('#translated_msg_pane').size() == 1) {
      JQ('#translated_msg_pane').append(html).children(':last-child').each(function() {
        if ((currentscroll / maxscroll) < pWin.XC.scrollthreshold ) { return false; };
        this.scrollIntoView(false);
      });
    }
  }
  // putting the focus on this window
  window.focus();
};

/**
 * @param {String} string
 *      The string that we are going to translate
 * @param {String} lang
 *      Base language that the string is in
 */
function xcTranslateString(string, lang) {
  JQ.ajax({
    type: 'POST',
    url: '/cgi-bin/trans/' + lang + '/' + pWin.XC.xc_translation_lge,
    data: string,
    processData: false, // dont make data into query string
    contentType: 'text/plain',
    dataType: 'text', // in return we expect the response to be text
    success: function(translated) { xcMessageShow(translated, string); },
    error: function(xhr, status, thrown) { pWin.oDbg.log('Failed: ' + status); }
  });
};

/**
 * Sets the presence of the user based off the presence packets coming in
 * @param {JSJaCPresence} p Presence packet with information pertaining to the user presence
 */
function xcPresence(p) {
  var jid = pWin.xcJID(p.getFrom(), false);
  if ((contact = pWin.xcContactExists(jid))) {
    var show = contact.getShow();
    if (show == presence) { return true; };
    xcSetContactPresence(JQ('#xcContactTitle'), show, contact.getStatus());
    var html = '<div class="xcSystemMessage">' + pWin.XC.pMsgs[show] + '</div>';
    JQ('#msg_pane').append(html).children(':last-child').each(function() {
      if ((currentscroll / maxscroll) < pWin.XC.scrollthreshold ) { return false; };
      this.scrollIntoView(false);
    });
    // if we have translation set then we need to put this information in also
    if (pWin.XC.xc_translation) {
      JQ('#translated_msg_pane').append(html).children(':last-child').each(function() {
        if ((currentscroll / maxscroll) < pWin.XC.scrollthreshold ) { return false; };
        this.scrollIntoView(false);
      });
    }
    presence = show;
  };
};

/**
 * Sends the message to the server for delivery to the appropriate person
 */
function xcMsgSend() {
  if ((body = JQ('#body').val()) == '') { return false; };
  // do checks to determine if it is just empty lines or not
  if (body.substitute(' ', '').substitute('\r\n', '').substitute('\n', '').substitute('\r', '').length == 0) {
    JQ('#body').val(''); // clear the message box for the next message
    JQ('#body')[0].focus(); // reset focus back onto the message box
    return false;
  };
  pWin.xcMsgSend(body, JQ('#jid').val(), null, null); // send the message to the server
  // add the message to the message pane on the screen
  var html = '<div class="xcChatMessage sent">' +
             '<div class="xcChatMessageHeader">' +
             '<span class="xcChatMessageTimestamp">' + (pWin.XC.xc_showtimestamps == 1 ? pWin.xcDate() : '') + '</span>' +
             '<span class="xcChatMessageSender">' + pWin.XC.ujid + '</span>' +
             '</div>' +
             '<p class="xcChatMessageBody">' + body + '</p>' +
             '</div>';
  JQ('#msg_pane').append(html).children(':last-child').each(function() {
    if ((currentscroll / maxscroll) < pWin.XC.scrollthreshold ) { return false; };
    this.scrollIntoView(false);
  });
  // if we have translation set then we need to put this information in also
  if (pWin.XC.xc_translation) {
    // add the message to the message pane on the screen
    var html = '<div class="xcChatMessage sent">' +
               '<div class="xcChatMessageHeader">' +
               '<span class="xcChatMessageTimestamp">' + (pWin.XC.xc_showtimestamps == 1 ? pWin.xcDate() : '') + '</span>' +
               '<span class="xcChatMessageSender">' + pWin.XC.ujid + '</span>' +
               '</div>' +
               '<p class="xcChatMessageBody">' + body + '</p>' +
               '</div>';
    JQ('#translated_msg_pane').append(html).children(':last-child').each(function() {
      if ((currentscroll / maxscroll) < pWin.XC.scrollthreshold ) { return false; };
      this.scrollIntoView(false);
    });
  }

  // clear the message box and reset focus onto it
  JQ('#body').val('');
  JQ('#body')[0].focus();
};

/**
 * Sets right click information regarding the users name so you can get menus based off that
 */
function xcRightClickName() {
  JQ('#xcContactMenuTrigger').mousedown(function(e) {
    var buttoncode = e.which ? e.which : e.button; // msie specific checks does not support e.which
    var pageX = e.pageX ? e.pageX : e.clientX; // msie specific checks does not support e.page
    var pageY = e.pageY ? e.pageY : e.clientY; // msie specific checks does not support e.page
    //if (buttoncode != 1) {
      JQ('#xcRightMenu').css({ top: pageY + 'px', left: pageX + 'px' }).show();
      JQ(document).one("click" , function() { JQ('#xcRightMenu').hide(); });
    //};
    // setting the click menu functionality for the right click menu on the roster
    JQ('.xcContextMenu > ul > li').unbind().click(function() {
      if (this.id == 'archive') {
        pWin.xcViewUserLog(jid); // view archived information for that user
      } else if (this.id == 'info') {
        pWin.xcUserInfo(jid); // get user vcard information
      } else if (this.id == 'getpresence') {
        pWin.xcSendPresenceType(jid, 'probe'); // get the users current online presence
      } else if (this.id == 'setpresence') {
        pWin.xcWinUpdatePresenceOpen(jid);
      } else if (this.id == 'remove') {
        if ((contact = pWin.xcContactExists(jid))) {
          if (confirm(pWin.xcT('Are you sure you wish to remove the user from your roster?'))) {
            xcDeleteContact(jid); // remove the contact from your roster
          };
        } else {
          xcSetMsg(pWin.xcT('Contact is not in your roster'), true);
        };
      } else if (this.id == 'resubscribe') {
        if ((contact = pWin.xcContactExists(jid))) {
          if (contact.getSubscription() != 'both') {
            xcSendPresenceType(jid, 'subscribe'); // send subscription presence to the user
            xcSetMsg(xcT('Resubscription sent'), false);
          };
        } else {
          xcSetMsg(pWin.xcT('Contact is not in your roster'), true);
        };
      } else if (this.id == 'update') {
        if ((contact = pWin.xcContactExists(jid))) {
          pWin.xcWinUpdateUserOpen(jid); // open the update user window
        } else {
          xcSetMsg(pWin.xcT('Contact is not in your roster'), true);
        };
      };
    }).mouseover(function() { JQ(this).addClass('xcOver'); }).mouseout(function() { JQ(this).removeClass('xcOver'); });
  });
};

/**
 * Initializing the message window making sure all variables and blocks are set
 */
JQ(document).ready(function(){
  xcMsgInit();
  xcRightClickName(); // setting the right click functionality
  JQ('#body').keypress(function(e) {
    var keycode = e.charCode ? e.charCode : e.keyCode ? e.keyCode : 0;
    if (keycode == 13) {
      xcMsgSend(); // enter key pressed
      return false;
    };
    return true;
  });
  JQ('#body').resizable(); // making the textarea resizable for the user
  JQ('#body').focus(); // putting focus onto the body
});

/**
 * Making sure the window is removed from the open windows list on closing
 */
JQ(window).unload(function(){
  try { pWin.xcWinClose(pWin.xcDec(self.name)); } catch (e) {}
});

function xcSetContactPresence(contact, show, status) {
  var contactElement = contact.children('.presence');
  JQ.each(pWin.XC.pIcon, function(k, v) {
    contactElement.removeClass(k);
  })
  contact.children('.xcContactStatus').html(status);
  contactElement.addClass(show);
}
