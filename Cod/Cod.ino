#include <DHT.h>
#define DHTTYPE DHT11   // DHT 11 
int pin1 = 22;
int pin2 = 23;
int pin3 = 24;
DHT hum1(pin1, DHTTYPE);
DHT hum2(pin2, DHTTYPE);
DHT hum3(pin3, DHTTYPE);
// Variables para los dato de humedad relativa del aire
float humi1, humi2, humi3;
/*  Range:              0-2500 uMol/m^2s, +-5%
    Excitation Voltage  3.0-5.0VDC
    Sensor Output       0-2.5V
                        Linear: uMol/m^2s = V*1000
*/
float ldr1, ldr2, ldr3; // Variables para sensor de radiación fotosintéticamente activa (PAR, en μmol de fotones m.-2 s-1)
//#include <dht.h>
//const int DHT_RETRY_DELAY = 400;
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
//#define DHTPIN 2
#include "RTClib.h"
#include <Wire.h>
#include <BH1750.h>
RTC_DS1307 RTC;
LiquidCrystal_I2C lcd(0x3F, 16, 2); // No funcionó así. Al parecer es por la librería, no existe el constructor
/* Variables para los sensores de humedad de la tierra */
float HR1 = 0;
float HR2 = 0;
float HR3 = 0;
float temp1 = 0;
float temp2 = 0;
float temp3 = 0;
uint16_t lux1;
uint16_t lux2;
uint16_t lux3;
/* Sensores de luz */
BH1750 lightMeter1;
BH1750 lightMeter2;
BH1750 lightMeter3;
int idinsert = 27; // Número de la consulta en la tabla 'consultas' de la base de datos serviciow
int cabina = 0;
#define SSID "UDLA-WIFI" //-> Nombre De Red
#define PASS "invitado"  //-> Clave De La Red
#define IP "191.102.85.226"
#include <SoftwareSerial.h>
void setup() {
  Serial.begin(115200);
  Serial1.begin(115200);
  hum1.begin();
  hum2.begin();
  hum3.begin();
  //Verificar que el ESP está funcionando
  //Serial.println("Comprobando modulo WiFi");
  Serial1.println("AT");
  delay(10);
  int com = 1;
  while (com) {
    if (Serial1.find('OK')) {
      //Serial.println("Modulo funcionando");
      com = 0;
    } else {
      //Serial.println("Reiniciando modulo");
      Serial1.println("AT+RST");
      delay(100);
      Serial1.println("AT");
    }
  }
  lcd.begin();
  Wire.begin();
  lightMeter1.begin();
  lightMeter2.begin();
  lightMeter3.begin();
  RTC.begin();
  if (!RTC.isrunning()) {
    lcd.println("RTC is not running!");
    delay(10000);
  } else {
    lcd.println("Conectando...");
  }
  RTC.adjust(DateTime(__DATE__, __TIME__));
  while (!ConectarWiFi()) {}
  lcd.clear();
}
void loop() {
Serial.println("Iniciando muestreo");
  mostrarRTCLCD(); //Se manda a llamar la funcion "mostrarRTCLCD" y despues se espera un tienpo de 1seg
  delay(2000);
  lcd.clear();
  LeerHumedadSuelo();
  LeerTemperaturaAire();
  LeerHumedadRelaticaAire();
  LeerLuminosidad();
  LeerSensoresPar();
  enviar_wifi();
}
void mostrarRTCLCD() {
  DateTime now = RTC.now(); // Obtiene datos del modulo RTC (pila de reloj)
  lcd.clear();
  lcd.setCursor(0, 0); // Top left
  if (now.day() < 10) {
    lcd.print("0");
  }
  lcd.print(now.day(), DEC); //Imprime día
  lcd.print('/');
  if (now.month() < 10) {
    lcd.print("0");
  }
  lcd.print(now.month(), DEC); //Imprime mes
  lcd.print('/');
  lcd.print(now.year(), DEC);  //Imprime el año
  lcd.setCursor(0, 1);
  if (now.hour() < 10) {
    lcd.print("0");
  }
  lcd.print(now.hour(), DEC); //Imprime hora
  lcd.print(':');
  if (now.minute() < 10) {
    lcd.print("0");
  }
  lcd.print(now.minute(), DEC); //Imprime minutos
  lcd.print(':');
  if (now.second() < 10) {
    lcd.print("0");
  }
  lcd.print(now.second(), DEC); //Imprime segundos
}
void LeerHumedadSuelo() {
  HR1 = analogRead(A0); // Lee el sensor 1 de humedad del suelo <----
  Serial.println("Hum. suelo 1: " + String(HR1) + "%");
  mostrar_lcd("Hum.Rel.suelo 1 ", " %", HR1);
  HR2 = analogRead(A1); // Lee el sensor 2 de humedad del suelo <----
  Serial.println("Hum. suelo 2: " + String(HR2) + "%");
  mostrar_lcd("Hum.Rel.suelo 2 ", " %", HR2);
  HR3 = analogRead(A2); // Lee el sensor 3 de humedad del suelo <----
  Serial.println("Hum. suelo 3: " + String(HR3) + "%");
  mostrar_lcd("Hum.Rel.suelo 3 ", " %", HR3);
}
void LeerTemperaturaAire() {
  temp1 = analogRead(A3); // Lee el sensor 1 de temperatura <----
  temp1 = (5.0 * temp1 * 100.0) / 1024.0;
  Serial.println("Temp. 1: " + String((temp1 / 2.5)) + " Celsius");
  mostrar_lcd("Temp. 1 ", " Celsius", (temp1 / 2.5));
  temp2 = analogRead(A4); // Lee el sensor 2 de temperatura <----
  temp2 = (5.0 * temp2 * 100.0) / 1024.0;
  Serial.println("Temp. 2: " + String((temp2 / 2.5)) + " Celsius");
  mostrar_lcd("Temp. 2 ", " Celsius", (temp2 / 2.5));
  temp3 = analogRead(A5); // Lee el sensor 3 de temperatura <----
  temp3 = (5.0 * temp3 * 100.0) / 1024.0;
  Serial.println("Temp. 3: " + String((temp3 / 2.5)) + " Celsius");
  mostrar_lcd("Temp. 3 ", " Celsius", (temp3 / 2.5));
}
void LeerHumedadRelaticaAire() {
  humi1 = hum1.readHumidity();
  if (isnan(humi1)) { // Comprueba si son numeros las variables indicadas
    Serial.println();
    Serial.print("Error No :");
    Serial.print(humi1);
    Serial.println();
  } else {
    Serial.println("Humedad relativa del aire 1: " + String(humi1) + "%");
    mostrar_lcd("Hum.R.Aire 1", " %", humi1);
  }
  humi2 = hum2.readHumidity();
  if (isnan(humi2)) { // Comprueba si son numeros las variables indicadas
    Serial.println();
    Serial.print("Error No :");
    Serial.print(humi2);
    Serial.println();
  } else {
    Serial.println("Humedad relativa del aire 2: " + String(humi2) + "%");
    mostrar_lcd("Hum.R.Aire 2", " %", humi2);
  }
  humi3 = hum3.readHumidity();
  if (isnan(humi3)) { // Comprueba si son numeros las variables indicadas
    Serial.println();
    Serial.print("Error No :");
    Serial.print(humi3);
    Serial.println();
  } else {
    Serial.println("Humedad relativa del aire 3: " + String(humi3) + "%");
    mostrar_lcd("Hum.R.Aire 3", " %", humi3);
  }
}
void LeerLuminosidad() {
  lux1 = lightMeter1.readLightLevel(); // Lee el sensor 1 de luminocidad
  Serial.println("Sensor de luminocidad 1: " + String(lux1));
  mostrar_lcd("Luminosidad 1 ", " lux", lux1);
  lux2 = lightMeter2.readLightLevel(); // Lee el sensor 2 de luminocidad
  Serial.println("Sensor de luminocidad 2: " + String(lux2));
  mostrar_lcd("Luminosidad 2 ", " lux", lux2);
  lux3 = lightMeter3.readLightLevel(); // Lee el sensor 3 de luminocidad
  Serial.println("Sensor de luminocidad 3: " + String(lux3));
  mostrar_lcd("Luminosidad 3 ", " lux", lux3);
}
int contador_con = 0;
boolean ConectarWiFi() {                            // metodo para conecta el wifi a la red
  Serial.println("Configurando wifi ");
  Serial1.println("AT+CWMODE=1");
  delay(2000);
  String cmd = "AT+CWJAP=\"";
  cmd += SSID;
  cmd += "\",\"";
  cmd += PASS;
  cmd += "\"";
  Serial1.println(cmd);
  Serial.println(cmd);
  delay(4000);
  contador_con++;
  if (contador_con == 5) {
    Serial.println("Reiniciando WiFi");
    Serial1.println("AT+RST");
    contador_con = 0;
  }
  Serial.println("Intento de conexión:" + String(contador_con));
  if (Serial1.find('OK')) {
    Serial.println("Conectado wifi ");
    return true;
  }
  return false;
}
void enviar_wifi() { // Metodo para enviar los datos a la base de datos.
  lcd.print("Enviando...");
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += IP;
  cmd += "\",80";
  Serial.print("CMD en enviar_wifi(): ");
  Serial.println(cmd);
  Serial1.println(cmd);
  Serial.println(cmd);
  if (Serial1.find("Error")) {
    Serial.println("AT+CIPSTART error");
    digitalWrite(47, HIGH);
    delay(2000);
    analogWrite(47, LOW);
    return;
  }
  String Envio = "GET /Sweb_gral/Service1.svc/Servicio_Grabar/";
  Envio += String(idinsert);
  Envio += ",";
  Envio += String(cabina);
  Envio += "@";
  Envio += String(HR1); // Sensor 1 de humedad de la tierra
  Envio += "@";
  Envio += String(HR2); // Sensor 1 de humedad de la tierra
  Envio += "@";
  Envio += String(HR3); // Sensor 1 de humedad de la tierra
  Envio += "@";
  Envio += String(temp1); // Sensor 1 de temperatura del aire
  Envio += "@";
  Envio += String(temp2); // Sensor 2 de temperatura del aire
  Envio += "@";
  Envio += String(temp3); // Sensor 3 de temperatura del aire
  Envio += "@";
  Envio += String(humi1); // Humedad relativa del aire 1
  Envio += "@";
  Envio += String(humi2); // Humedad relativa del aire 2
  Envio += "@";
  Envio += String(humi3); // Humedad relativa del aire 3
  Envio += "@";
  Envio += String(lux1); // Sensor 1 de luz
  Envio += "@";
  Envio += String(lux2); // Sensor 2 de luz
  Envio += "@";
  Envio += String(lux3); // Sensor 3 de luz
  Envio += "@";
  Envio += String(ldr1); // Sensor PAR 1
  Envio += "@";
  Envio += String(ldr2); // Sensor PAR 2
  Envio += "@";
  Envio += String(ldr3); // Sensor PAR 3
  Envio += "@";
  Envio += "\r\n\r\n";
  cmd = "AT+CIPSEND=";
  cmd += String(Envio.length());
  Serial1.println(cmd);
  Serial.println(cmd);
  if (Serial1.find(">")) {
    Serial1.print(Envio);
    Serial.print(Envio);
    Serial1.println("AT+CIPCLOSE");
  } else {
    Serial1.println("AT+CIPCLOSE");
    Serial.println("AT+CIPCLOSE");
  }
  digitalWrite(47, HIGH);
  delay(500);
  digitalWrite(47, LOW);
  delay(250);
  digitalWrite(47, HIGH);
  delay(250);
  digitalWrite(47, LOW);
  lcd.clear();
}
void LeerSensoresPar() {
  ldr1 = analogRead(A6);
  ldr2 = analogRead(A7);
  ldr3 = analogRead(A8);
  Serial.println("Lectura sensor PAR 1: " + String(ldr1));
  mostrar_lcd("PAR 1 ", "", ldr1);
  Serial.println("Lectura sensor PAR 2: " + String(ldr2));
  mostrar_lcd("PAR 2 ", "", ldr2);
  Serial.println("ectura sensor PAR 3: " + String(ldr3));
  mostrar_lcd("PAR 3 ", "", ldr3);
}
/*
   @parameter titulo: Nombre del dato a mostrar
   @parameter unidad: Unidades del dato a mostrar, por ejemplo: mm, ml, g, cm, m
   @parameter dato: Valor numérico del dato
*/
void mostrar_lcd(String titulo, String unidad, float dato) {
  lcd.print(titulo);
  lcd.setCursor(0, 1); // Bottom left
  lcd.print(dato);
  lcd.print(unidad);
  delay(2000);
  lcd.clear();
}
/*
  Envío: http://191.102.85.226/Sweb_gral/Service1.svc/Servicio_Grabar/27,0@12@12@12@12@12@12@12@12@12
  Result: {"ServicioGuardarResult":"{\"Resultado\":\"OK\",\"Mensaje\":\"OK\"}"}
*/
