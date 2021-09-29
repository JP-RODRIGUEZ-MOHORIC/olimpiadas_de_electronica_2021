#include <SPI.h>
#include <Ethernet.h>
#include <Keypad.h>     // importa libreria Keypad

#define DEBUG 0

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 10, 177);
EthernetServer server(80);

//Variables del sensor de CO2
int CO2;
int mq135 = A0;
bool ventilacion;
#define CO2_LIMITE 500

//variables del keypad

const byte FILAS = 4;     // define numero de filas
const byte COLUMNAS = 4;    // define numero de columnas
char keys[FILAS][COLUMNAS] = {    // define la distribucion de teclas
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte pinesFilas[FILAS] = {9,8,7,6};   // pines correspondientes a las filas
byte pinesColumnas[COLUMNAS] = {5,4,3,2}; // pines correspondientes a las columnas

Keypad teclado = Keypad(makeKeymap(keys), pinesFilas, pinesColumnas, FILAS, COLUMNAS);  // crea objeto

char TECLA;       // almacena la tecla presionada
char CLAVE[7];        // almacena en un array 6 digitos ingresados
int comprobacion_clave = 0;

char CLAVE_MAESTRA_1[7] = "147147";   // almacena en un array la contraseña maestra
char CLAVE_MAESTRA_2[7] = "123456";
char CLAVE_MAESTRA_3[7] = "123456";
char CLAVE_MAESTRA_4[7] = "123456";
char CLAVE_MAESTRA_5[7] = "123456";
char CLAVE_MAESTRA_6[7] = "123456";

byte INDICE = 0;      // indice del array

//Estrusctura de datos
struct ingresantes{
  int id;
  char usuario[20];
  unsigned long int hora-ingreso;
  unsigned long int hora-egreso;
  char habitacion[20];
  int estado-sistema;
  int estado-bus;
} tabla[6];

void setup()
{
  Serial.begin(9600);

  Ethernet.begin(mac, ip);
  server.begin();

   if(DEBUG){
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
   }
}

void loop()
{
  //Codigo deteccion del sensor
  CO2 = analogRead(mq135); //Lemos la salida analógica del MQ
  float voltaje = CO2 * (5.0 / 1023.0); //Convertimos la lectura en un valor de voltaje
  if(DEBUG){
  Serial.print("adc:");
  Serial.print(CO2);
  Serial.print("    voltaje:");
  Serial.println(voltaje);
  delay(1000);
  }
  
//  if(CO2 >= 500){
//    ventilacion = 1;
//  }else if(CO2 < 500){
//    ventilacion = 0;
//  }

   ventilacion = CO2 >= CO2_LIMITE; //si la condición es verdadera, el resultado de ventilacion será true, caso contrario false.
  
  if(DEBUG){
  Serial.println(ventilacion);
  }
  
  //codigo keypad
  TECLA = teclado.getKey();   // obtiene tecla presionada y asigna a variable
  if (TECLA)        // comprueba que se haya presionado una tecla
  {
    CLAVE[INDICE] = TECLA;    // almacena en array la tecla presionada
    INDICE++;       // incrementa indice en uno
    Serial.print(TECLA);    // envia a monitor serial la tecla presionada
  }

  if(INDICE == 6)       // si ya se almacenaron los 6 digitos
  {
    if(!strcmp(CLAVE, CLAVE_MAESTRA_1)){   // compara clave ingresada con clave maestra
      Serial.println(" Correcta"); // imprime en monitor serial que es correcta la clave
      comprobacion_clave = 1;}
    else{
      Serial.println(" Incorrecta");  // imprime en monitor serial que es incorrecta la clave
      comprobacion_clave = 2;
    }
  }
  if(TECLA == 'A'){
      Serial.println(" ");
      Serial.println("Ingresar Contraseña");
      for (int x = 0; x < sizeof(CLAVE) / sizeof(CLAVE[0]); x++)
      {
        CLAVE[x] = 0;
      }
      comprobacion_clave = 0;
      INDICE = 0;
      }
  
  
  EthernetClient client = server.available();
  if (client)
  {
    Serial.println("new client");
    bool currentLineIsBlank = true;
    String cadena = "";
    while (client.connected()) 
    {
      if (client.available()) 
      {
        char c = client.read();
        Serial.write(c);
        
        // Al recibir linea en blanco, servir página a cliente
        if (c == '\n' && currentLineIsBlank)
        {
          client.println(F("HTTP/1.1 200 OK\nContent-Type: text/html"));
          client.println("Conenection: close");
      
          //client.println(F("Refresh:0")); //Refresca la pagina automaticamente cada x tiempo
          String box_css = "display: inline-block; border: 2px solid black; border-radius: 10px; background-color: #fff; color: black;"
          String table_css = "background-color: darkblue; text-align: center; font-size: 25px;"
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
          client.println(F("<div style='"+box_css+"font-size: 50px;  width: 200px; height: 100px;'><p style='margin-top: 18px;'>"));
          client.println(CO2);
          client.println(F("</p></div>"));
          client.println(F("</div>"));
          client.println(F("<div style='display: flex; flex-direction: column; text-align: center; margin-right: auto; margin-left: auto; font-size: 25px; width: 100%; border-left: 2px solid white;  border-bottom: 2px solid white;'>"));
          client.println(F("<div>"));
          client.println(F("<p>Contraseña</p>"));
          client.println(F("<p style='"+box_css+"padding: 5px; width: 100px; min-height: 25px;'>"));
          client.println(CLAVE);
          client.println(F("</p>"));
          client.println(F("</div>"));
          client.println(F("<div>"));
          client.println(F("<p>Datos 2</p>"));
          client.println(F("<p style='+"box_css+"padding: 5px; width: 100px;'>-</p>"));
          client.println(F("</div>"));

//          if (ventilacion == 0){
//          client.println(F("<div>"));
//          client.println(F("<p>Ventiladores</p>"));
//          client.println(F("<p style='display: inline-block; border: 2px solid black; border-radius: 10px; padding: 5px; width: 100px; color: black; background-color: #fff;'>off</p>"));
//          client.println(F("</div>"));
//          }         
//          if (ventilacion == 1){
//          client.println(F("<div>"));
//          client.println(F("<p>Ventiladores</p>"));
//          client.println(F("<p style='display: inline-block; border: 2px solid black; border-radius: 10px; padding: 5px; width: 100px; color: black; background-color: #fff;'>on</p>"));
//          client.println(F("</div>"));
//          }
          
          //con operador ternario.
          client.println(F("<div>"));
          client.println(F("<p>Ventiladores</p>"));
          client.println(F("<p style='display: inline-block; border: 2px solid black; border-radius: 10px; padding: 5px; width: 100px; color: black; background-color: #fff;'>"));
          client.println(F(ventilacion ? "on" : "off");
          client.println(F("</p>");
          client.println(F("</div>"));
          client.println(F("</div>"));
          client.println(F("<a href='http://192.168.10.177/' style='display: inline-block; text-decoration: none; color: white; font-size: 20px; padding: 20px; border: 2px solid white;  margin-top: 15px; width:97%; text-align: center; background-color: black;'>Refrescar</a>"));
          
          if(comprobacion_clave == 0){ 
          client.println(F("<h1>Ingresar contraseña<h1>"));
          }
          if(comprobacion_clave == 1){ 
          client.println(F("<h1>Contraseña Correcta, Bienvenido<h1>"));
          client.println(F("<h1 style='text-align: center;'>Panel de Control</h1>"));
          client.println(F("<table style='margin: auto; margin-top: 10px; background-color: #fff; text-align: center;height: 500px; width: 750px; border: 2px solid #fff; border-top: none;'>"));
          client.println(F("<caption style='"+table_css+"border: solid 4px #fff; border-bottom: none;'>Tabla de Datos</caption>"));
          client.println(F("<thead style="table_css">"));
          client.println(F("<th>ID</th>"));
          client.println(F("<th>Nombre</th>"));
          client.println(F("<th>Ingreso</th>"));
          client.println(F("<th>Egreso</th>"));
          client.println(F("<th>Ubicación</th>"));
          client.println(F("<th>Estado del sistema</th>"));
          client.println(F("<th>Estado del Bus</th>"));
          client.println(F("</thead>"));
          client.println(F("<tbody style='text-align: center;'>"));
          client.println(F("<tr style="table_css">"));
          client.println(F("<td>dato</td>"));
          client.println(F("<td>dato</td>"));
          client.println(F("<td>dato</td>"));
          client.println(F("<td>dato</td>"));
          client.println(F("<td>dato</td>"));
          client.println(F("<td>dato</td>"));
          client.println(F("<td>dato</td>"));
          client.println(F("</tr>"));
          client.println(F("<tr style="table_css">"));
          client.println(F("<td>dato</td>"));
          client.println(F("<td>dato</td>"));
          client.println(F("<td>dato</td>"));
          client.println(F("<td>dato</td>"));
          client.println(F("<td>dato</td>"));
          client.println(F("<td>dato</td>"));
          client.println(F("<td>dato</td>"));
          client.println(F("</tr>"));
          client.println(F("<tr style="table_css">"));
          client.println(F("<td>dato</td>"));
          client.println(F("<td>dato</td>"));
          client.println(F("<td>dato</td>"));
          client.println(F("<td>dato</td>"));
          client.println(F("<td>dato</td>"));
          client.println(F("<td>dato</td>"));
          client.println(F("<td>dato</td>"));
          client.println(F("</tr>"));
          client.println(F("</tbody>"));
          client.println(F("</table>"));
          client.println(F("<a href='#' download style='width: 50px; height: 50px;margin-left: 290px; font-size: 30px; color: #fff;'>Descargar datos de la tabla</a>"));
          
          }
          if(comprobacion_clave == 2){ 
          client.println(F("<h1>Contraseña Incorrecta<h1>"));
          }
          
          client.println(F("</body>"));
          client.println(F("</html>"));
          break;
          }
        if (c == '\n') 
        {
          currentLineIsBlank = true;
        }
        else if (c != '\r') 
        {
          currentLineIsBlank = false;
        }
      }
    }
    delay(1);
    client.stop();
  }
}
