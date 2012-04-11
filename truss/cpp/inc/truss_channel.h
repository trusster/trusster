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
#ifndef __truss_channel__
#define __truss_channel__

namespace truss {
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    template<class data_type> 
      class channel_put {
      public:
        virtual ~channel_put () {}
        void put (const data_type& d)  {put_ (d);}
        size_t size ()  {return size_ ();}
	std::string name () {return name_ ();}

      protected:
        virtual void put_ (const data_type& d)  = 0;
	virtual size_t size_ () = 0;
	virtual std::string name_ () const  = 0;
    };


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    template<class data_type> 
      class channel_get {
      public:

         virtual ~channel_get () {};
         data_type get ()  {return get_ ();}
         size_t size ()  {return size_ ();}
	 std::string name () {return name_ ();}

      protected:
        virtual data_type get_ ()  = 0;
	virtual size_t size_ () = 0;
	virtual std::string name_ () const  = 0;
    };


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    template<typename  data_type> 
      class channel : public channel_put<data_type> , public channel_get<data_type> {
      public:
	channel (const std::string& n, teal::uint64 d = ~0ULL) :
	  depth (depth_),
	  put_condition_ (n + "_put_channel_condition"), get_condition_ (n + "_get_channel_condition"),
	  depth_ (d), mutex_ (n + "channel_mutex"), log_ (n) 
	  
	  {log_.debug_level (teal::debug); log_ << teal_debug << " ctor xxx" << teal::endm;};

	virtual ~channel () {};

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	 void put_ (const data_type& d) 
	  {
	    if (size() >= depth_) {
	      get_condition_.wait ();
	    }
	    teal::mutex_sentry foo (mutex_);
	    	    log_ << teal_debug << "put: data is " << (int) (&d) << teal::endm;
	    storage_.push_back (d);
	    	    log_ << teal_debug << "put: done data is " << (int) (&d) << teal::endm;
	    put_condition_.signal ();

	    //now for the attached channels
	    //	    for (std::deque<channel_put < data_type >* >::iterator it (listners_.begin ());
	    typename std::deque<channel_put < data_type >* >::iterator it;
	    for (it = listners_.begin ();
		 it != listners_.end (); ++it) {
	      (*it)->put (d); //may stall this one if it's depth is reached
	    }
	  }

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	 data_type get_ () 
	{
	  log_ << teal_debug << "get: size is " << size () << teal::endm;
	  while  (!size ()) {
	    put_condition_.wait ();
	        log_ << teal_debug << "get: after wait. size is " << size () << teal::endm;
	    //	    TRUSS_ASSERT (size ());  --put back when in cpp
	  }
	  teal::mutex_sentry foo (mutex_);
	  data_type returned (storage_.front());
	  log_ << teal_debug << "get: data is " << (int) (&returned) << teal::endm;
	  storage_.pop_front ();
	  get_condition_.signal ();
	  return returned;
	}

	 
         size_t size ()  {return channel_get<data_type>::size ();} //either will work

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	 size_t size_ () 
	{
	    teal::mutex_sentry foo (mutex_);	  
	    return (storage_.size());
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	const teal::uint64& depth;


	 std::string name_ () const {return put_condition_.name();} 

	 void add_listner (channel_put < data_type > * new_one) {
	  truss_assert (new_one);
	  listners_.push_back (new_one);
	}

      protected:
	std::deque<data_type> storage_;
	teal::condition put_condition_;
	teal::condition get_condition_;
	teal::uint64 depth_;
	teal::mutex mutex_;
	std::deque<channel_put < data_type >* > listners_;
	teal::vout log_;
      private:
	void operator= (const channel&);
	channel (const channel&);
    };

};

#endif
