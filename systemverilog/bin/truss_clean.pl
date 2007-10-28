sub truss_clean{
  my (@current_command) = @_;

  foreach (@current_command){
    if ($_ eq "NONE") {last;}
    elsif ($_ eq "LOGS") {
      print "Cleaning all $_ files\n";
      $clean_logs_command = "cd $RESULT_DIR; rm -rf *.log";
      truss_system ($clean_logs_command); 
#	sleep(5);
    }

    elsif ($_ eq "CPP") {
	if (uc ($ENV{SIM}) eq "ATSIM") {
	    print "ATSIM doesn't support this option for CPP; hence skipping\n";
#	    sleep(5);
	}
	else {
      print "Cleaning all vip files\n";
      truss_system ("cd $ENV{PROJECT_HOME}/verification/vip ; $MAKE " . ($options{"verbose"} ? "" : "-s") . " SIM=" . lc $ENV{SIM} . " clean");

      print "Cleaning all test component files\n";
      truss_system ("cd $ENV{PROJECT_HOME}/verification/test_components ; $MAKE " . ($options{"verbose"} ? "" : "-s") . " SIM=" . lc $ENV{SIM} . " clean");

      print "Cleaning all testbench files\n";
      truss_system ("cd $ENV{PROJECT_HOME}/verification/testbench/top ; $MAKE " . ($options{"verbose"} ? "" : "-s") . " SIM=" . lc $ENV{SIM} . " clean");

      print "Cleaning all test files \n";
      truss_system ("cd $ENV{PROJECT_HOME}/verification/tests; $MAKE " . ($options{"verbose"} ? "" : "-s") . " SIM=" . lc $ENV{SIM} . " clean");
	}
    }


    elsif ($_ eq "HDL") {
      print "Cleaning all HDL files\n";
#      sleep(5);
      if (uc ($ENV{SIM}) eq "MTI") { $clean_hdl_command = "cd $RESULT_DIR; rm -rf work";}
      elsif (uc ($ENV{SIM}) eq "VCS") { 
	$clean_hdl_command = "cd $RESULT_DIR; rm -rf simv.daidir simv";}
      elsif (uc ($ENV{SIM}) eq "ALDEC") { 
	$clean_hdl_command = "cd $RESULT_DIR; rm -rf hdl/*";}
      elsif (uc ($ENV{SIM}) eq "IVL") { 
	$clean_hdl_command = "cd $RESULT_DIR; rm -rf hdl/*";}
      elsif (uc ($ENV{SIM}) eq "ATSIM") {
	$clean_hdl_command = "cd $RESULT_DIR; rm -rf hdl/*";}
      else {$clean_hdl_command = "Error! Unknown Simulator\n";}
      truss_system ($clean_hdl_command);
    }

    elsif ($_ eq "TRUSS") {
      print "Cleaning all truss files \n";
      if (uc ($ENV{SIM}) eq "ATSIM") {
	truss_system ("cd $ENV{TRUSS_HOME}/bin; rm -rf .athdl_compile_incr/ .athdl_compile/ athdl_sv athdlsv.log hdloffice.log");
#	sleep(5);
	}
      else {
      truss_system ("cd $ENV{TRUSS_HOME}/src; $MAKE " . ($options{"verbose"} ? "" : "-s") . " SIM=" . lc $ENV{SIM} . " clean");
	}
    }

    elsif ($_ eq "TEAL") {
      print "Cleaning all TEAL files here\n";
	if (uc ($ENV{SIM}) eq "ATSIM") {
	  $teal_clean =  "cd $ENV{TEAL_HOME}/test; rm -rf .athdl_compile_incr/ .athdl_compile/ athdl_sv athdlsv.log hdloffice.log";
#	sleep(5);
	}
	else {	
      $teal_clean = "cd $ENV{\"TEAL_HOME\"}; $MAKE " . ($options{"verbose"} ? "" : "-s") . " SIM=" . lc ($ENV{SIM}) . " ARCH=$ENV{\"ARCH\"} clean";
	}
      truss_system ($teal_clean);
    }
    else { croak "ARGH! Error! You shouldn't be here but Cleaning command \'$_\' not understood";}
  }
}

RE;

