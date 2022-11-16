/*
 * This file is part of the tiny-osean project, https://github.com/gluonsandquarks/tiny-osean
 * main.ino - main entry point describing the functionality of the board.
 * This file contains the main logic of the board.
 * 
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 gluons.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <avr/sleep.h>            // Power saving library

/* 
 * Pins PB1 and PB0 on the ATTiny1616 are PWM, so analogWrite() should work on them
 * More about how the megaTinyCore lib maps the arduino pins: https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/ATtiny_x16.md
 * More about the schematic/pinout of the tiny-osean board: https://github.com/gluonsandquarks/tiny-osean/blob/main/imgs/schematic.PNG
 */
#define LEFT_EYE      PIN_PB1     // PB1
#define RIGHT_EYE     PIN_PB0     // PB0
#define BUTTON        PIN_PC2     // PC2

enum State
{
  ON = 0,
  BREATH_LO,
  BREATH_MID,
  BREATH_HI,
  ALT_LO,
  ALT_MID,
  ALT_HI,
  RUSH,
  RAND_DELAY,
  SOS,
  SMOOTH,
  SLEEP,
  MAX_STATE
};

/* Global variables */
State stat = ON;

/* Forward declarations */
void on();
void breathing(int ms);
void alternating(int ms);
void rush(int ms);
void rand_delay();
void sos();
void smooth();


/* Interrupt Service Routines (ISRs) */
void on_press();
void go_to_sleep();


void on()
{
  digitalWrite(RIGHT_EYE, LOW);
  digitalWrite(LEFT_EYE, LOW);
}

/* Function definitions for the different light shows */

/*
 * @brief Light show that smoothly dims and lights the LEDs through PWM.
 * 
 * @param ms Delay in milliseconds
 * @return Void
 */
void breathing(int ms)
{
  State temp = stat;
  for (int i = 0; i < 256; i++)
  {
    if (temp != stat) { break; }
    analogWrite(RIGHT_EYE, i);
    analogWrite(LEFT_EYE, i);
    delay(ms);
  }
  for (int i = 0; i < 256; i++)
  {
    if (temp != stat) { break; }
    analogWrite(RIGHT_EYE, 255 - i);
    analogWrite(LEFT_EYE, 255 - i);
    delay(ms);
  }
}

/*
 * @brief Light show that alternates both LEDs on/off.
 * 
 * @param ms Delay in milliseconds
 * @return Void
 */
void alternating(int ms)
{
  digitalWrite(RIGHT_EYE, HIGH);
  digitalWrite(LEFT_EYE, LOW);
  delay(ms);
  digitalWrite(RIGHT_EYE, LOW);
  digitalWrite(LEFT_EYE, HIGH);
  delay(ms);
}

/*
 * @brief Light show that alternates both LEDs on/off from dim to bright.
 * 
 * @param ms Delay in milliseconds
 * @return Void
 */
void rush(int ms)
{
  State temp = stat;
  for (float i = 0; i < 128.0F; i += 2.5F)
  {
    if (i >= 64.0F) { i += 3.0F; }
    if (temp != stat) { break; }
    analogWrite(RIGHT_EYE, 255 - i);
    analogWrite(LEFT_EYE, 255);
    delay(ms);
    if (temp != stat) { break; }
    analogWrite(RIGHT_EYE, 255);
    analogWrite(LEFT_EYE, 255 - i);
    delay(ms);
  }
  
  for (int i = 0; i < 5; i++)
  {
    if (temp != stat) { break; }
    digitalWrite(RIGHT_EYE, LOW);
    digitalWrite(LEFT_EYE, HIGH);
    delay(ms * 2);
    if (temp != stat) { break; }
    digitalWrite(RIGHT_EYE, HIGH);
    digitalWrite(LEFT_EYE, LOW);
    delay(ms * 2);
  }

  digitalWrite(RIGHT_EYE, LOW);
  digitalWrite(LEFT_EYE, LOW);
}

/*
 * @brief Light show that lights each LED at random intervals.
 * 
 * @return Void
 */
void rand_delay()
{
  unsigned long timer_1 = 570;
  unsigned long last_timer_1 = 0;
  unsigned long timer_2 = 350;
  unsigned long last_timer_2 = 0;
  State temp = stat;
  while (temp == stat)
  {
    if (millis() - last_timer_1 >= timer_1)
    {
      float multiplier = random(6.0F);
      for (float i = 0; i < 256 * multiplier; i++)
      {
        if (temp != stat) { break ;}
        analogWrite(RIGHT_EYE, (i / multiplier));
      }
      last_timer_1 = millis() * random(500);
    }

    if (millis() - last_timer_2 >= timer_2)
    {
      float multiplier = random(6.0F);
      for (float i = 0; i < 256 * multiplier; i++)
      {
        if (temp != stat) { break; }
        analogWrite(LEFT_EYE, (i / multiplier));
      }
      last_timer_2 = millis() * random(500);
    }
  }
}

/*
 * @brief Light show that emits SOS in morse code.
 * 
 * @param ms Delay in milliseconds
 * @return Void
 */
