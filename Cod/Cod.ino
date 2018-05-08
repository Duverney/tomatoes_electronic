#include <DHT.h>
#define DHTTYPE DHT11   // DHT 11 
int pin1 = 22;
int pin2 = 23;
int pin3 = 24;
DHT hum1(pin1, DHTTYPE);
DHT hum2(pin2, DHTTYPE);
DHT hum3(pin3, DHTTYPE);
// Variables para los dato de humedad relativa del aire
float humi1;
float humi2;
float humi3;

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
//#define SSID "Vela" //-> Nombre De Red
//#define PASS "vela2018"  //-> Clave De La Red
#define SSID "UDLA-WIFI" //-> Nombre De Red
#define PASS "invitado"  //-> Clave De La Red
#define IP "191.102.85.226"
#include <SoftwareSerial.h>
//SoftwareSerial ESP(3, 2); // RX | TX

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200);
  hum1.begin();
  hum2.begin();
  hum3.begin();
  //Verificar que el ESP está funcionando
  Serial.println("Comprobando modulo WiFi");
  Serial1.println("AT");
  delay(10);
  int com = 1;
  while (com)
  {
    if (Serial1.find('OK')) {
      Serial.println("Modulo funcionando");
      com = 0;
    }
    else
    {
      Serial.println("Reiniciando modulo");
      Serial1.println("AT+RST");
      delay(100);
      Serial1.println("AT");
    }
  }
  lcd.begin();
  //lcd.backlight();
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
  HR1 = analogRead(A0); // Lee el sensor 1 de humedad del suelo <----
  Serial.println("Hum Relativa1: " + String(HR1) + "%");
  lcd.print("Hum Relativa11: ");
  lcd.setCursor(0, 1); // Bottom left
  lcd.print(HR1);
  lcd.print(" %");
  delay(2000);
  lcd.clear();
  HR2 = analogRead(A1); // Lee el sensor 2 de humedad del suelo <----
  Serial.println("Hum Relativa2: " + String(HR2) + "%");
  lcd.print("Hum Relativa2: ");
  lcd.setCursor(0, 1); // Bottom left
  lcd.print(HR2);
  lcd.print(" %");
  delay(2000);
  lcd.clear();
  HR3 = analogRead(A2); // Lee el sensor 3 de humedad del suelo <----
  Serial.println("Hum Relativa3: " + String(HR3) + "%");
  lcd.print("Hum Relativa3: ");
  lcd.setCursor(0, 1); // Bottom left
  lcd.print(HR3);
  lcd.print(" %");
  delay(2000);
  lcd.clear();
  temp1 = analogRead(A3); // Lee el sensor 1 de temperatura <----
  temp1 = (5.0 * temp1 * 100.0) / 1024.0;
  Serial.println("Temp1: " + String(temp1) + "°C");
  lcd.print("Temp1: ");
  lcd.setCursor(0, 1); // Bottom left
  lcd.print(temp1 / 2.5);
  lcd.print(" Celsius");
  delay(2000);
  lcd.clear();
  temp2 = analogRead(A4); // Lee el sensor 2 de temperatura <----
  temp2 = (5.0 * temp2 * 100.0) / 1024.0;
  Serial.println("Temp2: " + String(temp2) + "°C");
  lcd.print("Temp2: ");
  lcd.setCursor(0, 1);
  lcd.print(temp2 / 2.5);
  lcd.print(" Celsius");
  delay(2000);
  lcd.clear();
  temp3 = analogRead(A5); // Lee el sensor 3 de temperatura <----
  temp3 = (5.0 * temp3 * 100.0) / 1024.0;
  Serial.println("Temp3: " + String(temp3) + "°C");
  lcd.print("Temp3: ");
  lcd.setCursor(0, 1); // Bottom left
  lcd.print(temp3 / 2.5);
  lcd.print(" Celsius");
  delay(2000);
  lcd.clear();

  //Serial.println("Iniciando lectura de sensores DHT");
  //int err1;
  //if ((err1 = hum1.read(humi1, temp1)) == 0) {

  humi1 = hum1.readHumidity();
  //float t = dht.readTemperature();
  if (isnan(humi1)) { // Comprueba si son numeros las variables indicadas
    Serial.println();
    Serial.print("Error No :");
    Serial.print(humi1);
    Serial.println();
  } else {
    Serial.println("Humedad relativa del aire 1: " + String(humi1) + "%");
    lcd.print("Hum Aire1:");
    lcd.setCursor(0, 1);
    lcd.print(humi1);
    lcd.print(" %");
    delay(2000);
    lcd.clear();
  }
  /*
    if ((err1 = hum1.read(humi1)) == 0) {
    lcd.print("Hum Aire1:");
    lcd.setCursor(0, 1);
    lcd.print(humi1);
    lcd.print(" %");
    delay(2000);
    lcd.clear();
    } else {
    Serial.println();
    Serial.print("Error No :");
    Serial.print(err1);
    Serial.println();
    }
    delay(DHT_RETRY_DELAY);
  */
  // int err2;
  //if ((err2 = hum2.read(humi2, temp2)) == 0) {

  humi2 = hum2.readHumidity();
  //float t = dht.readTemperature();
  if (isnan(humi2)) { // Comprueba si son numeros las variables indicadas
    Serial.println();
    Serial.print("Error No :");
    Serial.print(humi2);
    Serial.println();
  } else {
    Serial.println("Humedad relativa del aire 2: " + String(humi2) + "%");
    lcd.print("Hum Aire2:");
    lcd.setCursor(0, 1);
    lcd.print(humi2);
    lcd.print(" %");
    delay(2000);
    lcd.clear();
  }
  /*if ((err2 = hum2.read(humi2)) == 0) {
    lcd.print("Hum Aire2:");
    lcd.setCursor(0, 1);
    lcd.print(humi2);
    lcd.print(" %");
    delay(2000);
    lcd.clear();
    } else {
    Serial.println();
    Serial.print("Error No :");
    Serial.print(err2);
    Serial.println();
    }
    delay(DHT_RETRY_DELAY);
  */
  //int err3;
  //if ((err3 = hum3.read(humi3, temp3)) == 0) {

  humi3 = hum3.readHumidity();
  //float t = dht.readTemperature();
  if (isnan(humi3)) { // Comprueba si son numeros las variables indicadas
    Serial.println();
    Serial.print("Error No :");
    Serial.print(humi3);
    Serial.println();
  } else {
    Serial.println("Humedad relativa del aire 3: " + String(humi3) + "%");
    lcd.print("Hum Aire3:");
    lcd.setCursor(0, 1);
    lcd.print(humi3);
    lcd.print(" %");
    delay(2000);
    lcd.clear();
  }

  /*if ((err3 = hum3.read(humi3)) == 0) {
    lcd.print("Hum Aire3:");
    lcd.setCursor(0, 1);
    lcd.print(humi3);
    lcd.print(" %");
    delay(2000);
    lcd.clear();
    } else {
    Serial.println();
    Serial.print("Error No :");
    Serial.print(err3);
    Serial.println();
    }
    delay(DHT_RETRY_DELAY);
  */
  lux1 = lightMeter1.readLightLevel(); // Lee el sensor 1 de luminocidad
  Serial.println("Sensor de luminocidad 1: " + String(lux1));
  lcd.print("Luminosidad1: ");
  lcd.setCursor(0, 1); // Bottom left
  lcd.print(lux1);
  lcd.print(" lux");
  delay(2000);
  lcd.clear();
  lux2 = lightMeter2.readLightLevel(); // Lee el sensor 2 de luminocidad
  Serial.println("Sensor de luminocidad 2: " + String(lux2));
  lcd.print("Luminosidad2: ");
  lcd.setCursor(0, 1);
  lcd.print(lux2);
  lcd.print(" lux");
  delay(2000);
  lcd.clear();
  lux3 = lightMeter3.readLightLevel(); // Lee el sensor 3 de luminocidad
  Serial.println("Sensor de luminocidad 3: " + String(lux3));
  lcd.print("Luminosidad3: ");
  lcd.setCursor(0, 1); // Bottom left
  lcd.print(lux3);
  lcd.print(" lux");
  delay(2000);
  lcd.clear();

  enviar_wifi();
}

