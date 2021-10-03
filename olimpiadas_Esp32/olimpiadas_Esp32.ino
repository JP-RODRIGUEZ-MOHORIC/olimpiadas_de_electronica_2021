#include <analogWrite.h> // importa libreria para usar analogWrite con ESP32
#include <Keypad.h> // importa libreria Keypad
#include <WiFi.h> // Load Wi-Fi library
#define DEBUG 0
#define CONEXION 0

// Replace with your network credentials
const char* ssid = "Leo mar juan";
const char* password = "IVOM8264";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

//Variables del sensor de CO2
#define THRSH 600   //750
#define THRSL 500   //500
#define THRS_ALARM 750  //900

#define mq135 15

int estado_sensor;
int CO2;
int CO2_final;
bool ventilacion = 0;

//Variables maquina de estado CO2

#define stdby 1
#define extraccion 2
#define alarma 3

int estado_CO2;

//Conexion Motores

#define MOTOR_IN 2
#define MOTOR_OUT 4

//Conexion buzzer

#define BUZZER 15

//Conexion led RGB

#define pinR 19
#define pinG 18
#define pinB 5

//variables del keypad

#define RESET 0
#define CORRECTO 1
#define INCORRECTO 2

const byte FILAS = 4;          // define numero de filas
const byte COLUMNAS = 4;       // define numero de columnas
char keys[FILAS][COLUMNAS] = { // define la distribucion de teclas
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};

byte pinesFilas[FILAS] = {13, 12, 14, 27};       // pines correspondientes a las filas
byte pinesColumnas[COLUMNAS] = {26, 25, 33, 32}; // pines correspondientes a las columnas

Keypad teclado = Keypad(makeKeymap(keys), pinesFilas, pinesColumnas, FILAS, COLUMNAS); // crea objeto

char TECLA;    // almacena la tecla presionada
char CLAVE[7]; // almacena en un array 6 digitos ingresados
int comprobacion_clave = 0;
char CLAVE_MAESTRA_1[7] = "147147"; // almacena en un array la contraseña maestra
char CLAVE_MAESTRA_2[7] = "369369"; // almacena en un array la contraseña maestra
char CLAVE_MAESTRA_3[7] = "912018"; // almacena en un array la contraseña maestra
char CLAVE_MAESTRA_4[7] = "266011"; // almacena en un array la contraseña maestra
char CLAVE_MAESTRA_5[7] = "503282"; // almacena en un array la contraseña maestra
char CLAVE_MAESTRA_6[7] = "123456"; // almacena en un array la contraseña maestra

byte INDICE = 0; // indice del array

//Estructura tabla
struct Registro
   {   int id;
       String espacio;
       int ingreso;
       int egreso; 
   } ;

void setup()
{
  Serial.begin(115200);
  if(CONEXION){
  //Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
  }
  
  //pines led rgb
  pinMode(pinR, OUTPUT);
  pinMode(pinG, OUTPUT);
  pinMode(pinB, OUTPUT);

  //inicializacion Mef_CO2
  led_verde();
  digitalWrite(pinG, HIGH);
  estado_CO2 = stdby;
}

void loop()
{
  Mef_CO2();
  LeerTeclado();
  Server();
  
}


void LeerSensor()
{
  //Codigo deteccion del sensor
  CO2 = analogRead(mq135);              //Leemos la salida analógica del MQ
  CO2_final = map(CO2 , 0, 4095, 0, 1023);
  float voltaje = CO2_final * (5.0 / 1023.0); //Convertimos la lectura en un valor de voltaje
  if (!DEBUG)
  {
    Serial.print("adc:");
    Serial.print(CO2_final);
    Serial.print("    voltaje:");
    Serial.println(voltaje);
  }
}

