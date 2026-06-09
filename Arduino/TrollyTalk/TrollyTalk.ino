#include <Bounce2.h>
#include "pio_encoder.h"

#define OPTION_COUNT 8

#define ENCODER_A_PIN D27
#define ENCODER_B_PIN D26
#define ENCODER_Z_PIN D22

#define BUTTON_PIN D13
#define LED_PIN LED_BUILTIN

#define LIGHT_EXTERIOR_PIN D15
#define LIGHT_INTERIOR_PIN D14

#define MAX_BRIGHTNESS 64
#define FADE_DURATION 500.0

#define RESOLUTION 4000

PioEncoder encoder(ENCODER_B_PIN);
Bounce2::Button button = Bounce2::Button();

byte selection = 0;
byte lastSelection = 0;

int count;
int lastCount;

float pressTime = millis();

void encoderZInterruptHandler()
{
  encoder.reset();
}

void setup()
{
  Serial.begin(115200);

  // Lignt setup.
  {
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    pinMode(LIGHT_INTERIOR_PIN, OUTPUT);
    analogWrite(LIGHT_INTERIOR_PIN, 0);

    pinMode(LIGHT_EXTERIOR_PIN, OUTPUT);
    analogWrite(LIGHT_EXTERIOR_PIN, 0);
  }

  // Encoder setup.
  {
    pinMode(ENCODER_A_PIN, INPUT_PULLUP);
    pinMode(ENCODER_B_PIN, INPUT_PULLUP);
    pinMode(ENCODER_Z_PIN, INPUT_PULLUP);

    encoder.begin();
    attachInterrupt(digitalPinToInterrupt(ENCODER_Z_PIN), encoderZInterruptHandler, FALLING);
  }

  // Debounce setup.
  button.attach(BUTTON_PIN, INPUT_PULLUP);
  button.setPressedState(LOW);

  // Wav Trigger.
  {
    Serial1.begin(57600);
  }
}

void loop()
{

  // Encoder handling.
  {
    count = encoder.getCount();

    lastCount = count;
    if (count < 0)
      count = RESOLUTION + count;

    selection = (byte)((float)count / (RESOLUTION + 1) * OPTION_COUNT);

    int isEven = selection % 2;
    digitalWrite(LED_PIN, isEven ? HIGH : LOW);

    if (selection != lastSelection)
    {
      Serial.println(selection);
      lastSelection = selection;
    }
  }

  // Button handling.
  {
    button.update();

    if (button.pressed())
    {
      Serial1.print("stop ");
      Serial1.print(selection + 1);
      Serial1.print('\r');
      Serial1.flush();
      delay(100);
      Serial1.print("play ");
      Serial1.print(selection + 1);
      Serial1.print('\r');
      Serial1.flush();
    }

    if (button.isPressed())
    {
      pressTime = millis();
    }
  }

  if (millis() - pressTime < FADE_DURATION)
  {
    float t = (millis() - pressTime) / FADE_DURATION;
    int brightness = (int)((1 - t) * MAX_BRIGHTNESS);
    analogWrite(LIGHT_EXTERIOR_PIN, brightness);
    analogWrite(LIGHT_INTERIOR_PIN, brightness);
  }
}
