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
#ifndef __uart_1655_register_definitions__
#define __uart_1655_register_definitions__

#include "truss_register_access.h"

///note that its originally from uart_defines. <--- do this officilly in copyleft


#define UART_REG_RB 0	// receiver buffer, on read
#define UART_REG_TR 0	// transmitter , on write
#define UART_REG_IE 1	// Interrupt enable
#define UART_REG_II 2	// Interrupt identification, on read
#define UART_REG_FC 2	// FIFO control, on write
#define UART_REG_LC 3	// Line Control
#define UART_REG_MC 4	// Modem control
#define UART_REG_LS 5	// Line status
#define UART_REG_MS 6	// Modem status
#define UART_REG_SR 7	// Scratch register
#define UART_REG_DL1 0	// Divisor latch bytes (1-2)
#define UART_REG_DL2 1

//fields
  //    LCR             (R/W | ADR 3)
  //      [1:0] -RX-TX- "00" 5 bits in each character
  //      [1:0] -RX-TX- "01" 6 bits in each character
  //      [1:0] -RX-TX- "10" 7 bits in each character
  //      [1:0] -RX-TX- "11" 8 bits in each character
  //      [2]   -RX-TX- "0" 1 stop bit
  //      [2]   -RX-TX- "1" 1.5 stop bits (when 5 bits of char.) or 2 stop bits (when 6, 7 or 8 bits of char.)
  //      [3]   -RX-TX- "1" Parity bit enabled
  //      [5:4] -RX-TX- "00" NO Stick Parity & ODD Parity bit - ODD num. of '1's is transmitted
  //      [5:4] -RX-TX- "01" NO Stick Parity & EVEN Parity bit - EVEN num. of '1's is transmitted
  //      [5:4] -RX-TX- "10" Stick Parity bit - Stick '1' as Parity bit
  //      [5:4] -RX-TX- "11" Stick Parity bit - Stick '0' as Parity bit
  //      [6]   ----TX- "1" Break Control - Output is forced to '0'
  //      [7]   ------- "1" DLAB - for access to DLL and DLM

#define data_size_min 0
#define data_size_max 1
#define stop_bits_min 2
#define stop_bits_max 2
#define parity_enable_min 3
#define parity_enable_max 3
#define parity_type_min 4
#define parity_type_max 5
#define force_break_min 6
#define force_break_max 6
#define access_clock_divide_min 7
#define access_clock_divide_max 7


  //    LSR             (R/  | ADR 5)
  //      [0]   -RX---- "1" Data Ready - At least 1 char. received and is in Fifo----------> READ RBR (Fifo empty)
  //      [1]   -RX---- "1" Overrun Error - Fifo full & 1 char. received in shift reg. ----> READ LSR
  //      [2]   -RX---- "1" Parity Error - top Fifo char. has invalid parity bit ----------> READ LSR
  //      [3]   -RX---- "1" Framing Error - top Fifo char. has invalid stop bit -----------> READ LSR
  //      [4]   -RX---- "1" Break Int. - top Fifo char. bits are '0' and it's ctrl. bits --> READ LSR
  //      [5]   ----TX- "1" Transmitter Holding Register Empty - transmitter Fifo empty ---> WRITE THR
  //      [6]   ----TX- "1" Transmitter EMpTy - transmitter Fifo empty & shift reg. empty -> WRITE THR
  //      [7]   -RX---- "1" At least 1 Parity Error, Framing Error or Break Int. in Fifo --> READ LSR & No More Errors in Fifo
#define data_received_min 0
#define data_received_max 0
#define overrun_error_min 1
#define overrun_error_max 1
#define parity_error_min 2
#define parity_error_max 2
#define framing_error_min 3
#define framing_error_max 3
#define break_received_min 4
#define break_received_max 4
#define transmit_fifo_empty_min 5
#define transmit_fifo_empty_max 5
#define transmit_empty_min 6
#define transmit_empty_max 6
#define receive_error_min 7
#define receive_error_max 7


  //    IER             (R/W | ADR 1 | DLAB 0) 
  //      [0]   -RX---- "1" Received Data Available & Receive Fifo Timeout
  //      [1]   ----TX- "1" Transmitter Holding Register Empty
  //      [2]   -RX---- "1" Receiver Line Status
  //      [3]   -MODEM- "1" Modem Status

