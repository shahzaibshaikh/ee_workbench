String mode = "";

//Pins for VoltMeter
const int VoltMeter = A0; 
float V = 0.00;

//Pins for Ammeter
const int Ammeter = A1;
float A = 0.00;

//Pins for OhmMeter
#define CH0  12
#define CH1  11
#define CH2  10
#define CH3  9
#define CH4  8

byte ch_number;
uint32_t res;
const uint32_t res_table[5] = {100, 1000, 10000, 100000, 1000000};
char _buffer[11];

//Pins for Function Generator
const int funcGen = 13;
int frequency = 1500;

//Pins for Oscilloscope
const int Scope = A4;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);  
  pinMode(VoltMeter, INPUT);
  pinMode(Ammeter, INPUT);
  pinMode(funcGen, OUTPUT);
  pinMode(Scope, INPUT);

  pinMode(CH0, OUTPUT);
  pinMode(CH1, OUTPUT);
  pinMode(CH2, OUTPUT);
  pinMode(CH3, OUTPUT);
  pinMode(CH4, OUTPUT);
 
  ch_number = 4;
  ch_select(ch_number);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available()){
    String c = Serial.readString();

    if (isValidNumber(c) == true){
      //Serial.println(c);
      frequency = c.toInt();
    }
    
    if (c == "voltmeter"){
      mode = "voltmeter";
    }
    
    if (c == "ammeter"){
      mode = "ammeter";
    }

    if (c == "funcgen"){
      mode = "funcgen";
    }

    if (c == "ohmmeter"){
      mode = "ohmmeter";
    }

    if (c == "oscilloscope"){
      mode = "oscilloscope";
    }
  }

  if (mode == "voltmeter"){
    calculate_voltage();
  }

  if (mode == "ohmmeter"){
    calculate_resistor();
  }

  if (mode == "funcgen"){
    tone(funcGen, frequency);
    Serial.println(frequency);
  }

  if (mode == "ammeter"){
    calculate_current();
  }

  if (mode == "oscilloscope"){
    oscilloscope();
  }
}

void oscilloscope (){
  int adcValue = analogRead(Scope);
  float V = fmap(adcValue,139 , 639, -10, 10);
  Serial.println(V);
}

void calculate_current() {
  float R = 10;
  float adc_value = 0.00;
  float v_ref = 5;
  float voltage_read = 0.00;
  
  for (int i = 0; i < 10 ; i++) 
  { 
    adc_value = adc_value + analogRead(Ammeter);
    delay(3); 
  }

  adc_value = adc_value / 10; 
  voltage_read = ((adc_value * v_ref) / 1024);
  A = (voltage_read / R);

  String temp1 = "";
  String temp = " A";
  temp1.concat(A);
  temp1.concat(temp);
  Serial.println(temp1);
}

void calculate_voltage() { 
  float R1 = 22000.00; 
  float R2 = 110.00; 
  float v_ref = 4.975; 
  float resistor_ratio = 0.00;
  float adc_value = 0.00; 
  float voltage = 0.00; 
  resistor_ratio = (R2 / (R1 + R2)); 
  
  for (int i = 0; i < 10 ; i++) 
  { 
    adc_value = adc_value + analogRead(VoltMeter);
    delay(3); 
  } 
  adc_value = adc_value / 10; 
  voltage = ((adc_value * v_ref) / 1024); 
  V = voltage / resistor_ratio; 
 
  //Serial.print("DC Voltage = ");
  String temp1 = "";
  String temp = " V";
  temp1.concat(V);
  temp1.concat(temp);
  Serial.println(temp1);

}

void calculate_resistor() { 
  uint16_t volt_image = analogRead(A3) + 1;
 
  if(volt_image >= 550 && ch_number < 4) {
    ch_number++;
    ch_select(ch_number);
    delay(50);
    return;
  }
 
  if(volt_image <= 90 && ch_number > 0) {
    ch_number--;
    ch_select(ch_number);
    delay(50);
    return;
  }
 
  if(volt_image < 900) {
    float value = (float)volt_image*res/(1023 - volt_image);
    if(value < 1000.0)
      sprintf(_buffer, "%03u.%1u Ohm ", (uint16_t)value, (uint16_t)(value*10)%10);
    else if(value < 10000.0)
           sprintf(_buffer, "%1u.%03u kOhm", (uint16_t)(value/1000), (uint16_t)value%1000);
    else if(value < 100000.0)
           sprintf(_buffer, "%02u.%02u kOhm", (uint16_t)(value/1000), (uint16_t)(value/10)%100);
    else if(value < 1000000.0)
           sprintf(_buffer, "%03u.%1u kOhm", (uint16_t)(value/1000), (uint16_t)(value/100)%10);
    else
      sprintf(_buffer, "%1u.%03u MOhm", (uint16_t)(value/1000000), (uint16_t)(value/1000)%1000);
  }
 
  else
    sprintf(_buffer, "Over Load ");

  Serial.println(_buffer);
  Serial.println();
}

void ch_select(byte n) {
  switch(n) {
    case 0:
      digitalWrite(CH0, LOW);
      digitalWrite(CH1, HIGH);
      digitalWrite(CH2, HIGH);
      digitalWrite(CH3, HIGH);
      digitalWrite(CH4, HIGH);
      break;
    case 1:
      digitalWrite(CH0, HIGH);
      digitalWrite(CH1, LOW);
      digitalWrite(CH2, HIGH);
      digitalWrite(CH3, HIGH);
      digitalWrite(CH4, HIGH);
      break;
    case 2:
      digitalWrite(CH0, HIGH);
      digitalWrite(CH1, HIGH);
      digitalWrite(CH2, LOW);
      digitalWrite(CH3, HIGH);
      digitalWrite(CH4, HIGH);
      break;
    case 3:
      digitalWrite(CH0, HIGH);
      digitalWrite(CH1, HIGH);
      digitalWrite(CH2, HIGH);
      digitalWrite(CH3, LOW);
      digitalWrite(CH4, HIGH);
      break;
    case 4:
      digitalWrite(CH0, HIGH);
      digitalWrite(CH1, HIGH);
      digitalWrite(CH2, HIGH);
      digitalWrite(CH3, HIGH);
      digitalWrite(CH4, LOW);
  }
  res = res_table[n];
}

boolean isValidNumber(String str){
  for(byte i=0;i<str.length();i++)
  {
     if(isDigit(str.charAt(i))) return true;
       }
  return false;
} 

float fmap(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
