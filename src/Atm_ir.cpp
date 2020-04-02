#include "Atm_ir.hpp"

#define BIT_LEN 0
#define BIT_START_H 1
#define BIT_START_L 2
#define BIT_DATA_H 3
#define BIT_DATA_L 4
#define BIT_DATA_LEN 5
#define BIT_DATA 6

/* Add optional parameters for the state machine to begin()
 * Add extra initialization code
 */

Atm_ir &Atm_ir::begin(uint8_t pin) {
  // clang-format off
  const static state_t state_table[] PROGMEM = {
    /*                             ON_ENTER  ON_LOOP  ON_EXIT    EVT_GOT_DATA  ELSE */
    /*        WAITING */                 -1,      -1,      -1, DATA_AVAILABLE,   -1,
    /* DATA_AVAILABLE */ ENT_DATA_AVAILABLE,      -1,      -1, DATA_AVAILABLE,   -1,
  };
  // clang-format on
  Machine::begin( state_table, ELSE );
  IR.Init(pin);
  return *this;
}

/* Add C++ code for each internally handled event (input) 
 * The code must return 1 to trigger the event
 */

int Atm_ir::event( int id ) {
  switch ( id ) {
    case EVT_GOT_DATA:
      return IR.IsDta();
  }
  return 0;
}

/* Add C++ code for each action
 * This generates the 'output' for the state machine
 *
 * Available connectors:
 *   push( connectors, ON_DATA, 0, <v>, <up> );
 */

void Atm_ir::action( int id ) {
  switch ( id ) {
    case ENT_DATA_AVAILABLE:
      readData();
      return;
  }
}

void Atm_ir::readData() {
  IR.Recv(irMessage);
  uint32_t irData = *((uint32_t *)(irMessage + BIT_DATA));
  uint16_t irData1 = (uint16_t)(irData >> 16);
  uint16_t irData2 = (uint16_t)(irData & 0x0000FFFFuL);
  push(connectors, ON_DATA, 0, irData1, irData2);
}

/* Optionally override the default trigger() method
 * Control how your machine processes triggers
 */

Atm_ir& Atm_ir::trigger( int event ) {
  Machine::trigger( event );
  return *this;
}

/* Optionally override the default state() method
 * Control what the machine returns when another process requests its state
 */

int Atm_ir::state( void ) {
  return Machine::state();
}

/* Nothing customizable below this line
 ************************************************************************************************
*/

/* Public event methods
 *
 */

/*
 * onData() push connector variants ( slots 1, autostore 0, broadcast 0 )
 */

Atm_ir& Atm_ir::onData( Machine& machine, int event ) {
  onPush( connectors, ON_DATA, 0, 1, 1, machine, event );
  return *this;
}

Atm_ir& Atm_ir::onData( atm_cb_push_t callback, int idx ) {
  onPush( connectors, ON_DATA, 0, 1, 1, callback, idx );
  return *this;
}

/* State trace method
 * Sets the symbol table and the default logging method for serial monitoring
 */

Atm_ir& Atm_ir::trace( Stream & stream ) {
  Machine::setTrace( &stream, atm_serial_debug::trace,
    "IR\0EVT_GOT_DATA\0ELSE\0WAITING\0DATA_AVAILABLE" );
  return *this;
}

