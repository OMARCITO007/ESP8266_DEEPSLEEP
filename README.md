# ESP8266_DEEPSLEEP
DEEP SLEEP Y WIFI MANAGER

if (digitalRead(MC) == 0) { modoconf(); }//LEE EL ESTADO DEL PIN GPIO0, CUANDO ES PRESIONADO INGRESA
AL MODO CONFIGURACIÓN INICIANDO EL WEB SERVER ESPMODO AP.
const char *ssidConf = "ESP_CONF";//NOMBRE DE RED DEL MODO CONFIGURACIÓN
const char *passConf = "123456789";//CONTRASEÑA DE WIFI
LA PUERTA DE ENLACE ES POR DEFECTO DEL ESP8266 192.168.4.1