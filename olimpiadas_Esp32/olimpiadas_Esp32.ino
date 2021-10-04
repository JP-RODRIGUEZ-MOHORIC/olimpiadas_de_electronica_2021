#include <analogWrite.h> // importa libreria para usar analogWrite con ESP32
#include <Keypad.h>      // importa libreria Keypad
#include <WiFi.h>        /// importa libreria WiFi
#define DEBUG 0
#define CONEXION 1
#define PORT 80
// Conexion al wifi
const char *ssid = "Leo mar juan";
const char *WiFi_password = "IVOM8264";

// Seteamos el server en el puerto 80
WiFiServer server(PORT);

// Variable para almacenar la peticion http
String header;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0;
// Define el tiempo de timeout en milisegundos (ejemplo: 2000ms = 2s)
const long timeoutTime = 2000;

//Variables del sensor de CO2
#define THRSH 600      //750
#define THRSL 500      //500
#define THRS_ALARM 750 //900

#define MQ135 35
#define ON true
#define OFF false

int CO2;
int CO2_final;
bool ventilation_status = false;
bool alarm_status = false;

//Variables maquina de estado CO2

#define STDBY 1
#define EXTRACCION 2
#define ALARM 3

int CO2_Status;

//Conexion Motores

#define MOTOR_IN 2
#define MOTOR_OUT 4

//Conexion buzzer

#define BUZZER 15

//Conexion led RGB

#define PIN_RED 19
#define PIN_GREEN 18
#define PIN_BLUE 5

//variables del keypad

#define RESET 0
#define CORRECT 1
#define INCORRECT 2

const byte ROW = 4;          // define numero de filas
const byte COLUMN = 4;       // define numero de columnas
char keys[ROW][COLUMN] = { // define la distribucion de teclas
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};

byte rowpins[ROW] = {13, 12, 14, 27};       // pines correspondientes a las filas
byte columnpins[COLUMN] = {26, 25, 33, 32}; // pines correspondientes a las columnas

Keypad keypad = Keypad(makeKeymap(keys), rowpins, columnpins, ROW, COLUMN); // crea objeto

char key;    // almacena la tecla presionada
char password[7]; // almacena en un array 6 digitos ingresados
int key_comprobation = 0;
char master_key_1[7] = "147147"; // almacena en un array la contraseña maestra
char master_key_2[7] = "369369"; // almacena en un array la contraseña maestra
char master_key_3[7] = "912018"; // almacena en un array la contraseña maestra
char master_key_4[7] = "266011"; // almacena en un array la contraseña maestra
char master_key_5[7] = "503282"; // almacena en un array la contraseña maestra
char master_key_6[7] = "123456"; // almacena en un array la contraseña maestra

int index_keypad = 0; // indice del array

//Estructura tabla
struct User
{
  int id;
  String habitacion;
  int ingreso;
  int egreso;
  bool login = false;
};

User user_1 = User();
User user_2 = User();
User user_3 = User();
User user_4 = User();
User user_5 = User();
User user_6 = User();

int Users_login_count = 0;
User sessions[20];

User GenerateUser(int id)
{
  User user = User();
  user.id = id;
  user.habitacion = "Habitacion " + id;
  user.ingreso = random(5, 20);
  user.egreso = random(5, 20);
  return user;
}

void setup()
{

  Serial.begin(115200);
  if (CONEXION)
  {
    //Conecta a Wifi mediante ssid y passwor
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, WiFi_password);
    while (WiFi.status() != WL_CONNECTED)
    {
      delay(500);
      Serial.print(".");
    }

    // Imprime el IP local y inicia el servidor
    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    server.begin();
  }

  //pines led rgb
  pinMode(PIN_RED, OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);
  pinMode(PIN_BLUE, OUTPUT);

  //inicializacion Mef_CO2
  green_led();
  CO2_Status = STDBY;

  //Inicializacion Motores
  pinMode(MOTOR_IN, OUTPUT);
  pinMode(MOTOR_OUT, OUTPUT);

  //Inicializacion buzzer
  pinMode(BUZZER, OUTPUT);

  user_1 = GenerateUser(1);
  user_2 = GenerateUser(2);
  user_3 = GenerateUser(3);
  user_4 = GenerateUser(4);
  user_5 = GenerateUser(5);
  user_6 = GenerateUser(6);
}

void loop()
{
  Mef_CO2();
  KeypadRead();
  Server();
}

