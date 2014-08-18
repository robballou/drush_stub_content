<?php

/**
 * Implements hook_function().
 */
function stub_content_drush_command() {
  $commands = array();

  $commands['stub-content'] = array(
    'callback' => 'drush_stub_content',
    'description' => 'Output YAML for a content type',
    'aliases' => array('ssc'),
    'examples' => array(),
    'arguments' => array(
      'type' => 'The bundle name',
      'entity_type' => 'The entity type. Defaults to node',
      'count' => 'The number of items',
    ),
    'options' => array(
      'content-generation' => array(
        'description' => 'Options for generating content. Usually set in a configuration file.',
      ),
      'count' => array(
        'description' => 'Number of items to create',
        'default' => 1,
      ),
      'include-id' => array(
        'description' => 'Include an auto-generated ID in the id field',
      ),
      'include-title' => array(
        'description' => 'Include an auto-generated title in the title field',
      ),
      'output' => array(
        'default' => 'yaml',
        'description' => 'Output type',
      ),
    ),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL,
  );

  return $commands;
}

/**
 * List fields in an entity/bundle
 *
 * @param string $type
 *   A string for the bundle.
 * @param string $entity_type
 *   The entity type for this bundle. Defaults to node.
 */
function drush_stub_content($type = NULL, $entity_type = 'node') {
  // if no types are passed, output types
  if (!$type) {
    drush_log(t('No type specified'), 'error');
    if (function_exists('drush_typeinfo_list')) {
      return drush_typeinfo_list();
    }
    return;
  }

  $type_info = entity_get_info($entity_type);
  if (!isset($type_info['bundles'][$type])) {
    drush_log(t('The requested type does not exist. If you have drush_typeinfo, use: drush typeinfo-list'), 'error');
    return;
  }

  $label = $type_info['bundles'][$type]['label'];
  $field_info = field_info_instances($entity_type, $type);
  $fields = array(
    'id',
    'title'
  );

  foreach ($field_info as $field => $info) {
    array_push($fields, $field);
  }

  $output_function = 'stub_content_output_' . drush_get_option('output', 'yaml');
  if (!function_exists($output_function)) {
    drush_log(t('Invalid output type'), 'error');
    return;
  }

  $data = stub_content_build_data_array($type, $fields);
  $output = $output_function($data);
  print $output;
}

/**
 * Build an array data structure for output
 */
function stub_content_build_data_array($type, $fields) {
  $include_id = drush_get_option('include-id', FALSE);
  $include_title = drush_get_option('include-title', FALSE);

  $content_generation = drush_get_option('content-generation', array());

  $data = array();
  foreach (range(1, drush_get_option('count', 1)) as $item) {
    $this_item = array();
    foreach ($fields as $field) {
      // default to blank value
      $this_item[$field] = '';

      // if we enumerating ids, use the row number
      if ($include_id && $field === 'id') {
        $this_item[$field] = $item;
      }

      // if we are including titles, do that
      elseif ($include_title && $field === 'title') {
        $this_item[$field] = ucfirst(str_replace('_', ' ', $type)) . ' ' . $item;
      }

      // call the callback for this field
      $callback = NULL;
      if (isset($content_generation[$field])) {
        $callback = $content_generation[$field];
      }
      elseif (is_string($content_generation) && is_callable($content_generation)) {
        $callback = $content_generation;
      }
      if ($callback) {
        $this_item[$field] = call_user_func($callback, $this_item[$field]);
      }
    }
    array_push($data, $this_item);
  }
  return $data;
}

/**
 * Generate content
 */
function stub_content_generate_content() {
  $content = '';
  if (function_exists('devel_create_content')) {
    $content = devel_create_content();
  }
  else {
    // get a quote
    $quote = file_get_contents('http://www.iheartquotes.com/api/v1/random');
    $content = str_replace(array("\r\n", "\r", "\n"), " ", $quote);
  }
  return $content;
}

/**
 * Output CSV for stubbed content
 */
function stub_content_output_csv($data) {
  ob_start();
  $df = fopen('php://output', 'w');
  foreach ($data as $row) {
    fputcsv($df, $row);
  }
  $output = ob_get_contents();
  ob_end_clean();
  return $output;
}

/**
 * Output YAML for stubbed content
 */
function stub_content_output_yaml($data) {
  $output = '';

  foreach ($data as $row) {
    if (function_exists('yaml_emit')) {
      $output .= yaml_emit($row);
    }
  }

  return $output;
}
