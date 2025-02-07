#include "MIDIUSB.h"
#define POT1 A0
#define POT2 A1
#define POT3 A9
#define POT4 A3
#define POT5 A8
#define NUM_POTS 5

int potPins[5] = {A0, A1, A9, A3, A8};

int potValues[5];

struct rotEncoder{
  uint8_t state = 0;
  int16_t inputDelta = 0;
  bool printFlag = false;
  uint8_t CLK_PIN;
  uint8_t DT_PIN;
};

rotEncoder encList[2];

void setup() {
  pinMode(POT1, INPUT);
  pinMode(POT2, INPUT);
  pinMode(POT3, INPUT);
  pinMode(POT4, INPUT);
  pinMode(POT5, INPUT);

  encList[0].CLK_PIN = 6;
  encList[0].DT_PIN = 7;

  encList[1].CLK_PIN = 4;
  encList[1].DT_PIN = 5;

  for(int i = 0; i < 2; i++){
    pinMode(encList[i].CLK_PIN, INPUT_PULLUP);
    pinMode(encList[i].DT_PIN, INPUT_PULLUP);
  }

}

void loop() {
  checkPotUpdate();
  for(int i = 0; i < 2; i++){
    readEncoder(i, encList[i].CLK_PIN, encList[i].DT_PIN);
    sendEncoder(i);
  }
}

void readEncoder(int encoderNum, uint8_t pinA, uint8_t pinB) {
    bool CLKstate = digitalRead(pinA);
    bool DTstate = digitalRead(pinB);
    switch (encList[encoderNum].state) {
        case 0:                         // Idle state, encoder not turning
            if (!CLKstate){             // Turn clockwise and CLK goes low first
                encList[encoderNum].state = 1;
            } else if (!DTstate) {      // Turn anticlockwise and DT goes low first
                encList[encoderNum].state = 4;
            }
            break;
        // Clockwise rotation
        case 1:                     
            if (!DTstate) {             // Continue clockwise and DT will go low after CLK
                encList[encoderNum].state = 2;
            } 
            break;
        case 2:
            if (CLKstate) {             // Turn further and CLK will go high first
                encList[encoderNum].state = 3;
            }
            break;
        case 3:
            if (CLKstate && DTstate) {  // Both CLK and DT now high as the encoder completes one step clockwise
                encList[encoderNum].state = 0;
                if(encList[encoderNum].inputDelta > 0){
                  --encList[encoderNum].inputDelta;
                }
                encList[encoderNum].printFlag = true;
            }
            break;
        // Anticlockwise rotation
        case 4:                         // As for clockwise but with CLK and DT reversed
            if (!CLKstate) {
                encList[encoderNum].state = 5;
            }
            break;
        case 5:
            if (DTstate) {
                encList[encoderNum].state = 6;
            }
            break;
        case 6:
            if (CLKstate && DTstate) {
                encList[encoderNum].state = 0;
                if(encList[encoderNum].inputDelta < 24){
                  ++encList[encoderNum].inputDelta;
                }
                encList[encoderNum].printFlag = true;
            }
            break; 
    }
}

void sendEncoder(int index) {
    if (encList[index].printFlag) {
        encList[index].printFlag = false;
        midiEventPacket_t sendMidi = {0x0B, 0xB0, 20 + NUM_POTS + index, encList[index].inputDelta};
        MidiUSB.sendMIDI(sendMidi);
        MidiUSB.flush();
    }
}
  
void checkPotUpdate(){
  for(int i = 0; i < NUM_POTS; i++){
    int newPot = map(analogRead(potPins[i]), 0, 1023, 0, 127);
    if(abs(newPot - potValues[i]) > 0){
      potValues[i] = newPot;
      midiEventPacket_t sendMidi = {0x0B, 0xB0, 20 + i, potValues[i]};
      MidiUSB.sendMIDI(sendMidi);
      MidiUSB.flush();
    }
  }
}

