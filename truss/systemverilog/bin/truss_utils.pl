
sub truss_system{
  my ($command) = $_;

  if ($debug) print $command . "\n";
  system ($command) == 1 or croak "System call failed";
}
