<?php

class IslandoraYuiUploader {

  /**
   *
   * @param string $allowed_file_types
   * @return string
   */
  function get_allowed_files_string($allowed_file_types = NULL){
    if(!isset($allowed_file_types)){
      return '{description:"Images", extensions:"*.jpg;*.png;*.gif"},{description:"Videos", extensions:"*.avi;*.mov;*.mpg"}';//arbitraryly chosen default
    }
    return $allowed_file_types;
  }

  /**
   * @param array $form_values
   * @param array $allowed_file_types
   * @return string
   * add required js files and somd of the js as a string here so we can inject paths like
   * YAHOO.widget.Uploader.SWFURL = "/'.drupal_get_path('module','islandora_yui_uploader').'/assets/uploader.swf";
   * var uploader = new YAHOO.widget.Uploader( "uploaderUI","/'.drupal_get_path('module','islandora_yui_uploader').'/js/assets/selectFileButton.png" );
   *
   * Also creates a div so the multifile uploader has a place to exist
   */
  function islandora_yui_uploader_add_js(&$form_state, $allowed_file_types = NULL) {
    //$form_build_id = $form_values['form_token'];
    $tmp_directory = file_directory_temp();
    $allowed_files_string = $this->get_allowed_files_string($allowed_file_types);
    //create a unique id here, we can't use form_build_id as it is per form build not per form so
    //the form_build_id could change per form, the form token is also based on session id
    //so can be the same for different forms
    //$form_build_id = 'yui-form-' . drupal_hash_base64(uniqid(mt_rand(), TRUE) . mt_rand());
    $form_build_id = 'yui-form-' . drupal_get_token(mt_rand());
    $form_state['values']['yui_form_build_id'] = $form_build_id; //so we can use this when we submit the form
    global $user;
    drupal_add_js(drupal_get_path('module', 'islandora_yui_uploader') . '/js/yahoo-dom-event.js');
    drupal_add_js(drupal_get_path('module', 'islandora_yui_uploader') . '/js/element-min.js');
    drupal_add_js(drupal_get_path('module', 'islandora_yui_uploader') . '/js/uploader.js');
    drupal_add_js(drupal_get_path('module', 'islandora_yui_uploader') . '/js/yuiloader-min.js');
    drupal_add_js(drupal_get_path('module', 'islandora_yui_uploader') . '/js/event-min.js');
    drupal_add_js(drupal_get_path('module', 'islandora_yui_uploader') . '/js/dom-min.js');
    drupal_add_js(drupal_get_path('module', 'islandora_yui_uploader') . '/js/logger-min.js');
    drupal_add_js(drupal_get_path('module', 'islandora_yui_uploader') . '/js/uploader-debug.js');
    drupal_add_js(drupal_get_path('module', 'islandora_yui_uploader') . '/js/datasource-min.js');
    drupal_add_js(drupal_get_path('module', 'islandora_yui_uploader') . '/js/event-delegate-min.js');
    drupal_add_js(drupal_get_path('module', 'islandora_yui_uploader') . '/js/datatable-min.js');
    drupal_add_js(drupal_get_path('module', 'islandora_yui_uploader') . '/js/button-min.js');

    drupal_add_css(drupal_get_path('module', 'islandora_yui_uploader') . '/css/upload_button.css');
    drupal_add_css(drupal_get_path('module', 'islandora_yui_uploader') . '/css/logger.css');
    drupal_add_css(drupal_get_path('module', 'islandora_yui_uploader') . '/css/datatable.css');
    $div = '<strong>Please upload files before clicking Submit</strong><br /><div id="uiElements" style="display:inline;">
		<div id="uploaderContainer">
			<div id="uploaderOverlay" style="position:absolute; z-index:2"></div>
			<div id="selectFilesLink" style="z-index:1"><a id="selectLink" href="#">Select Files</a></div>
		</div>
		<div id="uploadFilesLink"><a id="uploadLink" onClick="upload(); return false;" href="#">Upload Files</a></div>
</div>

<div id="simUploads"> Number of simultaneous uploads:
	<select id="simulUploads">
		<option value="1">1</option>
		<option value="2">2</option>
		<option value="3">3</option>
		<option value="4">4</option>
	</select>
</div>

<div id="dataTableContainer"></div>';

    $js = '<script type="text/javascript">

YAHOO.util.Event.onDOMReady(function () {
var uiLayer = YAHOO.util.Dom.getRegion(\'selectLink\');
var overlay = YAHOO.util.Dom.get(\'uploaderOverlay\');
YAHOO.util.Dom.setStyle(overlay, \'width\', uiLayer.right-uiLayer.left + "px");
YAHOO.util.Dom.setStyle(overlay, \'height\', uiLayer.bottom-uiLayer.top + "px");
});

	// Custom URL for the uploader swf file (same folder).
	YAHOO.widget.Uploader.SWFURL = "/' . drupal_get_path('module', 'islandora_yui_uploader') . '/assets/uploader.swf";

    // Instantiate the uploader and write it to its placeholder div.
	var uploader = new YAHOO.widget.Uploader( "uploaderOverlay" );

	// Add event listeners to various events on the uploader.
	// Methods on the uploader should only be called once the
	// contentReady event has fired.

	uploader.addListener(\'contentReady\', handleContentReady);
	uploader.addListener(\'fileSelect\', onFileSelect)
	uploader.addListener(\'uploadStart\', onUploadStart);
	uploader.addListener(\'uploadProgress\', onUploadProgress);
	uploader.addListener(\'uploadCancel\', onUploadCancel);
	uploader.addListener(\'uploadComplete\', onUploadComplete);
	uploader.addListener(\'uploadCompleteData\', onUploadResponse);
	uploader.addListener(\'uploadError\', onUploadError);
    uploader.addListener(\'rollOver\', handleRollOver);
    uploader.addListener(\'rollOut\', handleRollOut);
    uploader.addListener(\'click\', handleClick);

    // Variable for holding the filelist.
	var fileList;

	// When the mouse rolls over the uploader, this function
	// is called in response to the rollOver event.
	// It changes the appearance of the UI element below the Flash overlay.
	function handleRollOver () {
		YAHOO.util.Dom.setStyle(YAHOO.util.Dom.get(\'selectLink\'), \'color\', "#FFFFFF");
		YAHOO.util.Dom.setStyle(YAHOO.util.Dom.get(\'selectLink\'), \'background-color\', "#000000");
	}

	// On rollOut event, this function is called, which changes the appearance of the
	// UI element below the Flash layer back to its original state.
	function handleRollOut () {
		YAHOO.util.Dom.setStyle(YAHOO.util.Dom.get(\'selectLink\'), \'color\', "#0000CC");
		YAHOO.util.Dom.setStyle(YAHOO.util.Dom.get(\'selectLink\'), \'background-color\', "#FFFFFF");
	}

	// When the Flash layer is clicked, the "Browse" dialog is invoked.
	// The click event handler allows you to do something else if you need to.
	function handleClick () {
	}

	// When contentReady event is fired, you can call methods on the uploader.
	function handleContentReady () {
	    // Allows the uploader to send log messages to trace, as well as to YAHOO.log
		uploader.setAllowLogging(true);

		// Allows multiple file selection in "Browse" dialog.
		uploader.setAllowMultipleFiles(true);

		// New set of file filters.
		var ff = new Array('.$allowed_files_string.');

		// Apply new set of file filters to the uploader.
		uploader.setFileFilters(ff);
	}

	// Actually uploads the files. In this case,
	// uploadAll() is used for automated queueing and upload
	// of all files on the list.
	// You can manage the queue on your own and use "upload" instead,
	// if you need to modify the properties of the request for each
	// individual file.
	function upload() {
	if (fileList != null) {
		uploader.setSimUploadLimit(parseInt(document.getElementById("simulUploads").value));
		//uploader.uploadAll("http://www.yswfblog.com/upload/upload_simple.php", "POST", null, "Filedata");
    //we are going to inject the form build id of the form as we upload the files first then submit the form and there is no real link between the two.  We will create a tmp directory based on the form_id then read from that directory when the form is submitted
    //we have to do stuff like this as the actual file upload is based on flash and it is not aware of the sessionid.
    //also inject the drupal tmp directory via post variables
    uploader.uploadAll("/' . drupal_get_path('module', 'islandora_yui_uploader') . '/uploader.php","POST",{dr_tmp_dir:"' . $tmp_directory . '",yui_form_build_id:"' . $form_build_id . '",user:"' . $user->name . '"},"Filedata");
	}
	}

	// Fired when the user selects files in the "Browse" dialog
	// and clicks "Ok".
	function onFileSelect(event) {
		if(\'fileList\' in event && event.fileList != null) {
			fileList = event.fileList;
			createDataTable(fileList);
		}
	}

	function createDataTable(entries) {
	  rowCounter = 0;
	  this.fileIdHash = {};
	  this.dataArr = [];
	  for(var i in entries) {
	     var entry = entries[i];
		 entry["progress"] = "<div style=\'height:5px;width:100px;background-color:#CCC;\'></div>";
	     dataArr.unshift(entry);
	  }

	  for (var j = 0; j < dataArr.length; j++) {
	    this.fileIdHash[dataArr[j].id] = j;
	  }

	    var myColumnDefs = [
	        {key:"name", label: "File Name", sortable:false},
	     	{key:"size", label: "Size", sortable:false},
	     	{key:"progress", label: "Upload progress", sortable:false}
	    ];

	  this.myDataSource = new YAHOO.util.DataSource(dataArr);
	  this.myDataSource.responseType = YAHOO.util.DataSource.TYPE_JSARRAY;
      this.myDataSource.responseSchema = {
          fields: ["id","name","created","modified","type", "size", "progress"]
      };

	  this.singleSelectDataTable = new YAHOO.widget.DataTable("dataTableContainer",
	           myColumnDefs, this.myDataSource, {
	               caption:"Files To Upload",
	               selectionMode:"single"
	           });
	}

    // Do something on each file\'s upload start.
	function onUploadStart(event) {

	}

	// Do something on each file\'s upload progress event.
	function onUploadProgress(event) {
		rowNum = fileIdHash[event["id"]];
		prog = Math.round(100*(event["bytesLoaded"]/event["bytesTotal"]));
		progbar = "<div style=\'height:5px;width:100px;background-color:#CCC;\'><div style=\'height:5px;background-color:#00F;width:" + prog + "px;\'></div></div>";
		singleSelectDataTable.updateRow(rowNum, {name: dataArr[rowNum]["name"], size: dataArr[rowNum]["size"], progress: progbar});
	}

	// Do something when each file\'s upload is complete.
	function onUploadComplete(event) {
		rowNum = fileIdHash[event["id"]];
		prog = Math.round(100*(event["bytesLoaded"]/event["bytesTotal"]));
		progbar = "<div style=\'height:5px;width:100px;background-color:#CCC;\'><div style=\'height:5px;background-color:#00F;width:100px;\'></div></div>";
		singleSelectDataTable.updateRow(rowNum, {name: dataArr[rowNum]["name"], size: dataArr[rowNum]["size"], progress: progbar});
	}

	// Do something if a file upload throws an error.
	// (When uploadAll() is used, the Uploader will
	// attempt to continue uploading.
	function onUploadError(event) {

	}

	// Do something if an upload is cancelled.
	function onUploadCancel(event) {

	}

	// Do something when data is received back from the server.
	function onUploadResponse(event) {

	}
</script>';

    return $div . $js;
  }

}

?>
