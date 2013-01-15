<?php

function address_book_contact_view($contact) {
  $title = "$contact->first_name $contact->last_name";
  drupal_set_title($title);

  $breadcrumbs   = drupal_get_breadcrumb();
  $breadcrumbs[] = drupal_get_title();
  drupal_set_breadcrumb($breadcrumbs);

  $contact->photo_file_obj = file_load($contact->photo);
  if ($contact->photo_file_obj) {
    $contact->photo_file_image = theme_image_style(
      array(
        'style_name' => 'medium',
        'path'       => $contact->photo_file_obj->uri,
        'width'      => '',
        'height'     => '',
      ));
  }
  else {
    $contact->photo_file_image = t('NO PHOTO');
  }

  $contact->category_obj  = taxonomy_term_load($contact->category);
  $contact->category_name = $contact->category_obj->name;

  $content['photo'] = array(
    '#type'   => 'item',
    '#markup' => $contact->photo_file_image,
  );

  $content['first_name'] = array(
    '#type'   => 'item',
    '#title'  => t('First name'),
    '#markup' => $contact->first_name,
  );

  $content['last_name'] = array(
    '#type'   => 'item',
    '#title'  => t('Last name'),
    '#markup' => $contact->last_name,
  );

  $content['email'] = array(
    '#type'   => 'item',
    '#title'  => t('E-mail'),
    '#markup' => $contact->email,
  );

  $content['phone'] = array(
    '#type'   => 'item',
    '#title'  => t('Phone number'),
    '#markup' => $contact->phone,
  );

  $content['category'] = array(
    '#type'   => 'item',
    '#title'  => t('Category'),
    '#markup' => $contact->category_name,
  );

  // @todo: improve this field
  $content['birthday'] = array(
    '#type'   => 'item',
    '#title'  => t('Birthday'),
    '#markup' => $contact->birthday,
  );

  $content['comment'] = array(
    '#type'   => 'item',
    '#title'  => t('comment'),
    '#markup' => $contact->comment,
  );

  return $content;
}