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
#include <iostream>
#include <sstream>

typedef enum {teal_param_source_env,
              teal_param_source_plusarg,
              teal_param_source_file,
              teal_param_source_put,
              teal_param_source_default
              } teal_param_source;

std::ostream& operator<< (std::ostream& output, const teal_param_source& p)
{
  switch(p) {
  case teal_param_source_env:
    output << "teal_param_source_env";
    break;
  case teal_param_source_plusarg:
    output << "teal_param_source_plusarg";
    break;
  case teal_param_source_file:
    output << "teal_param_source_file";
    break;
  case teal_param_source_put:
    output << "teal_param_source_put";
    break;
  case teal_param_source_default:
    output << "teal_param_source_default";
    break;
  }
  return output;
}

// Put all the info about a parameter in the same place.
class teal_param_info {
public:
  teal_param_info() {found = false; source = teal_param_source_file;}
  teal_param_source source;
  std::string       file_name;    // If from a file which one
  bool              found;        // Has there been a find on this one
  std::string       val;
  std::string       plusarg_val;
  std::string       env_val;
};


std::ostream& operator<< (std::ostream& output, const teal_param_info& p)
{
  output << p.source << ", sourcefile<" << p.file_name << ">,";

  if ( ! p.env_val.empty()) 
    output <<     "env_val<"   << p.env_val     << ">, ";

  if ( ! p.plusarg_val.empty()) 
    output << "plusarg_val<"   << p.plusarg_val << ">, ";

  if ( ! p.val.empty()) 
    output <<         "val<"   << p.val         << ">";

  return output;
}

// This typedef is also used to declare iterators.
typedef std::map<std::string, teal_param_info> paramsT;
static paramsT lines_;

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
static bool  chatty_ = false;


///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
 bool teal::dictionary::chatty () {  return chatty_;}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
