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

//WARNING: This is not a stand-alone header file. It is intended to be used as part of teal.h

  ////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////
  ///////////////// Vlog/Vout  /////////////////
  ////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////
  typedef std::pair<int, std::string> message; //first is message id
  typedef std::deque<message> message_list;
  typedef std::deque<message>::const_iterator message_list_iterator;

  typedef std::pair<message,message> message_guide; //first is match, second is action
  typedef std::deque<message_guide> message_guide_list;
  typedef std::deque<message_guide>::iterator message_guide_list_iterator;


  ////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////
  class vlog {
  public:
    //Teal predefined message IDs (reserved from 0x800 - 0xf00)
    //Note: Applications can use 0x00 through 0x7ff
    typedef enum {first_id = 0x800, time, thread_name, functional_area,
		  fatal, error, info, debug, coverage, message_data, endl, 
		  file, line, endm,
		  last_id = endm} teal_predefined_message_ids;

    virtual ~vlog ();

    static vlog& get (); //gets the top-most one (local_vlog one initially)

    virtual void output_message (const message_list & msg);

    virtual void local_print (const std::string& val)
    {
      std::string val2 = local_print_ (val);
      if (after_me_ && (val2 != "")) after_me_->local_print (val2);
    }
    virtual unsigned int how_many (int); //given a meta-info tag, how many got printed?


  protected:
    //The impelmentation exists and just increments how_many(id) and packs into a string
    virtual message_list output_message_ (const message_list& m) {return m;}
    //if the vlog::output_message_() decides to put out a meddage_id's string...
    virtual std::string local_print_ (const std::string& val) {return val;}

    vlog ();

  protected:
    static vlog* the_;
    std::map <int, int> id_count_;

    //    vlog (const vlog&);
    //    vlog& operator= (const vlog&);
    vlog* after_me_; //use by the push logic to create a chain (intrusive list)
  };

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  class file_vlog : public vlog {
  public:
    file_vlog (std::string file_name, bool also_to_screen = true);
    ~file_vlog ();

  protected:
    virtual std::string local_print_ (const std::string& val);
    virtual message_list output_message_ (const message_list& m) {return m;} 
 
  private:
      FILE* out_file_;
      bool also_to_screen_;
  };



  ////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////
    const uint32 debug = 1; //use with debug_level()
    const uint32 no_debug = 0; //use with debug_level()

  ////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////
  class vout {
  public:
    vout (const std::string& functional_area);
    vout (const std::string& functional_area, uint32 initial_show_level);
    virtual ~vout () {};

    static void set_global_debug_level (teal::uint32 level, teal::uint32 start_time);

    ////////////////////////////////////////////////////////////////
    //Use these to print out all data (as opposed to meta-level info).
    virtual vout& operator<< (char);
    virtual vout& operator<< (int);
    virtual vout& operator<< (long);
    virtual vout& operator<< (long long);
    virtual vout& operator<< (unsigned char);
    virtual vout& operator<< (unsigned int);
    virtual vout& operator<< (unsigned long);
    virtual vout& operator<< (long long unsigned int);
    virtual vout& operator<< (const std::string&);
    virtual vout& operator<< (double);

    //The ones below are "lower" in the interface. Consider using the manipulators.

    void put_message (int id, std::string msg);

    //nominally the enum info_type, but left as int for expansion
    //The first way to not print some bit of data.
    //if message_data is used, not printing will be produced (unless if error?)
    virtual /*previous*/ bool message_display (int id, bool new_value);

    ////////////////////////////////////////////////////////////////
    typedef enum vout_base {dec = 11, hex = 22, binary = 33};
    void base (vout_base);
    vout_base base ();
    
    //some simulators do not like tf_getlongtime() at end of compile.
    //See synch.cpp


    ////////////////////////////////////////////////////////////////
    //This allows the endl, dec, and hex manips to execute
    vout& operator<< (vout& (*f)(vout&)) {return (*f)(*this);}

    virtual std::string feature (int f, const std::string& val) {std::string ret (feature_[f]); feature_[f] = val; return ret;};
    virtual std::string feature (int f) const {return (feature_[f]);};


    //return true if the passed level would pass the level test. Might still be squashed by vlog processing.
    bool might_print (teal::uint32 l);

  private:
    uint32 show_debug_level_;
    uint32 current_line_debug_level_;
    uint32 debug_level_;
    std::map<int,bool> message_display_;
    mutable std::map<int,std::string> feature_;

    static teal::uint32 global_debug_level_floor_;
    static teal::uint32 global_debug_level_floor_time_;


  public:
    ////////////////////////////////////////////////////////////////
    //ships it off to the logger
    void end_message_ ();
    void clear_message_ (); //forget the whole message
    void end_a_line_ ();

    void set_file_and_line (const std::string & file, uint32 line);

    uint32 current_line_debug_level (uint32 new_one) 
      {uint32 returned (current_line_debug_level_); current_line_debug_level_ = new_one; return (returned);}

    uint32 debug_level (uint32 new_one) {uint32 returned (debug_level_); debug_level_ = new_one; return (returned);}

    uint32 show_debug_level (uint32 new_one) 
      {uint32 returned (show_debug_level_); show_debug_level_ = new_one; return (returned);}

    virtual void start_a_message_check_ (); //called before every create
    virtual void start_a_message_ ();

    static void set_knob_file (const std::string&);

  private:    
    vout_base vout_base_;
    bool begin_message_flag_;
    std::string message_data_;
    std::string functional_area_;
    message_list message_list_;
    uint32 line_;
    std::string file_;
    //allow copy and assignment
    static std::ofstream* knob_file_;
  }; //class vout

  //MUST be called to release this threads use of vout
  inline vout& endl (vout& a_vout) {a_vout.end_a_line_ (); return a_vout;}
  inline vout& endm (vout& a_vout) {a_vout.end_message_ (); return a_vout;}

  //to set the output format
  inline vout& hex (vout& a_vout) {a_vout.base (teal::vout::hex); return a_vout;}
  inline vout& dec (vout& a_vout) {a_vout.base (teal::vout::dec); return a_vout;}
  inline vout& bin (vout& a_vout) {a_vout.base (teal::vout::binary); return a_vout;}

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  template <class TP1, class TP2> struct vmanip {
    vout& (*f_)(vout&, TP1, TP2);
    TP1 a_;
    TP2 b_;
  public:
    vmanip(vout& (*f)(vout&, TP1, TP2), TP1 a, TP2 b) : f_(f), a_(a), b_(b) {}
  };

  template<class TP1, class TP2>
    teal::vout& operator<<(teal::vout& o, const teal::vmanip < TP1, TP2 > & m)  { (*m.f_)(o, m.a_, m.b_); return o;}



  extern inline vout& __vmanip_set_file_and_line (vout& a_vout, const std::string & file, int  value) {
    //    a_vout.set_file_and_line (file, value);
    return a_vout;
  }

  inline vmanip<const std::string &,int> file_and_line_only (const std::string & file, int n) {return vmanip<const std::string &, int> (__vmanip_set_file_and_line, file, n); }

  inline vout& __vmanip_set_start_file_and_line (vout& a_vout, const std::string & file, std::pair < int, int > mark_me) {
    a_vout.set_file_and_line (file, mark_me.second);
    switch (mark_me.first) {
    case vlog::info: a_vout.put_message (vlog::info, "[INFO]"); break;
    case vlog::coverage: a_vout.put_message (vlog::coverage, "[COVERAGE]"); break;
    case vlog::error: a_vout.put_message (vlog::error, "[ERROR]"); break;
    case vlog::fatal: a_vout.put_message (vlog::fatal, "[FATAL_ERROR]"); break;
    case vlog::debug: {a_vout.put_message (vlog::debug, "[DEBUG]"); a_vout.current_line_debug_level (1); break;};
    default: a_vout.put_message (vlog::info, "[UNKNOWN???]");
    }
    return a_vout;
  }

  inline teal::vmanip< const std::string &, std::pair <int,int > > mark_start (const std::string & file, std::pair < int, int > mark_me) {return teal::vmanip < const std::string&, std::pair <int, int> > (__vmanip_set_start_file_and_line, file, mark_me); }



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define teal_fatal teal::mark_start (__FILE__, std::pair < int, int > (teal::vlog::fatal, __LINE__))
#define teal_error teal::mark_start (__FILE__, std::pair < int, int > (teal::vlog::error, __LINE__))
#define teal_info teal::mark_start (__FILE__, std::pair < int, int > (teal::vlog::info, __LINE__))
#define teal_cov teal::mark_start (__FILE__, std::pair < int, int > (teal::vlog::coverage, __LINE__))
#define teal_debug teal::mark_start (__FILE__, std::pair < int, int > (teal::vlog::debug, __LINE__))


#if 0
//Robert, your html goes here,
  extern inline vout& __vmanip_setfeature (vout& a_vout, int a_feature, const std::string&  value) {
    a_vout.feature (a_feature, value);
    return a_vout;
  }
  inline vmanip<int,int> feature (int f, const std::string& n) {return vmanip<int, int> (__vmanip_setfeature, f, n); }
#endif

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  template <class TP1> struct vmanip_1 {
    vout& (*f_)(vout&, TP1);
    TP1 a_;
  public:
    vmanip_1 (vout& (*f)(vout&, TP1), TP1 a) : f_(f), a_(a) {}
  };

  template<class TP1>
    teal::vout& operator<<(teal::vout& o, const teal::vmanip_1 < TP1 > & m)  { (*m.f_)(o, m.a_); return o;}

  inline vout& __vmanip_set_current_level (vout& a_vout, uint32  value) {
    a_vout.current_line_debug_level (value);
    return a_vout;
  }
  inline vmanip_1<uint32> level (uint32 n) {return vmanip_1<uint32> (__vmanip_set_current_level, n); }

