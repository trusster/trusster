function dictionary_impl::new ();
   //ARRGH because of no statics dictionary cannot have a logger!
   //This is to allow the logger to use the dictionary to see what level the user wants to debug at
   //This log feature is far more important than logger in the dictionary implementtaion class
//   log_ = new ("Dictionary");

endfunction // dictionary_impl

function string dictionary_impl::teal_scan_plusargs (string name);
   string returned;
`ifdef ncsim
   bit found;
   found  = $value$plusargs ({name, "+%s"},returned);
`else
   bit found = $value$plusargs ({name, "+%s"},returned);
`endif
//   $display ("plus args search for %s found %s", name, returned);

  if (found) return returned; else return "";
endfunction 


function string dictionary_impl::find_on_command_line ( string name,  string default_name);
`ifdef ncsim
  string arg;
  arg = teal_scan_plusargs (name);
`else
  string arg = teal_scan_plusargs (name);
`endif
  return (arg != "") ? arg : default_name;
endfunction



task dictionary_impl::process_file_ (string path);
`ifdef ncsim
    integer file_id;
   file_id = $fopen (path, "r");
`else
    integer file_id = $fopen (path, "r");
`endif
//   log_.debug ({"Process file: ", path});
$display ({"Process file: ", path});
//   if (file_id == 0) log_.error ({"unable to open file ", path}); return; end
   if (file_id == 0) begin $display ({"unable to open file ", path});    return; end  //non error now ;-(
   while (! $feof (file_id)) 
     begin
	//get frst word
	string param ;
	byte c;
	string value;
	integer unused;

	unused = $fscanf (file_id, "%s", param);
	if (param.len ()) begin
`ifndef ATHDL_SIM
	   c = $fgetc (file_id);  //eat the space between symbol and value
`endif
	   unused = $fgets (value, file_id);
	   value = value.substr (0, value.len() - 2);
	end
	
//	$display ($psprintf ("got \"%s\" and \"%s\" ", param, value));

	if (param == "#include") begin
	   process_file_ (value);
	end
	else begin
	   lines_[param] = value;
//	      $display ("lines22[%s] is \"%s\"", param, lines_[param]);
	end
     end // while (! feof (file_id))
//      log_.debug ({"Completed process file: ", path});
   $display ({"Completed process file: ", path});
endtask

task dictionary_impl::read ( string path);
   process_file_ (path);
endtask


task dictionary_impl::clear ();
   lines_.delete ();
endtask

function bit dictionary_impl::put ( string name,  string value, input bit replace_existing);
`ifdef ncsim
  bit returned;
  returned = (find (name) != "");
`else
  bit returned = (find (name) != "");
`endif
  if ( (! returned) || (replace_existing)) begin
    lines_[name] = value;
  end
  return returned;
endfunction

function string dictionary_impl::find ( string name); 
`ifdef ncsim
  string arg;
  arg = teal_scan_plusargs (name);
`else
  string arg = teal_scan_plusargs (name);
`endif
//   $display ("%t lines \"%s\" is \"%s\" command line is \"%s\"", $time, name, lines_[name], arg);
   
  return (arg != "") ? arg : lines_[name];
endfunction

function integer dictionary_impl::find_integer ( string name, integer default_value);
`ifdef ncsim
   string value;
   integer returned;
   integer scan_count;

   value = find (name);
   scan_count = 0;
`else
   string value = find (name);
   integer returned;
   integer scan_count = 0;
`endif
   if (name != "") begin
      scan_count = $sscanf (value, "%d", returned);
   end
//   $display ("%t lines \"%s\" is \"%s\" ret is %0d", $time, name, lines_[name],  (scan_count == 1) ? returned : default_value);   
   return (scan_count == 1) ? returned : default_value;
endfunction
