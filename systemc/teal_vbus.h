#ifndef __teal_vbus__
#define __teal_vbus__


#define SC_THREAD2(classm,func)                                                       \
    declare_thread_process( func ## _handle,                                  \
                            #func,                                            \
                            classm,                           \
                            func )

namespace __vbus___ {

  template <class data_t>
  std::string stringize (const data_t& d) {
    std::ostringstream o; o << d; return o.str();
  }

}

#define MARK __FILE__ << " " <<std::dec << __LINE__ << " " << path_and_name_ <<  " [ " << teal::vtime () << "] " 

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template <int data_length>
class vbus : public ::vreg_internal, public sc_core::sc_module {
 public:
  explicit vbus (const std::string& path, teal::vreg::signal_control control, sc_core::sc_module_name n = "foo") : 
        vreg_internal (sc_core::sc_module_name (std::string (path + "__" + __vbus___::stringize (data_length)).c_str())),
    //    vreg_internal (sc_module_name (path.c_str())),
          reg_rd_ (new sc_core::sc_signal_rv <data_length > ("reg_rd_")), // Modified by Arnab
          reg_wr_ (new sc_core::sc_signal_rv <data_length > ("reg_wr_"))  // Modified by Arnab
    {
      resize (data_length);
#if defined (MTI_SYSTEMC) 
      teal::vout x("vbus::ctor_" + path); 
      if (control == teal::vreg::observe_and_control) {
	bool success = (path == "") or reg_wr_->control_foreign_signal (path.c_str ());
	if (!success) x << teal_fatal << "cannot find signal \"" << path << "\"" << teal::endm;
	std::string arrgh;
	for (uint32 i(0); i < data_length; ++i) arrgh += "z";
	sc_dt::sc_lv<data_length> value = arrgh.c_str();
	reg_wr_->write (value);
	reg_rd_->write (value);
        for (uint32 i(0); (i < word_length_); i++) {teal_acc_vecval_[i].aval = 0; teal_acc_vecval_[i].bval = ~0;}      

	success = (path == "") or reg_rd_->observe_foreign_signal (path.c_str ()); 
	if (!success) x << teal_fatal << "cannot find signal \"" << path << "\"" << teal::endm;


      }
      else {
	bool success = (path == "") or reg_rd_->observe_foreign_signal (path.c_str ()); 
	if (!success) x << teal_fatal << "cannot find signal \"" << path << "\"" << teal::endm;
        
        reg_wr_ = reg_rd_; 

      }
#endif
      for (int i = 0; i < data_length; ++i) {
	wires_.push_back (new vreg_internal (sc_core::sc_module_name (std::string ("dummy" + __vbus___::stringize (i) + "_" + __vbus___::stringize (data_length)).c_str())));
      }
      SC_THREAD2 (vbus, propagate_to_dut);
      SC_THREAD2 (vbus, propagate_from_dut);
    }


  //bug here - needs to be delayed one delta cycle ???
  virtual const sc_core::sc_event& value_changed_event () const {return reg_rd_->value_changed_event();};

  virtual vreg_internal& operator= (const reg& r) {
    this->reg::operator= (r);
    return *this;
  }

 private:
  mutable   sc_core::sc_signal_rv <data_length >*  reg_wr_;
  mutable   sc_core::sc_signal_rv <data_length >*  reg_rd_;