void Mef_CO2()
{
  LeerSensor();
  switch (estado_CO2)
  {
  case stdby:
    if (CO2_final > THRSH)
    { //EVALUO SI EL NIVEL DE CO2 SUPERO EL PRIMER UMBRAL
      Serial.println("Enciendo led Amarillo");
      estado_CO2 = extraccion;
      digitalWrite(MOTOR_IN, HIGH);
      digitalWrite(MOTOR_OUT, HIGH);
      led_amarillo();
    }
    break;
  case extraccion:
    if (CO2_final > THRS_ALARM)
    {
      Serial.println("Enciendo led rojo");
      estado_CO2 = alarma;
      digitalWrite(BUZZER, HIGH);
      led_rojo();
    }

    if (CO2_final < THRSL)
    { //EVALUO SI EL NIVEL DE CO2 ES MENOR AL UMBRAL MAS BAJO POR LO QUE PASO A MODO STDBY
      Serial.println("Enciendo led Verde");
      estado_CO2 = stdby;
      digitalWrite(MOTOR_IN, LOW);
      digitalWrite(MOTOR_OUT, LOW);
      led_verde();
    }
    break;

  case alarma:
    if (CO2_final < THRS_ALARM)
    {
      Serial.println("Enciendo led Amarillo");
      estado_CO2 = extraccion;
      digitalWrite(BUZZER, LOW);
      led_amarillo();
    }
    break;
  }
}

void led_verde()
{
  analogWrite(pinR, 255);
  analogWrite(pinG, 0);
  analogWrite(pinB, 255);
}

void led_rojo()
{
  analogWrite(pinR, 0);   //azul
  analogWrite(pinG, 255);     //verde
  analogWrite(pinB, 255);    
}

void led_amarillo()
{
  analogWrite(pinR, 0);  // en 0 se prende azul
  analogWrite(pinG, 207); 
  analogWrite(pinB, 255);
}

void LeerTeclado()
{
  //codigo keypad
  TECLA = teclado.getKey(); // obtiene tecla presionada y asigna a variable
  if (TECLA)                // comprueba que se haya presionado una tecla
  {
    CLAVE[INDICE] = TECLA; // almacena en array la tecla presionada
    INDICE++;              // incrementa indice en uno
    Serial.print(TECLA);   // envia a monitor serial la tecla presionada
  }

  if (INDICE == 6) // si ya se almacenaron los 6 digitos
  { 
    if (!strcmp(CLAVE, CLAVE_MAESTRA_1) or !strcmp(CLAVE, CLAVE_MAESTRA_2) or !strcmp(CLAVE, CLAVE_MAESTRA_3) or !strcmp(CLAVE, CLAVE_MAESTRA_4) or !strcmp(CLAVE, CLAVE_MAESTRA_5) or !strcmp(CLAVE, CLAVE_MAESTRA_6))
    { // compara clave ingresada con clave maestra
      if (DEBUG)
      {
        Serial.println(" Correcta"); // imprime en monitor serial que es correcta la clave
      }
      comprobacion_clave = CORRECTO;
    }
    else
    {
      if (DEBUG)
      {
        Serial.println(" Incorrecta"); // imprime en monitor serial que es incorrecta la clave
      }
      comprobacion_clave = INCORRECTO;
    }
  }
  if (TECLA == 'A') //Si apretamos la tecla A reseteamos los valores a 0 e ingresamos la contraseña nuevamente.
  {
    if (DEBUG)
    {
      Serial.println(" ");
      Serial.println("Ingresar Contraseña");
    }
    for (int x = 0; x < sizeof(CLAVE) / sizeof(CLAVE[0]); x++)
    {
      CLAVE[x] = 0;
    }
    comprobacion_clave = RESET;
    INDICE = 0;
  }
}