/* previous*/ bool teal::dictionary::chatty (bool new_value)
{
  bool returned (chatty_);
  chatty_ = new_value;
  return returned;
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
// Strip leading and trailing whitespace
std::string strip_whitespace(std::string val)
{
  size_t find;

  // Skip leading whitespace
  find = val.find_first_not_of(' ');
  if (find && (find != std::string::npos)) {
    val.erase(0, find);
  }

  // Strip trailing whitespace
  find = val.find_last_not_of(' ');
  if (find && ((find+1) != val.length()) && (find != std::string::npos))
    val.erase(find+1, val.length());

  return val;
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////

bool is_range(std::string parm)
{
  if (parm.find(".r", parm.length()-2) == std::string::npos)
    return false;
  else
    return true;
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
// Low level, simple parameter.
static bool _set_param(std::string name, std::string value, teal_param_source source, std::string file)
{
  if (0)
    std::cout << "_set_param(" << name << "=" << value << ")" << std::endl;

  // If this is the first time we have seen this parameter allocate space for it.
  if (lines_.find(name) == lines_.end()) {
    teal_param_info info;

    lines_[name] = info;
  }

  // Strip leading and trailing whitespace.
  value = strip_whitespace(value);

  switch (source) {

  case teal_param_source_default:
    // Comes from the user. Believe it.
    lines_[name].val =  value;
    lines_[name].file_name = "from default";
    lines_[name].source = source;
    break;

  case teal_param_source_env:
    // env always wins. Highhest priority.
    lines_[name].env_val =  value;
    lines_[name].file_name = "from env";
    lines_[name].source = source;
    break;

  case teal_param_source_plusarg:
    lines_[name].plusarg_val =  value;

    if (lines_[name].source != teal_param_source_env) {
      lines_[name].file_name = "from plus_arg";
      lines_[name].source = source;
    }
    break;
  
  case teal_param_source_put:
    lines_[name].val =  value;
    if ((lines_[name].source != teal_param_source_env) &&
        (lines_[name].source != teal_param_source_plusarg)) {
      lines_[name].file_name = "from put";
      lines_[name].source = source;
    }
    break;
  
  case teal_param_source_file: 
    lines_[name].val = value;

    // Lowest priority, overrides nothing.
    // source_file is the default from the constructor.
    // See if it is still there.
    if (lines_[name].source == teal_param_source_file)
      lines_[name].file_name = file;
      
   break;
  }

  if (0)
    std::cout << "_set_param: " << source << " lines_[" << name << "] = " << lines_[name] << std::endl;

  return true;
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
// Note optional filename arguement
static bool set_param(std::string name, std::string value, teal_param_source source, std::string file = "")
{
  // Handle range range parameters
  if (is_range(name)) {
    std::string min;
    std::string max;

    name = name.erase(name.length()-2, name.length());
    value = strip_whitespace(value);

    // Split value into min and max. Delimited by commas or spaces.
    size_t find = value.find(',');
    if (find == std::string::npos) {
      // No commas. Look for spaces.
      size_t find = value.find(' ');
      if (find == std::string::npos) {
        // One value means they are the same.
        min = max = value;
      } else {
        // Looks like they are space delimited
        min = std::string(value, 0, find);
        max = std::string(value, find+1, std::string::npos);
      }
    }  else {
      // multiple values are comma delimited
      min = std::string(value, 0, find);
      max = std::string(value, find+1, std::string::npos);
    }
    
    min = strip_whitespace(min);
    max = strip_whitespace(max);

    bool return_val = _set_param(name + "_max", max, source, file) &&
                      _set_param(name + "_min", min, source, file);
    return return_val;

  } else {

    value = strip_whitespace(value);
    return _set_param(name, value, source, file);
  }

}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
std::string teal::dictionary::expand_variables (const std::string& path)
{
  bool parsingEnvVar = false;
  std::string envVar, resolvedPath;
  for (unsigned i=0;i<path.length();i++) {
    if ( ! parsingEnvVar && path[i] == '$') {
      parsingEnvVar = true;
    } else if (parsingEnvVar) {
      if (path[i] == '/') {
        assert(getenv(envVar.c_str()));
        resolvedPath.append(getenv(envVar.c_str()));
        resolvedPath.append(1, path[i]);
        envVar.clear();
        parsingEnvVar = false;
      } else {
        envVar += path[i];
      }
    } else {
      resolvedPath += path[i];
    }
  }

  return resolvedPath;
}


///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////

static void process_file (const std::string& name, bool fatal_if_unfound) {
  if (chatty_)  std::cout << "processing file \"" << name << "\"" << std::endl;
  std::ifstream  file (teal::dictionary::expand_variables (name).c_str());

  if ( (!file) && fatal_if_unfound) {
    vout foo ("dictionary::process_file()");
    foo << teal_fatal << " Unable to read config file: <\"" << name << "\"" << ">" << teal::endm;
  }

  uint32 line_number (1);
  while ((file) && ( ! file.eof())) {
    std::string symbol;
    file >> symbol;

    if ( (!file.eof()) && (!file.good())) {
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

    if(0) {
      // Strip comments
      size_t find;
      find = value.find('#');
      if (find != std::string::npos)
        value.erase(find, std::string::npos);
      find = value.find("//");
      if (find != std::string::npos)
        value.erase(find, std::string::npos);
    }

    if (chatty_) std::cout << "in file " << name << " for word \"" << symbol << "\" value is \"" << value << "\"" << std::endl;
    if (symbol == "#include") {
      if (0) {
        std::cout<< "#include " << name << " including " << value << std::endl;
      }

      process_file ( strip_whitespace(value), true);
    }
    else {

      if(1) {
        // Strip comments and trailing and leading whitespace
        size_t find;
        find = symbol.find('#');
        if (find != std::string::npos)
          symbol.erase(find, std::string::npos);
        find = symbol.find("//");
        if (find != std::string::npos)
          symbol.erase(find, std::string::npos);
        symbol = strip_whitespace(symbol);
      }

      if ( ! (symbol.empty() || value.empty()) ) {
        set_param(symbol, value, teal_param_source_file, name);
      }
    }
    ++line_number;
  }
}


///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
void teal::dictionary::read (const std::string & name, bool fatal_if_unfound)
{
  process_file (name, fatal_if_unfound);
  teal::dictionary::print_map("rerun.cfg", false, true, true);
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
void teal::dictionary::clear ()
{
  lines_.clear ();
}

#include "systemc.h"

// States for plusarg parse state machine
typedef enum {
  START,
  NAME,
  VALUE,
  DONE
} teal_parse_state;

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
static void teal_scan_plusargs (void)
{
  static bool loaded = false; // Only parse the command line once

  if ( ! loaded) {
    for (int i = 1; i < sc_core::sc_argc(); ++i) {
      std::string arg = sc_core::sc_argv() [i];
      std::string name;
      std::string value;

      if (0) {
        std::cout << "arg[" << i << "] = " << arg << std::endl;
      }

      // Each SystemC arguement can have multiple plusarg elements
      teal_parse_state state = START;
      while(state != DONE) {
        switch (state) {
        // Looking for the start of a plusarg value
        case START: {
          if (arg.empty()) {
            state = DONE;
          } else {
            name.clear();
            value.clear();
          
            // Skip leading whitespace
            size_t find = arg.find_first_not_of(' ');
            if (find != std::string::npos) {
              arg.erase(0, find);
            }
          
            // Look for the plus
            if (arg[0] == '+') {
              arg.erase(0, 1);
              state = NAME;
              
            } else {
              state = DONE;
            }
          }
          break;
        }
        case NAME: {
          // names are terminated by the first '=' or '+'
          if (arg.empty()) {
            // Nothing after the plus. Should be at least a warning
            state = DONE;

          } else {
            // Take whichever comes first
            size_t find = arg.find_first_of("+=");

            if (find != std::string::npos) {
              name = std::string(arg, 0, find);
              arg = arg.erase(0, find+1);
            } else {
              // Should this be an error? Handle in the VALUE state
              name = arg;
              arg = "";
            }
            state = VALUE;
          }


          break;
        }
        case VALUE: {
          // Value is everything up to the end or the next '+'
          if (arg.empty()) {
            // Nothing after the plus. Should be at least a warning
            state = DONE;

          } else {
            size_t find = arg.find('+');

            if (find != std::string::npos) {
              value = std::string(arg, 0, find);

              // Strip trailin whitespace
              size_t last = value.find_last_not_of(' ');
              if (last != std::string::npos)
                value.erase(last+1, value.length());

              arg = arg.erase(0, find);

            } else {
              
              value = arg;
              arg.clear();
            }

            set_param(name, value, teal_param_source_plusarg);

            // Look for another
            state = START;
          }
          break;
        }
        case DONE: {
          break; // Can't happen but quiet the compiler warning
        }
        }
      }
    }
    loaded = true;
  }
  return;
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
bool teal::dictionary::put (const std::string& name, const std::string& value, bool replace_existing)
{
  bool found = !find (name).empty();

  if (( ! found) || (replace_existing)) {
    set_param(name, value, teal_param_source_put);
  }

  return found;
}


///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
std::string teal::dictionary::find (const std::string& name)
{
  std::string return_val;

  teal_scan_plusargs(); // Load plusargs into the dictionary.


  // First check for unlogged environment valiable
  if (getenv(name.c_str())) {
      set_param(name, getenv(name.c_str()), teal_param_source_env);
  }

  if (lines_.find(name) == lines_.end()) {
    return_val = "";
  } else {

    teal_param_info& info = lines_[name];

    switch (info.source) {
      // Check in priority order, env first, cli second, file or put last.
    case teal_param_source_env:
      return_val = info.env_val; break;
    case teal_param_source_plusarg: 
      return_val = info.plusarg_val; break;
    default: 
      return_val = info.val; break;
    }
    info.found = true;
   }

  return return_val;
}


///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
std::string teal::dictionary::find_on_command_line (const std::string& name, const std::string& default_name)
{
  std::string return_val;

  teal_scan_plusargs(); // Load plusargs into the dictionary.

  if (lines_.find(name) == lines_.end()) {

    if (0)
      _set_param(name, default_name, teal_param_source_default, "from default");

    return_val =  default_name;

  } else {
    teal_param_info& info = lines_[name];
    info.found = true;

    switch (info.source) {
    case teal_param_source_plusarg: 
      return_val = info.plusarg_val;break;
    default: 
      return_val = default_name; break;
    }
  }
  
  if (0) {
    std::cout << "find_on_command_line returns [" << name << "] = " << return_val << std::endl;
  }
  return return_val;
}


///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
void teal::dictionary::note_from_default (const std::string& name) {
  teal_param_info& info = (lines_[name]);
  std::string old;

  switch(info.source) {
  case teal_param_source_env:
    old = info.env_val;
    break;
  case teal_param_source_plusarg:
    old = info.plusarg_val;
    break;
  case teal_param_source_file:
    old = info.val;
    break;
  case teal_param_source_put:
    old = info.val;
    break;
  case teal_param_source_default:
    old = info.val;
    break;
  }

  _set_param(name, old, teal_param_source_default, "from default");

}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
void teal::dictionary::print_map (const std::string filename, bool include_filename, bool show_nulls, bool show_defaults)
{
  std::ofstream  file (teal::dictionary::expand_variables (filename).c_str());
  assert (file.good());
  
  // walk through the dictionary
  for (paramsT::iterator the_line = lines_.begin();
       the_line != lines_.end();
       ++the_line) {
    std::string  name = the_line->first;
    teal_param_info info = the_line->second;
    
    bool show_me = (!info.file_name.empty() || (show_nulls));
    show_me &= ((info.file_name != "from_default") || (show_defaults));
    
    if (! show_me) continue;

    if (include_filename) {  file << "[" << info.file_name << "] ";  }
    
    // Comment out the dictionary entry
    if (name == "dictionary")
      file << "# ";

    file << name << " ";

    switch (info.source) {
    case teal_param_source_env:
      file << info.env_val;
      file << 
      file << " # From ENV variable";
      break;
    case teal_param_source_plusarg:
      file << info.plusarg_val;
      file << " # From plusarg";
      break;
    case teal_param_source_file:
      file << info.val;
      //file << " # From " << info.file_name;
      break;
    case teal_param_source_put:
      file << info.val;
      file << " # From put()";
      break;
    case teal_param_source_default:
      file << info.val;
      file << " # From default arg of find()";
      break;
    }

    file << std::endl;
  }
}


///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
std::string teal::dictionary::unused_plusargs (void)
{
  std::ostringstream os;

  for (paramsT::iterator the_line = lines_.begin();
       the_line != lines_.end();
       ++the_line)
    {
      std::string  name = the_line->first;
      teal_param_info info = the_line->second;
      
      if ((info.source == teal_param_source_plusarg) && (!info.found)) {
        os << name << "=" << info.plusarg_val << std::endl;
      }
    }

  return os.str();
}

// Parse a value string into a map.
bool teal::dictionary::set_weight_map(std::string in, teal::dictionary::enumWeights & out)
{
  vout foo(__FUNCTION__);

  size_t i = 0;
  size_t j = 0;
  double total = 0;

  if (in.size() == 0) {
    foo << teal_fatal << ":" << __FUNCTION__ << " called with null init string" << teal::endm;
    return false;
  }
      
  // Split string on comma boundaries
  do {
      std::string nxt;

      j = in.find(',', j);
      if (j == std::string::npos) {
        nxt = in.substr(i, in.length( ));
      } else {
        nxt = in.substr(i, j-i);
        i = ++j;
      }
          
      // If no colon assume a specified weight of 1.
      size_t colon_where = nxt.find(':');
      if (colon_where == std::string::npos) {
        nxt = nxt + ":1";
        colon_where = nxt.find(':');          
      }

      // Two colon delimited fields
      std::string element = nxt.substr(0,colon_where);
      std::string weight_str = nxt.substr(colon_where+1);
      std::istringstream ws(weight_str);

      double weight;

      if (!(ws >> weight)) {
        foo << teal_fatal << "parsing parameter value: <" << in << "> " << nxt << " needs to have a number after the colon" << teal::endm;
        return false;
      }

      teal::dictionary::enumWeights::iterator it = out.find(element);
      if (it == out.end()) {
        foo << teal_fatal << "<" << element << "> not found in enum" << teal::endm;
        return false;
      }
      
      teal::dictionary::elementWeight& w (it->second);
      w.in_weight = weight_str;
      w.weight = weight;

      total += weight;
    }
  while (j != std::string::npos);
  
  // Normalize to sum of one.
  double delta = 0;
  teal::dictionary::enumWeights::iterator it;
  for (it = out.begin(); it != out.end(); ++it) {
    elementWeight& ew(it->second);
    ew.weight /= total;
    delta += ew.weight;
    ew.delta = delta;
  }
    
  return true;
}

//////////////////////////////////////////////////////////////////////////
//  This is a little more complex than it might be because it tries to  //
//  keep the distribution as close to the requested weights as          //
//  possible. It does that by only allocating from the set of values    //
//  that are lower than or equal to their assigned probablility.        //
//////////////////////////////////////////////////////////////////////////

std::string teal::dictionary::rand_from_weight_map(teal::dictionary::enumWeights& in, double rand) {
  teal::dictionary::enumWeights::iterator it;

  // Calculate the weight of the already allocated distribution.
  int total = 0;
  teal::dictionary::enumWeights actuals = in;
  for (it = in.begin(); it != in.end(); ++it) {
    total += it->second.count;
  }

  teal::dictionary::enumWeights out;
  if (total == 0) {
    out = in;

  } else {
    for (it = actuals.begin(); it != actuals.end(); ++it) {
      elementWeight& ew(it->second);
      ew.weight = ew.count / total;
    }

    // Remove (locally) from the set all that are above their probability
    double d_total = 0;
    for (it = in.begin(); it != in.end(); ++it) {
      std::string key = it->first;
      if (actuals[key].weight <= in[key].weight) {
        out[key].weight = in[key].weight;
        d_total += in[key].weight;
      }
    }

    // Normalize the new list
    double delta = 0;
    for (it = out.begin(); it != out.end(); ++it) {
      elementWeight& ew(it->second);
      ew.weight /= d_total;
      delta += ew.weight;
      ew.delta = delta;
    }
  }

  if (0) {
    std::cout << " Allocating from set<";
    for (it = out.begin(); it != out.end(); ++it) {
      elementWeight& ew(it->second);
      std::string key = it->first;
      std::cout << key << "=";
      std::cout << ew.delta << ",";
    }
    std::cout << std::endl;
  }

  // Allocate randomly from the new list
  std::string key("bad");
  for (it = out.begin(); it != out.end(); ++it) {
    elementWeight& ew(it->second);
    key = it->first;
    if (ew.weight && ew.delta >= rand) {
      break;
    }   
  }

  if (key == "bad") {
    teal::vout log_(__FUNCTION__);
    log_ << teal_fatal << " Software error. This should be impossible." << teal::endm;
  }

  in[key].count++;
  return key;
}

void teal::dictionary::test_weight_example_test()
{
  std::cout << __FUNCTION__ << ":" << std::endl;

  std::vector<std::string> tests;

  tests.push_back("v1:24,v3:75,v4:1");
  tests.push_back("v1:.24,v3:.75,v4:.01");
  tests.push_back("v1,v3");
  tests.push_back("v3:7");
  tests.push_back("v3");

  for (int i = 0; i < (int) tests.size(); ++i) {
    std::string& in(tests[i]);
    teal::dictionary::enumWeights::iterator w_it;

    // Init the  map
    teal::dictionary::enumWeights test_map;
    teal::dictionary::elementWeight e(0);
    test_map["v1"] = e;
    test_map["v2"] = e;
    test_map["v3"] = e;
    test_map["v4"] = e;
    teal::dictionary::set_weight_map(in, test_map);

    // Collect stats data
    std::map<std::string, int> resultCount;
    const int num = 1000;
    for (int i = 0; i < num; ++i) {
      teal::uint64 tmp;
      RAND_RANGE(tmp, 0, 9999999);
      double rand = tmp / 10000000.0;  // rand value from zero to one.
      std::string s = teal::dictionary::rand_from_weight_map(test_map, rand);
      resultCount[s]++;
    }

    // Calculate the sum of weights from the original value string
    double d_total = 0;
    for (w_it = test_map.begin(); w_it != test_map.end(); ++w_it) {
      const elementWeight& ew(w_it->second);
      const std::string& name(w_it->first);

      if (0) {
        std::cout << "  " <<  name << std::endl;
        std::cout << "     in_weight: " << ew.in_weight << std::endl;
        std::cout << "        weight: " << ew.weight << std::endl;
        std::cout << "         delta: " << ew.delta << std::endl;
        std::cout << "         count: " << ew.count << std::endl;
      }

      if (ew.in_weight.size()) {
        std::string w_str(ew.in_weight);
        std::istringstream ws(w_str);
        double weight;
        ws >> weight;
        d_total += weight;
      }
    }

    // std::cout << "d_total: " << d_total << std::endl;

    std::cout << "  Specified: <" << in << ">" << std::endl;
    std::cout << "     Actual: <";
    std::map<std::string, int>::iterator it;
    bool first = true;
    for (it = resultCount.begin(); it != resultCount.end(); ++it) {
      if (first)
        first = false;
      else
        std::cout << ",";
      std::string name(it->first);
      std::cout << name << ":";
      std::cout << (d_total * it->second) / num;
    }
    std::cout << ">" << std::endl;

  }
  
  std::cout.flush();
}
