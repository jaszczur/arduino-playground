#pragma once

#include <Automaton.h>
#include <IRSendRev.h>

class Atm_ir : public Machine {

public:
  enum { WAITING, DATA_AVAILABLE }; // STATES
  enum { EVT_GOT_DATA, ELSE };      // EVENTS
  Atm_ir(void) : Machine(){};
  Atm_ir &begin(uint8_t pin);
  Atm_ir &trace(Stream &stream);
  Atm_ir &trigger(int event);
  int state(void);
  Atm_ir &onData(Machine &machine, int event = 0);
  Atm_ir &onData(atm_cb_push_t callback, int idx = 0);

private:
  enum { ENT_DATA_AVAILABLE }; // ACTIONS
  enum { ON_DATA, CONN_MAX };  // CONNECTORS
  atm_connector connectors[CONN_MAX];
  int event(int id);
  void action(int id);
  void readData();
  unsigned char irMessage[20];
};

/*
Automaton::ATML::begin - Automaton Markup Language

<?xml version="1.0" encoding="UTF-8"?>
<machines>
  <machine name="Atm_ir">
    <states>
      <WAITING index="0">
        <EVT_GOT_DATA>DATA_AVAILABLE</EVT_GOT_DATA>
      </WAITING>
      <DATA_AVAILABLE index="1" on_enter="ENT_DATA_AVAILABLE">
        <EVT_GOT_DATA>DATA_AVAILABLE</EVT_GOT_DATA>
      </DATA_AVAILABLE>
    </states>
    <events>
      <EVT_GOT_DATA index="0" access="PRIVATE"/>
    </events>
    <connectors>
      <DATA autostore="0" broadcast="0" dir="PUSH" slots="1"/>
    </connectors>
    <methods>
    </methods>
  </machine>
</machines>

Automaton::ATML::end
*/
