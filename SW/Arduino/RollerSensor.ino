
const byte sensor = 2;
int sensor_count = 0;
long lst_trig = 0;
int lst_update = 0;

//telemetry values
float speed_val = 0;
float cadence_val = 0;
float power_val = 0;

void setup() {

  Serial.begin(9600);
  pinMode(sensor, INPUT);
  attachInterrupt(digitalPinToInterrupt(sensor), opt_sw, RISING);
  delay(1000);
}




//main loop for packet generation
void loop() {
  delay(100);
  update_telem();
  Serial.print("Power value: ");
  Serial.println(power_val);

}

//ISR
void opt_sw(){
  long this_trig = millis();
  if (this_trig - lst_trig>250){ //debounce
    sensor_count++;              //increment the counter
  }
  lst_trig = this_trig;
}




///telemetry value update
void update_telem(){
   float drum_rpm = 60*sensor_count * 1000 / (millis() - lst_update);
   float wheel_rpm = drum_rpm * 6.7;
   speed_val = wheel_rpm*2.1/60;
   cadence_val = wheel_rpm*15/49;
   power_val = cadence_val*cadence_val*0.0440501;
   sensor_count = 0;
   lst_update = millis();
}





