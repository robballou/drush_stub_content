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
    return drush_typeinfo_list();
  }

  $type_info = entity_get_info($entity_type);
  if (!isset($type_info['bundles'][$type])) {
    drush_log(t('The requested type does not exist. Try: drush typeinfo-list'), 'error');
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

  $data = stub_content_build_data_array($fields);
  $output = $output_function($data);
  print $output;
}

function stub_content_build_data_array($fields) {
  $include_id = drush_get_option('include-id', FALSE);
  $include_title = drush_get_option('include-title', FALSE);

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
    }
    array_push($data, $this_item);
  }
  return $data;
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
      $output = yaml_emit($row);
    }
  }

  return $output;
}