#define interrupt_enable_receive_min 0
#define interrupt_enable_receive_max 0
#define interrupt_enable_transmit_empty_min 1
#define interrupt_enable_transmit_empty_max 1
#define interrupt_enable_line_status_change_min 2
#define interrupt_enable_line_status_change_max 2
#define interrupt_enable_modem_change_min 3
#define interrupt_enable_modem_change_max 3


  //    ----------------
  //    IIR             (R/  | ADR 2)
  //      [0]   ------- "0" Interrupt is Pending (decreasing priority level in following 3 bits)
  //      [3:1] -RX---- "011" Receiver Line Status - Overrun, Parity, Framing error or Break int. ---> READ LSR
  //      [3:1] -RX---- "010" Received Data Available - Fifo Trigger Level Reached ------------------> READ RBR (Fifo lower than trig.)
  //      [3:1] -RX---- "110" Timeout Indication - Fifo not empty & no Fifo action for 4 char times -> READ RBR
  //      [3:1] ----TX- "001" Transmitter Holding Register Empty - THR Empty ------------------------> READ IIR | WRITE THR
  //      [3:1] -MODEM- "000" Modem Status - CTS, DSR, DCD changed or RI changed from '0' to '1' ----> READ MSR

#define interrupt_identification_interrupt_pending_min 0
#define interrupt_identification_interrupt_pending_max 0

#define interrupt_identification_min 1
#define interrupt_identification_max 3
//values for above
#define interrupt_identification_modem_status 0
#define interrupt_identification_transmit_empty 1
#define interrupt_identification_received_data 2
#define interrupt_identification_line_status 3
#define interrupt_identification_timeout 6



#define interrupt_identification_received_data_availible_max 0
#define interrupt_identification_timeout_min 0
#define interrupt_identification_timeout_max 0
#define interrupt_identification_transmit_empty_min 0
#define interrupt_identification_transmit_ending_max 0



