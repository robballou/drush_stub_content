# Stub content

Tool for creating exported stub content. This can be used in conjunction with Migrate or other tools to pull this content into a Drupal project.

## Options

- content-generation: best set in code (see below)
- count: number of items to generate
- include-id: auto-generate an ID number
- include-title: auto-generate a title
- output: content output mode. Supports CSV, JSON, YAML

## Content generation

You can see an option for content generation, which is probably best to set in a drushrc.php file:

    <?php
    $options['content-generation'] = array(
      'field' => 'function_to_generate_content',
    );

Content generation can either be an array of field -> callback values or a single callback. The callback receives the current field and content (if any).
