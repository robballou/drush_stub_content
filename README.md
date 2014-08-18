# Stub content

Tool for creating exported stub content. This can be used in conjunction with Migrate or other tools to pull this content into a Drupal project.

## Usage

    drush stub-content article --include-id --include-title --count=4

## Options

- content-generation: best set in code (see below)
- count: number of items to generate
- include-id: auto-generate an ID number
- include-title: auto-generate a title
- output: content output mode. Supports CSV, JSON, YAML
- skip-content-generation: do not generate content. This does not affect the id/title options.

## Content generation

You can see an option for content generation, which is probably best to set in a drushrc.php file:

    <?php
    $options['content-generation'] = array(
      'field' => 'function_to_generate_content',
    );

Content generation can either be an array of field -> callback values or a single callback. The callback receives the current field and content (if any).

## Todo

* Identify when a field relates to another entity (node, taxonomy term, etc.). It would be great if this could be used to pull ID values randomly from other stubbed content as well.
* Allow it to pull existing references from the database if desired (e.g., use terms that exist on the site)

## Related projects

* [Devel generate](http://drupal.org/project/devel)
* [Realistic Dummy Content](http://drupal.org/project/realistic_dummy_content)
