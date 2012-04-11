
sub usage{
#  print "-" x 80 . "\n";
  print "\ntruss --test <test name>\n";
  print "\nOptional:\n";
  print "      [--help]\n";
  print "      [--clean [<@CLEAN_COMMANDS>]\n";
  print "      [--config <configuration name>]\n";
  print "      [--simulator <@SUPPORTED_SIMULATORS>]\n";
  print "      [--nocpp_compile]\n";
  print "      [--nohdl_compile]\n";
  print "      [--norun]\n";
  print "      [--noverbose]\n";
  print "      [--debug]\n";
  print "      [--seed <value>]\n";
  print "      [--timeout <value>]\n";
  print "      [--runs <value>]\n";
  print "      [--args_cpp_compile <\'string\'>]\n";
  print "      [--args_hdl_compile <\'string\'>]\n";
  print "      [--args_run <\'string\'>]\n";
  print "\n";
#  print "-" x 80 . "\n";

#  print "\nSUGGESTED, BUT NOT IMPLEMENTED:\n";
#  print " [--regression]";
#  print " [--lsf]";
#  print " [--include <file name>]";
#  print " [--batch]";
#  print "\n";
  
}

sub detailed_usage{
  print "Switch Name    Default Description\n";
  print "-" x 80 . "\n";
  print "--help                 Print this message\n";
  print "\n";
  print "--test                 <Test name> to run\n";
  print "\n";
  print "--clean        USER    Clean results before running. Options include:\n";
  print "                       LOGS - Cleans simulation result files (log & waves)\n";
  print "                       C++  - Cleans all user's C++ files\n";
  print "                       HDL - Cleans all user's compiled files\n";
  print "                       USER - Same as --clean LOGS C++ HDL\n";
  print "                       TRUSS- Cleans all truss related files \n";
  print "                       TEAL - Cleans all teal related files\n";
  print "                       ALL - Same as --clean LOGS C++ HDL USER TRUSS TEAL\n";
  print "\n";
  print "--config      top      <configuration name> to tun\n";
  print "\n";
  print "--simulator            truss looks at \$SIM to determine HDL simulator if no switch is\n";
  print "                       provided. This switch selects another supported simulator:\n";
  print "                       <@SUPPORTED_SIMULATORS>\n";
  print "\n"; 
  print "--nocpp_compile        Will bypass C++ compile step.\n";
  print "--nohdl_compile        Will bypass HDL compile step.\n";
  print "--norun                Will not run simulation (i.e compile only)\n";
  print "\n";
  print "--seed                 Integer \'value\' will be used instead of random seed\n";
  print "\n";
  print "--timeout              Integer \'value\' will be used instead of default timeout\n";
  print "\n";
  print "--runs                 Repeat \'value\' number of times, incrementing the seed\n";
  print "\n";
  print "--args_cpp_compile     String will be passed on to cpp make command.\n";
  print "--args_hdl_compile     String will be passed on to hdl compile\n";
  print "--args_run             String will be passed to simulation launch command\n";
  print "-" x 80 . "\n";
}

sub example{
  print "Example usage not written\n";
}

sub copyright{
  print "--------------------------------------------\n";
  print "Truss run script Copyright 2006\n";
  print "by Robert Ekendahl and Mike Mintz\n";
  print "Available as open-source at www.trusster.com\n";
  print "--------------------------------------------\n";

}


RE;
