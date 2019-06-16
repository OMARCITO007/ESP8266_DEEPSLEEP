#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <EEPROM.h>

#define SW1 4
#define SW2 5
#define SW3 12
#define MC 0
//-------------------VARIABLES GLOBALES--------------------------
int contconexion = 0;

char ssid[50];      
char pass[50];

const char *ssidConf = "ESP_CONF";//NOMBRE DE RED DEL MODO CONFIGURACIÓN
const char *passConf = "123456789";//CONTRASEÑA DE WIF

String mensaje = "";

char *host = "http://api.app.com";
String Dservidor = "http://api.app.com/api/test";
String chipid = "";
String DATOS; //ALMACENA TODOS LOS DATOS A ENVIAR

//-----------CODIGO HTML PAGINA DE CONFIGURACION---------------
String pagina = "<!DOCTYPE html>"
"<html>"
"<head>"
"<title>ESP CONF</title>"
"<meta charset='UTF-8'>"
"</head>"
"<body>"
"</form>"
"<form action='guardar_conf' method='get'>"
"<br>NOMBRE DE RED:<br>"
"<input class='input1' name='ssid' type='text'><br>"
"<br>CONTRASEÑA:<br>"
"<input class='input1' name='pass' type='password'><br><br>"
"<input class='boton' type='submit' value='GUARDAR'/><br><br>"
"</form>"
"<a href='escanear'><button class='boton'>ESCANEAR REDES</button></a><br><br>";

String paginafin = "</body>"
"</html>";


//------------------------SETUP WIFI MODE-----------------------------
void setup_wifi() {
// Conexión WIFI
  WiFi.mode(WIFI_OFF);        //Evita problemas de reconexión (tarda demasiado en conectarse)****
  delay(1000);
  WiFi.mode(WIFI_STA); //para que no inicie el SoftAP en el modo normal
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED and contconexion <50) { //Cuenta hasta 50 si no se puede conectar lo cancela
    ++contconexion;
    delay(250);
    Serial.print(".");
    digitalWrite(13, HIGH);
    delay(250);
    digitalWrite(13, LOW);
  }
  if (contconexion <50) {   
      Serial.println("");
      Serial.println("WiFi conectado");
      Serial.println(WiFi.localIP());
      digitalWrite(13, HIGH);  
  }
  else { 
      Serial.println("");
      Serial.println("Error de conexion");
      digitalWrite(13, LOW);
  }
}

//--------------------------------------------------------------
WiFiClient espClient;
ESP8266WebServer server(80);
//--------------------------------------------------------------

//-------------------PAGINA DE CONFIGURACION--------------------
void paginaconf() {
  server.send(200, "text/html", pagina + mensaje + paginafin); 
}

//--------------------MODO_CONFIGURACION------------------------
void modoconf() {
   
  delay(50);
  digitalWrite(13, HIGH);
  delay(50);
  digitalWrite(13, LOW);
  delay(50);
  digitalWrite(13, HIGH);
  delay(50);
  digitalWrite(13, LOW);

  WiFi.softAP(ssidConf, passConf);
  IPAddress myIP = WiFi.softAPIP(); 
  Serial.print("IP del acces point: ");
  Serial.println(myIP);
  Serial.println("WebServer iniciado...");

  server.on("/", paginaconf); //esta es la pagina de configuracion

  server.on("/guardar_conf", guardar_conf); //Graba en la eeprom la configuracion

  server.on("/escanear", escanear); //Escanean las redes wifi disponibles
  
  server.begin();

  while (true) {
      server.handleClient();
  }
}

//---------------------GUARDAR CONFIGURACION-------------------------
void guardar_conf() {
  
  Serial.println(server.arg("ssid"));//Recibimos los valores que envia por GET el formulario web
  grabar(0,server.arg("ssid"));
  Serial.println(server.arg("pass"));
  grabar(50,server.arg("pass"));

  mensaje = "Configuracion Guardada...";
  paginaconf();
}

