<?php
// $Id: template.php,v 1.1.2.1 2008/09/24 14:56:36 eaton Exp $

/**
 * @file template.php
 *
 * Theme override and template preprocessing functions.
 */

function scruffy_preprocess_page(&$vars) {
  $vars['main_feed'] = l(t('Subscribe'), 'rss.xml', array('attributes' => array('class' => 'subscribe')));

  if (empty($vars['footer_message'])) {
    $vars['footer_message'] = '&copy; ' . $vars['site_name'] . ' 2008 | Designed by  <a title="Fresh01 - Freelance Web Design" href="http://fresh01.co.za">FRESH01</a>';
  }
}

function scruffy_preprocess_node(&$vars) {
  // Set up CSS classes for the node so we don't have to do it in the template.
  static $first;
  $vars['classes'] = array('post');
  $vars['classes'][] = 'post-' . str_replace('_', '-', $vars['node']->type);
  if (!isset($first)) {
    $first = TRUE;
    $vars['classes'][] = 'first';
  }
  if ($vars['sticky']) {
    $vars['classes'][] = 'sticky';
  }
  if ($vars['status']) {
    $vars['classes'][] = 'post-unpublished';
  }
  $vars['classes'] = implode(' ', $vars['classes']);
  
  // Populate more granular submitted-by information.
  $vars['postdate'] = format_date($vars['node']->created, 'custom', 'd F Y');
  $vars['author'] = theme('username', $vars['node']);
  
  $vars['comments_link'] = l(
    $vars['comment_count'], 'node/' . $vars['node']->nid,
    array(
      'fragment' => 'comment',
      'title' => t('Comment on !title', array('!title' => $vars['node']->title))
    )
  );
}

function scruffy_preprocess_search_theme_form(&$vars) {
  $vars['theme_directory'] = drupal_get_path('theme', 'scruffy');
}

function scruffy_preprocess_comment_wrapper(&$vars) {
  $node = $vars['node'];
  $vars['header'] = t('<strong>!count comments</strong> on %title', array('!count' => $node->comment_count, '%title' => $node->title));
}

function scruffy_preprocess_comment(&$vars) {
  $vars['classes'] = array('comment');
  if ($vars['zebra'] == 'odd') {
    $vars['classes'][] = 'alt';
  }
  if ($vars['comment']->uid == $vars['node']->uid) {
    $vars['classes'][] = 'authorcomment';
  }
  $vars['classes'] = implode(' ', $vars['classes']);
}
