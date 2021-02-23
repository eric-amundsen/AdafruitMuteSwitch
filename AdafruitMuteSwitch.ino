//////////////////////////////////////////////////////////////////////
//
// AdafruitMuteSwitch
// Monitor pins on a Adafruit Flora, when state change (1->0 or 0->) output
// a keyboard key combination to facilitate an action in Microsoft Teams
//
// Author : Eric Amundsen
// Creation Date : 2021-02-19

#include <Keyboard.h>

//////////////////////////////////////////////////////////////////////
// PIN DEFINITION
#define AMUTE_SWITCH 10
#define VMUTE_SWITCH 9
#define HAND_SWITCH  6

//////////////////////////////////////////////////////////////////////
// CHARACTER DEFINITION
#define AMUTE_CHAR 'm'
#define VMUTE_CHAR 'o'
#define  HAND_CHAR 'k'

//////////////////////////////////////////////////////////////////////
// states to keep track of pin state and create action during transition
typedef enum states {S_IDLE, S_ZERO, S_ONE, S_XFER};

//////////////////////////////////////////////////////////////////////
// Globals
// Initialize the 3 pins' state machines
states amute_state = S_IDLE;
states vmute_state = S_IDLE; 
states hand_state  = S_IDLE; 

//////////////////////////////////////////////////////////////////////
// 
int buttonState(  // return 1 when in XFER state, 0 otherwise
  int button,     // the pin to read
  states *st_ptr) // the state of the pin, updated herein
  {

  int rv = 0;

  // start in IDLE, move to S_ZERO if pin at 0, S_ONE if pin at 1
  // stay in S_ZERO if pin at 0, stay in S_ONE if pin at 1
  // move to S_XFER when pin doesn't match state return 1
  // when in S_XFER move to appropiate state (pin == 0, S_ZERO)
  switch (*st_ptr) {
    case S_IDLE : if (digitalRead(button) == 0) *st_ptr = S_ZERO;
                  else                          *st_ptr = S_ONE;
                  break;
              
    case S_ZERO : if (digitalRead(button) == 0) *st_ptr = S_ZERO;
                  else                          *st_ptr = S_XFER;
                  break;
              
    case S_ONE  : if (digitalRead(button) == 1) *st_ptr = S_ONE;
                  else                          *st_ptr = S_XFER;
                  break;
              
    case S_XFER : if (digitalRead(button) == 0) *st_ptr = S_ZERO;
                  else                          *st_ptr = S_ONE;
                  rv = 1;
                  break;
                  
    default : *st_ptr = S_IDLE;
              break;
  }
  return (rv);
}

void setup() {

  Keyboard.begin();

  pinMode(AMUTE_SWITCH, INPUT);
  pinMode(VMUTE_SWITCH, INPUT);
  pinMode(HAND_SWITCH, INPUT);

}

// press modifier keys and write character
void sendToggle(char chr, int shift, int ctrl, int alt, int gui) {

  if (shift) Keyboard.press(KEY_LEFT_SHIFT);
  if (ctrl)  Keyboard.press(KEY_LEFT_CTRL);
  if (alt)   Keyboard.press(KEY_LEFT_ALT);
  if (gui)   Keyboard.press(KEY_LEFT_GUI);
  Keyboard.write(chr);
  delay(250);
  Keyboard.releaseAll();
}

// call state machine and if return 1 send character
void checkSwitch(int swtch, char chr, states *s_ptr) {
  if (buttonState(swtch, s_ptr)) {
    sendToggle(chr, 1, 1, 0, 0);  // TODO this assumes all characters use shift-ctrl modifier,
                                  // find elegant way to not do this
  }
}


void loop() {

  checkSwitch(AMUTE_SWITCH, AMUTE_CHAR, &amute_state);
  checkSwitch(VMUTE_SWITCH, VMUTE_CHAR, &vmute_state);
  checkSwitch( HAND_SWITCH,  HAND_CHAR, &hand_state);

  delay(250);

}