//----------------Función para grabar en la EEPROM-------------------
void grabar(int addr, String a) {
  int tamano = a.length(); 
  char inchar[50]; 
  a.toCharArray(inchar, tamano+1);
  for (int i = 0; i < tamano; i++) {
    EEPROM.write(addr+i, inchar[i]);
  }
  for (int i = tamano; i < 50; i++) {
    EEPROM.write(addr+i, 255);
  }
  EEPROM.commit();
}

//-----------------Función para leer la EEPROM------------------------
String leer(int addr) {
   byte lectura;
   String strlectura;
   for (int i = addr; i < addr+50; i++) {
      lectura = EEPROM.read(i);
      if (lectura != 255) {
        strlectura += (char)lectura;
      }
   }
   return strlectura;
}

//---------------------------ESCANEAR REDES----------------------------
void escanear() {  
  int n = WiFi.scanNetworks(); //devuelve el número de redes encontradas
  Serial.println("ESCANEO TERMINADO");
  if (n == 0) { //si no encuentra ninguna red
    Serial.println("NO SE ENCONTRARON REDES");
    mensaje = "no se encontraron redes";
  }  
  else
  {
    Serial.print(n);
    Serial.println("REDES ENCOTRADAS");
    mensaje = "";
    for (int i = 0; i < n; ++i)
    {
      // agrega al STRING "mensaje" la información de las redes encontradas 
      mensaje = (mensaje) + "<p>" + String(i + 1) + ": " + WiFi.SSID(i) + " (" + WiFi.RSSI(i) + ") Ch: " + WiFi.channel(i) + " Enc: " + WiFi.encryptionType(i) + " </p>\r\n";
      //WiFi.encryptionType 5:WEP 2:WPA/PSK 4:WPA2/PSK 7:open network 8:WPA/WPA2/PSK
      delay(10);
    }
    Serial.println(mensaje);
    paginaconf();
  }
}

void enviardatos()
  {
    HTTPClient http;    // objeto de la clase HTTPClient
    http.begin(Dservidor);              //Specify request destination
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");    //Specify content-type header
    int httpCode = http.POST(DATOS);   //Send the request
    String payload = http.getString();    //Get the response payload
    Serial.println(httpCode);   //Print HTTP return code
  Serial.println(payload);    //Print request response payload
  http.end();  //Close connection
  }

//------------------------SETUP-----------------------------
void setup()
{	
  pinMode(13, OUTPUT); // D7 
  pinMode(SW1, INPUT_PULLUP);
  pinMode(SW2, INPUT_PULLUP);
  pinMode(SW3, INPUT_PULLUP);  
  // Inicia Serial
  Serial.begin(115200);
  Serial.println("");

  Serial.print("chipId: "); 
  chipid = String(ESP.getChipId());
  Serial.println(chipid);

  EEPROM.begin(512);
  leer(0).toCharArray(ssid, 50);
  leer(50).toCharArray(pass, 50);

  setup_wifi();
  }



  void loop()
 {

  if (digitalRead(MC) == 0) { modoconf(); }//LEE EL ESTADO DEL PIN GPIO0

 int V_SW1 = digitalRead(SW1);//LECTURA ESTADO PULSADOR
 int V_SW2 = digitalRead(SW2);
 int V_SW3 = digitalRead(SW3);

 if(V_SW1 == LOW){
    Serial.println("BOTON1"); 
    DATOS = "sw_id=" + String(1) + "&id_mod=" + chipid ;
    enviardatos();
    delay(600); ESP.deepSleep(0);}

  if(V_SW2 == LOW){
    Serial.println("BOTON2");  
    DATOS = "sw_id=" + String(2) + "&id_mod=" + chipid ;
    enviardatos();
    delay(600); ESP.deepSleep(0);}

  if(V_SW3 == LOW){
    Serial.println("BOTON3");  
    DATOS = "sw_id=" + String(3) + "&id_mod=" + chipid ;
    enviardatos();
    delay(600); ESP.deepSleep(0);}



  }

  
  