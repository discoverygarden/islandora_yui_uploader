<?php
//TODO: maybe move this into a module and link to this from a drupal menu path as
//we would still not have the session id in the header but we could proably rebuild it
//if we send the session as a postvar
//not sure below whether to use the form_build_id or form_token
//form_build_id may not be stable enough.
$yui_tmp_directory_name = $_POST['yui_form_build_id'];
$drupal_tmp_dir = $_POST['dr_tmp_dir'];
$new_directory = $drupal_tmp_dir.'/'.$yui_tmp_directory_name;
//create a new directory based on the form_id and then put the files there so we can act on them when the form is submitted
if(isset($drupal_tmp_dir)){
  if(!file_exists($new_directory)){
    $directory_ok = mkdir($new_directory);
  }
} else {
  //TODO: should do something here like return an error to the form uploader
  exit();
}
//if(!$directory_ok){//have to see how this failed before we exit as it looks like we get multiple posts so the directory may have already been created
  //TODO: should do something here like return an error to the form uploader
//  exit();
//}
foreach ($_FILES as $fieldName => $file) {
move_uploaded_file($file['tmp_name'], $new_directory .'/'. $file['name']);
echo (" ");
}
//we've put the file in the correct location so we have to wait for a form to be submitted to act on them
//there will be times when someone uploads the files but does not submit the form so we would need a cron
//or something to clean the tmp directory.  Maybe we should create a yuiupload directory in the temp directory
//then we would only have to worry about it.
exit();?>


