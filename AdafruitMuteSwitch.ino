#include <Keyboard.h>

#define AMUTE_SWITCH 10
#define VMUTE_SWITCH 9
#define HAND_SWITCH  6

#define AMUTE_CHAR 'm'
#define VMUTE_CHAR 'o'
#define  HAND_CHAR 'k'


typedef enum states {S_IDLE, S_ZERO, S_ONE, S_XFER};

states amute_state = S_IDLE;
states vmute_state = S_IDLE; 
states hand_state  = S_IDLE; 

int buttonState(int button, states *st_ptr) {

  // return value, only true upon release of the button
  int rv = 0;

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

void sendToggle(char chr, int shift, int ctrl, int alt, int gui) {

  if (shift) Keyboard.press(KEY_LEFT_SHIFT);
  if (ctrl)  Keyboard.press(KEY_LEFT_CTRL);
  if (alt)   Keyboard.press(KEY_LEFT_ALT);
  if (gui)   Keyboard.press(KEY_LEFT_GUI);
  Keyboard.write(chr);
  delay(250);
  Keyboard.releaseAll();
}

void checkSwitch(int swtch, char chr, states *s_ptr) {
  if (buttonState(swtch, s_ptr)) {
    sendToggle(chr, 1, 1, 0, 0);
  }
}


void loop() {

  checkSwitch(AMUTE_SWITCH, AMUTE_CHAR, &amute_state);
  checkSwitch(VMUTE_SWITCH, VMUTE_CHAR, &vmute_state);
  checkSwitch( HAND_SWITCH,  HAND_CHAR, &hand_state);

  delay(250);

}
