
const byte mag_sensor = 2;
int sensor_count = 0;
long lst_trig = 0;
int lst_update = 0;

//telemetry values
float speed_val = 0;
float cadence_val = 0;
float power_val = 0;

void setup() {

  Serial.begin(9600);
  pinMode(mag_sensor, INPUT);
  attachInterrupt(0, mag_sw, RISING);
  delay(1000);
  setup_ant(); //on boot - setup ant dongle
}


//void loop() {
//  delay(1000);
//  Serial.println(sensor_count);
//
//}

//main loop for packet generation
void loop() {
  for (int b=0;b<2;b++){
    for (int a=0;a<2;a++){ 
      send_speed();
      delay(250);
    }
    for (int a=0;a<2;a++){
      send_cad_pow();
      delay(250);
    }
  }
  send_general();
  delay(250);

}

//ISR
void mag_sw(){
  long this_trig = millis();
  if (this_trig - lst_trig>250){
    sensor_count++;                      //increment the counter
  }
  lst_trig = this_trig;
}

///packet formation
void send_speed(){
    Serial.println("Send_speed");
    byte elapse_time = 0x00;
    byte dist_tr = 0x00;
    byte speed_MSB = (byte) (speed_val/256);
    byte speed_LSB = (byte) (speed_val - 256.0*(float)speed_MSB);;
    byte HR = 0x00;
    byte data_ar[] = {0x4E,0x00,0x10,elapse_time,dist_tr,speed_LSB,speed_MSB,HR,0x03};
    send_msg(data_ar,9);
    update_telem();
}

void send_cad_pow(){
    Serial.println("Send_cad_pow");
    byte RPM = (byte) cadence_val;
    byte power_MSB = (byte) (power_val/256);
    byte power_LSB = (byte) (power_val - 256.0*(float)power_MSB);;
    byte data_ar[] = {0x4E,0x00,0x15,0xFF,0xFF,0xFF,RPM,power_LSB,power_MSB,0x03};
    send_msg(data_ar,10);
    update_telem();
}

void send_general(){
    Serial.println("Send_general");
    byte data_ar[] = {0x4E,0x00,0x11,0xFF,0xFF,0x23,0x00,0x44,0x58,0x03};
    send_msg(data_ar,10);
    update_telem();
    
}


void setup_ant() {
  byte data_ar1[] = {0x4A,0x00};
  send_msg(data_ar1,2);
  delay(250);
  byte data_ar2[] = {0x4D,0x00,0x54};
  send_msg(data_ar2,3);
  delay(250);
  byte data_ar3[] = {0x46,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
  send_msg(data_ar3,10);
  delay(250);
  byte data_ar4[] = {0x46,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
  send_msg(data_ar4,10);
  delay(250);
  byte data_ar5[] = {0x46,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
  send_msg(data_ar5,10);
  delay(250);
  byte data_ar6[] = {0x46,0x00,0xB9,0xA5,0x21,0xFB,0xBD,0x72,0xC3,0x45};
  send_msg(data_ar6,10);
  delay(250);
  byte data_ar7[] = {0x42,0x00,0x10,0x00};
  send_msg(data_ar7,4);
  delay(250);
  byte data_ar8[] = {0x51,0x00,0x01,0x02,0x11,0x10};
  send_msg(data_ar8,6);
  delay(250);
  byte data_ar9[] = {0x44,0x00,0xFF};
  send_msg(data_ar9,3);
  delay(250);
  byte data_ar10[] = {0x43,0x00,0x86,0x1F};
  send_msg(data_ar10,4);
  delay(250);
  byte data_ar11[] = {0x45,0x00,0x39};
  send_msg(data_ar11,3);
  delay(250);
  byte data_ar12[] = {0x4B,0x00};
  send_msg(data_ar12,2);
}

///packet transmission
void send_msg(byte bd[],byte bd_size){
  byte pay_size = bd_size-1;
  byte check_val = checksum_calc(pay_size,bd);
  
  Serial.write(0xA4); //send the sync
  Serial.write(pay_size); //send the payload size
  Serial.write(bd,bd_size);//send payload
  Serial.write(check_val); //send checksum
  Serial.flush();//flush 
}

///checksum calculation
byte checksum_calc(byte pay_size, byte bd[]){
  byte cv = 0xA4;
  cv = (cv ^ pay_size) % 0xFF;
  for (int a = 0; a<pay_size+1; a++){
    cv = (cv ^ bd[a]) % 0xFF;
  }

  return  cv;
}

///telemetry value update
void update_telem(){
   float drum_rpm = 60*sensor_count * 1000 / (millis() - lst_update);
   float wheel_rpm = drum_rpm * 6.7;
   speed_val = wheel_rpm*2.1/60;
   cadence_val = wheel_rpm*15/49;
   power_val = cadence_val*cadence_val*0.0440501;
  
   lst_update = millis();
}





