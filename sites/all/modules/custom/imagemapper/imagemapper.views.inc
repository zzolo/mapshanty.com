<?php
// $Id$
/**
 * @file
 * Views functions for Image Mapper Module
 *
 * @notes
 *
 * @todo
 */
 

/**
 * Implementation of template_preprocess().
 */
function template_preprocess_imagemapper_view_imagemapper(&$vars) {
  // Rows is actually our map object.
  $vars['map_object'] = $vars['rows'];
  // Remove old var
  unset($vars['rows']);
  // Theme the map.
  $vars['map'] = theme('gmap', array('#settings' => $vars['map_object']));
}