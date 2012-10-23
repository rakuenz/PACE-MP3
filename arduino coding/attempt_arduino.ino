/*
 * NunchuckPrint
 *
 * 2007 Tod E. Kurt, http://todbot.com/blog/
 *
 * The Wii Nunchuck reading code is taken from Windmeadow Labs
 *   http://www.windmeadow.com/node/42
 */
 
#include <Wire.h>

void setup()
{
  Serial.begin(19200);
  nunchuck_setpowerpins(); // use analog pins 2&3 as fake gnd & pwr
  nunchuck_init(); // send the initilization handshake
  Serial.print ("Pacer Activated\n");
}

void loop()
{
  nunchuck_get_data();
  nunchuck_print_data();
  delay(100);
}


//
// Nunchuck functions
//

static uint8_t nunchuck_buf[6];   // array to store nunchuck data,

// Uses port C (analog in) pins as power & ground for Nunchuck
static void nunchuck_setpowerpins()
{
#define pwrpin PORTC3
#define gndpin PORTC2
    DDRC |= _BV(pwrpin) | _BV(gndpin);
    PORTC &=~ _BV(gndpin);
    PORTC |=  _BV(pwrpin);
    delay(100);  // wait for things to stabilize        
}

// initialize the I2C system, join the I2C bus,
// and tell the nunchuck we're talking to it
void nunchuck_init()
{ 
  Wire.begin();	                // join i2c bus as master
  Wire.beginTransmission(0x52);	// transmit to device 0x52
  Wire.write(0x40);		// sends memory address
  Wire.write(0x00);		// sends sent a zero.  
  Wire.endTransmission();	// stop transmitting
}

// Send a request for data to the nunchuck
// was "send_zero()"
void nunchuck_send_request()
{
  Wire.beginTransmission(0x52);	// transmit to device 0x52
  Wire.write(0x00);		// sends one byte
  Wire.endTransmission();	// stop transmitting
}

// Receive data back from the nunchuck, 
int nunchuck_get_data()
{
    int cnt=0;
    Wire.requestFrom (0x52, 6);	// request data from nunchuck
    while (Wire.available ()) {
      // receive byte as an integer
      nunchuck_buf[cnt] = nunchuk_decode_byte(Wire.read());
      cnt++;
    }
    nunchuck_send_request();  // send request for next data payload
    // If we recieved the 6 bytes, then go print them
    if (cnt >= 5) {
     return 1;   // success
    }
    return 0; //failure
}

// Print the input data we have recieved
// accel data is 10 bits long
// so we read 8 bits, then we have to add
// on the last 2 bits.  That is why I
// multiply them by 2 * 2
void nunchuck_print_data()
{ 
  static int i=0;
  int joy_x_axis = nunchuck_buf[0];
  int joy_y_axis = nunchuck_buf[1];
  int accel_x_axis = nunchuck_buf[2]; // * 2 * 2; 
  int accel_y_axis = nunchuck_buf[3]; // * 2 * 2;
  int accel_z_axis = nunchuck_buf[4]; // * 2 * 2;

  int z_button = 0;
  int c_button = 0;
  
  int mid_x = 140; 
int mid_y = 160;
  int mid_z = 156;
  
  int diff_x = accel_x_axis - mid_x;
  int diff_y = accel_y_axis - mid_y;
  int diff_z = accel_z_axis - mid_z;
  
  int modulus_x = abs(diff_x);
  int modulus_y = abs(diff_y);
  int modulus_z = abs(diff_z);
  
  int filtered_modulus_x = modulus_x * 0.3;
  int filtered_modulus_y = modulus_y * 0.3;
  int filtered_modulus_z = modulus_z * 0.3;
  
  int value_x;
  int value_y;
  int value_z;
  
  //int sensor_value = (filtered_modulus_y + filtered_modulus_z)/2;
 // int sensor_val;
  
  // byte nunchuck_buf[5] contains bits for z and c buttons
  // it also contains the least significant bits for the accelerometer data
  // so we have to check each bit of byte outbuf[5]
  if ((nunchuck_buf[5] >> 0) & 1) 
    z_button = 1;
  if ((nunchuck_buf[5] >> 1) & 1)
    c_button = 1;

  if ((nunchuck_buf[5] >> 2) & 1) 
    accel_x_axis += 2;
  if ((nunchuck_buf[5] >> 3) & 1)
    accel_x_axis += 1;

  if ((nunchuck_buf[5] >> 4) & 1)
    accel_y_axis += 2;
  if ((nunchuck_buf[5] >> 5) & 1)
    accel_y_axis += 1;

  if ((nunchuck_buf[5] >> 6) & 1)
    accel_z_axis += 2;
  if ((nunchuck_buf[5] >> 7) & 1)
    accel_z_axis += 1;


  //Serial.print(accel_x_axis, DEC);
  //Serial.print(",");
  //Serial.print(accel_y_axis, DEC);
  //Serial.print(",");
  //Serial.print(accel_z_axis, DEC);
  //Serial.print(",");
  //Serial.print(z_button, DEC);
  //Serial.print(",");
  //Serial.print(c_button, DEC);
  //Serial.print(",");
  //Serial.print(modulus_x,DEC);
  //Serial.print(",");
  //Serial.print(modulus_y,DEC);
  //Serial.print(",");
  //Serial.print(modulus_z,DEC);
  //Serial.print(",");
 
  //Serial.print(filtered_modulus_x,DEC);
  //Serial.print(",");
  //Serial.print(filtered_modulus_y,DEC);
  //Serial.print(",");
  //Serial.print(filtered_modulus_z,DEC);
  //Serial.print(',');
  //Serial.print(sensor_value,DEC);
  //Serial.print("\r\n");  // newline
  
  value_x = map(filtered_modulus_x,0,20,0,255);
  value_y = map(filtered_modulus_y,0,20,0,255);
  value_z = map(filtered_modulus_z,0,20,0,255);
  //sensor_val = map(sensor_value,0,40,0,255);
  
  
 //Serial.print(sensor_val);
 
  Serial.print(value_x);
  Serial.print(',');
  Serial.print(value_y);
  Serial.print("\n");  // newline
  
  //Serial.print(value_x);
  //Serial.print(',');
  //Serial.print(value_y);
  //Serial.print(',');
  //Serial.print(value_z);
  //Serial.print('\n');
  //Serial.print("\r\n");  // newline
  i++;
}

// Encode data to format that most wiimote drivers except
// only needed if you use one of the regular wiimote drivers
char nunchuk_decode_byte (char x)
{
  x = (x ^ 0x17) + 0x17;
  return x;
}
