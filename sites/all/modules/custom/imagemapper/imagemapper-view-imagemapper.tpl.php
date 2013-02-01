<?php
// $Id$
/**
 * @file imagemapper-view-imagemapper.tpl.php
 * Default view template for a GMap Image Mapper.
 *
 * - $map contains a themed map object.
 * - $map_object contains an unthemed map object.
 *
 * @ingroup views_templates
 */
?>
<?php if (!empty($title)) : ?>
  <h3><?php print $title; ?></h3>
<?php endif; ?>

<?php print $map; ?>

<pre>
<?php print_r($map_object); ?>
<?php print_r($var_dump); ?>
</pre>