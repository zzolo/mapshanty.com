<?php
// $Id: comment.tpl.php,v 1.1.2.1 2008/09/24 14:56:36 eaton Exp $

/**
 * @file comment.tpl.php
 * Default theme implementation for comments.
 *
 * Available variables:
 * - $author: Comment author. Can be link or plain text.
 * - $content: Body of the post.
 * - $date: Date and time of posting.
 * - $links: Various operational links.
 * - $new: New comment marker.
 * - $picture: Authors picture.
 * - $signature: Authors signature.
 * - $status: Comment status. Possible values are:
 *   comment-unpublished, comment-published or comment-preview.
 * - $submitted: By line with date and time.
 * - $title: Linked title.
 *
 * These two variables are provided for context.
 * - $comment: Full comment object.
 * - $node: Node object the comments are attached to.
 *
 * @see template_preprocess_comment()
 * @see theme_comment()
 */
?>
<li id="comment-<?php print $comment->cid ?>" class="<?php print $classes ?>">
  <div class="commentmeta clearfix">
    <div class="gravatar">
    <?php print $picture ?>
    </div>
    <span class="commentauthor"><?php print $author ?></span><br />
    <span class="commentdate"><?php print $date ?></span>
  </div>

  <div class="commententry">
          
    <?php if ($status == 'comment-unpublished') : ?>
    <p class="moderate"><?php print t('Your comment is awaiting moderation.'); ?></p>
    <?php endif; ?>
    
    <?php print $content ?>
    <?php if ($signature): ?>
    <div class="user-signature clear-block">
      <?php print $signature ?>
    </div>
    <?php endif; ?>
    
    <?php print $links ?>
  </div><!-- /commententry -->
<li>
