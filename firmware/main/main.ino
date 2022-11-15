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
  BREATH_5,
  BREATH_2,
  BREATH_0,
  ALT_100,
  ALT_50,
  ALT_25,
  DELAY,
  MAX_STATE
};

/* Global variables */
State stat = ON;

/* Forward declarations */
void breathing(int ms);
void alternating(int ms);
void on();
void delay_fun();


/* Interrupt Service Routines (ISRs) */
void onPress();
void wakeUp();

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

void on()
{
  digitalWrite(RIGHT_EYE, LOW);
  digitalWrite(LEFT_EYE, LOW);
}

void delay_fun()
{
  unsigned long timer_1 = 500;
  unsigned long last_timer_1;
  unsigned long timer_2 = 300;
  unsigned long last_timer_2;
  State temp = stat;
  while (temp == stat)
  {
    if (millis() - last_timer_1 >= timer_1)
    {
      for (float i = 0; i < 256 * 5; i++)
      {
        analogWrite(RIGHT_EYE, (i / 5.0F));
      }
      last_timer_1 = millis();
    }

    if (millis() - last_timer_2 >= timer_2)
    {
      for (float i = 0; i < 256 * 3; i++)
      {
        analogWrite(LEFT_EYE, (i / 3.0F));
      }
      last_timer_2 = millis();
    }
  }
}

void onPress()
{
  stat = State(stat + 1);
  if (stat >= MAX_STATE) { stat = ON; }
}

void setup()
{
  /* Initialize peripherals */
  pinMode(RIGHT_EYE, OUTPUT);
  pinMode(LEFT_EYE, OUTPUT);
  pinMode(BUTTON, INPUT);

  attachInterrupt(digitalPinToInterrupt(BUTTON), onPress, RISING);
}

void loop()
{
  switch(stat)
  {
    case ON:
      on();
      break;
    case BREATH_5:
      breathing(5);
      break;
    case BREATH_2:
      breathing(2);
      break;
    case BREATH_0:
      breathing(0);
      break;
    case ALT_100:
      alternating(100);
      break;
    case ALT_50:
      alternating(50);
      break;
    case ALT_25:
      alternating(25);
      break;
    case DELAY:
      delay_fun();
      break;
    default:
      break;
  }
}