  //handle when a user changes one of our wires.
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  void propagate_to_dut (){
    for (;;) {
      sc_core::sc_event_or_list& something_changed = wires_[0]->value_changed_event () | wires_[0]->value_changed_event ();
      for (int i = 1; i < bit_length_; ++i) something_changed | wires_[i]->value_changed_event ();
      sc_core::wait (something_changed);
      //            std::cout << MARK << path_and_name_ << " vbus::propigate_to_dut test this =" << (uint*)this << " " << *this << std::endl;
      if (update_time_ == teal::vtime()) {
	continue;
      }
      //          std::cout << MARK << path_and_name_ << " vbus::propigate_to_dut really this =" << (uint*)this << " " << *this << std::endl;
      read_check2 (); //move from wires[i] to reg
      write_through (); //move from reg to systemc
    }
  }


  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  void write_through () const {
    //        std::cout << MARK << "teal::vreg_internal::write_through()  begin this =" << (uint*)this << " " << *this << std::endl;
    teal::vreg_internal::write_through ();  //move from reg to wires[i]
    //    std::cout << MARK << "teal::vreg_internal::write_through()  after vreg_internal::write_through this =" << (uint*)this << " " << *this << std::endl;

    update_time_ = teal::vtime ();

    sc_dt::sc_lv<data_length> value;
    for (int i = 0; i < bit_length_; ++i) {
      sc_dt::sc_logic val;
      switch (reg::operator() (i)) {  //move from reg to systemc
      case reg::one: val = sc_dt::Log_1; break;
      case reg::zero: val = sc_dt::Log_0; break;
      case reg::X: val = sc_dt::Log_X; break;
      case reg::Z: val = sc_dt::Log_Z; break;
      }
      value[i]= val;
    }
    //        std::cout << MARK << "teal::vreg_internal::write_through()  set DUT signals this =" << (uint*)this << " value " << value << " " << *this << std::endl;
    reg_wr_->write (value);
  }

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  void propagate_from_dut () {
    for (;;) {
      wait (reg_rd_->value_changed_event ());
      invalidate_all_vreg_internals ();
      sc_dt::sc_lv<data_length> value (reg_rd_->read());
      //          std::cout << MARK << "::propagate_from_dut" << " dut is " << value.to_string () << " this is: " << *this << endl;
      for (uint32 i(0); (i < word_length_); i++) {teal_acc_vecval_[i] = 0;}      
      for (int i = 0; i < bit_length_; ++i) {
	four_state val;
	sc_dt::sc_logic_value_t data = value[i].value();
	switch (data) {
	case sc_dt::Log_1: val = reg::one; break;
	case sc_dt::Log_0: val = reg::zero; break;
	case sc_dt::Log_X: val = reg::X; break;
	case sc_dt::Log_Z: val = reg::Z; break;
	}
	update_time_ = teal::vtime ();
	*wires_[i] = teal::reg (val); ///will cause a propagate_to_dut (), but update time will be set
	//now get the vec_val up to date (cannot do read_check because SC returns previous value)
	if ((data == sc_dt::Log_1) || (data == sc_dt::Log_X)) {
	  teal_acc_vecval_[which_word_(i)].aval |= mask_bit_ (i);
	}
	if ((data == sc_dt::Log_X) || (data == sc_dt::Log_Z)) {
	  teal_acc_vecval_[which_word_(i)].bval |= mask_bit_ (i);
	}
      }
    }
  }
};

#undef MARK

typedef vbus<2> vreg_internal2;
typedef vbus<3> vreg_internal3;
typedef vbus<4> vreg_internal4;
typedef vbus<5> vreg_internal5;
typedef vbus<6> vreg_internal6;
typedef vbus<7> vreg_internal7;
typedef vbus<8> vreg_internal8;
typedef vbus<9> vreg_internal9;
typedef vbus<10> vreg_internal10;
typedef vbus<11> vreg_internal11;
typedef vbus<12> vreg_internal12;
typedef vbus<13> vreg_internal13;
typedef vbus<14> vreg_internal14;
typedef vbus<15> vreg_internal15;
typedef vbus<16> vreg_internal16;
typedef vbus<17> vreg_internal17;
typedef vbus<18> vreg_internal18;
typedef vbus<19> vreg_internal19;
typedef vbus<20> vreg_internal20;
typedef vbus<21> vreg_internal21;
typedef vbus<22> vreg_internal22;
typedef vbus<23> vreg_internal23;
typedef vbus<24> vreg_internal24;
typedef vbus<25> vreg_internal25;
typedef vbus<26> vreg_internal26;
typedef vbus<27> vreg_internal27;
typedef vbus<28> vreg_internal28;
typedef vbus<29> vreg_internal29;
typedef vbus<30> vreg_internal30;
typedef vbus<31> vreg_internal31;
typedef vbus<32> vreg_internal32;
typedef vbus<33> vreg_internal33;
typedef vbus<34> vreg_internal34;
typedef vbus<35> vreg_internal35;
typedef vbus<36> vreg_internal36;
typedef vbus<37> vreg_internal37;
typedef vbus<38> vreg_internal38;
typedef vbus<39> vreg_internal39;
typedef vbus<40> vreg_internal40;
typedef vbus<41> vreg_internal41;
typedef vbus<42> vreg_internal42;
typedef vbus<43> vreg_internal43;
typedef vbus<44> vreg_internal44;
typedef vbus<45> vreg_internal45;
typedef vbus<46> vreg_internal46;
typedef vbus<47> vreg_internal47;
typedef vbus<48> vreg_internal48;
typedef vbus<49> vreg_internal49;
typedef vbus<50> vreg_internal50;
typedef vbus<51> vreg_internal51;
typedef vbus<52> vreg_internal52;
typedef vbus<53> vreg_internal53;
typedef vbus<54> vreg_internal54;
typedef vbus<55> vreg_internal55;
typedef vbus<56> vreg_internal56;
typedef vbus<57> vreg_internal57;
typedef vbus<58> vreg_internal58;
typedef vbus<59> vreg_internal59;
typedef vbus<60> vreg_internal60;
typedef vbus<61> vreg_internal61;
typedef vbus<62> vreg_internal62;
typedef vbus<63> vreg_internal63;
typedef vbus<64> vreg_internal64;
typedef vbus<96> vreg_internal96;

typedef vbus<192> vreg_internal192;

#endif