void sos(int ms)
{
  State temp = stat;
  unsigned long timer = 400;
  unsigned long last_timer = 0;
  for (int i = 0; i < 3; i++)
  {
    if (temp != stat) { break; }
    digitalWrite(RIGHT_EYE, LOW);
    digitalWrite(LEFT_EYE, LOW);
    delay(ms);
    digitalWrite(RIGHT_EYE, HIGH);
    digitalWrite(LEFT_EYE, HIGH);
    delay(ms);
  }

  for (int i = 0; i < 3; i++)
  {
    if (temp != stat) { break; }
    digitalWrite(RIGHT_EYE, LOW);
    digitalWrite(LEFT_EYE, LOW);
    delay(ms * 2);
    digitalWrite(RIGHT_EYE, HIGH);
    digitalWrite(LEFT_EYE, HIGH);
    delay(ms * 2);
  }

  for (int i = 0; i < 3; i++)
  {
    if (temp != stat) { break; }
    digitalWrite(RIGHT_EYE, LOW);
    digitalWrite(LEFT_EYE, LOW);
    delay(ms);
    digitalWrite(RIGHT_EYE, HIGH);
    digitalWrite(LEFT_EYE, HIGH);
    delay(ms);
  }
  last_timer = millis();
  while (temp == stat)
  {
    if (millis() - last_timer >= timer)
    {
      last_timer = millis();
      break;
    }
  }
}

/*
 * @brief Light show that transitions smoothly between both LEDs on board.
 * 
 * @param ms Delay in milliseconds
 * @return Void
 */
void smooth(int ms)
{
  State temp = stat;
  int i = 0;
  int j = 0;

  for (i; i < 256; i++)
  {
    if (temp != stat) { break; }
    if (i > 32) { i += 30; }
    j = i - 8;
    if (j < 0) { j = 0; }
    analogWrite(RIGHT_EYE, 255 - i);
    analogWrite(LEFT_EYE, 255 - j);
    delay(ms);
  }

  i = 255;
  j = 255;
  for (i; i > 0; i--)
  {
    if (temp != stat) { break; }
    if (i > 32) { i -= 30; }
    j = i + 8;
    if (j > 255) { j = 255; }
    analogWrite(RIGHT_EYE, 255 - i);
    analogWrite(LEFT_EYE, 255 - j);
    delay(ms);
  }
  i = 0;
  analogWrite(RIGHT_EYE, 255 - i);
  j = 8;
  for (j; j >= 0; j--)
  {
    if (temp != stat) { break; }
    analogWrite(LEFT_EYE, 255 - j);
    delay(ms);
  }
}

/*
 * @brief ISR called when a button is pressed.
 * 
 * @return Void
 */
void on_press()
{
  stat = State(stat + 1);
  if (stat >= MAX_STATE) { stat = ON; }
}

/*
 * @brief Last mode of the device. Shuts down LEDs and goes to sleep.
 * 
 * @return Void
 */
void go_to_sleep()
{
  digitalWrite(RIGHT_EYE, HIGH);
  digitalWrite(LEFT_EYE, HIGH);
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  // TODO: research these parameters for the ATTiny1616. For some reason
  // these guys aren't defined for this MCU. Look into the avr/sleep.h implementation.
  //  byte adcsra = ADCSRA;                             // save ADCSRA
  //  ADCSRA &= ~_BV(ADEN);                             // disable ADC
  cli();                                                 // stop interrupts to ensure the BOD timed sequence executes as required
  //  byte mcucr1 = MCUCR | _BV(BODS) | _BV(BODSE);     //turn off the brown-out detector
  //  byte mcucr2 = mcucr1 & ~_BV(BODSE);
  //  MCUCR = mcucr1;
  //  MCUCR = mcucr2;
  sei();                                                // ensure interrupts enabled so we can wake up again
  sleep_cpu();                                          // go to sleep
  sleep_disable();                                      // wake up here
  //  ADCSRA = adcsra;                                  // restore ADCSRA
}

void setup()
{
  /* Initialize peripherals */
  pinMode(RIGHT_EYE, OUTPUT);
  pinMode(LEFT_EYE, OUTPUT);
  pinMode(BUTTON, INPUT);
  /* Disable floating pins */
  pinMode(PIN_PA1 ,INPUT_PULLUP);
  pinMode(PIN_PA2 ,INPUT_PULLUP);
  pinMode(PIN_PA3 ,INPUT_PULLUP);
  pinMode(PIN_PA4 ,INPUT_PULLUP);
  pinMode(PIN_PA5 ,INPUT_PULLUP);
  pinMode(PIN_PA6 ,INPUT_PULLUP);
  pinMode(PIN_PA7 ,INPUT_PULLUP);
  pinMode(PIN_PB2 ,INPUT_PULLUP);
  pinMode(PIN_PB3 ,INPUT_PULLUP);
  pinMode(PIN_PB4 ,INPUT_PULLUP);
  pinMode(PIN_PB5 ,INPUT_PULLUP);
  pinMode(PIN_PC0 ,INPUT_PULLUP);
  pinMode(PIN_PC1 ,INPUT_PULLUP);
  pinMode(PIN_PC3 ,INPUT_PULLUP);
  
  attachInterrupt(digitalPinToInterrupt(BUTTON), on_press, RISING);
}

void loop()
{
  switch(stat)
  {
    case ON:
      on();
      break;
    case BREATH_LO:
      breathing(5);
      break;
    case BREATH_MID:
      breathing(2);
      break;
    case BREATH_HI:
      breathing(0);
      break;
    case ALT_LO:
      alternating(100);
      break;
    case ALT_MID:
      alternating(50);
      break;
    case ALT_HI:
      alternating(25);
      break;
    case RUSH:
      rush(50);
      break;
    case RAND_DELAY:
      rand_delay();
      break;
    case SOS:
      sos(100);
      break;
    case SMOOTH:
      smooth(50);
      break;
    case SLEEP:
      go_to_sleep();
      break;
    default:
      break;
  }
}