// UART registers
//######################

  // UART registers:
  //    ----------------
  //    RBR             (R/  | ADR 0 | DLAB 0)
  //      [7:0] -RX---- "rxdata" Receiver Buffer Register
  //    ----------------
  //    THR             ( /W | ADR 0 | DLAB 0)
  //      [7:0] ----TX- "txdata" Transmitter Holding Register
  //    ----------------
  //    IER             (R/W | ADR 1 | DLAB 0) 
  //      [0]   -RX---- "1" Received Data Available & Receive Fifo Timeout
  //      [1]   ----TX- "1" Transmitter Holding Register Empty
  //      [2]   -RX---- "1" Receiver Line Status
  //      [3]   -MODEM- "1" Modem Status
  //    ----------------
  //    IIR             (R/  | ADR 2)
  //      [0]   ------- "0" Interrupt is Pending (decreasing priority level in following 3 bits)
  //      [3:1] -RX---- "011" Receiver Line Status - Overrun, Parity, Framing error or Break int. ---> READ LSR
  //      [3:1] -RX---- "010" Received Data Available - Fifo Trigger Level Reached ------------------> READ RBR (Fifo lower than trig.)
  //      [3:1] -RX---- "110" Timeout Indication - Fifo not empty & no Fifo action for 4 char times -> READ RBR
  //      [3:1] ----TX- "001" Transmitter Holding Register Empty - THR Empty ------------------------> READ IIR | WRITE THR
  //      [3:1] -MODEM- "000" Modem Status - CTS, DSR, DCD changed or RI changed from '0' to '1' ----> READ MSR
  //    ----------------
  //    FCR             ( /W | ADR 2)
  //      [1]   -RX---- "1" Clear only Receiver Fifo (not shift register)
  //      [2]   ----TX- "1" Clear only Transmitter Fifo (not shift register)
  //      [7:6] -RX---- "00"  1 BYTE  Receiver Fifo Interrupt trigger level
  //      [7:6] -RX---- "01"  4 BYTEs Receiver Fifo Interrupt trigger level
  //      [7:6] -RX---- "10"  8 BYTEs Receiver Fifo Interrupt trigger level
  //      [7:6] -RX---- "11" 14 BYTEs Receiver Fifo Interrupt trigger level
  //    ----------------
  //    LCR             (R/W | ADR 3)
  //      [1:0] -RX-TX- "00" 5 bits in each character
  //      [1:0] -RX-TX- "01" 6 bits in each character
  //      [1:0] -RX-TX- "10" 7 bits in each character
  //      [1:0] -RX-TX- "11" 8 bits in each character
  //      [2]   -RX-TX- "0" 1 stop bit
  //      [2]   -RX-TX- "1" 1.5 stop bits (when 5 bits of char.) or 2 stop bits (when 6, 7 or 8 bits of char.)
  //      [3]   -RX-TX- "1" Parity bit enabled
  //      [5:4] -RX-TX- "00" NO Stick Parity & ODD Parity bit - ODD num. of '1's is transmitted
  //      [5:4] -RX-TX- "01" NO Stick Parity & EVEN Parity bit - EVEN num. of '1's is transmitted
  //      [5:4] -RX-TX- "10" Stick Parity bit - Stick '1' as Parity bit
  //      [5:4] -RX-TX- "11" Stick Parity bit - Stick '0' as Parity bit
  //      [6]   ----TX- "1" Break Control - Output is forced to '0'
  //      [7]   ------- "1" DLAB - for access to DLL and DLM
  //    ----------------
  //    MCR             ( /W | ADR 4)
  //      [0]   -MODEM- "1" Force DTR to '0' - in LoopBack connected to DSR input
  //      [1]   -MODEM- "1" Force RTS to '0' - in LoopBack connected to CTS input
  //      [2]   -MODEM- "1" Force N.C.1 to '0' - in LoopBack connected to RI input
  //      [3]   -MODEM- "1" Force N.C.2 to '0' - in LoopBack connected to DCD input
  //      [4]   -MODEM- "1" LoopBack mode
  //    ----------------
  //    ----------------
  //    MSR             (R/  | ADR 6)
  //      [0]   -MODEM- "1" Delta CTS indicator - CTS has changed it's state --------------> READ MSR
  //      [1]   -MODEM- "1" Delta DSR indicator - DSR has changed it's state --------------> READ MSR
  //      [2]   -MODEM- "1" Trailing Edge of RI - RI has changed from '0' to '1' ----------> READ MSR
  //      [3]   -MODEM- "1" Delta DCD indicator - DCD has changed it's state --------------> READ MSR
  //      [4]   -MODEM- "x" Complement of CTS input | in LoopBack equal to RTS = MCR[1]
  //      [5]   -MODEM- "x" Complement of DSR input | in LoopBack equal to DTR = MCR[0]
  //      [6]   -MODEM- "x" Complement of RI input | in LoopBack equal to N.C.1 = MCR[2]
  //      [7]   -MODEM- "x" Complement of DCD input | in LoopBack equal to N.C.2 = MCR[3]
  //    ----------------
  //    DLL             (R/W | ADR 0 | DLAB 1)
  //      [7:0] ------- "dl[ 7:0]" LSB of DL Reg. written 2. - dl == '0' disables outputs / dl = 1/(T_wb_clk_period*16*BaudRate)
  //    ----------------
  //    DLM             (R/W | ADR 1 | DLAB 1)
  //      [7:0] ------- "dl[15:8]" MSB of DL Reg. written 1. - dl == '0' disables outputs / dl = 1/(T_wb_clk_period*16*BaudRate)
  //    ----------------

#endif
