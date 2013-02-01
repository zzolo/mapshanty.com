<?php
// $Id$
/**
 * @file
 * Views Plugin Style Class file for the Image Mapper Module
 *
 * @notes
 *
 * @todo
 */

/**
 * Style plugin to render a map.
 *
 * @ingroup views_style_plugins
 */
class imagemapper_plugin_style_imagemapper extends views_plugin_style {
  /**
   * Set Default Options
   */
  function option_definition() {
    // Set default options
    $options = parent::option_definition();
    $options = array(
      'macro' => array(
        'default' => '[gmap ]',
      ),
      'datasource' => array(
        'default' => 'location',
      ),
      'markers' => array(
        'default' => 'static',
      ),
      'markertype' => array(
        'default' => 'default',
      )
    );

    // Return options
    return $options;
  }
  
  /**
   * Query function
   */
  function query() {
    // Get lat and long from location
    if ($this->options['datasource'] == 'location') {
      $this->view->query->add_field('location', 'latitude', 'gmap_lat');
      $this->view->query->add_field('location', 'longitude', 'gmap_lon');
    }
    // Get nodetype
    if ($this->options['markers'] == 'nodetype') {
      $this->view->query->add_field('node', 'type', 'gmap_node_type');
    }
    // Get imagefield data
    //$this->view->query->add_field('files', $this->options['marker_field'], 'imagefield_data');
  }
  
