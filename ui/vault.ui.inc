<?php

/**
 * Create a list of all vaults
 */
function vault_list() {
  $header = array(
    t('Name'),
    t('Machine Name'),
    t('Operations'),
  );
  $rows = array();
  
  foreach(vault_multiload() as $vault) {
    $links = menu_contextual_links('vault', 'vault', array($vault->machine_name));

    $rows[] = array(
      check_plain($vault->name),
      check_plain($vault->machine_name),
      theme('links', array('links' => $links, 'attributes' => array('class' => 'links operations'))),
    );
  }



  $build['vault_table'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
    '#empty' => t('There are no vaults. <a href="@link">Add a vault</a>.', array('@link' => url('vault/add'))),
  );


  return $build;
}


/**
 * Add or edit a vault
 */
function vault_form($form_state, $vault = array()) {
  $form['#redirect'] = 'vault';

  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#description' => t('The name for this vault.'),
    '#maxlength' => 32,
  );

  $form['machine_name'] = array(
    '#type' => 'machine_name',
    '#title' => t('Machine Name'),
    '#description' => t('The machine-readable name for this vault.'),
    '#maxlength' => 32,
    '#machine_name' => array(
      'exists' => 'vault_exists',
      'source' => array('name'),
    ),
  );

  $form['key'] = array(
    '#type' => 'password',
    '#title' => t('Key'),
    '#description' => t('The key that will be used to encrypt data in this vault and will also be used to unlock it.'),
  );

  $form['description'] = array(
    '#type' => 'textarea',
    '#title' => t('Description'),
    '#description' => t('A description for the contents that should placed inside of this vault.'),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Add Vault'),
  );


  return $form;
}

/**
 * Submit callback
 */
function vault_form_submit($form, $form_state) {
    vault_save($form_state['values']);
    drupal_set_message(t('Your vault has been created.'));
}

function vault_view($vault) {
 
  
  $vault_keyname = $vault->machine_name . '_key';

  // If the vault hasn't been unlocked collect the key
  if (!isset($_SESSION[$vault_keyname])) {
    return drupal_get_form('vault_open_form', $vault);
  }
  else {
    return "The vault is open!";
  }
  // If the vault has been unlocked then allow browsing of vault data and adding vault data
}



function vault_open_form($form_state, $vault) {


  $form['key'] = array(
    '#type' => 'password',
    '#title' => t('Key'),
    '#description' => t('Please enter the key to open this vault.'),
    '#required' => TRUE,
  );

  $form['open'] = array(
    '#type' => 'submit',
    '#value' => t('Unlock'),
  );

  return $form;
}


function vault_open_form_submit($form, $form_state) {
  $key = $form_state['values']['key'];
  $vault = $form_state['build_info']['args'][0];


  if (vault_key_match($vault, $key)) {
    $_SESSION[$vault->machine_name . '_key'] = $key;
    drupal_set_message(t('Vault has been opened'));
  }
  else {
    drupal_set_message(t('Could not open vault.'), 'error');
  }
}