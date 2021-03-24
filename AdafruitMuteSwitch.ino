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
#define APP_SWITCH   12
#define OS_SWITCH    3

//#define USE_MAC
//#define USE_WIN

#if !(defined(USE_WIN) || defined(USE_MAC))
#error "have to define at least 1 OS"
#endif

//////////////////////////////////////////////////////////////////////
// MASKS

// OS
#define WINDOWS 0x100
#define MAC     0x200

// APP
#define TEAMS 0x1000
#define ZOOM  0x2000

// MODIFIER
#define SHIFT_MASK  0x001
#define CTRL_MASK   0x002
#define ALT_MASK    0x004
#define GUI_MASK    0x008

//////////////////////////////////////////////////////////////////////
// CHARACTER DEFINITION
//
// Windows
//   Teams
#define WIN_TEAMS_AMUTE_MOD  SHIFT_MASK | CTRL_MASK
#define WIN_TEAMS_VMUTE_MOD  SHIFT_MASK | CTRL_MASK
#define WIN_TEAMS_HAND_MOD   SHIFT_MASK | CTRL_MASK

#define WIN_TEAMS_AMUTE_CHAR 'm'
#define WIN_TEAMS_VMUTE_CHAR 'o'
#define WIN_TEAMS_HAND_CHAR  'k'

//   Zoom
#define WIN_ZOOM_AMUTE_MOD  ALT_MASK
#define WIN_ZOOM_VMUTE_MOD  ALT_MASK
#define WIN_ZOOM_HAND_MOD   ALT_MASK

#define WIN_ZOOM_AMUTE_CHAR 'a'
#define WIN_ZOOM_VMUTE_CHAR 'v'
#define WIN_ZOOM_HAND_CHAR  'y'

// MAC
//   Teams
#define MAC_TEAMS_AMUTE_MOD  SHIFT_MASK | GUI_MASK
#define MAC_TEAMS_VMUTE_MOD  SHIFT_MASK | GUI_MASK
#define MAC_TEAMS_HAND_MOD   SHIFT_MASK | GUI_MASK

#define MAC_TEAMS_AMUTE_CHAR 'm'
#define MAC_TEAMS_VMUTE_CHAR 'o'
#define MAC_TEAMS_HAND_CHAR  'k'

//   Zoom
#define MAC_ZOOM_AMUTE_MOD  SHIFT_MASK | GUI_MASK
#define MAC_ZOOM_VMUTE_MOD  SHIFT_MASK | GUI_MASK
#define MAC_ZOOM_HAND_MOD   ALT_MASK

#define MAC_ZOOM_AMUTE_CHAR 'a'
#define MAC_ZOOM_VMUTE_CHAR 'v'
#define MAC_ZOOM_HAND_CHAR  'y'


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

// press modifier keys and write character
void sendToggle(char chr, int modifier) {


  if (SHIFT_MASK & modifier)  Keyboard.press(KEY_LEFT_SHIFT);
  if (CTRL_MASK  & modifier)  Keyboard.press(KEY_LEFT_CTRL);
  if (ALT_MASK   & modifier)  Keyboard.press(KEY_LEFT_ALT);
  if (GUI_MASK   & modifier)  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.write(chr);
  delay(250);
  Keyboard.releaseAll();
}

// call state machine and if return 1 send character
void checkSwitch(int swtch, char chr, int modifier, states *s_ptr) {
  if (buttonState(swtch, s_ptr)) {
    sendToggle(chr, modifier);
  }
}

// check the switch for windows/mac os
int checkOS() {
#if defined(USE_MAC) && defined(USE_WIN) 
  if (digitalRead(OS_SWITCH) == 1) {
    return MAC;
  } else {
    return WINDOWS;
  }
#else

#ifdef USE_MAC
    return MAC;
#endif

#ifdef USE_WIN
    return WINDOWS;
#endif

#endif
}

int checkApp() {
  if (digitalRead(APP_SWITCH) == 0) {
    return TEAMS;
  } else {
    return ZOOM;
  }
}

void setup() {


  Keyboard.begin();

  pinMode(AMUTE_SWITCH, INPUT_PULLUP);
  pinMode(VMUTE_SWITCH, INPUT_PULLUP);
  pinMode(HAND_SWITCH, INPUT_PULLUP);
  pinMode(APP_SWITCH, INPUT_PULLUP);

#if defined(USE_MAC) && defined(USE_WIN) 
  pinMode(OS_SWITCH, INPUT_PULLUP);
#endif

}

void loop() {

  int amute_char, amute_mod;
  int vmute_char, vmute_mod;
  int  hand_char,  hand_mod;

#ifdef USE_WIN
  if (checkOS() == WINDOWS) {
    if (checkApp() == TEAMS) {
      amute_char = WIN_TEAMS_AMUTE_CHAR;
      vmute_char = WIN_TEAMS_VMUTE_CHAR;
       hand_char = WIN_TEAMS_HAND_CHAR;

      amute_mod = WIN_TEAMS_AMUTE_MOD;
      vmute_mod = WIN_TEAMS_VMUTE_MOD;
       hand_mod = WIN_TEAMS_HAND_MOD;
    } else {
      amute_char = WIN_ZOOM_AMUTE_CHAR;
      vmute_char = WIN_ZOOM_VMUTE_CHAR;
       hand_char = WIN_ZOOM_HAND_CHAR;

      amute_mod = WIN_ZOOM_AMUTE_MOD;
      vmute_mod = WIN_ZOOM_VMUTE_MOD;
       hand_mod = WIN_ZOOM_HAND_MOD;
     }
  }
#endif

#ifdef USE_MAC
  if (checkOS() == MAC) {
    if (checkApp() == TEAMS) {
      amute_char = MAC_TEAMS_AMUTE_CHAR;
      vmute_char = MAC_TEAMS_VMUTE_CHAR;
       hand_char = MAC_TEAMS_HAND_CHAR;

      amute_mod = MAC_TEAMS_AMUTE_MOD;
      vmute_mod = MAC_TEAMS_VMUTE_MOD;
       hand_mod = MAC_TEAMS_HAND_MOD;
    } else {
      amute_char = MAC_ZOOM_AMUTE_CHAR;
      vmute_char = MAC_ZOOM_VMUTE_CHAR;
       hand_char = MAC_ZOOM_HAND_CHAR;

      amute_mod = MAC_ZOOM_AMUTE_MOD;
      vmute_mod = MAC_ZOOM_VMUTE_MOD;
       hand_mod = MAC_ZOOM_HAND_MOD;
     }
  }
#endif

  checkSwitch(AMUTE_SWITCH, amute_char, amute_mod, &amute_state);
  checkSwitch(VMUTE_SWITCH, vmute_char, vmute_mod, &vmute_state);
  checkSwitch( HAND_SWITCH,  hand_char,  hand_mod, &hand_state);

  delay(250);

}
