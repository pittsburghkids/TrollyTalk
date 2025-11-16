#include <Bounce2.h>
#include "pio_encoder.h"

#include "libraries/WAV-Trigger-Arduino-Serial-Library/wavtrigger.h"
#include "libraries/WAV-Trigger-Arduino-Serial-Library/wavtrigger.cpp"

#define OPTION_COUNT 10

#define ENCODER_A_PIN 13
#define ENCODER_B_PIN 12
#define ENCODER_Z_PIN 11

#define BUTTON_PIN 5
#define LED_PIN 6

#define RESOLUTION 4000

PioEncoder encoder(ENCODER_B_PIN);
Bounce2::Button button = Bounce2::Button();
wavTrigger wavTriggerControl;

byte selection = 0;
byte lastSelection = 0;

int count;
int lastCount;

void encoderZInterruptHandler() {
  encoder.reset();
}

void setup() {
  Serial.begin(115200);

  // Lignt setup.
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // Encoder setup.
  {
    encoder.begin();
    pinMode(ENCODER_Z_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(ENCODER_Z_PIN), encoderZInterruptHandler, FALLING);
  }

  // Debounce setup.
  button.attach(BUTTON_PIN, INPUT_PULLUP);
  button.setPressedState(LOW); 

  // Wav Trigger.
  {
    // Use alternate serial pins.
    Serial1.setRX(17u);
    Serial1.setTX(16u);

    // Wait for board reset.
    delay(100);

    // Initialize board.
    wavTriggerControl.start();

    // Wait for board.
    delay(100);

    // Reset board.
    wavTriggerControl.stopAllTracks();
    wavTriggerControl.samplerateOffset(0);
    wavTriggerControl.setReporting(true);
    
  }
}

void loop() {

  wavTriggerControl.update();

  // Encoder handling.
  {
    count = encoder.getCount();
    //if (count != lastCount) Serial.println(count);
    lastCount = count;
    if (count < 0) count = RESOLUTION + count;

    selection = (byte)((float)count / (RESOLUTION + 1) * OPTION_COUNT);

    if (selection != lastSelection) {
      Serial.println(selection);
      lastSelection = selection;
    }
  }

  // Button handling.
  {
    button.update();

    if (button.pressed()) {
      Serial.println(selection);
      wavTriggerControl.trackStop(selection + 1);
      wavTriggerControl.trackPlayPoly(selection + 1);
    }

    if (button.isPressed()) {
      digitalWrite(LED_PIN, HIGH);
    } else {
      digitalWrite(LED_PIN, LOW);
    }
  }
}
