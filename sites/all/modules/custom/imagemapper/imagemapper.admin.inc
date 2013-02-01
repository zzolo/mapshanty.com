<?php
// $Id$
/**
 * @file
 * Admin functions for the Image Mapper
 * 
 * @notes
 *
 * @todo
 */

/**
 * Implemntation of admin setting form
 */
function imagemapper_admin_setting() {
   $form['advanced_view'] = array(
    '#type' => 'fieldset',
    '#title' => t('GMap Image Mapper View Settings'),
    '#collapsible' => true,
    '#collapsed' => false,
  );
  // Default Marker Image
  $form['advanced_view']['imagemapper_default_marker_image'] = array(
    '#type' => 'textfield',
    '#title' => t('Default Marker Path'),
    '#description' => t('Path (relative to site) to the default image to use for markers that do not have images.'),
    '#default_value' => variable_get('imagemapper_default_marker_image', drupal_get_path('module', 'imagemapper').'/images/default.png'),
    '#maxlength' => 1024,
    '#required' => false,
  );
  // Default Image Cache Preset
  $form['advanced_view']['imagemapper_default_imagecache_preset'] = array(
    '#type' => 'select',
    '#title' => t('Default Image Cache Preset'),
    '#description' => t('Default Image Cache Preset.'),
    '#default_value' => variable_get('imagemapper_default_imagecache_preset', '0'),
    '#options' => imagemapper_get_image_cache_presets(),
    '#required' => false,
  );
  
  return system_settings_form($form);
}