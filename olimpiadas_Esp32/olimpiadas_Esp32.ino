#include <ESP32Time.h>   // Load Esp32 RTC libbrary
#include <analogWrite.h> // Load Esp32 analogwrite library
#include <Keypad.h>      // Load Keypad library
#include <WiFi.h>        // Load Wi-Fi library
#define DEBUG 0
#define CONECTION 1
#define PORT 80

// Replace with your network credentials
const char *ssid = "Leo mar juan";
const char *WiFi_password = "IVOM8264";

// Set web server port number to 80
WiFiServer server(PORT);

// Variable to store the HTTP request
String header;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0;
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

//RTC
ESP32Time rtc;
String str_now;

//CO2 Sensor variables
#define THRSH 600      //750
#define THRSL 500      //500
#define THRS_ALARM 750 //900
#define MQ135 35
#define ON true
#define OFF false
int CO2_Status;
int CO2;
int CO2_final;
bool ventilation_status = false;
bool alarm_status = false;

//Finite-state Machine variables
#define STDBY 1
#define EXTRACCION 2
#define ALARM 3

//Motors
#define MOTOR_IN 2
#define MOTOR_OUT 4

//Buzzer
#define BUZZER 15

//RGB Led
#define PIN_RED 19
#define PIN_GREEN 18
#define PIN_BLUE 5

//Keypad variables
#define RESET 0
#define CORRECT 1
#define INCORRECT 2

const byte ROW = 4;        // Defines rows
const byte COLUMN = 4;     // Defines columns
char keys[ROW][COLUMN] = { // Defines keys distribution
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};

byte rowpins[ROW] = {13, 12, 14, 27};       // Row pins
byte columnpins[COLUMN] = {26, 25, 33, 32}; // Column pins

Keypad keypad = Keypad(makeKeymap(keys), rowpins, columnpins, ROW, COLUMN); // Creates an object

char key;         // Stores the pressed key
char password[7]; // Stores in an array the 6 keys pressed
int key_comprobation = 0;
char master_key_1[7] = "147147"; // Stores in an array the correct password 1
char master_key_2[7] = "369369"; // Stores in an array the correct password 2
char master_key_3[7] = "912018"; // Stores in an array the correct password 3
char master_key_4[7] = "266011"; // Stores in an array the correct password 4
char master_key_5[7] = "503282"; // Stores in an array the correct password 5
char master_key_6[7] = "123456"; // Stores in an array the correct password 6

int index_keypad = 0; // Array index

//Table structure
struct User
{
  int id;
  String entrance;
  int CO2_level;
  String system_status;
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
  user.entrance = str_now;
  user.CO2_level = CO2_final;
  user.system_status = "OK";
  return user;
}

void setup()
{

  Serial.begin(115200);
  if (CONECTION)
  {
    //Connect to Wi-Fi network with SSID and password
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, WiFi_password);
    while (WiFi.status() != WL_CONNECTED)
    {
      delay(500);
      Serial.print(".");
    }

    //Print local IP address and start web server
    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    server.begin();
  }

  //Set RGB led pins as OUTPUTS
  pinMode(PIN_RED, OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);
  pinMode(PIN_BLUE, OUTPUT);

  //Starts Finite-state machine
  green_led();
  CO2_Status = STDBY;

  //Set Motors as OUTPUTS
  pinMode(MOTOR_IN, OUTPUT);
  pinMode(MOTOR_OUT, OUTPUT);

  //Set buzzer pin as OUTPUT
  pinMode(BUZZER, OUTPUT);

  //Generate 6 users
  user_1 = GenerateUser(1);
  user_2 = GenerateUser(2);
  user_3 = GenerateUser(3);
  user_4 = GenerateUser(4);
  user_5 = GenerateUser(5);
  user_6 = GenerateUser(6);

  //Starts RTC
  RtcInit();
}

void loop()
{

  Rtc();
  Mef_CO2();
  KeypadRead();
  Server();
}

void RtcInit()
{
  String months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
  int mm = 1;

  int hh = String(__TIME__).substring(0, 2).toInt();
  int mn = String(__TIME__).substring(3, 5).toInt();
  int dd = String(__DATE__).substring(4, 6).toInt();
  String mm_aux = String(__DATE__).substring(0, 3);
  int yy = String(__DATE__).substring(7, 11).toInt();

  for (int x = 0; x < 12; x++)
  {
    if (months[x] == mm_aux)
    {
      mm = x + 1;
      break;
    }
  }
  rtc.setTime(00, mn, hh, dd, mm, yy); // (seconds, minutes, hours, days, months, years)
}

void Rtc()
{
  str_now = rtc.getTime("%d/%m/%Y %H:%M:%S ");
  Serial.println(str_now);
}

