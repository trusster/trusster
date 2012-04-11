/*
Trusster Open Source License version 1.0a (TRUST)
copyright (c) 2006 Mike Mintz and Robert Ekendahl.  All rights reserved. 

Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met: 
   
  * Redistributions of source code must retain the above copyright notice, 
    this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, 
    this list of conditions and the following disclaimer in the documentation 
    and/or other materials provided with the distribution.
  * Redistributions in any form must be accompanied by information on how to obtain 
    complete source code for this software and any accompanying software that uses this software.
    The source code must either be included in the distribution or be available in a timely fashion for no more than 
    the cost of distribution plus a nominal fee, and must be freely redistributable under reasonable and no more 
    restrictive conditions. For an executable file, complete source code means the source code for all modules it 
    contains. It does not include source code for modules or files that typically accompany the major components 
    of the operating system on which the executable file runs.
 

THIS SOFTWARE IS PROVIDED BY MIKE MINTZ AND ROBERT EKENDAHL ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, 
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, 
OR NON-INFRINGEMENT, ARE DISCLAIMED. IN NO EVENT SHALL MIKE MINTZ AND ROBERT EKENDAHL OR ITS CONTRIBUTORS 
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "teal.h"
using namespace teal;
#include <fstream>

static std::map<std::string, std::string> lines_;

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
static void process_file (const std::string& name) {
  //std::cout << "processing file " << name << std::endl;
  std::ifstream  file (name.c_str());
  uint32 line_number (1);
  while ((file.good()) && (!file.eof())) {
    std::string symbol;
    file >> symbol;

    if ((!file.eof()) && (!file.good())) {
      vout foo ("dictionary::process_file()a");
      foo << teal_error << " In file: " << file.fail() << " " << file.bad() << " " << file.eof () << name <<  " Parse error on line " << line_number << " line skipped" << endm;
      file.clear ();
      continue;
    }

    std::string value;
    std::getline (file, value);

    if ((!file.eof()) && (!file.good())) {
      vout foo ("dictionary::process_file()b");
      foo << teal_error << " In file: " << name << " Parse error on line " << line_number << " line skipped" << endm;
      continue;
    }

    //std::cout << "in file " << name << " for word \"" << symbol << "\" value is \"" << value << "\"" << std::endl;
    if (symbol == "#include") {
      process_file ( std::string (value, 1, std::string::npos));
    }
    else {
      if ((symbol != "") && (value != "")) lines_[symbol] = std::string (value, 1, std::string::npos);
    }
    ++line_number;
  }
}


///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
void teal::dictionary::read (const std::string & name)
{
  process_file (name);
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
void teal::dictionary::clear ()
{
  lines_.clear ();
}


////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
#if defined vpi_2_0
std::string teal_scan_plusargs (const std::string& name)
{
  std::string full_name ("+" + name);
  s_vpi_vlog_info info;
  int result = vpi_get_vlog_info (&info);
  result++;
  //  std::cout << "teal_scan_plus_args count is: " << info.argc << std::endl;
  //  ASSERT (result);
  for (int i(0); i < info.argc; ++i) {
    std::string argv (info.argv[i]);
    //std::cout << "teal_scan_plus_args arg[ " << i << "] is: " << argv << std::endl;
    if (argv.find (full_name) != std::string::npos) {
      std::string after_part (argv, argv.find (full_name) + full_name.size () + 1, std::string::npos);
    return after_part;
    }
  }
  return std::string ("");
}
#else
std::string teal_scan_plusargs (const std::string& name)
{
  //  std::cout << " teal_scan_plusargs() for " << name << std::endl;
  char* text = mc_scan_plusargs ((PLI_BYTE8*)name.c_str ());
    if (text) ++text;  //skip the leading +
    //            std::cout << "teal_scan_plus_args (mc version) for \"" << name << "\" returns " 
    //        	    << (void*)text << std::endl;
  return (text ? text: ""); //could do a scan with acc_fetch_argv, acc_fetch_argc
}
#endif


///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
bool teal::dictionary::put (const std::string& name, const std::string& value, bool replace_existing)
{
  bool returned = (find (name) != "");
  if ( (! returned) || (replace_existing)) {
    lines_[name] = value;
  }
  return returned;
}


///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
std::string teal::dictionary::find (const std::string& name)
{
  std::string arg = teal_scan_plusargs (name); 
  // std::cout << " for word " << name << " lins is " << lines_[name] << std::endl;
  return (arg != "") ? arg : lines_[name]; //NOTE: will pollute lines_ could have used find(name).
}


///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
std::string teal::dictionary::find_on_command_line (const std::string& name, const std::string& default_name)
{
  std::string arg = teal_scan_plusargs (name);
  return (arg != "") ? arg : default_name;
}

