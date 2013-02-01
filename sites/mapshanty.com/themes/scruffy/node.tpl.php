<?php
// $Id: node.tpl.php,v 1.1.2.1 2008/09/24 14:56:36 eaton Exp $

/**
 * @file node.tpl.php
 *
 * Theme implementation to display a node.
 *
 * Available variables:
 * - $title: the (sanitized) title of the node.
 * - $content: Node body or teaser depending on $teaser flag.
 * - $picture: The authors picture of the node output from
 *   theme_user_picture().
 * - $date: Formatted creation date (use $created to reformat with
 *   format_date()).
 * - $links: Themed links like "Read more", "Add new comment", etc. output
 *   from theme_links().
 * - $name: Themed username of node author output from theme_user().
 * - $node_url: Direct url of the current node.
 * - $terms: the themed list of taxonomy term links output from theme_links().
 * - $submitted: themed submission information output from
 *   theme_node_submitted().
 *
 * Other variables:
 * - $node: Full node object. Contains data that may not be safe.
 * - $type: Node type, i.e. story, page, blog, etc.
 * - $comment_count: Number of comments attached to the node.
 * - $uid: User ID of the node author.
 * - $created: Time the node was published formatted in Unix timestamp.
 * - $zebra: Outputs either "even" or "odd". Useful for zebra striping in
 *   teaser listings.
 * - $id: Position of the node. Increments each time it's output.
 *
 * Node status variables:
 * - $teaser: Flag for the teaser state.
 * - $page: Flag for the full page state.
 * - $promote: Flag for front page promotion state.
 * - $sticky: Flags for sticky post setting.
 * - $status: Flag for published status.
 * - $comment: State of comment settings for the node.
 * - $readmore: Flags true if the teaser content of the node cannot hold the
 *   main body content.
 * - $is_front: Flags true when presented in the front page.
 * - $logged_in: Flags true when the current user is a logged-in member.
 * - $is_admin: Flags true when the current user is an administrator.
 *
 * Custom Scruffy theme variables:
 * - $classes: CSS classes for the node itself.
 * - $comments_link: A themed link to the comments page.
 * 
 * @see template_preprocess()
 * @see template_preprocess_node()
 */
?>
<div id="post-<?php print $node->nid; ?>" class="<?php print $classes ?> clear-block">
  <h2 class="title"><a href="<?php print $node_url ?>" title="<?php print $title ?>"><?php print $title ?></a></h2>

  <?php if ($submitted): ?>
  <p class="postmetadata">
    <span class="date"><?php print $postdate ?></span>
    <span class="author"><?php print t('By') . ' ' . $author ?></span>
    <span class="comments"><?php print $comments_link; ?></span>
  </p>
  <?php endif; ?>


  <div class="entry">
    <?php print $content ?>

    <?php if ($terms): ?>
      <p class="tags"><?php print $terms ?></p>
    <?php endif;?>
  </div>

  <?php print $links; ?>
  <div class="postbottom"></div>
</div>