void LeerSensor()
{
  //Codigo deteccion del sensor
  CO2 = analogRead(MQ135); //Leemos la salida analógica del MQ
  CO2_final = map(CO2, 0, 4095, 0, 1023);
  float voltaje = CO2_final * (5.0 / 1023.0); //Convertimos la lectura en un valor de voltaje
  if (DEBUG)
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
  switch (CO2_Status)
  {
  case STDBY:
    if (CO2_final > THRSH)
    { //EVALUO SI EL NIVEL DE CO2 SUPERO EL PRIMER UMBRAL
      Serial.println("Enciendo led Amarillo");
      CO2_Status = EXTRACCION;
      ventilation_status = ON;
      digitalWrite(MOTOR_IN, HIGH);
      digitalWrite(MOTOR_OUT, HIGH);
      yellow_led();
    }
    break;
  case EXTRACCION:
    if (CO2_final > THRS_ALARM)
    {
      Serial.println("Enciendo led rojo");
      CO2_Status = ALARM;
      alarm_status = ON;
      digitalWrite(BUZZER, HIGH);
      red_led();
    }

    if (CO2_final < THRSL)
    { //EVALUO SI EL NIVEL DE CO2 ES MENOR AL UMBRAL MAS BAJO POR LO QUE PASO A MODO STDBY
      Serial.println("Enciendo led Verde");
      CO2_Status = STDBY;
      ventilation_status = OFF;
      digitalWrite(MOTOR_IN, LOW);
      digitalWrite(MOTOR_OUT, LOW);
      green_led();
    }
    break;

  case ALARM:
    if (CO2_final < THRS_ALARM)
    {
      Serial.println("Enciendo led Amarillo");
      CO2_Status = EXTRACCION;
      alarm_status = OFF;
      digitalWrite(BUZZER, LOW);
      yellow_led();
    }
    break;
  }
}

void green_led()
{
  analogWrite(PIN_RED, 255);
  analogWrite(PIN_GREEN, 0);
  analogWrite(PIN_BLUE, 255);
}

void red_led()
{
  analogWrite(PIN_RED, 0);   //azul
  analogWrite(PIN_GREEN, 255); //verde
  analogWrite(PIN_BLUE, 255);
}

void yellow_led()
{
  analogWrite(PIN_RED, 0); // en 0 se prende azul
  analogWrite(PIN_GREEN, 207);
  analogWrite(PIN_BLUE, 255);
}

void KeypadRead()
{
  //codigo keypad
  key = keypad.getKey(); // obtiene tecla presionada y asigna a variable
  if (key)                // comprueba que se haya presionado una tecla
  {
    password[index_keypad] = key; // almacena en array la tecla presionada
    index_keypad++;              // incrementa indice en uno
    Serial.print(key);   // envia a monitor serial la tecla presionada
  }

  if (index_keypad == 6) // si ya se almacenaron los 6 digitos
  {
    Autentification();
  }
  Logout();
}

void Autentification()
{
  if (!strcmp(password, master_key_1) or !strcmp(password, master_key_2) or !strcmp(password, master_key_3) or !strcmp(password, master_key_4) or !strcmp(password, master_key_5) or !strcmp(password, master_key_6))
  { // compara clave ingresada con clave maestra

    Serial.print("Inicio de session:");
    Serial.println(Users_login_count);
    User session = User();

    if (!strcmp(password, master_key_1))
    {
      session = user_1;
    }
    if (!strcmp(password, master_key_2))
    {
      session = user_2;
    }
    if (!strcmp(password, master_key_3))
    {
      session = user_3;
    }
    if (!strcmp(password, master_key_4))
    {
      session = user_4;
    }
    if (!strcmp(password, master_key_5))
    {
      session = user_5;
    }
    if (!strcmp(password, master_key_6))
    {
      session = user_6;
    }
    session.login = true;
    sessions[Users_login_count] = session;
    Users_login_count++;
    index_keypad = 0;
    key_comprobation = CORRECT;
  }
  else
  {
    key_comprobation = INCORRECT;
  }
}

void Logout()
{
  if (key == 'A') //Si apretamos la tecla A reseteamos los valores a 0 e ingresamos la contraseña nuevamente.
  {
    if (DEBUG)
    {
      Serial.println(" ");
      Serial.println("Ingresar Contraseña");
    }
    for (int x = 0; x < sizeof(password) / sizeof(password[0]); x++)
    {
      password[x] = 0;
    }
    key_comprobation = RESET;
    index_keypad = 0;
  }
}

