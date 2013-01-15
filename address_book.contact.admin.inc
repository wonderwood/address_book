<?php

function address_book_contact_adminform($form, $form_state) {

  $breadcrumbs   = drupal_get_breadcrumb();
  $breadcrumbs[] = drupal_get_title();
  drupal_set_breadcrumb($breadcrumbs);

  /**
   * Set 0 element of array as "Any"
   * Create $var['id'] = $value
   */
  $vocabulary        = taxonomy_vocabulary_machine_name_load('contact_categories');
  $terms             = taxonomy_get_tree($vocabulary->vid);
  $select_options[0] = t('Any');
  foreach ($terms as $term) {
    $select_options[$term->tid] = $term->name;
  }

  /**
   * If category is not selected, use 0 as value for display all items.
   */
  if (isset($form_state['values']['category_select'])) {
    $current_category = $form_state['values']['category_select'];
  }
  else {
    $current_category = 0;
  }

  $form['category_select']        = array(
    '#type'          => 'select',
    '#title'         => t('Select category of contacts'),
    '#options'       => $select_options,
    '#default_value' => $current_category,
//    '#description' => t(''),
    '#weight'        => 10,
  );
  $form['category_select_filter'] = array(
    '#type'   => 'submit',
    '#value'  => t('Apply'),
    '#submit' => array('address_book_contact_adminform_filter'),
    '#weight' => 15,
  );

  $contacts = db_select('edu_contacts', 'c');
  $contacts = $contacts->fields('c');
  if ($current_category != 0) {
    $contacts = $contacts->condition('category', $current_category);
  }
  $contacts = $contacts->execute();
  $contacts = $contacts->fetchAll();

  /**
   * Content of the tableselect
   */
  $contacts_array = array();
  foreach ($contacts as $row) {
    $edit_link   = l(t('Edit'), "address_book/$row->cid/edit");
    $delete_link = l(t('Delete'), "address_book/$row->cid/delete");

    $view_link = l(t('View'), "address_book/$row->cid");

    $category = taxonomy_term_load($row->category);

    $photo_file = file_load($row->photo);
    if ($photo_file) {
      $photo = theme_image_style(
        array(
          'style_name' => 'contact_preview',
          'path'       => $photo_file->uri,
          'width'      => '',
          'height'     => '',
        ));
    }
    else {
      $photo = '';
    }

    $contacts_array[$row->cid] = array(
      'photo'     => $photo,
      'full_name' => $row->first_name . ' ' . $row->last_name,
      'category'  => $category->name,
      'email'     => $row->email,
      'phone'     => $row->phone,
      'birthday'  => $row->birthday,
      'email'     => $row->email,
      'links'     => $view_link . ' | ' . $edit_link . ' | ' . $delete_link
    );
  }

  /**
   * Tableselect initialization
   */
  $form['table'] = array(
    '#type'    => 'tableselect',
    '#header'  => array(
      'photo'     => t('Photo'),
      'full_name' => t('Full name'),
      'category'  => t('Category'),
      'phone'     => t('Phone number'),
      'birthday'  => t('Birthday'),
      'email'     => t('E-mail'),
      'links'     => t(''),
    ),
    '#options' => $contacts_array,
    '#empty'   => t('No contacts found'),
    '#weight'  => 40,
  );

  $form['submit'] = array(
    '#type'   => 'submit',
    '#value'  => t('Delete selected contacts'),
    '#weight' => 50,
    '#submit' => array('address_book_contact_adminform_delete'),
  );

  if (isset($form_state['storage']['delete'])) {
    foreach ($form_state['values']['table'] as $cid) {
      if ($cid != 0) {
        $cids[] = (int) $cid;
      }
    }

    if (isset($cids) && is_array($cids)) {
      drupal_set_title(t('Contacts delete'));
      $breadcrumbs   = array();
      $breadcrumbs[] = l(t('Home'), url());
      $breadcrumbs[] = l(t('Address book'), 'address_book');
      $breadcrumbs[] = t('Contacts delete');
      drupal_set_breadcrumb($breadcrumbs);

      $form = array();

      $form['cids'] = array(
        '#type'  => 'hidden',
        '#value' => $cids,
      );

      $form['text'] = array(
        '#type'   => 'item',
        '#markup' => t('Confirm you want to delete this items'),
      );

      $contacts_list = '<ul>';
      foreach ($cids as $cid) {
        $contact = address_book_contact_load($cid);
        $contacts_list .= "<li>$contact->first_name $contact->last_name</li>";
      }
      $contacts_list .= '</ul>';

      $form['contacts_list'] = array(
        '#type'   => 'item',
        '#markup' => $contacts_list,
      );

      $form['warning_text'] = array(
        '#type'   => 'item',
        '#markup' => '<strong>'.t('This action can not be undone.').'</strong>',
      );

      $form['submit'] = array(
        '#type'   => 'submit',
        '#value'  => t('Yes, delete'),
        '#submit' => array('address_book_contact_adminform_delete_multiple'),
      );
    }

  }

  return $form;
}

/**
 *  Submitter for admin form.
 * Deletes multiple contacts from database.
 *
 */
function address_book_contact_adminform_delete($form, &$form_state) {
  $form_state['storage']['delete'] = TRUE;
  $form_state['rebuild']           = TRUE;
}

/**
 *  Form rebuild after filtering
 */
function address_book_contact_adminform_filter($form, &$form_state) {
  $form_state['rebuild'] = TRUE;
}

function address_book_contact_adminform_delete_multiple($form, &$form_state) {
  address_book_contact_delete_multiple($form['cids']['#value']);
  drupal_goto('address_book');
}