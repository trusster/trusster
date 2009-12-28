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

#include "systemc.h"
#define teal_printf_io 

#if defined (teal_printf_io)
#  define print printf
#else
#  if defined (vpi_2_0)
#    define print vpi_printf
#  else
#    define print io_printf
#  endif
#endif

#include <stdio.h> //HACK for FILE*
#include "time.h"
#include <algorithm>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
teal::vlog* teal::vlog::the_(0);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//put in unnamed namespace
class local_vlog : public teal::vlog {
public:
  local_vlog () : vlog (),   fatal_message_seen_ (false),  error_message_seen_ (false)

  {
    //    std::cout << " local_vlog::local_vlog() " << std::endl;
    time_t dummy;
    ::time (&dummy);
    char m[256];
    sprintf (m, "Start Time: %s\n", ctime (&dummy));
    //    std::cout << " local_vlog::local_vlog() before local print" << std::endl;
    cout << m << std::endl;
  }
  
protected:
  void output_message (const teal::message_list& m) {
    teal::vlog::output_message (m);  //do standard accounting
    //    teal::message_list m  = teal::vlog::output_message_ (foo);
    std::string completed_message;
    for (teal::message_list_iterator it5 (m.begin()) ; it5 != m.end(); ++it5) {
      completed_message += (*it5).second;
      if ((*it5).first == fatal) {
	fatal_message_seen_ = true;
      }
      if ((*it5).first == error) {
	error_message_seen_ = true;
      }
    }
    //    cout << "Start print of:" << completed_message << std::endl;
    teal::vlog::get().local_print (completed_message);
    //    cout << "Done print of:" << completed_message << std::endl;
    //    return m;
  }

