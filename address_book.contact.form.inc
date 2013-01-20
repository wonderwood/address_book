<?php

/**
 * Implements contact add/edit form
 */
function address_book_contactform($form, $form_state, $contact = NULL) {
  drupal_set_title(t('Add contact'));
  $breadcrumbs = drupal_get_breadcrumb();
  $breadcrumbs[] = drupal_get_title();
  drupal_set_breadcrumb($breadcrumbs);

  if (isset($contact->cid)) {
    drupal_set_title(t("Edit contact #@contact_id", array('@contact_id' => $contact->cid)));
    $breadcrumbs = array();
    $breadcrumbs[] = l(t('Home'), url());
    $breadcrumbs[] = l(t('Address book'), 'address_book');
    $breadcrumbs[] = l($contact->first_name . ' ' . $contact->last_name, "address_book/$contact->cid");
    $breadcrumbs[] = t('Edit');
    drupal_set_breadcrumb($breadcrumbs);
  }

  $form['form_intro_text'] = array(
    '#markup' => t('Contact add form intro text, what explains something to user.'),
    '#weight' => 0,
  );
  $form['photo'] = array(
    '#title' => t('Photo'),
    '#type' => 'managed_file',
    '#description' => t('Upload contact`s photo.'),
    '#upload_location' => 'public://contact_photos/',
    '#weight' => 10,
  );
  $form['first_name'] = array(
    '#type' => 'textfield',
    '#title' => t('First name'),
    '#size' => 60,
    '#maxlength' => 256,
    '#required' => TRUE,
    '#weight' => 20,
  );
  $form['last_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Last name'),
    '#size' => 60,
    '#maxlength' => 256,
    '#required' => TRUE,
    '#weight' => 30,
  );
  $form['email'] = array(
    '#type' => 'textfield',
    '#title' => t('e-mail'),
    '#size' => 60,
    '#maxlength' => 256,
    '#required' => TRUE,
    '#weight' => 40,
  );
  $form['phone'] = array(
    '#type' => 'textfield',
    '#title' => t('Phone'),
    '#size' => 21,
    '#maxlength' => 256,
    '#required' => TRUE,
    '#weight' => 50,
  );

  $vocabulary = taxonomy_vocabulary_machine_name_load('contact_categories');
  $terms = taxonomy_get_tree($vocabulary->vid);
  foreach ($terms as $term) {
    $termsarray[$term->tid] = $term->name;
  }

  $form['birthday'] = array(
    '#type' => 'date',
    '#title' => t('Birthday'),
    '#default_value' => time(),
    '#description' => t('Your birth day'),
    '#required' => TRUE

  );
  /*
  '#type'                => 'date',
  '#default_value'       => time(),
  '#title'               => t('Birthday'),
  '#date_format'         => 'd-m-Y',
  '#date_label_position' => 'within',
  '#date_year_range'     => '-100:0',
  '#required'            => TRUE,
);*/

  $form['category'] = array(
    '#type' => 'select',
    '#title' => t('Category'),
    '#options' => $termsarray,
    '#description' => t('Select category of contact.'),
    '#required' => TRUE,
    '#weight' => 70,
  );

  $form['comment'] = array(
    '#type' => 'textarea',
    '#title' => t('Note'),
    '#weight' => 80,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Add contact'),
    '#weight' => 90,
  );


  /**
   * check for $cid is present
   */
  if (!is_null($contact)) {
    /**
     * Query to the DB and check if contact in DB
     * Set default values for form
     * Add hidden field with $cid
     * Add delete link
     * Rewrite submit button text
     */
    if (isset($contact->cid)) {

      $form['cid'] = array(
        '#type' => 'hidden',
        '#value' => $contact->cid,
        '#weight' => 150,
      );

      $form['delete'] = array(
        '#markup' => l(t('Delete contact'), "admin/address_book/$contact->cid/delete"),
        '#weight' => 100,
      );

      $form['first_name']['#default_value'] = $contact->first_name;
      $form['last_name']['#default_value'] = $contact->last_name;
      $form['email']['#default_value'] = $contact->email;
      $form['phone']['#default_value'] = $contact->phone;
      $form['photo']['#default_value'] = $contact->photo;
      $form['comment']['#default_value'] = $contact->comment;
      $form['birthday']['#default_value'] = $contact->birthday;
      $form['category']['#default_value'] = $contact->category;

      $form['submit']['#value'] = t('Edit contact');
    }
  }

  return $form;
}

function address_book_contactform_validate(&$form, &$form_state) {
  if (!valid_email_address($form_state['values']['email'])) {
    form_set_error('email', 'Enter email in correct format! address@domain.com');
  }
}

/**
 *  Implements contact add form submit
 */
function address_book_contactform_submit($form_id, $form_values) {
  global $user;
  $current_user_uid = $user->uid;

  /**
   * if there is contact id, it means a contact is in db, and we want to edit contact.
   */
  dpm($form_values);
  dpm($form_values['values']['birthday']['month']);
  dpm($form_values['values']['birthday']['day']);
  dpm($form_values['values']['birthday']['year']);
  $date = mktime(1,1,1,$form_values['values']['birthday']['month'],$form_values['values']['birthday']['day'],$form_values['values']['birthday']['year']);
  dpm($date);
  $contact = array(
    'first_name' => $form_values['values']['first_name'],
    'last_name' => $form_values['values']['last_name'],
    'email' => $form_values['values']['email'],
    'phone' => $form_values['values']['phone'],
    'photo' => $form_values['values']['photo'],
    'category' => $form_values['values']['category'],
    'birthday' => $date,
    'comment' => $form_values['values']['comment'],
  );

  if (isset($form_values['values']['cid'])) {
    $contact['cid'] = $form_values['values']['cid'];

    drupal_write_record('edu_contacts', $contact, 'cid');

    drupal_set_message(t('Contact edited'));
    drupal_goto('address_book');
  }

  /**
   * If there is no hidden field than insert form into database
   */
  else {
    drupal_write_record('edu_contacts', $contact);
    drupal_set_message(t('Contact "@first_name @last_name" added', array(
          '@first_name' => $contact['first_name'],
          '@last_name' => $contact['last_name'],
        )
      )
    );
  }
}