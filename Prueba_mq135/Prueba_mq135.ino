int CO2;
int mq135 = A0;

void setup() {
  Serial.begin(9600);

}

void loop() {
  CO2 = analogRead(mq135); //Lemos la salida analógica del MQ
  float voltaje = CO2 * (5.0 / 1023.0); //Convertimos la lectura en un valor de voltaje
  Serial.print("adc:");
  Serial.print(CO2);
  Serial.print("    voltaje:");
  Serial.println(voltaje);
  delay(100);
}