void Server()
{
  /****************************Servidor*****************************************/
  WiFiClient client = server.available(); // Listen for incoming clients

  if (client)
  { // Si se conecta alguien,
    currentTime = millis();
    previousTime = currentTime;
    //  Serial.println("New Client."); // Imprime un mensaje en el puerto serial
    String currentLine = ""; // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime)
    { // loop while the client's connected
      currentTime = millis();
      if (client.available())
      {                         // if there's bytes to read from the client,
        char c = client.read(); // read a byte, then
        Serial.write(c);        // print it out the serial monitor
        header += c;
        if (c == '\n')
        { // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0)
          {

            HtmlHeaders(client);

            client.println(F("<html>"));
            client.println(F("<head>"));
            client.println(F("<meta charset='UTF-8'>"));
            client.println(F("<title>Panel de Control</title>"));
            WebRefresh(client);
            client.println(F("</head>"));
            client.println(F("<body onload='GetSwitchState()' id =  'switch_txt' style='background-color: #333; color: #fff;'>"));
            HtmlCo2View(client);
            client.println(F("<div style='display: flex; flex-direction: column; text-align: center; margin-right: auto; margin-left: auto; font-size: 25px; width: 100%; border-left: 2px solid white;  border-bottom: 2px solid white;'>"));
            HtmlClaveView(client);
            HtmlStatus(client, "Ventiladores", ventilation_status);
            HtmlStatus(client, "Alarma", alarm_status);
            client.println(F("</div>"));
            client.println(F("</div>"));

            if (key_comprobation == RESET)
            {
              client.println(F("<h1>Ingresar contraseña<h1>"));
            }

            /***************************TABLA********************************/
            if (key_comprobation == CORRECT)
            {
              HtmlTableUsersSession(client);
              client.println(F("<a href='#' download style='width: 50px; height: 50px;margin-left: 290px; font-size: 30px; color: #fff;'>Descargar datos de la tabla</a>"));
            }

            if (key_comprobation == INCORRECT)
            {
              client.println(F("<h1>Contraseña Incorrecta<h1>"));
            }
            client.println(F("</body>"));
            client.println(F("</html>"));

            break;
          }
          else
          { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        }
        else if (c != '\r')
        {                   // if you got anything else but a carriage return character,
          currentLine += c; // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    // Serial.println("Client disconnected.");
    //Serial.println("");
  }
}

void WebRefresh(WiFiClient client)
{
  client.println(F("<script>"));
  client.println(F("function GetSwitchState(){nocache = '&nocache=' + Math.random() * 1000000;"));
  client.println(F("var request = new XMLHttpRequest();"));
  client.println(F("request.onreadystatechange = function(){"));
  client.println(F("  if(this.readyState == 4){"));
  client.println(F("if(this.status == 200){"));
  client.println(F("if(this.responseText != null){"));
  client.println(F("document.getElementById('switch_txt').innerHTML = this.responseText;}}}}"));
  client.println(F("request.open('GET', 'ajax_switch' + nocache, true);"));
  client.println(F("request.send(null);"));
  client.println(F("setTimeout('GetSwitchState()', 1000);}"));
  client.println(F("</script>"));
}

void HtmlHeaders(WiFiClient client)
{
  // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
  // and a content-type so the client knows what's coming, then a blank line:
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html");
  client.println("Connection: close");
  client.println();
  client.println();
}

void HtmlCo2View(WiFiClient client)
{
  client.println(F("<div style='display: flex; justify-content: space-between; height: 80vh;'>"));
  client.println(F("<div style='text-align: center; width: 40%; padding-top: 100px; border-bottom: 2px solid white;'>"));
  client.println(F("<p style='font-size: 40px; font-weight: bold;'>Sensor de CO2</p>"));
  client.println(F("<div style='display: inline-block; border: 2px solid black; border-radius: 10px; background-color: #fff; color: black;font-size: 50px;  width: 200px; height: 100px;'><p style='margin-top: 18px;'>"));
  client.println(CO2_final);
  client.println(F("</p></div>"));
  client.println(F("</div>"));
}

void HtmlClaveView(WiFiClient client)
{
  client.println(F("<div>"));
  client.println(F("<p>Contraseña</p>"));
  client.println(F("<p style='display: inline-block; border: 2px solid black; border-radius: 10px; background-color: #fff; color: black;padding: 5px; width: 100px; min-height: 25px;'>"));
  client.println(password);
  client.println(F("</p>"));
  client.println(F("</div>"));
}

void HtmlStatus(WiFiClient client, String title, bool sensor)
{
  String colorOn = "#00ff00";
  String colorOff = "#ff0000";
  client.println(F("<div>"));
  client.println(F("<p>"));
  client.print(title);
  client.println(F("</p>"));
  client.println(F("<p style='display: inline-block; border: 2px solid black; border-radius: 10px; padding: 5px; width:100px; color: white; background-color:"));
  client.print(sensor ? colorOn : colorOff);
  client.println(F(";'>"));
  client.print(sensor ? "on" : "off");
  client.println(F("</p>"));
  client.println(F("</div>"));
}

void HtmlTableUserRow(WiFiClient client, User user)
{
  client.println(F("<tr style='background-color: darkblue; text-align: center; font-size: 25px;'>"));
  client.println(F("<td>"));
  client.println(user.id);
  client.println(F("</td>"));
  client.println(F("<td>"));
  client.println(user.habitacion);
  client.println(F("</td>"));
  client.println(F("<td>"));
  client.println(user.ingreso);
  client.println(F("</td>"));
  client.println(F("<td>"));
  client.println(user.egreso);
  client.println(F("</td>"));
  client.println(F("<td>"));
  client.println(user.id);
  client.println(F("</td>"));
  client.println(F("<td>"));
  client.println(user.id);
  client.println(F("</td>"));
  client.println(F("<td>"));
  client.println(user.id);
  client.println(F("</td>"));
  client.println(F("</tr>"));
}

void HtmlTableUsersSession(WiFiClient client)
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
  for (User session : sessions)
  {
    //Si existe usuarios logeados. Muestro su informacion en la table.
    if (session.login)
    {
      HtmlTableUserRow(client, session);
    }
  }
  client.println(F("</tbody>"));
  client.println(F("</table>"));
}