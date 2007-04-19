sub check_switches{
  #From supported commands above create regular expressions matches
  $joined = join("|", @SUPPORTED_SIMULATORS );
  $simulator_pattern = qr/$joined/i;
  $joined = join("|", @CLEAN_COMMANDS);
  $clean_pattern = qr/$joined/i;
  
  #
  # --test checking
  #
  # Croak if no --test option
  if (!defined($options{"test"})){croak "Error! --test <test_name> is a mandatory switch";}
  # Croak if --test but no argument
  if (defined($options{"test"}) and $options{"test"} eq "") {croak "Error! --test requires a test name to be provided";}
  
  
  #
  # --simulator checking
  #
  if (!defined($options{"simulator"})){
    if (defined ($ENV{SIM})) {$options{"simulator"}=$ENV{"SIM"};}
    else {croak "Error! \$SIM not specified and --simulator SIM not provided. Not sure what HDL simulator to use."}
  }; #Use simulator set by environment
  $options{"simulator"} = uc $options{simulator};
  $ENV{SIM} = lc $options{"simulator"};
  if ($options{"simulator"} !~ $simulator_pattern){
    croak "Error! --simulator don't recognize option \"$options{\"simulator\"}\".",
      " Please use one of @SUPPORTED_SIMULATORS";
  }
  
  
  #
  # --clean checking
  #
  #--clean is special... if not defined then no clean... if defined but empty then clean USER else check string (below)
  if (defined ($options{"clean"}) and ($options{"clean"}[0] eq "")) { $options{"clean"}[0] = "USER";};
  if (!defined($options{"clean"})){$options{"clean"}[0]       = "NONE";} #Clean nothing by deafult

  foreach $cmd ( @{$options{"clean"}} ) {
    if ($cmd eq "") { $cmd = "USER";}
    if ($cmd !~ /^$clean_pattern$/ ){
      croak "Error!--clean don't recognize option $cmd",
	" Please use one of @CLEAN_COMMANDS";
    }
  }

#    if ($options{"clean"} !~ $clean_pattern){
#      croak "Error!--clean don't recognize option \"$options{\"clean\"}\".",
#	" Please use one of @SUPPORTED_SIMULATORS";
#    }
  
  
  #
  # --check --cpp_compile, --hdl_compile &--run
  #
  
  if (!defined($options{"cpp_compile"})){$options{"cpp_compile"} = 1;} # Compile the C++ side
  if (!defined($options{"hdl_compile"})){$options{"hdl_compile"} = 1;} # Compile the hdl side
  if (!defined($options{"run"}))        {$options{"run"} = 1;}         # Run simulation
  if (!defined($options{"verbose"}))    {$options{"verbose"} = 1;}     # Be chatty by default
  if (defined($options{"debug"}))       {$debug = 1;}                  # This will print more messages
  
  # Croak if no --seed option
  if (!defined($options{"seed"})){$seed = 1;}
  if (defined($options{"seed"}) and $options{"seed"} eq "") {croak "Error! --seed requires an integer seed value to be provided";}
  if (defined($options{"seed"}) and $options{"seed"} ne "") {$seed = $options{"seed"};};

  # runs
  if (!defined($options{"runs"})){$runs = 1;}
  if (defined($options{"runs"}) and $options{"runs"} eq "") {croak "Error! --runs requires an integer value >0 to be provided";}
  if (defined($options{"runs"}) and $options{"runs"} <= 0)  {croak "Error! --runs requires an integer value >0 to be provided";}
  if (defined($options{"runs"}) and $options{"runs"} ne "") {$runs = $options{"runs"};};


  if (!defined($options{"config"})) {$config = top;}
  elsif (defined($options{"config"}) and ($options{"config"}eq "")) {croak "Error! --config requires a valid configuration to be provided";}
  else {$config = $options{"config"}}

}


sub check_env{
  my($env_err); #ERROR COUNT

  #
  # CHECK TRUSS, TEAL AND PROJECT SET-UP
  #
  print "Checking truss environment:";

  #Check that each env. variable needed is defined and that directory behind it exsist.
  @env_array = ("TRUSS_HOME", "TEAL_HOME", "PROJECT_HOME", "SIMULATOR_HOME");
  foreach (@env_array){
    if (!defined ($ENV{$_})){
      print STDERR "\nERROR! \$$_ not defined but needed by environment. Please refer to installation instruction\n";
      ++$env_err;
    }
    elsif (!(-e $ENV{$_})){
      print STDERR "\nERROR! \$$_ defined but directory [$ENV{$_}] doesn't exsist. Please verify variable and try again\n";
      ++$env_err;
    }
  }

  # Used to pcik correct make file by Teal
  if (!defined ($ENV{ARCH})){
    if (qx/uname -s/ =~ /^Solaris|Linux$/) {
      $ENV{ARCH} = `uname -s`;
      chomp $ENV{ARCH};
    }
    else {
      print STDERR "\nERROR! \$ARCH not defined but needed to build Teal. Please define \$ARCH as either \"Solaris\" or \"Linux\"\n";
      ++$env_err;
    }
  }
  if ($ENV{ARCH} !~ /^Solaris|Linux$/){
    print STDERR "\nERROR! \Please redefine \$ARCH. It is defined as \"$ENV{ARCH}\" but only \"Solaris\" or \"Linux\" are valid values";
    ++$env_err;
  }

  
  if (!defined ($env_err)) { print " Done.\n";} else { print "\n";}


  #
  # CHECKS FOR HDL SIMULATORS GO HERE ENVIRONMENT
  #

  print "Checking HDL simulator environment($options{\"simulator\"}):";
  # Check that we have a simulator defined at all!
  if($options{"simulator"} eq "VCS"){ 
  }
  elsif($options{"simulator"} eq "NCSIM"){
  }
  elsif($options{"simulator"} eq "MTI"){
  }
  elsif($options{"simulator"} eq "MTI_2_0"){
  }
  elsif($options{"simulator"} eq "ALDEC"){
  }
  elsif($options{"simulator"} eq "IVL"){
  }
  else{
    print "\nUnsupported simulator ", $options{"simulator"}, " detected. Simulators supported are $SUPPORTED_SIMULATORS";
    ++$env_err;
  }

  if (!defined ($env_err)) { print " Done.\n";} else { print "\n";}

  #
  # EXIT IF ANY ERRORS ABOVE WAS SEEN
  #
  if ($env_err) {croak "$env_err problem", ($env_err >1) ? "s " : " ", "seen. Cannot continue.";}
}

RE;