  /**
   * Render function
   */
  function render() {
    // Check row plugin
    if ($this->uses_row_plugin() && empty($this->row_plugin)) {
      // Views debug
      vpr('imagemapper_plugin_style_imagemapper: Missing row plugin');
      return;
    }

    // Set default marker
    $markername = 'drupal';

    $markertypes = variable_get('gmap_node_markers', array());
    if ($this->options['markers'] == 'nodetype') {
      $markertypes = variable_get('gmap_node_markers', array());
    }

    // Group the rows according to the grouping field, if specified.
    $sets = $this->render_grouping($this->view->result, $this->options['grouping']);
    // Render each group separately and concatenate.  Plugins may override this
    // method if they wish some other way of handling grouping.
    $output = '';
    // Go through sets/groups
    foreach ($sets as $title => $records) {
      // Intialize
      $markers = array();
      $offsets = array();
      // Go through records
      foreach ($records as $label => $row) {
        // Get lat/long
        $lat = (float)$row->gmap_lat;
        $lon = (float)$row->gmap_lon;
        // Check if lat/long
        if (!empty($lat) && !empty($lon)) {
          // Check markers
          if ($this->options['markers'] == 'nodetype') {
            // Set markername
            $markername = isset($markertypes[$row->gmap_node_type]) ? $markertypes[$row->gmap_node_type] : 'drupal';
          }
          // Check offset of marker
          if (!isset($offsets[$markername])) {
            $offsets[$markername] = 0;
          }
          // Create Text
          if ($this->uses_row_plugin()) {
            $text = $this->row_plugin->render($row);
          } else {
            $text = 'Hi'; //print_r($row, true);
          };
          
          // Get image path
          $imagepath = db_result(db_query('select filepath from {files} where fid = %d', $row->node_data_field_node_image_field_node_image_fid));
          $imageurl = file_create_url(imagepath);
          
          // Change image if preset
          if (module_exists('imagecache') && $this->options['default_preset']) {
            // Get preset name (this shoudl be static)
            $preset = imagecache_preset($this->options['default_preset']);
            $imageurl = imagecache_create_url($preset['presetname'], $imagepath);
            $imagepath = imagecache_create_path($preset['presetname'], $imagepath);
          }
          
          // Get image size
          $image = image_get_info($imagepath);
          
          // Create imagemapper array
          $imagemapper[] = array(
            'latitude' => $lat,
            'longitude' => $lon,
            'offset' => $offsets[$markername],
            'text' => $text,
            'imageurl' => $imageurl,
            'imagepath' => $imagepath,
            'iconsize' => array(
              'x' => $image['width'],
              'y' => $image['width'],
            ),
            'iconanchor' => array(
              'x' => '20',
              'y' => '23',
            ),
            'iconwindowanchor' => array(
              'x' => '20',
              'y' => '12',
            ),
          );
          // Create marker array
          $markers[] = array(
            'latitude' => $lat,
            'longitude' => $lon,
            'markername' => $markername,
            'offset' => $offsets[$markername],
            'text' => $text,
          );
          // Count offset per marker
          $offsets[$markername]++;
        }
      }
      // Check marker, Don't draw empty maps.
      if (!empty($imagemapper)) {
        // Get intial map
        $map = gmap_parse_macro($this->options['macro']);
        // Add markers
        $map['markers'] = $markers;
        $map['imagemapper'] = $imagemapper;
        // Add JS data
        drupal_add_js("
          Drupal.imagemapper = new Object; 
          Drupal.imagemapper.markers = " . drupal_to_js($imagemapper) . ";\n", 'inline');
        // Get next map id
        $map['id'] = gmap_get_auto_mapid();
        // Add to output
        $output .= theme($this->theme_functions(), $this->view, $this->options, $map, $title);
        // krumo($this);
        // krumo($row);
        // krumo($m);
      }
    }
    
    // return full output
    return $output;
  }

  /**
   * Define option form for plugin
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $handlers = $this->display->handler->get_handlers('field');
    if (empty($handlers)) {
      $form['error_markup'] = array(
        '#value' => t('You need at least one field before you can configure your table settings'),
        '#prefix' => '<div class="error form-item description">',
        '#suffix' => '</div>',
      );
      return;
    }
    // Create options for fields, get only fields that are from image module
    $field_names = array();
    foreach ($handlers as $field => $handler) {
      if ($handler->content_field['module'] == 'imagefield') {
        if ($label = $handler->label()) {
          $field_names[$field] = $label;
        } else {
          $field_names[$field] = $handler->ui_name();
        }
      }
    }
    if (empty($field_names  )) {
      $form['error_markup'] = array(
        '#value' => t('You need at least one field that is an ImageField before you can configure your table settings'),
        '#prefix' => '<div class="error form-item description">',
        '#suffix' => '</div>',
      );
      return;
    }
    
    // Create form items
    $form['macro'] = array(
      '#type' => 'textfield',
      '#title' => t('Macro'),
      '#description' => t('This is the macro used for the map view.'),
      '#size' => 1000,
      '#default_value' => $this->options['macro'],
    );
    // Data source for lat and long
    $form['datasource'] = array(
      '#type' => 'radios',
      '#title' => t('Data Source'),
      '#options' => array(
        'location' => t('Location.module'),
      ),
      '#default_value' => $this->options['datasource'],
      '#multiple' => false,
      '#description' => t('Source of coordinate data'),
    );
    // Field to use for Marker
    $form['marker_field'] = array(
      '#type' => 'select',
      '#title' => t('Marker Field'),
      '#description' => t('Choose which field to use for Marker.'),
      '#default_value' => $this->options['marker_field'],
      '#options' => $field_names,
      '#required' => true,
    );
    // Default Marker Image
    $form['default_marker'] = array(
      '#type' => 'textfield',
      '#title' => t('Default Marker Path'),
      '#description' => t('Path (relative to site) to the default image to use for markers that do not have images.'),
      '#default_value' => ($this->options['default_marker']) ? $this->options['default_marker'] : variable_get('imagemapper_default_marker_image', ''),
      '#maxlength' => 1024,
    );
    // Default Image Cache Preset
    if (module_exists('imagecache')) {
      $form['default_preset'] = array(
        '#type' => 'select',
        '#title' => t('Image Cache Preset'),
        '#description' => t('Image cache preset to render image field.'),
        '#default_value' => ($this->options['default_preset']) ? $this->options['default_preset'] : variable_get('imagemapper_default_imagecache_preset', '0'),
        '#options' => imagemapper_get_image_cache_presets(),
        '#required' => false,
      );
    }
        
    watchdog('debug', '<pre>!debug</pre>', array('!debug' => print_r($handlers, true)), WATCHDOG_DEBUG);
  }
  
}
