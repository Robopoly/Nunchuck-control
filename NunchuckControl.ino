/***************************************************************************************
 * Copyright (c) 2016, Robopoly Development Team
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 ******************************************************************************
 
 *
 * Title:       Nunchuck control
 * File:        NunchuckControl.ino
 * Date:        2016-06-01
 * Author:      Karl Kangur, Patrick Bobbink
 * Website:     https://github.com/Robopoly/Nunchuck-control
 *
 * Description: Uses the wireless nunchuck joystick to control the robot.
 *
 ***************************************************************************************/

#include <prismino.h>
#include <Wire.h>

uint8_t outbuf[6];
int cnt = 0;
int joy_x_axis, joy_y_axis, dirY, dirX, speedLeft, speedRight;
//uint16_t accX, accY, accZ;
//uint8_t button_c, button_z;

void setup()
{
  Serial.begin(19200);
  pinMode(LED, OUTPUT);
  Wire.begin();
  nunchuckInit();
  
  play(440, 300);
  play(880, 300);
}

void nunchuckInit()
{
  // transmit to device 0x52
  Wire.beginTransmission(0x52);
  // sends memory address
  Wire.write(0x40);
  // sends sent a zero
  Wire.write(0x00);
  // stop transmitting
  Wire.endTransmission();
}

void sendZero()
{
  // transmit to device 0x52
  Wire.beginTransmission(0x52);
  // sends one byte
  Wire.write(0x00);
  // stop transmitting
  Wire.endTransmission();
}

void loop()
{
  // request data from nunchuck
  Wire.requestFrom(0x52, 6);
  while(Wire.available())
  {
    // receive byte as an integer
    outbuf[cnt++] = Wire.read();
    digitalWrite(LED, HIGH);
  }
  
  if(cnt >= 5)
  {
    // get button states
    /*button_c = 0;
    button_z = 0;
    
    if((outbuf[5] >> 0) & 1)
    {
      button_z = 1;
    }
    
    if((outbuf[5] >> 1) & 1)
    {
      button_c = 1;
    }*/
    
    // accelerometer data over 10 bits
    /*accX = (outbuf[2] << 2) | ((outbuf[6] >> 2) & 0b11);
    accY = (outbuf[3] << 2) | ((outbuf[6] >> 4) & 0b11);
    accZ = (outbuf[4] << 2) | ((outbuf[6] >> 6) & 0b11);*/
    
    // only send new commands if they change
    if(joy_x_axis != outbuf[0] || joy_y_axis != outbuf[1])
    {
      joy_x_axis = outbuf[0];
      joy_y_axis = outbuf[1];
    
      // map joystick range to -100 to 100 range
      dirY = map(joy_y_axis, 0, 255, -100, 100);
      dirX = map(joy_x_axis, 0, 255, -100, 100);
      
      speedLeft = dirY + dirX;
      speedRight = dirY - dirX;
      
      // scale if needed
      if(speedLeft > 100 || speedLeft < -100 || speedRight > 100 || speedRight < -100)
      {
        // get speed norm, casing needed because of overflow
        uint16_t norm = sqrt((long) speedLeft * speedLeft + (long) speedRight * speedRight); // This one should be declared somewhere else. 99% chance it'll be used more than once. Better lose 2 octets than allocating every time
        speedLeft = (long) speedLeft * 100 / norm;
        speedRight = (long) speedRight * 100 / norm;
      }
      
      setSpeed(speedLeft, -speedRight); //  reason for - ?
    }
  }
  else
  {
    digitalWrite(LED, LOW);
  }

  cnt = 0;
  // send the request for next bytes
  sendZero();
  delay(200);
}

