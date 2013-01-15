<?php
/**
 * Delete form initialization
 *
 * @param type $form_state
 *
 * @return $form array
 */
function address_book_contact_deleteform($form, $form_state, $contact) {
  /**
   * Check if is in database
   */
  if (isset($contact->cid) && !is_null($contact->cid)) {
    $title = t('Delete @first_name @last_name', array(
        '@first_name' => $contact->first_name,
        '@last_name'  => $contact->last_name,
      )
    );

    drupal_set_title($title);
    $breadcrumbs   = array();
    $breadcrumbs[] = l(t('Home'), url());
    $breadcrumbs[] = l(t('Address book'), 'address_book');
    $breadcrumbs[] = l($contact->first_name . ' ' . $contact->last_name, "address_book/$contact->cid");
    $breadcrumbs[] = t('Delete');

    drupal_set_breadcrumb($breadcrumbs);


    $form['cid']             = array(
      '#type'  => 'hidden',
      '#value' => $contact->cid,
    );

    $form['form_intro_text'] = array(
      '#markup' => t('Are you sure about delete %fist_name %last_name contact?',
        array(
          '%fist_name' => $contact->first_name,
          '%last_name' => $contact->last_name,
        ))
    );
      
    $form['warning_text'] = array(
      '#type'   => 'item',
      '#markup' => '<strong>'.t('This action can not be undone.').'</strong>',
    );
    
    $form['submit']          = array(
      '#type'  => 'submit',
      '#value' => t('Yep, delete it.'),
    );
  }
  else {
    $form['form_error_text'] = array(
      '#markup' => t('There is no contact with id = %cid', array('%cid' => $contact))
    );
  }

  return $form;
}

/**
 *  Submitter for delete form
 *
 * @param type $form_id
 * @param type $form_values
 */
function address_book_contact_deleteform_submit($form_id, $form_values) {
  $contact = address_book_contact_load($form_values['values']['cid']);

  // delete contact
  $delete_contact_query = db_delete('edu_contacts')
    ->condition('cid', $contact->cid)
    ->execute();

  //delete photo
  $photo = file_load($contact->photo);
  file_delete($photo);

  drupal_set_message(t('Contact deleted successful'));
  drupal_goto('address_book');
}