void Server()
{
  /****************************Datos Tabla*****************************************/
  Registro Usuario_1 ;
     Usuario_1.id = 1 ;
     Usuario_1.espacio = "Habitacion 1" ;
     Usuario_1.ingreso = 10 ;
     Usuario_1.egreso = 15 ;

 Registro Usuario_2 ;
     Usuario_2.id = 2 ;
     Usuario_2.espacio = "Habitacion 2" ;
     Usuario_2.ingreso = 9 ;
     Usuario_2.egreso = 12 ;

     Registro Usuario_3 ;
     Usuario_3.id = 3 ;
     Usuario_3.espacio = "Habitacion 3" ;
     Usuario_3.ingreso = 10 ;
     Usuario_3.egreso = 12 ;

     Registro Usuario_4 ;
     Usuario_4.id = 4 ;
     Usuario_4.espacio = "Habitacion 4" ;
     Usuario_4.ingreso = 20 ;
     Usuario_4.egreso = 32 ;

     Registro Usuario_5 ;
     Usuario_5.id = 5 ;
     Usuario_5.espacio = "Habitacion 5" ;
     Usuario_5.ingreso = 5 ;
     Usuario_5.egreso = 52 ;

     Registro Usuario_6 ;
     Usuario_6.id = 6 ;
     Usuario_6.espacio = "Habitacion 6" ;
     Usuario_6.ingreso = 8;
     Usuario_6.egreso = 43 ;
  /****************************Servidor*****************************************/
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

          //client.println(F("Refresh:0")); //Refresca la pagina automaticamente cada x tiempo
          String box_css = "display: inline-block; border: 2px solid black; border-radius: 10px; background-color: #fff; color: black;";
          String table_css = "background-color: darkblue; text-align: center; font-size: 25px;";
          client.println();
          client.println(F("<html>"));
          client.println(F("<head>"));
          client.println(F("<meta charset='UTF-8'>"));
          client.println(F("<title>Panel de Control</title>"));
          client.println(F("</head>"));
          client.println(F("<body style='background-color: #333; color: #fff;'>"));
          client.println(F("<div style='display: flex; justify-content: space-between; height: 80vh;'>"));
          client.println(F("<div style='text-align: center; width: 40%; padding-top: 100px; border-bottom: 2px solid white;'>"));
          client.println(F("<p style='font-size: 40px; font-weight: bold;'>Sensor de CO2</p>"));
          client.println(F("<div style='display: inline-block; border: 2px solid black; border-radius: 10px; background-color: #fff; color: black;font-size: 50px;  width: 200px; height: 100px;'><p style='margin-top: 18px;'>"));
          client.println(CO2);
          client.println(F("</p></div>"));
          client.println(F("</div>"));
          client.println(F("<div style='display: flex; flex-direction: column; text-align: center; margin-right: auto; margin-left: auto; font-size: 25px; width: 100%; border-left: 2px solid white;  border-bottom: 2px solid white;'>"));
          client.println(F("<div>"));
          client.println(F("<p>Contraseña</p>"));
          client.println(F("<p style='display: inline-block; border: 2px solid black; border-radius: 10px; background-color: #fff; color: black;padding: 5px; width: 100px; min-height: 25px;'>"));
          client.println(CLAVE);
          client.println(F("</p>"));
          client.println(F("</div>"));
          client.println(F("<div>"));
          client.println(F("<p>Datos 2</p>"));
          client.println(F("<p style='display: inline-block; border: 2px solid black; border-radius: 10px; background-color: #fff; color: black;padding: 5px; width: 100px;'>-</p>"));
          client.println(F("</div>"));
          client.println(F("<div>"));
          client.println(F("<p>Ventiladores</p>"));
          if (ventilacion == 1){
            client.println(F("<p style='display: inline-block; border: 2px solid black; border-radius: 10px; padding: 5px; width: 100px; color: black; background-color: #fff;'>on</p>"));
          }
          if (ventilacion == 0){
            client.println(F("<p style='display: inline-block; border: 2px solid black; border-radius: 10px; padding: 5px; width: 100px; color: black; background-color: #fff;'>off</p>"));
          }
          client.println(F("</div>"));
          client.println(F("</div>"));
          client.println(F("</div>"));
          client.println(F("<a href='http://192.168.10.177/' style='display: inline-block; text-decoration: none; color: white; font-size: 20px; padding: 20px; border: 2px solid white;  margin-top: 15px; width:97%; text-align: center; background-color: black;'>Refrescar</a>"));

          if (comprobacion_clave == 0)
          {
            client.println(F("<h1>Ingresar contraseña<h1>"));
          }
          /***************************TABLA********************************/
          if (comprobacion_clave == CORRECTO)
          {
            client.println(F("<h1>Contraseña Correcta, Bienvenido<h1>"));
            client.println(F("<h1 style='text-align: center;'>Panel de Control</h1>"));
            client.println(F("<table style='margin: auto; margin-top: 10px; background-color: #fff; text-align: center;height: 500px; width: 750px; border: 2px solid #fff; border-top: none;'>"));
            client.println(F("<caption style='background-color: darkblue; text-align: center; font-size: 25px;border: solid 4px #fff; border-bottom: none;'>Tabla de Datos</caption>"));
            client.println(F("<thead style='background-color: darkblue; text-align: center; font-size: 25px;'>"));
            client.println(F("<th>ID</th>"));
            client.println(F("<th>Nombre</th>"));
            client.println(F("<th>Ingreso</th>"));
            client.println(F("<th>Egreso</th>"));
            client.println(F("<th>Ubicación</th>"));
            client.println(F("<th>Estado del sistema</th>"));
            client.println(F("<th>Estado del Bus</th>"));
            client.println(F("</thead>"));
            client.println(F("<tbody style='text-align: center;'>"));
            
            if (!strcmp(CLAVE, CLAVE_MAESTRA_1)){
            client.println(F("<tr style='background-color: darkblue; text-align: center; font-size: 25px;'>"));
            client.println(F("<td>"));
            client.println(Usuario_1.id);
            client.println(F("</td>"));
            client.println(F("<td>"));
            client.println(Usuario_1.espacio);
            client.println(F("</td>"));
            client.println(F("<td>"));
            client.println(Usuario_1.ingreso);
            client.println(F("</td>"));
            client.println(F("<td>"));
            client.println(Usuario_1.egreso);
            client.println(F("</td>"));
            client.println(F("<td>"));
            client.println(Usuario_1.id);
            client.println(F("</td>"));
            client.println(F("<td>"));
            client.println(Usuario_1.id);
            client.println(F("</td>"));
            client.println(F("<td>"));
            client.println(Usuario_1.id);
            client.println(F("</td>"));
            client.println(F("</tr>"));
            }
            if (!strcmp(CLAVE, CLAVE_MAESTRA_2)){
            client.println(F("<tr style='background-color: darkblue; text-align: center; font-size: 25px;'>"));
            client.println(F("<td>"));
            client.println(Usuario_2.id);
            client.println(F("</td>"));
            client.println(F("<td>"));
            client.println(Usuario_2.espacio);
            client.println(F("</td>"));
            client.println(F("<td>"));
            client.println(Usuario_2.ingreso);
            client.println(F("</td>"));
            client.println(F("<td>"));
            client.println(Usuario_2.egreso);
            client.println(F("</td>"));
            client.println(F("<td>"));
            client.println(Usuario_2.id);
            client.println(F("</td>"));
            client.println(F("<td>"));
            client.println(Usuario_2.id);
            client.println(F("</td>"));
            client.println(F("<td>"));
            client.println(Usuario_2.id);
            client.println(F("</td>"));
            client.println(F("</tr>"));
            }
            if (!strcmp(CLAVE, CLAVE_MAESTRA_3)){
            client.println(F("<tr style='background-color: darkblue; text-align: center; font-size: 25px;'>"));
            client.println(F("<td>"));
            client.println(Usuario_3.id);
            client.println(F("</td>"));
            client.println(F("<td>"));
            client.println(Usuario_3.espacio);
            client.println(F("</td>"));
            client.println(F("<td>"));
            client.println(Usuario_3.ingreso);
            client.println(F("</td>"));
            client.println(F("<td>"));
            client.println(Usuario_3.egreso);
            client.println(F("</td>"));
            client.println(F("<td>"));
            client.println(Usuario_3.id);
            client.println(F("</td>"));
            client.println(F("<td>"));
            client.println(Usuario_3.id);
            client.println(F("</td>"));
            client.println(F("<td>"));
            client.println(Usuario_3.id);
            client.println(F("</td>"));
            client.println(F("</tr>"));
            }
            if (!strcmp(CLAVE, CLAVE_MAESTRA_4)){
            client.println(F("<tr style='background-color: darkblue; text-align: center; font-size: 25px;'>"));
            client.println(F("<td>"));
            client.println(Usuario_4.id);
            client.println(F("</td>"));
            client.println(F("<td>"));
            client.println(Usuario_4.espacio);
            client.println(F("</td>"));
            client.println(F("<td>"));
            client.println(Usuario_4.ingreso);
            client.println(F("</td>"));
            client.println(F("<td>"));
            client.println(Usuario_4.egreso);
            client.println(F("</td>"));
            client.println(F("<td>"));
            client.println(Usuario_4.id);
            client.println(F("</td>"));
            client.println(F("<td>"));
            client.println(Usuario_4.id);
            client.println(F("</td>"));
            client.println(F("<td>"));
            client.println(Usuario_4.id);
            client.println(F("</td>"));
            client.println(F("</tr>"));
            }
            if (!strcmp(CLAVE, CLAVE_MAESTRA_5)){
            client.println(F("<tr style='background-color: darkblue; text-align: center; font-size: 25px;'>"));
            client.println(F("<td>"));
            client.println(Usuario_5.id);
            client.println(F("</td>"));
            client.println(F("<td>"));
            client.println(Usuario_5.espacio);
            client.println(F("</td>"));
            client.println(F("<td>"));
            client.println(Usuario_5.ingreso);
            client.println(F("</td>"));
            client.println(F("<td>"));
            client.println(Usuario_5.egreso);
            client.println(F("</td>"));
            client.println(F("<td>"));
            client.println(Usuario_5.id);
            client.println(F("</td>"));
            client.println(F("<td>"));
            client.println(Usuario_5.id);
            client.println(F("</td>"));
            client.println(F("<td>"));
            client.println(Usuario_5.id);
            client.println(F("</td>"));
            client.println(F("</tr>"));
            }
            if (!strcmp(CLAVE, CLAVE_MAESTRA_6)){
            client.println(F("<tr style='background-color: darkblue; text-align: center; font-size: 25px;'>"));
            client.println(F("<td>"));
            client.println(Usuario_6.id);
            client.println(F("</td>"));
            client.println(F("<td>"));
            client.println(Usuario_6.espacio);
            client.println(F("</td>"));
            client.println(F("<td>"));
            client.println(Usuario_6.ingreso);
            client.println(F("</td>"));
            client.println(F("<td>"));
            client.println(Usuario_6.egreso);
            client.println(F("</td>"));
            client.println(F("<td>"));
            client.println(Usuario_6.id);
            client.println(F("</td>"));
            client.println(F("<td>"));
            client.println(Usuario_6.id);
            client.println(F("</td>"));
            client.println(F("<td>"));
            client.println(Usuario_6.id);
            client.println(F("</td>"));
            client.println(F("</tr>"));
            }
            client.println(F("</tbody>"));
            client.println(F("</table>"));
            client.println(F("<a href='#' download style='width: 50px; height: 50px;margin-left: 290px; font-size: 30px; color: #fff;'>Descargar datos de la tabla</a>"));
          }
          
          if (comprobacion_clave == INCORRECTO)
          {
            client.println(F("<h1>Contraseña Incorrecta<h1>"));
          }

          client.println(F("</body>"));
          client.println(F("</html>"));
          break;
        }
        else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");  
    }
  }