void LeerSensor()
{
  //Sensor code
  CO2 = analogRead(MQ135); //Read the sensor´s analogic pin
  CO2_final = map(CO2, 0, 4095, 0, 1023);
  float voltaje = CO2_final * (5.0 / 1023.0); //Gets the equivalent in volts
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
    { 
      //Evaluates if CO2 level its over the limit
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
    { 
      //Evaluates if CO2 level is lower than the limit and sets the FSM in STDBY mode. 
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
  analogWrite(PIN_RED, 0);     
  analogWrite(PIN_GREEN, 255); 
  analogWrite(PIN_BLUE, 255);
}

void yellow_led()
{
  analogWrite(PIN_RED, 0); 
  analogWrite(PIN_GREEN, 207);
  analogWrite(PIN_BLUE, 255);
}

void KeypadRead()
{
  //keypad code
  key = keypad.getKey(); //Obtains the key pressed and asigns a variable
  if (key)               //Checks that a key was pressed
  {
    password[index_keypad] = key; //Stores in the array the key pressed
    index_keypad++;               //Increases the index + 1
    Serial.print(key);            //Sends to the serial monitor the key pressed
  }

  if (index_keypad == 6) //Evaluates if there are 6 keys stored
  {
    Autentification();
  }
  Logout();
}

void Autentification()
{
  if (!strcmp(password, master_key_1) or !strcmp(password, master_key_2) or !strcmp(password, master_key_3) or !strcmp(password, master_key_4) or !strcmp(password, master_key_5) or !strcmp(password, master_key_6))
  { //Compares the password whith the correct passwords

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
    session.CO2_level = CO2_final;
    session.entrance = str_now;
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
  if (key == 'A') //"A" Key restart the count and forces to type the password again
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
  //Server
  WiFiClient client = server.available(); //Listen for incoming clients

  if (client)
  { // //If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    //  Serial.println("New Client."); //print a message out in the serial port
    String currentLine = ""; //make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime)
    { //loop while the client's connected
      currentTime = millis();
      if (client.available())
      {                         //if there's bytes to read from the client,
        char c = client.read(); //read a byte, then
        Serial.write(c);        //print it out the serial monitor
        header += c;
        if (c == '\n')
        { //if the byte is a newline character
          //if the current line is blank, you got two newline characters in a row.
          //that's the end of the client HTTP request, so send a response:
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

            //Table
            if (key_comprobation == CORRECT)
            {
              HtmlTableUsersSession(client);
              HtmlTableDownload(client);
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
          { //If you got a newline, then clear currentLine
            currentLine = "";
          }
        }
        else if (c != '\r')
        {                   //If you got anything else but a carriage return character,
          currentLine += c; //Add it to the end of the currentLine
        }
      }
    }
    //Clear the header variable
    header = "";
    //Close the connection
    client.stop();
    //Serial.println("Client disconnected.");
    //Serial.println("");
  }
}

void WebRefresh(WiFiClient client)
{
  client.println(F("<script>"));
  HtmlScriptDownloadTable(client);
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
  client.println(user.entrance);
  client.println(F("</td>"));
  client.println(F("<td>"));
  client.println(user.CO2_level);
  client.println(F("</td>"));
  client.println(F("<td>"));
  client.println(user.system_status);
  client.println(F("</td>"));
  client.println(F("</tr>"));
}

void HtmlTableUsersSession(WiFiClient client)
{

  client.print(F(
      "    <h1>Contraseña Correcta, Bienvenido</h1>"
      "    <h1 style='text-align: center'>Panel de Control</h1>"
      "    <div id='table_element'>"
      "      <table"
      "        style='"
      "          margin: auto;"
      "          margin-top: 10px;"
      "          background-color: #fff;"
      "          text-align: center;"
      "          height: 500px;"
      "          width: 900px;"
      "          border: 2px solid #fff;"
      "          border-top: none;"
      "        '"
      "      >"
      "        <caption"
      "          style='"
      "            background-color: darkblue;"
      "            text-align: center;"
      "            font-size: 25px;"
      "            border: solid 4px #fff;"
      "            border-bottom: none;"
      "          '"
      "        >"
      "          Tabla de Datos"
      "        </caption>"
      "        <thead"
      "          style='"
      "            background-color: darkblue;"
      "            text-align: center;"
      "            font-size: 25px;"
      "          '"
      "        >"
      "          <th>ID</th>"
      "          <th>Ingreso</th>"
      "          <th>Nivel Co2</th>"
      "          <th>Estado del sistema</th>"
      "        </thead>"
      "        <tbody style='text-align: center'>"));
  for (User session : sessions)
  {
    //Si existe usuarios logeados. Muestro su informacion en la table.
    if (session.login)
    {
      HtmlTableUserRow(client, session);
    }
  }
  client.print(F("</tbody>"
                 "      </table>"
                 "    </div>"));
}

void HtmlScriptDownloadTable(WiFiClient client)
{
  client.println(F(
      "function downloadTable(){"
      "   const tableElement = document.getElementById('table_element').innerHTML;"
      "   const bodyElement = document.body.innerHTML;"
      "   document.body.innerHTML = tableElement;"
      "   window.print();"
      "   document.body.innerHTML = bodyElement;"
      "}"));
}

void HtmlTableDownload(WiFiClient client)
{
  client.println(F(
      "<a"
      "  onclick='downloadTable()'"
      "  style='"
      "    width: 50px;"
      "    height: 50px;"
      "    margin-left: 290px;"
      "    font-size: 30px;"
      "    color: #fff;"
      "    cursor: pointer;"
      "  '>"
      "  Descargar datos de la tabla"
      "</a>"));
}