  std::string local_print_ (const std::string& val) {
    cout << val << std::endl;  //silly (mentor) systemc wants the endl to force a flush
    if (val.find ("FATAL_ERROR") != std::string::npos) {
    //    if ((fatal_message_seen_) && (val.find ("FATAL") != std::string::npos)) {
    //    if (how_many(vlog::fatal)) {
      //if (fatal_message_seen_) {
      fatal_message_seen_ = false;  //need to clear now since teal::finish prints someting out!
      //      SC_REPORT_FATAL (41062, "TEST FAILED");
      //print failed line so greps will work
      char msg[256];
      sprintf (msg, " TEST FAILED : Contained a FATAL error and %d  errors.\n", teal::vlog::get().how_many (teal::vlog::error));
      cout << msg << std::endl; 
      teal::finish ();
    }
    fatal_message_seen_ = false;
    error_message_seen_ = false;
    return val;
  }
private:
    bool fatal_message_seen_;
    bool error_message_seen_;
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
teal::file_vlog::file_vlog (std::string file_name, bool a) : out_file_ (0), also_to_screen_ (a) {
  if ((file_name != "") && file_name[0]) out_file_ = fopen ((char*)file_name.c_str (), "w");
  if (out_file_) {
    std::ostringstream o; 
    o << "Output is " <<  ((also_to_screen_) ? "copied " : "sent ") <<  "to " << file_name << " .\n";
    teal::vlog::get().local_print (o.str());
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
teal::file_vlog::~file_vlog () {if (out_file_) {FILE* t = out_file_; out_file_ = 0; fclose (t);}}

 
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string teal::file_vlog::local_print_ (const std::string& val) {
  std::string val2 = val + "\n";
  //  if (out_file_) fprintf (out_file_, const_cast<char*> (val.c_str ()));
 if (out_file_) fprintf (out_file_, val2.c_str ());
  return (also_to_screen_) ? val : "";
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void teal::vlog::output_message (const message_list & msg) {
  message_list msg2 (output_message_ (msg)); //filter/transform it

  //do standard accounting
  for (teal::message_list_iterator it5 (msg2.begin()) ; it5 != msg2.end(); ++it5) {    ++id_count_[(*it5).first];  }

  if (after_me_) after_me_->output_message (msg2);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
teal::vlog::vlog () : after_me_ (the_)
{
  the_ = this;
  //  std::cout <<  "vlog::vlog() the_ = " << std::hex << (size_t)the_ << std::endl;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
teal::vlog::~vlog () 
{
  if (the_ == this) {
    assert (after_me_); //ow no printing
    the_ = after_me_;
  }
  else { //take me out of the chain
    vlog* ptr = the_;
    while (ptr && (ptr->after_me_ != this)) ++ptr;
    assert (ptr);
    if (ptr) {ptr->after_me_ = after_me_;} //clip me out
  }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
teal::vlog& teal::vlog::get () 
{
  //  std::cout <<  "vlog::get() the_ = " << std::hex << (size_t)the_ << std::endl;
  if (!the_) {new local_vlog ();}
  //  std::cout <<  "vlog::get() the_ = " << std::hex << (size_t)the_ << std::endl;
  return *the_;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned int teal::vlog::how_many (int id)
{
  return (after_me_) ? after_me_->how_many (id) : id_count_ [id];
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// And now for something completely different

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
teal::vout::vout (const std::string& fa) :
  show_debug_level_ (dictionary::find (fa + "_show_debug_level", (uint32)0)),
  current_line_debug_level_ (0),
  debug_level_ (0), //everything but debug messages
  vout_base_ (hex),
  begin_message_flag_ (true),
  functional_area_ (fa)
{
  //  std::cout << "vout created " << fa << std::endl;
  for (int i (teal::vlog::first_id); (i < teal::vlog::last_id); ++i) {
    message_display (i, true);
  }
  feature(vlog::functional_area, functional_area_); //TBD remove functional_area_ and use feature()
  if (knob_file_) {
    (*knob_file_) << fa << "_show_debug_level " << show_debug_level_ << std::endl;
    knob_file_->flush ();
  }

  if (!dictionary::find ("global_debug_level").empty()) {
    set_global_debug_level (dictionary::find ("global_debug_level", 0), dictionary::find ("global_debug_level_time", 0));
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//WARNING: NO DICTIONARY CALLS IN THIS ONE AS THE DICTIONARY HAS NOT BEEN INITED YET!!!!
teal::vout::vout (const std::string& fa, uint32 initial_show_level) :
  show_debug_level_ (initial_show_level),
  current_line_debug_level_ (0),
  debug_level_ (0), //everything but debug messages
  vout_base_ (hex),
  begin_message_flag_ (true),
  functional_area_ (fa)
{
  //  std::cout << "vout created " << fa << std::endl;
  for (int i (teal::vlog::first_id); (i < teal::vlog::last_id); ++i) {
    message_display (i, true);
  }
  if (knob_file_) {
    (*knob_file_) << fa << "_show_debug_level " << show_debug_level_ << std::endl;
    knob_file_->flush ();
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void teal::vout::base (vout_base a_value) {vout_base_ = a_value;}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
teal::vout::vout_base teal::vout::base () { return vout_base_;}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
teal::vout& teal::vout::operator<< (char val)
{
  if (might_print (current_line_debug_level_)) {
    start_a_message_check_ ();
    std::ostringstream o;
    (vout_base_ == dec) ? (o << std::dec << val) :
      (o << "0x" << std::hex << val);
    message_data_ += o.str ();
  }
  return *this;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
teal::vout& teal::vout::operator<< (unsigned char val)
{
  if (might_print (current_line_debug_level_)) {
    start_a_message_check_ ();
    std::ostringstream o;
    (vout_base_ == dec) ? (o << std::dec << val) :
      (o << "0x" << std::hex << val);
    message_data_ += o.str ();
  }
  return *this;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
teal::vout& teal::vout::operator<< (unsigned int val)
{
  if (might_print (current_line_debug_level_)) {
    start_a_message_check_ ();
  std::ostringstream o;
  (vout_base_ == dec) ? (o << std::dec << val) :
    (o << "0x" << std::hex << val);
  message_data_ += o.str ();
}
return *this;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
teal::vout& teal::vout::operator<< (int val)
{
  if (might_print (current_line_debug_level_)) {
    start_a_message_check_ ();
    std::ostringstream o;
    (vout_base_ == dec) ? (o << std::dec << val) :
      (o << "0x" << std::hex << val);
    message_data_ += o.str ();
  }
  return *this;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
teal::vout& teal::vout::operator<< (long val)
{
  if (might_print (current_line_debug_level_)) {
    start_a_message_check_ ();
    std::ostringstream o;
    (vout_base_ == dec) ? (o << std::dec << val) :
      (o << "0x" << std::hex << val);
    message_data_ += o.str ();
  }
  return *this;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
teal::vout& teal::vout::operator<< (unsigned long val)
{
  if (might_print (current_line_debug_level_)) {
    start_a_message_check_ ();
    std::ostringstream o;
    (vout_base_ == dec) ? (o << std::dec << val) :
      (o << "0x" << std::hex << val);
    message_data_ += o.str ();
  }
  return *this;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
teal::vout& teal::vout::operator<< (long long unsigned int val)
{
  if (might_print (current_line_debug_level_)) {
    start_a_message_check_ ();
    std::ostringstream o;
    (vout_base_ == dec) ? (o << std::dec << val) :
      (o << "0x" << std::hex << val);
    message_data_ += o.str ();
  }
  return *this;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
teal::vout& teal::vout::operator<< (long long val)
{
  if (might_print (current_line_debug_level_)) {
    start_a_message_check_ ();
    std::ostringstream o;
    (vout_base_ == dec) ? (o << std::dec << val) :
      (o << "0x" << std::hex << val);
    message_data_ += o.str ();
  }
  return *this;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
teal::vout& teal::vout::operator<< (const std::string& val)
{
  if (might_print (current_line_debug_level_)) {
    //  std::cout << functional_area_ << " :vout::operator<< (" << val << ")" << std::endl;
    start_a_message_check_ ();
    std::ostringstream o;
    o << ((vout_base_ == dec) ? std::dec : std::hex) << val;
    message_data_ += o.str ();
  }
  return *this;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
teal::vout& teal::vout::operator<< (double val)
{
  if (might_print (current_line_debug_level_)) {
    start_a_message_check_ ();
    std::ostringstream o; o << val; //no hex for float
    message_data_ += o.str ();
  }
  return *this;
}


#if !defined (teal_printf_io)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string find_timescale ()
{
  //    std::cout << " vout::find_timescale() " << std::endl;
#if defined (vpi_2_0)
  //  static vpiHandle top_mod_iterator = vpi_iterate (vpiModule, NULL);
  //  static vpiHandle top_mod_handle = vpi_scan (top_mod_iterator);
  //  print ("Top module is %s\n", vpi_get_str (vpiName, top_mod_handle));

  //  switch (vpi_get (vpiTimeUnit, top_mod_handle)) {  //BUG IN IVERLOG??????, null doesn;t work either
  //  switch (vpi_get (vpiTimeUnit, NULL)) {  //BUG IN IVERLOG??????, null doesn;t work either
#if SIM==ivl
  switch (vpi_get (vpiTimePrecision,NULL)) { //just wrong, should ne unit
#else
    switch (vpi_get (vpiTimeUnit, NULL)) {
#endif

#else
  switch (tf_igettimeunit (0)) {
#endif
  case   2: return "* 100s";
  case   1: return "* 10s";
  case   0: return "s";
  case  -1: return "* 100ms";
  case  -2: return "* 10ms";
  case  -3: return "ms";
  case  -4: return "* 100us";
  case  -5: return "* 10us";
  case  -6: return "us";
  case  -7: return "* 100ns";
  case  -8: return "* 10ns";
  case  -9: return "ns";
  case -10: return "* 100ps";
  case -11: return "* 10ps";
  case -12: return "ps";
  case -13: return "* 100fs";
  case -14: return "* 10fs";
  case -15: return "fs";
  default : return "unknown timescale";
  }
  return "unknown timescale";
}


#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void teal::vout::start_a_message_check_ ()
{
  //  std::cout << functional_area_ << " vout::start_a_message_check_ "  << std::endl;
  if (!begin_message_flag_) return;
  start_a_message_ ();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool teal::vout::message_display (int id, bool new_value) 
{
  bool r (message_display_[id]);
  message_display_[id] = new_value;
  return r;
}
  
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void teal::vout::put_message (int id, std::string msg) 
{
  //  std::cout << functional_area_ << " vout may put message  " << id << "\"" << msg << "\"" << std::endl;

  if (message_display_[id]) {
    message_list_.push_back (message (id, msg));
  }
  //  std::cout << functional_area_ << " :vout::put_message end" << std::endl;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void teal::vout::start_a_message_ ()
{
  std::ostringstream o;
  o << "[" << sc_core::sc_time_stamp().to_string() << "]";
  put_message (vlog::time, o.str ());
  put_message (vlog::functional_area, "[" + functional_area_ + "]");
  std::string thread_name = "Unknown thread name";
  sc_core::sc_process_b* proc = sc_core::sc_get_curr_process_handle ();
  if (proc) thread_name = proc->name();
  put_message (vlog::thread_name, "[" + thread_name + "]");
  
  begin_message_flag_ = false;
  //    std::cout << functional_area_ << " vout::start_a_message_() end " << message_data_ << std::endl;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void teal::vout::end_a_line_ ()
{
  //    std::cout << functional_area_ << " vout end line with data " << message_data_ << std::endl;
  //  if (message_data_ != "")  put_message (vlog::message_data, message_data_);
  //  put_message (vlog::endl, "\n");
  message_data_ += "\n";
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void teal::vout::end_message_ ()
{
  //  std::cout << functional_area_ << " vout end message  "  << std::endl;
  //end_a_line_ ();
  //  put_message (vlog::endl, "\n");
  message_data_ = " "+message_data_;
  put_message (vlog::message_data, message_data_);
  //should put this in the set_file_and_line directly???
  if (might_print (current_line_debug_level_)) vlog::get().output_message (message_list_);
  clear_message_ ();
}

//let c++ do the setting to 0 as the default
teal::uint32 teal::vout::global_debug_level_floor_;
teal::uint32 teal::vout::global_debug_level_floor_time_;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void teal::vout::set_global_debug_level (teal::uint32 level, teal::uint32 start_time)
{
  global_debug_level_floor_ = level;
  global_debug_level_floor_time_ = start_time;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool teal::vout::might_print (teal::uint32 l) {
  return ((l <= show_debug_level_) || ((l <= global_debug_level_floor_) && (vtime() >= global_debug_level_floor_time_)));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void teal::vout::clear_message_ ()
{
  message_list_.clear ();
  begin_message_flag_ = true;
  file_ = "";
  line_ = (uint32)-1;
  message_data_ = "";
  current_line_debug_level_ = debug_level_; //reset to default
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void teal::vout::set_file_and_line (const std::string & file, uint32 line) 
{
  start_a_message_check_ ();
  std::string short_file = (file.find ("/") == std::string::npos) ? file : std::string (file, file.rfind ("/") + 1, std::string::npos);
  put_message (vlog::file, "[FILE: " + short_file + "]");
  char temp[256]; //big enough for a uint64
  sprintf (temp, "[line: %4d]", line);
  put_message (vlog::line, std::string (temp));
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::ofstream* teal::vout::knob_file_;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void teal::vout::set_knob_file (const std::string& n) {
  if (n == "") return;
  // knob_file_ =  fopen ((char*)n.c_str (), "w");
  knob_file_ = new std::ofstream (teal::dictionary::expand_variables(n).c_str(), std::ios::app);
}