void mostrarRTCLCD()
{
  DateTime now = RTC.now(); // Obtiene datos del modulo RTC (pila de reloj)
  lcd.clear();
  lcd.setCursor(0, 0); // Top left
  if (now.day() < 10)
  {
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
  if (now.hour() < 10)
  {
    lcd.print("0");
  }
  lcd.print(now.hour(), DEC); //Imprime hora
  lcd.print(':');
  if (now.minute() < 10)
  {
    lcd.print("0");
  }
  lcd.print(now.minute(), DEC); //Imprime minutos
  lcd.print(':');
  if (now.second() < 10)
  {
    lcd.print("0");
  }
  lcd.print(now.second(), DEC); //Imprime segundos
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


void enviar_wifi() {     // metodo para enviar los datos a la base de datos.
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += IP;
  cmd += "\",80";
  Serial.print("CMD en enviar_wifi(): ");
  Serial.println(cmd);
  Serial1.println(cmd);
  Serial.println(cmd);
  if (Serial1.find("Error")) {
    Serial.println("AT+CIPSTART error");
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
}
/*
  Envío: http://191.102.85.226/Sweb_gral/Service1.svc/Servicio_Grabar/27,0@12@12@12@12@12@12@12@12@12
  Result: {"ServicioGuardarResult":"{\"Resultado\":\"OK\",\"Mensaje\":\"OK\"}"}
*/
