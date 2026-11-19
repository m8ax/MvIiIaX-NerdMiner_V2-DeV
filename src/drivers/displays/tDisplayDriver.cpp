/********************************************************************************************
 *
 *   Escrito por: M8AX
 *
 *   Descripción:
 *   ------------
 *
 *   Expansión de las pantallas del NerdMinerV2 con aún más datos y mucho más "nerd"
 *   de lo que ya era xD... Optimizada Para Minar A 255 KH/s Con Pantalla.
 *
 *   Se añaden más funciones y datos para que puedas ver en tiempo real lo que está
 *   ocurriendo en tu minero, además de poder enviar mensajes a tu canal de telegram.
 *
 *   Se añaden más funciones para que puedas ver datos en pantalla y enviarlos a tu
 *   canal de telegram, como la factorización prima de un número aleatorio, el día de
 *   la semana, la hora, la fecha, la IP pública, el precio de Bitcoin, el promedio por
 *   transacción, la altura de bloque, el hash rate global, el tiempo minando, la mejor
 *   dificultad alcanzada, la dificultad de la red, el cómputo total, el hash rate actual,
 *   la temperatura de la CPU, las plantillas de bloque, los shares enviados a la pool, En
 *   resumen, un montón de datos que podrás ver en pantalla y enviar a tu canal de telegram
 *   si lo has configurado, sino, no te preocupes, no pasa nada, todo seguirá funcionando y
 *   minando como siempre.
 *
 *   Es **necesario** que durante la **primera configuración del minero** se introduzca correctamente
 *   la zona horaria (timezone). Este valor es esencial para inicializar correctamente la lógica de
 *   sincronización y control horario del sistema.
 *
 *   Esto asegura que, tras la configuración inicial correcta, el sistema mantenga siempre la hora local
 *   precisa sin necesidad de intervención manual.
 *
 *   Posteriormente, la zona horaria se actualizará automáticamente mediante la IP pública del dispositivo,
 *   ajustándose de forma dinámica a la ubicación real del usuario, incluyendo también el cambio automático
 *   por horario de verano (si aplica).
 *
 *   Cuando se ajusta la hora por horario de verano o invierno, el sistema puede tardar en actualizar la hora
 *   en pantalla un máximo de 5 horas, el tiempo establecido entre sincronizaciones. Esto se debe a que el
 *   sistema no puede sincronizar la hora de forma continua, ya que esto podría causar problemas de rendimiento.
 *   Asi que si llega el día de cambio de hora que es de madrugada y no cambia al instante no te preocupes
 *   que el sistema lo hará automáticamente en la siguiente sincronización.
 *
 *
 *           Un minero de Bitcoin es un dispositivo o software que realiza cálculos
 *           matemáticos complejos para verificar y validar transacciones en la red.
 *           Los mineros compiten para resolver estos problemas y añadir un bloque
 *           a la cadena. A cambio, reciben bitcoins recién creados como recompensa.
 *
 *
 *                              PARA MÁS INFORMACIÓN LEER PDF
 *
 *                     Tmp. De Programación 24H - 12175 Líneas De Código
 *                     -------------------------------------------------
 *
 ********************************************************************************************/

// Invocando las poderosas librerías que hacen posible esta obra maestra del minado nerd

#include "displayDriver.h"
#ifdef T_DISPLAY
#include <TFT_eSPI.h>
#include "media/images_320_170.h"
#include "media/myFonts.h"
#include "media/Free_Fonts.h"
#include "version.h"
#include <inttypes.h>
#include "moonPhase.h"
#include <FS.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <HTTPClient.h>
#include <SPIFFS.h>
#include <Arduino.h>
#include <stdint.h>
#include <qrcode.h>
#include <urlencode.h>
#include <limits.h>
#include <stdio.h>
#include "monitor.h"
#include <math.h>
#include "time.h"
#include <SPI.h>
#include <ArduinoJson.h>
#include <esp_system.h>
#include <stdlib.h>
#include "OpenFontRender.h"
#include "rotation.h"
#include <string>
#include <ctime>
#include <clientntp.h>
#include "drivers/storage/storage.h"
#include "drivers/devices/device.h"
#include "drivers/storage/nvMemory.h"

// Variables externas que cruzan fronteras en el código para hacer magia

extern TSettings Settings;
extern nvMemory nvMem;

// Definiciones que configuran el cerebro del sistema

#define WIDTH 340
#define HEIGHT 170
#define MIN_KH 0
#define MAX_KH 360
#define MAX_RESULT_LENGTH 500
#define MAX_NUMBERS 6
#define MAX_DEPTH 4
#define MAX_GEN_COUNT 1000
#define GRIDX 320
#define GRIDY 170
#define CELLXY 2
#define GEN_DELAY 1

// Declaración de variables globales

OpenFontRender render;
TFT_eSPI tft = TFT_eSPI();
TFT_eSprite background = TFT_eSprite(&tft);
uint8_t grid[GRIDX][GRIDY], newgrid[GRIDX][GRIDY];
uint16_t colors[] = {TFT_WHITE, TFT_RED, TFT_GREEN, TFT_BLUE, TFT_YELLOW, TFT_CYAN, TFT_MAGENTA, TFT_ORANGE, TFT_GREENYELLOW, TFT_PINK, TFT_LIGHTGREY, TFT_SKYBLUE, TFT_OLIVE, TFT_GOLD, TFT_SILVER};
uint16_t coloris[] = {TFT_WHITE, TFT_YELLOW, TFT_CYAN, TFT_GREENYELLOW, TFT_LIGHTGREY, TFT_BLACK, TFT_ORANGE, TFT_GOLD, TFT_SILVER}, genCount = 0;
int columna = 0, secondCounter = 0, random_number = 1, limite = 0, aciertos = 0, fallos = 0, totalci = 0, colorIndex = 0, colorI = 0;
int mirarTiempo = 0, sumatele = 1, abortar = 0, alertatemp = 0, maxtemp = 0, mintemp = 1000, diadecambios, zonilla, colorrrr = esp_random() % 9;
float anterBTC = 0.0, maxkh = 0.00, minkh = 1000.00, porcentaje = 0.00;
const char *nombrecillo;
const char *apiUrl = "http://ip-api.com/json/";
const char *serverName = "https://dummyjson.com/quotes/random";

// Criptomonedas De Binance, Todas De Golpe

#define NUM_CRIPTOS 20

struct CriptoInfo
{
  const char *simbolo;
  const char *nombre;
  float precio;
};

CriptoInfo criptomonedas[NUM_CRIPTOS] = {
    {"BTCUSDT", "BTC", 0},
    {"ETHUSDT", "ETH", 0},
    {"XRPUSDT", "XRP", 0},
    {"ADAUSDT", "ADA", 0},
    {"SOLUSDT", "SOL", 0},
    {"BNBUSDT", "BNB", 0},
    {"DOGEUSDT", "DOGE", 0},
    {"TRXUSDT", "TRX", 0},
    {"AVAXUSDT", "AVAX", 0},
    {"LINKUSDT", "LINK", 0},
    {"SUIUSDT", "SUI", 0},
    {"DOTUSDT", "DOT", 0},
    {"LTCUSDT", "LTC", 0},
    {"BCHUSDT", "BCH", 0},
    {"XLMUSDT", "XLM", 0},
    {"UNIUSDT", "UNI", 0},
    {"NEARUSDT", "NEAR", 0},
    {"ATOMUSDT", "ATOM", 0},
    {"AAVEUSDT", "AAVE", 0},
    {"FILUSDT", "FIL", 0}};
const char *ciudades[] = {
    "Nueva York", "Londres", "Paris", "Tokio", "Sidney",
    "Los Angeles", "Beijing", "Moscu", "Delhi", "Buenos Aires",
    "Berlin", "Mexico", "Madrid", "Seul", "Roma",
    "El Cairo", "Amsterdam", "Toronto", "Sao Paulo", "Cape Town"};
const char *urlsm8ax[] = {
    "YT - https://youtube.com/m8ax",
    "OS - https://opensea.io/es/m8ax",
    "OC - https://oncyber.io/m8ax",
    "FW - https://m8ax.github.io/MvIiIaX-NerdMiner_V2-DeV/",
    "GH - https://github.com/m8ax"};
String urls[] = {
    "https://cointelegraph.com/rss",
    "https://cointelegraph.com/rss/tag/altcoins",
    "https://cointelegraph.com/rss/tag/defi",
    "https://cointelegraph.com/rss/tag/regulation",
    "https://cointelegraph.com/rss/tag/bitcoin",
    "https://cointelegraph.com/rss/tag/blockchain",
    "https://cointelegraph.com/rss/tag/ethereum",
    "https://cointelegraph.com/rss/tag/nft",
    "https://cointelegraph.com/rss/tag/stablecoin"};
int zonasHorarias[] = {
    -5, 0, 1, 9, 11, // Nueva York, Londres, Paris, Tokio, Sidney
    -8, 8, 3, 5, -3, // Los Angeles, Beijing, Moscu, Delhi, Buenos Aires
    1, -6, 1, 9, 1,  // Berlin, Mexico, Madrid, Seul, Roma
    2, 1, -5, -3, 2  // El Cairo, Amsterdam, Toronto, Sao Paulo, Cape Town
};
bool mensajeEnviado = false;
char result[MAX_RESULT_LENGTH];
String textoFinalm8ax1, textoFinalm8ax2, textoFinalm8ax3, textoFinalm8ax4, cadenanoti = "", ciudad = "", tempciudad = "", BOT_TOKEN, CHAT_ID, subebaja = ". ESPERANDO .";
uint32_t rndnumero = 0, rndnumero2 = 0, actualizarcalen = 0, actuanot = 0, actualizarc = 0, actual = 0, correccion = 0, numfrases = 0, numnotis = 0, ContadorEspecial = 0, uncontadormas = 0;
WiFiUDP udp;
HTTPClient http;
mining_data mineria;
clock_data relojete;
coin_data monedilla;
moonPhase mymoonPhase;

unsigned long lastTelegramEpochTime = 0;       // Guarda el tiempo de la última ejecución (en segundos desde Epoch)
unsigned long startTime = 0;                   // Para guardar Epoch de inicio
unsigned long arrankito = 0;                   // Para guardar millis al arrancar
const unsigned long interval = 60 * 2 * 60;    // 2 horas en segundos (2 horas * 60 minutos * 60 segundos)
const unsigned long minStartupTime = interval; // Segundos para que no envíe mensaje a telegram si esta configurado, nada más arrancar

template <typename T, typename T2>

inline T map(T2 val, T2 in_min, T2 in_max, T out_min, T out_max)
{
  return (val - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void tDisplay_Init(void)
{

  // Inicializar el pin 15 para habilitar la alimentación externa de 5V (LilyGo bug)

#ifdef PIN_ENABLE5V
  pinMode(PIN_ENABLE5V, OUTPUT);
  digitalWrite(PIN_ENABLE5V, HIGH);
#endif
  tft.init();
#ifdef LILYGO_S3_T_EMBED
  tft.setRotation(ROTATION_270);
#else
  tft.setRotation(ROTATION_90);
#endif
  tft.setSwapBytes(true);
  background.createSprite(WIDTH, HEIGHT);
  background.setSwapBytes(true);
  render.setDrawer(background);
  render.setLineSpaceRatio(0.9);

  // Cargamos fuente y vemos si se puede leer

  if (render.loadFont(DigitalNumbers, sizeof(DigitalNumbers)))
  {
    Serial.println("M8AX - Error En Carga De Fuente...");
    return;
  }
}

// Cambiamos estado de la pantalla al pulsar el botón 1 y abortar=1, para abortar la actualiazación de la pantalla de precios de criptomonedas

void tDisplay_AlternateScreenState(void)
{
  int screen_state = digitalRead(TFT_BL);
  Serial.println("M8AX - Cambiando Estado De La Pantalla");
  digitalWrite(TFT_BL, !screen_state);
  abortar = 1;
}

// Rotamos la pantalla

void tDisplay_AlternateRotation(void)
{
  tft.setRotation(flipRotation(tft.getRotation()));
}

// Funcion para obtener nuestra ip pública

String getPublicIP()
{
  HTTPClient http;
  String publicIP = "";

  http.begin("http://api.ipify.org"); // Servicio que devuelve la IP pública
  int httpCode = http.GET();

  if (httpCode == HTTP_CODE_OK)
  {
    publicIP = http.getString(); // Obtener la respuesta del servidor
  }

  http.end();
  return publicIP;
}

/**
 * Obtiene el desfase horario (UTC offset) de una dirección IP utilizando la API pública ipwho.is.
 * Realiza una solicitud HTTP GET a la URL "http://ipwho.is/[IP]" para recuperar información de
 * geolocalización y extrae el campo "timezone.utc" del JSON devuelto. Devuelve el offset horario
 * como un entero que representa únicamente las horas completas de diferencia respecto a UTC
 * (valores positivos o negativos, sin minutos).
 *
 * Si la conexión WiFi no está disponible, la solicitud HTTP falla o el JSON no puede analizarse,
 * el proceso se reintenta hasta cinco veces con una espera incremental entre intentos.
 *
 * Si tras los cinco intentos no se obtiene un resultado válido, la función devuelve 1000 como
 * código de error. En caso contrario, devuelve el offset horario obtenido y actualiza la variable
 * temporal `offset` partiendo del valor `Settings.Timezone`.
 *
 * @param ip Dirección IP en formato de cadena para la cual se desea determinar la zona horaria.
 * @return Entero representando el desfase horario en horas respecto a UTC, o 1000 si ocurre un error.
 */

int obtenHoraPorIP(const String &ip)
{
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("M8AX - Error: No Hay Conexión WiFi Para Obtener La Zona Horaria\n");
    return 1000;
  }
  String api_url = "http://ipwho.is/" + ip;
  int offset = Settings.Timezone;
  bool success = false;
  for (int intento = 1; intento <= 5; intento++)
  {
    HTTPClient http;
    http.setTimeout(2000);
    http.begin(api_url);
    int httpCode = http.GET();
    if (httpCode == 200)
    {
      StaticJsonDocument<64> filter;
      filter["timezone"]["utc"] = true;
      StaticJsonDocument<128> doc;
      auto error = deserializeJson(doc, http.getStream(), DeserializationOption::Filter(filter));
      http.end();
      if (error)
      {
        Serial.printf("M8AX - Error Al Parsear El JSON ( Intento %d )\n", intento);
        delay(1000 * intento);
        continue;
      }
      const char *utc_offset = doc["timezone"]["utc"];
      if (!utc_offset)
      {
        Serial.printf("M8AX - UTC No Encontrado ( Intento %d )\n", intento);
        delay(1000 * intento);
        continue;
      }
      if ((utc_offset[0] == '+' || utc_offset[0] == '-') &&
          utc_offset[1] >= '0' && utc_offset[1] <= '9' &&
          utc_offset[2] >= '0' && utc_offset[2] <= '9')
      {
        int hours = (utc_offset[1] - '0') * 10 + (utc_offset[2] - '0');
        if (utc_offset[0] == '-')
          hours = -hours;
        offset = hours;
      }
      Serial.printf("M8AX - Zona Horaria Obtenida Correctamente ( Intento %d ): UTC %+d\n", intento, offset);
      success = true;
      break;
    }
    else
    {
      Serial.printf("M8AX - Error HTTP ( %d ) En ( Intento %d )\n", httpCode, intento);
      http.end();
      delay(1000 * intento);
    }
  }
  if (!success)
    Serial.println("M8AX - No Se Pudo Obtener La Zona Horaria Tras ( 5 Intentos )\n");
  return offset;
}

/**
 * Convierte un número `float` en un entero sin signo de 32 bits (`uint32_t`), redondeándolo
 * al entero más cercano.
 *
 * Si el valor es negativo devuelve 0, y si supera el máximo de `uint32_t` devuelve
 * `UINT32_MAX`, de modo que el resultado siempre queda dentro del rango válido.
 */

uint32_t floatToUint32(float num)
{
  if (num < 0)
    return 0;
  if (num > UINT32_MAX)
    return UINT32_MAX;
  return static_cast<uint32_t>(round(num));
}

/**
 * Calcula cuántos días han pasado desde que se compiló el firmware hasta la hora actual.
 *
 * Obtiene la fecha y hora de compilación a partir de las macros `__DATE__` y `__TIME__`,
 * y la compara con la hora actual que da el cliente NTP (`timeClient`). Devuelve la
 * diferencia en días como `float`, con decimales (por ejemplo, 2.5 son dos días y medio).
 *
 * Depende de que el NTP ya se haya sincronizado: si no es así, la hora actual será
 * incorrecta y el resultado saldrá negativo o absurdo.
 */

uint32_t segundosDesdeCompilacion()
{
  struct tm compileTime = {0};
  char fechaHoraCompilacion[25];
  snprintf(fechaHoraCompilacion, sizeof(fechaHoraCompilacion), "%s %s", __DATE__, __TIME__);
  strptime(fechaHoraCompilacion, "%b %d %Y %H:%M:%S", &compileTime);
  time_t compiledEpoch = mktime(&compileTime);
  unsigned long epochTime = timeClient.getEpochTime();
  time_t epoch = (time_t)epochTime;
  struct tm *timeinfo = localtime(&epoch);
  time_t currentEpoch = mktime(timeinfo);
  if (currentEpoch < compiledEpoch)
    return 0;
  return (uint32_t)(currentEpoch - compiledEpoch);
}

/**
 * Convierte una cantidad de segundos en una cadena de texto con formato
 * "DDd HHh MMm SSs" (días, horas, minutos y segundos, cada uno con dos dígitos).
 *
 * Devuelve un puntero a un búfer estático interno de 20 bytes, por lo que el
 * resultado se sobrescribe en cada llamada y la función no es segura entre hilos.
 * Si hay más de 99 días, el campo de días ocupa más de dos dígitos y la cadena
 * se trunca al llegar al límite del búfer.
 */

const char *convertirTiempo(uint32_t segundos)
{
  uint32_t dias, horas, minutos, segs;
  dias = segundos / 86400;
  segundos %= 86400;
  horas = segundos / 3600;
  segundos %= 3600;
  minutos = segundos / 60;
  segs = segundos % 60;
  static char buffer[20];
  snprintf(buffer, sizeof(buffer), "%02lud %02luh %02lum %02lus", (unsigned long)dias, (unsigned long)horas, (unsigned long)minutos, (unsigned long)segs);
  return buffer;
}

/**
 * Ajusta la zona horaria del sistema comparando la configuración actual con la zona horaria
 * obtenida dinámicamente mediante la IP pública. Si la diferencia entre ambas zonas horarias
 * es menor o igual a 1 hora y no son iguales, actualiza la configuración local (`Settings.Timezone`),
 * guarda los cambios en la memoria no volátil y aplica el nuevo desfase horario al cliente NTP.
 *
 * La función asume que `getPublicIP()` retorna la IP pública del dispositivo y que `obtenHoraPorIP()`
 * devuelve el desfase horario correspondiente en horas (UTC offset).
 *
 * No realiza ajustes si la diferencia es mayor a 1 hora para evitar modificaciones drásticas por
 * errores o inconsistencias en la geolocalización.
 */

void ajustarZonaHoraria()
{
  zonilla = obtenHoraPorIP(getPublicIP());
  int timezoneDifference = abs(Settings.Timezone - zonilla);
  if (timezoneDifference <= 1)
  {
    if (Settings.Timezone != zonilla)
    {
      Settings.Timezone = zonilla;
      nvMem.saveConfig(&Settings);
      Serial.println(String("M8AX - Cambiando TimeZone... Actualmente Es UTC ") + (Settings.Timezone >= 0 ? "+" : "") + String(Settings.Timezone));
      timeClient.setTimeOffset(3600 * Settings.Timezone);
    }
  }
  Serial.println("M8AX - Combrobación De Ajuste De Hora ( TimeZone ), Correcta...");
}

/**
 * Simula una pantalla de televisión con efectos de barras y diagonales de colores aleatorios.
 *
 * La función genera y muestra en pantalla una serie de barras de colores en diferentes direcciones
 * (horizontales, verticales y diagonales), con un breve retraso entre cada una para crear una animación.
 * Al finalizar, la pantalla se limpia y se muestra la palabra "HOLA" o "HELLO" en un color aleatorio
 * O bien, se muestra "AUPA" o "ALOHA" en función de otro número aleatorio.
 *
 * - El ancho de las barras y la velocidad del efecto se generan aleatoriamente dentro de un rango.
 * - Se utilizan colores aleatorios para cada barra y se eliminan tras un breve retraso.
 * - Finalmente, la pantalla se limpia y se muestra el mensaje "HOLA" o "HELLO" con un tamaño de texto grande.
 * - O bien, se muestra "AUPA" o "ALOHA" en función de otro número aleatorio, (numeroSaludo).
 */

void television()
{
  int barWidth = (esp_random() % (41 - 5)) + 5; // Genera un número entre 5 y 40
  int speed = (esp_random() % (26 - 5)) + 5;    // Genera un número entre 5 y 25

  // Barras horizontales
  for (int y = 0; y < tft.height(); y += barWidth)
  {
    colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
    tft.fillRect(0, y, tft.width(), barWidth, colors[colorI]);
    delay(speed);
    tft.fillRect(0, y, tft.width(), barWidth, TFT_BLACK);
  }

  // Barras verticales
  for (int x = 0; x < tft.width(); x += barWidth)
  {
    colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
    tft.fillRect(x, 0, barWidth, tft.height(), colors[colorI]);
    delay(speed);
    tft.fillRect(x, 0, barWidth, tft.height(), TFT_BLACK);
  }

  // Barras diagonales (de esquina a esquina)
  for (int i = 0; i < tft.width() + tft.height(); i += barWidth)
  {
    colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
    tft.drawLine(i, 0, 0, i, colors[colorI]); // Diagonal de arriba a la izquierda
    delay(speed);
    tft.drawLine(i, 0, 0, i, TFT_BLACK); // Borrar la línea
  }

  for (int i = 0; i < tft.width() + tft.height(); i += barWidth)
  {
    colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
    tft.drawLine(tft.width() - i, tft.height(), tft.width(), tft.height() - i, colors[colorI]); // Diagonal de abajo a la derecha
    delay(speed);
    tft.drawLine(tft.width() - i, tft.height(), tft.width(), tft.height() - i, TFT_BLACK); // Borrar la línea
  }

  // Finalizar con la pantalla en negro
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  int numeroSaludo = (esp_random() % 10) + 1;
  tft.setTextColor(colors[colorI], TFT_BLACK);
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(7);

  if (colorI % 2 == 0)
  {
    tft.setCursor(76, 52);
    (numeroSaludo % 2 == 0) ? tft.print("HOLA") : tft.print("AUPA");
  }
  else
  {
    tft.setCursor(60, 52);
    (numeroSaludo % 2 == 0) ? tft.print("HELLO") : tft.print("ALOHA");
  }

  tft.setTextSize(1);
}

/**
 * Animación de inicio para el ESP32-S3 con pantalla TFT.
 *
 * Esta función genera una animación de bienvenida que dura aproximadamente 5 segundos.
 * Incluye varios efectos visuales antes de mostrar el texto central "M8AX" y "MINADOR DE BTC".
 *
 * Pasos de la animación:
 * 1. Llena la pantalla de negro como fondo inicial.
 * 2. Dibuja círculos concéntricos de colores aleatorios desde el centro, creando un efecto de expansión.
 * 3. Realiza un efecto de destello cambiando la pantalla a diferentes colores rápidamente.
 * 4. Muestra el texto "M8AX" en grande en el centro y debajo "MINADOR DE BTC".
 * 5. Mantiene el mensaje en pantalla por un breve tiempo.
 * 6. Finaliza con la animación television().
 */

void animacionInicio()
{
  tft.fillScreen(TFT_BLACK); // Pantalla en negro
  int centroX = tft.width() / 2;
  int centroY = tft.height() / 2;
  int maxRadio = min(centroX, centroY);

  // Efecto de círculos concéntricos

  for (int r = 5; r < maxRadio; r += 5)
  {
    colorIndex = esp_random() % (sizeof(colors) / sizeof(colors[0]));
    tft.drawCircle(centroX, centroY, r, colors[colorIndex]);
    delay(20);
  }

  // Efecto de destello

  for (int i = 0; i < 3; i++)
  {
    colorIndex = esp_random() % (sizeof(colors) / sizeof(colors[0]));
    tft.fillScreen(colors[colorIndex]);
    delay(50);
    colorIndex = esp_random() % (sizeof(colors) / sizeof(colors[0]));
    tft.fillScreen(colors[colorIndex]);
    delay(50);
  }

  // Mostrar el texto principal

  tft.setTextColor(TFT_BLACK, colors[colorIndex]);
  tft.setTextDatum(MC_DATUM);
  tft.setTextSize(3);
  tft.drawString("M8AX", centroX, centroY - 20);
  tft.setTextSize(2);
  tft.drawString("MINADOR DE BTC", centroX, centroY + 20);
  delay(750); // Mantiene el mensaje en pantalla

  // Finalizar con la animación television

  television();
  delay(1000);
  tft.setTextSize(1);
}

// Funcion para convertir un numero entero a números romanos

String convertirARomanos(int num)
{
  if (num == 0)
  {
    return "CERO";
  }

  String result = "";
  int values[] = {1000, 900, 500, 400, 100, 90, 50, 40, 10, 9, 5, 4, 1};
  String romans[] = {"M", "CM", "D", "CD", "C", "XC", "L", "XL", "X", "IX", "V", "IV", "I"};

  for (int i = 0; i < 13; i++)
  {
    while (num >= values[i])
    {
      result += romans[i];
      num -= values[i];
    }
  }

  return result;
}

// Funcion para obtener el día de la semana, Lun Mar Mie...

std::string obtenerDiaSemana(const std::string &fecha)
{
  static const char *diasem[] = {"Dom", "Lun", "Mar", "Mie", "Jue", "Vie", "Sab"};
  int dia = 1, mes = 1, anio = 1970;
  if (sscanf(fecha.c_str(), "%d/%d/%d", &dia, &mes, &anio) != 3)
    return "???";
  std::tm t = {};
  t.tm_mday = dia;
  t.tm_mon = mes - 1;
  t.tm_year = anio - 1900;
  std::mktime(&t);
  if (t.tm_wday < 0 || t.tm_wday > 6)
    return "???";
  return diasem[t.tm_wday];
}

// Función para quitar acentos de una cadena de texto

String quitarAcentos(String str)
{
  str.replace("á", "a");
  str.replace("é", "e");
  str.replace("í", "i");
  str.replace("ó", "o");
  str.replace("ú", "u");
  str.replace("Á", "A");
  str.replace("É", "E");
  str.replace("Í", "I");
  str.replace("Ó", "O");
  str.replace("Ú", "U");
  str.replace("ñ", "n");
  str.replace("Ñ", "N");
  str.replace("¿", ""); // Eliminar signos de interrogación al principio de la frase
  str.replace("¡", ""); // Eliminar signos de exclamación al principio de la frase
  return str;
}

// Funcion para cortar una cadena desde el ultimo guión ( - )

void borrarDesdeUltimoGuion(char *str)
{
  char *pos = strrchr(str, '-'); // Encuentra el último '-'
  if (pos)
  {
    *pos = '\0'; // Corta la cadena en ese punto
  }
}

// Tiempo máximo en milisegundos que dejo al buscador para encontrar la mejor cuenta.
// Pasado este tiempo corto la búsqueda y me quedo con lo mejor que haya encontrado hasta ese momento.
// Si en la pantalla se me queda congelado mucho rato, bajo este valor ( 800 - 1000 ).

#define CIFRAS_TIME_BUDGET_MS 2500

// Número máximo de pasos que puede tener una cuenta. Con 6 números solo puedo hacer 5 operaciones,
// porque cada operación gasta 2 números y devuelve 1.

#define CIFRAS_MAX_STEPS (MAX_NUMBERS - 1)

// Estructura que guarda un paso de la cuenta. Ejemplo: ( 45 + 3 ) = 48
// - a  : Primer número de la operación ( siempre el mayor de los dos ).
// - op : Operación realizada ( + - * / ).
// - b  : Segundo número de la operación.
// - r  : Resultado de la operación.
// - ia : De dónde sale 'a'. Si es -1, es uno de los 6 números originales. Si no, es el
//        índice del paso anterior que lo produjo ( 0 = primer paso, 1 = segundo, etc. ).
// - ib : Lo mismo que 'ia', pero para 'b'.
//
// ia e ib sirven para saber con exactitud qué pasos hacen falta de verdad, sin tener que
// adivinarlo comparando valores. Así no se imprime ninguna cuenta que luego no se use.

struct CifraPaso
{
  int a;
  char op;
  int b;
  int r;
  int ia;
  int ib;
};

// Estructura con todo el contexto de la búsqueda. Así no necesito variables globales
// ni pasar 10 parámetros a la función recursiva.
// - target    : Número objetivo que quiero alcanzar.
// - bestVal   : Mejor valor que he conseguido hasta ahora ( el más cercano al objetivo ).
// - bestDiff  : Diferencia entre el mejor valor y el objetivo ( 0 = exacto ).
// - bestCount : Cuántos pasos tiene la mejor cuenta encontrada.
// - best[]    : Pasos de la mejor cuenta encontrada.
// - cur[]     : Pasos de la cuenta que estoy probando ahora mismo.
// - nodes     : Contador de operaciones probadas, lo uso para mirar el reloj de vez en cuando.
// - deadline  : Momento ( millis ) en el que tengo que dejar de buscar.
// - done      : Si es true paro la búsqueda ( exacto encontrado o tiempo agotado ).

struct CifrasCtx
{
  int target;
  int bestVal;
  int bestDiff;
  int bestCount;
  CifraPaso best[CIFRAS_MAX_STEPS];
  CifraPaso cur[CIFRAS_MAX_STEPS];
  uint32_t nodes;
  uint32_t deadline;
  bool done;
};

// Función que comprueba si un valor que acabo de calcular es mejor que el mejor que tengo guardado.
// Si está más cerca del objetivo, me lo quedo y copio los pasos de la cuenta actual a la mejor.
// Si la diferencia es 0 es que he dado con el número exacto, así que marco done = true y paro todo.

// Parámetros:
// - c     : Contexto de la búsqueda.
// - value : Valor que acabo de obtener con la última operación.
// - steps : Número de pasos que lleva la cuenta actual.

static inline void cifras_candidate(CifrasCtx &c, int value, int steps)
{
  int diff = abs(value - c.target);
  if (diff < c.bestDiff)
  {
    c.bestDiff = diff;
    c.bestVal = value;
    c.bestCount = steps;
    for (int k = 0; k < steps; k++)
      c.best[k] = c.cur[k];
    if (diff == 0)
      c.done = true;
  }
}

// Función recursiva que es el cerebro del juego de las cifras. Prueba todas las combinaciones posibles.

// Procedimiento:
// 1. Cojo todas las parejas posibles de números de la lista ( i, j ).
// 2. Salto las parejas repetidas ( si tengo dos 7, probar el primero o el segundo es lo mismo ).
// 3. Pongo siempre el mayor delante ( x >= y ), así la resta nunca da negativo.
// 4. Apunto en 'rest' los números que no he usado en esta pareja.
// 5. Pruebo las 4 operaciones ( + - * / ), descartando las inútiles:
//    - Resta con números iguales ( da 0 ).
//    - Multiplicar o dividir por 1 ( no cambia nada ).
//    - División que no sea exacta ( nada de decimales ).
// 6. Cada 1024 operaciones miro el reloj. Si me he pasado de tiempo paro, y si no, cedo el procesador
//    un instante con vTaskDelay para no petar el watchdog del ESP32.
// 7. Guardo el paso, miro si es el mejor resultado hasta ahora y si me quedan números,
//    meto el resultado en la lista y me llamo a mí misma con un número menos.

// Parámetros:
// - c     : Contexto de la búsqueda.
// - vals  : Lista de números disponibles en este nivel.
// - n     : Cuántos números hay en la lista.
// - depth : Profundidad actual ( cuántas operaciones llevo hechas ).

static void cifras_search(CifrasCtx &c, const int *vals, const int *ids, int n, int depth)
{
  for (int i = 0; i < n - 1 && !c.done; i++)
  {
    bool dupI = false;
    for (int k = 0; k < i; k++)
      if (vals[k] == vals[i])
      {
        dupI = true;
        break;
      }
    if (dupI)
      continue;
    for (int j = i + 1; j < n && !c.done; j++)
    {
      bool dupJ = false;
      for (int k = i + 1; k < j; k++)
        if (vals[k] == vals[j])
        {
          dupJ = true;
          break;
        }
      if (dupJ)
        continue;
      int x = vals[i], y = vals[j];
      int ix = ids[i], iy = ids[j];
      if (x < y)
      {
        int t = x;
        x = y;
        y = t;
        t = ix;
        ix = iy;
        iy = t;
      }
      int rest[MAX_NUMBERS];
      int restId[MAX_NUMBERS];
      int m = 0;
      for (int k = 0; k < n; k++)
        if (k != i && k != j)
        {
          rest[m] = vals[k];
          restId[m] = ids[k];
          m++;
        }
      for (int o = 0; o < 4 && !c.done; o++)
      {
        int r;
        char op;
        switch (o)
        {
        case 0:
          if (x + y > 1000000000)
            continue;
          r = x + y;
          op = '+';
          break;
        case 1:
          if (x == y)
            continue;
          r = x - y;
          op = '-';
          break;
        case 2:
          if (x == 1 || y == 1 || (int64_t)x * y > 1000000000)
            continue;
          r = x * y;
          op = '*';
          break;
        default:
          if (y == 1 || x % y != 0)
            continue;
          r = x / y;
          op = '/';
          break;
        }
        if ((++c.nodes & 0x3FF) == 0)
        {
          if ((int32_t)(millis() - c.deadline) > 0)
          {
            c.done = true;
            return;
          }
          vTaskDelay(1);
        }
        c.cur[depth] = {x, op, y, r, ix, iy};
        cifras_candidate(c, r, depth + 1);
        if (c.done)
          return;
        if (m >= 1)
        {
          rest[m] = r;
          restId[m] = depth;
          cifras_search(c, rest, restId, m + 1, depth + 1);
        }
      }
    }
  }
}

// Función principal del juego de las cifras. Intenta llegar al número objetivo usando los 6 números
// y las 4 operaciones básicas. Si no llega exacto, se queda con el resultado más cercano.
// Deja en 'result' el texto con la cuenta completa, el resultado, la diferencia y el porcentaje
// de aciertos acumulado. También actualiza los contadores globales aciertos, fallos y totalci.

// Procedimiento:
// 1. Preparo el contexto: objetivo, tiempo límite y como mejor valor inicial el primer número.
// 2. Miro si alguno de los 6 números ya es el objetivo o está más cerca que el que tengo.
// 3. Si no he dado con el exacto, lanzo la búsqueda recursiva con cifras_search.
// 4. Limpio la cuenta: el buscador guarda todos los pasos que lleva hasta dar con el mejor resultado,
//    y alguno puede no servir para nada. Empiezo por el último paso, que seguro se usa, y voy hacia
//    atrás marcando los pasos de los que salen sus números. Los que no queden marcados no los imprimo.
// 5. Construyo el texto: los pasos que sí se usan separados por ' -> ' y debajo el resultado.
//    Si no hizo falta ninguna operación pongo 'Inicio - X'.
// 6. Actualizo aciertos, fallos, totalci y el porcentaje.
// 7. Añado al final ' ( EXACTO ) ' o ' Dif. - ( n ) ' junto con el porcentaje de aciertos.

// Parámetros:
// - numbers : Lista con los 6 números disponibles.
// - target  : Número objetivo.
// - result  : Cadena donde dejo el texto final ( tamaño MAX_RESULT_LENGTH ).

void calculate_operations(int numbers[], int target, char *result)
{
  static CifrasCtx c;
  c.target = target;
  c.bestVal = numbers[0];
  c.bestDiff = abs(numbers[0] - target);
  c.bestCount = 0;
  c.nodes = 0;
  c.done = false;
  c.deadline = millis() + CIFRAS_TIME_BUDGET_MS;
  for (int i = 0; i < MAX_NUMBERS && c.bestDiff != 0; i++)
  {
    int d = abs(numbers[i] - target);
    if (d < c.bestDiff)
    {
      c.bestDiff = d;
      c.bestVal = numbers[i];
    }
  }
  if (c.bestDiff == 0)
    c.done = true;
  if (!c.done)
  {
    int ids0[MAX_NUMBERS];
    for (int k = 0; k < MAX_NUMBERS; k++)
      ids0[k] = -1;
    cifras_search(c, numbers, ids0, MAX_NUMBERS, 0);
  }

  // Marco los pasos que de verdad se usan para llegar al resultado final.
  // Empiezo por el último y voy hacia atrás buscando de qué pasos salen sus números.

  bool usado[CIFRAS_MAX_STEPS];
  for (int k = 0; k < CIFRAS_MAX_STEPS; k++)
    usado[k] = false;
  if (c.bestCount > 0)
  {
    usado[c.bestCount - 1] = true;
    for (int k = c.bestCount - 1; k >= 0; k--)
    {
      if (!usado[k])
        continue;
      if (c.best[k].ia >= 0)
        usado[c.best[k].ia] = true;
      if (c.best[k].ib >= 0)
        usado[c.best[k].ib] = true;
    }
  }

  size_t len = 0;
  result[0] = '\0';
  len = snprintf(result, MAX_RESULT_LENGTH, "Pasos -> ");
  if (c.bestCount == 0)
  {
    len += snprintf(result + len, MAX_RESULT_LENGTH - len, "Inicio - %d", c.bestVal);
  }
  else
  {
    bool primero = true;
    for (int k = 0; k < c.bestCount && len < MAX_RESULT_LENGTH - 60; k++)
    {
      if (!usado[k])
        continue;
      len += snprintf(result + len, MAX_RESULT_LENGTH - len, "%s(%d %c %d) = %d",
                      primero ? "" : " -> ", c.best[k].a, c.best[k].op, c.best[k].b, c.best[k].r);
      primero = false;
    }
  }
  snprintf(result + len, MAX_RESULT_LENGTH - len, "\nResultado - %d", c.bestVal);

  bool exacto = (c.bestVal == target);
  if (exacto)
    aciertos++;
  else
    fallos++;
  totalci++;
  porcentaje = (static_cast<float>(aciertos) * 100) / totalci;
  char extra[64];
  if (exacto)
    snprintf(extra, sizeof(extra), ", ( EXACTO ) - ( AC %.2f%% )", porcentaje);
  else
    snprintf(extra, sizeof(extra), ", Dif. - ( %d ) - ( AC %.2f%% )", abs(c.bestVal - target), porcentaje);
  strncat(result, extra, MAX_RESULT_LENGTH - strlen(result) - 1);
}

// Genera una serie de números aleatorios dentro de un rango especificado
// y los almacena en el arreglo 'numbers[]'.
// 'size' define cuántos números generar, 'min' es el valor mínimo
// y 'max' es el valor máximo del rango.
// Los números generados se distribuyen de forma uniforme en el rango [min, max].

void generate_random_numbers(int numbers[], int size, int min, int max)
{
  for (int i = 0; i < size; i++)
  {
    numbers[i] = min + (esp_random() % (max - min + 1));
  }
}

// Realiza la factorización prima de un número entero positivo 'number'.
// La función descompone el número en factores primos y sus respectivos exponentes.
// Los factores y exponentes se almacenan en dos arreglos: 'factors[]' y 'exponents[]'.
// La cadena de caracteres 'result' contiene la representación de la factorización
// en el formato: "factor1^exponente1 * factor2^exponente2 ..."
// Si el número es primo, la cadena resultante también incluirá "( PRIMO )" al final.

const char *factorize(uint32_t number)
{
  char buffer[20];       // Buffer temporal para formatear factores
  uint32_t factors[16];  // Almacena factores primos
  uint8_t exponents[16]; // Almacena exponentes
  uint8_t count = 0;     // Número de factores encontrados

  // Si número menor que 2
  if (number < 2)
  {
    strcpy(result, number == 0 ? "0 ( SIN FACTORES )" : "1 ( SIN FACTORES )");
    return result;
  }

  // Factorización por 2
  if (number % 2 == 0)
  {
    factors[count] = 2;
    exponents[count] = 0;
    while (number % 2 == 0)
    {
      number >>= 1; // Dividir por 2 usando desplazamiento de bits
      exponents[count]++;
    }
    count++;
  }

  // Factorización por divisores impares
  uint32_t sqrt_num = sqrt(number);
  for (uint32_t divisor = 3; divisor <= sqrt_num; divisor += 2)
  {
    if (number % divisor == 0)
    {
      factors[count] = divisor;
      exponents[count] = 0;
      while (number % divisor == 0)
      {
        number /= divisor;
        exponents[count]++;
      }
      count++;
      sqrt_num = sqrt(number);
    }
  }

  // Si el número restante es primo
  if (number > 1)
  {
    factors[count] = number;
    exponents[count] = 1;
    count++;
  }

  // Construir la cadena de resultado
  result[0] = '\0'; // Inicializar el buffer de resultado
  for (uint8_t i = 0; i < count; i++)
  {
    if (i > 0)
    {
      strcat(result, " * "); // Separador
    }
    if (exponents[i] == 1)
    {
      sprintf(buffer, "%u", factors[i]);
    }
    else
    {
      sprintf(buffer, "%ue%u", factors[i], exponents[i]); // Formato "2e3"
    }
    strcat(result, buffer);
  }

  // Verificar si el número era primo
  if (count == 1 && exponents[0] == 1)
  {
    strcat(result, " ( PRIMO )");
  }
  return result;
}

// Función que envía un texto que contiene estadísticas de minería a tu canal de telegram

bool enviarMensajeATelegram(String &mensaje)
{
  bool ok = false;
  // Verificar y reconectar WiFi si es necesario
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("M8AX - Reconectando WiFi...");
    WiFi.disconnect();
    WiFi.reconnect();
    delay(5000); // Espera para reconectar

    // Si después del intento sigue sin conexión, salir de la función

    if (WiFi.status() != WL_CONNECTED)
    {
      Serial.println("M8AX - No Se Pudo Reconectar WiFi...");
      return false;
    }
  }
  WiFiClientSecure client;
  client.setInsecure();
  String mensajeCodificado = urlEncode(mensaje); // Mirar función
  mensajeCodificado.reserve(mensajeCodificado.length() + 100);
  mensaje = "";
  mensaje.reserve(0);

  // Reemplazar los saltos de línea con %0A

  mensajeCodificado.replace("\n", "%0A");

  Serial.println("M8AX - Enviando Mensaje A Telegram...");

  String url = "https://api.telegram.org/bot" + BOT_TOKEN + "/sendMessage?chat_id=" + CHAT_ID + "&text=" + mensajeCodificado;
  url.reserve(url.length() + 50);
  mensajeCodificado = "";
  mensajeCodificado.reserve(0);

  if (client.connect("api.telegram.org", 443))
  {
    client.print("GET ");
    client.print(url);
    client.print(" HTTP/1.1\r\nHost: api.telegram.org\r\nConnection: close\r\n\r\n");
    unsigned long t0 = millis();
    while (!client.available() && client.connected() && millis() - t0 < 5000)
    {
      delay(10);
    }
    String estado = client.readStringUntil('\n');
    client.stop();
    if (estado.indexOf(" 200") > 0)
    {
      Serial.println("M8AX - Mensaje Enviado A Telegram...");
      sumatele += 1;
      ok = true;
    }
    else
    {
      estado.trim();
      Serial.println("M8AX - Telegram No Lo Aceptó - Respuesta: [" + estado + "]");
    }
    url = "";
    url.reserve(0);
  }
  else
  {
    Serial.println("M8AX - Error De Conexión, Mensaje A Telegram Falló...");
    client.flush();
    client.stop();
    url = "";
    url.reserve(0);
  }
  return ok;
}

// Función para recopilar todo lo que vamos a enviar a nuestro canal de telegram
// Se ha añadido el envío del RSSI de la señal WiFi

bool recopilaTelegram()
{
  unsigned long epochTime = timeClient.getEpochTime(); // Obtener segundos desde 1970
  time_t epoch = (time_t)epochTime;                    // Convertir a time_t
  struct tm *timeinfo = localtime(&epoch);             // Convertir a una estructura de tiempo local
  int dia = timeinfo->tm_mday;                         // Día del mes (1 a 31)
  int mes = timeinfo->tm_mon + 1;                      // Mes (1 a 12)
  int anio = timeinfo->tm_year + 1900;                 // Año (por defecto es desde 1900)
  int horita = timeinfo->tm_hour;                      // Hora
  int minutitos = timeinfo->tm_min;                    // Minutos
  int segundos = timeinfo->tm_sec;                     // Segundos
  int indice = esp_random() % 5;

  // Formatear la hora en "00:00:00 - UTC+/-X"
  char horaFormateada[23];
  sprintf(horaFormateada, "%02d:%02d:%02d - UTC %s%d", horita, minutitos, segundos, (zonilla >= 0 ? "+" : ""), zonilla);

  // Formatear la fecha en "dia/mes/año"
  char fechaFormateada[11];
  sprintf(fechaFormateada, "%02d/%02d/%04d", dia, mes, anio);

  // Extraer los últimos 4 dígitos de la mac
  uint8_t mac[6];
  WiFi.macAddress(mac);
  char u4digits[5];
  sprintf(u4digits, "%02X%02X", mac[4], mac[5]);

  std::string quediase = obtenerDiaSemana(std::string(fechaFormateada));
  String telrb = monedilla.remainingBlocks;
  String cadenaEnvio;
  cadenaEnvio.reserve(5000);
  cadenaEnvio = "";
  cadenaEnvio = F("------------------------------------------------------------------------------------------------\n");
  cadenaEnvio += "------------------------ M8AX - NerdMinerV2-" + String(u4digits) + " DATOS DE MINERÍA - M8AX -----------------------\n";
  cadenaEnvio += "------------------------------ " + String(fechaFormateada) + " " + quediase.c_str() + " - " + horaFormateada + " ------------------------------\n";
  cadenaEnvio += F("------------------------------------------------------------------------------------------------\n");
  quediase.clear();
  quediase.shrink_to_fit();
  if (sumatele <= 3999)
  {
    cadenaEnvio += "Mensaje Número - " + convertirARomanos(sumatele) + " | F.C.FW - " + String(__DATE__) + " A Las " + String(__TIME__) + " | Hace - " + String(convertirTiempo(segundosDesdeCompilacion())).c_str() + "\n";
  }
  else
  {
    cadenaEnvio += "Mensaje Número - " + String(sumatele) + " | F.C.FW - " + String(__DATE__) + " A Las " + String(__TIME__) + " | Hace - " + String(convertirTiempo(segundosDesdeCompilacion())).c_str() + "\n";
  }
  cadenaEnvio += "Tiempo Minando - " + (mineria.timeMining.substring(0, mineria.timeMining.indexOf(" ")).length() == 1 ? "0" + mineria.timeMining.substring(0, mineria.timeMining.indexOf(" ")) : mineria.timeMining.substring(0, mineria.timeMining.indexOf(" "))) + " Días" + mineria.timeMining.substring(mineria.timeMining.indexOf(" ") + 1) + "\n";
  cadenaEnvio += "HR Actual - " + mineria.currentHashRate + " KH/s ( MAX - " + String(maxkh) + " | MIN - " + String(minkh) + " )\n";
  cadenaEnvio += "Temp. De CPU - " + mineria.temp + "° ( MAX - " + String(maxtemp) + "° | MIN - " + String(mintemp) + "° | TMP>70° - " + String(alertatemp) + " )\n";
  cadenaEnvio += "Plantillas De Bloque - " + mineria.templates + "\n";
  cadenaEnvio += "Shares Enviados A La Pool - " + mineria.completedShares + "\n";
  cadenaEnvio += "Mejor Dificultad Alcanzada - " + mineria.bestDiff + "\n";
  cadenaEnvio += "Dificultad De La Red - " + monedilla.netwrokDifficulty + "\n";
  cadenaEnvio += "Cómputo Total - " + mineria.totalKHashes + " KH - ( " + String(atof(mineria.totalKHashes.c_str()) / 1000, 3) + " MH )\n";
  cadenaEnvio += "HR Global - " + monedilla.globalHashRate + " EH/s\n";
  cadenaEnvio += "Precio De BTC - " + monedilla.btcPrice + " | Ayer -> " + subebaja + " |\n";
  cadenaEnvio += "FEE Promedio Por TX - " + monedilla.halfHourFee + "\n";
  cadenaEnvio += "Altura De Bloque - " + relojete.blockHeight + "\n";
  telrb.replace("BLOCKS", "");
  cadenaEnvio += "Bloques Entre Halvings - 210000\n";
  cadenaEnvio += "Bloques Hasta El Halving - " + String(telrb) + "\n";
  long int hechos = 210000 - telrb.toInt();
  cadenaEnvio += "Bloques Minados Post-Halving - " + String(hechos) + "\n";
  char buffer[20];
  dtostrf((hechos * 100.0) / 210000.0, 0, 3, buffer); // Convierte float a string con 3 decimales
  cadenaEnvio += "% Completado Desde El Último Halving - " + String(buffer) + "%\n";
  cadenaEnvio += "Pool De Minería - " + Settings.PoolAddress + "\n";
  cadenaEnvio += "Puerto Del Pool - " + String(Settings.PoolPort) + "\n";
  cadenaEnvio += "Tu Wallet De BTC - " + String(Settings.BtcWallet) + "\n";
  cadenaEnvio += "Tu IP - " + getPublicIP() + " | WiFi RSSI " + String(WiFi.RSSI()) + "\n";
  cadenaEnvio += urlsm8ax[indice];
  cadenaEnvio += F("\n------------------------------------------------------------------------------------------------\n");
  if (mineria.valids.toInt() > 0)
  {
    cadenaEnvio += "||| ¡ BLOQUE MINADO ! ¡ A COBRAR ! :) |||\n";
  }
  else
  {
    cadenaEnvio += "||| ¡ SIN PASTA, SIN GLORIA ! ¡ A SEGUIR CON LA HISTORIA ! |||\n";
  }
  cadenaEnvio += F("------------------------------------------------------------------------------------------------\n");
  cadenaEnvio += F("---------------------------------- M8AX - DATOS NERD - M8AX ------------------------------------\n");
  cadenaEnvio += F("------------------------------------------------------------------------------------------------\n");
  rndnumero = esp_random();
  cadenaEnvio += "Factorización De Número - " + String(rndnumero) + " -> " + factorize(rndnumero) + "\n";
  cadenaEnvio += "------------------------------------------------------------------------------------------------\n                                       By M8AX Corp. " + convertirARomanos(anio);
  cadenaEnvio += F("\n------------------------------------------------------------------------------------------------");
  bool enviado = enviarMensajeATelegram(cadenaEnvio);
  cadenaEnvio = "";
  cadenaEnvio.reserve(0);
  return enviado;
}

// Función para imprimir datos en pantalla en formato texto plano, todo seguido con información
// Se añaden todos los datos disponibles para mostrar + RSSI de la red WiFi

void datosPantallaTextoPlano()
{
  unsigned long epochTime = timeClient.getEpochTime(); // Obtener segundos desde 1970
  time_t epoch = (time_t)epochTime;                    // Convertir a time_t
  struct tm *timeinfo = localtime(&epoch);             // Convertir a una estructura de tiempo local
  int dia = timeinfo->tm_mday;                         // Día del mes (1 a 31)
  int mes = timeinfo->tm_mon + 1;                      // Mes (1 a 12)
  int anio = timeinfo->tm_year + 1900;                 // Año (por defecto es desde 1900)
  int horita = timeinfo->tm_hour;                      // Hora
  int minutitos = timeinfo->tm_min;                    // Minutos
  int segundos = timeinfo->tm_sec;                     // Segundos
  String telrb = monedilla.remainingBlocks;
  String cadenaEnvio2;
  cadenaEnvio2.reserve(5000);
  cadenaEnvio2 = "";
  cadenaEnvio2 += relojete.currentDate + " - " + relojete.currentTime + " - UTC " + ((zonilla >= 0) ? "+" : "") + String(zonilla);
  cadenaEnvio2 += ". WiFi RSSI " + String(WiFi.RSSI());
  cadenaEnvio2 += ". Tiempo Minando - " + (mineria.timeMining.substring(0, mineria.timeMining.indexOf(" ")).length() == 1 ? "0" + mineria.timeMining.substring(0, mineria.timeMining.indexOf(" ")) : mineria.timeMining.substring(0, mineria.timeMining.indexOf(" "))) + " Días" + mineria.timeMining.substring(mineria.timeMining.indexOf(" ") + 1);
  cadenaEnvio2 += ". HR Actual - " + mineria.currentHashRate + " KH/s ( MAX - " + String(maxkh) + " | MIN - " + String(minkh) + " )";
  cadenaEnvio2 += ". Temp. De CPU - " + mineria.temp + "g ( MAX - " + String(maxtemp) + "g | MIN - " + String(mintemp) + "g | TMP>70g - " + String(alertatemp) + " )";
  cadenaEnvio2 += ". Plantillas De Bloque - " + mineria.templates;
  cadenaEnvio2 += ". Shares Enviados A La Pool - " + mineria.completedShares;
  cadenaEnvio2 += ". Mejor Dificultad Alcanzada - " + mineria.bestDiff;
  cadenaEnvio2 += ". Dificultad De La Red - " + monedilla.netwrokDifficulty;
  cadenaEnvio2 += ". Cómputo Total - " + mineria.totalKHashes + " KH - ( " + String(atof(mineria.totalKHashes.c_str()) / 1000, 3) + " MH )";
  cadenaEnvio2 += ". HR Global - " + monedilla.globalHashRate + " EH/s";
  cadenaEnvio2 += ". Precio De BTC - " + monedilla.btcPrice + " | Ayer -> " + subebaja + " |";
  cadenaEnvio2 += ". FEE Promedio Por TX - " + monedilla.halfHourFee;
  cadenaEnvio2 += ". Altura De Bloque - " + relojete.blockHeight;
  telrb.replace("BLOCKS", "");
  cadenaEnvio2 += ". Bloques Entre Halvings - 210000";
  cadenaEnvio2 += ". Bloques Hasta El Halving - " + String(telrb);
  long int hechos = 210000 - telrb.toInt();
  cadenaEnvio2 += ". Bloques Minados Post-Halving - " + String(hechos);
  char buffer[20];
  dtostrf((hechos * 100.0) / 210000.0, 0, 3, buffer); // Convierte float a string con 5 decimales
  cadenaEnvio2 += ". % Completado Desde El Último Halving - " + String(buffer) + "%";
  cadenaEnvio2 += ". % Restante Para Próximo Halving - " + String(100.000 - round(atof(buffer) * 1000) / 1000, 3) + "%";
  if (mineria.valids.toInt() > 0)
  {
    cadenaEnvio2 += ". ||| BLOQUE MINADO, A COBRAR xD |||";
  }
  else
  {
    cadenaEnvio2 += ". ||| SIN PASTA, SIN GLORIA, A SEGUIR CON LA HISTORIA... |||";
  }
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(1, 1);
  tft.setTextSize(2);
  tft.print("   DATOS EN TEXTO PLANO");
  tft.setCursor(1, 22);
  tft.setTextSize(1);
  tft.setTextColor(colors[colorIndex]);
  tft.print(quitarAcentos(cadenaEnvio2));
  cadenaEnvio2 = "";
  cadenaEnvio2.reserve(0);
}

// Función para obterner la ciudad y la temperatura mediante la IP pública

std::pair<String, String> obtenerCiudadYTemperatura(const String &ip)
{
  String ciudad = "";
  String temperatura = "";
  String latitud = "";
  String longitud = "";

  // Obtener la ciudad, latitud y longitud usando la API de geolocalización
  HTTPClient http;
  String urlGeo = "http://ip-api.com/json/" + ip + "?fields=city,lat,lon"; // Obtener ciudad, latitud y longitud
  http.begin(urlGeo);

  int httpCode = http.GET();
  if (httpCode == HTTP_CODE_OK)
  {
    String payload = http.getString();

    // Parsear el JSON para obtener la ciudad y coordenadas
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, payload);
    payload = "";
    payload.reserve(0);
    if (error)
    {
      Serial.println("M8AX - Error Al Parsear JSON De Geolocalización");
      http.end();
      return std::make_pair(ciudad, temperatura); // Devolver vacías en caso de error
    }
    ciudad = doc["city"].as<String>();           // Ciudad obtenida
    latitud = String(doc["lat"].as<float>(), 6); // Convertir a string con 6 decimales
    longitud = String(doc["lon"].as<float>(), 6);
  }
  else
  {
    Serial.println("M8AX - Error Al Obtener Datos De Geolocalización");
    http.end();
    return std::make_pair(ciudad, temperatura); // Devolver vacías si hay error
  }

  http.end();

  // Obtener la temperatura usando wttr.in con latitud y longitud
  if (latitud != "" && longitud != "")
  {
    // Usamos WiFiClientSecure para HTTPS
    WiFiClientSecure client;
    client.setInsecure(); // Permitir conexiones HTTPS sin verificar certificado

    String urlTemp = "https://wttr.in/" + latitud + "," + longitud + "?format=%t"; // Solicitar temperatura con coordenadas
    http.begin(client, urlTemp);                                                   // Iniciar la solicitud HTTPS
    httpCode = http.GET();
    urlTemp = "";
    urlTemp.reserve(0);

    if (httpCode == HTTP_CODE_OK)
    {
      temperatura = http.getString(); // Obtener la temperatura
    }
    else
    {
      Serial.println("M8AX - Error Al Obtener Temperatura");
    }
    http.end();
  }
  else
  {
    Serial.println("M8AX - Coordenadas No Encontradas Para La IP");
  }

  return std::make_pair(ciudad, temperatura); // Devolver ciudad y temperatura
}

// Dibuja una cuadrícula en la pantalla TFT, actualizando las celdas cuya
// condición ha cambiado entre el estado anterior (grid) y el nuevo (newgrid).
// Cada celda se dibuja con el color correspondiente (blanco si el valor es 1,
// negro si el valor es 0), utilizando el tamaño de celda definido por 'CELLXY'.
// La función recorre todas las celdas de la cuadrícula y actualiza las que han cambiado.

void drawGrid(void)
{
  uint16_t color = color = TFT_WHITE;
  for (int16_t x = 1; x < GRIDX - 1; x++)
  {
    for (int16_t y = 1; y < GRIDY - 1; y++)
    {
      if ((grid[x][y]) != (newgrid[x][y]))
      {
        if (newgrid[x][y] == 1)
          color = 0xFFFF; // random(0xFFFF);
        else
          color = 0;
        tft.fillRect(CELLXY * x, CELLXY * y, CELLXY, CELLXY, color);
      }
    }
  }
}

// Calcula y devuelve el número de vecinos activos (celdas con valor 1)
// alrededor de una celda ubicada en las coordenadas (x, y) en la cuadrícula.
// La función considera las 8 celdas adyacentes (incluyendo diagonales) y suma
// el valor de cada celda vecina (1 si está activa, 0 si no lo está).

int getNumberOfNeighbors(int x, int y)
{
  return grid[x - 1][y] + grid[x - 1][y - 1] + grid[x][y - 1] + grid[x + 1][y - 1] + grid[x + 1][y] + grid[x + 1][y + 1] + grid[x][y + 1] + grid[x - 1][y + 1];
}

// Inicializa la cuadrícula 'grid' y 'newgrid'. La cuadrícula está compuesta por
// celdas activas (valor 1) e inactivas (valor 0). Las celdas en los bordes
// están inactivas, mientras que las celdas internas se asignan aleatoriamente como
// activas con una probabilidad de 1/3. 'newgrid' se inicializa a 0 en todas sus celdas.

void initGrid(void)
{
  for (int16_t x = 0; x < GRIDX; x++)
  {
    for (int16_t y = 0; y < GRIDY; y++)
    {
      newgrid[x][y] = 0;
      if (x == 0 || x == GRIDX - 1 || y == 0 || y == GRIDY - 1)
      {
        grid[x][y] = 0;
      }
      else
      {
        if (esp_random() % 3 == 1)
          grid[x][y] = 1;
        else
          grid[x][y] = 0;
      }
    }
  }
}

// Calcula el siguiente estado de la cuadrícula en base a las reglas del
// autómata celular. Para cada celda de la cuadrícula, se evalúa el número
// de vecinos activos (1). Si la celda está activa (1) y tiene 2 o 3 vecinos,
// permanece activa en el siguiente paso. Si tiene menos de 2 o más de 3 vecinos,
// se desactiva. Si la celda está inactiva (0) y tiene exactamente 3 vecinos,
// se activa en el siguiente paso. El resultado se almacena en 'newgrid'.

void computeCA()
{
  for (int16_t x = 1; x < GRIDX; x++)
  {
    for (int16_t y = 1; y < GRIDY; y++)
    {
      int neighbors = getNumberOfNeighbors(x, y);
      if (grid[x][y] == 1 && (neighbors == 2 || neighbors == 3))
      {
        newgrid[x][y] = 1;
      }
      else if (grid[x][y] == 1)
        newgrid[x][y] = 0;
      if (grid[x][y] == 0 && (neighbors == 3))
      {
        newgrid[x][y] = 1;
      }
      else if (grid[x][y] == 0)
        newgrid[x][y] = 0;
    }
  }
}

// Función que recibe una palabra y la devuelve con la primera letra en mayúscula

String capitalizar(String palabra)
{
  if (palabra.length() > 0)
  {
    palabra[0] = toupper(palabra[0]);
  }
  return palabra;
}

// Función que recibe un número entero y lo devuelve en formato cadena y escrito en letra

String numeroAEscrito(int num, bool esDecimal = false)
{
  if (num < 0 || num > 9999)
    return "Numero Fuera De Rango";

  String unidades[] = {"cero", "uno", "dos", "tres", "cuatro", "cinco", "seis", "siete", "ocho", "nueve"};
  String especiales[] = {"Diez", "Once", "Doce", "Trece", "Catorce", "Quince",
                         "Dieciseis", "Diecisiete", "Dieciocho", "Diecinueve"};
  String decenas[] = {"", "", "Veinte", "Treinta", "Cuarenta", "Cincuenta",
                      "Sesenta", "Setenta", "Ochenta", "Noventa"};
  String centenas[] = {"", "Ciento", "Doscientos", "Trescientos", "Cuatrocientos",
                       "Quinientos", "Seiscientos", "Setecientos", "Ochocientos", "Novecientos"};

  if (num == 100)
    return "Cien"; // Caso especial
  if (num == 0)
    return "Cero";

  String resultado = "";

  // Si es la parte decimal y el número es menor que 10, agregar "Cero"

  if (esDecimal && num < 10)
  {
    resultado += "Cero ";
  }

  // Miles

  if (num >= 1000)
  {
    if (num / 1000 == 1)
    {
      resultado += "Mil";
    }
    else
    {
      resultado += capitalizar(unidades[num / 1000]) + " Mil";
    }
    num %= 1000; // Eliminamos los miles
    if (num > 0)
      resultado += " ";
  }

  // Centenas

  if (num >= 100)
  {
    resultado += (num == 100) ? String("Cien") : capitalizar(centenas[num / 100]);
    num %= 100; // Eliminamos las centenas
    if (num > 0)
      resultado += " ";
  }

  // Decenas y unidades

  if (num >= 10 && num <= 19)
  {
    resultado += capitalizar(especiales[num - 10]); // Números entre 10 y 19
  }
  else
  {
    if (num >= 20)
    {
      if (num == 20)
      {
        resultado += "Veinte";
      }
      else if (num < 30)
      {
        resultado += "Veinti";
        if (num % 10 > 0)
          resultado += unidades[num % 10];
      }
      else
      {
        resultado += capitalizar(decenas[num / 10]);
        if (num % 10 > 0)
          resultado += " Y " + capitalizar(unidades[num % 10]);
      }
    }
    else if (num > 0)
    {
      resultado += capitalizar(unidades[num]);
    }
  }
  return resultado;
}

/**
 * Dibuja un dado en la pantalla TFT en la posición especificada.
 *
 * @param numero Número del dado a dibujar (1 a 6).
 * @param x Coordenada X del centro del dado.
 * @param y Coordenada Y del centro del dado.
 *
 * La función dibuja un cuadrado representando el dado y coloca los puntos según el número indicado.
 * - El dado tiene un tamaño de 60x60 píxeles, centrado en (x, y).
 * - Los puntos se dibujan con un radio de 5 píxeles.
 * - Se usa `fillCircle` para dibujar los puntos en la posición correcta según el número del dado.
 * - Se verifica cada número de 1 a 6 para posicionar los puntos correctamente.
 */

void dibujarDado(int numero, int x, int y)
{
  // Dibuja el borde del dado
  tft.drawRect(x - 30, y - 30, 60, 60, TFT_WHITE); // Cuadrado del dado

  int r = 5; // Radio de los puntos

  // Posiciones de los puntos del dado según el número (del 1 al 6)
  // Definimos los puntos en un arreglo para que siempre se muestren correctamente
  switch (numero)
  {
  case 1:
    // Un solo punto en el centro
    tft.fillCircle(x, y, r, TFT_WHITE);
    break;
  case 2:
    // Dos puntos en esquinas opuestas
    tft.fillCircle(x - 15, y - 15, r, TFT_WHITE);
    tft.fillCircle(x + 15, y + 15, r, TFT_WHITE);
    break;
  case 3:
    // Tres puntos: dos en las esquinas y uno en el centro
    tft.fillCircle(x, y, r, TFT_WHITE);
    tft.fillCircle(x - 15, y - 15, r, TFT_WHITE);
    tft.fillCircle(x + 15, y + 15, r, TFT_WHITE);
    break;
  case 4:
    // Cuatro puntos: en las esquinas
    tft.fillCircle(x - 15, y - 15, r, TFT_WHITE);
    tft.fillCircle(x + 15, y - 15, r, TFT_WHITE);
    tft.fillCircle(x - 15, y + 15, r, TFT_WHITE);
    tft.fillCircle(x + 15, y + 15, r, TFT_WHITE);
    break;
  case 5:
    // Cinco puntos: cuatro en las esquinas y uno en el centro
    tft.fillCircle(x, y, r, TFT_WHITE);
    tft.fillCircle(x - 15, y - 15, r, TFT_WHITE);
    tft.fillCircle(x + 15, y - 15, r, TFT_WHITE);
    tft.fillCircle(x - 15, y + 15, r, TFT_WHITE);
    tft.fillCircle(x + 15, y + 15, r, TFT_WHITE);
    break;
  case 6:
    // Seis puntos: en filas de 3
    tft.fillCircle(x - 15, y - 15, r, TFT_WHITE);
    tft.fillCircle(x + 15, y - 15, r, TFT_WHITE);
    tft.fillCircle(x - 15, y, r, TFT_WHITE);
    tft.fillCircle(x + 15, y, r, TFT_WHITE);
    tft.fillCircle(x - 15, y + 15, r, TFT_WHITE);
    tft.fillCircle(x + 15, y + 15, r, TFT_WHITE);
    break;
  }
}

/**
 * Dibuja una moneda en la pantalla TFT en la posición especificada.
 *
 * @param moneda Valor de la moneda: 0 para "Cara", 1 para "Cruz".
 * @param x Coordenada X del centro de la moneda.
 * @param y Coordenada Y del centro de la moneda.
 *
 * La función representa una moneda con:
 * - Un círculo de radio 45 píxeles para la forma de la moneda.
 * - Un texto "C" en color verde si la moneda muestra "Cara".
 * - Un texto "X" en color rojo si la moneda muestra "Cruz".
 * - El texto se posiciona centrado dentro de la moneda.
 */

void dibujarMoneda(int moneda, int x, int y)
{
  int radio = 45; // Un tamaño más grande para la moneda

  tft.drawCircle(x, y, radio, TFT_WHITE); // Dibujar la moneda

  if (moneda == 0)
  {
    tft.setTextColor(TFT_GREEN);
    tft.setTextSize(2);
    tft.drawCentreString("C", x, y - 20, 4); // Dibuja "C" para cara
  }
  else
  {
    tft.setTextColor(TFT_RED);
    tft.setTextSize(2);
    tft.drawCentreString("X", x, y - 20, 4); // Dibuja "X" para cruz
  }
}

/**
 * Dibuja un texto centrado en la pantalla con efecto de escritura letra por letra.
 *
 * La función calcula el ancho del texto y lo centra horizontalmente en la pantalla.
 * Luego, dibuja cada letra una por una con un pequeño retardo para generar un efecto
 * de escritura progresiva. Cada letra se muestra con un color aleatorio.
 *
 * @param text       Cadena de texto a mostrar.
 * @param y          Coordenada Y donde se dibujará el texto.
 * @param delayTime  Tiempo de espera (en milisegundos) entre cada letra.
 */

void drawCenteredText(const char *text, int y, int delayTime)
{
  int screenWidth = tft.width();         // Ancho de la pantalla
  int textWidth = tft.textWidth(text);   // Ancho del texto
  int x = (screenWidth - textWidth) / 5; // Posición X para centrar el texto

  // Efecto de escritura letra por letra
  for (int i = 0; i < strlen(text); i++)
  {
    colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
    tft.setTextColor(colors[colorI], TFT_BLACK);
    tft.drawChar(text[i], x, y, 2);          // Dibuja una letra
    x += tft.textWidth(String(text[i])) + 5; // Ajusta la posición X para la siguiente letra
    delay(delayTime);                        // Retardo para el efecto
  }
}

/**
 * Simula una animación de nieve cayendo en la pantalla.
 *
 * La función dibuja copos de nieve que caen desde posiciones aleatorias en la parte superior
 * de la pantalla y los mueve hacia abajo con velocidades aleatorias. Cuando un copo alcanza
 * la parte inferior, reaparece en la parte superior en una nueva posición horizontal.
 *
 * - Se generan 100 copos de nieve con posiciones iniciales aleatorias.
 * - La animación dura 5 segundos y se actualiza cada 35 ms.
 * - Al finalizar, se muestra el mensaje "FELIZ NAVIDAD" centrado en la pantalla con un color aleatorio.
 */

void nevar()
{
  tft.fillScreen(TFT_BLACK); // Fondo negro (puedes cambiarlo)

  const int NUM_COPOS = 100;      // Número de copos de nieve en pantalla
  int x[NUM_COPOS], y[NUM_COPOS]; // Coordenadas de los copos

  // Inicializa copos en posiciones aleatorias
  for (int i = 0; i < NUM_COPOS; i++)
  {
    x[i] = (esp_random() % 320); // Genera un número entre 0 y 319
    y[i] = (esp_random() % 170); // Genera un número entre 0 y 169
  }

  unsigned long startTime = millis();

  while (millis() - startTime < 5000)
  {                            // 5 segundos
    tft.fillScreen(TFT_BLACK); // Borra pantalla

    for (int i = 0; i < NUM_COPOS; i++)
    {
      tft.drawPixel(x[i], y[i], TFT_WHITE); // Dibuja copo de nieve
      y[i] += (esp_random() % 4) + 1;       // Baja la posición del copo

      if (y[i] > 170)
      { // Si sale de la pantalla, reaparece arriba
        y[i] = 0;
        x[i] = esp_random() % 320;
      }
    }

    delay(35); // Controla la velocidad de la animación
  }

  // Muestra "FELIZ NAVIDAD" al final
  tft.fillScreen(TFT_BLACK);
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.setTextColor(colors[colorI], TFT_BLACK);
  tft.setTextDatum(MC_DATUM);               // Centra el texto
  tft.setFreeFont(FSB18);                   // Fuente grande (cambia si es necesario)
  tft.drawString("FELIZ NAVIDAD", 160, 85); // Texto en el centro
  delay(1500);
  tft.setFreeFont(NULL);
}

/**
 * Simula una animación de nieve cayendo con mayor densidad y velocidad.
 *
 * Esta función es una versión mejorada de "nevar()", aumentando el número de copos a 200
 * y ajustando la velocidad de caída para un efecto más dinámico.
 *
 * - Se generan 200 copos de nieve en posiciones aleatorias dentro de la pantalla.
 * - La animación dura 5 segundos, con actualizaciones cada 25 ms para una caída más rápida.
 * - Al finalizar, muestra el mensaje "HAPPY NEW YEAR" centrado en la pantalla con un color aleatorio.
 */

void nevar2()
{
  tft.fillScreen(TFT_BLACK); // Fondo negro (puedes cambiarlo)

  const int NUM_COPOS = 200;      // Número de copos de nieve en pantalla
  int x[NUM_COPOS], y[NUM_COPOS]; // Coordenadas de los copos

  // Inicializa copos en posiciones aleatorias
  for (int i = 0; i < NUM_COPOS; i++)
  {
    x[i] = esp_random() % 320;
    y[i] = esp_random() % 170;
  }

  unsigned long startTime = millis();

  while (millis() - startTime < 5000)
  {                            // 5 segundos
    tft.fillScreen(TFT_BLACK); // Borra pantalla

    for (int i = 0; i < NUM_COPOS; i++)
    {
      tft.drawPixel(x[i], y[i], TFT_WHITE); // Dibuja copo de nieve
      y[i] += (esp_random() % 5) + 1;       // Baja Posición Del Copo
      if (y[i] > 170)
      { // Si sale de la pantalla, reaparece arriba
        y[i] = 0;
        x[i] = esp_random() % 320;
      }
    }

    delay(25); // Controla la velocidad de la animación
  }

  // Muestra "FELIZ --- AÑO" al final
  tft.fillScreen(TFT_BLACK);
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.setTextColor(colors[colorI], TFT_BLACK);
  tft.setTextDatum(MC_DATUM);                // Centra el texto
  tft.setFreeFont(FSB18);                    // Fuente grande (cambia si es necesario)
  tft.drawString("HAPPY NEW YEAR", 160, 85); // Texto en el centro
  delay(1500);
  tft.setFreeFont(NULL);
}

/**
 * Muestra un mensaje animado y ejecuta un efecto de televisión.
 *
 * - Borra la pantalla y selecciona un color aleatorio para el texto.
 * - Muestra tres líneas de texto centradas con un efecto de escritura.
 * - Espera 500 ms y luego ejecuta la animación "television()".
 * - Tras la animación, espera 1 segundo y restablece el tamaño del texto a 1.
 */

void M8AXTicker()
{
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(colors[colorI], TFT_BLACK);
  tft.setTextSize(2);
  drawCenteredText("M v I i I a X", 30, 100);
  drawCenteredText("NerdMiner V2", 70, 100);
  drawCenteredText("V  10 . 03 . 77", 110, 100);
  delay(500);
  television();
  delay(1000);
  tft.setTextSize(1);
}

/**
 * Simula una nevada animada en la pantalla con colores aleatorios.
 *
 * - Llena la pantalla de negro como fondo inicial.
 * - Genera 500 copos de nieve con posiciones aleatorias.
 * - Durante 2 segundos, los copos caen con velocidades aleatorias y se
 *   regeneran en la parte superior cuando salen de la pantalla.
 * - Cada copo de nieve se dibuja con un color aleatorio.
 * - La velocidad de la animación varía aleatoriamente en cada iteración.
 * - Al finalizar, se ejecuta la función M8AXTicker().
 */

void nevar3()
{
  tft.fillScreen(TFT_BLACK); // Fondo negro (puedes cambiarlo)

  const int NUM_COPOS = 500;      // Número de copos de nieve en pantalla
  int x[NUM_COPOS], y[NUM_COPOS]; // Coordenadas de los copos

  // Inicializa copos en posiciones aleatorias
  for (int i = 0; i < NUM_COPOS; i++)
  {
    x[i] = esp_random() % 320;
    y[i] = esp_random() % 170;
  }

  unsigned long startTime = millis();

  while (millis() - startTime < 2000)
  {                            // 5 segundos
    tft.fillScreen(TFT_BLACK); // Borra pantalla
    for (int i = 0; i < NUM_COPOS; i++)
    {
      colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
      tft.drawPixel(x[i], y[i], colors[colorI]); // Dibuja copo de nieve
      y[i] += (esp_random() % 5) + 1;            // Baja La Posición DeL Copo

      if (y[i] > 170)
      { // Si sale de la pantalla, reaparece arriba
        y[i] = 0;
        x[i] = esp_random() % 320;
      }
    }
    int nnumeroAleatorio = esp_random() % 20 + 1;
    delay(nnumeroAleatorio); // Controla la velocidad de la animación
  }
  M8AXTicker();
}

/**
 * Simula el cierre de unas cortinas en la pantalla.
 *
 * - Dibuja dos líneas verticales negras que se alejan del centro de la pantalla.
 * - La animación comienza desde el centro de la pantalla y las líneas se expanden hacia los bordes.
 * - Utiliza líneas verticales para dar el efecto de cortinas cerrándose.
 * - La velocidad de la animación se controla con un retraso de 10 ms entre cada paso.
 */

void cortinas2()
{
  int centerX = tft.width() / 2;  // Centro de la pantalla en X
  int centerY = tft.height() / 2; // Centro de la pantalla en Y

  for (int i = 0; i <= centerX; i++)
  {
    // Dibuja líneas verticales que se alejan del centro
    tft.drawFastVLine(centerX - i, 0, tft.height(), TFT_BLACK); // Izquierda
    tft.drawFastVLine(centerX + i, 0, tft.height(), TFT_BLACK); // Derecha
    delay(10);                                                  // Ajusta la velocidad de la animación
  }
}

/**
 * Muestra una animación de cortinas cerrándose y luego muestra información de la versión del software.
 *
 * - Llame a la función `cortinas2()` para crear el efecto de cortinas que se cierran en la pantalla.
 * - Después de la animación de las cortinas, se muestran textos centrados en la pantalla con información:
 *   - "M 8 A X"
 *   - "NerdMiner V2"
 *   - "V 10 . 03 . 77"
 * - Luego de mostrar el texto, hay un pequeño retraso de 500 ms y después se ejecuta la función `television()`.
 * - Después de 1000 ms, restablece el tamaño del texto a 1.
 */

void cortinas()
{
  tft.fillScreen(TFT_BLACK); // Fondo negro
  cortinas2();
  drawCenteredText("M 8 A X", 30, 100);
  drawCenteredText("NerdMiner V2", 70, 100);
  drawCenteredText("V  10 . 03 . 77", 110, 100);
  delay(500);
  television();
  delay(1000);
  tft.setTextSize(1);
}

/**
 * Muestra una animación similar al M8AXTicker pero con un texto diferente, específicamente diseñado para el minador BTC.
 *
 * - Comienza llenando la pantalla con color negro y eligiendo un color de texto aleatorio.
 * - Luego muestra tres líneas de texto centradas en la pantalla con la siguiente información:
 *   - "B T C  M I N E R"
 *   - "NerdMiner V2"
 *   - "V  10 . 03 . 77"
 * - Después de mostrar el texto, realiza una pausa de 500 ms antes de ejecutar la función `television()`.
 * - Después de 1000 ms, restablece el tamaño del texto a 1.
 */

void M8AXTicker2()
{
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(colors[colorI], TFT_BLACK);
  tft.setTextSize(2);
  drawCenteredText("B T C  M I N E R", 30, 100);
  drawCenteredText("NerdMiner V2", 70, 100);
  drawCenteredText("V  10 . 03 . 77", 110, 100);
  delay(500);
  television();
  delay(1000);
  tft.setTextSize(1);
}

/**
 * Muestra una animación similar al M8AXTicker, pero con el texto "M I N E  T E C H".
 *
 * - Llenará la pantalla con un fondo negro y luego seleccionará un color de texto aleatorio.
 * - Muestra tres líneas de texto centradas:
 *   - "M I N E  T E C H"
 *   - "NerdMiner V2"
 *   - "V  10 . 03 . 77"
 * - Después de mostrar el texto, realiza una pausa de 500 ms.
 * - Luego ejecuta la función `television()`, que probablemente genera algún tipo de animación visual.
 * - Después de 1000 ms, restablece el tamaño del texto a 1.
 */

void M8AXTicker3()
{
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(colors[colorI], TFT_BLACK);
  tft.setTextSize(2);
  drawCenteredText("M I N E  T E C H", 30, 100);
  drawCenteredText("NerdMiner V2", 70, 100);
  drawCenteredText("V  10 . 03 . 77", 110, 100);
  delay(500);
  television();
  delay(1000);
  tft.setTextSize(1);
}

/**
 * Muestra una animación similar al M8AXTicker, pero con el texto "I M O D   T E C H".
 *
 * - Llena la pantalla con un fondo negro y luego selecciona un color de texto aleatorio.
 * - Muestra tres líneas de texto centradas:
 *   - "I M O D   T E C H"
 *   - "NerdMiner V2"
 *   - "V  10 . 03 . 77"
 * - Después de mostrar el texto, realiza una pausa de 500 ms.
 * - Luego ejecuta la función `television()`, que probablemente genera algún tipo de animación visual.
 * - Después de 1000 ms, restablece el tamaño del texto a 1.
 */

void M8AXTicker4()
{
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(colors[colorI], TFT_BLACK);
  tft.setTextSize(2);
  drawCenteredText("I M O D   T E C H", 30, 100);
  drawCenteredText("NerdMiner V2", 70, 100);
  drawCenteredText("V  10 . 03 . 77", 110, 100);
  delay(500);
  television();
  delay(1000);
  tft.setTextSize(1);
}

/**
 * Muestra una animación similar al M8AXTicker, pero con el texto "I M O D   T E C H".
 *
 * - Llena la pantalla con un fondo negro y luego selecciona un color de texto aleatorio.
 * - Muestra tres líneas de texto centradas:
 *   - "E H D  -  M D D D"
 *   - "NerdMiner V2"
 *   - "V  10 . 03 . 77"
 * - Después de mostrar el texto, realiza una pausa de 500 ms.
 * - Luego ejecuta la función `television()`, que probablemente genera algún tipo de animación visual.
 * - Después de 1000 ms, restablece el tamaño del texto a 1.
 */

void M8AXTicker5()
{
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(colors[colorI], TFT_BLACK);
  tft.setTextSize(2);
  drawCenteredText("E H D  -  M D D D", 30, 100);
  drawCenteredText("NerdMiner V2", 70, 100);
  drawCenteredText("V  10 . 03 . 77", 110, 100);
  delay(500);
  television();
  delay(1000);
  tft.setTextSize(1);
}

/**
 * Dibuja un código QR en la pantalla TFT.
 *
 * Esta función genera un código QR a partir de una cadena de texto (`data`) y lo dibuja en la pantalla TFT
 * en una posición específica con un tamaño y color determinado.
 *
 * @param data Cadena de texto que contiene la información a codificar en el código QR.
 * @param xPos Posición X en la pantalla donde se ubicará el código QR.
 * @param yPos Posición Y en la pantalla donde se ubicará el código QR.
 * @param qrSize Tamaño total del código QR que se desea dibujar (en píxeles).
 * @param color Color del código QR (normalmente blanco o negro para los módulos).
 */

void dibujaQR(String data, int xPos, int yPos, int qrSize, int color)
{
  QRCode qrcode;
  uint8_t qrcodeData[qrcode_getBufferSize(3)]; // Tamaño del QR, nivel 3

  // Inicializa el QR con los datos
  qrcode_initText(&qrcode, qrcodeData, 3, 0, data.c_str());

  // Tamaño real del QR en módulos
  int qrRealSize = qrcode.size;

  // Calcular el tamaño de cada bloque (módulo) según el tamaño total deseado
  int blockSize = qrSize / qrRealSize;

  // Dibujar los módulos (pixeles) del QR en la pantalla
  for (int y = 0; y < qrRealSize; y++)
  {
    for (int x = 0; x < qrRealSize; x++)
    {
      // Si es un módulo negro, dibujarlo
      if (qrcode_getModule(&qrcode, x, y))
      {
        tft.fillRect(xPos + x * blockSize, yPos + y * blockSize, blockSize, blockSize, color);
      }
      // Si es un módulo blanco, NO hacer nada (deja el fondo transparente)
    }
  }
}

/**
 * Verifica si es horario de verano.
 *
 * Esta función determina si una fecha específica (mes y día) cae dentro del horario de verano.
 *
 * @param mes Mes del año (1-12).
 * @param dia Día del mes.
 * @return true si es horario de verano, false en caso contrario.
 */

bool EsHVerano(int mes, int dia)
{
  if (mes > 3 && mes < 10)
  {
    return true;
  }
  if (mes == 3 && dia >= 25)
  {
    return true;
  }
  if (mes == 10 && dia <= 31)
  {
    return false;
  }
  return false;
}

/**
 * Dibuja un medidor analógico de KH/s.
 *
 * Esta función dibuja un medidor circular en la pantalla con graduaciones y una aguja que se mueve
 * según el valor de KH/s. Además, muestra el valor numérico de KH/s en el centro del medidor.
 *
 * @param khs Valor de KH/s que se usa para mover la aguja.
 */

void dibujaAnalogKH(float khs)
{
  int centerX = 160;
  int centerY = 85;
  int radius = 60;
  tft.setTextSize(1);
  tft.drawCircle(centerX, centerY, radius, TFT_WHITE);
  for (int i = 0; i < 360; i += 15)
  {
    float angle = (i - 90) * DEG_TO_RAD;
    int outerX = centerX + radius * cos(angle);
    int outerY = centerY + radius * sin(angle);
    int innerX = centerX + (radius - 5) * cos(angle);
    int innerY = centerY + (radius - 5) * sin(angle);
    if (i % 90 == 0)
    {
      innerX = centerX + (radius - 10) * cos(angle);
      innerY = centerY + (radius - 10) * sin(angle);
      tft.drawLine(innerX, innerY, outerX, outerY, TFT_WHITE);
    }
    else
    {
      tft.drawLine(innerX, innerY, outerX, outerY, TFT_WHITE);
    }
    if (i % 90 == 0)
    {
      tft.setTextColor(TFT_WHITE);
      int textX = centerX + (radius - 20) * cos(angle); // Coordenadas para el texto
      int textY = centerY + (radius - 20) * sin(angle);
      tft.setCursor(textX - 5, textY - 5); // Ajustar el texto
      if (i == 0)
        tft.print("360"); // 12 horas -> 360 KH/s
      else if (i == 90)
        tft.print("90"); // 15 minutos -> 90 KH/s
      else if (i == 180)
        tft.print("180"); // 30 minutos -> 180 KH/s
      else if (i == 270)
        tft.print("270"); // 45 minutos -> 270 KH/s
    }
  }

  // No dibujar la aguja si khs es 0
  if (khs == 0)
  {
    return;
  }

  // Mapear el valor de KH/s a un ángulo de -90 (0 KH/s) a 90 (360 KH/s)
  float angle = map(khs, 0, 360, -90, 270); // Amplitud del reloj ajustada a 360 grados

  // Convertir el ángulo a radianes
  float radianes = angle * DEG_TO_RAD;

  // Coordenadas de la punta de la aguja (calculada a partir del ángulo)
  int agujaX = centerX + radius * cos(radianes);
  int agujaY = centerY + radius * sin(radianes);

  // Dibujar la aguja
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.drawLine(centerX, centerY, agujaX, agujaY, TFT_GREENYELLOW); // Dibuja la aguja
  tft.setCursor(142, 96);
  tft.setTextSize(1);
  tft.setTextColor(colors[colorI]);
  tft.print(String(khs));
  tft.setCursor(142, 105);
  tft.print(" KH/s");
}

/**
 * Obtiene y muestra hasta 5 titulares desde un feed RSS.
 *
 * Elige un feed al azar de los 9 de "urls". Si falla o no da noticias, prueba el siguiente (dando la vuelta) hasta recorrer los 9.
 * Lee la respuesta en streaming, sin guardarla entera, para gastar poca RAM. Omite el título del canal "Cointelegraph.com News".
 *
 * Muestra los titulares en el monitor serial y en la pantalla LCD. Si ningún feed da noticias, muestra un mensaje fijo.
 */

void obtenerNoticias()
{
  int total = 9;
  int inicio = esp_random() % total;
  bool hayNoticias = false;

  for (int intento = 0; intento < total && !hayNoticias; intento++)
  {
    HTTPClient http;
    int indice = (inicio + intento) % total;
    String urlnot = urls[indice];
    http.begin(urlnot);
    http.useHTTP10(true);
    int httpCode = http.GET();
    cadenanoti = "";
    if (httpCode == HTTP_CODE_OK)
    {
      WiFiClient *stream = http.getStreamPtr();
      stream->setTimeout(3000);
      Serial.println("M8AX - Noticias Obtenidas. Procesando...");
      int itemIndex = 0;

      while (itemIndex < 5)
      {
        if (!stream->find("<item>"))
          break;
        if (!stream->find("<title>"))
          break;

        String title = "";
        bool completo = false;
        uint32_t t0 = millis();

        while (millis() - t0 < 3000 && title.length() < 300)
        {
          if (!stream->available())
          {
            if (!http.connected())
              break;
            delay(1);
            continue;
          }
          title += (char)stream->read();
          if (title.endsWith("</title>"))
          {
            title.remove(title.length() - 8);
            completo = true;
            break;
          }
        }

        if (!completo)
          break;

        int cdataStart = title.indexOf("<![CDATA[");
        int cdataEnd = title.indexOf("]]>");
        if (cdataStart != -1 && cdataEnd != -1)
        {
          title = title.substring(cdataStart + 9, cdataEnd);
        }

        if (title.indexOf("Cointelegraph.com News") != -1)
        {
          continue;
        }

        Serial.println(String(itemIndex + 1) + ". " + title);
        cadenanoti += String(itemIndex + 1) + ". " + title + "\n\n";
        itemIndex++;
        taskYIELD();
      }

      if (itemIndex > 0)
      {
        hayNoticias = true;
        tft.setCursor(1, 26);
        tft.setTextSize(1);
        colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
        tft.setTextColor(colors[colorI]);
        tft.print(quitarAcentos(cadenanoti));
      }
      else
      {
        Serial.println("M8AX - No Se Encontraron Noticias Válidas.");
      }
      cadenanoti = "";
      cadenanoti.reserve(0);
    }
    else
    {
      Serial.println("M8AX - Error Al Obtener El Feed RSS. Código HTTP: " + String(httpCode));
    }

    http.end();
  }

  if (!hayNoticias)
  {
    Serial.println("M8AX - Ningún Feed Ha Dado Noticias.");
    tft.setCursor(1, 26);
    tft.setTextSize(1);
    colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
    tft.setTextColor(colors[colorI]);
    tft.print("POR MUCHAS VUELTAS QUE DEMOS, SIEMPRE TENDREMOS EL CULO ATRAS...");
  }
}

bool preciosBinanceLote(const char *host, float *precios)
{
  String url = "https://" + String(host) + "/api/v3/ticker/price?symbols=%5B";
  for (int i = 0; i < NUM_CRIPTOS; i++)
  {
    if (i > 0)
      url += ",";
    url += "%22" + String(criptomonedas[i].simbolo) + "%22";
  }
  url += "%5D";

  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient http;
  http.setTimeout(5000);
  if (!http.begin(client, url))
    return false;
  int codigo = http.GET();
  if (codigo != 200)
  {
    Serial.printf("M8AX - %s Responde %d\n", host, codigo);
    http.end();
    return false;
  }
  String payload = http.getString();
  http.end();

  DynamicJsonDocument doc(4096);
  if (deserializeJson(doc, payload))
    return false;

  JsonArray lista = doc.as<JsonArray>();
  for (JsonObject item : lista)
  {
    const char *simbolo = item["symbol"] | "";
    for (int i = 0; i < NUM_CRIPTOS; i++)
      if (strcmp(simbolo, criptomonedas[i].simbolo) == 0)
        precios[i] = atof(item["price"] | "-1");
  }
  return true;
}

float precioBinanceUno(const char *host, const char *simbolo)
{
  String url = "https://" + String(host) + "/api/v3/ticker/price?symbol=" + simbolo;
  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient http;
  http.setTimeout(5000);
  if (!http.begin(client, url))
    return -1;
  float precio = -1;
  if (http.GET() == 200)
  {
    DynamicJsonDocument doc(256);
    if (!deserializeJson(doc, http.getString()))
      precio = atof(doc["price"] | "-1");
  }
  http.end();
  return precio;
}

bool obtenerPreciosTodos(float *precios)
{
  for (int i = 0; i < NUM_CRIPTOS; i++)
    precios[i] = -1;
  if (WiFi.status() != WL_CONNECTED)
    return false;

  if (preciosBinanceLote("api.binance.com", precios))
    return true;
  if (preciosBinanceLote("data-api.binance.vision", precios))
    return true;

  // Seguramente hay un par deslistado: una a una, y la deslistada se queda en ERROR
  bool alguno = false;
  for (int i = 0; i < NUM_CRIPTOS && abortar == 0; i++)
  {
    precios[i] = precioBinanceUno("data-api.binance.vision", criptomonedas[i].simbolo);
    if (precios[i] > 0)
      alguno = true;
  }
  return alguno;
}

// Función que recibe un número entero y devuelve una cadena con el número en morse

String Amorse(int n)
{
  // Diccionario de los números y sus representaciones en código morse
  const String morse_dict[] = {
      "-----", ".----", "..---", "...--", "....-", ".....", "-....", "--...", "---..", "----."};

  // Convertir el número a cadena de caracteres
  String num_str = String(n);

  // Variable para almacenar el resultado en código Morse
  String morse_code = "";

  // Convertir cada dígito del número a morse y concatenarlo a la cadena resultante
  for (unsigned int i = 0; i < num_str.length(); i++)
  {
    char digit = num_str.charAt(i);
    morse_code += morse_dict[digit - '0'] + " "; // Restamos '0' para convertir el carácter a un índice
  }

  return morse_code;
}

// Función que recibe un número entero y devuelve una cadena con el número en binario

String ABinario(int num)
{
  String binary = "";

  if (num == 0)
  {
    return "0"; // Si el número es 0, devuelve "0"
  }

  while (num > 0)
  {
    binary = (num % 2 == 0 ? "0" : "1") + binary; // Agregar el bit al principio
    num /= 2;                                     // Divide el número entre 2
  }

  return binary;
}

/**
 * Obtiene una cita desde un servidor remoto.
 *
 * Realiza múltiples intentos para obtener una cita de una API. Si no se puede obtener la cita después de varios intentos, devuelve un mensaje de error.
 *
 * @return La cita obtenida o un mensaje de error si no se puede recuperar.
 */

String getQuote()
{
  HTTPClient http;
  http.begin(serverName);
  http.addHeader("Content-Type", "application/json");

  String quote = "ERROR AL OBTENER LA CITA... Mi Lema Es - ( ... Por Muchas Vueltas Que Demos, Siempre Tendremos El Culo Atras ... )\n\n- M8AX"; // Valor por defecto en caso de error

  int httpResponseCode = 0;
  int attempts = 0;
  const int maxAttempts = 5; // Número máximo de intentos

  while (httpResponseCode <= 0 && attempts < maxAttempts)
  {
    // Realizar la solicitud GET
    httpResponseCode = http.GET();
    attempts++;
    taskYIELD();
    if (httpResponseCode > 0)
    {
      String payload = http.getString();

      // Parsear el JSON
      DynamicJsonDocument doc(1024);
      deserializeJson(doc, payload);
      payload = "";
      payload.reserve(0);

      // Extraer la cita y el autor
      String quoteText = doc["quote"].as<String>();
      String author = doc["author"].as<String>();

      quote = "\"" + quoteText + "\"\n\n- " + author;
      Serial.println("M8AX - Frase Número - " + String(numfrases) + " - " + quote + " - " + String(quote.length()) + " Caracteres");
      quoteText = "";
      author = "";
      doc.clear();
      doc.shrinkToFit();
      taskYIELD();
      quoteText.reserve(0);
      author.reserve(0);
    }
    else
    {
      Serial.println("M8AX - Error Al Recibir La Cita. Reintentando...");
      delay(1000); // Esperar 1 segundos antes de volver a intentar
    }
  }

  if (attempts == maxAttempts)
  {
    Serial.println("M8AX - Se Alcanzó El Número Máximo De Intentos...");
  }
  http.end();
  return quote;
}

/**
 * Muestra una cita en la pantalla TFT.
 *
 * Esta función imprime una cita en la pantalla TFT, añadiendo una línea decorativa encima del texto y aplicando un color aleatorio a la cita.
 *
 * @param quote La cita a mostrar en la pantalla.
 */

void displayQuote(String quote)
{
  tft.setTextColor(TFT_WHITE); // Establecer color de texto
  // Configuración de la fuente
  int numCaracteres = quote.length();
  if (numCaracteres <= 200)
  {
    tft.setTextSize(2);
  }
  else
  {
    tft.setTextSize(1);
  }
  tft.setCursor(0, 28);
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.setTextColor(colors[colorI]);
  tft.drawLine(0, 20, 320, 20, colors[colorI]); // Dibujar línea de (0, y) a (320, y)
  tft.print(quitarAcentos(quote));
  quote = "";
  quote.reserve(0);
  taskYIELD();
}

// Función para obtener el nombre del mes

String obtenerNombreMes(int mes)
{
  switch (mes)
  {
  case 1:
    return "Enero";
  case 2:
    return "Febrero";
  case 3:
    return "Marzo";
  case 4:
    return "Abril";
  case 5:
    return "Mayo";
  case 6:
    return "Junio";
  case 7:
    return "Julio";
  case 8:
    return "Agosto";
  case 9:
    return "Septiembre";
  case 10:
    return "Octubre";
  case 11:
    return "Noviembre";
  case 12:
    return "Diciembre";
  default:
    return "Mes Inválido";
  }
}

/**
 * Dibuja un porcentaje de un círculo como un arco iluminado.
 * Esta función dibuja un círculo completo y luego ilumina un porcentaje del mismo, según el valor de `porcentaje` proporcionado. El área iluminada es proporcional a dicho porcentaje y se pinta de color.
 *
 * @param centroX El valor de la coordenada X del centro del círculo.
 * @param centroY El valor de la coordenada Y del centro del círculo.
 * @param radio El radio del círculo.
 * @param porcentaje El porcentaje de iluminación del círculo (0 a 100).
 */

void dibujarPorcentajeLunar(int centroX, int centroY, int radio, float porcentaje)
{
  // Aseguramos que el porcentaje esté en un rango válido (0% a 100%)
  if (porcentaje < 0)
    porcentaje = 0;
  if (porcentaje > 100)
    porcentaje = 100;
  // Calculamos la posición horizontal hasta donde se debe rellenar
  int limiteX = centroX + (porcentaje / 100.0) * (2 * radio) - radio; // Punto X del límite
  // Dibujar el círculo completo con el color de fondo
  tft.fillCircle(centroX, centroY, radio, TFT_TRANSPARENT);
  // Dibujar el porcentaje iluminado horizontalmente
  for (int x = -radio; x <= radio; x++)
  { // Recorrer el ancho del círculo
    for (int y = -radio; y <= radio; y++)
    { // Recorrer la altura del círculo
      // Verificar si el punto está dentro del círculo
      if (x * x + y * y <= radio * radio)
      {
        // Verificar si el punto está dentro del porcentaje horizontal
        if (centroX + x <= limiteX)
        {
          tft.drawPixel(centroX + x, centroY + y, colors[colorIndex]); // Color del área iluminada
        }
      }
    }
  }
}

/**
 * Dibuja el reloj con las manecillas de horas, minutos y segundos, junto con la fase lunar y otros datos.
 *
 * Esta función actualiza la pantalla con la hora actual, las manecillas del reloj, la fase lunar, el estado de la riqueza (millonario/no millonario), la ciudad y la temperatura. Los datos se extraen de varias fuentes y se visualizan de manera interactiva en la pantalla TFT.
 *
 * @param horas La hora actual.
 * @param minutos Los minutos actuales.
 * @param segundos Los segundos actuales.
 * @param dia El día actual en formato de cadena.
 * @param mes El mes actual en formato de cadena.
 * @param anio El año actual en formato de cadena.
 * @param quediase El texto adicional que se mostrará en el reloj.
 * @param HRate La tasa de hash actual (en KH/s).
 * @param tempera La temperatura actual.
 * @param millonario Indica si el usuario es millonario (1) o no (0).
 */

void dibujarReloj(int horas, int minutos, int segundos, String dia, String mes, String anio, std::string quediase, String HRate, String tempera, int millonario)
{
  moonData_t moon;
  int diaa = dia.toInt();
  int mess = mes.toInt();
  int anioo = anio.toInt();
  int horaa = horas, minutoo = minutos;
  unsigned long segundo = timeClient.getSeconds();
  String mesecillo = obtenerNombreMes(mess).substring(0, 3);
  // Inicializar la estructura tm
  struct tm timeinfo;
  timeinfo.tm_year = anioo - 1900; // Año desde 1900
  timeinfo.tm_mon = mess - 1;      // Mes (0 = enero)
  timeinfo.tm_mday = diaa;         // Día del mes
  timeinfo.tm_hour = horaa;        // Hora
  timeinfo.tm_min = minutoo;       // Minutos
  timeinfo.tm_sec = segundo;       // Segundos
  timeinfo.tm_isdst = -1;          // Determina si es horario de verano (automático)
  // Convertir a time_t
  time_t cadenaDeTiempo = mktime(&timeinfo);
  moon = mymoonPhase.getPhase(cadenaDeTiempo);
  double porcentajeIluminado = moon.percentLit * 100;
  char porcentajeTexto[10];
  snprintf(porcentajeTexto, sizeof(porcentajeTexto), "%.2f%%", porcentajeIluminado);
  String datoslunarelogrande = String(porcentajeTexto);
  // Limpia la pantalla para redibujar
  background.pushImage(0, 0, M8AXRelojLunarWidth, M8AXRelojLunarHeight, M8AXRelojLunar);
  background.pushSprite(0, 0);
  // Centro del reloj
  int centroX = 160;     // Centro horizontal
  int centroY = 170 / 2; // Centro vertical
  int radio = 80;        // Aumenta el radio para hacer el reloj más grande
  // Dibuja el círculo del reloj
  tft.drawCircle(centroX, centroY, radio, colors[colorIndex]);
  // Calcula los ángulos para las manecillas
  float anguloSegundos = (segundos * 6) - 90;                         // 360° / 60 = 6° por segundo
  float anguloMinutos = (minutos * 6) - 90;                           // 360° / 60 = 6° por minuto
  float anguloHoras = (horas % 12) * 30 - 90 + (minutos / 60.0) * 30; // 360° / 12 = 30° por hora
  // Dibuja la manecilla de los segundos
  int xSegundos = centroX + radio * 0.85 * cos(radians(anguloSegundos)); // Ajusta la longitud de la manecilla
  int ySegundos = centroY + radio * 0.85 * sin(radians(anguloSegundos));
  tft.drawLine(centroX, centroY, xSegundos, ySegundos, TFT_RED);
  // Dibuja la manecilla de los minutos
  int xMinutos = centroX + radio * 0.75 * cos(radians(anguloMinutos)); // Manecilla de minutos más larga
  int yMinutos = centroY + radio * 0.75 * sin(radians(anguloMinutos));
  tft.drawLine(centroX, centroY, xMinutos, yMinutos, TFT_WHITE);
  // Dibuja la manecilla de las horas
  int xHoras = centroX + radio * 0.60 * cos(radians(anguloHoras)); // Manecilla de horas más corta
  int yHoras = centroY + radio * 0.60 * sin(radians(anguloHoras));
  tft.drawLine(centroX, centroY, xHoras, yHoras, TFT_WHITE);
  // Dibuja las marcas de las horas
  for (int i = 0; i < 12; i++)
  {
    float anguloMarca = i * 30 - 90; // 360° / 12 = 30° por hora
    int xExterior = centroX + radio * 0.95 * cos(radians(anguloMarca));
    int yExterior = centroY + radio * 0.95 * sin(radians(anguloMarca));
    int xInterior = centroX + radio * 0.8 * cos(radians(anguloMarca));
    int yInterior = centroY + radio * 0.8 * sin(radians(anguloMarca));
    tft.drawLine(xInterior, yInterior, xExterior, yExterior, colors[colorIndex]);
  }
  tft.setCursor(4, 5);
  tft.setTextColor(colors[colorIndex]);
  tft.setTextSize(3);
  tft.print(dia);
  tft.setCursor(4, 37);
  tft.print(mesecillo);
  tft.setCursor(4, 69);
  tft.print(String(quediase.c_str()));
  tft.setCursor(4, 101);
  tft.print(anio);
  tft.setCursor(4, 133);
  tft.setTextSize(2);
  tft.print(quitarAcentos(ciudad).substring(0, 7));
  tft.setCursor(23, 153);
  tempciudad.replace("°", "");
  tft.print(tempciudad);
  tft.setTextSize(1);
  tft.setCursor(143, 115);
  tft.print(HRate);
  tft.setCursor(143, 125);
  tft.print(" KH/s");
  tft.setCursor(254, 35);
  tft.setTextSize(2);
  tft.print("LUNA");
  tft.setCursor(236, 121);
  tft.setTextSize(2);
  tft.print(datoslunarelogrande);
  tft.setTextSize(1);
  tft.setCursor(143, 42);
  tft.print("  " + tempera);
  tft.setCursor(143, 52);
  tft.print("Grados");
  tft.setTextColor(TFT_DARKGREY);
  tft.setCursor(254, 5);
  tft.setTextSize(2);
  tft.print("RICO");
  tft.setCursor(230, 150);
  tft.setTextSize(2);
  tft.print("NoRICO");
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  if (millonario == 0)
  {
    tft.setTextColor(colors[colorI]);
    tft.setCursor(230, 150);
    tft.setTextSize(2);
    tft.print("NoRICO");
  }
  else
  {
    tft.setTextColor(colors[colorI]);
    tft.setCursor(254, 5);
    tft.setTextSize(2);
    tft.print("RICO");
  }
  dibujarPorcentajeLunar(281, 84, 25, porcentajeIluminado);
  if (mirarTiempo == 0 || ciudad == "ERROR" || (minutos % 10 == 0 && segundos == 0))
  {
    mirarTiempo = 1;
    std::pair<String, String> resultado = obtenerCiudadYTemperatura(getPublicIP());
    // Verificar si los valores fueron obtenidos correctamente
    if (resultado.first != "" && resultado.second != "")
    {
      // Imprimir los resultados en el Monitor Serie
      Serial.println("M8AX - Ciudad: " + resultado.first);
      Serial.println("M8AX - Temperatura: " + resultado.second);
      ciudad = resultado.first;
      tempciudad = resultado.second;
    }
    else
    {
      Serial.println("M8AX - No Se Pudo Obtener La Ciudad O La Temperatura.");
      ciudad = "ERROR";
      tempciudad = "ERROR";
    }
  }
  actualizarc = 0;
}

// Función para generar 6 números únicos y devolverlos como String

String generarNumerosPrimitiva()
{
  const int MIN = 1;
  const int MAX = 49;
  const int NUM_COUNT = 6;
  int numeros[NUM_COUNT];
  int contador = 0;

  while (contador < NUM_COUNT)
  {
    int num = (esp_random() % 49) + 1; // Generar número aleatorio
    bool repetido = false;
    // Comprobar si el número ya existe en el array
    for (int i = 0; i < contador; i++)
    {
      if (numeros[i] == num)
      {
        repetido = true;
        break;
      }
    }
    // Si no está repetido, lo añadimos al array
    if (!repetido)
    {
      numeros[contador++] = num;
    }
  }
  // Convertir los números a un String
  String resultado = "";
  for (int i = 0; i < NUM_COUNT; i++)
  {
    resultado += String(numeros[i]);
    if (i < NUM_COUNT - 1)
    {
      resultado += ", "; // Separador entre números
    }
  }
  return resultado; // Retornar como String
}

// Función para calcular el primer día del mes

int calcularPrimerDia(int dia, int mes, int anio)
{
  // Si el mes es enero o febrero, se ajusta como en el algoritmo de Zeller
  if (mes == 1)
  {
    mes = 13;
    anio--;
  }
  if (mes == 2)
  {
    mes = 14;
    anio--;
  }
  // Cálculo según la fórmula de Zeller
  int q = dia;
  int m = mes;
  int k = anio % 100;
  int j = anio / 100;
  int h = (q + (13 * (m + 1)) / 5 + k + k / 4 + j / 4 + 5 * j) % 7;
  // Ajuste del valor de h para alinear correctamente los días de la semana
  // La fórmula de Zeller devuelve 0 = sábado, 1 = domingo, ..., 6 = viernes
  // Queremos que 0 sea domingo, 1 sea lunes, ..., 6 sea sábado
  h = (h + 5) % 7;
  return h;
}

/*
  Función: mostrarCalendario
  Propósito: Esta función dibuja un calendario en una pantalla TFT con el mes y año especificados,
  mostrando los días de la semana y resaltando el día actual. Además, muestra la hora en formato
  de 24 horas y el mes y año en la parte inferior del calendario. Abajo a la derecha, se mostrará
  el hashrate actualizado cada 5 segundos.

  Parámetros:
    - dia: El día actual del mes (1 a 31).
    - mes: El mes del calendario (1 a 12).
    - anio: El año del calendario (por ejemplo, 2025).
    - h1, h2: Las dos partes de la hora actual (por ejemplo, 14 para las 2 PM).
    - m1, m2: Las dos partes de los minutos actuales (por ejemplo, 30 para los 30 minutos).
    - mineria.currentHashRate: (el hashrate actualizado cada 5 segundos.)

  La función calcula el primer día del mes y muestra los días de la semana en la parte superior.
  Luego, llena el calendario con los días correspondientes, comenzando desde el día correcto.
  Resalta el día actual y marca los fines de semana con un color distinto.
  También muestra la hora actual, el mes/año en la parte inferior del calendario y el hashrate.
*/

void mostrarCalendario(int dia, int mes, int anio, int h1, int h2, int m1, int m2)
{
  // Calcular el primer día del mes usando la función calcularPrimerDia
  int primerDia = calcularPrimerDia(1, mes, anio); // El día 1 del mes
  // Días de la semana
  String diasSemana[7] = {"LUN", "MAR", "MIE", "JUE", "VIE", "SAB", "DOM"};
  // Número de días en el mes (esto debe tenerse en cuenta para cada mes)
  int diasDelMes = 31; // Por defecto, asumir 31 días
  if (mes == 4 || mes == 6 || mes == 9 || mes == 11)
    diasDelMes = 30; // Meses con 30 días
  if (mes == 2)
  { // Febrero, comprobar si es bisiesto
    if ((anio % 4 == 0 && anio % 100 != 0) || (anio % 400 == 0))
    {
      diasDelMes = 29; // Año bisiesto
    }
    else
    {
      diasDelMes = 28; // Año no bisiesto
    }
  }
  // Mostrar los días de la semana
  for (int i = 0; i < 7; i++)
  {
    tft.setCursor(20 + (i * 40), 10);
    tft.setTextColor(colors[colorIndex]);
    tft.setTextSize(1);
    tft.print(diasSemana[i]);
  }
  // Mostrar los días del mes
  int x = primerDia * 40;
  int y = 30;
  for (int i = 1; i <= diasDelMes; i++)
  {
    // Si es el primer día, comenzamos en la posición correcta según el primer día del mes
    int diaDeLaSemana = (primerDia + i - 1) % 7;
    // Imprimir el día
    tft.setCursor(20 + x, y);
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE);
    if (i == dia)
    {
      tft.setTextColor(TFT_YELLOW);
      tft.setTextSize(2);
    }
    else if (diaDeLaSemana == 5 || diaDeLaSemana == 6)
    {
      tft.setTextColor(TFT_ORANGE);
    }
    else
    { // Días regulares
      tft.setTextColor(TFT_WHITE);
    }
    tft.print(i);
    // Mover a la siguiente columna
    x += 40;
    // Si hemos llegado al final de la fila (7 días), pasamos a la siguiente fila
    if (x >= 7 * 40)
    {
      x = 0;
      y += 20;
    }
    String mesecillo = String(obtenerNombreMes(mes));
    tft.setTextColor(colors[colorIndex]);
    tft.setTextSize(3);
    tft.setCursor(5, 145);
    tft.print(mesecillo + " " + String(anio));
    tft.setCursor(290, 11);
    tft.print(String(h1));
    tft.setCursor(290, 41);
    tft.print(String(h2));
    tft.setCursor(290, 71);
    tft.print("-");
    tft.setCursor(290, 101);
    tft.print(String(m1));
    tft.setCursor(290, 131);
    tft.print(String(m2));
    tft.setCursor(280, 158);
    tft.setTextSize(1);
    tft.print(mineria.currentHashRate);
  }
}

/*
  Función: esPrimo
  Propósito: Determina si un número entero positivo es primo. Un número es primo si es mayor que 1
  y no tiene divisores positivos distintos de 1 y él mismo.

  Parámetros:
    - n: Número entero positivo (de tipo uint32_t) a verificar si es primo.

  Retorno:
    - Devuelve 'true' si el número n es primo.
    - Devuelve 'false' si el número n no es primo.

  Descripción:
    La función comienza verificando si el número es menor que 2, en cuyo caso devuelve 'false' ya que los números menores a 2 no son primos.
    Luego, maneja los casos especiales para los números 2 y 3, que son primos.
    Si el número es par, lo descarta de inmediato, ya que todos los números pares mayores que 2 no son primos.
    Para números impares mayores que 3, la función comprueba si existen divisores hasta la raíz cuadrada de n, incrementando de 2 en 2 (verificando solo números impares).
    Si se encuentra un divisor, devuelve 'false', y si no se encuentran divisores, devuelve 'true'.
*/

bool esPrimo(uint32_t n)
{
  // Si el número es menor que 2, no es primo
  if (n <= 1)
    return false;
  // Caso base para 2 y 3
  if (n == 2 || n == 3)
    return true;
  // Eliminar los números pares
  if (n % 2 == 0)
    return false;
  // Solo verificamos hasta la raíz cuadrada de n
  uint32_t limite = sqrt(n); // Usar la raíz cuadrada para limitar las verificaciones
  // Verificar divisibilidad solo con números impares
  for (uint32_t i = 3; i <= limite; i += 2)
  {
    if (n % i == 0)
    {
      return false; // Si encontramos un divisor, el número no es primo
    }
  }
  return true; // Si no encontramos divisores, el número es primo
}

/*
  Función: _fhour
  Propósito: Calcula la hora en formato decimal, donde la parte entera representa las horas completas y la parte decimal
  representa la fracción del minuto y el segundo transcurridos.

  Parámetros:
    - timeinfo: Una referencia constante a una estructura `tm` que contiene la información de tiempo (hora, minuto, segundo).
      - timeinfo.tm_hour: Hora (entero de 0 a 23).
      - timeinfo.tm_min: Minuto (entero de 0 a 59).
      - timeinfo.tm_sec: Segundo (entero de 0 a 59).

  Retorno:
    - Devuelve un valor de tipo `double` que representa la hora como un número decimal.
      - La parte entera de este valor corresponde a las horas (`timeinfo.tm_hour`).
      - La parte decimal se calcula a partir de los minutos y segundos, mapeándolos a un rango de [0.0, 1.0].

  Descripción:
    Esta función toma la hora, los minutos y los segundos de la estructura `tm` y los convierte en un valor decimal representando
    las horas transcurridas en el día. La función utiliza la función `map` para transformar los segundos del minuto (`timeinfo.tm_min * 60 + timeinfo.tm_sec`)
    en una fracción entre 0 y 1, que se suma a las horas.
    El valor retornado es una representación continua del tiempo, donde la parte entera son las horas y la parte decimal
    refleja el tiempo adicional transcurrido en fracciones de hora.
*/

double moonPhase::_fhour(const struct tm &timeinfo)
{
  return timeinfo.tm_hour + map((timeinfo.tm_min * 60) + timeinfo.tm_sec, 0, 3600, 0.0, 1.0);
}

/*
  Función: _Julian
  Propósito: Calcula la fecha juliana a partir de una fecha gregoriana especificada por el año, mes y día.

  Parámetros:
    - year: El año de la fecha en formato de 4 dígitos (ejemplo: 2025).
    - month: El mes de la fecha (1 = enero, 12 = diciembre).
    - day: El día del mes (valor decimal, por ejemplo, 15.5 representaría el 15 de un mes a la mitad del día).

  Retorno:
    - Devuelve un valor de tipo `double` que representa la fecha juliana correspondiente a la fecha proporcionada.
      La fecha juliana es el número de días (y fracción de días) desde el mediodía del 1 de enero de 4713 a.C.

  Descripción:
    Esta función convierte una fecha en el calendario gregoriano (proporcionada en términos de año, mes y día) a una fecha juliana,
    que es utilizada comúnmente en astronomía para cálculos de tiempos largos y para evitar los problemas con los calendarios.
    La función implementa la corrección del calendario gregoriano, que fue introducida después del 15 de octubre de 1582, momento en el cual
    el calendario juliano fue reemplazado. El ajuste es hecho a través del valor de `b`, que depende del año, mes y día.
    La fórmula utilizada es:
    - Si la fecha es posterior al 15 de octubre de 1582, se aplica el ajuste en `b`.
    - Se calcula el número de días del año (365.25 * year), y se ajusta con el factor `30.6001 * (month + 1)` para el mes y día.
    - Se suman todos los componentes, incluyendo el valor base de 1720994.5, que corresponde a la fecha juliana del 1 de enero de 4713 a.C.

    El valor retornado es un número decimal que representa el número de días transcurridos desde la fecha de inicio de la fecha juliana.

  Ejemplo:
    Para la fecha 15 de octubre de 2025, la función calcularía el número correspondiente en el calendario juliano.
*/

static double _Julian(int32_t year, int32_t month, const double &day)
{
  int32_t b, c, e;
  b = 0;
  if (month < 3)
  {
    year--;
    month += 12;
  }
  if (year > 1582 || (year == 1582 && month > 10) || (year == 1582 && month == 10 && day > 15))
  {
    int32_t a;
    a = year / 100;
    b = 2 - a + a / 4;
  }
  c = 365.25 * year;
  e = 30.6001 * (month + 1);
  return b + c + e + day + 1720994.5;
}

/*
  Función: _sun_position
  Propósito: Calcula la posición del sol en el eclíptico, dado un valor de fecha juliana (j).

  Parámetros:
    - j: El valor de la fecha juliana (un número decimal que representa la fecha en el calendario juliano).

  Retorno:
    - Devuelve un valor de tipo `double` que representa la longitud del sol en grados eclípticos (0-360°),
      es decir, la posición del sol a lo largo de su órbita en el plano de la eclíptica en un momento específico.

  Descripción:
    Esta función calcula la posición del sol a lo largo de su órbita elíptica en el sistema solar, basándose en la fecha juliana proporcionada.
    El cálculo se realiza utilizando la fórmula estándar de la astronomía para la longitud del sol, que incluye varios parámetros y correcciones
    para obtener una mayor precisión.

    El proceso incluye los siguientes pasos:
    1. Se calcula el ángulo de la órbita del sol (n) con la fórmula `360 / 365.2422 * j`, donde `j` es la fecha juliana.
    2. Se ajusta el valor de `n` para asegurarse de que esté dentro del rango de 0 a 360 grados.
    3. Se obtiene el ángulo eclíptico inicial (x), y se ajusta para asegurarse de que esté dentro del rango adecuado.
    4. Luego, se resuelve una ecuación iterativa (Newton-Raphson) para obtener una corrección precisa a la longitud del sol (`dl`).
    5. Finalmente, la longitud del sol `l` se calcula sumando un término constante a `v`, el valor obtenido de la función trigonométrica `atan`,
       que depende del ángulo corregido `e`.
    6. El valor final de `l` es normalizado para que esté entre 0 y 360 grados, y este es el valor retornado, que representa la longitud del sol.

    La longitud del sol es importante en astronomía para determinar diversas variables, como la posición del sol respecto a las estrellas fijas,
    y se utiliza en el cálculo de fenómenos astronómicos como los equinoccios y los solsticios.

  Ejemplo:
    Si se pasa un valor de `j = 2451545.0` (la fecha juliana correspondiente al 1 de enero de 2000), la función calcularía la posición del sol
    en esa fecha, devolviendo un valor entre 0 y 360 grados.
*/

static double _sun_position(const double &j)
{
  double n, x, e, l, dl, v;
  int32_t i;
  n = 360 / 365.2422 * j;
  i = n / 360;
  n = n - i * 360.0;
  x = n - 3.762863;
  x += (x < 0) ? 360 : 0;
  x *= DEG_TO_RAD;
  e = x;
  do
  {
    dl = e - .016718 * sin(e) - x;
    e = e - dl / (1 - .016718 * cos(e));
  } while (fabs(dl) >= 1e-12);
  v = 360 / PI * atan(1.01686011182 * tan(e / 2));
  l = v + 282.596403;
  i = l / 360;
  l = l - i * 360.0;
  return l;
}

/*
  Función: _moon_position
  Propósito: Calcula la posición de la Luna en el eclíptico, dado un valor de fecha juliana (j) y la longitud del sol (ls).

  Parámetros:
    - j: El valor de la fecha juliana (un número decimal que representa la fecha en el calendario juliano).
    - ls: La longitud del sol en grados eclípticos, calculada previamente. Representa la posición del sol en su órbita.

  Retorno:
    - Devuelve un valor de tipo `double` que representa la longitud de la Luna en grados eclípticos (0-360°), es decir, la posición de la Luna
      a lo largo de su órbita en el plano de la eclíptica en un momento específico.

  Descripción:
    Esta función calcula la posición de la Luna a lo largo de su órbita en el sistema solar, basándose en la fecha juliana proporcionada y
    la longitud del sol. El cálculo se realiza utilizando una serie de correcciones empíricas que se utilizan comúnmente en astronomía para
    modelar el movimiento lunar.

    El proceso incluye los siguientes pasos:
    1. Se calcula el ángulo del movimiento medio de la Luna (`ms`), que depende de la fecha juliana.
    2. Se calcula la longitud media del sol (`l`), ajustada para asegurarse de que esté dentro del rango adecuado.
    3. Se calcula la longitud media de la Luna (`mm`), ajustada también para estar dentro de un rango adecuado.
    4. Se calcula el valor de la variación lunar (`ev`), que es una corrección basada en la diferencia de las longitudes entre la Luna y el sol.
    5. Se calcula la excentricidad (`ae`), que es una corrección adicional que depende del ángulo `ms`.
    6. Se ajusta la longitud de la Luna (`mm`) mediante la corrección de variación y excentricidad.
    7. Se calcula la corrección de la Luna debido a la excentricidad (`ec`), que se basa en el ángulo `mm`.
    8. Finalmente, se ajusta la longitud de la Luna (`l`) utilizando la corrección de variación, la corrección de excentricidad y una corrección adicional
       que involucra un término de segundo orden.

    El resultado final de `l` es la posición de la Luna en el plano eclíptico, y es utilizado para cálculos de la fase lunar, eclipses y otros
    fenómenos relacionados con el movimiento de la Luna.

  Ejemplo:
    Si se pasa un valor de `j = 2451545.0` (la fecha juliana correspondiente al 1 de enero de 2000) y `ls = 280.0` (la longitud del sol en esa fecha),
    la función calcularía la posición de la Luna en esa fecha, devolviendo un valor entre 0 y 360 grados.
*/

static double _moon_position(const double &j, const double &ls)
{
  double ms, l, mm, ev, sms, ae, ec;
  int32_t i;
  ms = 0.985647332099 * j - 3.762863;
  ms += (ms < 0) ? 360.0 : 0;
  l = 13.176396 * j + 64.975464;
  i = l / 360;
  l = l - i * 360.0;
  l += (l < 0) ? 360 : 0;
  mm = l - 0.1114041 * j - 349.383063;
  i = mm / 360;
  mm -= i * 360.0;
  ev = 1.2739 * sin((2 * (l - ls) - mm) * DEG_TO_RAD);
  sms = sin(ms * DEG_TO_RAD);
  ae = 0.1858 * sms;
  mm += ev - ae - 0.37 * sms;
  ec = 6.2886 * sin(mm * DEG_TO_RAD);
  l += ev + ec - ae + 0.214 * sin(2 * mm * DEG_TO_RAD);
  l = 0.6583 * sin(2 * (l - ls) * DEG_TO_RAD) + l;
  return l;
}

/*
  Función: _getPhase
  Propósito: Calcula la fase de la Luna en un momento específico, dada una fecha y una hora.

  Parámetros:
    - year: El año en formato entero (ej. 2025).
    - month: El mes en formato entero (1-12).
    - day: El día del mes en formato entero (1-31).
    - hour: La hora del día en formato decimal (por ejemplo, 2.5 para 2 horas y 30 minutos).

  Retorno:
    - Devuelve un objeto de tipo `moonData_t` que contiene dos valores:
      1. El ángulo de la fase lunar en grados (0-360), representando la posición angular de la Luna en su órbita respecto al Sol.
      2. El porcentaje de la Luna iluminada (un valor entre 0 y 1).

  Descripción:
    Esta función calcula la fase de la Luna en un momento específico utilizando la fecha (año, mes, día) y la hora proporcionada. Se basa en dos cálculos
    astronómicos fundamentales:
    1. La posición del Sol en la órbita de la Tierra (utilizando la función `_sun_position`).
    2. La posición de la Luna en su órbita (utilizando la función `_moon_position`).

    El proceso general es el siguiente:
    1. Se calcula el valor de la fecha juliana con la función `_Julian` ajustando la hora proporcionada para obtener la fracción del día.
    2. Se calcula la posición del Sol (`ls`) usando la fecha juliana.
    3. Se calcula la posición de la Luna (`lm`) usando la fecha juliana y la posición del Sol.
    4. Se calcula el ángulo de la fase lunar (`angle`) como la diferencia entre la posición de la Luna y la del Sol.
       - Si el ángulo es negativo, se ajusta para que esté en el rango de 0-360 grados.
    5. El ángulo calculado (`angle`) representa la fase lunar, indicando la posición de la Luna en su órbita.
    6. Se calcula el porcentaje de la Luna iluminada, utilizando la fórmula `(1 - cos(angle)) / 2`. Esta fórmula se basa en el ángulo de la fase y describe la proporción de la superficie lunar iluminada por el Sol.

    El objeto `moonData_t` que se retorna contiene:
    - `angle`: El ángulo de fase de la Luna (0-360 grados), que indica en qué fase se encuentra la Luna en relación con el Sol (nueva, creciente, llena, menguante, etc.).
    - El porcentaje de la Luna iluminada, que varía de 0 (Luna nueva) a 1 (Luna llena).

  Ejemplo:
    Si se pasa `year = 2025`, `month = 2`, `day = 10`, y `hour = 12.0` (mediodía), la función calculará la fase lunar y el porcentaje de iluminación para ese momento específico, devolviendo un objeto `moonData_t` con los resultados.

  Notas:
    - El valor de `angle` se encuentra en el rango [0, 360], donde:
      - 0 grados = Luna nueva
      - 180 grados = Luna llena
    - El valor de la fracción iluminada es un número real entre 0 y 1, donde:
      - 0 = Luna nueva
      - 1 = Luna llena
*/

moonData_t moonPhase::_getPhase(const int32_t year, const int32_t month, const int32_t day, const double &hour)
{
  const double j{_Julian(year, month, (double)day + hour / 24.0) - 2444238.5};
  const double ls{_sun_position(j)};
  const double lm{_moon_position(j, ls)};
  double angle = lm - ls;
  angle += (angle < 0) ? 360 : 0;
  const moonData_t returnValue{
      (int32_t)angle,
      (1.0 - cos((lm - ls) * DEG_TO_RAD)) / 2};
  return returnValue;
}

/*
  Función: incrementCounter
  Propósito: Incrementa un contador de segundos y actualiza el índice de color y el nombre mostrado según el valor del contador.

  Descripción:
    Esta función incrementa un contador de segundos (`secondCounter`) y, cuando alcanza el valor de 59, se reinicia a 0 y se selecciona un nuevo índice de color aleatorio.
    Además, actualiza una variable de texto (`nombrecillo`) dependiendo del valor de `colorIndex`, el cual se ajusta cada vez que el contador llega a 59.

  Proceso:
    1. Se incrementa el valor de `secondCounter` en cada llamada a la función.
    2. Si el valor de `secondCounter` alcanza 59 (lo que indica que un minuto ha pasado), se realiza lo siguiente:
       - Se selecciona aleatoriamente un nuevo valor para `colorIndex` usando `esp_random()`, el cual es un número aleatorio entre 0 y el número total de colores disponibles en el array `colors`.
       - El contador `secondCounter` se reinicia a 0.
    3. Si el valor de `secondCounter` es menor que 59, se comprueba si `colorIndex` es par o impar:
       - Si `colorIndex` es par, se asigna el texto `" - M 8 A X -"`.
       - Si `colorIndex` es impar, se asigna el texto `"- MvIiIaX -"`.

  Efectos secundarios:
    - Se actualiza `colorIndex` con un valor aleatorio cuando el contador llega a 59.
    - Se cambia el valor de `nombrecillo` dependiendo del valor de `colorIndex`.

  Ejemplo:
    Si `secondCounter` es 59, el valor de `colorIndex` será actualizado a un nuevo valor aleatorio, y `nombrecillo` podría cambiar a `" - M 8 A X -"`, dependiendo si el valor de `colorIndex` es par o impar.

  Notas:
    - La función `esp_random()` genera un número aleatorio entre 0 y el tamaño del array `colors` para seleccionar un índice aleatorio de color.
    - El valor de `secondCounter` se mantiene entre 0 y 59, representando los segundos de un minuto.
*/

void incrementCounter()
{
  secondCounter++;
  if (secondCounter >= 59)
  {
    colorIndex = esp_random() % (sizeof(colors) / sizeof(colors[0]));
    secondCounter = 0;
  }
  else
  {
    if (colorIndex < 4)
    {
      nombrecillo = "EHD-MDDD";
    }
    else if (colorIndex % 2 == 0)
    {
      nombrecillo = " - M 8 A X -";
    }
    else
    {
      nombrecillo = "- MvIiIaX -";
    }
  }
}

/*
  Función: tDisplay_MinerScreen
  Propósito: Muestra la pantalla de minería con datos en tiempo real como hashrate, shares, temperatura, entre otros.

  Descripción:
    Esta función se encarga de mostrar la pantalla principal de la minería, que incluye datos como el hashrate actual, el total de hashes, los bloques validados, la temperatura y más.
    La información mostrada se actualiza cada vez que se llama a esta función, utilizando datos proporcionados por otras funciones auxiliares como `getMiningData`, `getClockData`, y `getCoinData`.
    Además, el fondo de la pantalla se actualiza según ciertos parámetros, como la hora del día o el estado de ciertos contadores.

  Proceso:
    1. **Obtención de datos**:
       Se obtienen varios datos relacionados con la minería, la hora, y la criptomoneda actual utilizando funciones como `getMiningData`, `getClockData`, y `getCoinData`.

    2. **Renderización del fondo**:
       Se dibuja un fondo de pantalla utilizando `background.pushImage()` con la imagen de la pantalla de minería (`MinerScreen`).

    3. **Hashrate de día y noche**:
       Dependiendo de la hora actual (`horiac`), se elige un color de texto diferente para el hashrate (negro para la noche y blanco para el día). Esto se renderiza en la pantalla con `render.rdrawString()`.

    4. **Mostrar datos de minería**:
       Se dibujan otros datos de la minería como el total de hashes, las plantillas de bloques, la mejor dificultad, el número de shares completados y la hora de minería. Estos datos se renderizan utilizando `render.drawString()` y `render.rdrawString()` con diferentes tamaños y colores de texto.

    5. **Mostrar texto de nombre**:
       Se muestra un texto personalizado basado en la variable `nombrecillo` que cambia dependiendo del índice de color (`colorIndex`).

    6. **Mostrar bloques válidos**:
       Se verifica si el número de bloques válidos (`valids`) es 0, y si es así, se muestra en color rojo, de lo contrario, se muestra en color verde/amarillo.

    7. **Mostrar temperatura**:
       Se imprime la temperatura de la minería utilizando `render.rdrawString()`.

    8. **Mostrar hora**:
       Se muestra la hora de minería (`currentTime`) con un tamaño de fuente pequeño.

    9. **Efecto visual con "..." o ". ."**:
       Basado en la hora de minería (últimos dos dígitos), se decide si mostrar "..." o ". ." de manera intermitente. Esto se hace dependiendo de si el valor es par o impar.

    10. **Empujar imagen final**:
        Finalmente, se actualiza la pantalla con `background.pushSprite()` para reflejar todos los cambios realizados en la interfaz.

  Efectos secundarios:
    - La pantalla se actualiza con la nueva información de minería y hora.
    - Se modifican los elementos visuales basados en condiciones como la hora del día o la paridad de los segundos.

  Notas:
    - Se utiliza un fondo estático que se actualiza con la imagen de la minería.
    - El uso de `esp_random()` para la intermitencia de los puntos (". .", "...") agrega un efecto visual aleatorio que hace que la pantalla sea más dinámica.
    - Los datos de minería y de reloj se obtienen de otras funciones, lo que implica que las funciones como `getMiningData`, `getClockData` y `getCoinData` deben estar implementadas correctamente para obtener la información correcta.
    - Añadido RSSI de la conexión WiFi, en la parte central superior de la pantalla.
*/

void tDisplay_MinerScreen(unsigned long mElapsed)
{
  mining_data data = getMiningData(mElapsed);
  clock_data dataa = getClockData(mElapsed);
  monedilla = getCoinData(mElapsed);
  relojete = getClockData(mElapsed);
  mineria = getMiningData(mElapsed);
  int horiac = dataa.currentTime.substring(0, 2).toInt();
  int lastTwoInt = atoi(data.timeMining.c_str() + data.timeMining.length() - 2);
  incrementCounter();
  actualizarcalen = 0;
  actualizarc = 0;
  actual = 0;
  actuanot = 0;
  correccion = 0;
  background.pushImage(0, 0, MinerWidth, MinerHeight, MinerScreen);
  Serial.printf("M8AX - >>> Completados %s Share(s), %s Khashes, Prom. Hashrate %s KH/s %s°\n",
                data.completedShares.c_str(), data.totalKHashes.c_str(), data.currentHashRate.c_str(), data.temp.c_str());
  // Total Mhashes
  render.setFontSize(18);
  render.rdrawString(data.totalMHashes.c_str(), 268, 139, TFT_BLACK);
  // Block templates
  render.setFontSize(18);
  render.drawString(data.templates.c_str(), 186, 20, 0xDEDB);
  // Best diff
  render.drawString(data.bestDiff.c_str(), 186, 47, 0xDEDB);
  // 32Bit shares
  render.setFontSize(18);
  render.drawString(data.completedShares.c_str(), 186, 74, 0xDEDB);
  // Hores
  render.setFontSize(14);
  render.rdrawString(data.timeMining.c_str(), 319, 104, 0xDEDB);
  // By M8AX
  if (lastTwoInt % 30 == 0)
  {
    colorrrr = esp_random() % 9;
  }
  if (horiac >= 22 || horiac < 8)
  {
    background.setFreeFont(FSSBO9);
    background.setTextSize(0);
    background.setTextDatum(TL_DATUM);
    background.setTextColor(TFT_BLACK);
  }
  else
  {
    background.setFreeFont(FSSBO9);
    background.setTextSize(0);
    background.setTextDatum(TL_DATUM);
    background.setTextColor(coloris[colorrrr]);
  }
  background.drawString(nombrecillo, (strcmp(nombrecillo, "EHD-MDDD") == 0) ? 20 : 23, 101, GFXFF);
  // Valid Blocks
  int validInt = atoi(data.valids.c_str());
  if (validInt == 0)
  {
    render.setFontSize(24);
    render.drawString(data.valids.c_str(), 281, 56, TFT_RED);
  }
  else
  {
    render.setFontSize(24);
    render.drawString(data.valids.c_str(), 281, 56, TFT_GREENYELLOW);
  }
  // Print Temp
  render.setFontSize(10);
  render.rdrawString(data.temp.c_str(), 239, 1, TFT_BLACK);
  render.setFontSize(4);
  render.rdrawString(String(0).c_str(), 244, 2, TFT_BLACK);
  // Print Hour
  render.setFontSize(10);
  render.rdrawString(data.currentTime.c_str(), 286, 1, TFT_BLACK);
  if (lastTwoInt % 2 == 0)
  {
    // Es par
    background.setFreeFont(FSSBO9);
    background.setTextSize(2);
    background.setTextDatum(TL_DATUM);
    background.setTextColor(colors[colorIndex]);
    if ((esp_random() % 10 + 1) % 2 == 0)
    {
      background.drawString("...", 275, 68, GFXFF);
    }
    else
    {
      background.drawString(". .", 275, 68, GFXFF);
    }
  }
  else
  {
    // Es impar
    background.setFreeFont(FSSBO9);
    background.setTextSize(2);
    background.setTextDatum(TL_DATUM);
    background.setTextColor(colors[colorIndex]);
    if ((esp_random() % 10 + 1) % 2 == 0)
    {
      background.drawString(".  ", 275, 68, GFXFF);
    }
    else
    {
      background.drawString("   ", 275, 68, GFXFF);
    }
  }
  // Hashrate Day And Night
  if (horiac >= 22 || horiac < 8)
  {
    render.setFontSize(30);
    render.setFontColor(TFT_BLACK);
    render.rdrawString(data.currentHashRate.c_str(), 128, 120, TFT_BLACK);
    background.pushSprite(0, 0);
    tft.setCursor(4, 160);
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE);
    tft.print("Max HR " + String(maxkh) + " Min HR " + String(minkh));
    tft.setTextColor(TFT_ORANGE);
    tft.setCursor(137, 8);
    tft.print("RSSI " + String(WiFi.RSSI()));
  }
  else
  {
    render.setFontSize(30);
    render.setFontColor(TFT_WHITE);
    render.rdrawString(data.currentHashRate.c_str(), 128, 120, TFT_WHITE);
    background.pushSprite(0, 0);
    tft.setCursor(4, 160);
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE);
    tft.print("Max HR " + String(maxkh) + " Min HR " + String(minkh));
    tft.setTextColor(TFT_WHITE);
    tft.setCursor(137, 8);
    tft.print("RSSI " + String(WiFi.RSSI()));
  }
}

/*
  Función: tDisplay_ClockScreen
  Propósito: Mostrar la pantalla del reloj con datos en tiempo real, incluyendo la hora, fecha, precio del BTC, hashrate y fase lunar.

  Descripción:
    Esta función actualiza la pantalla con información relevante del reloj y la minería, incluyendo la hora actual, la fecha con el día de la semana, el hashrate, el precio del BTC y la fase de la luna.
    La pantalla de fondo se actualiza antes de dibujar los textos, asegurando una visualización clara de la información.

  Proceso:
    1. **Obtención de datos**:
       Se recuperan los datos de la minería, el reloj y la moneda usando `getClockData()`, `getMiningData()` y `getCoinData()`.

    2. **Renderización del fondo**:
       Se dibuja la imagen de fondo de la pantalla de reloj con `background.pushImage()`.

    3. **Muestra de hashrate**:
       Se imprime el hashrate en negro en la pantalla utilizando `render.rdrawString()`.

    4. **Muestra del precio del BTC**:
       Se imprime el precio actual del BTC en la parte superior derecha de la pantalla.

    5. **Muestra de la altura del bloque**:
       Se imprime la altura actual del bloque minado usando `render.rdrawString()`.

    6. **Muestra de la hora y la fecha**:
       - La hora actual se imprime con una fuente grande y en color blanco.
       - La fecha incluye el día de la semana obtenido con `obtenerDiaSemana()` y se imprime en color basado en `colors[colorIndex]`.

    7. **Cálculo de la fase de la luna**:
       - Se extrae la fecha y la hora actual en variables separadas.
       - Se usa la estructura `tm` y `mktime()` para convertir la fecha a formato `time_t`.
       - Se obtiene la fase de la luna con `myMoonPhase.getPhase()`.
       - Se calcula el porcentaje iluminado de la luna y se imprime en la pantalla.

    8. **Actualización final**:
       Se actualiza la pantalla con `background.pushSprite()` para mostrar todos los datos en tiempo real.

  Efectos secundarios:
    - Se modifica el fondo de pantalla con la imagen de `minerClockScreen`.
    - Se cambian los valores mostrados según la fecha, la hora y los datos obtenidos en tiempo real.
    - Se usa un color dinámico para algunos textos según el `colorIndex`.

  Notas:
    - La función `obtenerDiaSemana()` convierte una fecha en string en el nombre del día de la semana correspondiente.
    - Se usa `mktime()` para calcular la fecha en `time_t`, lo que permite obtener la fase lunar correctamente.
    - `esp_random()` no se usa aquí, pero podría añadirse para variar la visualización de ciertos elementos dinámicos.
*/

void tDisplay_ClockScreen(unsigned long mElapsed)
{
  clock_data data = getClockData(mElapsed);
  mining_data dataa = getMiningData(mElapsed);
  monedilla = getCoinData(mElapsed);
  relojete = getClockData(mElapsed);
  mineria = getMiningData(mElapsed);
  moonData_t moon;
  int horiac = dataa.currentTime.substring(0, 2).toInt();
  unsigned long segundo = timeClient.getSeconds();
  actualizarcalen = 0;
  actualizarc = 0;
  actual = 0;
  actuanot = 0;
  correccion = 0;
  incrementCounter();
  // Print background screen
  background.pushImage(0, 0, minerClockWidth, minerClockHeight, minerClockScreen);
  Serial.printf("M8AX - >>> Completados %s Share(s), %s Khashes, Prom. Hashrate %s KH/s %s°\n",
                data.completedShares.c_str(), data.totalKHashes.c_str(), data.currentHashRate.c_str(), dataa.temp.c_str());
  // Print BTC Price
  background.setFreeFont(FSSB9);
  background.setTextSize(1);
  background.setTextDatum(TL_DATUM);
  background.setTextColor(TFT_BLACK);
  background.drawString(data.btcPrice.c_str(), 198, 3, GFXFF);
  // Print BlockHeight
  render.setFontSize(18);
  render.rdrawString(data.blockHeight.c_str(), 254, 139, TFT_BLACK);
  // Print Hour And Date
  background.setFreeFont(FF23);
  background.setTextSize(2);
  background.setTextColor(TFT_WHITE);
  background.drawString(data.currentTime.c_str(), 130, 29, GFXFF);
  background.setFreeFont(FSSBO9);
  background.setTextSize(1);
  background.setTextDatum(TL_DATUM);
  background.setTextColor(colors[colorIndex]);
  std::string quediase = obtenerDiaSemana(std::string(data.currentDate.c_str()));
  String fechacondiasemana = data.currentDate + " " + String(quediase.c_str());
  background.drawString(fechacondiasemana, 164, 86, GFXFF);
  int dia = data.currentDate.substring(0, 2).toInt();
  int mes = data.currentDate.substring(3, 5).toInt();
  int anio = data.currentDate.substring(6, 10).toInt();
  int hora = data.currentTime.substring(0, 2).toInt();
  int minuto = data.currentTime.substring(3, 5).toInt();
  // Inicializar la estructura tm
  struct tm timeinfo;
  timeinfo.tm_year = anio - 1900; // Año desde 1900
  timeinfo.tm_mon = mes - 1;      // Mes (0 = enero)
  timeinfo.tm_mday = dia;         // Día del mes
  timeinfo.tm_hour = hora;        // Hora
  timeinfo.tm_min = minuto;       // Minutos
  timeinfo.tm_sec = segundo;      // Segundos
  timeinfo.tm_isdst = -1;         // Determina si es horario de verano (automático)
  // Convertir a time_t
  time_t cadenaDeTiempo = mktime(&timeinfo);
  moon = mymoonPhase.getPhase(cadenaDeTiempo);
  double porcentajeIluminado = moon.percentLit * 100;
  char porcentajeTexto[10];
  snprintf(porcentajeTexto, sizeof(porcentajeTexto), "%.2f%%", porcentajeIluminado);
  background.setFreeFont(FSSBO9);
  background.setTextSize(1);
  background.setTextDatum(TL_DATUM);
  background.setTextColor(colors[colorIndex]);
  String textoFinal = "Luna.Ilu - " + String(porcentajeTexto);
  background.drawString(textoFinal, 156, 106, GFXFF);
  float porcentaje = ((24 - (hora + minuto / 60.0)) / 24.0) * 100;
  int angulo = map(porcentaje, 0, 100, 0, 360);
  int centroX = 130, centroY = 100, radio = 15;
  // Hashrate
  render.setFontSize(21);
  render.setCursor(19, 126);
  if (horiac >= 22 || horiac < 8)
  {
    render.setFontColor(TFT_BLACK);
    render.rdrawString(data.currentHashRate.c_str(), 94, 133, TFT_BLACK);
    background.pushSprite(0, 0);
    tft.setCursor(4, 123);
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE);
    tft.print("+ " + String(maxkh) + " - " + String(minkh));
    tft.setCursor(4, 162);
    tft.print("BTC AYER - " + subebaja);
    if (segundo % 2 == 0)
    {
      tft.fillCircle(123, 20, 8, colors[colorIndex]);
      tft.fillCircle(centroX, centroY, radio, TFT_BLACK);
      for (int a = 0; a <= angulo; a += 5)
      {
        float rad = a * PI / 180.0;
        int x = centroX + cos(rad) * radio;
        int y = centroY + sin(rad) * radio;
        tft.drawLine(centroX, centroY, x, y, colors[colorIndex]);
      }
    }
    else
    {
      tft.fillCircle(123, 20, 8, TFT_BLACK);
    }
  }
  else
  {
    render.setFontColor(TFT_WHITE);
    render.rdrawString(data.currentHashRate.c_str(), 94, 133, TFT_WHITE);
    background.pushSprite(0, 0);
    tft.setCursor(4, 123);
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE);
    tft.print("+ " + String(maxkh) + " - " + String(minkh));
    tft.setCursor(4, 162);
    tft.print("BTC AYER - " + subebaja);
    if (segundo % 2 == 0)
    {
      tft.fillCircle(123, 20, 8, colors[colorIndex]);
      tft.fillCircle(centroX, centroY, radio, TFT_BLACK);
      for (int a = 0; a <= angulo; a += 5)
      {
        float rad = a * PI / 180.0;
        int x = centroX + cos(rad) * radio;
        int y = centroY + sin(rad) * radio;
        tft.drawLine(centroX, centroY, x, y, colors[colorIndex]);
      }
    }
    else
    {
      tft.fillCircle(123, 20, 8, TFT_BLACK);
    }
  }
}

/*
  Función: tDisplay_GlobalHashScreen
  Propósito: Mostrar información global de minería, incluyendo el precio del BTC, hashrate global, dificultad de red y progreso del bloque.

  Descripción:
    Esta función actualiza la pantalla con información en tiempo real relacionada con el estado de la red de minería.
    Muestra el precio del BTC, el hashrate global, la altura del bloque, la dificultad de la red y el porcentaje de progreso hacia el siguiente bloque.

  Proceso:
    1. **Obtención de datos**:
       Se recuperan los datos de la moneda (`getCoinData()`), minería (`getMiningData()`) y reloj (`getClockData()`).

    2. **Renderización del fondo**:
       Se dibuja la imagen de fondo de la pantalla con `background.pushImage()`.

    3. **Impresión de datos clave**:
       - **Precio del BTC**: Se imprime en la parte superior derecha.
       - **Hora actual**: Se imprime en la parte superior izquierda en color blanco.
       - **Hashrate actual**: Se muestra en un color dinámico basado en `colorIndex`.
       - **Último bloque de la pool**: Se imprime con color `0x9C92` en la parte derecha de la pantalla.
       - **Dificultad de la red**: Se imprime en la parte derecha con el mismo color `0x9C92`.
       - **Hashrate global**: Se imprime en la parte inferior en negro.
       - **Altura del bloque**: Se muestra en grande en el centro de la pantalla.

    4. **Dibujo del rectángulo de progreso**:
       - Se calcula la longitud del rectángulo en función del porcentaje de progreso del bloque (`progressPercent`).
       - Se usa `fillRect()` para representar gráficamente el progreso hacia el siguiente bloque.

    5. **Impresión de bloques restantes**:
       - Se imprime el número de bloques restantes hasta el próximo ajuste de dificultad.

    6. **Actualización final**:
       - Se actualiza la pantalla con `background.pushSprite()` para mostrar todos los datos en tiempo real.

  Efectos secundarios:
    - Se modifica el fondo de pantalla con `globalHashScreen`.
    - Se cambia dinámicamente el color del hashrate en función de `colorIndex`.
    - Se actualiza el progreso del bloque de forma visual con una barra de progreso.

  Notas:
    - `data.progressPercent` determina el avance visual del bloque.
    - `colorIndex` se actualiza con `incrementCounter()`, lo que puede modificar los colores en cada actualización.
    - `data.globalHashRate` representa la potencia total de cómputo de la red en KH/s.
*/

void tDisplay_GlobalHashScreen(unsigned long mElapsed)
{
  coin_data data = getCoinData(mElapsed);
  mining_data dataa = getMiningData(mElapsed);
  monedilla = getCoinData(mElapsed);
  relojete = getClockData(mElapsed);
  mineria = getMiningData(mElapsed);
  incrementCounter();
  actualizarcalen = 0;
  // Print background screen
  background.pushImage(0, 0, globalHashWidth, globalHashHeight, globalHashScreen);
  Serial.printf("M8AX - >>> Completados %s Share(s), %s Khashes, Prom. Hashrate %s KH/s %s°\n",
                data.completedShares.c_str(), data.totalKHashes.c_str(), data.currentHashRate.c_str(), dataa.temp.c_str());
  // Print BTC Price
  background.setFreeFont(FSSB9);
  background.setTextSize(1);
  background.setTextDatum(TL_DATUM);
  background.setTextColor(TFT_BLACK);
  background.drawString(data.btcPrice.c_str(), 198, 3, GFXFF);
  // Print Hour
  background.setFreeFont(FSSB9);
  background.setTextSize(1);
  background.setTextDatum(TL_DATUM);
  background.setTextColor(TFT_WHITE);
  background.drawString(data.currentTime.c_str(), 46, 0, GFXFF);
  // Print HR
  background.setFreeFont(FSSB9);
  background.setTextSize(1);
  background.setTextDatum(TL_DATUM);
  background.setTextColor(colors[colorIndex]);
  background.drawString(String(data.currentHashRate) + " KH/s", 1, 64, GFXFF);
  // Print Last Pool Block
  background.setFreeFont(FSS9);
  background.setTextDatum(TR_DATUM);
  background.setTextColor(0x9C92);
  background.drawString(data.halfHourFee.c_str(), 302, 52, GFXFF);
  // Print Difficulty
  background.setFreeFont(FSS9);
  background.setTextDatum(TR_DATUM);
  background.setTextColor(0x9C92);
  background.drawString(data.netwrokDifficulty.c_str(), 302, 88, GFXFF);
  // Print Global Hashrate
  render.setFontSize(17);
  render.rdrawString(data.globalHashRate.c_str(), 274, 142, TFT_BLACK);
  // Print BlockHeight
  render.setFontSize(28);
  render.rdrawString(data.blockHeight.c_str(), 140, 104, 0xDEDB);
  // Draw percentage rectangle
  int x2 = 2 + (138 * data.progressPercent / 100);
  background.fillRect(2, 149, x2, 168, 0xDEDB);
  // Print Remaining BLocks
  background.setTextFont(FONT2);
  background.setTextSize(1);
  background.setTextDatum(MC_DATUM);
  background.setTextColor(TFT_BLACK);
  background.drawString(data.remainingBlocks.c_str(), 72, 159, FONT2);
  // Push prepared background to screen
  background.pushSprite(0, 0);
}

/**
 * Actualiza la pantalla con datos en tiempo real sobre minería de Bitcoin.
 *
 * Esta función obtiene y muestra información relevante en la pantalla, incluyendo:
 * - Hashrate actual del minero.
 * - Altura del bloque en la blockchain.
 * - Hora actual con segundos sincronizados.
 * - Precio de Bitcoin en tiempo real.
 * - Temperatura del minero.
 * - Un mensaje que cambia cada 30 segundos.
 *
 * @param mElapsed Tiempo transcurrido en milisegundos desde la última actualización.
 */

void tDisplay_BTCprice(unsigned long mElapsed)
{
  clock_data data = getClockData(mElapsed);
  mining_data dataa = getMiningData(mElapsed);
  monedilla = getCoinData(mElapsed);
  relojete = getClockData(mElapsed);
  mineria = getMiningData(mElapsed);
  unsigned long segundo = timeClient.getSeconds();
  int segundos = segundo % 60;
  int horiac = dataa.currentTime.substring(0, 2).toInt();
  incrementCounter();
  actualizarcalen = 0;
  actualizarc = 0;
  actual = 0;
  actuanot = 0;
  correccion = 0;
  // Print background screen
  background.pushImage(0, 0, priceScreenWidth, priceScreenHeight, priceScreen);
  Serial.printf("M8AX - >>> Completados %s Share(s), %s Khashes, Prom. Hashrate %s KH/s %s°\n",
                data.completedShares.c_str(), data.totalKHashes.c_str(), data.currentHashRate.c_str(), dataa.temp.c_str());
  // Hashrate
  render.setFontSize(22);
  if (horiac >= 22 || horiac < 8)
  {
    render.setFontColor(TFT_BLACK);
    render.rdrawString(data.currentHashRate.c_str(), 94, 131, TFT_BLACK);
  }
  else
  {
    render.setFontColor(TFT_WHITE);
    render.rdrawString(data.currentHashRate.c_str(), 94, 131, TFT_WHITE);
  }
  // Print BlockHeight
  render.setFontSize(18);
  render.rdrawString(data.blockHeight.c_str(), 254, 138, TFT_WHITE);
  // Print Hour
  background.setFreeFont(FSSB9);
  background.setTextSize(1);
  background.setTextDatum(TL_DATUM);
  background.setTextColor(TFT_BLACK);
  char segundosFormateados[3];
  sprintf(segundosFormateados, "%02d", segundos);
  background.drawString(data.currentTime.c_str() + String(":") + String(segundosFormateados), 177, 3, GFXFF);
  background.drawString(dataa.temp.c_str(), 256, 3, GFXFF);
  render.setFontSize(4);
  render.rdrawString(String(0).c_str(), 281, 5, TFT_BLACK);
  // Print BTC Price
  background.setFreeFont(FSSB9);
  background.setTextDatum(TR_DATUM);
  background.setTextSize(2);
  background.setTextColor(TFT_WHITE);
  background.drawString(data.btcPrice.c_str(), 245, 26, GFXFF);
  background.setTextSize(1);
  background.setTextColor(colors[colorIndex]);

  if (segundos <= 30)
  {
    textoFinalm8ax1 = "El Futuro No Esta";
    textoFinalm8ax2 = "Establecido, Solo";
    textoFinalm8ax3 = "Existe El Que";
    textoFinalm8ax4 = "Nosotros Hacemos.";
  }
  else
  {
    textoFinalm8ax1 = "Por Muchas Vueltas";
    textoFinalm8ax2 = "Que Demos, Siempre";
    textoFinalm8ax3 = "Tendremos El";
    textoFinalm8ax4 = "Culo Atras...";
  }

  background.drawString(textoFinalm8ax1.c_str(), 310, 60, GFXFF);
  background.drawString(textoFinalm8ax2.c_str(), 310, 76, GFXFF);
  background.drawString(textoFinalm8ax3.c_str(), 310, 95, GFXFF);
  background.drawString(textoFinalm8ax4.c_str(), 310, 111, GFXFF);
  background.pushSprite(0, 0);
  tft.setCursor(4, 123);
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE);
  tft.print("+ " + String(maxkh) + " - " + String(minkh));
  tft.setCursor(4, 162);
  tft.print("BTC AYER - " + subebaja);
}

/*
  Función: tDisplay_m8axScreen1
  Propósito: Mostrar información sobre la fecha, hora, estado de la minería y fase de la luna en la pantalla.

  Descripción:
    Esta función obtiene los datos actuales de minería, hora y fecha, calcula la fase de la luna, y los muestra en la pantalla junto con una imagen de fondo.

  Proceso:
    1. **Obtención de datos**:
       - Se recuperan datos de minería (`getMiningData()`), reloj (`getClockData()`) y moneda (`getCoinData()`).
       - Se obtiene el día de la semana con `obtenerDiaSemana()` basado en la fecha actual.

    2. **Construcción de cadenas de texto**:
       - Se genera la fecha completa con el día de la semana (`fechacondiasemana`).
       - Se muestra la fecha en la pantalla con `background.drawString()`.

    3. **Conversión de fecha y hora a estructura `tm`**:
       - Se extraen día, mes, año, hora y minutos desde las cadenas `dataa.currentDate` y `dataa.currentTime`.
       - Se llena la estructura `tm` con estos valores y se convierte a `time_t` con `mktime()`.

    4. **Cálculo de la fase lunar**:
       - Se obtiene la fase de la luna con `mymoonPhase.getPhase(cadenaDeTiempo)`.
       - Se calcula el porcentaje de iluminación de la luna y se convierte a texto (`porcentajeTexto`).
       - Se construye la cadena `datoslunarelogrande` con la información lunar.

    5. **Renderización de la interfaz**:
       - Se imprime la imagen de fondo `ImagenM8AX` en la pantalla.
       - Se muestra el estado de minería en el puerto serie con `Serial.printf()`.
       - Se imprimen en la pantalla:
         - **Hora actual** (con fuente grande).
         - **Fecha completa** con el día de la semana.
         - **Estado de iluminación lunar**.

    6. **Actualización final**:
       - Se usa `background.pushSprite(0, 0);` para refrescar la pantalla con los nuevos datos.

  Notas:
    - `data.currentTime` muestra la hora actual en formato HH:MM.
    - `dataa.currentDate` es la fecha en formato DD/MM/YYYY.
    - `mymoonPhase.getPhase()` calcula la fase lunar basada en `time_t`.
    - `percentLit` indica el porcentaje de iluminación de la luna.

  Efectos secundarios:
    - Se actualizan dinámicamente la hora, fecha y fase lunar en pantalla.
    - Se imprime información de minería en el puerto serie.
*/

void tDisplay_m8axScreen1(unsigned long mElapsed)
{
  mining_data data = getMiningData(mElapsed);
  clock_data dataa = getClockData(mElapsed);
  monedilla = getCoinData(mElapsed);
  relojete = getClockData(mElapsed);
  mineria = getMiningData(mElapsed);
  moonData_t moon;
  incrementCounter();
  actualizarcalen = 0;
  actualizarc = 0, mirarTiempo = 0;
  std::string quediase = obtenerDiaSemana(std::string(dataa.currentDate.c_str()));
  String fechita = dataa.currentDate + " " + String(quediase.c_str());
  String fechacondiasemana = dataa.currentDate + " " + String(quediase.c_str());
  background.drawString(fechacondiasemana, 164, 86, GFXFF);
  int dia = dataa.currentDate.substring(0, 2).toInt();
  int mes = dataa.currentDate.substring(3, 5).toInt();
  int anio = dataa.currentDate.substring(6, 10).toInt();
  int hora = dataa.currentTime.substring(0, 2).toInt();
  int minuto = dataa.currentTime.substring(3, 5).toInt();
  unsigned long segundo = timeClient.getSeconds();
  // Inicializar la estructura tm
  struct tm timeinfo;
  timeinfo.tm_year = anio - 1900; // Año desde 1900
  timeinfo.tm_mon = mes - 1;      // Mes (0 = enero)
  timeinfo.tm_mday = dia;         // Día del mes
  timeinfo.tm_hour = hora;        // Hora
  timeinfo.tm_min = minuto;       // Minutos
  timeinfo.tm_sec = segundo;      // Segundos
  timeinfo.tm_isdst = -1;         // Determina si es horario de verano (automático)
  // Convertir a time_t
  time_t cadenaDeTiempo = mktime(&timeinfo);
  moon = mymoonPhase.getPhase(cadenaDeTiempo);
  double porcentajeIluminado = moon.percentLit * 100;
  char porcentajeTexto[10];
  snprintf(porcentajeTexto, sizeof(porcentajeTexto), "%.2f%%", porcentajeIluminado);
  String datoslunarelogrande = "Luna.Ilu - " + String(porcentajeTexto);
  background.pushImage(0, 0, ImagenM8AXWidth, ImagenM8AXHeight, ImagenM8AX);
  Serial.printf("M8AX - >>> Completados %s Share(s), %s Khashes, Prom. Hashrate %s KH/s %s°\n",
                data.completedShares.c_str(), data.totalKHashes.c_str(), data.currentHashRate.c_str(), data.temp.c_str());
  background.setFreeFont(FSSB9);
  background.setTextSize(7);
  background.setTextColor(colors[colorIndex]);
  background.drawString(data.currentTime.c_str(), 0, 33, GFXFF);
  background.setTextSize(2);
  background.drawString(fechita, 34, 0, GFXFF);
  background.drawString(datoslunarelogrande, 12, 140, GFXFF);
  background.pushSprite(0, 0);
}

/*
 * Función: tDisplay_m8axScreen2
 * ------------------------------------
 *  Esta función actualiza la pantalla con información de minería y reloj.
 *
 *  Parámetro:
 *    - mElapsed: Tiempo transcurrido desde la última actualización.
 *
 *  Flujo de ejecución:
 *  1. Obtiene los datos de minería, moneda y reloj.
 *  2. Reinicia varias variables de control usadas en la lógica de actualización.
 *  3. Dibuja la imagen de fondo de la pantalla.
 *  4. Muestra en la consola (Serial) la información de minería:
 *      - Shares completados
 *      - Total de Khashes minados
 *      - Hashrate promedio
 *      - Temperatura
 *  5. Comprueba si el usuario ha minado alguna recompensa:
 *      - Si no ha obtenido validaciones, muestra el mensaje "AUN NO ERES RICO".
 *      - Si ha obtenido al menos una validación, muestra "ERES MILLONARIO".
 *  6. Configura el estilo del texto en la pantalla.
 *  7. Muestra en pantalla:
 *      - Hashrate actual en la parte superior.
 *      - Mensaje sobre la temperatura.
 *      - Dirección de BTC parcialmente oculta con un identificador.
 *      - Dirección y puerto de la pool.
 *      - Otros valores visuales relacionados con la minería.
 *  8. Muestra la hora actual y el tiempo total de minería.
 */

void tDisplay_m8axScreen2(unsigned long mElapsed)
{
  mining_data data = getMiningData(mElapsed);
  monedilla = getCoinData(mElapsed);
  relojete = getClockData(mElapsed);
  mineria = getMiningData(mElapsed);
  incrementCounter();
  actualizarcalen = 0, actualizarc = 0, actual = 0, actuanot = 0, correccion = 0;
  background.pushSprite(0, 0);
  background.pushImage(0, 0, ImagenM8AXWidth, ImagenM8AXHeight, ImagenM8AX);
  Serial.printf("M8AX - >>> Completados %s Share(s), %s Khashes, Prom. Hashrate %s KH/s %s°\n",
                data.completedShares.c_str(), data.totalKHashes.c_str(), data.currentHashRate.c_str(), data.temp.c_str());
  int millonario = atoi(data.valids.c_str());
  const char *mensajericono;
  if (millonario == 0)
  {
    mensajericono = "--- AUN NO ERES RICO ---";
  }
  else
  {
    mensajericono = "--- ERES MILLONARIO ---";
  }
  background.setFreeFont(FSSB9);
  background.setTextSize(6);
  background.setTextColor(colors[colorIndex]);
  background.drawString(data.currentHashRate.c_str(), 0, 2, GFXFF);
  background.setFreeFont(FSSBO9);
  background.setTextSize(1);
  background.setTextDatum(TL_DATUM);
  background.setTextColor(colors[colorIndex]);
  tft.setTextColor(colors[colorIndex]);
  background.drawString("TEMP", 16, 93, GFXFF);
  background.drawString(mensajericono, 80, 150, GFXFF);
  background.setTextSize(2);
  background.drawString(data.temp.c_str(), 15, 111, GFXFF);
  background.setTextSize(1);
  String wallet = Settings.BtcWallet;
  int puntoIndex = wallet.indexOf('.');
  String direccion_btc;
  String identificador;
  String nueva_wallet;
  if (puntoIndex != -1)
  {
    // Si hay punto, separar dirección e identificador
    direccion_btc = wallet.substring(0, puntoIndex);
    identificador = wallet.substring(puntoIndex + 1);
    // Generar la nueva wallet con identificador + últimos 6 caracteres
    nueva_wallet = identificador + direccion_btc.substring(direccion_btc.length() - 6);
  }
  else
  {
    // Si no hay punto, toda la cadena es la dirección BTC
    direccion_btc = wallet;
    // Generar la nueva wallet con los últimos 12 caracteres
    nueva_wallet = "xxxxxxxxxxxx" + direccion_btc.substring(direccion_btc.length() - 12);
  }
  tft.setCursor(82, 117);
  tft.print("WBTC " + nueva_wallet);
  background.drawString(Settings.PoolAddress + ":" + String(Settings.PoolPort), 80, 127, GFXFF);
  background.drawString("c", 62, 107, GFXFF);
  background.setTextColor(colors[colorIndex]);
  background.drawString("d", 125, 92, GFXFF);
  background.drawString("KH/s", 275, 87, GFXFF);
  background.drawString(relojete.currentTime, 12, 150, GFXFF);
  render.setCursor(11, 128);
  render.setFontSize(14);
  render.rdrawString(data.timeMining.c_str(), 232, 92, colors[colorIndex]);
}

/*
 * Función: tDisplay_m8axScreen3
 * ------------------------------------
 *  Esta función actualiza la pantalla con información del reloj y el calendario.
 *
 *  Parámetro:
 *    - mElapsed: Tiempo transcurrido desde la última actualización.
 *
 *  Flujo de ejecución:
 *  1. Obtiene los datos de:
 *      - Reloj (fecha y hora).
 *      - Minería (estadísticas de hashrate y shares).
 *      - Moneda y reloj adicionales.
 *  2. Incrementa el contador de actualización y reinicia la variable de corrección.
 *  3. Muestra en la consola (Serial) los datos de minería:
 *      - Shares completados.
 *      - Total de Khashes minados.
 *      - Hashrate promedio.
 *      - Temperatura del dispositivo.
 *  4. Extrae la fecha actual y la convierte a valores enteros:
 *      - Día, mes y año.
 *  5. Extrae los dígitos individuales de la hora y los minutos.
 *  6. Cada 30 ciclos (o la primera vez que se ejecuta):
 *      - Dibuja la imagen de fondo.
 *      - Llama a `mostrarCalendario()` para visualizar la fecha y hora de forma gráfica.
 *  7. Incrementa los contadores de actualización para controlar la frecuencia de actualización del calendario.
 */

void tDisplay_m8axScreen3(unsigned long mElapsed)
{
  clock_data dataa = getClockData(mElapsed);
  mining_data data = getMiningData(mElapsed);
  monedilla = getCoinData(mElapsed);
  relojete = getClockData(mElapsed);
  mineria = getMiningData(mElapsed);
  incrementCounter();
  correccion = 0;
  Serial.printf("M8AX - >>> Completados %s Share(s), %s Khashes, Prom. Hashrate %s KH/s %s°\n",
                data.completedShares.c_str(), data.totalKHashes.c_str(), data.currentHashRate.c_str(), data.temp.c_str());
  int dia = dataa.currentDate.substring(0, 2).toInt();
  int mes = dataa.currentDate.substring(3, 5).toInt();
  int anio = dataa.currentDate.substring(6, 10).toInt();
  int num1 = dataa.currentTime.charAt(0) - '0'; // Primer dígito de la hora
  int num2 = dataa.currentTime.charAt(1) - '0'; // Segundo dígito de la hora
  int num3 = dataa.currentTime.charAt(3) - '0'; // Primer dígito de los minutos
  int num4 = dataa.currentTime.charAt(4) - '0'; // Segundo dígito de los minutos
  if (actualizarcalen % 30 == 0 || actual == 0)
  {
    background.pushImage(0, 0, ImagenM8AXWidth, ImagenM8AXHeight, ImagenM8AX);
    background.pushSprite(0, 0);
    mostrarCalendario(dia, mes, anio, num1, num2, num3, num4);
  }
  actualizarcalen++;
  actual++;
  actualizarc = 0;
}

/*
 * Función: tDisplay_m8axScreen4
 * ------------------------------------
 *  Esta función actualiza la pantalla con información de minería, un número aleatorio primo y datos del sistema.
 *
 *  Parámetro:
 *    - mElapsed: Tiempo transcurrido desde la última actualización.
 *
 *  Flujo de ejecución:
 *  1. Obtiene los datos de:
 *      - Minería (estadísticas de hashrate y shares).
 *      - Moneda.
 *      - Reloj.
 *  2. Incrementa el contador de actualización y reinicia la variable `actualizarcalen`.
 *  3. Genera un número aleatorio (`rndnumero`) usando `esp_random()`.
 *  4. Muestra en la consola (Serial) los datos de minería:
 *      - Shares completados.
 *      - Total de Khashes minados.
 *      - Hashrate promedio.
 *      - Temperatura del dispositivo.
 *  5. Si el número aleatorio es primo (`esPrimo(rndnumero)`):
 *      - Muestra una pantalla de créditos.
 *      - Dibuja el número primo en la pantalla.
 *  6. Cada 30 ciclos (`actualizarc % 30 == 0`):
 *      - Muestra la pantalla de créditos.
 *      - Genera y muestra números de la lotería "Primitiva".
 *      - Divide los números en dos grupos de tres para mostrarlos en la pantalla.
 *      - Muestra datos del sistema, como:
 *          - Hora actual.
 *          - Dirección IP pública y local.
 *          - Espacio libre en la memoria Flash.
 *          - Memoria RAM total (Heap + PSRAM).
 *          - Frecuencia de la CPU y número de núcleos.
 *  7. Incrementa `actualizarc` para controlar la frecuencia de actualización.
 */

void tDisplay_m8axScreen4(unsigned long mElapsed)
{
  mining_data data = getMiningData(mElapsed);
  monedilla = getCoinData(mElapsed);
  relojete = getClockData(mElapsed);
  mineria = getMiningData(mElapsed);
  incrementCounter();
  actualizarcalen = 0;
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(1);
  rndnumero = esp_random();
  Serial.printf("M8AX - >>> Completados %s Share(s), %s Khashes, Prom. Hashrate %s KH/s %s°\n",
                data.completedShares.c_str(), data.totalKHashes.c_str(), data.currentHashRate.c_str(), data.temp.c_str());
  if (esPrimo(rndnumero))
  {
    String rndnumeroStr = String(rndnumero);
    tft.fillRect(167, 137, 123, 19, TFT_BLACK);
    tft.setCursor(169, 139);
    tft.setTextSize(2);
    tft.print(rndnumeroStr.c_str());
  }
  if (actualizarc % 30 == 0)
  {
    background.pushImage(0, 0, CreditosScreenWidth, CreditosScreenHeight, CreditosScreen);
    background.pushSprite(0, 0);
    String numerosp = generarNumerosPrimitiva();
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(1);
    String primeros_tres = "";
    String ultimos_tres = "";
    int comaIndex = numerosp.indexOf(',');

    // Primeros tres números
    primeros_tres = numerosp.substring(0, comaIndex); // Extrae los tres primeros números
    numerosp.remove(0, comaIndex + 1);                // Elimina lo que ya hemos extraído (primeros tres)

    comaIndex = numerosp.indexOf(',');
    primeros_tres += "," + numerosp.substring(0, comaIndex); // Agregar el segundo número
    numerosp.remove(0, comaIndex + 1);                       // Elimina el segundo número

    comaIndex = numerosp.indexOf(',');
    primeros_tres += "," + numerosp.substring(0, comaIndex); // Agregar el tercer número
    numerosp.remove(0, comaIndex + 1);                       // Elimina el tercer número

    // Últimos tres números
    ultimos_tres = numerosp.substring(0); // Lo que queda es la parte de los últimos tres números
    tft.setCursor(27, 123);
    tft.print(primeros_tres);
    tft.setCursor(27, 134);
    tft.print(ultimos_tres);
    tft.setCursor(212, 4);
    tft.setTextColor(colors[colorIndex]);
    tft.setTextSize(2);
    tft.print(data.currentTime.c_str());
    tft.setTextSize(1);
    tft.setCursor(225, 28);
    tft.print("PUBLIC IP");
    tft.setCursor(225, 39);
    tft.print(getPublicIP());
    tft.setCursor(225, 50);
    tft.print("LOCAL IP");
    tft.setCursor(225, 61);
    tft.print(WiFi.localIP());
    tft.setCursor(225, 72);
    tft.print("FREE FLASH");
    tft.setCursor(225, 83);
    tft.println(ESP.getFreeSketchSpace());
    tft.setCursor(225, 94);
    tft.print("RAM TOTAL");
    unsigned long ramTotal = ESP.getHeapSize();
    unsigned long psramTotal = ESP.getPsramSize(); // Método directo para PSRAM
    tft.setCursor(225, 105);
    tft.print(ramTotal + psramTotal);
    tft.setCursor(225, 116);
    tft.print("CPU: ");
    tft.print(getCpuFrequencyMhz());
    tft.print(" MHz,");
    tft.print(ESP.getChipCores());
    tft.print("C");
  }
  actualizarc++;
}

/*
 * Función: tDisplay_m8axScreen5
 * ------------------------------------
 *  Esta función actualiza la pantalla con información de minería, elige una imagen aleatoria y muestra el texto desplazándose.
 *
 *  Parámetro:
 *    - mElapsed: Tiempo transcurrido desde la última actualización.
 *
 *  Flujo de ejecución:
 *  1. Incrementa el contador global `incrementCounter()`.
 *  2. Reinicia `actualizarcalen` a 0.
 *  3. Obtiene datos de:
 *      - Minería (shares, hashrate, temperatura).
 *      - Moneda.
 *      - Reloj.
 *  4. Formatea los datos de minería en un buffer (`bbuffer`) y los imprime en la consola (Serial).
 *  5. Si `columna` es 0, se genera un número aleatorio (`random_number`) entre 1 y 4 para seleccionar una imagen.
 *  6. Dependiendo del número aleatorio:
 *      - `1`: Muestra `ImagenM8AX`.
 *      - `2`: Muestra `M8AXRelojLunar`.
 *      - `3`: Muestra `M8AXQuote1`.
 *      - `4`: Muestra `M8AXQuote2`.
 *  7. Muestra el texto con el color de índice `colorIndex`, ajusta el tamaño del texto y lo imprime en la posición `(2, columna)`.
 *  8. Incrementa `columna` para el desplazamiento del texto en la pantalla.
 *  9. Si `columna` alcanza 166, se reinicia a 0 para repetir el ciclo.
 */

void tDisplay_m8axScreen5(unsigned long mElapsed)
{
  incrementCounter();
  actualizarcalen = 0;
  mining_data data = getMiningData(mElapsed);
  monedilla = getCoinData(mElapsed);
  relojete = getClockData(mElapsed);
  mineria = getMiningData(mElapsed);
  char bbuffer[100];
  snprintf(bbuffer, sizeof(bbuffer), "C. %s Share(s) %s KH. M-HR %s KH/s - %sg",
           data.completedShares.c_str(), data.totalKHashes.c_str(), data.currentHashRate.c_str(), data.temp.c_str());
  Serial.printf("M8AX - >>> Completados %s Share(s), %s Khashes, Prom. Hashrate %s KH/s %s°\n",
                data.completedShares.c_str(), data.totalKHashes.c_str(), data.currentHashRate.c_str(), data.temp.c_str());
  String lineaa = bbuffer;
  if (columna == 0)
  {
    random_number = 1 + (esp_random() % 4);
  }
  if (random_number == 1)
  {
    tft.pushImage(0, 0, ImagenM8AXWidth, ImagenM8AXHeight, ImagenM8AX);
  }
  else if (random_number == 2)
  {
    tft.pushImage(0, 0, M8AXRelojLunarWidth, M8AXRelojLunarHeight, M8AXRelojLunar);
  }
  else if (random_number == 3)
  {
    tft.pushImage(0, 0, M8AXQuote1Width, M8AXQuote1Height, M8AXQuote1);
  }
  else if (random_number == 4)
  {
    tft.pushImage(0, 0, M8AXQuote2Width, M8AXQuote2Height, M8AXQuote2);
  }
  tft.setTextColor(colors[colorIndex]);
  tft.setTextSize(1);
  tft.setCursor(2, columna);
  tft.print(lineaa);
  columna += 1;
  if (columna >= 166)
  {
    columna = 0; // Reinicia la columna
  }
}

/*
 * Función: tDisplay_m8axScreen6
 * ------------------------------------
 *  Esta función actualiza la pantalla con la imagen final y muestra datos de minería.
 *
 *  Parámetro:
 *    - mElapsed: Tiempo transcurrido desde la última actualización.
 *
 *  Flujo de ejecución:
 *  1. Incrementa el contador global de actualización (`incrementCounter()`).
 *  2. Reinicia múltiples variables de control (`actualizarcalen`, `actualizarc`, `actual`, `actuanot`, `correccion`).
 *  3. Obtiene los datos de:
 *      - Minería (shares completados, hashrate, temperatura).
 *      - Información de moneda.
 *      - Hora actual del reloj.
 *  4. Imprime en la consola (Serial) las estadísticas de minería:
 *      - Shares completados.
 *      - Total de Khashes minados.
 *      - Hashrate promedio.
 *      - Temperatura del dispositivo.
 *  5. Muestra en pantalla la imagen final (`ImagenFinalPM8AX`).
 *  6. Establece el color y tamaño del texto para la información superpuesta.
 *  7. Muestra en pantalla:
 *      - La hora actual en `(64,160)`.
 *      - El hashrate en `(137,160)`, seguido de `"KH/s "`.
 *      - La temperatura en `(236,160)`, seguida de `"g"` (grados).
 */

void tDisplay_m8axScreen6(unsigned long mElapsed)
{
  incrementCounter();
  actualizarcalen = 0;
  actualizarc = 0;
  actual = 0;
  actuanot = 0;
  correccion = 0;
  mining_data data = getMiningData(mElapsed);
  monedilla = getCoinData(mElapsed);
  relojete = getClockData(mElapsed);
  mineria = getMiningData(mElapsed);
  Serial.printf("M8AX - >>> Completados %s Share(s), %s Khashes, Prom. Hashrate %s KH/s %s°\n",
                data.completedShares.c_str(), data.totalKHashes.c_str(), data.currentHashRate.c_str(), data.temp.c_str());
  tft.pushImage(0, 0, ImagenFinalPWidth, ImagenFinalPHeight, ImagenFinalPM8AX); // Muestra la imagen
  tft.setTextColor(colors[colorIndex]);
  tft.setTextSize(1);
  tft.setCursor(64, 160);
  tft.print(data.currentTime);
  tft.setCursor(137, 160);
  tft.print(data.currentHashRate + "KH/s ");
  tft.setCursor(236, 160);
  tft.print(data.temp + "g");
}

/*
 * Función: tDisplay_m8axScreen7
 * ------------------------------------
 *  Esta función actualiza la pantalla con un reloj, la fecha y los datos de minería.
 *
 *  Parámetro:
 *    - mElapsed: Tiempo transcurrido desde la última actualización.
 *
 *  Flujo de ejecución:
 *  1. Obtiene los datos actualizados de:
 *      - Minería (`data`): información sobre shares, Khashes, hashrate y temperatura.
 *      - Reloj (`dataa`): hora y fecha actual.
 *      - Moneda (`monedilla`).
 *  2. Convierte `data.valids` a un entero (`millonario`).
 *  3. Reinicia `actualizarc` y llama a `incrementCounter()`.
 *  4. Si `actualizarcalen` es 0, inicia el cliente NTP (`timeClient.begin()`).
 *  5. Cada 1800 ciclos, actualiza la hora desde el servidor NTP (`timeClient.update()`).
 *  6. Obtiene los segundos actuales del servidor NTP (`timeClient.getSeconds()`).
 *  7. Extrae y convierte la hora y los minutos de `dataa.currentTime`.
 *  8. Calcula los segundos actuales (`segundos = segundo % 60`).
 *  9. Imprime en consola los datos de minería.
 * 10. Obtiene el día de la semana con `obtenerDiaSemana()`.
 * 11. Extrae el día, mes y año de la fecha (`dataa.currentDate`).
 * 12. Convierte la hora a formato de 12 horas si es necesario.
 * 13. Incrementa `actualizarcalen`.
 * 14. Llama a `dibujarReloj()` con la hora, fecha y datos de minería para actualizar la pantalla.
 * 15. Reinicia `actual` a 0.
 */

void tDisplay_m8axScreen7(unsigned long mElapsed)
{
  mining_data data = getMiningData(mElapsed);
  clock_data dataa = getClockData(mElapsed);
  monedilla = getCoinData(mElapsed);
  relojete = getClockData(mElapsed);
  mineria = getMiningData(mElapsed);
  int millonario = atoi(data.valids.c_str());
  actualizarc = 0;
  incrementCounter();
  if (actualizarcalen == 0)
  {
    timeClient.begin();
  }
  if (actualizarcalen % 1800 == 0)
  {
    timeClient.update();
  }
  unsigned long segundo = timeClient.getSeconds();
  int horas = dataa.currentTime.substring(0, 2).toInt();
  int horras = horas;
  int minutos = dataa.currentTime.substring(3, 5).toInt();
  int segundos = segundo % 60;
  Serial.printf("M8AX - >>> Completados %s Share(s), %s Khashes, Prom. Hashrate %s KH/s %s°\n",
                data.completedShares.c_str(), data.totalKHashes.c_str(), data.currentHashRate.c_str(), data.temp.c_str());
  std::string quediase = obtenerDiaSemana(std::string(dataa.currentDate.c_str()));
  String dia = dataa.currentDate.substring(0, 2);
  String mes = dataa.currentDate.substring(3, 5);
  String anio = dataa.currentDate.substring(6, 10);

  // Ajusta al formato de 12 horas
  if (horas >= 12)
  {
    horas -= 12;
  }

  // Llama a la función para dibujar el reloj
  actualizarcalen++;
  dibujarReloj(horras, minutos, segundos, dia, mes, anio, quediase, data.currentHashRate.c_str(), data.temp.c_str(), millonario);
  actual = 0;
}

/*
 * Función: tDisplay_m8axScreen8
 * ------------------------------------
 *  Esta función actualiza la pantalla con una frase aleatoria, imagen de fondo y datos de minería.
 *
 *  Parámetro:
 *    - mElapsed: Tiempo transcurrido desde la última actualización.
 *
 *  Flujo de ejecución:
 *  1. Obtiene los datos actualizados de:
 *      - Minería (`data`): información sobre shares, Khashes, hashrate y temperatura.
 *      - Reloj (`dataa`): hora y fecha actual.
 *      - Moneda (`monedilla`).
 *  2. Llama a `incrementCounter()`.
 *  3. Imprime en consola los datos de minería.
 *  4. Si `actualizarcalen` es múltiplo de 15 o `correccion` es 0:
 *      - Aumenta el contador de frases (`numfrases`).
 *      - Elige una imagen aleatoria (1 a 4) para mostrar de fondo:
 *        1. Imagen de M8AX.
 *        2. Reloj Lunar.
 *        3. Cita 1.
 *        4. Cita 2.
 *      - Muestra la imagen seleccionada en el fondo.
 *      - Solicita una cita aleatoria a través de la función `getQuote()`.
 *      - Muestra la cita en la pantalla con `displayQuote()`.
 *      - Imprime el número de la frase.
 *  5. Establece la corrección de visualización a 1 (`correccion = 1`).
 *  6. Convierte `data.valids` a un entero (`millonario`).
 *  7. Si `millonario` es 0, imprime "NO RICO". De lo contrario, imprime "SI RICO" junto con los datos de minería (hora, fecha, hashrate, temperatura).
 *  8. Incrementa `actualizarcalen`, reinicia `actualizarc` y `actuanot`.
 */

void tDisplay_m8axScreen8(unsigned long mElapsed)
{
  mining_data data = getMiningData(mElapsed);
  clock_data dataa = getClockData(mElapsed);
  monedilla = getCoinData(mElapsed);
  relojete = getClockData(mElapsed);
  mineria = getMiningData(mElapsed);
  incrementCounter();
  Serial.printf("M8AX - >>> Completados %s Share(s), %s Khashes, Prom. Hashrate %s KH/s %s°\n",
                data.completedShares.c_str(), data.totalKHashes.c_str(), data.currentHashRate.c_str(), data.temp.c_str());
  if (actualizarcalen % 15 == 0 || correccion == 0)
  {
    numfrases++;
    int random_number = 1 + (esp_random() % 4);
    if (random_number == 1)
    {
      background.pushImage(0, 0, ImagenM8AXWidth, ImagenM8AXHeight, ImagenM8AX);
      background.pushSprite(0, 0);
    }
    else if (random_number == 2)
    {
      background.pushImage(0, 0, M8AXRelojLunarWidth, M8AXRelojLunarHeight, M8AXRelojLunar);
      background.pushSprite(0, 0);
    }
    else if (random_number == 3)
    {
      background.pushImage(0, 0, M8AXQuote1Width, M8AXQuote1Height, M8AXQuote1);
      background.pushSprite(0, 0);
    }
    else if (random_number == 4)
    {
      background.pushImage(0, 0, M8AXQuote2Width, M8AXQuote2Height, M8AXQuote2);
      background.pushSprite(0, 0);
    }
    HTTPClient http;
    String quote = getQuote();
    displayQuote(quote);
    quote = "";
    quote.reserve(0);
    tft.setTextColor(TFT_WHITE);
    tft.setCursor(112, 160);
    tft.setTextSize(1);
    tft.print("M8AX-Frase Num-" + String(numfrases));
    tft.setTextColor(TFT_WHITE);
    tft.setCursor(1, 6);
    tft.setTextSize(1);
    correccion = 1;
    int millonario = atoi(data.valids.c_str());
    if (millonario == 0)
    {
      tft.print(data.currentTime + " - " + dataa.currentDate + " - " + data.currentHashRate + " KH/s - " + data.temp + " Grados." + " NO RICO");
    }
    else
    {
      tft.print(data.currentTime + " - " + dataa.currentDate + " - " + data.currentHashRate + " KH/s - " + data.temp + " Grados." + " SI RICO");
    }
  }
  actualizarcalen++;
  actualizarc = 0;
  actuanot = 0;
}

/*
 * Función: tDisplay_m8axScreen9
 * ------------------------------------
 *  Esta función actualiza la pantalla con la hora en formato romano, la fecha, el estado de la minería,
 *  la temperatura del procesador, y un indicador de riqueza ("ERES RICO" o "NO RICO").
 *
 *  Parámetros:
 *    - mElapsed: Tiempo transcurrido desde la última actualización.
 *
 *  Flujo de ejecución:
 *  1. Obtiene los datos actualizados de:
 *      - Minería (`data`): información sobre shares completados, Khashes, hashrate y temperatura.
 *      - Reloj (`dataa`): hora y fecha actual.
 *      - Moneda (`monedilla`).
 *  2. Convierte la hora, minutos, segundos, día, mes, año, y temperatura en formato romano utilizando `convertirARomanos()`.
 *  3. Si `segundos % 59 == 0`, reinicia el contador de actualización de pantalla (`actualizarc`).
 *  4. Si `actualizarc` es 0, selecciona aleatoriamente una imagen de fondo (entre 4 opciones).
 *  5. Muestra la imagen seleccionada como fondo.
 *  6. Imprime en la pantalla la hora, los valores romanos de día, mes, año y temperatura, junto con el estado de la minería.
 *  7. Si el valor de `data.valids` es 1, imprime "ERES RICO", de lo contrario, imprime "NO RICO".
 *  8. Muestra los valores de "V.BLOCKS" (número de bloques de minería) y la temperatura del procesador en formato romano.
 *  9. El texto se muestra con colores aleatorios seleccionados de un arreglo `colors[]`.
 *  10. Incrementa `actualizarc` y realiza actualizaciones periódicas de la pantalla.
 */

void tDisplay_m8axScreen9(unsigned long mElapsed)
{
  mining_data data = getMiningData(mElapsed);
  clock_data dataa = getClockData(mElapsed);
  monedilla = getCoinData(mElapsed);
  relojete = getClockData(mElapsed);
  mineria = getMiningData(mElapsed);
  int millonario = atoi(data.valids.c_str());
  incrementCounter();
  unsigned long segundo = timeClient.getSeconds();
  int horas = dataa.currentTime.substring(0, 2).toInt();
  int minutos = dataa.currentTime.substring(3, 5).toInt();
  int segundos = segundo % 60;
  if (segundos % 59 == 0)
  {
    actualizarc = 0;
  }
  std::string quediase = obtenerDiaSemana(std::string(dataa.currentDate.c_str()));
  int dia = dataa.currentDate.substring(0, 2).toInt();
  int mes = dataa.currentDate.substring(3, 5).toInt();
  int anio = dataa.currentDate.substring(6, 10).toInt();
  String hRoman = convertirARomanos(horas);
  String mRoman = convertirARomanos(minutos);
  String sRoman = convertirARomanos(segundos);
  String RDia = convertirARomanos(dia);
  String RMes = convertirARomanos(mes);
  String RAnio = convertirARomanos(anio);
  String TempCPU = convertirARomanos(atoi(data.temp.c_str()));
  if (actualizarc == 0)
  {
    random_number = 1 + (esp_random() % 4);
  }
  if (random_number == 1)
  {
    background.pushImage(0, 0, ImagenM8AXWidth, ImagenM8AXHeight, ImagenM8AX);
    background.pushSprite(0, 0);
  }
  else if (random_number == 2)
  {
    background.pushImage(0, 0, M8AXRelojLunarWidth, M8AXRelojLunarHeight, M8AXRelojLunar);
    background.pushSprite(0, 0);
  }
  else if (random_number == 3)
  {
    background.pushImage(0, 0, M8AXQuote1Width, M8AXQuote1Height, M8AXQuote1);
    background.pushSprite(0, 0);
  }
  else if (random_number == 4)
  {
    background.pushImage(0, 0, M8AXQuote2Width, M8AXQuote2Height, M8AXQuote2);
    background.pushSprite(0, 0);
  }
  actualizarc++;
  Serial.printf("M8AX - >>> Completados %s Share(s), %s Khashes, Prom. Hashrate %s KH/s %s°\n",
                data.completedShares.c_str(), data.totalKHashes.c_str(), data.currentHashRate.c_str(), data.temp.c_str());
  tft.setTextSize(6);
  tft.setCursor(3, 2);
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.setTextColor(colors[colorI]);
  tft.print(hRoman);
  tft.setCursor(3, 62);
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.setTextColor(colors[colorI]);
  tft.print(mRoman);
  tft.setCursor(3, 122);
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.setTextColor(colors[colorI]);
  tft.print(sRoman);
  tft.setTextSize(1);
  tft.setCursor(250, 2);
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.setTextColor(colors[colorI]);
  tft.print("d " + RDia);
  tft.setCursor(250, 16);
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.setTextColor(colors[colorI]);
  tft.print("m " + RMes);
  tft.setCursor(250, 30);
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.setTextColor(colors[colorI]);
  tft.print("a " + RAnio);
  tft.setCursor(250, 44);
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.setTextColor(colors[colorI]);
  tft.print(String(quediase.c_str()));
  tft.setCursor(250, 58);
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.setTextColor(colors[colorI]);
  tft.print(String(data.currentHashRate.c_str()) + " KH/s");
  if (millonario == 1)
  {
    tft.setCursor(250, 72);
    colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
    tft.setTextColor(colors[colorI]);
    tft.print("ERES RICO");
  }
  else
  {
    tft.setCursor(250, 72);
    colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
    tft.setTextColor(colors[colorI]);
    tft.print("NO RICO");
  }
  tft.setCursor(250, 93);
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.setTextColor(colors[colorI]);
  tft.print("V.BLOCKS");
  tft.setCursor(260, 109);
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.setTextColor(colors[colorI]);
  tft.setTextSize(5);
  tft.print(millonario);
  tft.setTextSize(1);
  tft.setCursor(250, 153);
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.setTextColor(colors[colorI]);
  tft.print("T " + TempCPU + "g");
}

/*
 * Función: tDisplay_m8axScreen10
 * ------------------------------------
 *  Esta función actualiza la pantalla con la hora en formato binario, la fecha en formato binario,
 *  el estado de la minería, la temperatura del procesador y un indicador de riqueza ("ERES RICO" o "NO RICO").
 *
 *  Parámetros:
 *    - mElapsed: Tiempo transcurrido desde la última actualización.
 *
 *  Flujo de ejecución:
 *  1. Obtiene los datos actualizados de:
 *      - Minería (`data`): información sobre shares completados, Khashes, hashrate y temperatura.
 *      - Reloj (`dataa`): hora y fecha actual.
 *      - Moneda (`monedilla`).
 *  2. Convierte la hora, minutos, segundos, día, mes, año y temperatura en formato binario utilizando `ABinario()`.
 *  3. Si `segundos % 59 == 0`, reinicia el contador de actualización de pantalla (`actualizarc`).
 *  4. Si `actualizarc` es 0, selecciona aleatoriamente una imagen de fondo (entre 4 opciones).
 *  5. Muestra la imagen seleccionada como fondo.
 *  6. Imprime en la pantalla la hora, los valores binarios de día, mes, año y temperatura, junto con el estado de la minería.
 *  7. Si el valor de `data.valids` es 1, imprime "ERES RICO", de lo contrario, imprime "NO RICO".
 *  8. Muestra los valores de "V.BLOCKS" (número de bloques de minería) y la temperatura del procesador en formato binario.
 *  9. El texto se muestra con colores aleatorios seleccionados de un arreglo `colors[]`.
 *  10. Incrementa `actualizarc` y realiza actualizaciones periódicas de la pantalla.
 */

void tDisplay_m8axScreen10(unsigned long mElapsed)
{
  mining_data data = getMiningData(mElapsed);
  clock_data dataa = getClockData(mElapsed);
  monedilla = getCoinData(mElapsed);
  relojete = getClockData(mElapsed);
  mineria = getMiningData(mElapsed);
  int millonario = atoi(data.valids.c_str());
  incrementCounter();
  unsigned long segundo = timeClient.getSeconds();
  int horas = dataa.currentTime.substring(0, 2).toInt();
  int minutos = dataa.currentTime.substring(3, 5).toInt();
  int segundos = segundo % 60;
  if (segundos % 59 == 0)
  {
    actualizarc = 0;
  }
  std::string quediase = obtenerDiaSemana(std::string(dataa.currentDate.c_str()));
  int dia = dataa.currentDate.substring(0, 2).toInt();
  int mes = dataa.currentDate.substring(3, 5).toInt();
  int anio = dataa.currentDate.substring(6, 10).toInt();
  String hRoman = ABinario(horas);
  String mRoman = ABinario(minutos);
  String sRoman = ABinario(segundos);
  String RDia = ABinario(dia);
  String RMes = ABinario(mes);
  String RAnio = ABinario(anio);
  String TempCPU = ABinario(atoi(data.temp.c_str()));
  if (actualizarc == 0)
  {
    random_number = 1 + (esp_random() % 4);
  }
  if (random_number == 1)
  {
    background.pushImage(0, 0, ImagenM8AXWidth, ImagenM8AXHeight, ImagenM8AX);
    background.pushSprite(0, 0);
  }
  else if (random_number == 2)
  {
    background.pushImage(0, 0, M8AXRelojLunarWidth, M8AXRelojLunarHeight, M8AXRelojLunar);
    background.pushSprite(0, 0);
  }
  else if (random_number == 3)
  {
    background.pushImage(0, 0, M8AXQuote1Width, M8AXQuote1Height, M8AXQuote1);
    background.pushSprite(0, 0);
  }
  else if (random_number == 4)
  {
    background.pushImage(0, 0, M8AXQuote2Width, M8AXQuote2Height, M8AXQuote2);
    background.pushSprite(0, 0);
  }
  actualizarc++;
  Serial.printf("M8AX - >>> Completados %s Share(s), %s Khashes, Prom. Hashrate %s KH/s %s°\n",
                data.completedShares.c_str(), data.totalKHashes.c_str(), data.currentHashRate.c_str(), data.temp.c_str());
  tft.setTextSize(6);
  tft.setCursor(3, 2);
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.setTextColor(colors[colorI]);
  tft.print(hRoman);
  tft.setCursor(3, 62);
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.setTextColor(colors[colorI]);
  tft.print(mRoman);
  tft.setCursor(3, 122);
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.setTextColor(colors[colorI]);
  tft.print(sRoman);
  tft.setTextSize(1);
  tft.setCursor(240, 2);
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.setTextColor(colors[colorI]);
  tft.print("d " + RDia);
  tft.setCursor(240, 16);
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.setTextColor(colors[colorI]);
  tft.print("m " + RMes);
  tft.setCursor(240, 30);
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.setTextColor(colors[colorI]);
  tft.print("a " + RAnio);
  tft.setCursor(240, 44);
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.setTextColor(colors[colorI]);
  tft.print(String(quediase.c_str()));
  tft.setCursor(240, 58);
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.setTextColor(colors[colorI]);
  tft.print(String(data.currentHashRate.c_str()) + " KH/s");
  if (millonario == 1)
  {
    tft.setCursor(240, 72);
    colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
    tft.setTextColor(colors[colorI]);
    tft.print("ERES RICO");
  }
  else
  {
    tft.setCursor(240, 72);
    colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
    tft.setTextColor(colors[colorI]);
    tft.print("NO RICO");
  }
  tft.setCursor(240, 93);
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.setTextColor(colors[colorI]);
  tft.print("V.BLOCKS");
  tft.setCursor(250, 109);
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.setTextColor(colors[colorI]);
  tft.setTextSize(5);
  tft.print(millonario);
  tft.setTextSize(1);
  tft.setCursor(240, 153);
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.setTextColor(colors[colorI]);
  tft.print("T " + TempCPU + "g");
}

/*
 * Función: tDisplay_m8axScreen11
 * ------------------------------------
 *  Esta función actualiza la pantalla con la hora, fecha, el estado de la minería, la temperatura del procesador
 *  y un indicador de riqueza ("ERES RICO" o "NO RICO"), pero con una variación en el manejo de la fecha y el año
 *  en comparación con la función anterior.
 *
 *  Parámetros:
 *    - mElapsed: Tiempo transcurrido desde la última actualización.
 *
 *  Flujo de ejecución:
 *  1. Obtiene los datos actualizados de:
 *      - Minería (`data`): información sobre shares completados, Khashes, hashrate y temperatura.
 *      - Reloj (`dataa`): hora y fecha actual.
 *      - Moneda (`monedilla`).
 *  2. Extrae la hora, minutos y segundos actuales, además de la fecha completa (día, mes, año).
 *  3. Divide el año en dos partes (los primeros dos dígitos y los últimos dos dígitos).
 *  4. Convierte la hora, minutos, segundos, día, mes y año en formato Morse utilizando la función `Amorse()`.
 *  5. Si `segundos % 59 == 0`, reinicia el contador de actualización de pantalla (`actualizarc`).
 *  6. Si `actualizarc` es 0, selecciona aleatoriamente una imagen de fondo (entre 4 opciones).
 *  7. Muestra la imagen seleccionada como fondo.
 *  8. Imprime en la pantalla la hora en formato Morse, junto con los valores binarios de día, mes, año y temperatura.
 *  9. Si el valor de `data.valids` es 1, imprime "ERES RICO", de lo contrario, imprime "NO RICO".
 *  10. Muestra los valores de "V.BLOCKS" (número de bloques de minería) y la temperatura del procesador en formato Morse.
 *  11. El texto se muestra con colores aleatorios seleccionados de un arreglo `colors[]`.
 *  12. Incrementa `actualizarc` y realiza actualizaciones periódicas de la pantalla.
 */

void tDisplay_m8axScreen11(unsigned long mElapsed)
{
  mining_data data = getMiningData(mElapsed);
  clock_data dataa = getClockData(mElapsed);
  monedilla = getCoinData(mElapsed);
  relojete = getClockData(mElapsed);
  mineria = getMiningData(mElapsed);
  unsigned long segundo = timeClient.getSeconds();
  int millonario = atoi(data.valids.c_str());
  int horas = dataa.currentTime.substring(0, 2).toInt();
  int minutos = dataa.currentTime.substring(3, 5).toInt();
  int segundos = segundo % 60;
  incrementCounter();

  if (segundos % 59 == 0)
  {
    actualizarc = 0;
  }

  std::string quediase = obtenerDiaSemana(std::string(dataa.currentDate.c_str()));
  int dia = dataa.currentDate.substring(0, 2).toInt();
  int mes = dataa.currentDate.substring(3, 5).toInt();
  String anio = dataa.currentDate.substring(6, 10);
  int anio2d = anio.substring(0, 2).toInt();  // Extraer los primeros dos dígitos
  int aniol2d = anio.substring(2, 4).toInt(); // Extraer los últimos dos dígitos
  String TempCPU = Amorse(atoi(data.temp.c_str()));
  String hRoman = Amorse(horas);
  String mRoman = Amorse(minutos);
  String sRoman = Amorse(segundos);
  String RDia = Amorse(dia);
  String RMes = Amorse(mes);
  String RAnio = Amorse(anio2d);
  String RAnio2 = Amorse(aniol2d);
  if (actualizarc == 0)
  {
    random_number = 1 + (esp_random() % 4);
  }
  if (random_number == 1)
  {
    background.pushImage(0, 0, ImagenM8AXWidth, ImagenM8AXHeight, ImagenM8AX);
    background.pushSprite(0, 0);
  }
  else if (random_number == 2)
  {
    background.pushImage(0, 0, M8AXRelojLunarWidth, M8AXRelojLunarHeight, M8AXRelojLunar);
    background.pushSprite(0, 0);
  }
  else if (random_number == 3)
  {
    background.pushImage(0, 0, M8AXQuote1Width, M8AXQuote1Height, M8AXQuote1);
    background.pushSprite(0, 0);
  }
  else if (random_number == 4)
  {
    background.pushImage(0, 0, M8AXQuote2Width, M8AXQuote2Height, M8AXQuote2);
    background.pushSprite(0, 0);
  }
  actualizarc++;
  Serial.printf("M8AX - >>> Completados %s Share(s), %s Khashes, Prom. Hashrate %s KH/s %s°\n",
                data.completedShares.c_str(), data.totalKHashes.c_str(), data.currentHashRate.c_str(), data.temp.c_str());
  tft.setTextSize(3);
  tft.setCursor(3, 0);
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.setTextColor(colors[colorI]);
  tft.print(hRoman);
  tft.setCursor(3, 75);
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.setTextColor(colors[colorI]);
  tft.print(mRoman);
  tft.setCursor(3, 150);
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.setTextColor(colors[colorI]);
  tft.print(sRoman);
  tft.setTextSize(1);
  tft.setCursor(240, 2);
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.setTextColor(colors[colorI]);
  tft.print("d " + RDia);
  tft.setCursor(240, 16);
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.setTextColor(colors[colorI]);
  tft.print("m " + RMes);
  tft.setCursor(240, 30);
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.setTextColor(colors[colorI]);
  tft.print("a " + RAnio);
  tft.setCursor(252, 38);
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.setTextColor(colors[colorI]);
  tft.print(RAnio2);
  tft.setCursor(240, 52);
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.setTextColor(colors[colorI]);
  tft.print(String(quediase.c_str()));
  tft.setCursor(240, 66);
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.setTextColor(colors[colorI]);
  tft.print(String(data.currentHashRate.c_str()) + " KH/s");
  if (millonario == 1)
  {
    tft.setCursor(240, 80);
    colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
    tft.setTextColor(colors[colorI]);
    tft.print("ERES RICO");
  }
  else
  {
    tft.setCursor(240, 80);
    colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
    tft.setTextColor(colors[colorI]);
    tft.print("NO RICO");
  }
  tft.setCursor(240, 95);
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.setTextColor(colors[colorI]);
  tft.print("V.BLOCKS");
  tft.setCursor(250, 113);
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.setTextColor(colors[colorI]);
  tft.setTextSize(5);
  tft.print(millonario);
  tft.setTextSize(1);
  tft.setCursor(225, 155);
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.setTextColor(colors[colorI]);
  tft.print("T " + TempCPU + "g");
}

/*
 * Función: tDisplay_m8axScreen12
 * ------------------------------------
 *  Esta función actualiza la pantalla con información sobre la minería, un mensaje personalizado y gráficos aleatorios,
 *  además de cambiar el fondo entre varias opciones. También se genera un patrón aleatorio de píxeles en la pantalla.
 *
 *  Parámetros:
 *    - mElapsed: Tiempo transcurrido desde la última actualización.
 *
 *  Flujo de ejecución:
 *  1. Obtiene los datos actualizados de:
 *      - Minería (`data`): información sobre shares completados, Khashes, hashrate y temperatura.
 *      - Moneda (`monedilla`).
 *      - Reloj (`relojete`).
 *  2. Incrementa el contador de actualizaciones (`incrementCounter()`).
 *  3. Imprime en el monitor serial información sobre el progreso de la minería.
 *  4. Selecciona aleatoriamente una imagen de fondo (entre 4 opciones) usando un número aleatorio generado por `esp_random()`.
 *  5. Si el número es par, se establece un valor aleatorio para `limite` entre 0 y 15000, de lo contrario, entre 0 y 5000.
 *  6. Muestra el hashrate actual en la pantalla con un tamaño de texto grande, utilizando un color aleatorio.
 *  7. Muestra un mensaje en el centro de la pantalla ("... GRACIAS ...").
 *  8. Muestra una URL debajo del mensaje principal (https://youtube.com/m8ax).
 *  9. Dibuja un patrón de píxeles aleatorios en la pantalla. Cada píxel tiene un color aleatorio de un conjunto `colors[]`,
 *      y las posiciones se generan aleatoriamente dentro de los límites de la pantalla (ancho 0 a 340, alto 0 a 170).
 *  10. Reinicia varias variables (`actualizarcalen`, `actualizarc`, `actual`, `actuanot`, `correccion`) para preparar la pantalla
 *      para la siguiente actualización.
 */

void tDisplay_m8axScreen12(unsigned long mElapsed)
{
  mining_data data = getMiningData(mElapsed);
  clock_data dataa = getClockData(mElapsed);
  monedilla = getCoinData(mElapsed);
  relojete = getClockData(mElapsed);
  mineria = getMiningData(mElapsed);
  incrementCounter();
  Serial.printf("M8AX - >>> Completados %s Share(s), %s Khashes, Prom. Hashrate %s KH/s %s°\n",
                data.completedShares.c_str(), data.totalKHashes.c_str(), data.currentHashRate.c_str(), data.temp.c_str());
  int random_number = 1 + (esp_random() % 4);
  switch (random_number)
  {
  case 1:
    background.pushImage(0, 0, ImagenM8AXWidth, ImagenM8AXHeight, ImagenM8AX);
    background.pushSprite(0, 0);
    break;
  case 2:
    background.pushImage(0, 0, M8AXRelojLunarWidth, M8AXRelojLunarHeight, M8AXRelojLunar);
    background.pushSprite(0, 0);
    break;
  case 3:
    background.pushImage(0, 0, M8AXQuote1Width, M8AXQuote1Height, M8AXQuote1);
    background.pushSprite(0, 0);
    break;
  case 4:
    background.pushImage(0, 0, M8AXQuote2Width, M8AXQuote2Height, M8AXQuote2);
    background.pushSprite(0, 0);
    break;
  default:
    // En caso de que el número esté fuera de rango, puedes poner una imagen predeterminada o hacer algo más.
    break;
  }
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  if (colorI % 2 == 0)
  {
    limite = esp_random() % 15000; // Rango: 0 a 54400
  }
  else
  {
    limite = esp_random() % 5000; // Rango: 0 a 54400
  }
  tft.setCursor(30, 60);
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.setTextColor(colors[colorI]);
  tft.setTextSize(7);
  tft.print(String(data.currentHashRate.c_str()));
  tft.setCursor(25, 15);
  tft.setTextSize(3);
  tft.print("... GRACIAS ...");
  tft.setCursor(16, 129);
  tft.setTextSize(2);
  tft.print("https://youtube.com/m8ax");
  tft.setCursor(16, 149);
  tft.print("https://paypal.me/m8ax/2");
  for (int i = 0; i < limite; i++)
  {
    colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
    int ancho = esp_random() % 341; // Rango: 0 a 340
    int alto = esp_random() % 171;  // Rango: 0 a 170
    tft.drawPixel(ancho, alto, colors[colorI]);
  }
  tft.setTextSize(1);
  actualizarcalen = 0;
  actualizarc = 0;
  actual = 0;
  actuanot = 0;
  correccion = 0;
}

/*
 * Función: tDisplay_m8axScreen13
 * -----------------------------------
 *  Esta función es responsable de actualizar la pantalla con información sobre la minería,
 *  el precio de las criptomonedas y un mensaje personalizado. Además, selecciona aleatoriamente una imagen de fondo y
 *  muestra una lista de criptomonedas con sus precios actualizados.
 *
 *  Parámetros:
 *    - mElapsed: Tiempo transcurrido desde la última actualización.
 *
 *  Flujo de ejecución:
 *  1. Obtiene los datos actualizados de:
 *      - Minería (`data`): Información sobre shares completados, Khashes, hashrate y temperatura.
 *      - Moneda (`monedilla`).
 *      - Reloj (`relojete`).
 *  2. Realiza una comprobación para saber si es el momento de actualizar la pantalla basándose en el minuto y segundo actuales.
 *  3. Si es el momento de actualizar, selecciona aleatoriamente una de las 4 imágenes posibles para usar como fondo de pantalla.
 *  4. Imprime en el monitor serial información sobre el progreso de la minería.
 *  5. Para cada una de las 20 criptomonedas (a través del índice `iii`), se obtiene el precio y se muestra en la pantalla:
 *      - Se distribuyen las criptomonedas en 4 columnas en función del índice `iii`.
 *      - Si el precio de la criptomoneda es válido, se muestra el nombre y el precio en dólares.
 *      - Si el precio no es válido (error al obtenerlo), se muestra un mensaje de error para esa criptomoneda.
 *  6. Si la actualización de la pantalla se realiza, se reinician varias variables relacionadas con el ciclo de actualización.
 *  7. Se dibuja una línea en la parte superior de la pantalla para separar la información.
 */

void tDisplay_m8axScreen13(unsigned long mElapsed)
{
  mining_data data = getMiningData(mElapsed);
  clock_data dataa = getClockData(mElapsed);
  monedilla = getCoinData(mElapsed);
  relojete = getClockData(mElapsed);
  mineria = getMiningData(mElapsed);
  int x, y;
  incrementCounter();
  unsigned long segundo = timeClient.getSeconds();
  int horas = dataa.currentTime.substring(0, 2).toInt();
  int minutos = dataa.currentTime.substring(3, 5).toInt();
  int segundos = segundo % 60;
  abortar = 0;
  if (minutos % 2 == 0 && segundos <= 1)
  {
    actualizarc = 0;
  }
  if (actualizarc == 0)
  {
    random_number = 1 + (esp_random() % 4);
    if (random_number == 1)
    {
      background.pushImage(0, 0, ImagenM8AXWidth, ImagenM8AXHeight, ImagenM8AX);
      background.pushSprite(0, 0);
    }
    else if (random_number == 2)
    {
      background.pushImage(0, 0, M8AXRelojLunarWidth, M8AXRelojLunarHeight, M8AXRelojLunar);
      background.pushSprite(0, 0);
    }
    else if (random_number == 3)
    {
      background.pushImage(0, 0, M8AXQuote1Width, M8AXQuote1Height, M8AXQuote1);
      background.pushSprite(0, 0);
    }
    else if (random_number == 4)
    {
      background.pushImage(0, 0, M8AXQuote2Width, M8AXQuote2Height, M8AXQuote2);
      background.pushSprite(0, 0);
    }
  }
  Serial.printf("M8AX - >>> Completados %s Share(s), %s Khashes, Prom. Hashrate %s KH/s %s°\n",
                data.completedShares.c_str(), data.totalKHashes.c_str(), data.currentHashRate.c_str(), data.temp.c_str());
  if (actualizarc == 0)
  {
    // Mensaje de espera mientras se piden los precios
    tft.fillRoundRect(40, 40, 240, 90, 10, TFT_BLACK);
    tft.drawRoundRect(40, 40, 240, 90, 10, TFT_WHITE);
    tft.setTextSize(3);
    tft.setTextColor(TFT_YELLOW);
    tft.setCursor((tft.width() - tft.textWidth("POR FAVOR")) / 2, 53);
    tft.print("POR FAVOR");
    tft.setCursor((tft.width() - tft.textWidth("ESPERA...")) / 2, 93);
    tft.print("ESPERA...");
    tft.setTextSize(1);
    float precios[NUM_CRIPTOS];
    for (int iii = 0; iii < NUM_CRIPTOS; iii++)
      precios[iii] = -2; // -2 = todavía sin pedir
    if (abortar == 0)
      obtenerPreciosTodos(precios);
    // Quitar el mensaje: repone el fondo
    background.pushSprite(0, 0);
    int segundosAhora = timeClient.getSeconds() % 60;
    String Textito = "M8AX - CryptoChrono 2Min - " + String(horas) + ":" + String(minutos) + ":" + String(segundosAhora) + " - " + data.currentHashRate + " KH/s";
    colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
    tft.setTextColor(colors[colorI]);
    tft.setTextSize(1);
    tft.drawLine(0, 13, 320, 13, colors[colorI]);
    tft.setCursor(16, 2);
    tft.print(Textito);
    Textito = "";
    Textito.reserve(0);
    for (int iii = 0; iii < NUM_CRIPTOS; iii++)
    {
      if (precios[iii] == -2)
        continue;
      int col = iii / 5;
      x = 5 + col * (tft.width() / 4) + col * 5;
      y = 20 + (iii % 5) * 30;
      colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
      tft.setTextColor(colors[colorI]);
      tft.setCursor(x, y);
      tft.print(criptomonedas[iii].nombre);
      tft.setCursor(x, y + 15);
      if (precios[iii] != -1)
      {
        tft.print("$");
        tft.print(precios[iii]);
      }
      else
      {
        tft.print("$-ERROR-$");
        Serial.print("M8AX - Error Al Obtener El Precio De ");
        Serial.println(criptomonedas[iii].nombre);
      }
    }
    actualizarc++;
    actualizarcalen = 0;
    actual = 0;
    actuanot = 0;
    correccion = 0;
  }
}

/*
 * Función: tDisplay_m8axScreen14
 * ------------------------------
 *  Esta función se encarga de actualizar la pantalla con información sobre el estado de la minería,
 *  noticias aleatorias y un mensaje sobre el "estado de riqueza" basado en la cantidad de shares validados.
 *  La información se presenta de una manera gráfica con un fondo y se actualiza en intervalos de tiempo específicos.
 *
 *  Parámetros:
 *    - mElapsed: Tiempo transcurrido desde la última actualización.
 *
 *  Flujo de ejecución:
 *  1. Obtiene los datos actualizados sobre:
 *      - Minería (`data`): Información sobre shares completados, Khashes, hashrate y temperatura.
 *      - Reloj (`relojete`).
 *  2. Si es el momento adecuado (basado en el valor de `actuanot` y `correccion`), selecciona aleatoriamente uno de los 4 fondos disponibles:
 *      - `ImagenM8AX`, `M8AXRelojLunar`, `M8AXQuote1`, o `M8AXQuote2`.
 *  3. Obtiene las noticias a través de la función `obtenerNoticias()`.
 *  4. Actualiza la pantalla con el texto "M8AX-Noticias Num-" seguido del número de noticias actuales (`numnotis`).
 *  5. Dibuja una línea horizontal en la parte superior de la pantalla.
 *  6. Imprime información adicional sobre el tiempo actual, la fecha, el hashrate y la temperatura en la pantalla.
 *      - Si el número de shares válidos (`data.valids`) es igual a 0, se imprime "NO RICO".
 *      - Si el número de shares válidos es mayor a 0, se imprime "SI RICO".
 *  7. Incrementa el contador `actuanot` y reinicia las variables relacionadas con el ciclo de actualización (`actualizarcalen`, `actualizarc`, `actual`).
 */

void tDisplay_m8axScreen14(unsigned long mElapsed)
{
  mining_data data = getMiningData(mElapsed);
  clock_data dataa = getClockData(mElapsed);
  monedilla = getCoinData(mElapsed);
  relojete = getClockData(mElapsed);
  mineria = getMiningData(mElapsed);
  incrementCounter();
  Serial.printf("M8AX - >>> Completados %s Share(s), %s Khashes, Prom. Hashrate %s KH/s %s°\n",
                data.completedShares.c_str(), data.totalKHashes.c_str(), data.currentHashRate.c_str(), data.temp.c_str());
  if (actuanot % 59 == 0 || correccion == 0)
  {
    numnotis++;
    int random_number = 1 + (esp_random() % 4);

    if (random_number == 1)
    {
      background.pushImage(0, 0, ImagenM8AXWidth, ImagenM8AXHeight, ImagenM8AX);
      background.pushSprite(0, 0);
    }
    else if (random_number == 2)
    {
      background.pushImage(0, 0, M8AXRelojLunarWidth, M8AXRelojLunarHeight, M8AXRelojLunar);
      background.pushSprite(0, 0);
    }
    else if (random_number == 3)
    {
      background.pushImage(0, 0, M8AXQuote1Width, M8AXQuote1Height, M8AXQuote1);
      background.pushSprite(0, 0);
    }
    else if (random_number == 4)
    {
      background.pushImage(0, 0, M8AXQuote2Width, M8AXQuote2Height, M8AXQuote2);
      background.pushSprite(0, 0);
    }

    obtenerNoticias();
    tft.setTextColor(TFT_WHITE);
    tft.setCursor(112, 160);
    tft.setTextSize(1);
    tft.print("M8AX-Noticias Num-" + String(numnotis));
    tft.setTextColor(TFT_WHITE);
    tft.drawLine(0, 20, 320, 20, colors[colorI]); // Dibujar línea de (0, y) a (320, y)
    tft.setCursor(1, 6);
    tft.setTextSize(1);
    correccion = 1;
    int millonario = atoi(data.valids.c_str());
    if (millonario == 0)
    {
      tft.print(data.currentTime + " - " + dataa.currentDate + " - " + data.currentHashRate + " KH/s - " + data.temp + " Grados." + " NO RICO");
    }
    else
    {
      tft.print(data.currentTime + " - " + dataa.currentDate + " - " + data.currentHashRate + " KH/s - " + data.temp + " Grados." + " SI RICO");
    }
  }
  actuanot++;
  actualizarcalen = 0;
  actualizarc = 0;
  actual = 0;
}

/*
 * Función: tDisplay_m8axScreen15
 * ------------------------------
 *  Esta función es responsable de actualizar la pantalla con información sobre el estado de la minería,
 *  incluyendo el hash rate, temperaturas, y datos adicionales como la factorización de un número aleatorio.
 *  También visualiza barras de progreso que representan los segundos, horas y minutos actuales.
 *
 *  Parámetros:
 *    - mElapsed: Tiempo transcurrido desde la última actualización.
 *
 *  Flujo de ejecución:
 *  1. Obtiene los datos actualizados sobre:
 *      - Minería (`data`): Información sobre shares completados, Khashes, hashrate y temperatura.
 *      - Reloj (`dataa`): Información sobre la hora y fecha.
 *  2. Calcula el tiempo actual (segundos, minutos, horas) y actualiza las barras de progreso visualizando los valores en tiempo real.
 *  3. Si `actualizarcalen` es 0, establece una imagen de fondo inicial en la pantalla y la actualiza.
 *  4. Cada 30 segundos (segundo % 30 == 0), selecciona aleatoriamente una de las 4 imágenes de fondo y la muestra en la pantalla.
 *  5. Convierte el valor de `currentHashRate` a un número flotante y lo usa para dibujar un gráfico analógico del hashrate.
 *  6. Muestra un número aleatorio generado (`rndnumero2`) y su factorización.
 *  7. Muestra el estado de la minería, especificando si está "SI RICO" o "NO RICO" basado en el número de shares válidos.
 *  8. Muestra datos adicionales como:
 *      - Hashes por segundo.
 *      - Operaciones por segundo.
 *      - TFLOPS, GOPS, MOPS, TOPS.
 *  9. Muestra la temperatura de la minería en la pantalla.
 * 10. Dibuja tres barras de progreso:
 *      - Una para los segundos (basada en `segundos`).
 *      - Otra para las horas (basada en `horas`).
 *      - Otra para los minutos (basada en `minutos`).
 *  11. Resetea varios contadores (`actualizarcalen`, `actualizarc`, `actuanot`) al final de la función.
 */

void tDisplay_m8axScreen15(unsigned long mElapsed)
{
  mining_data data = getMiningData(mElapsed);
  clock_data dataa = getClockData(mElapsed);
  monedilla = getCoinData(mElapsed);
  relojete = getClockData(mElapsed);
  mineria = getMiningData(mElapsed);
  unsigned long segundo = timeClient.getSeconds();
  int segundos = segundo % 60;
  int horas = dataa.currentTime.substring(0, 2).toInt();
  int minutos = dataa.currentTime.substring(3, 5).toInt();
  incrementCounter();
  if (actualizarcalen == 0)
  {
    background.pushImage(0, 0, ImagenM8AXWidth, ImagenM8AXHeight, ImagenM8AX);
    background.pushSprite(0, 0);
    actualizarcalen++;
  }
  background.pushSprite(0, 0);
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  Serial.printf("M8AX - >>> Completados %s Share(s), %s Khashes, Prom. Hashrate %s KH/s %s°\n",
                data.completedShares.c_str(), data.totalKHashes.c_str(), data.currentHashRate.c_str(), data.temp.c_str());
  if (segundo % 30 == 0)
  {
    int random_number = 1 + (esp_random() % 4);
    if (random_number == 1)
    {
      background.pushImage(0, 0, ImagenM8AXWidth, ImagenM8AXHeight, ImagenM8AX);
      background.pushSprite(0, 0);
    }
    else if (random_number == 2)
    {
      background.pushImage(0, 0, M8AXRelojLunarWidth, M8AXRelojLunarHeight, M8AXRelojLunar);
      background.pushSprite(0, 0);
    }
    else if (random_number == 3)
    {
      background.pushImage(0, 0, M8AXQuote1Width, M8AXQuote1Height, M8AXQuote1);
      background.pushSprite(0, 0);
    }
    else if (random_number == 4)
    {
      background.pushImage(0, 0, M8AXQuote2Width, M8AXQuote2Height, M8AXQuote2);
      background.pushSprite(0, 0);
    }
  }
  const char *hashRateStr = dataa.currentHashRate.c_str(); // Obtener la cadena
  float hashRa = atof(hashRateStr);                        // Convertirla a flotante (float)
  dibujaAnalogKH(hashRa);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(1);
  rndnumero2 = esp_random();
  const char *factorizacion = factorize(rndnumero2);
  tft.setCursor(4, 154);
  tft.print(String(rndnumero2) + " - " + (factorizacion));
  Serial.println("M8AX - Factorizando Número - " + String(rndnumero2) + " - " + (factorizacion));
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.setCursor(4, 144);
  tft.setTextColor(colors[colorI]);
  tft.print("Factorizacion Nerd");
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(2, 6);
  tft.setTextSize(1);
  int millonario = atoi(data.valids.c_str());
  if (millonario == 0)
  {
    tft.print(data.currentTime + " - " + dataa.currentDate + " - " + data.currentHashRate + " KH/s - " + data.temp + " Grados." + " NO RICO");
  }
  else
  {
    tft.print(data.currentTime + " - " + dataa.currentDate + " - " + data.currentHashRate + " KH/s - " + data.temp + " Grados." + " SI RICO");
  }
  tft.setCursor(4, 23);
  tft.setTextSize(1);
  tft.setTextColor(colors[colorI]);
  tft.print("... DATOS NERD I ...");
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(4, 40);
  tft.setTextColor(colors[colorI]);
  tft.print("Hashes/Seg");
  tft.setTextColor(TFT_WHITE);
  float hasheseg = hashRa * 1000;
  tft.setCursor(4, 50);
  tft.print((uint32_t)hasheseg);
  tft.setCursor(4, 65);
  tft.setTextColor(colors[colorI]);
  tft.print("+-/Ops_Simp/Seg");
  tft.setTextColor(TFT_WHITE);
  float opsseg = (hashRa * 1000) * 2560;
  tft.setCursor(4, 75);
  tft.print((uint32_t)opsseg);
  tft.setCursor(4, 90);
  tft.setTextColor(colors[colorI]);
  tft.print("TFLOPS");
  tft.setTextColor(TFT_WHITE);
  float tflops = ((hashRa * 1000) * 2560) / 1000000000000;
  tft.setCursor(4, 100);
  char buffer[20];               // Buffer para almacenar el número formateado
  dtostrf(tflops, 8, 6, buffer); // (valor, ancho mínimo, decimales, buffer)
  tft.print(buffer);
  tft.setCursor(4, 115);
  tft.setTextColor(colors[colorI]);
  tft.print("GOPS");
  tft.setTextColor(TFT_WHITE);
  float gops = ((hashRa * 1000) * 2560) / 1000000;
  tft.setCursor(4, 125);
  tft.print(gops);
  tft.setTextColor(colors[colorI]);
  tft.setCursor(65, 115);
  tft.print("MH/s");
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(65, 125);
  float mhss = (hashRa / 1000);
  tft.print(mhss);
  tft.setCursor(193, 23);
  tft.setTextColor(colors[colorI]);
  tft.print("... DATOS NERD II ...");
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(230, 40);
  tft.setTextColor(colors[colorI]);
  tft.print("MOPS");
  tft.setTextColor(TFT_WHITE);
  float Mops = ((hashRa * 1000) * 2560) / 1000000;
  tft.setCursor(230, 50);
  char bufferrr[20];             // Buffer para almacenar el número formateado
  dtostrf(Mops, 8, 6, bufferrr); // (valor, ancho mínimo, decimales, buffer)
  tft.print(bufferrr);
  tft.setCursor(230, 65);
  tft.setTextColor(colors[colorI]);
  tft.print("TOPS");
  tft.setTextColor(TFT_WHITE);
  float Tops = ((hashRa * 1000) * 2560) / 1000000000;
  tft.setCursor(230, 75);
  char bufferr[20];
  dtostrf(Tops, 8, 6, bufferr);
  tft.print(bufferr);
  tft.drawLine(230, 90, 310, 90, TFT_WHITE);
  tft.setCursor(236, 95);
  tft.setTextColor(colors[colorI]);
  tft.print("TEMPERATURA");
  tft.setTextSize(3);
  tft.setCursor(246, 107);
  tft.setTextColor(TFT_WHITE);
  tft.print(data.temp);
  tft.setTextSize(1);
  tft.setCursor(283, 103);
  tft.print("o");
  tft.setCursor(208, 138);
  tft.print("HMS");
  std::string quediase = obtenerDiaSemana(std::string(dataa.currentDate.c_str()));
  tft.setCursor(153, 68);
  tft.setTextColor(colors[colorI]);
  tft.print(quediase.c_str());
  tft.setTextColor(TFT_WHITE);
  int X_INICIO = 230;                                      // Columna fija
  int X_FINAL = 310;                                       // Base de la barra
  int Y_POS = 147;                                         // Punto más alto
  int longitud_total = X_FINAL - X_INICIO;                 // Longitud total de la barra (80 píxeles)
  int longitud_pintada = (longitud_total * segundos) / 59; // Porcentaje de la barra según los segundos
  tft.fillRect(X_INICIO, Y_POS - 1, longitud_total + 1, 3, TFT_BLACK);
  tft.drawLine(X_INICIO, Y_POS, X_INICIO + longitud_pintada, Y_POS, TFT_WHITE);
  Y_POS = 135;
  longitud_pintada = (longitud_total * horas) / 23; // Porcentaje de la barra según las horas
  tft.fillRect(X_INICIO, Y_POS - 1, longitud_total + 1, 3, TFT_BLACK);
  tft.drawLine(X_INICIO, Y_POS, X_INICIO + longitud_pintada, Y_POS, TFT_WHITE);
  Y_POS = 141;
  longitud_pintada = (longitud_total * minutos) / 59; // Porcentaje de la barra según los minutos
  tft.fillRect(X_INICIO, Y_POS - 1, longitud_total + 1, 3, TFT_BLACK);
  tft.drawLine(X_INICIO, Y_POS, X_INICIO + longitud_pintada, Y_POS, TFT_WHITE);
  actualizarcalen = 0;
  actualizarc = 0;
  actuanot = 0;
}

// Función tDisplay_m8axScreen16: Actualiza la pantalla con datos de minería, hora mundial, y datos de configuración del sistema.
// Utiliza imágenes de fondo que cambian aleatoriamente cada 30 segundos y muestra la hora de 20 ciudades en varias columnas.
// Además, incluye la visualización de los datos de minería, temperatura y estado actual.

void tDisplay_m8axScreen16(unsigned long mElapsed)
{
  mining_data data = getMiningData(mElapsed);
  clock_data dataa = getClockData(mElapsed);
  monedilla = getCoinData(mElapsed);
  relojete = getClockData(mElapsed);
  mineria = getMiningData(mElapsed);
  unsigned long segundo = timeClient.getSeconds();
  int segundos = segundo % 60;
  int horita = dataa.currentTime.substring(0, 2).toInt();
  int minutitos = dataa.currentTime.substring(3, 5).toInt();
  int currentDay = dataa.currentDate.substring(0, 2).toInt();
  int currentMonth = dataa.currentDate.substring(3, 5).toInt();
  incrementCounter();
  if (actualizarcalen == 0)
  {
    background.pushImage(0, 0, ImagenM8AXWidth, ImagenM8AXHeight, ImagenM8AX);
    background.pushSprite(0, 0);
    actualizarcalen++;
  }
  background.pushSprite(0, 0);
  Serial.printf("M8AX - >>> Completados %s Share(s), %s Khashes, Prom. Hashrate %s KH/s %s°\n",
                data.completedShares.c_str(), data.totalKHashes.c_str(), data.currentHashRate.c_str(), data.temp.c_str());
  if (segundo % 30 == 0)
  {
    int random_number = 1 + (esp_random() % 4);
    if (random_number == 1)
    {
      background.pushImage(0, 0, ImagenM8AXWidth, ImagenM8AXHeight, ImagenM8AX);
      background.pushSprite(0, 0);
    }
    else if (random_number == 2)
    {
      background.pushImage(0, 0, M8AXRelojLunarWidth, M8AXRelojLunarHeight, M8AXRelojLunar);
      background.pushSprite(0, 0);
    }
    else if (random_number == 3)
    {
      background.pushImage(0, 0, M8AXQuote1Width, M8AXQuote1Height, M8AXQuote1);
      background.pushSprite(0, 0);
    }
    else if (random_number == 4)
    {
      background.pushImage(0, 0, M8AXQuote2Width, M8AXQuote2Height, M8AXQuote2);
      background.pushSprite(0, 0);
    }
  }
  int y = 30; // Coordenada Y inicial
  int horitaUTC = horita - (EsHVerano(currentMonth, currentDay) ? 2 : 1);
  for (int i = 0; i < 20; i++)
  {
    // Calcula la hora local para cada ciudad, ajustando con la zona horaria de cada ciudad
    int ciudadHora = horitaUTC + zonasHorarias[i]; // Ajusta la hora sumando la zona horaria de cada ciudad

    // Ajusta la hora si excede 24 horas o es menor a 0
    if (ciudadHora < 0)
    {
      ciudadHora += 24;
    }
    if (ciudadHora >= 24)
    {
      ciudadHora -= 24;
    }
    // Formatea la hora (sin segundos)
    char timeStr[9];
    sprintf(timeStr, "%02d:%02d:%02d", ciudadHora, minutitos, segundos);

    // Calcula la posición X en base a la columna (4 columnas)
    int x = (i % 4) * 80; // Las ciudades se distribuyen en 4 columnas

    // Muestra la ciudad y la hora
    colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
    tft.drawLine(0, 20, 320, 20, colors[colorI]); // Dibujar línea de (0, y) a (320, y)
    tft.setTextColor(colors[colorI]);
    tft.setTextSize(1);
    tft.setCursor(x + 5, y);
    tft.print(ciudades[i]);
    tft.setCursor(x + 5, y + 10); // Posición de la hora
    tft.print(timeStr);
    tft.setTextColor(TFT_WHITE);
    tft.setCursor(1, 6);
    tft.setTextSize(1);
    int millonario = atoi(data.valids.c_str());
    if (millonario == 0)
    {
      tft.print(data.currentTime + " - " + dataa.currentDate + " - " + data.currentHashRate + " KH/s - " + data.temp + " Grados." + " NO RICO");
    }
    else
    {
      tft.print(data.currentTime + " - " + dataa.currentDate + " - " + data.currentHashRate + " KH/s - " + data.temp + " Grados." + " SI RICO");
    }

    // Cambia la posición Y para la siguiente fila (5 filas)
    if ((i + 1) % 4 == 0)
    {          // Se cambia después de cada 4 columnas
      y += 30; // Aumenta la posición Y después de cada 4 ciudades
    }
  }
  actualizarc = 0;
  actuanot = 0;
}

// Función tDisplay_m8axScreen17: Actualiza la pantalla con información de minería, dirección BTC y muestra un código QR.
// La pantalla muestra información de minería, una dirección de Bitcoin, y un QR asociado a esa dirección.
// Además, cada 30 segundos cambia la imagen de fondo de manera aleatoria, y se muestran letras relacionadas con Bitcoin en diferentes posiciones de la pantalla.

void tDisplay_m8axScreen17(unsigned long mElapsed)
{
  mining_data data = getMiningData(mElapsed);
  monedilla = getCoinData(mElapsed);
  relojete = getClockData(mElapsed);
  mineria = getMiningData(mElapsed);
  String btcm8 = "bitcoin:bc1qljq00pm2plq2l9jxzdzt0xc8t79j9wcmu7r8em";
  unsigned long segundo = timeClient.getSeconds();
  incrementCounter();
  if (actualizarcalen == 0)
  {
    background.pushImage(0, 0, ImagenM8AXWidth, ImagenM8AXHeight, ImagenM8AX);
    background.pushSprite(0, 0);
    actualizarcalen++;
  }
  background.pushSprite(0, 0);
  Serial.printf("M8AX - >>> Completados %s Share(s), %s Khashes, Prom. Hashrate %s KH/s %s°\n",
                data.completedShares.c_str(), data.totalKHashes.c_str(), data.currentHashRate.c_str(), data.temp.c_str());
  if (segundo % 30 == 0)
  {
    int random_number = 1 + (esp_random() % 4);
    if (random_number == 1)
    {
      background.pushImage(0, 0, ImagenM8AXWidth, ImagenM8AXHeight, ImagenM8AX);
      background.pushSprite(0, 0);
    }
    else if (random_number == 2)
    {
      background.pushImage(0, 0, M8AXRelojLunarWidth, M8AXRelojLunarHeight, M8AXRelojLunar);
      background.pushSprite(0, 0);
    }
    else if (random_number == 3)
    {
      background.pushImage(0, 0, M8AXQuote1Width, M8AXQuote1Height, M8AXQuote1);
      background.pushSprite(0, 0);
    }
    else if (random_number == 4)
    {
      background.pushImage(0, 0, M8AXQuote2Width, M8AXQuote2Height, M8AXQuote2);
      background.pushSprite(0, 0);
    }
  }
  actualizarc = 0;
  actual = 0;
  actuanot = 0;
  correccion = 0;
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.setTextColor(colors[colorI]);
  tft.setTextSize(3);
  tft.setCursor(20, 1);
  tft.print("D");
  tft.setCursor(20, 30);
  tft.print("O");
  tft.setCursor(20, 60);
  tft.print("N");
  tft.setCursor(20, 90);
  tft.print("A");
  tft.setCursor(20, 120);
  tft.print("M");
  tft.setCursor(20, 149);
  tft.print("E");
  tft.setCursor(50, 1);
  tft.print("B");
  tft.setCursor(50, 26);
  tft.print("I");
  tft.setCursor(50, 51);
  tft.print("T");
  tft.setCursor(50, 76);
  tft.print("C");
  tft.setCursor(50, 101);
  tft.print("O");
  tft.setCursor(50, 126);
  tft.print("I");
  tft.setCursor(50, 151);
  tft.print("N");
  tft.setCursor(242, 20);
  tft.print("S");
  tft.setCursor(242, 55);
  tft.print("I");
  tft.setCursor(242, 106);
  tft.print("T");
  tft.setCursor(242, 131);
  tft.print("E");
  tft.setCursor(272, 1);
  tft.print("M");
  tft.setCursor(272, 26);
  tft.print("O");
  tft.setCursor(272, 51);
  tft.print("L");
  tft.setCursor(272, 76);
  tft.print("A");
  tft.setCursor(272, 101);
  tft.print(" ");
  tft.setCursor(272, 126);
  tft.print("M");
  tft.setCursor(272, 151);
  tft.print("I");
  tft.setCursor(300, 1);
  tft.print("T");
  tft.setCursor(300, 26);
  tft.print("R");
  tft.setCursor(300, 51);
  tft.print("A");
  tft.setCursor(300, 76);
  tft.print("B");
  tft.setCursor(300, 101);
  tft.print("A");
  tft.setCursor(300, 126);
  tft.print("J");
  tft.setCursor(300, 151);
  tft.print("O");
  tft.setCursor(128, 162);
  tft.setTextSize(1);
  tft.print(data.currentHashRate + " KH/s");
  uint16_t colorss[] = {TFT_WHITE, TFT_YELLOW, TFT_CYAN, TFT_GREENYELLOW}; // Array de colores
  int colorrr = esp_random() % 4;
  dibujaQR(btcm8, (320 - 150) / 2, (170 - 150) / 2, 150, colorss[colorrr]); // Dibuja el QR centrado en la pantalla 320x170
}

// tDisplay_m8axScreen18: Esta función se encarga de actualizar la pantalla TFT con diversos datos relacionados con el proceso de minería y el reloj.
// La pantalla muestra estadísticas de minería, un reloj actualizado, números aleatorios, y un "objetivo" para los números generados,
// así como varios elementos gráficos (como líneas, imágenes, y códigos QR). Además, se calcula un valor relacionado con la minería y el tiempo,
// y se actualizan los valores visualizados en la pantalla dependiendo del estado actual de la minería.

// Parámetros:
// - mElapsed: Un valor de tiempo transcurrido utilizado para obtener datos de minería y del reloj.

// Procedimiento:
// 1. Obtiene los datos de minería, reloj y moneda mediante las funciones `getMiningData`, `getClockData` y `getCoinData`.
// 2. Extrae la hora, minuto y segundos actuales, además de generar números aleatorios y un destino para el "juego de números".
// 3. Si el segundo actual es divisible entre 30, se elige aleatoriamente una imagen para mostrar en la pantalla (por ejemplo, una imagen de fondo o una cita).
// 4. Actualiza la pantalla con texto informativo: estadísticas de minería (como shares completados, tasa de hash y temperatura),
//    y muestra los números generados junto con el objetivo a alcanzar en el "juego de números".
// 5. Se dibujan líneas en la pantalla para separar las secciones de información.
// 6. Calcula las operaciones para el "juego de números" y muestra el resultado en la pantalla.
// 7. Dibuja códigos QR con la hora, minuto y segundo actuales, utilizando colores aleatorios.
// 8. Muestra un reloj visualizado en la parte superior con los valores de hora, minuto y segundo.
// 9. Finalmente, imprime en el monitor serial información relevante sobre los números generados y el objetivo.

void tDisplay_m8axScreen18(unsigned long mElapsed)
{
  mining_data data = getMiningData(mElapsed);
  clock_data dataa = getClockData(mElapsed);
  monedilla = getCoinData(mElapsed);
  relojete = getClockData(mElapsed);
  mineria = getMiningData(mElapsed);
  int horita = dataa.currentTime.substring(0, 2).toInt();
  int minutitos = dataa.currentTime.substring(3, 5).toInt();
  unsigned long segundo = timeClient.getSeconds();
  int segundos = segundo % 60;
  incrementCounter();
  if (actualizarcalen == 0)
  {
    background.pushImage(0, 0, ImagenM8AXWidth, ImagenM8AXHeight, ImagenM8AX);
    background.pushSprite(0, 0);
    actualizarcalen++;
  }
  background.pushSprite(0, 0);
  Serial.printf("\nM8AX - >>> Completados %s Share(s), %s Khashes, Prom. Hashrate %s KH/s %s°\n",
                data.completedShares.c_str(), data.totalKHashes.c_str(), data.currentHashRate.c_str(), data.temp.c_str());
  if (segundo % 30 == 0)
  {
    int random_number = 1 + (esp_random() % 4);
    if (random_number == 1)
    {
      background.pushImage(0, 0, ImagenM8AXWidth, ImagenM8AXHeight, ImagenM8AX);
      background.pushSprite(0, 0);
    }
    else if (random_number == 2)
    {
      background.pushImage(0, 0, M8AXRelojLunarWidth, M8AXRelojLunarHeight, M8AXRelojLunar);
      background.pushSprite(0, 0);
    }
    else if (random_number == 3)
    {
      background.pushImage(0, 0, M8AXQuote1Width, M8AXQuote1Height, M8AXQuote1);
      background.pushSprite(0, 0);
    }
    else if (random_number == 4)
    {
      background.pushImage(0, 0, M8AXQuote2Width, M8AXQuote2Height, M8AXQuote2);
      background.pushSprite(0, 0);
    }
  }
  actualizarc = 0;
  actual = 0;
  correccion = 0;
  actuanot = 0;
  tft.setTextColor(TFT_WHITE);
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.setTextSize(1);
  tft.setCursor(1, 90);
  tft.print("'M8AX - Las Cifras' Num - " + String(totalci) + ". AC - " + String(aciertos) + " FA - " + String(fallos));
  tft.drawLine(0, 100, 320, 100, colors[colorI]); // Dibujar línea de (0, y) a (320, y)
  int numeritos[6];
  int destino = 1 + (esp_random() % 1000);
  generate_random_numbers(numeritos, 6, 1, 100);

  // Imprime los números generados
  tft.setCursor(2, 104);
  tft.print("Numeros - ");
  tft.print(numeritos[0]);
  tft.print(" ");
  tft.print(numeritos[1]);
  tft.print(" ");
  tft.print(numeritos[2]);
  tft.print(" ");
  tft.print(numeritos[3]);
  tft.print(" ");
  tft.print(numeritos[4]);
  tft.print(" ");
  tft.print(numeritos[5]);
  tft.setCursor(2, 114);
  tft.print("Numero Objetivo - " + String(destino));
  tft.setCursor(160, 114);
  int millonario = atoi(data.valids.c_str());
  if (millonario == 0)
  {
    tft.print("NoRICO - ");
    tft.print(data.currentHashRate);
    tft.print(" KH/s. ");
    tft.print(data.temp);
    tft.print(" g");
  }
  else
  {
    tft.print("SiRICO - ");
    tft.print(data.currentHashRate);
    tft.print(" KH/s. ");
    tft.print(data.temp);
    tft.print(" g");
  }
  tft.drawLine(0, 124, 320, 124, colors[colorI]); // Dibujar línea de (0, y) a (320, y)
  tft.setCursor(2, 128);
  Serial.print("M8AX - Números: ");
  for (int i = 0; i < 6; i++)
  {
    Serial.print(numeritos[i]); // Imprime el número
    Serial.print(" ");          // Imprime un espacio
  }
  Serial.println();
  Serial.print("M8AX - Objetivo: ");
  Serial.println("M8AX . " + String(destino));

  int X_INICIO = 173;                                      // Columna fija
  int X_FINAL = 315;                                       // Base de la barra
  int Y_POS = 107;                                         // Punto más alto
  int longitud_total = X_FINAL - X_INICIO;                 // Longitud total de la barra (80 píxeles)
  int longitud_pintada = (longitud_total * segundos) / 59; // Porcentaje de la barra según los segundos
  tft.fillRect(X_INICIO, Y_POS - 1, longitud_total + 1, 3, TFT_BLACK);
  tft.drawLine(X_INICIO, Y_POS, X_INICIO + longitud_pintada, Y_POS, TFT_WHITE);

  // Calcula las operaciones

  calculate_operations(numeritos, destino, result);
  tft.print(result);
  uint16_t colorss[] = {TFT_WHITE, TFT_YELLOW, TFT_CYAN, TFT_GREENYELLOW, TFT_LIGHTGREY, TFT_SILVER}; // Array de colores
  int colorrr = esp_random() % 6;
  dibujaQR(String(horita), 0, 0, 98, colorss[colorrr]); // Dibuja el QR centrado en la pantalla 320x170
  colorrr = esp_random() % 6;
  dibujaQR(String(minutitos), 115, 0, 98, colorss[colorrr]); // Dibuja el QR centrado en la pantalla 320x170
  colorrr = esp_random() % 6;
  dibujaQR(String(segundos), 228, 0, 98, colorss[colorrr]); // Dibuja el QR centrado en la pantalla 320x170
  tft.setTextSize(5);
  tft.setCursor(87, 30);
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.setTextColor(colors[colorI]);
  tft.print(":");
  tft.setCursor(202, 30);
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.setTextColor(colors[colorI]);
  tft.setTextColor(TFT_WHITE);
  tft.print(":");
  Serial.print("M8AX - " + String(result));
  tft.setTextSize(1);
}

// Función principal para mostrar la simulación de un dado y una moneda en la pantalla,
// junto con información relacionada con la minería de criptomonedas, la hora y la temperatura.
// Esta función se ejecuta repetidamente en cada ciclo de actualización, mostrando datos
// como la tasa de hashrate, el estado del dado (número entre 1 y 6), y el resultado de la
// moneda (cara o cruz) junto con la hora actual y la temperatura de la minería

void monedaYdado(unsigned long mElapsed)
{
  mining_data data = getMiningData(mElapsed);
  clock_data dataa = getClockData(mElapsed);
  monedilla = getCoinData(mElapsed);
  relojete = getClockData(mElapsed);
  mineria = getMiningData(mElapsed);
  int horita = dataa.currentTime.substring(0, 2).toInt();
  int minutitos = dataa.currentTime.substring(3, 5).toInt();
  unsigned long segundo = timeClient.getSeconds();
  int segundos = segundo % 60;
  char horaStr[9]; // Buffer para almacenar la hora en formato HH:MM:SS
  sprintf(horaStr, "%02d:%02d:%02d", horita, minutitos, segundos);
  incrementCounter();
  if (actualizarc == 0)
  {
    background.pushImage(0, 0, ImagenM8AXWidth, ImagenM8AXHeight, ImagenM8AX);
    background.pushSprite(0, 0);
    actualizarc++;
  }
  background.pushSprite(0, 0);
  Serial.printf("M8AX - >>> Completados %s Share(s), %s Khashes, Prom. Hashrate %s KH/s %s°\n",
                data.completedShares.c_str(), data.totalKHashes.c_str(), data.currentHashRate.c_str(), data.temp.c_str());
  if (segundo % 30 == 0)
  {
    int random_number = 1 + (esp_random() % 4);
    if (random_number == 1)
    {
      background.pushImage(0, 0, ImagenM8AXWidth, ImagenM8AXHeight, ImagenM8AX);
      background.pushSprite(0, 0);
    }
    else if (random_number == 2)
    {
      background.pushImage(0, 0, M8AXRelojLunarWidth, M8AXRelojLunarHeight, M8AXRelojLunar);
      background.pushSprite(0, 0);
    }
    else if (random_number == 3)
    {
      background.pushImage(0, 0, M8AXQuote1Width, M8AXQuote1Height, M8AXQuote1);
      background.pushSprite(0, 0);
    }
    else if (random_number == 4)
    {
      background.pushImage(0, 0, M8AXQuote2Width, M8AXQuote2Height, M8AXQuote2);
      background.pushSprite(0, 0);
    }
  }
  actual = 0;
  correccion = 0;
  actuanot = 0;
  actualizarcalen = 0;
  int dado = esp_random() % 6 + 1; // Números del 1 al 6
  int moneda = esp_random() % 2;   // 0 (cara), 1 (cruz)
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(108, 88);
  tft.setTextSize(1);
  tft.print("DADO - ");
  tft.print(dado);
  tft.setCursor(108, 99);
  tft.print("MONEDA - ");
  tft.print(moneda == 0 ? "CARA" : "CRUZ");
  tft.setCursor(108, 110);
  tft.print(horaStr);
  tft.setCursor(108, 121);
  tft.print("TEMP - ");
  tft.print(data.temp.c_str());
  tft.print(" G");
  tft.setTextSize(2);
  tft.setCursor(46, 4);
  tft.print("EL DADO Y LA MONEDA");
  tft.setCursor(46, 27);
  tft.print("DE LA SUERTE");
  String cadena = data.currentHashRate.c_str(); // O "234,65" si necesitas reemplazar la coma
  cadena.replace(',', '.');                     // Si es necesario
  String valor = data.currentHashRate;
  int parteEntera = valor.substring(0, valor.indexOf(".")).toInt();
  int parteDecimal = valor.substring(valor.indexOf(".") + 1).toInt();
  dibujarDado(dado, 60, 85);
  dibujarMoneda(moneda, 250, 80);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(0, 132);
  tft.setTextSize(2);
  tft.setTextDatum(MC_DATUM); // Centra el texto
  tft.print(quitarAcentos(numeroAEscrito(parteEntera, false)) + " Con ");
  tft.print(quitarAcentos(numeroAEscrito(parteDecimal, true)) + " KH/s.");
  tft.setCursor(115, 50);
  tft.setTextSize(2);
  tft.print(cadena);
  tft.setCursor(114, 69);
  tft.print(" KH/s");
  tft.setTextSize(1);
}

// Funcionalidad:
// 1. Recupera los datos de minería utilizando la función `getMiningData()` para mostrar información sobre
//    el número de "shares" completados, los "khashes" totales, la tasa de hashrate y la temperatura.
// 2. Si el contador `actuanot` alcanza un múltiplo de 10 o si `correccion` es igual a 0, selecciona
//    aleatoriamente un fondo de una lista de imágenes predefinidas para mostrar en la pantalla, usando
//    la función `esp_random()` para generar el índice aleatorio.
// 3. Actualiza la pantalla con el fondo seleccionado y dibuja el texto centrado "M 8 A X", "JUEGO DE", y "LA VIDA",
//    indicativos del nombre del juego y su propósito visual.
// 4. Rellena la pantalla con un fondo negro y reinicia la cuadrícula del "Juego de la Vida" llamando a las
//    funciones `initGrid()` y `drawGrid()`.
// 5. Inicia una simulación del "Juego de la Vida" con un número máximo de generaciones (`MAX_GEN_COUNT`).
//    En cada generación, se calcula el siguiente estado de la cuadrícula (celulas vivas/muertas) mediante la
//    función `computeCA()`, luego se actualiza la cuadrícula y se vuelve a dibujar en pantalla, haciendo uso de
//    la función `drawGrid()`. Este ciclo se repite con un pequeño retraso (`GEN_DELAY`) entre cada paso.
// 6. Los valores de la cuadrícula se actualizan entre generaciones con la lógica de pasar de `newgrid` a `grid`,
//    donde las celdas se actualizan en función de las reglas del "Juego de la Vida".
// 7. Después de completar todas las generaciones, se restablecen varios contadores y banderas como `actual`,
//    `actualizarc`, `actualizarcalen` y `correccion` para la próxima iteración de la función.
// 8. La función se asegura de dibujar y mostrar información visual dinámica como el "Juego de la Vida" y las
//    estadísticas de minería, en conjunto con las imágenes de fondo aleatorias, creando una experiencia visual
//    completa e interactiva para el usuario.

#define VIDA_SLICE_MS 1000
static bool vidaInit = false;
static unsigned long vidaUlt = 0;
static uint16_t vidaGen = 0;

void tDisplay_m8axvida(unsigned long mElapsed)
{
  mining_data data = getMiningData(mElapsed);
  monedilla = getCoinData(mElapsed);
  relojete = getClockData(mElapsed);
  mineria = getMiningData(mElapsed);
  incrementCounter();
  Serial.printf("M8AX - >>> Completados %s Share(s), %s Khashes, Prom. Hashrate %s KH/s %s°\n",
                data.completedShares.c_str(), data.totalKHashes.c_str(), data.currentHashRate.c_str(), data.temp.c_str());
  bool nueva = !vidaInit || (millis() - vidaUlt > 2500) || vidaGen >= MAX_GEN_COUNT;
  if (nueva)
  {
    vidaInit = true;
    vidaGen = 0;
    int random_number = 1 + (esp_random() % 4);
    if (random_number == 1)
    {
      background.pushImage(0, 0, ImagenM8AXWidth, ImagenM8AXHeight, ImagenM8AX);
      background.pushSprite(0, 0);
    }
    else if (random_number == 2)
    {
      background.pushImage(0, 0, M8AXRelojLunarWidth, M8AXRelojLunarHeight, M8AXRelojLunar);
      background.pushSprite(0, 0);
    }
    else if (random_number == 3)
    {
      background.pushImage(0, 0, M8AXQuote1Width, M8AXQuote1Height, M8AXQuote1);
      background.pushSprite(0, 0);
    }
    else if (random_number == 4)
    {
      background.pushImage(0, 0, M8AXQuote2Width, M8AXQuote2Height, M8AXQuote2);
      background.pushSprite(0, 0);
    }
    correccion = 1;
    tft.setTextSize(2);
    drawCenteredText("M 8 A X", 30, 100);
    drawCenteredText("J U E G O   D E", 70, 100);
    drawCenteredText("L A   V I D A", 110, 100);
    tft.setTextSize(1);
    tft.setCursor(0, 0);
    tft.fillScreen(TFT_BLACK);
    initGrid();
    genCount = MAX_GEN_COUNT;
    drawGrid();
  }
  unsigned long start = millis();
  while (millis() - start < VIDA_SLICE_MS && vidaGen < MAX_GEN_COUNT)
  {
    computeCA();
    drawGrid();
    delay(GEN_DELAY);
    for (int16_t x = 1; x < GRIDX - 1; x++)
    {
      for (int16_t y = 1; y < GRIDY - 1; y++)
      {
        grid[x][y] = newgrid[x][y];
      }
    }
    vidaGen++;
  }
  vidaUlt = millis();
  actualizarcalen = 0;
  actualizarc = 0, actual = 0;
}

// Función que gestiona la visualización de un reloj en formato de números romanos en la pantalla TFT,
// junto con información sobre minería, temperatura, tiempo y día, en una interfaz de usuario interactiva.
// La función actualiza la pantalla con la hora actual, el día de la semana, la temperatura, el hash rate y
// otros datos en un formato llamativo y dinámico, con colores aleatorios para los textos y barras de progreso
// para representar el tiempo y la actividad de minería.
//
// Parámetros:
// - mElapsed: Tiempo transcurrido desde el inicio o la última actualización.
//
// Funcionalidad:
// 1. Recupera los datos de minería, reloj y moneda utilizando las funciones `getMiningData()`, `getClockData()` y
//    `getCoinData()` para obtener la información que se mostrará en la pantalla.
// 2. Extrae y calcula los componentes de la hora, minuto y segundo a partir de los datos de reloj, dividiendo los
//    valores de horas, minutos y segundos en dígitos individuales para su visualización.
// 3. Calcula el tiempo transcurrido en el día en segundos (`segundosDelDia`) y convierte las horas, minutos y
//    segundos en números romanos utilizando la función `numeroAEscrito()` para mostrarlos en la pantalla.
// 4. Recupera el día de la semana y la fecha actual en formato "día/mes/año" para mostrar información adicional
//    sobre la fecha y el estado del reloj.
// 5. Si el valor de `actualizarc` es 0, selecciona aleatoriamente un fondo visual de una lista predefinida de
//    imágenes usando el valor generado por `esp_random()`, y luego lo muestra en la pantalla.
// 6. Muestra la hora en formato de números romanos (de 1 a 10 para las horas, minutos y segundos) en diferentes
//    posiciones de la pantalla con colores aleatorios.
// 7. Muestra la fecha, el hash rate, y si el usuario es "millonario" (según un cálculo basado en `valids` de minería)
//    en varias posiciones de la pantalla, usando colores aleatorios para cada texto.
// 8. Actualiza barras de progreso que representan el tiempo transcurrido en el día (0-24 horas) y el nivel de
//    actividad de minería (0-360 KH/s). Las barras cambian de color aleatoriamente según el valor calculado.
// 9. La función también actualiza la información en la pantalla de manera continua con cada llamada, mostrando
//    datos actualizados sobre el reloj, minería y temperatura, lo que permite tener un reloj interactivo con
//    detalles sobre la actividad de la minería en tiempo real.
// 10. Al final de la ejecución, todos los textos y gráficos se muestran con colores aleatorios para mejorar
//     la estética visual.

void RelojDeNumeros(unsigned long mElapsed)
{
  mining_data data = getMiningData(mElapsed);
  clock_data dataa = getClockData(mElapsed);
  monedilla = getCoinData(mElapsed);
  relojete = getClockData(mElapsed);
  mineria = getMiningData(mElapsed);
  int millonario = atoi(data.valids.c_str());
  incrementCounter();
  mirarTiempo = 0;
  unsigned long segundo = timeClient.getSeconds();
  int horas = dataa.currentTime.substring(0, 2).toInt();
  int minutos = dataa.currentTime.substring(3, 5).toInt();
  int segundos = segundo % 60;
  int horis1 = horas / 10;       // Primer dígito de las horas
  int horis2 = horas % 10;       // Segundo dígito de las horas
  int minutis1 = minutos / 10;   // Primer dígito de los minutos
  int minutis2 = minutos % 10;   // Segundo dígito de los minutos
  int segundis1 = segundos / 10; // Primer dígito de los segundos
  int segundis2 = segundos % 10; // Segundo dígito de los segundos
  if (segundos % 59 == 0)
  {
    actualizarc = 0;
  }
  std::string quediase = obtenerDiaSemana(std::string(dataa.currentDate.c_str()));
  int dia = dataa.currentDate.substring(0, 2).toInt();
  int mes = dataa.currentDate.substring(3, 5).toInt();
  int anio = dataa.currentDate.substring(6, 10).toInt();
  int horis = dataa.currentTime.substring(0, 2).toInt();
  int mins = dataa.currentTime.substring(3, 5).toInt();
  int segundosDelDia = (horis * 3600) + (mins * 60) + segundos;
  String hRoman1 = numeroAEscrito(horis1);
  hRoman1.toUpperCase();
  String hRoman2 = numeroAEscrito(horis2);
  hRoman2.toUpperCase();
  String mRoman1 = numeroAEscrito(minutis1);
  mRoman1.toUpperCase();
  String mRoman2 = numeroAEscrito(minutis2);
  mRoman2.toUpperCase();
  String sRoman1 = numeroAEscrito(segundis1);
  sRoman1.toUpperCase();
  String sRoman2 = numeroAEscrito(segundis2);
  sRoman2.toUpperCase();
  String RDia = String(dia);
  String RMes = String(mes);
  String RAnio = String(anio);
  String TempCPU = String(data.temp.c_str());
  if (actualizarc == 0)
  {
    random_number = 1 + (esp_random() % 4);
  }
  if (random_number == 1)
  {
    background.pushImage(0, 0, ImagenM8AXWidth, ImagenM8AXHeight, ImagenM8AX);
    background.pushSprite(0, 0);
  }
  else if (random_number == 2)
  {
    background.pushImage(0, 0, M8AXRelojLunarWidth, M8AXRelojLunarHeight, M8AXRelojLunar);
    background.pushSprite(0, 0);
  }
  else if (random_number == 3)
  {
    background.pushImage(0, 0, M8AXQuote1Width, M8AXQuote1Height, M8AXQuote1);
    background.pushSprite(0, 0);
  }
  else if (random_number == 4)
  {
    background.pushImage(0, 0, M8AXQuote2Width, M8AXQuote2Height, M8AXQuote2);
    background.pushSprite(0, 0);
  }
  actualizarc++;
  Serial.printf("M8AX - >>> Completados %s Share(s), %s Khashes, Prom. Hashrate %s KH/s %s°\n",
                data.completedShares.c_str(), data.totalKHashes.c_str(), data.currentHashRate.c_str(), data.temp.c_str());
  tft.setTextSize(3);
  tft.setCursor(3, 2);
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.setTextColor(colors[colorI]);
  tft.print(hRoman1);
  tft.setCursor(133, 2);
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.setTextColor(colors[colorI]);
  tft.print(hRoman2);
  tft.setCursor(3, 77);
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.setTextColor(colors[colorI]);
  tft.print(mRoman1);
  tft.setCursor(133, 77);
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.setTextColor(colors[colorI]);
  tft.print(mRoman2);
  tft.setCursor(3, 148);
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.setTextColor(colors[colorI]);
  tft.print(sRoman1);
  tft.setCursor(133, 148);
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.setTextColor(colors[colorI]);
  tft.print(sRoman2);
  tft.setTextSize(1);
  tft.setCursor(250, 2);
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.setTextColor(colors[colorI]);
  tft.print("d " + RDia);
  tft.setCursor(250, 16);
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.setTextColor(colors[colorI]);
  tft.print("m " + RMes);
  tft.setCursor(250, 30);
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.setTextColor(colors[colorI]);
  tft.print("a " + RAnio);
  tft.setCursor(250, 44);
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.setTextColor(colors[colorI]);
  tft.print(String(quediase.c_str()));
  tft.setCursor(250, 58);
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.setTextColor(colors[colorI]);
  tft.print(String(data.currentHashRate.c_str()) + " KH/s");
  if (millonario == 1)
  {
    tft.setCursor(250, 72);
    colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
    tft.setTextColor(colors[colorI]);
    tft.print("ERES RICO");
  }
  else
  {
    tft.setCursor(250, 72);
    colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
    tft.setTextColor(colors[colorI]);
    tft.print("NO RICO");
  }
  tft.setCursor(250, 93);
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.setTextColor(colors[colorI]);
  tft.print("V.BLOCKS");
  tft.setCursor(260, 109);
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.setTextColor(colors[colorI]);
  tft.setTextSize(5);
  tft.print(millonario);
  tft.setTextSize(1);
  tft.setCursor(250, 153);
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.setTextColor(colors[colorI]);
  tft.print("T " + TempCPU + "g");
  tft.setTextSize(1);
  tft.setCursor(5, 50);
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.setTextColor(colors[colorI]);
  tft.print("0 KH/s ");
  tft.setCursor(192, 50);
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.setTextColor(colors[colorI]);
  tft.print("360 KH/s");
  int grosor = 5;
  int X_INICIO = 5;                                                             // Columna fija
  int X_FINAL = 242;                                                            // Base de la barra
  int Y_POS = 43;                                                               // Punto más alto
  int longitud_total = X_FINAL - X_INICIO;                                      // Longitud total de la barra (80 píxeles)
  int longitud_pintada = (longitud_total * data.currentHashRate.toInt()) / 360; // Porcentaje de la barra según los segundos
  tft.fillRect(X_INICIO, Y_POS - (grosor / 2), longitud_total + 1, grosor, TFT_BLACK);
  tft.fillRect(X_INICIO, Y_POS - (grosor / 2), longitud_pintada, grosor, colors[colorI]);
  tft.setCursor(5, 124);
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.setTextColor(colors[colorI]);
  tft.print("0 Horas");
  tft.setCursor(192, 124);
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.setTextColor(colors[colorI]);
  tft.print("24 Horas");
  grosor = 5;
  X_INICIO = 4;                                                 // Columna fija
  X_FINAL = 242;                                                // Base de la barra
  Y_POS = 117;                                                  // Punto más alto
  longitud_total = X_FINAL - X_INICIO;                          // Longitud total de la barra (80 píxeles)
  longitud_pintada = (longitud_total * segundosDelDia) / 86400; // Porcentaje de la barra según los segundos
  tft.fillRect(X_INICIO, Y_POS - (grosor / 2), longitud_total + 1, grosor, TFT_BLACK);
  tft.fillRect(X_INICIO, Y_POS - (grosor / 2), longitud_pintada, grosor, colors[colorI]);
}

// Función que maneja la actualización de la pantalla con información en texto plano sobre minería y reloj,
// mostrando una imagen de fondo seleccionada aleatoriamente, y actualizando los datos de la minería cada ciertos
// intervalos de tiempo. Además, gestiona la visualización continua de información textual sobre el estado de la
// minería y la actividad en tiempo real. También incluye la gestión de imágenes aleatorias para dar variedad visual.
//
// Parámetros:
// - mElapsed: Tiempo transcurrido desde el inicio o la última actualización.
//
// Funcionalidad:
// 1. Recupera los datos de minería, reloj y moneda utilizando las funciones `getMiningData()`, `getClockData()`
//    y `getCoinData()` para obtener la información que se mostrará en la pantalla.
// 2. Se obtiene el tiempo actual (segundos) utilizando la función `timeClient.getSeconds()` para poder manejar
//    los intervalos de tiempo para la actualización de la pantalla.
// 3. Se incrementa el contador mediante `incrementCounter()` para hacer un seguimiento del tiempo transcurrido o
//    el número de actualizaciones.
// 4. Si el valor de `actualizarc` es 0, se selecciona una imagen de fondo aleatoria y se muestra en la pantalla.
//    Las imágenes de fondo pueden ser: `ImagenM8AX`, `M8AXRelojLunar`, `M8AXQuote1` o `M8AXQuote2`.
//    Esta imagen se muestra al inicio de la función y cada vez que `segundo % 30 == 0` para crear un cambio visual.
// 5. La función `datosPantallaTextoPlano()` se encarga de actualizar la información textual que se muestra sobre la
//    pantalla, como el estado de la minería, el tiempo y otros detalles en formato de texto plano.
// 6. El uso de `background.pushImage()` y `background.pushSprite()` asegura que se actualice la pantalla correctamente
//    con la nueva imagen o los datos mostrados.
// 7. La función imprime en el monitor serie el estado de la minería, mostrando el número de shares completados, el
//    total de Khashes, el hash rate actual y la temperatura.
// 8. Cada 30 segundos (`segundo % 30 == 0`), se elige aleatoriamente una nueva imagen de fondo y se muestra en la
//    pantalla, proporcionando una experiencia visual dinámica.
// 9. Cada 5 segundos (`segundo % 5 == 0`), se actualiza nuevamente la información textual sobre la pantalla para
//    mostrar los datos más recientes sobre la minería.
// 10. Al final de la ejecución, los valores de las variables `actual`, `correccion`, `actuanot` y `actualizarcalen`
//     se restablecen a 0 para preparar la función para la siguiente iteración.

void datoTextPlano(unsigned long mElapsed)
{
  mining_data data = getMiningData(mElapsed);
  clock_data dataa = getClockData(mElapsed);
  monedilla = getCoinData(mElapsed);
  relojete = getClockData(mElapsed);
  mineria = getMiningData(mElapsed);
  unsigned long segundo = timeClient.getSeconds();
  incrementCounter();
  if (actualizarc == 0)
  {
    background.pushImage(0, 0, ImagenM8AXWidth, ImagenM8AXHeight, ImagenM8AX);
    background.pushSprite(0, 0);
    actualizarc++;
    background.pushSprite(0, 0);
    datosPantallaTextoPlano();
  }
  Serial.printf("M8AX - >>> Completados %s Share(s), %s Khashes, Prom. Hashrate %s KH/s %s°\n",
                data.completedShares.c_str(), data.totalKHashes.c_str(), data.currentHashRate.c_str(), data.temp.c_str());
  if (segundo % 30 == 0)
  {
    int random_number = 1 + (esp_random() % 4);
    if (random_number == 1)
    {
      background.pushImage(0, 0, ImagenM8AXWidth, ImagenM8AXHeight, ImagenM8AX);
      background.pushSprite(0, 0);
    }
    else if (random_number == 2)
    {
      background.pushImage(0, 0, M8AXRelojLunarWidth, M8AXRelojLunarHeight, M8AXRelojLunar);
      background.pushSprite(0, 0);
    }
    else if (random_number == 3)
    {
      background.pushImage(0, 0, M8AXQuote1Width, M8AXQuote1Height, M8AXQuote1);
      background.pushSprite(0, 0);
    }
    else if (random_number == 4)
    {
      background.pushImage(0, 0, M8AXQuote2Width, M8AXQuote2Height, M8AXQuote2);
      background.pushSprite(0, 0);
    }
  }
  if (segundo % 5 == 0)
  {
    background.pushSprite(0, 0);
    datosPantallaTextoPlano();
  }
  actual = 0;
  correccion = 0;
  actuanot = 0;
  actualizarcalen = 0;
}

// Función que muestra una pantalla de carga con efectos visuales aleatorios al iniciar el sistema,
// mostrando una imagen de inicio y el texto que indica la versión actual del programa.
//
// Funcionalidad:
// 1. Se genera un número aleatorio entre 0 y 7 utilizando `esp_random() % 8`, lo que determina cuál de los
//    efectos visuales se aplicará durante la pantalla de carga. Los efectos posibles son:
//    - `cortinas()`
//    - `M8AXTicker2()`
//    - `M8AXTicker()`
//    - `M8AXTicker3()`
//    - `M8AXTicker4()`
//    - `nevar3()`
//    - `animacionInicio()`
//    - `M8AXTicker5()`
//    Cada uno de estos efectos está definido en otras funciones, y se ejecuta uno de ellos de forma aleatoria.
// 2. Después de aplicar el efecto visual correspondiente, se limpia la pantalla con `tft.fillScreen(TFT_BLACK)`
//    para asegurarse de que no haya restos de otras pantallas previas.
// 3. Se dibuja la imagen de inicio en la pantalla con `tft.pushImage(0, 0, initWidth, initHeight, initScreen)`,
//    donde `initScreen` es la imagen de inicio que se va a mostrar, utilizando sus dimensiones `initWidth`
//    y `initHeight`.
// 4. Se establece el color del texto a negro con `tft.setTextColor(TFT_BLACK)` y el tamaño de texto a 1 con
//    `tft.setTextSize(1)` para las siguientes cadenas de texto.
// 5. Se dibuja el texto de la versión actual del programa en las coordenadas (25, 148) usando la fuente
//    `FONT2` y el texto correspondiente a la constante `CURRENT_VERSION`. Esto muestra la versión del software
//    en la pantalla de carga.

void tDisplay_LoadingScreen(void)
{
  int effect = esp_random() % 8;
  arrankito = millis();
  switch (effect)
  {
  case 0:
    cortinas();
    break;
  case 1:
    M8AXTicker2();
    break;
  case 2:
    M8AXTicker();
    break;
  case 3:
    M8AXTicker3();
    break;
  case 4:
    M8AXTicker4();
    break;
  case 5:
    nevar3();
    break;
  case 6:
    animacionInicio();
    break;
  case 7:
    M8AXTicker5();
    break;
  }
  tft.fillScreen(TFT_BLACK);
  tft.pushImage(0, 0, initWidth, initHeight, initScreen);
  tft.setTextColor(TFT_BLACK);
  tft.setTextSize(1);
  tft.drawString(CURRENT_VERSION, 25, 148, FONT2);
}

// Función que muestra la pantalla de configuración del dispositivo o programa.
//
// Funcionalidad:
// 1. Se utiliza la función `tft.pushImage` para mostrar una imagen de la pantalla de configuración en el
//    display TFT.
// 2. Los parámetros pasados a `tft.pushImage` son:
//    - `(0, 0)`: Las coordenadas de inicio donde se colocará la imagen en la pantalla, en este caso, la
//      esquina superior izquierda de la pantalla (0,0).
//    - `setupModeWidth`: El ancho de la imagen que se quiere mostrar.
//    - `setupModeHeight`: La altura de la imagen que se quiere mostrar.
//    - `setupModeScreen`: El array de bytes que contiene la imagen que se debe mostrar.

void tDisplay_SetupScreen(void)
{
  tft.pushImage(0, 0, setupModeWidth, setupModeHeight, setupModeScreen);
}

/****************************************************************************************************************
 *
 *   Escrito por: M8AX
 *
 *   Descripción: AJEDREZ CPU VS CPU - Pantalla para NerdminerV2
 *   -----------------------------------------------------------
 *
 *   Dos CPUs juegan al ajedrez de verdad en la pantalla del minero, con todas las reglas: enroque corto y
 *   largo, captura al paso, coronación, jaque, jaque mate, tablas por ahogado, por repetición (3 veces),
 *   por la regla de los 50 movimientos y por falta de material. Nada de partidas guionizadas, cada jugada
 *   sale de una búsqueda (alfa-beta con tabla de transposición, jugada nula, reducciones, evaluación con
 *   peones pasados, movilidad y seguridad del rey; ~1800-1900 de Elo estimado) y entre jugadas parecidas sortea una, así que no
 *   se repite ninguna partida xD...
 *
 *   Arriba a la izquierda la hora y arriba a la derecha el hashrate, igual que en el Pong. Abajo el
 *   marcador: CPU-1 a la izquierda, CPU-2 a la derecha. Cuando acaba una partida el ganador suma 1 punto
 *   y el otro se queda como estaba (las tablas no puntúan, se cuentan en el numerito del centro).
 *   Cada partida se cambia de color, para que ninguna CPU tenga siempre las blancas.
 *
 *   Los paneles de los lados enseñan el color de cada CPU, quién está pensando, el balance de material y
 *   sus últimas jugadas en notación algebraica (C = caballo, A = alfil, T = torre, D = dama, R = rey).
 *   La última jugada y el rey en jaque se marcan en el tablero.
 *
 *   Para activarla solo hay que meter tDisplay_Ajedrez en el array
 *   esp32_2432S028RCyclicScreens[] y pegar este bloque antes de ese array (encima de analiCadaSegundo vale).
 *
 *   El motor (todo lo que hay entre ">>> MOTOR" y "<<< MOTOR") no toca la pantalla para nada, así que se
 *   puede probar en el PC aparte. Pasa el perft en 6 posiciones de referencia, que es la prueba de que las
 *   reglas están bien hechas.
 *
 ****************************************************************************************************************/

// >>> MOTOR
#define CH_P 1
#define CH_N 2
#define CH_B 3
#define CH_R 4
#define CH_Q 5
#define CH_K 6

#define CHF_CAP 1   // captura
#define CHF_EP 2    // captura al paso
#define CHF_ENR 4   // enroque
#define CHF_DOBLE 8 // peon que sale de dos en dos

#define CH_MAXMOV 128
#define CH_MAXPLY 16 // plies maximos de la busqueda (incluye la busqueda de capturas)
#define CH_MATE 30000
#define CH_INF 32000
#define CH_KI(s) ((s) == 1 ? 0 : 1)

#ifndef CH_TT_BITS
#define CH_TT_BITS 10 // tabla de transposicion: 2^10 entradas x 8 bytes = 8 KB (pon 11 para 16 KB, 12 para 32 KB)
#endif
#ifndef CH_TT_RESERVA
#define CH_TT_RESERVA 60000 // bytes de heap que deben quedar libres tras reservar la tabla (si no cabe, juega sin tabla)
#endif
#ifndef CH_CONTEMPT
#define CH_CONTEMPT 12 // centipeones: prefieren jugar a ganar antes que tablas si estan igualadas
#endif
#ifndef CH_CEDE
#define CH_CEDE() vTaskDelay(1) // cede CPU al minero cada cierto numero de nodos
#endif

struct ChMove
{
  int8_t from, to, promo;
  uint8_t flag;
};

struct ChUndo
{
  int8_t cap, ep;
  uint8_t castle, half;
  uint64_t key;
};

// Casillas 0..63: 0 = a8 (arriba izquierda) y 63 = h1. Blancas en positivo, negras en negativo
struct ChGame
{
  int8_t sq[64];
  int8_t side;    // 1 = mueven blancas, -1 = mueven negras
  uint8_t castle; // 1 = O-O blancas, 2 = O-O-O blancas, 4 = O-O negras, 8 = O-O-O negras
  int8_t ep;      // casilla "al paso" o -1
  uint8_t half;   // medios movimientos desde la ultima captura o peon (regla de los 50)
  int8_t kp[2];   // donde esta cada rey (0 = blanco, 1 = negro)
  uint64_t key;   // clave Zobrist de la posicion (se actualiza sola en chMake/chUnmake)
};

// ---- Zobrist y tablas de saltos del caballo ----
// RAM del motor: va en el HEAP y solo existe mientras estas en la pantalla de ajedrez (chMemAlloc / chMemLibera).
// Asi, en las demas pantallas el ajedrez no ocupa nada (~22 KB menos de RAM fija que antes).
struct ChMem
{
  uint64_t Z[13][64], Zs, Zc[16], Ze[8];
  int8_t Kn[64][9]; // destinos del caballo desde cada casilla, terminado en -1
  ChMove buf[CH_MAXPLY + 1][CH_MAXMOV];
  int16_t sc[CH_MAXPLY + 1][CH_MAXMOV];
  uint16_t kill[CH_MAXPLY + 1][2]; // jugadas "asesinas"
  int16_t hist[7][64];             // historial de jugadas tranquilas buenas
  uint64_t path[CH_MAXPLY + 2];    // claves de la rama actual (repeticiones)
};
static ChMem *chMem = NULL;
#define chZ (chMem->Z)
#define chZs (chMem->Zs)
#define chZc (chMem->Zc)
#define chZe (chMem->Ze)
#define chKn (chMem->Kn)
#define chBuf (chMem->buf)
#define chSc (chMem->sc)
#define chKill (chMem->kill)
#define chHist (chMem->hist)
#define chPath (chMem->path)
static bool chZok = false;

static uint64_t chSplit(uint64_t &x)
{
  x += 0x9E3779B97F4A7C15ULL;
  uint64_t z = x;
  z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ULL;
  z = (z ^ (z >> 27)) * 0x94D049BB133111EBULL;
  return z ^ (z >> 31);
}

static void chZInit()
{
  if (chZok)
    return;
  uint64_t x = 0x4D38415853454E47ULL;
  for (int i = 0; i < 13; i++)
    for (int j = 0; j < 64; j++)
      chZ[i][j] = chSplit(x);
  chZs = chSplit(x);
  for (int i = 0; i < 16; i++)
    chZc[i] = chSplit(x);
  for (int i = 0; i < 8; i++)
    chZe[i] = chSplit(x);
  static const int8_t kn[8][2] = {{-2, -1}, {-2, 1}, {-1, -2}, {-1, 2}, {1, -2}, {1, 2}, {2, -1}, {2, 1}};
  for (int i = 0; i < 64; i++)
  {
    int k = 0;
    for (int d = 0; d < 8; d++)
    {
      int r = (i >> 3) + kn[d][0], c = (i & 7) + kn[d][1];
      if (r >= 0 && r < 8 && c >= 0 && c < 8)
        chKn[i][k++] = r * 8 + c;
    }
    chKn[i][k] = -1;
  }
  chZok = true;
}

static uint64_t chKeyOf(const ChGame &g)
{
  uint64_t k = 0;
  for (int i = 0; i < 64; i++)
    if (g.sq[i])
      k ^= chZ[g.sq[i] + 6][i];
  if (g.side == -1)
    k ^= chZs;
  k ^= chZc[g.castle & 15];
  if (g.ep >= 0)
    k ^= chZe[g.ep & 7];
  return k;
}

static void chRecalcKey(ChGame &g)
{
  chZInit();
  g.key = chKeyOf(g);
}

static const int16_t chVal[7] = {0, 100, 320, 330, 500, 900, 0};

// Tablas de casillas (vista de las blancas, fila 0 = octava). 1..5 piezas, 6 rey medio juego, 7 rey final
static const int8_t chPst[8][64] = {
    {0},
    {0, 0, 0, 0, 0, 0, 0, 0, 50, 50, 50, 50, 50, 50, 50, 50, 10, 10, 20, 30, 30, 20, 10, 10, 5, 5, 10, 25, 25, 10, 5, 5, 0, 0, 0, 20, 20, 0, 0, 0, 5, -5, -10, 0, 0, -10, -5, 5, 5, 10, 10, -20, -20, 10, 10, 5, 0, 0, 0, 0, 0, 0, 0, 0},
    {-50, -40, -30, -30, -30, -30, -40, -50, -40, -20, 0, 0, 0, 0, -20, -40, -30, 0, 10, 15, 15, 10, 0, -30, -30, 5, 15, 20, 20, 15, 5, -30, -30, 0, 15, 20, 20, 15, 0, -30, -30, 5, 10, 15, 15, 10, 5, -30, -40, -20, 0, 5, 5, 0, -20, -40, -50, -40, -30, -30, -30, -30, -40, -50},
    {-20, -10, -10, -10, -10, -10, -10, -20, -10, 0, 0, 0, 0, 0, 0, -10, -10, 0, 5, 10, 10, 5, 0, -10, -10, 5, 5, 10, 10, 5, 5, -10, -10, 0, 10, 10, 10, 10, 0, -10, -10, 10, 10, 10, 10, 10, 10, -10, -10, 5, 0, 0, 0, 0, 5, -10, -20, -10, -10, -10, -10, -10, -10, -20},
    {0, 0, 0, 0, 0, 0, 0, 0, 5, 10, 10, 10, 10, 10, 10, 5, -5, 0, 0, 0, 0, 0, 0, -5, -5, 0, 0, 0, 0, 0, 0, -5, -5, 0, 0, 0, 0, 0, 0, -5, -5, 0, 0, 0, 0, 0, 0, -5, -5, 0, 0, 0, 0, 0, 0, -5, 0, 0, 0, 5, 5, 0, 0, 0},
    {-20, -10, -10, -5, -5, -10, -10, -20, -10, 0, 0, 0, 0, 0, 0, -10, -10, 0, 5, 5, 5, 5, 0, -10, -5, 0, 5, 5, 5, 5, 0, -5, 0, 0, 5, 5, 5, 5, 0, -5, -10, 5, 5, 5, 5, 5, 0, -10, -10, 0, 5, 0, 0, 0, 0, -10, -20, -10, -10, -5, -5, -10, -10, -20},
    {-30, -40, -40, -50, -50, -40, -40, -30, -30, -40, -40, -50, -50, -40, -40, -30, -30, -40, -40, -50, -50, -40, -40, -30, -30, -40, -40, -50, -50, -40, -40, -30, -20, -30, -30, -40, -40, -30, -30, -20, -10, -20, -20, -20, -20, -20, -20, -10, 20, 20, 0, 0, 0, 0, 20, 20, 20, 30, 10, 0, 0, 10, 30, 20},
    {-50, -40, -30, -20, -20, -30, -40, -50, -30, -20, -10, 0, 0, -10, -20, -30, -30, -10, 20, 30, 30, 20, -10, -30, -30, -10, 30, 40, 40, 30, -10, -30, -30, -10, 30, 40, 40, 30, -10, -30, -30, -10, 20, 30, 30, 20, -10, -30, -30, -30, 0, 0, 0, 0, -30, -30, -50, -30, -30, -30, -30, -30, -30, -50}};

// Si se mueve o se captura en estas casillas, se pierde el derecho a enrocar
static uint8_t chCastleMask(int i)
{
  switch (i)
  {
  case 60:
    return ~3;
  case 63:
    return ~1;
  case 56:
    return ~2;
  case 4:
    return ~12;
  case 7:
    return ~4;
  case 0:
    return ~8;
  }
  return 0xFF;
}

// Valores y tablas extra para el final de partida
static const int16_t chValEg[7] = {0, 120, 300, 320, 520, 950, 0};
static const int8_t chPstPEg[64] = {0, 0, 0, 0, 0, 0, 0, 0, 80, 80, 80, 80, 80, 80, 80, 80, 50, 50, 50, 50, 50, 50, 50, 50, 30, 30, 30, 30, 30, 30, 30, 30, 15, 15, 15, 15, 15, 15, 15, 15, 5, 5, 5, 5, 5, 5, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
static const int16_t chPasMg[8] = {0, 2, 5, 10, 20, 40, 70, 0}; // peon pasado segun fila relativa
static const int16_t chPasEg[8] = {0, 8, 15, 30, 55, 90, 140, 0};

static void chInicio(ChGame &g)
{
  static const int8_t fila[8] = {CH_R, CH_N, CH_B, CH_Q, CH_K, CH_B, CH_N, CH_R};
  memset(g.sq, 0, 64);
  for (int c = 0; c < 8; c++)
  {
    g.sq[c] = -fila[c];
    g.sq[8 + c] = -CH_P;
    g.sq[48 + c] = CH_P;
    g.sq[56 + c] = fila[c];
  }
  g.side = 1;
  g.castle = 15;
  g.ep = -1;
  g.half = 0;
  g.kp[0] = 60;
  g.kp[1] = 4;
  chRecalcKey(g);
}

// true si la casilla idx esta atacada por el bando "por" (1 blancas, -1 negras)
static bool chAtacada(const ChGame &g, int idx, int por)
{
  static const int8_t kn[8][2] = {{-2, -1}, {-2, 1}, {-1, -2}, {-1, 2}, {1, -2}, {1, 2}, {2, -1}, {2, 1}};
  static const int8_t dg[4][2] = {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}};
  static const int8_t rt[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
  int r = idx >> 3, c = idx & 7;

  int pr = r + (por == 1 ? 1 : -1); // un peon blanco ataca hacia arriba, asi que esta una fila por debajo
  if (pr >= 0 && pr < 8)
  {
    if (c > 0 && g.sq[pr * 8 + c - 1] == por * CH_P)
      return true;
    if (c < 7 && g.sq[pr * 8 + c + 1] == por * CH_P)
      return true;
  }
  for (int k = 0; k < 8; k++)
  {
    int nr = r + kn[k][0], nc = c + kn[k][1];
    if (nr >= 0 && nr < 8 && nc >= 0 && nc < 8 && g.sq[nr * 8 + nc] == por * CH_N)
      return true;
  }
  for (int dr = -1; dr <= 1; dr++)
    for (int dc = -1; dc <= 1; dc++)
    {
      if (!dr && !dc)
        continue;
      int nr = r + dr, nc = c + dc;
      if (nr >= 0 && nr < 8 && nc >= 0 && nc < 8 && g.sq[nr * 8 + nc] == por * CH_K)
        return true;
    }
  for (int d = 0; d < 4; d++) // diagonales: alfil o dama
  {
    int nr = r + dg[d][0], nc = c + dg[d][1];
    while (nr >= 0 && nr < 8 && nc >= 0 && nc < 8)
    {
      int p = g.sq[nr * 8 + nc];
      if (p)
      {
        if (p == por * CH_B || p == por * CH_Q)
          return true;
        break;
      }
      nr += dg[d][0];
      nc += dg[d][1];
    }
  }
  for (int d = 0; d < 4; d++) // rectas: torre o dama
  {
    int nr = r + rt[d][0], nc = c + rt[d][1];
    while (nr >= 0 && nr < 8 && nc >= 0 && nc < 8)
    {
      int p = g.sq[nr * 8 + nc];
      if (p)
      {
        if (p == por * CH_R || p == por * CH_Q)
          return true;
        break;
      }
      nr += rt[d][0];
      nc += rt[d][1];
    }
  }
  return false;
}

static inline void chAdd(ChMove *l, int &n, int f, int t, int promo, int flag)
{
  if (n < CH_MAXMOV)
  {
    l[n].from = f;
    l[n].to = t;
    l[n].promo = promo;
    l[n].flag = flag;
    n++;
  }
}

static void chPromos(ChMove *l, int &n, int f, int t, int flag, bool soloCap)
{
  static const int8_t pz[4] = {CH_Q, CH_N, CH_R, CH_B};
  for (int k = 0; k < (soloCap ? 1 : 4); k++)
    chAdd(l, n, f, t, pz[k], flag);
}

// Movimientos pseudo-legales (aun puede quedar el rey en jaque). soloCap = solo capturas y coronaciones
static int chGen(const ChGame &g, ChMove *l, bool soloCap)
{
  static const int8_t kn[8][2] = {{-2, -1}, {-2, 1}, {-1, -2}, {-1, 2}, {1, -2}, {1, 2}, {2, -1}, {2, 1}};
  static const int8_t kg[8][2] = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}};
  static const int8_t sl[8][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}, {-1, -1}, {-1, 1}, {1, -1}, {1, 1}}; // 0..3 recto, 4..7 diagonal
  int n = 0;
  const int s = g.side;

  for (int i = 0; i < 64; i++)
  {
    int p = g.sq[i] * s;
    if (p <= 0)
      continue;
    int r = i >> 3, c = i & 7;

    if (p == CH_P)
    {
      int dr = (s == 1) ? -1 : 1;
      int r1 = r + dr;
      int promoRow = (s == 1) ? 0 : 7;
      int startRow = (s == 1) ? 6 : 1;
      for (int dc = -1; dc <= 1; dc += 2)
      {
        int c1 = c + dc;
        if (c1 < 0 || c1 > 7)
          continue;
        int to = r1 * 8 + c1;
        if (g.sq[to] * s < 0)
        {
          if (r1 == promoRow)
            chPromos(l, n, i, to, CHF_CAP, soloCap);
          else
            chAdd(l, n, i, to, 0, CHF_CAP);
        }
        else if (to == g.ep)
          chAdd(l, n, i, to, 0, CHF_CAP | CHF_EP);
      }
      int to = r1 * 8 + c;
      if (g.sq[to] == 0)
      {
        if (r1 == promoRow)
          chPromos(l, n, i, to, 0, soloCap);
        else if (!soloCap)
        {
          chAdd(l, n, i, to, 0, 0);
          if (r == startRow && g.sq[to + dr * 8] == 0)
            chAdd(l, n, i, to + dr * 8, 0, CHF_DOBLE);
        }
      }
    }
    else if (p == CH_N || p == CH_K)
    {
      const int8_t (*d)[2] = (p == CH_N) ? kn : kg;
      for (int k = 0; k < 8; k++)
      {
        int nr = r + d[k][0], nc = c + d[k][1];
        if (nr < 0 || nr > 7 || nc < 0 || nc > 7)
          continue;
        int to = nr * 8 + nc;
        int t = g.sq[to] * s;
        if (t > 0)
          continue;
        if (t < 0)
          chAdd(l, n, i, to, 0, CHF_CAP);
        else if (!soloCap)
          chAdd(l, n, i, to, 0, 0);
      }
      if (p == CH_K && !soloCap)
      {
        // enroques: casillas libres y que el rey no salga, pase ni caiga en jaque
        if (s == 1 && i == 60)
        {
          if ((g.castle & 1) && !g.sq[61] && !g.sq[62] && g.sq[63] == CH_R &&
              !chAtacada(g, 60, -1) && !chAtacada(g, 61, -1) && !chAtacada(g, 62, -1))
            chAdd(l, n, 60, 62, 0, CHF_ENR);
          if ((g.castle & 2) && !g.sq[59] && !g.sq[58] && !g.sq[57] && g.sq[56] == CH_R &&
              !chAtacada(g, 60, -1) && !chAtacada(g, 59, -1) && !chAtacada(g, 58, -1))
            chAdd(l, n, 60, 58, 0, CHF_ENR);
        }
        else if (s == -1 && i == 4)
        {
          if ((g.castle & 4) && !g.sq[5] && !g.sq[6] && g.sq[7] == -CH_R &&
              !chAtacada(g, 4, 1) && !chAtacada(g, 5, 1) && !chAtacada(g, 6, 1))
            chAdd(l, n, 4, 6, 0, CHF_ENR);
          if ((g.castle & 8) && !g.sq[3] && !g.sq[2] && !g.sq[1] && g.sq[0] == -CH_R &&
              !chAtacada(g, 4, 1) && !chAtacada(g, 3, 1) && !chAtacada(g, 2, 1))
            chAdd(l, n, 4, 2, 0, CHF_ENR);
        }
      }
    }
    else // alfil, torre, dama
    {
      int d0 = (p == CH_B) ? 4 : 0;
      int d1 = (p == CH_R) ? 4 : 8;
      for (int k = d0; k < d1; k++)
      {
        int nr = r + sl[k][0], nc = c + sl[k][1];
        while (nr >= 0 && nr < 8 && nc >= 0 && nc < 8)
        {
          int to = nr * 8 + nc;
          int t = g.sq[to] * s;
          if (t > 0)
            break;
          if (t < 0)
          {
            chAdd(l, n, i, to, 0, CHF_CAP);
            break;
          }
          if (!soloCap)
            chAdd(l, n, i, to, 0, 0);
          nr += sl[k][0];
          nc += sl[k][1];
        }
      }
    }
  }
  return n;
}

static void chMake(ChGame &g, const ChMove &m, ChUndo &u)
{
  const int s = g.side;
  int p = g.sq[m.from];
  u.cap = g.sq[m.to];
  u.castle = g.castle;
  u.ep = g.ep;
  u.half = g.half;
  u.key = g.key;
  uint64_t k = g.key ^ chZ[p + 6][m.from];
  if (u.cap)
    k ^= chZ[u.cap + 6][m.to];

  g.sq[m.from] = 0;
  if (m.flag & CHF_EP)
  {
    int idx = m.to + (s == 1 ? 8 : -8); // el peon capturado esta detras de la casilla de destino
    u.cap = g.sq[idx];
    k ^= chZ[u.cap + 6][idx];
    g.sq[idx] = 0;
  }
  int np = m.promo ? s * m.promo : p;
  g.sq[m.to] = np;
  k ^= chZ[np + 6][m.to];
  if (m.flag & CHF_ENR)
  {
    int rf, rt;
    if (m.to > m.from)
    {
      rf = m.from + 3;
      rt = m.from + 1;
    }
    else
    {
      rf = m.from - 4;
      rt = m.from - 1;
    }
    int rook = g.sq[rf];
    g.sq[rt] = rook;
    g.sq[rf] = 0;
    k ^= chZ[rook + 6][rf] ^ chZ[rook + 6][rt];
  }
  k ^= chZc[g.castle & 15];
  g.castle &= chCastleMask(m.from) & chCastleMask(m.to);
  k ^= chZc[g.castle & 15];
  if (g.ep >= 0)
    k ^= chZe[g.ep & 7];
  g.ep = (m.flag & CHF_DOBLE) ? (m.from + m.to) / 2 : -1;
  if (g.ep >= 0)
    k ^= chZe[g.ep & 7];
  if (p * s == CH_K)
    g.kp[CH_KI(s)] = m.to;
  g.half = ((p * s == CH_P) || (m.flag & CHF_CAP)) ? 0 : (g.half < 250 ? g.half + 1 : 250);
  g.side = -s;
  g.key = k ^ chZs;
}

static void chUnmake(ChGame &g, const ChMove &m, const ChUndo &u)
{
  g.side = -g.side;
  const int s = g.side;
  int p = m.promo ? s * CH_P : g.sq[m.to];
  g.sq[m.from] = p;
  if (m.flag & CHF_EP)
  {
    g.sq[m.to] = 0;
    g.sq[m.to + (s == 1 ? 8 : -8)] = u.cap;
  }
  else
    g.sq[m.to] = u.cap;
  if (m.flag & CHF_ENR)
  {
    if (m.to > m.from)
    {
      g.sq[m.from + 3] = g.sq[m.from + 1];
      g.sq[m.from + 1] = 0;
    }
    else
    {
      g.sq[m.from - 4] = g.sq[m.from - 1];
      g.sq[m.from - 1] = 0;
    }
  }
  g.castle = u.castle;
  g.ep = u.ep;
  g.half = u.half;
  g.key = u.key;
  if (p * s == CH_K)
    g.kp[CH_KI(s)] = m.from;
}

// Jugada nula (pasar turno) para la poda del mismo nombre
static void chMakeNull(ChGame &g, ChUndo &u)
{
  u.ep = g.ep;
  u.key = g.key;
  if (g.ep >= 0)
    g.key ^= chZe[g.ep & 7];
  g.ep = -1;
  g.key ^= chZs;
  g.side = -g.side;
}

static void chUnmakeNull(ChGame &g, const ChUndo &u)
{
  g.side = -g.side;
  g.ep = u.ep;
  g.key = u.key;
}

static inline bool chReyEnJaque(const ChGame &g, int bando)
{
  return chAtacada(g, g.kp[CH_KI(bando)], -bando);
}

// Rellena l con los movimientos legales y devuelve cuantos hay
static int chLegales(ChGame &g, ChMove *l)
{
  ChMove tmp[CH_MAXMOV];
  int n = chGen(g, tmp, false), k = 0;
  const int s = g.side;
  for (int i = 0; i < n; i++)
  {
    ChUndo u;
    chMake(g, tmp[i], u);
    bool ok = !chReyEnJaque(g, s);
    chUnmake(g, tmp[i], u);
    if (ok)
      l[k++] = tmp[i];
  }
  return k;
}

// Casillas a las que llega una pieza deslizante por una direccion (propias no, capturas si)
static inline int chRayo(const ChGame &g, int i, int dr, int dc, int col)
{
  int r = (i >> 3) + dr, c = (i & 7) + dc, n = 0;
  while (r >= 0 && r < 8 && c >= 0 && c < 8)
  {
    int p = g.sq[r * 8 + c];
    if (p == 0)
      n++;
    else
    {
      if ((p > 0) != (col == 0))
        n++;
      break;
    }
    r += dr;
    c += dc;
  }
  return n;
}

// Evaluacion en centipeones desde el punto de vista de quien mueve.
// Material + casillas + movilidad + estructura de peones (doblados, aislados, pasados) + torres en columnas
// abiertas y en septima + pareja de alfiles + seguridad del rey (escudo de peones) + final de partida
// (mezcla suave entre medio juego y final segun el material que queda) + mates con rey y torre/dama.
static int chEval(const ChGame &g)
{
  int mg[2] = {0, 0}, eg[2] = {0, 0}, mat[2] = {0, 0}, npm[2] = {0, 0}, nb[2] = {0, 0};
  int fase = 0, peones = 0;
  int8_t pf[2][8], minR[2][8], maxR[2][8];
  for (int c = 0; c < 8; c++)
  {
    pf[0][c] = pf[1][c] = 0;
    minR[0][c] = minR[1][c] = 8;
    maxR[0][c] = maxR[1][c] = -1;
  }
  for (int i = 0; i < 64; i++)
  {
    int v = g.sq[i];
    if (v == CH_P || v == -CH_P)
    {
      int col = v > 0 ? 0 : 1, f = i & 7, r = i >> 3;
      pf[col][f]++;
      peones++;
      if (r < minR[col][f])
        minR[col][f] = r;
      if (r > maxR[col][f])
        maxR[col][f] = r;
    }
  }

  static const int8_t dg[4][2] = {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}};
  static const int8_t rt[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
  for (int i = 0; i < 64; i++)
  {
    int v = g.sq[i];
    if (!v)
      continue;
    int t = v < 0 ? -v : v;
    if (t == CH_K)
      continue;
    int col = v > 0 ? 0 : 1;
    int si = col ? (i ^ 56) : i;
    int f = i & 7, r = i >> 3;
    mat[col] += chVal[t];
    mg[col] += chVal[t] + chPst[t][si];
    eg[col] += chValEg[t] + (t == CH_P ? chPstPEg[si] : chPst[t][si]);
    if (t == CH_P)
    {
      int rel = col == 0 ? 7 - r : r;
      if ((f == 0 || !pf[col][f - 1]) && (f == 7 || !pf[col][f + 1]))
      {
        mg[col] -= 12;
        eg[col] -= 18;
      }
      bool pasado = true;
      for (int ff = f - 1; ff <= f + 1 && pasado; ff++)
      {
        if (ff < 0 || ff > 7)
          continue;
        if (col == 0 ? (minR[1][ff] < r) : (maxR[0][ff] > r))
          pasado = false;
      }
      if (pasado)
      {
        mg[col] += chPasMg[rel];
        eg[col] += chPasEg[rel];
      }
      continue;
    }
    npm[col] += chVal[t];
    if (t == CH_N)
    {
      fase += 1;
      int n = 0;
      for (const int8_t *q = chKn[i]; *q >= 0; q++)
      {
        int p = g.sq[*q];
        if (p == 0 || (p > 0) != (col == 0))
          n++;
      }
      mg[col] += 4 * (n - 4);
      eg[col] += 4 * (n - 4);
    }
    else if (t == CH_B)
    {
      fase += 1;
      nb[col]++;
      int n = 0;
      for (int d = 0; d < 4; d++)
        n += chRayo(g, i, dg[d][0], dg[d][1], col);
      mg[col] += 4 * (n - 6);
      eg[col] += 5 * (n - 6);
    }
    else if (t == CH_R)
    {
      fase += 2;
      int n = 0;
      for (int d = 0; d < 4; d++)
        n += chRayo(g, i, rt[d][0], rt[d][1], col);
      mg[col] += 2 * (n - 7);
      eg[col] += 4 * (n - 7);
      if (!pf[col][f])
      {
        if (!pf[1 - col][f])
        {
          mg[col] += 20;
          eg[col] += 10;
        }
        else
        {
          mg[col] += 10;
          eg[col] += 5;
        }
      }
      if (col == 0 ? r == 1 : r == 6)
      {
        mg[col] += 10;
        eg[col] += 20;
      }
    }
    else // dama
    {
      fase += 4;
      int n = 0;
      for (int d = 0; d < 4; d++)
        n += chRayo(g, i, dg[d][0], dg[d][1], col) + chRayo(g, i, rt[d][0], rt[d][1], col);
      mg[col] += 1 * (n - 14);
      eg[col] += 2 * (n - 14);
    }
  }
  if (peones == 0 && npm[0] <= 330 && npm[1] <= 330)
    return 0; // rey solo, rey y una pieza menor...: nadie puede ganar

  for (int col = 0; col < 2; col++)
  {
    for (int f = 0; f < 8; f++)
      if (pf[col][f] > 1)
      {
        mg[col] -= (pf[col][f] - 1) * 10;
        eg[col] -= (pf[col][f] - 1) * 18;
      }
    if (nb[col] >= 2)
    {
      mg[col] += 30;
      eg[col] += 45;
    }
    int k = g.kp[col], kr = k >> 3, kc = k & 7;
    if ((col == 0 ? 7 - kr : kr) <= 1) // rey en sus dos primeras filas: contar el escudo de peones
    {
      int dir = col == 0 ? -1 : 1, files = 0, found = 0;
      for (int ff = kc - 1; ff <= kc + 1; ff++)
      {
        if (ff < 0 || ff > 7)
          continue;
        files++;
        for (int d = 1; d <= 2; d++)
        {
          int rr = kr + dir * d;
          if (rr < 0 || rr > 7)
            break;
          if (g.sq[rr * 8 + ff] == (col == 0 ? CH_P : -CH_P))
          {
            found++;
            break;
          }
        }
      }
      mg[col] -= (files - found) * 12;
      if (!pf[col][kc])
        mg[col] -= 10;
    }
    int ks = col ? (g.kp[col] ^ 56) : g.kp[col];
    mg[col] += chPst[6][ks];
    eg[col] += chPst[7][ks];
  }
  if (fase > 24)
    fase = 24;
  int score = ((mg[0] - mg[1]) * fase + (eg[0] - eg[1]) * (24 - fase)) / 24;

  // Final con ventaja clara: empujar al rey contrario a una esquina y acercar el nuestro (si no, no sabe dar mate)
  int dif = mat[0] - mat[1];
  if (npm[0] + npm[1] <= 2600 && (dif >= 250 || dif <= -250))
  {
    int w = dif > 0 ? 0 : 1;
    int lk = g.kp[1 - w], wk = g.kp[w];
    int lr = lk >> 3, lc = lk & 7;
    int cd = (lr < 4 ? 3 - lr : lr - 4) + (lc < 4 ? 3 - lc : lc - 4);
    int dr = (lk >> 3) - (wk >> 3), dc = (lk & 7) - (wk & 7);
    int dist = (dr < 0 ? -dr : dr) + (dc < 0 ? -dc : dc);
    int b = cd * 10 + (14 - dist) * 4;
    score += (w == 0) ? b : -b;
  }
  return score * g.side + 8; // +8 por tener el turno
}

static uint32_t chNodes;
static uint32_t chDeadline;
static bool chAbort;
static const uint32_t *chHistG; // posiciones ya jugadas en la partida
static int chNHistG;

// Tabla de transposicion: recuerda lo ya calculado de cada posicion (8 bytes por entrada)
struct ChTT
{
  uint16_t k;  // 16 bits altos de la clave
  uint16_t mv; // mejor jugada empaquetada
  int16_t sc;
  int8_t d;
  uint8_t fg; // bits 0-1: 1 exacto, 2 cota inferior, 3 cota superior; bits 2-7: generacion
};
static ChTT *chTT = NULL;
static uint32_t chTTmask = 0;
static uint8_t chGen8 = 0;
static bool chTTtry = false;

static void chTTInit()
{
  if (chTTtry)
    return;
  chTTtry = true;
  size_t sz = sizeof(ChTT) * ((size_t)1 << CH_TT_BITS);
  if ((size_t)ESP.getMaxAllocHeap() >= sz + CH_TT_RESERVA)
  {
    chTT = (ChTT *)malloc(sz);
    if (chTT)
    {
      memset(chTT, 0, sz);
      chTTmask = ((uint32_t)1 << CH_TT_BITS) - 1;
    }
  }
}

// Reserva / libera toda la RAM del motor (llamar al entrar y al salir de la pantalla)
static bool chMemAlloc()
{
  if (chMem)
    return true;
  chMem = (ChMem *)calloc(1, sizeof(ChMem));
  chZok = false; // se recalculan las tablas (semilla fija: mismas claves siempre)
  return chMem != NULL;
}

static void chMemLibera()
{
  if (chTT)
  {
    free(chTT);
    chTT = NULL;
    chTTmask = 0;
  }
  chTTtry = false;
  if (chMem)
  {
    free(chMem);
    chMem = NULL;
  }
  chZok = false;
}

// Llamar al empezar cada partida
static void chNuevoJuego()
{
  chZInit();
  chTTInit();
  if (chTT)
    memset(chTT, 0, sizeof(ChTT) * ((size_t)1 << CH_TT_BITS));
  memset(chKill, 0, sizeof(chKill));
  memset(chHist, 0, sizeof(chHist));
  chGen8 = 0;
}

static inline uint16_t chPack(const ChMove &m) { return (uint16_t)(m.from | (m.to << 6) | (m.promo << 12)); }

static const int16_t chFutM[3] = {0, 140, 280};

// Ordena: jugada de la tabla, capturas buenas (victima valiosa, atacante barato), coronaciones, asesinas, historial
static void chPuntua(const ChGame &g, const ChMove *mv, int16_t *sc, int n, int ply, uint16_t ttMove)
{
  for (int i = 0; i < n; i++)
  {
    uint16_t pk = chPack(mv[i]);
    int v;
    int at = g.sq[mv[i].from] < 0 ? -g.sq[mv[i].from] : g.sq[mv[i].from];
    if (pk == ttMove)
      v = 32000;
    else if (mv[i].flag & CHF_CAP)
    {
      int cap = (mv[i].flag & CHF_EP) ? CH_P : (g.sq[mv[i].to] < 0 ? -g.sq[mv[i].to] : g.sq[mv[i].to]);
      v = 20000 + chVal[cap] * 10 - chVal[at] + 100;
      if (mv[i].promo == CH_Q)
        v += 8000;
    }
    else if (mv[i].promo)
      v = (mv[i].promo == CH_Q) ? 28000 : 100;
    else if (pk == chKill[ply][0])
      v = 15000;
    else if (pk == chKill[ply][1])
      v = 14500;
    else
      v = chHist[at][mv[i].to];
    sc[i] = (int16_t)(v > 31999 ? 31999 : v);
  }
}

static inline int chSiguiente(ChMove *mv, int16_t *sc, int n, int desde)
{
  int b = desde;
  for (int j = desde + 1; j < n; j++)
    if (sc[j] > sc[b])
      b = j;
  if (b != desde)
  {
    ChMove t = mv[b];
    mv[b] = mv[desde];
    mv[desde] = t;
    int16_t ts = sc[b];
    sc[b] = sc[desde];
    sc[desde] = ts;
  }
  return desde;
}

static inline bool chHayPiezas(const ChGame &g, int s)
{
  for (int i = 0; i < 64; i++)
  {
    int p = g.sq[i] * s;
    if (p >= CH_N && p <= CH_Q)
      return true;
  }
  return false;
}

// Busqueda de capturas: sigue hasta que la posicion se calma, para no evaluar en medio de un cambio de piezas
static int chQ(ChGame &g, int alpha, int beta, int ply)
{
  if ((++chNodes & 255) == 0)
  {
    if ((int32_t)(millis() - chDeadline) > 0)
      chAbort = true;
    if ((chNodes & 2047) == 0)
      CH_CEDE();
  }
  if (chAbort)
    return 0;
  int stand = chEval(g);
  if (ply >= CH_MAXPLY - 1)
    return stand;
  if (stand >= beta)
    return stand;
  if (stand > alpha)
    alpha = stand;
  ChMove *mv = chBuf[ply];
  int16_t *sc = chSc[ply];
  int n = chGen(g, mv, true);
  chPuntua(g, mv, sc, n, ply, 0);
  const int s = g.side;
  int best = stand;
  for (int i = 0; i < n; i++)
  {
    chSiguiente(mv, sc, n, i);
    if (!mv[i].promo) // poda delta: ni capturando esa pieza se llega a alpha
    {
      int cap = (mv[i].flag & CHF_EP) ? CH_P : (g.sq[mv[i].to] < 0 ? -g.sq[mv[i].to] : g.sq[mv[i].to]);
      if (stand + chVal[cap] + 200 < alpha)
        continue;
    }
    ChUndo u;
    chMake(g, mv[i], u);
    if (chReyEnJaque(g, s))
    {
      chUnmake(g, mv[i], u);
      continue;
    }
    int v = -chQ(g, -beta, -alpha, ply + 1);
    chUnmake(g, mv[i], u);
    if (chAbort)
      return 0;
    if (v > best)
    {
      best = v;
      if (v > alpha)
      {
        alpha = v;
        if (alpha >= beta)
          break;
      }
    }
  }
  return best;
}

// Busqueda principal: negamax con poda alfa-beta, variante principal (PVS), tabla de transposicion, jugada nula,
// reducciones de jugadas tardias (LMR), poda por futilidad, extension por jaque, asesinas e historial.
static int chNeg(ChGame &g, int depth, int alpha, int beta, int ply, bool nulaOK)
{
  if ((++chNodes & 255) == 0)
  {
    if ((int32_t)(millis() - chDeadline) > 0)
      chAbort = true;
    if ((chNodes & 2047) == 0)
      CH_CEDE();
  }
  if (chAbort)
    return 0;
  const int s = g.side;
  const bool pv = (beta - alpha) > 1;
  chPath[ply] = g.key;

  if (ply > 0)
  {
    if (g.half >= 100)
      return (ply & 1) ? CH_CONTEMPT : -CH_CONTEMPT;
    if (g.half >= 4) // repeticion: dentro de la busqueda o con posiciones ya jugadas en la partida
    {
      for (int j = ply - 2; j >= 0 && (ply - j) <= g.half; j -= 2)
        if (chPath[j] == g.key)
          return (ply & 1) ? CH_CONTEMPT : -CH_CONTEMPT;
      uint32_t fk = (uint32_t)(g.key ^ (g.key >> 32));
      for (int j = (ply & 1); j < chNHistG && ply + j <= g.half; j += 2)
        if (chHistG[chNHistG - 1 - j] == fk)
          return (ply & 1) ? CH_CONTEMPT : -CH_CONTEMPT;
    }
  }

  const bool jaque = chReyEnJaque(g, s);
  if (jaque && ply < CH_MAXPLY - 4)
    depth++;
  if (depth <= 0 || ply >= CH_MAXPLY - 1)
    return chQ(g, alpha, beta, ply);

  uint16_t ttMove = 0;
  if (chTT)
  {
    const ChTT *e = &chTT[(uint32_t)g.key & chTTmask];
    if (e->k == (uint16_t)(g.key >> 48))
    {
      ttMove = e->mv;
      if (!pv && e->d >= depth)
      {
        int v = e->sc;
        if (v > CH_MATE - 200)
          v -= ply;
        else if (v < -(CH_MATE - 200))
          v += ply;
        int f = e->fg & 3;
        if (f == 1 || (f == 2 && v >= beta) || (f == 3 && v <= alpha))
          return v;
      }
    }
  }

  int se = 0;
  if (!pv && !jaque)
  {
    se = chEval(g);
    if (depth <= 3 && se - 110 * depth >= beta && beta > -(CH_MATE - 200) && beta < CH_MATE - 200)
      return se; // tan por encima de beta que no hace falta mirar
    if (nulaOK && depth >= 3 && se >= beta && chHayPiezas(g, s))
    {
      ChUndo nu;
      chMakeNull(g, nu);
      int v = -chNeg(g, depth - 3 - (depth >= 6 ? 1 : 0), -beta, -beta + 1, ply + 1, false);
      chUnmakeNull(g, nu);
      if (chAbort)
        return 0;
      if (v >= beta)
        return v >= CH_MATE - 200 ? beta : v;
    }
  }

  ChMove *mv = chBuf[ply];
  int16_t *sc = chSc[ply];
  int n = chGen(g, mv, false);
  chPuntua(g, mv, sc, n, ply, ttMove);
  const int alpha0 = alpha;
  int best = -CH_INF, legal = 0;
  uint16_t bestMv = 0;
  for (int i = 0; i < n; i++)
  {
    chSiguiente(mv, sc, n, i);
    const ChMove m = mv[i];
    const bool tranquila = !(m.flag & CHF_CAP) && !m.promo;
    ChUndo u;
    chMake(g, m, u);
    if (chReyEnJaque(g, s))
    {
      chUnmake(g, m, u);
      continue;
    }
    legal++;
    int nd = depth - 1;
    if (legal > 1 && tranquila && !jaque)
    {
      if (!pv && depth <= 2 && se + chFutM[depth] <= alpha && alpha > -(CH_MATE - 200) && !chReyEnJaque(g, g.side))
      {
        chUnmake(g, m, u); // futilidad: una jugada tranquila no va a levantar la posicion hasta alpha
        continue;
      }
    }
    int v;
    if (legal == 1)
      v = -chNeg(g, nd, -beta, -alpha, ply + 1, true);
    else
    {
      int red = 0;
      if (depth >= 3 && tranquila && !jaque && legal >= 4 && chPack(m) != chKill[ply][0] && chPack(m) != chKill[ply][1])
      {
        red = 1;
        if (depth >= 6 && legal >= 9)
          red = 2;
        if (chReyEnJaque(g, g.side))
          red = 0;
      }
      v = -chNeg(g, nd - red, -alpha - 1, -alpha, ply + 1, true);
      if (v > alpha && red)
        v = -chNeg(g, nd, -alpha - 1, -alpha, ply + 1, true);
      if (v > alpha && v < beta)
        v = -chNeg(g, nd, -beta, -alpha, ply + 1, true);
    }
    chUnmake(g, m, u);
    if (chAbort)
      return 0;
    if (v > best)
    {
      best = v;
      bestMv = chPack(m);
      if (v > alpha)
      {
        alpha = v;
        if (alpha >= beta)
        {
          if (tranquila)
          {
            uint16_t pk = chPack(m);
            if (chKill[ply][0] != pk)
            {
              chKill[ply][1] = chKill[ply][0];
              chKill[ply][0] = pk;
            }
            int at = g.sq[m.from] < 0 ? -g.sq[m.from] : g.sq[m.from];
            int h = chHist[at][m.to] + depth * depth;
            if (h > 12000)
            {
              for (int a = 0; a < 7; a++)
                for (int b = 0; b < 64; b++)
                  chHist[a][b] >>= 1;
              h >>= 1;
            }
            chHist[at][m.to] = (int16_t)h;
          }
          break;
        }
      }
    }
  }
  if (!legal)
    return jaque ? -CH_MATE + ply : 0; // mate (cuanto mas cerca mejor) o ahogado

  if (chTT)
  {
    ChTT *e = &chTT[(uint32_t)g.key & chTTmask];
    uint16_t chk = (uint16_t)(g.key >> 48);
    if (e->k != chk || e->d <= depth || (e->fg >> 2) != chGen8)
    {
      int v = best;
      if (v > CH_MATE - 200)
        v += ply;
      else if (v < -(CH_MATE - 200))
        v -= ply;
      e->k = chk;
      e->mv = bestMv;
      e->sc = (int16_t)v;
      e->d = (int8_t)depth;
      e->fg = (uint8_t)((best <= alpha0 ? 3 : (best >= beta ? 2 : 1)) | (chGen8 << 2));
    }
  }
  return best;
}

// Clave de 32 bits de la posicion (para el historial de repeticiones de la partida)
static uint32_t chClave(const ChGame &g)
{
  return (uint32_t)(g.key ^ (g.key >> 32));
}

// Elige jugada: profundizacion iterativa con limite de tiempo. Entre las jugadas casi igual de buenas
// sortea una (mas margen en la apertura, casi nada en el final), asi no juegan siempre la misma partida.
static bool chElegir(ChGame &g, ChMove &out, int profMax, uint32_t ms, const uint32_t *hist, int nHist)
{
  ChMove root[CH_MAXMOV];
  int16_t rs[CH_MAXMOV], cur[CH_MAXMOV];
  chZInit();
  chTTInit();
  int n = chLegales(g, root);
  if (n == 0)
    return false;
  out = root[esp_random() % n];
  if (n == 1)
    return true;

  int piezas = 0;
  for (int i = 0; i < 64; i++)
    if (g.sq[i])
      piezas++;
#ifndef CH_M1
#define CH_M1 20 // margen (cp) para sortear entre jugadas casi iguales: apertura
#define CH_M2 12
#define CH_M3 6
#define CH_M4 3 // final
#endif
  const int margen = piezas >= 30 ? CH_M1 : (piezas >= 26 ? CH_M2 : (piezas >= 20 ? CH_M3 : CH_M4));

  chHistG = hist;
  chNHistG = nHist;
  chPath[0] = g.key;
  chGen8 = (chGen8 + 1) & 63;
  memset(chKill, 0, sizeof(chKill));
  for (int a = 0; a < 7; a++)
    for (int b = 0; b < 64; b++)
      chHist[a][b] >>= 1;

  const uint32_t t0 = millis();
  chAbort = false;
  chNodes = 0;
  chPuntua(g, root, rs, n, 0, 0);

  for (int d = 1; d <= profMax; d++)
  {
    chDeadline = (d == 1) ? t0 + 600000u : t0 + ms; // la primera vuelta siempre se completa
    for (int i = 1; i < n; i++)                     // la mejor de la vuelta anterior, primero
    {
      ChMove m = root[i];
      int16_t v = rs[i];
      int j = i - 1;
      while (j >= 0 && rs[j] < v)
      {
        root[j + 1] = root[j];
        rs[j + 1] = rs[j];
        j--;
      }
      root[j + 1] = m;
      rs[j + 1] = v;
    }
    int best = -CH_INF;
    bool completa = true;
    for (int i = 0; i < n; i++)
    {
      ChUndo u;
      chMake(g, root[i], u);
      int alpha = (best == -CH_INF) ? -CH_INF : best - margen;
      int v = -chNeg(g, d - 1, -CH_INF, -alpha, 1, true);
      chUnmake(g, root[i], u);
      if (chAbort)
      {
        completa = false;
        break;
      }
      cur[i] = (int16_t)v;
      if (v > best)
        best = v;
    }
    if (!completa)
      break;
    int cand[CH_MAXMOV], nc = 0;
    for (int i = 0; i < n; i++)
      if (cur[i] > best - margen)
        cand[nc++] = i;
    out = root[cand[esp_random() % nc]];
    for (int i = 0; i < n; i++)
      rs[i] = cur[i];
    if (best >= CH_MATE - 50 || best <= -CH_MATE + 50)
      break;
    if ((uint32_t)(millis() - t0) * 5 > ms * 2) // ya gastado el 40 %: otra vuelta mas no cabe
      break;
  }
  return true;
}

// Notacion algebraica (Cf3, exd5, O-O, e8=D#...). Hay que llamarla ANTES de hacer la jugada
static void chSAN(ChGame &g, const ChMove &m, char *b)
{
  static const char letras[7] = {' ', 'P', 'C', 'A', 'T', 'D', 'R'};
  int s = g.side;
  int p = g.sq[m.from] * s;
  int k = 0;
  if (m.flag & CHF_ENR)
  {
    const char *t = (m.to > m.from) ? "O-O" : "O-O-O";
    while (*t)
      b[k++] = *t++;
  }
  else
  {
    if (p == CH_P)
    {
      if (m.flag & CHF_CAP)
      {
        b[k++] = 'a' + (m.from & 7);
        b[k++] = 'x';
      }
    }
    else
    {
      b[k++] = letras[p];
      ChMove l[CH_MAXMOV];
      int n = chLegales(g, l);
      bool otra = false, mismaCol = false, mismaFila = false;
      for (int i = 0; i < n; i++)
        if (l[i].to == m.to && l[i].from != m.from && g.sq[l[i].from] == g.sq[m.from])
        {
          otra = true;
          if ((l[i].from & 7) == (m.from & 7))
            mismaCol = true;
          if ((l[i].from >> 3) == (m.from >> 3))
            mismaFila = true;
        }
      if (otra)
      {
        if (!mismaCol)
          b[k++] = 'a' + (m.from & 7);
        else if (!mismaFila)
          b[k++] = '8' - (m.from >> 3);
        else
        {
          b[k++] = 'a' + (m.from & 7);
          b[k++] = '8' - (m.from >> 3);
        }
      }
      if (m.flag & CHF_CAP)
        b[k++] = 'x';
    }
    b[k++] = 'a' + (m.to & 7);
    b[k++] = '8' - (m.to >> 3);
    if (m.promo)
    {
      b[k++] = '=';
      b[k++] = letras[m.promo];
    }
  }
  ChUndo u;
  chMake(g, m, u);
  if (chReyEnJaque(g, g.side))
  {
    ChMove l[CH_MAXMOV];
    b[k++] = chLegales(g, l) ? '+' : '#';
  }
  chUnmake(g, m, u);
  b[k] = 0;
}

// Rey solo, rey y un caballo/alfil, o solo alfiles del mismo color: nadie puede dar mate
static bool chMaterialInsuf(const ChGame &g)
{
  int menores = 0, caballos = 0;
  bool claro = false, oscuro = false;
  for (int i = 0; i < 64; i++)
  {
    int p = g.sq[i] < 0 ? -g.sq[i] : g.sq[i];
    if (p == CH_P || p == CH_R || p == CH_Q)
      return false;
    if (p == CH_N)
    {
      menores++;
      caballos++;
    }
    if (p == CH_B)
    {
      menores++;
      if (((i >> 3) + (i & 7)) & 1)
        oscuro = true;
      else
        claro = true;
    }
  }
  if (menores <= 1)
    return true;
  return caballos == 0 && !(claro && oscuro);
}
// <<< MOTOR

// ---------------------------------------------------------------------------------------------------------
//  PANTALLA
// ---------------------------------------------------------------------------------------------------------

#define CH_BX 104         // x del tablero
#define CH_BY 34          // y del tablero (casillas de 14x14 -> 112 px)
#define CH_SQ 14          // lado de la casilla
#define CH_SLICE_MS 1000  // tandas de 1 s: el driver puede cambiar de pantalla entre llamadas
#define CH_PROF 10        // profundidad máxima de búsqueda (manda el tiempo)
#define CH_TIEMPO_MS 1500 // tiempo máximo que piensa cada CPU por jugada
#define CH_PAUSA_MS 650   // pausa entre jugadas para que dé tiempo a verlas
#define CH_FIN_MS 7000    // cuánto se queda el tablero final antes de empezar otra

#define CH_C_CLARA 0xF6D6
#define CH_C_OSCURA 0xB44C
#define CH_C_CLARA_HL 0xCE8D
#define CH_C_OSCURA_HL 0xACC7
#define CH_C_JAQUE 0xD9E7
#define CH_GRIS 0xBDF7
#define CH_VERDE 0x07E0
#define CH_ROJO 0xF800
#define CH_AMARILLO 0xFFE0

// Dibujo de las piezas en 12x12 (X = pieza). Se pintan con borde para que se vean sobre cualquier casilla
static const char *const CH_SPR[7][12] = {
    {""},
    {"............", ".....XX.....", "....XXXX....", "....XXXX....", ".....XX.....", "....XXXX....", ".....XX.....", ".....XX.....", "....XXXX....", "...XXXXXX...", "..XXXXXXXX..", "............"},
    {"...XX.......", "..XXXXX.....", ".XXXXXXX....", ".XXXXXXXX...", "XXX.XXXXX...", "XX..XXXXX...", ".....XXXX...", "....XXXXX...", "...XXXXXX...", "..XXXXXXXX..", ".XXXXXXXXXX.", "............"},
    {".....XX.....", "....XXXX....", "...XXX.XX...", "...XXXXXX...", "....XXXX....", ".....XX.....", "....XXXX....", ".....XX.....", "....XXXX....", "..XXXXXXXX..", ".XXXXXXXXXX.", "............"},
    {"............", "..XX.XX.XX..", "..XXXXXXXX..", "...XXXXXX...", "...XXXXXX...", "...XXXXXX...", "...XXXXXX...", "...XXXXXX...", "..XXXXXXXX..", "..XXXXXXXX..", ".XXXXXXXXXX.", "............"},
    {"X..X.XX.X..X", "XX.XXXXXX.XX", "XXXXXXXXXXXX", ".XXXXXXXXXX.", "..XXXXXXXX..", "...XXXXXX...", "...XXXXXX...", "....XXXX....", "...XXXXXX...", "..XXXXXXXX..", ".XXXXXXXXXX.", "............"},
    {".....XX.....", "....XXXX....", ".....XX.....", "..XX.XX.XX..", ".XXXXXXXXXX.", ".XXXXXXXXXX.", "..XXXXXXXX..", "...XXXXXX...", "...XXXXXX...", "..XXXXXXXX..", ".XXXXXXXXXX.", "............"}};

struct ChPartida
{
  bool init;
  ChGame g;
  uint32_t hist[104];
  int nHist;
  int full;          // número de jugada (1, 2, 3...)
  int8_t cpuBlancas; // 1 o 2: qué CPU lleva las blancas en esta partida
  int estado;        // 0 jugando, 1 mate, 2 ahogado, 3 repetición, 4 regla 50, 5 material, 6 límite
  int ganador;       // 0 nadie (tablas), 1 CPU-1, 2 CPU-2
  int score1, score2, tablas;
  bool hayUlt, enJaque;
  ChMove ult;
  int matDif; // material blancas - negras (en peones)
  unsigned long siguiente, finAt, ultLlamada;
  char log[2][6][16];
  int16_t shown[64];
};
static ChPartida ch = {false};

static bool chM(int p, int x, int y)
{
  if (x < 0 || x > 11 || y < 0 || y > 11)
    return false;
  return CH_SPR[p][y][x] == 'X';
}

static void chPintaCasilla(int i, int pz, bool hl, bool jaque)
{
  uint16_t buf[CH_SQ * CH_SQ];
  int r = i >> 3, c = i & 7;
  uint16_t base = ((r + c) & 1) ? (hl ? CH_C_OSCURA_HL : CH_C_OSCURA) : (hl ? CH_C_CLARA_HL : CH_C_CLARA);
  if (jaque)
    base = CH_C_JAQUE;
  int t = pz < 0 ? -pz : pz;
  uint16_t relleno = pz > 0 ? 0xFFFF : 0x0000;
  uint16_t borde = pz > 0 ? 0x0000 : 0xFFFF;
  for (int y = 0; y < CH_SQ; y++)
    for (int x = 0; x < CH_SQ; x++)
    {
      uint16_t col = base;
      if (t)
      {
        if (chM(t, x - 1, y - 1))
          col = relleno;
        else if (chM(t, x - 2, y - 1) || chM(t, x, y - 1) || chM(t, x - 1, y - 2) || chM(t, x - 1, y))
          col = borde;
      }
      buf[y * CH_SQ + x] = col;
    }
  tft.startWrite();
  tft.setAddrWindow(CH_BX + c * CH_SQ, CH_BY + r * CH_SQ, CH_SQ, CH_SQ);
  tft.pushColors(buf, CH_SQ * CH_SQ, true);
  tft.endWrite();
}

// Solo repinta las casillas que han cambiado (sin parpadeo)
static void chPintaTablero(bool forzar)
{
  for (int i = 0; i < 64; i++)
  {
    int pz = ch.g.sq[i];
    bool hl = ch.hayUlt && (i == ch.ult.from || i == ch.ult.to);
    bool jaque = ch.enJaque && ch.estado != 2 && (pz == ((ch.g.side == 1) ? CH_K : -CH_K));
    int16_t key = (pz + 8) | (hl ? 16 : 0) | (jaque ? 32 : 0);
    if (forzar || key != ch.shown[i])
    {
      chPintaCasilla(i, pz, hl, jaque);
      ch.shown[i] = key;
    }
  }
}

static void chPintaMarcador()
{
  char b[16];
  tft.setFreeFont(NULL);
  tft.setTextFont(1);
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextPadding(144);
  tft.setTextDatum(TL_DATUM);
  snprintf(b, sizeof(b), "CPU-1 %05d", ch.score1);
  tft.drawString(b, 4, 153);
  tft.setTextDatum(TR_DATUM);
  snprintf(b, sizeof(b), "CPU-2 %05d", ch.score2);
  tft.drawString(b, 316, 153);
  // tablas, chiquito y en el centro
  tft.setTextSize(1);
  tft.setTextColor(CH_GRIS, TFT_BLACK);
  tft.setTextPadding(28);
  tft.setTextDatum(TC_DATUM);
  snprintf(b, sizeof(b), "=%d", ch.tablas);
  tft.drawString(b, 160, 157);
  tft.setTextPadding(0);
  tft.setTextDatum(TL_DATUM);
}

static void chPintaPanel(int cpu) // 1 = izquierda, 2 = derecha
{
  const int x = (cpu == 1) ? 4 : 222;
  const bool blancas = (ch.cpuBlancas == cpu);
  const int mover = (ch.g.side == 1) ? ch.cpuBlancas : 3 - ch.cpuBlancas;
  char b[24];
  const char *txt = "";
  uint16_t col = CH_GRIS;

  tft.setFreeFont(NULL);
  tft.setTextFont(1);
  tft.setTextDatum(TL_DATUM);
  tft.setTextPadding(94);

  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString(cpu == 1 ? "CPU-1" : "CPU-2", x, 36);

  tft.setTextSize(1);
  tft.setTextColor(CH_GRIS, TFT_BLACK);
  tft.drawString(blancas ? "BLANCAS" : "NEGRAS", x, 54);

  // linea de estado
  if (ch.estado == 0)
  {
    if (cpu == mover)
    {
      txt = ch.enJaque ? "EN JAQUE" : "Pensando...";
      col = ch.enJaque ? CH_ROJO : CH_AMARILLO;
    }
  }
  else if (ch.ganador == 0)
  {
    txt = "TABLAS";
  }
  else if (ch.ganador == cpu)
  {
    txt = "GANA";
    col = CH_VERDE;
  }
  else
  {
    txt = "PIERDE";
    col = CH_ROJO;
  }
  tft.setTextColor(col, TFT_BLACK);
  tft.drawString(txt, x, 64);

  // segunda linea: motivo del final o material
  tft.setTextColor(CH_GRIS, TFT_BLACK);
  if (ch.estado == 0)
  {
    int d = blancas ? ch.matDif : -ch.matDif;
    if (d == 0)
      snprintf(b, sizeof(b), "Mat: =");
    else
      snprintf(b, sizeof(b), "Mat: %+d", d);
  }
  else
  {
    static const char *const motivo[] = {"", "JAQUE MATE", "AHOGADO", "REPETICION", "REGLA 50 MOV", "SIN MATERIAL", "LIMITE JUGADAS"};
    snprintf(b, sizeof(b), "%s", motivo[ch.estado]);
  }
  tft.drawString(b, x, 74);

  // ultimas jugadas de esta CPU
  for (int i = 0; i < 6; i++)
    tft.drawString(ch.log[cpu - 1][i], x, 88 + i * 9);

  tft.setTextPadding(0);
  tft.setTextDatum(TL_DATUM);
}

static void chPintaTodo(bool limpiar)
{
  if (limpiar)
    tft.fillRect(0, 0, 320, 170, TFT_BLACK);
  tft.drawRect(CH_BX - 1, CH_BY - 1, CH_SQ * 8 + 2, CH_SQ * 8 + 2, CH_GRIS);
  memset(ch.shown, 0xFF, sizeof(ch.shown));
  chPintaTablero(true);
  chPintaPanel(1);
  chPintaPanel(2);
  chPintaMarcador();
}

// Hora arriba a la izquierda, hashrate arriba a la derecha
static void chPintaHud(const char *hash, const char *hora)
{
  tft.setFreeFont(NULL);
  tft.setTextFont(1);
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextPadding(100);
  tft.setTextDatum(TL_DATUM);
  tft.drawString(hora, 4, 9);
  char b[24];
  snprintf(b, sizeof(b), "%s KH/s", hash);
  tft.setTextPadding(150);
  tft.setTextDatum(TR_DATUM);
  tft.drawString(b, 316, 9);
  tft.setTextPadding(0);
  tft.setTextDatum(TL_DATUM);
}

static void chMaterial()
{
  int m = 0;
  for (int i = 0; i < 64; i++)
  {
    int v = ch.g.sq[i];
    if (!v)
      continue;
    int t = v < 0 ? -v : v;
    if (t == CH_K)
      continue;
    m += (v > 0 ? 1 : -1) * (t == CH_B ? 3 : chVal[t] / 100);
  }
  ch.matDif = m;
}

static void chNuevaPartida(bool primera)
{
  chInicio(ch.g);
  chNuevoJuego(); // limpia la memoria de posiciones del motor
  ch.nHist = 0;
  ch.hist[ch.nHist++] = chClave(ch.g);
  ch.full = 1;
  ch.estado = 0;
  ch.ganador = 0;
  ch.hayUlt = false;
  ch.enJaque = false;
  ch.matDif = 0;
  memset(ch.log, 0, sizeof(ch.log));
  if (primera)
  {
    ch.cpuBlancas = 1;
    ch.score1 = ch.score2 = ch.tablas = 0;
  }
  else
    ch.cpuBlancas = 3 - ch.cpuBlancas; // cambio de colores
  ch.siguiente = millis() + 1200;
}

static void chFin(int estado, int ganador)
{
  ch.estado = estado;
  ch.ganador = ganador;
  if (ganador == 1)
  {
    if (ch.score1 < 99999)
      ch.score1++;
  }
  else if (ganador == 2)
  {
    if (ch.score2 < 99999)
      ch.score2++;
  }
  else if (ch.tablas < 9999)
    ch.tablas++;
  ch.finAt = millis();
}

// Aplica la jugada, apunta la notacion y comprueba si la partida se ha acabado
static void chJuega(const ChMove &m)
{
  ChGame &g = ch.g;
  const int mover = (g.side == 1) ? ch.cpuBlancas : 3 - ch.cpuBlancas;
  char san[12];
  chSAN(g, m, san);
  const int num = ch.full;

  ChUndo u;
  chMake(g, m, u);
  if (g.side == 1)
    ch.full++; // acaban de mover las negras

  char (*lg)[16] = ch.log[mover - 1];
  for (int i = 0; i < 5; i++)
    memcpy(lg[i], lg[i + 1], 16);
  snprintf(lg[5], 16, "%3d %s", num, san);

  ch.ult = m;
  ch.hayUlt = true;
  ch.enJaque = chReyEnJaque(g, g.side);
  chMaterial();

  if (g.half == 0)
    ch.nHist = 0; // tras captura o peon no puede repetirse nada de lo anterior
  if (ch.nHist < 104)
    ch.hist[ch.nHist++] = chClave(g);

  ChMove l[CH_MAXMOV];
  int nl = chLegales(g, l);
  int rep = 0;
  for (int i = 0; i < ch.nHist; i++)
    if (ch.hist[i] == ch.hist[ch.nHist - 1])
      rep++;

  if (nl == 0)
  {
    if (ch.enJaque)
      chFin(1, mover);
    else
      chFin(2, 0);
  }
  else if (g.half >= 100)
    chFin(4, 0);
  else if (chMaterialInsuf(g))
    chFin(5, 0);
  else if (rep >= 3)
    chFin(3, 0);
  else if (ch.full > 250) // por si acaso, que ninguna partida dure eternamente
    chFin(6, 0);

  ch.siguiente = millis() + CH_PAUSA_MS;
}

// Aqui no hay tactil: el cambio de pantalla lo hace el driver. Por eso las tandas son de 1 s y se repinta
// todo al entrar (otras pantallas o los mensajes de Telegram pueden haber pisado lo que habia)
void tDisplay_Ajedrez(unsigned long mElapsed)
{
  mineria = getMiningData(mElapsed);
  Serial.printf("M8AX - >>> Completados %s Share(s), %s Khashes, Prom. Hashrate %s KH/s %s°\n",
                mineria.completedShares.c_str(), mineria.totalKHashes.c_str(), mineria.currentHashRate.c_str(), mineria.temp.c_str());

  // La RAM del motor solo existe durante esta llamada: se reserva ahora y se devuelve al final
  if (!chMemAlloc())
  {
    Serial.printf("M8AX - ### Ajedrez: Sin RAM Para El Motor ( Libre %u, Mayor Bloque %u ) ###\n", (unsigned)ESP.getFreeHeap(), (unsigned)ESP.getMaxAllocHeap());
    delay(1000);
    return;
  }

  // Si hace rato que no nos llaman es que se ha visto otra pantalla: borrar antes de pintar
  bool limpiar = !ch.init || (millis() - ch.ultLlamada > 2500);
  if (!ch.init)
  {
    ch.init = true;
    chNuevaPartida(true);
  }
  if (limpiar)
    chPintaTodo(true); // primera vez o venimos de otra pantalla: todo
  else
    chPintaTablero(false); // si no, solo las casillas que han cambiado (sin parpadeo)

  char horaStr[9] = "";
  char lastHora[9] = "";
  char hashStr[16];
  char lastHash[16] = "";

  unsigned long start = millis();
  unsigned long lastMiningUpdate = 0;

  while (millis() - start < CH_SLICE_MS)
  {
    unsigned long now = millis();

    // ---- Partida ----
    if (ch.estado == 0)
    {
      if (now >= ch.siguiente)
      {
        ChMove m;
        if (chElegir(ch.g, m, CH_PROF, CH_TIEMPO_MS, ch.hist, ch.nHist))
          chJuega(m);
        else
          chFin(2, 0); // no deberia pasar, pero asi no se cuelga
        chPintaTablero(false);
        chPintaPanel(1);
        chPintaPanel(2);
        if (ch.estado != 0)
          chPintaMarcador();
      }
    }
    else if (now - ch.finAt >= CH_FIN_MS)
    {
      chNuevaPartida(false);
      chPintaTodo(true);
    }

    // ---- Hora y hashrate ----
    if (now - lastMiningUpdate >= 1000)
    {
      lastMiningUpdate = now;
      mineria = getMiningData(mElapsed);
      snprintf(hashStr, sizeof(hashStr), "%s", mineria.currentHashRate.c_str());
      time_t epoch = (time_t)timeClient.getEpochTime();
      struct tm *ti = localtime(&epoch);
      snprintf(horaStr, sizeof(horaStr), "%02d:%02d:%02d", ti->tm_hour, ti->tm_min, ti->tm_sec);
      if (strcmp(horaStr, lastHora) != 0 || strcmp(hashStr, lastHash) != 0)
      {
        chPintaHud(hashStr, horaStr);
        strcpy(lastHora, horaStr);
        strcpy(lastHash, hashStr);
      }
    }

    delay(10);
  }

  tft.setTextPadding(0);
  tft.setTextDatum(TL_DATUM);
  ch.ultLlamada = millis();

  // El driver cambia de pantalla por su cuenta: no sabemos si volveremos, asi que se devuelve la RAM siempre
  chMemLibera();
}

/****************************************************************************************************************
 *
 *   Escrito por: M8AX
 *
 *   Descripción: TETRIS CPU - Pantalla para NerdminerV2
 *   ---------------------------------------------------
 *
 *   La CPU juega sola al Tetris. Antes de soltar cada pieza mira todos los giros y columnas posibles (y
 *   también dónde podría caer la siguiente) y se queda con lo que deja el campo más limpio: menos huecos,
 *   columnas más bajas y más parejas, y más líneas hechas. La pieza se va girando y moviendo a la vista
 *   mientras cae. Para que no juegue perfecta eternamente, según sube el nivel se equivoca un poquito
 *   más, así que tarde o temprano se llena, sale GAME OVER y empieza otra partida xD...
 *
 *   Arriba a la izquierda la hora y arriba a la derecha el hashrate, como en las demás. A la izquierda
 *   nivel, líneas y partidas jugadas; a la derecha la pieza siguiente, los puntos y el récord. Se puntúa
 *   como el Tetris de siempre (40, 100, 300 y 1200 por 1, 2, 3 o 4 líneas, por el nivel + 1) y se sube de
 *   nivel cada 10 líneas, con lo que cae más rápido.
 *
 *   Memoria: no pide NADA al heap. Mientras no está en pantalla solo guarda la partida en unos 150 bytes
 *   (el campo va comprimido a 4 bits por casilla). El campo de trabajo (200 bytes) y lo que usa la CPU
 *   para pensar viven en la pila solo mientras la pantalla corre, y se liberan al salir de cada tanda.
 *
 *   Para activarla hay que pegar este bloque antes del array tDisplayCyclicScreens[] y meter tDisplay_Tetris
 *   en ese array. No usa táctil ni botones, de eso se encarga el driver; como la pantalla puede cambiar en
 *   cualquier momento, la partida se guarda y sigue donde se quedó cuando vuelve a salir.
 *
 ****************************************************************************************************************/

// ---------------------------------------------------------------------------------------------------------
//  TETRIS
// ---------------------------------------------------------------------------------------------------------

#define TT_FX 125 // x del campo
#define TT_FY 28  // y del campo
#define TT_C 7    // lado de cada casilla
#define TT_COLS 10
#define TT_ROWS 20
#define TT_FIN_MS 5000 // lo que se queda el campo lleno antes de empezar otra partida

#define TT_GRIS 0xBDF7
#define TT_ROJO 0xF800
#define TT_AMARILLO 0xFFE0

// colores por pieza (I O T S Z J L) y el gris del game over; el 0 es casilla vacia
static const uint16_t TT_COL[9] = {0x0000, 0x07FF, 0xFFE0, 0xA01E, 0x07E0, 0xF800, 0x041F, 0xFD20, 0x7BEF};

// piezas en su posicion inicial (x, y de cada una de las 4 casillas)
static const int8_t TT_BASE[7][4][2] = {
    {{0, 0}, {1, 0}, {2, 0}, {3, 0}}, // I
    {{0, 0}, {1, 0}, {0, 1}, {1, 1}}, // O
    {{1, 0}, {0, 1}, {1, 1}, {2, 1}}, // T
    {{1, 0}, {2, 0}, {0, 1}, {1, 1}}, // S
    {{0, 0}, {1, 0}, {1, 1}, {2, 1}}, // Z
    {{0, 0}, {0, 1}, {1, 1}, {2, 1}}, // J
    {{2, 0}, {0, 1}, {1, 1}, {2, 1}}  // L
};
static const uint8_t TT_NROT[7] = {2, 1, 4, 2, 2, 4, 4}; // giros distintos de cada pieza

// Lo unico que se guarda entre tandas: ~150 bytes. El campo va a 4 bits por casilla (dos por byte)
struct TtGuardado
{
  uint8_t celdas[100];
  uint8_t init, estado; // estado: 0 jugando, 1 game over
  int8_t pieza, rot, px, py, sig, tgtRot, tgtX;
  uint8_t bolsa[7], bolsaI;
  uint32_t puntos, record, lineas, partidas;
  unsigned long finAt, ultLlamada;
};
static TtGuardado tt = {{0}, 0};

typedef uint8_t TtCampo[TT_ROWS][TT_COLS];

static void ttGuarda(TtCampo b)
{
  const uint8_t *p = &b[0][0];
  for (int i = 0; i < 100; i++)
    tt.celdas[i] = (p[i * 2] & 15) | (p[i * 2 + 1] << 4);
}

static void ttCarga(TtCampo b)
{
  uint8_t *p = &b[0][0];
  for (int i = 0; i < 100; i++)
  {
    p[i * 2] = tt.celdas[i] & 15;
    p[i * 2 + 1] = tt.celdas[i] >> 4;
  }
}

// Casillas de la pieza p girada rot veces (a la derecha), pegadas arriba a la izquierda
static void ttCeldas(int p, int rot, int8_t c[4][2], int &w, int &h)
{
  int x[4], y[4];
  for (int i = 0; i < 4; i++)
  {
    x[i] = TT_BASE[p][i][0];
    y[i] = TT_BASE[p][i][1];
  }
  for (int r = 0; r < rot; r++)
    for (int i = 0; i < 4; i++)
    {
      int nx = 3 - y[i];
      y[i] = x[i];
      x[i] = nx;
    }
  int mx = 9, my = 9, Mx = 0, My = 0;
  for (int i = 0; i < 4; i++)
  {
    if (x[i] < mx)
      mx = x[i];
    if (y[i] < my)
      my = y[i];
  }
  for (int i = 0; i < 4; i++)
  {
    c[i][0] = x[i] - mx;
    c[i][1] = y[i] - my;
    if (c[i][0] > Mx)
      Mx = c[i][0];
    if (c[i][1] > My)
      My = c[i][1];
  }
  w = Mx + 1;
  h = My + 1;
}

static bool ttChoca(TtCampo b, const int8_t c[4][2], int px, int py)
{
  for (int i = 0; i < 4; i++)
  {
    int x = px + c[i][0], y = py + c[i][1];
    if (x < 0 || x >= TT_COLS || y >= TT_ROWS)
      return true;
    if (y >= 0 && b[y][x])
      return true;
  }
  return false;
}

static void ttColoca(TtCampo b, const int8_t c[4][2], int px, int py, uint8_t id)
{
  for (int i = 0; i < 4; i++)
    if (py + c[i][1] >= 0)
      b[py + c[i][1]][px + c[i][0]] = id;
}

// Quita las filas completas y devuelve cuantas eran
static int ttLimpia(TtCampo b)
{
  int w = TT_ROWS - 1, n = 0;
  for (int r = TT_ROWS - 1; r >= 0; r--)
  {
    bool llena = true;
    for (int c = 0; c < TT_COLS; c++)
      if (!b[r][c])
      {
        llena = false;
        break;
      }
    if (llena)
    {
      n++;
      continue;
    }
    if (w != r)
      memcpy(b[w], b[r], TT_COLS);
    w--;
  }
  for (; w >= 0; w--)
    memset(b[w], 0, TT_COLS);
  return n;
}

// ---- La CPU: puntua como queda el campo (menos altura, huecos y desniveles; mas lineas hechas) ----
static int32_t ttEval(TtCampo b, int lineas)
{
  int h[TT_COLS], alto = 0, huecos = 0, desn = 0;
  for (int c = 0; c < TT_COLS; c++)
  {
    h[c] = 0;
    bool visto = false;
    for (int r = 0; r < TT_ROWS; r++)
    {
      if (b[r][c])
      {
        if (!visto)
        {
          h[c] = TT_ROWS - r;
          visto = true;
        }
      }
      else if (visto)
        huecos++;
    }
    alto += h[c];
  }
  for (int c = 0; c < TT_COLS - 1; c++)
    desn += h[c] > h[c + 1] ? h[c] - h[c + 1] : h[c + 1] - h[c];
  int mx = 0;
  for (int c = 0; c < TT_COLS; c++)
    if (h[c] > mx)
      mx = h[c];
  int32_t v = -510 * alto + 760 * lineas - 360 * huecos - 184 * desn;
  if (mx > 14)
    v -= (mx - 14) * 1500; // cerca del techo, a bajar como sea
  return v;
}

// Deja la pieza en la columna x con ese giro sobre una copia del campo. false si no cabe
static bool ttSim(TtCampo b, TtCampo nb, int p, int rot, int x, int &lineas)
{
  int8_t c[4][2];
  int w, h;
  ttCeldas(p, rot, c, w, h);
  if (x < 0 || x + w > TT_COLS || ttChoca(b, c, x, 0))
    return false;
  int y = 0;
  while (!ttChoca(b, c, x, y + 1))
    y++;
  memcpy(nb, b, TT_ROWS * TT_COLS);
  ttColoca(nb, c, x, y, p + 1);
  lineas = ttLimpia(nb);
  return true;
}

struct TtCand
{
  int8_t rot, x;
  int32_t v;
};

// Mira todas las colocaciones de la pieza y, para cada una, la mejor de la siguiente. Elige la mejor
// (o, de vez en cuando segun el nivel, una de las otras dos: asi acaba cometiendo fallos)
static void ttPlan(TtCampo b, int p, int sig, int nivel)
{
  TtCand cand[TT_COLS * 4];
  int n = 0;
  uint8_t nb[TT_ROWS][TT_COLS], nb2[TT_ROWS][TT_COLS];
  for (int rot = 0; rot < TT_NROT[p]; rot++)
    for (int x = 0; x < TT_COLS; x++)
    {
      int l1;
      if (!ttSim(b, nb, p, rot, x, l1))
        continue;
      int32_t v = ttEval(nb, l1);
      if (sig >= 0)
      {
        int32_t mejor = -2000000000;
        for (int r2 = 0; r2 < TT_NROT[sig]; r2++)
          for (int x2 = 0; x2 < TT_COLS; x2++)
          {
            int l2;
            if (!ttSim(nb, nb2, sig, r2, x2, l2))
              continue;
            int32_t v2 = ttEval(nb2, l2);
            if (v2 > mejor)
              mejor = v2;
          }
        v = (mejor == -2000000000) ? -900000 : v / 2 + mejor;
      }
      cand[n].rot = rot;
      cand[n].x = x;
      cand[n].v = v;
      n++;
    }
  if (n == 0)
  {
    tt.tgtRot = 0;
    tt.tgtX = 3;
    return;
  }
  for (int k = 0; k < 3 && k < n; k++) // las tres mejores al principio
  {
    int m = k;
    for (int j = k + 1; j < n; j++)
      if (cand[j].v > cand[m].v)
        m = j;
    TtCand t = cand[k];
    cand[k] = cand[m];
    cand[m] = t;
  }
  int perr = nivel * 25;
  if (perr > 450)
    perr = 450;
  int e = 0;
  if (n > 1 && (int)(esp_random() % 1000) < perr)
    e = 1 + esp_random() % ((n < 3 ? n : 3) - 1);
  tt.tgtRot = cand[e].rot;
  tt.tgtX = cand[e].x;
}

static int ttSiguiente()
{
  if (tt.bolsaI >= 7) // bolsa de 7: salen todas las piezas antes de repetir
  {
    for (int i = 0; i < 7; i++)
      tt.bolsa[i] = i;
    for (int i = 6; i > 0; i--)
    {
      int j = esp_random() % (i + 1);
      uint8_t t = tt.bolsa[i];
      tt.bolsa[i] = tt.bolsa[j];
      tt.bolsa[j] = t;
    }
    tt.bolsaI = 0;
  }
  return tt.bolsa[tt.bolsaI++];
}

static inline int ttNivel() { return (int)(tt.lineas / 10); }

// ---- Dibujo ----
static void ttCeldaPx(int x0, int y0, int id)
{
  if (!id)
  {
    tft.fillRect(x0, y0, TT_C, TT_C, TFT_BLACK);
    tft.drawPixel(x0 + 3, y0 + 3, 0x2104);
    return;
  }
  uint16_t c = TT_COL[id];
  tft.fillRect(x0, y0, TT_C, TT_C, (c >> 1) & 0x7BEF); // borde mas oscuro
  tft.fillRect(x0 + 1, y0 + 1, TT_C - 2, TT_C - 2, c);
}

static inline void ttCelda(int c, int r, int id) { ttCeldaPx(TT_FX + c * TT_C, TT_FY + r * TT_C, id); }

static void ttDibujaCampo(TtCampo b, bool gris)
{
  for (int r = 0; r < TT_ROWS; r++)
    for (int c = 0; c < TT_COLS; c++)
      ttCelda(c, r, (gris && b[r][c]) ? 8 : b[r][c]);
}

static void ttDibujaPieza(int id) // id 0 = borrarla
{
  int8_t c[4][2];
  int w, h;
  ttCeldas(tt.pieza, tt.rot, c, w, h);
  for (int i = 0; i < 4; i++)
  {
    int y = tt.py + c[i][1];
    if (y >= 0)
      ttCelda(tt.px + c[i][0], y, id);
  }
}

static void ttTxt(const char *s, int x, int y, int size, uint16_t col, int pad)
{
  tft.setFreeFont(NULL);
  tft.setTextFont(1);
  tft.setTextSize(size);
  tft.setTextColor(col, TFT_BLACK);
  tft.setTextPadding(pad);
  tft.setTextDatum(TL_DATUM);
  tft.drawString(s, x, y);
}

static void ttPanelIzq()
{
  char b[24];
  ttTxt("M8AX-TRIS", 4, 30, 2, TT_AMARILLO, 100);
  if (tt.estado)
    ttTxt("GAME OVER", 4, 50, 1, TT_ROJO, 112);
  else
    ttTxt("JUEGA LA CPU", 4, 50, 1, TT_GRIS, 112);
  ttTxt("NIVEL", 4, 72, 1, TT_GRIS, 100);
  snprintf(b, sizeof(b), "%d", ttNivel());
  ttTxt(b, 4, 82, 2, TFT_WHITE, 100);
  ttTxt("LINEAS", 4, 104, 1, TT_GRIS, 100);
  snprintf(b, sizeof(b), "%lu", (unsigned long)tt.lineas);
  ttTxt(b, 4, 114, 2, TFT_WHITE, 100);
  snprintf(b, sizeof(b), "PARTIDAS %lu", (unsigned long)tt.partidas);
  ttTxt(b, 4, 142, 1, TT_GRIS, 112);
  tft.setTextPadding(0);
}

static void ttPanelDer()
{
  char b[24];
  ttTxt("SIGUIENTE", 208, 30, 1, TT_GRIS, 100);
  tft.fillRect(208, 42, 60, 30, TFT_BLACK);
  int8_t c[4][2];
  int w, h;
  ttCeldas(tt.sig, 0, c, w, h);
  for (int i = 0; i < 4; i++)
    ttCeldaPx(214 + c[i][0] * TT_C, 46 + c[i][1] * TT_C, tt.sig + 1);
  ttTxt("PUNTOS", 208, 82, 1, TT_GRIS, 100);
  snprintf(b, sizeof(b), "%06lu", (unsigned long)tt.puntos);
  ttTxt(b, 208, 92, 2, TFT_WHITE, 108);
  ttTxt("RECORD", 208, 116, 1, TT_GRIS, 100);
  snprintf(b, sizeof(b), "%06lu", (unsigned long)tt.record);
  ttTxt(b, 208, 126, 2, TT_AMARILLO, 108);
  tft.setTextPadding(0);
}

static void ttPintaTodo(TtCampo b)
{
  tft.drawRect(TT_FX - 2, TT_FY - 2, TT_COLS * TT_C + 4, TT_ROWS * TT_C + 4, TT_GRIS);
  ttDibujaCampo(b, tt.estado != 0);
  if (!tt.estado)
    ttDibujaPieza(tt.pieza + 1);
  ttPanelIzq();
  ttPanelDer();
}

// ---- Partida ----
static void ttFin()
{
  tt.estado = 1;
  tt.finAt = millis();
  if (tt.puntos > tt.record)
    tt.record = tt.puntos;
  tt.partidas++;
}

// Saca la pieza siguiente arriba y planea donde ponerla. false = no cabe (game over)
static bool ttSpawn(TtCampo b)
{
  tt.pieza = tt.sig;
  tt.sig = ttSiguiente();
  tt.rot = 0;
  tt.px = 3;
  tt.py = 0;
  int8_t c[4][2];
  int w, h;
  ttCeldas(tt.pieza, 0, c, w, h);
  if (ttChoca(b, c, tt.px, tt.py))
    return false;
  ttPlan(b, tt.pieza, tt.sig, ttNivel());
  return true;
}

static void ttNueva(TtCampo b)
{
  memset(b, 0, TT_ROWS * TT_COLS);
  tt.puntos = 0;
  tt.lineas = 0;
  tt.estado = 0;
  tt.bolsaI = 7;
  tt.sig = ttSiguiente();
  ttSpawn(b);
}

static inline unsigned long ttPasoMs()
{
  int ms = 190 - ttNivel() * 14;
  return ms < 28 ? 28 : ms;
}

// Un paso: la CPU gira o mueve un poco la pieza y esta baja una fila
static void ttPaso(TtCampo b)
{
  int8_t c[4][2];
  int w, h;
  ttDibujaPieza(0);
  if (tt.rot != tt.tgtRot)
  {
    int nr = (tt.rot + 1) % TT_NROT[tt.pieza];
    ttCeldas(tt.pieza, nr, c, w, h);
    if (!ttChoca(b, c, tt.px, tt.py))
      tt.rot = nr;
    else
      tt.tgtRot = tt.rot; // no puede girar: se queda como esta
  }
  else if (tt.px != tt.tgtX)
  {
    int nx = tt.px + (tt.tgtX > tt.px ? 1 : -1);
    ttCeldas(tt.pieza, tt.rot, c, w, h);
    if (!ttChoca(b, c, nx, tt.py))
      tt.px = nx;
    else
      tt.tgtX = tt.px;
  }
  ttCeldas(tt.pieza, tt.rot, c, w, h);
  if (!ttChoca(b, c, tt.px, tt.py + 1))
  {
    tt.py++;
    ttDibujaPieza(tt.pieza + 1);
    return;
  }

  // aterriza
  ttColoca(b, c, tt.px, tt.py, tt.pieza + 1);
  bool llena[TT_ROWS];
  int n = 0;
  for (int r = 0; r < TT_ROWS; r++)
  {
    llena[r] = true;
    for (int x = 0; x < TT_COLS; x++)
      if (!b[r][x])
      {
        llena[r] = false;
        break;
      }
    if (llena[r])
      n++;
  }
  if (n)
  {
    static const uint16_t pts[5] = {0, 40, 100, 300, 1200};
    tt.puntos += pts[n] * (ttNivel() + 1);
    tt.lineas += n;
    for (int r = 0; r < TT_ROWS; r++) // destello antes de borrarlas
      if (llena[r])
        tft.fillRect(TT_FX, TT_FY + r * TT_C, TT_COLS * TT_C, TT_C, TFT_WHITE);
    delay(80);
    ttLimpia(b);
    ttDibujaCampo(b, false);
  }
  else
    ttDibujaPieza(tt.pieza + 1);
  tt.puntos += 4;
  if (!ttSpawn(b))
  {
    ttFin();
    ttDibujaCampo(b, true);
    ttPanelIzq();
    ttPanelDer();
    return;
  }
  ttDibujaPieza(tt.pieza + 1);
  ttPanelIzq();
  ttPanelDer();
}

// Aqui no hay tactil: el cambio de pantalla lo hace el driver, por eso las tandas son de 1 s
#define TT_SLICE_MS 1000

// Hora arriba a la izquierda, hashrate arriba a la derecha
static void ttPintaHud(const char *hash, const char *hora)
{
  tft.setFreeFont(NULL);
  tft.setTextFont(1);
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextPadding(100);
  tft.setTextDatum(TL_DATUM);
  tft.drawString(hora, 4, 9);
  char b[24];
  snprintf(b, sizeof(b), "%s KH/s", hash);
  tft.setTextPadding(150);
  tft.setTextDatum(TR_DATUM);
  tft.drawString(b, 316, 9);
  tft.setTextPadding(0);
  tft.setTextDatum(TL_DATUM);
}

void tDisplay_Tetris(unsigned long mElapsed)
{
  mineria = getMiningData(mElapsed);
  Serial.printf("M8AX - >>> Completados %s Share(s), %s Khashes, Prom. Hashrate %s KH/s %s°\n",
                mineria.completedShares.c_str(), mineria.totalKHashes.c_str(), mineria.currentHashRate.c_str(), mineria.temp.c_str());

  uint8_t b[TT_ROWS][TT_COLS]; // el campo vive aqui, en la pila, solo mientras dura la tanda
  // Si hace rato que no nos llaman es que se ha visto otra pantalla: borrar antes de pintar
  bool limpiar = !tt.init || (millis() - tt.ultLlamada > 2500);
  if (!tt.init)
  {
    tt.init = 1;
    tt.record = 0;
    tt.partidas = 0;
    ttNueva(b);
  }
  else
    ttCarga(b);
  if (limpiar)
    tft.fillRect(0, 0, 320, 170, TFT_BLACK);
  ttPintaTodo(b);

  char horaStr[9] = "";
  char lastHora[9] = "";
  char hashStr[16];
  char lastHash[16] = "";

  unsigned long start = millis();
  unsigned long lastMiningUpdate = 0;
  unsigned long siguiente = start + ttPasoMs();

  while (millis() - start < TT_SLICE_MS)
  {
    unsigned long now = millis();

    // ---- Partida ----
    if (!tt.estado)
    {
      if (now >= siguiente)
      {
        ttPaso(b);
        siguiente = millis() + ttPasoMs();
      }
    }
    else if (now - tt.finAt >= TT_FIN_MS)
    {
      ttNueva(b);
      tft.fillRect(TT_FX, TT_FY, TT_COLS * TT_C, TT_ROWS * TT_C, TFT_BLACK);
      ttPintaTodo(b);
      siguiente = millis() + ttPasoMs();
    }

    // ---- Hora y hashrate ----
    if (now - lastMiningUpdate >= 1000)
    {
      lastMiningUpdate = now;
      mineria = getMiningData(mElapsed);
      snprintf(hashStr, sizeof(hashStr), "%s", mineria.currentHashRate.c_str());
      time_t epoch = (time_t)timeClient.getEpochTime();
      struct tm *ti = localtime(&epoch);
      snprintf(horaStr, sizeof(horaStr), "%02d:%02d:%02d", ti->tm_hour, ti->tm_min, ti->tm_sec);
      if (strcmp(horaStr, lastHora) != 0 || strcmp(hashStr, lastHash) != 0)
      {
        ttPintaHud(hashStr, horaStr);
        strcpy(lastHora, horaStr);
        strcpy(lastHash, hashStr);
      }
    }

    delay(5);
  }

  ttGuarda(b); // la partida se guarda comprimida y el campo de trabajo desaparece con la pila
  tft.setTextPadding(0);
  tft.setTextDatum(TL_DATUM);
  tt.ultLlamada = millis();
}

/****************************************************************************************************************
 *
 *   Escrito por: M8AX
 *
 *   Descripción: PRIMOS A SACO - Pantalla para NerdminerV2
 *   ------------------------------------------------------
 *
 *   Un número enorme en el centro y debajo de él los primos anteriores, uno encima de otro y cada vez más
 *   apagados. Empiezan saliendo seguidos (2 3 5 7 11 13...) y después va subiendo, multiplicando por 1,25
 *   cada vez, hasta llegar al mayor primo que cabe en 64 bits, 18446744073709551557 (2^64 - 59). Ahí se
 *   para unos segundos y vuelve a empezar desde el 2.
 *
 *   El número grande es de 7 segmentos y se ajusta solo al ancho: con una cifra ocupa media pantalla y con
 *   las 20 cifras de un uint64 llena la pantalla entera de lado a lado, en una sola línea. Los primos no
 *   se sacan de una tabla, se calculan de verdad: es Miller-Rabin con las 12 primeras bases, que es exacto
 *   para cualquier número de 64 bits (la multiplicación grande se hace sumando y duplicando, así no hace
 *   falta nada de 128 bits). Cada primo gigante tarda unos pocos milisegundos en la ESP32.
 *
 *   Arriba a la izquierda la hora y arriba a la derecha el hashrate, como en las demás. Abajo una barra con
 *   cuántos de los 64 bits lleva usados el número y el 2^64-1 de referencia.
 *
 *   Memoria: no pide NADA al heap. Guarda solo unos 100 bytes de estado entre tandas, el resto vive en la
 *   pila mientras corre la pantalla.
 *
 *   Para activarla hay que pegar este bloque antes del array tDisplayCyclicScreens[] y meter tDisplay_Primos
 *   en ese array. No usa táctil ni botones, de eso se encarga el driver; como la pantalla puede cambiar en
 *   cualquier momento, el estado se guarda y sigue donde se quedó cuando vuelve a salir.
 *
 ****************************************************************************************************************/

// ---------------------------------------------------------------------------------------------------------
//  PRIMOS
// ---------------------------------------------------------------------------------------------------------

#define PM_MAXPRIMO 18446744073709551557ULL // el primo mas grande que cabe en un uint64 (2^64 - 59)
#define PM_UMBRAL 14000000000000000000ULL   // por encima de esto se salta directo al ultimo
#define PM_N_SEC 30                         // los primeros 30 salen seguidos: 2 3 5 7 11 13...
#define PM_PASO_SEC_MS 0                    // ms entre primos mientras salen seguidos
#define PM_PASO_SALTO_MS 90                 // ms entre primos mientras va subiendo
#define PM_FIN_MS 5000                      // lo que se queda el mayor primo antes de empezar otra vez

#define PM_BANDA_Y 76 // la banda del numero grande
#define PM_BANDA_H 54

static const uint16_t PM_COL[7] = {0x07FF, 0x07E0, 0xFFE0, 0xFD20, 0xF81F, 0xF800, 0x041F};

// ---- Primalidad exacta para cualquier uint64 (Miller-Rabin con las 12 primeras bases, determinista) ----
static uint64_t pmMulMod(uint64_t a, uint64_t b, uint64_t m)
{
  if (m <= 0xFFFFFFFFULL) // cabe en 64 bits, multiplicacion directa
    return (a % m) * (b % m) % m;
  uint64_t r = 0; // si no, multiplicar sumando y duplicando sin pasarse de 64 bits
  a %= m;
  while (b)
  {
    if (b & 1)
      r = (r >= m - a) ? r - (m - a) : r + a;
    a = (a >= m - a) ? a - (m - a) : a + a;
    b >>= 1;
  }
  return r;
}

static uint64_t pmPowMod(uint64_t a, uint64_t e, uint64_t m)
{
  uint64_t r = 1;
  a %= m;
  while (e)
  {
    if (e & 1)
      r = pmMulMod(r, a, m);
    a = pmMulMod(a, a, m);
    e >>= 1;
  }
  return r;
}

static bool pmEsPrimo(uint64_t n)
{
  static const uint8_t sp[12] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37};
  if (n < 2)
    return false;
  for (int i = 0; i < 12; i++)
  {
    if (n == sp[i])
      return true;
    if (n % sp[i] == 0)
      return false;
  }
  if (n < 1369) // 37 * 37
    return true;
  uint64_t d = n - 1;
  int s = 0;
  while (!(d & 1))
  {
    d >>= 1;
    s++;
  }
  for (int i = 0; i < 12; i++)
  {
    uint64_t x = pmPowMod(sp[i], d, n);
    if (x == 1 || x == n - 1)
      continue;
    bool compuesto = true;
    for (int r = 1; r < s; r++)
    {
      x = pmMulMod(x, x, n);
      if (x == n - 1)
      {
        compuesto = false;
        break;
      }
    }
    if (compuesto)
      return false;
  }
  return true;
}

// Primer primo >= n. Devuelve 0 si ya no queda ninguno dentro de los 64 bits
static uint64_t pmSiguiente(uint64_t n)
{
  if (n <= 2)
    return 2;
  if (!(n & 1))
    n++;
  while (n <= PM_MAXPRIMO)
  {
    if (pmEsPrimo(n))
      return n;
    if (n > PM_MAXPRIMO - 2)
      break;
    n += 2;
  }
  return 0;
}

// ---- Estado: lo unico que se guarda entre tandas (~100 bytes, nada de heap) ----
struct PmEstado
{
  uint8_t init, fase; // fase: 0 seguidos, 1 subiendo, 2 mayor primo (parado)
  uint64_t actual;
  uint64_t hist[4]; // los 4 anteriores, el mas reciente primero
  uint8_t nHist;
  uint32_t k; // numero de orden mientras salen seguidos
  unsigned long finAt, ultLlamada;
  char vista[21]; // lo que hay dibujado en la banda (para repintar solo las cifras que cambian)
};
static PmEstado pm = {0};
// Medicion de primos calculados por segundo
static uint32_t pmCuentaVelocidad = 0;
static uint32_t pmPrimosSegundo = 0;
static unsigned long pmTiempoVelocidad = 0;
static uint32_t pmVelocidadMostrada = 0;

static void pmStr(uint64_t v, char *b)
{
  char t[21];
  int n = 0;
  do
  {
    t[n++] = '0' + (int)(v % 10);
    v /= 10;
  } while (v);
  for (int i = 0; i < n; i++)
    b[i] = t[n - 1 - i];
  b[n] = 0;
}

static int pmBits(uint64_t v)
{
  int b = 0;
  while (v)
  {
    b++;
    v >>= 1;
  }
  return b;
}

// Siguiente primo de la lista: seguidos al principio y despues va multiplicando por ~1,25 hasta el mayor de 64 bits
// Busca TODOS los primos consecutivos, sin saltarse ninguno.
static void pmAvanza()
{
  for (int i = 3; i > 0; i--)
    pm.hist[i] = pm.hist[i - 1];

  pm.hist[0] = pm.actual;

  if (pm.nHist < 4)
    pm.nHist++;

  // Buscar siempre el siguiente primo consecutivo
  uint64_t p = pmSiguiente(pm.actual + 1);

  if (p == 0)
  {
    // Se han agotado los primos representables en uint64
    pm.fase = 2;
    pm.finAt = millis();
    return;
  }

  pm.actual = p;
  pm.k++;
  pmCuentaVelocidad++;
}

static void pmReinicia()
{
  pm.fase = 0;
  pm.actual = 2;
  pm.k = 1;
  pm.nHist = 0;
  pm.vista[0] = 0;
}

// ---- Dibujo ----
// Digitos de 7 segmentos: a(arriba) b(arriba dcha) c(abajo dcha) d(abajo) e(abajo izq) f(arriba izq) g(medio)
static const uint8_t PM_SEG[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};

static void pmDigito(int x0, int y0, int w, int h, int d, uint16_t col)
{
  int t = w / 6 < 2 ? 2 : w / 6;
  int hv = (h - 3 * t) / 2;
  uint8_t m = PM_SEG[d];
  tft.fillRect(x0, y0, w, h, TFT_BLACK); // borra la casilla del digito
  if (m & 0x01)
    tft.fillRect(x0 + t, y0, w - 2 * t, t, col);
  if (m & 0x02)
    tft.fillRect(x0 + w - t, y0 + t, t, hv, col);
  if (m & 0x04)
    tft.fillRect(x0 + w - t, y0 + 2 * t + hv, t, hv, col);
  if (m & 0x08)
    tft.fillRect(x0 + t, y0 + 2 * t + 2 * hv, w - 2 * t, t, col);
  if (m & 0x10)
    tft.fillRect(x0, y0 + 2 * t + hv, t, hv, col);
  if (m & 0x20)
    tft.fillRect(x0, y0 + t, t, hv, col);
  if (m & 0x40)
    tft.fillRect(x0 + t, y0 + t + hv, w - 2 * t, t, col);
}

// El numero grande, centrado y del tamano justo para que quepan hasta 20 cifras en toda la pantalla.
// Solo repinta las cifras que han cambiado; si cambia el numero de cifras, repinta la banda entera.
static void pmPintaNumero(bool forzar)
{
  char s[21];
  pmStr(pm.actual, s);
  int n = strlen(s);
  int p = 316 / n;
  if (p > 32)
    p = 32;
  int gap = p / 5 < 2 ? 2 : p / 5;
  int w = p - gap;
  int t = w / 6 < 2 ? 2 : w / 6;
  int hv = (w * 9 / 5 - 3 * t) / 2;
  int h = 3 * t + 2 * hv;
  int x0 = (320 - (n * p - gap)) / 2;
  int y0 = PM_BANDA_Y + (PM_BANDA_H - h) / 2;
  uint16_t col = (pm.fase == 2) ? 0xFFE0 : PM_COL[(n - 1) % 7];

  bool todo = forzar || (int)strlen(pm.vista) != n;
  if (todo)
    tft.fillRect(0, PM_BANDA_Y, 320, PM_BANDA_H, TFT_BLACK);
  for (int i = 0; i < n; i++)
    if (todo || pm.vista[i] != s[i] || pm.fase == 2)
      pmDigito(x0 + i * p, y0, w, h, s[i] - '0', col);
  strcpy(pm.vista, s);
}

static void pmTxt(const char *s, int y, int size, uint16_t col)
{
  tft.setFreeFont(NULL);
  tft.setTextFont(1);
  tft.setTextSize(size);
  tft.setTextColor(col, TFT_BLACK);
  tft.setTextPadding(320);
  tft.setTextDatum(TC_DATUM);
  tft.drawString(s, 160, y);
  tft.setTextPadding(0);
  tft.setTextDatum(TL_DATUM);
}

// Los anteriores, uno encima de otro y cada vez mas apagados
static void pmPintaPila()
{
  static const int ys[4] = {56, 44, 36, 28};
  static const int tam[4] = {2, 1, 1, 1};
  static const uint16_t cl[4] = {0xBDF7, 0x8410, 0x4A69, 0x2965};
  char s[21];
  for (int i = 0; i < 4; i++)
  {
    if (i < pm.nHist)
      pmStr(pm.hist[i], s);
    else
      s[0] = 0;
    pmTxt(s, ys[i], tam[i], cl[i]);
  }
}

static void pmPintaInfo()
{
  char b[64];
  uint16_t col = 0xBDF7;

  if (pm.fase == 2)
  {
    snprintf(b, sizeof(b), "MAYOR PRIMO DE 64 BITS");
    col = 0x07E0;
  }
  else
  {
    snprintf(b, sizeof(b), "PRIMO N. %llu",
             (unsigned long long)pm.k);
  }

  pmTxt(b, 136, 1, col);

  // Velocidad de calculo a la derecha
  char v[32];
  snprintf(v, sizeof(v), "%lu PRIMOS/SEG",
           (unsigned long)pmVelocidadMostrada);

  tft.setFreeFont(NULL);
  tft.setTextFont(1);
  tft.setTextSize(1);
  tft.setTextColor(0x07E0, TFT_BLACK);
  tft.setTextDatum(TR_DATUM);
  tft.drawString(v, 316, 136);
  tft.setTextDatum(TL_DATUM);

  // Barra de progreso de los 64 bits
  tft.drawRect(10, 148, 300, 8, 0xBDF7);

  int ancho = 296 * pmBits(pm.actual) / 64;

  tft.fillRect(12, 150, ancho, 4,
               (pm.fase == 2) ? 0x07E0 : 0x07FF);

  tft.fillRect(12 + ancho, 150, 296 - ancho, 4, TFT_BLACK);

  pmTxt("2^64-1 = 18446744073709551615", 160, 1, 0x4A69);
}

static void pmPintaTodo()
{
  pmPintaPila();
  pmPintaNumero(true);
  pmPintaInfo();
}

static inline unsigned long pmPasoMs() { return pm.fase == 0 ? PM_PASO_SEC_MS : PM_PASO_SALTO_MS; }

// Un paso de la animacion
static void pmPaso()
{
  pmAvanza();
  pmPintaPila();
  pmPintaNumero(false);
  pmPintaInfo();
}

// Aqui no hay tactil: el cambio de pantalla lo hace el driver, por eso las tandas son de 1 s
#define PM_SLICE_MS 1000

// Hora arriba a la izquierda, hashrate arriba a la derecha
static void pmPintaHud(const char *hash, const char *hora)
{
  tft.setFreeFont(NULL);
  tft.setTextFont(1);
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextPadding(100);
  tft.setTextDatum(TL_DATUM);
  tft.drawString(hora, 4, 9);
  char b[24];
  snprintf(b, sizeof(b), "%s KH/s", hash);
  tft.setTextPadding(150);
  tft.setTextDatum(TR_DATUM);
  tft.drawString(b, 316, 9);
  tft.setTextPadding(0);
  tft.setTextDatum(TL_DATUM);
}

void tDisplay_Primos(unsigned long mElapsed)
{
  mineria = getMiningData(mElapsed);
  Serial.printf("M8AX - >>> Completados %s Share(s), %s Khashes, Prom. Hashrate %s KH/s %s°\n",
                mineria.completedShares.c_str(), mineria.totalKHashes.c_str(), mineria.currentHashRate.c_str(), mineria.temp.c_str());

  // Si hace rato que no nos llaman es que se ha visto otra pantalla: borrar antes de pintar
  bool limpiar = !pm.init || (millis() - pm.ultLlamada > 2500);
  if (!pm.init)
  {
    pm.init = 1;
    pmReinicia();
  }
  if (limpiar)
    tft.fillRect(0, 0, 320, 170, TFT_BLACK);
  pmPintaTodo();

  char horaStr[9] = "";
  char lastHora[9] = "";
  char hashStr[16];
  char lastHash[16] = "";

  unsigned long start = millis();
  unsigned long lastMiningUpdate = 0;
  unsigned long siguiente = start + pmPasoMs();

  // Inicializar el cronometro solamente la primera vez
  if (pmTiempoVelocidad == 0)
    pmTiempoVelocidad = start;
  if (limpiar)
    pmTiempoVelocidad = millis();
  while (millis() - start < PM_SLICE_MS)
  {
    unsigned long now = millis();
    // Calcular primos encontrados por segundo
    if (now - pmTiempoVelocidad >= 1000)
    {
      pmVelocidadMostrada = pmCuentaVelocidad;
      pmCuentaVelocidad = 0;
      pmTiempoVelocidad += 1000;
      pmPintaInfo();
    }

    // ---- Primos ----

    if (pm.fase != 2)
    {
      if (now >= siguiente)
      {
        pmPaso();
        siguiente = millis() + pmPasoMs();
      }
    }

    // ---- Hora y hashrate ----

    if (now - lastMiningUpdate >= 1000)
    {
      lastMiningUpdate = now;
      mineria = getMiningData(mElapsed);
      snprintf(hashStr, sizeof(hashStr), "%s", mineria.currentHashRate.c_str());
      time_t epoch = (time_t)timeClient.getEpochTime();
      struct tm *ti = localtime(&epoch);
      snprintf(horaStr, sizeof(horaStr), "%02d:%02d:%02d", ti->tm_hour, ti->tm_min, ti->tm_sec);
      if (strcmp(horaStr, lastHora) != 0 || strcmp(hashStr, lastHash) != 0)
      {
        pmPintaHud(hashStr, horaStr);
        strcpy(lastHora, horaStr);
        strcpy(lastHash, hashStr);
      }
    }

    delay(30);
  }

  tft.setTextPadding(0);
  tft.setTextDatum(TL_DATUM);
  pm.ultLlamada = millis();
}

/****************************************************************************************************************
 *
 *   Escrito por: M8AX
 *
 *   Descripción: FRACTALES - Pantalla para NerdminerV2 ( T-Display )
 *   ----------------------------------------------------------------
 *
 *   Mandelbrot, Julia, Burning Ship, Tricorn y Newton calculados por el ESP32. Se pinta entrelazado: primero
 *   borroso en bloques de 8 filas y luego se va afinando hasta 1 fila. Al terminar se queda unos segundos
 *   y sale otro fractal con zoom, paleta y constante al azar. Cada vez que se entra en la pantalla sale uno nuevo.
 *
 *   Abajo a la izquierda el nombre del fractal y a la derecha el hashrate.
 *
 *   Aquí no hay táctil ni LEDs: el cambio de pantalla lo hace el driver, por eso las tandas son de 1 s y el
 *   fractal a medio pintar se guarda en una struct estática ( unos 1,2 KB, nada de heap ).
 *
 *   Para activarla hay que pegar este bloque antes del array tDisplayCyclicScreens[] y meter tDisplay_Fractal.
 *
 ****************************************************************************************************************/

#define FRAC_W 320
#define FRAC_H 152
#define FRAC_SLICE_MS 1000 // tandas de 1 s: el driver cambia de pantalla entre llamadas
#define FRAC_HOLD_MS 8000  // tiempo que se queda el fractal terminado antes de sacar otro

struct FracEstado
{
  uint8_t tipo; // 0 Mandelbrot, 1 Julia, 2 Burning Ship, 3 Tricorn, 4 Newton
  float cx, cy, ancho;
  float jx, jy; // constante c de Julia
  int iterMax;
  float escala;
  uint16_t paso; // 0..152 filas pintadas
  unsigned long holdHasta;
  char nombre[16];
  uint16_t pal[256];
};

static FracEstado fr;
static uint16_t frLinea[FRAC_W];
static bool fracInit = false;
static unsigned long fracUlt = 0; // cuándo terminó la última tanda

static float fracRnd() { return (esp_random() >> 8) / 16777216.0f; }

static uint16_t fracDim(uint16_t c, int f) // oscurece un color RGB565 ( f = 0..255 )
{
  int r = ((c >> 11) & 31) * f / 255;
  int g = ((c >> 5) & 63) * f / 255;
  int b = (c & 31) * f / 255;
  return (r << 11) | (g << 5) | b;
}

static uint16_t fracPunto(float x, float y)
{
  int n = 0;
  float r2 = 0;

  if (fr.tipo == 4) // ---- Newton z^3 - 1 ----
  {
    float zx = x, zy = y;
    for (n = 0; n < 40; n++)
    {
      float z2x = zx * zx - zy * zy, z2y = 2 * zx * zy;
      float z3x = z2x * zx - z2y * zy, z3y = z2x * zy + z2y * zx;
      float nx = 2 * z3x + 1, ny = 2 * z3y;
      float ax = 3 * z2x, ay = 3 * z2y;
      float den = ax * ax + ay * ay;
      if (den < 1e-12f)
        return 0;
      zx = (nx * ax + ny * ay) / den;
      zy = (ny * ax - nx * ay) / den;
      float d0 = (zx - 1) * (zx - 1) + zy * zy;
      float d1 = (zx + 0.5f) * (zx + 0.5f) + (zy - 0.8660254f) * (zy - 0.8660254f);
      float d2 = (zx + 0.5f) * (zx + 0.5f) + (zy + 0.8660254f) * (zy + 0.8660254f);
      int raiz = -1;
      if (d0 < 1e-4f)
        raiz = 0;
      else if (d1 < 1e-4f)
        raiz = 1;
      else if (d2 < 1e-4f)
        raiz = 2;
      if (raiz >= 0)
      {
        int f = 255 - n * 22;
        if (f < 60)
          f = 60;
        return fracDim(fr.pal[raiz * 85], f);
      }
    }
    return 0;
  }

  float zx, zy, cx, cy;
  if (fr.tipo == 1) // Julia: z empieza en el punto, c fija
  {
    zx = x;
    zy = y;
    cx = fr.jx;
    cy = fr.jy;
  }
  else // Mandelbrot, Burning Ship y Tricorn: z empieza en 0, c es el punto
  {
    zx = 0;
    zy = 0;
    cx = x;
    cy = y;
    if (fr.tipo == 0) // atajo: cardioide y bulbo principal, dentro seguro
    {
      float q = (x - 0.25f) * (x - 0.25f) + y * y;
      if (q * (q + (x - 0.25f)) < 0.25f * y * y)
        return 0;
      if ((x + 1.0f) * (x + 1.0f) + y * y < 0.0625f)
        return 0;
    }
  }

  if (fr.tipo == 2) // Burning Ship
  {
    while (n < fr.iterMax)
    {
      float zx2 = zx * zx, zy2 = zy * zy;
      r2 = zx2 + zy2;
      if (r2 > 256.0f)
        break;
      zy = fabsf(2 * zx * zy) + cy;
      zx = zx2 - zy2 + cx;
      n++;
    }
  }
  else if (fr.tipo == 3) // Tricorn
  {
    while (n < fr.iterMax)
    {
      float zx2 = zx * zx, zy2 = zy * zy;
      r2 = zx2 + zy2;
      if (r2 > 256.0f)
        break;
      zy = -2 * zx * zy + cy;
      zx = zx2 - zy2 + cx;
      n++;
    }
  }
  else // Mandelbrot y Julia
  {
    while (n < fr.iterMax)
    {
      float zx2 = zx * zx, zy2 = zy * zy;
      r2 = zx2 + zy2;
      if (r2 > 256.0f)
        break;
      zy = 2 * zx * zy + cy;
      zx = zx2 - zy2 + cx;
      n++;
    }
  }

  if (n >= fr.iterMax)
    return 0;                                   // dentro del conjunto: negro
  float mu = n + 1.0f - log2f(0.5f * logf(r2)); // color suave, sin bandas
  return fr.pal[((int)(mu * fr.escala)) & 255];
}

static void fracNuevo()
{
  fr.tipo = esp_random() % 5;
  fr.paso = 0;
  fr.holdHasta = 0;
  fr.ancho = 3.4f;
  fr.cx = -0.5f;
  fr.cy = 0.0f;

  switch (fr.tipo)
  {
  case 0: // Mandelbrot: vista completa o zoom a un sitio famoso con un poco de azar
  {
    static const float pts[][2] = {{-0.7436f, 0.1318f}, {0.2820f, 0.0107f}, {-0.0880f, 0.6540f}, {-1.7549f, 0.0f}, {-1.9854f, 0.0f}, {-0.7269f, 0.1889f}, {-0.1600f, 1.0405f}};
    if (esp_random() % 6 != 0)
    {
      int p = esp_random() % 7;
      fr.ancho = 3.0f / (float)(1 << (2 + esp_random() % 7)); // de 0.75 a 0.012 ( el float no da para más )
      fr.cx = pts[p][0] + (fracRnd() - 0.5f) * fr.ancho * 0.4f;
      fr.cy = pts[p][1] + (fracRnd() - 0.5f) * fr.ancho * 0.4f;
    }
    strcpy(fr.nombre, "MANDELBROT");
    break;
  }
  case 1: // Julia: constante conocida o una al azar en un círculo
  {
    static const float cs[][2] = {{-0.8f, 0.156f}, {-0.4f, 0.6f}, {0.285f, 0.01f}, {-0.70176f, -0.3842f}, {-0.835f, -0.2321f}, {-0.7269f, 0.1889f}, {0.355f, 0.355f}};
    if (esp_random() & 1)
    {
      int p = esp_random() % 7;
      fr.jx = cs[p][0];
      fr.jy = cs[p][1];
    }
    else
    {
      float a = fracRnd() * 6.2831853f;
      fr.jx = 0.7885f * cosf(a);
      fr.jy = 0.7885f * sinf(a);
    }
    fr.ancho = 1.8f + fracRnd() * 1.4f;
    fr.cx = (fracRnd() - 0.5f) * 0.4f;
    fr.cy = (fracRnd() - 0.5f) * 0.3f;
    strcpy(fr.nombre, "JULIA");
    break;
  }
  case 2: // Burning Ship
  {
    static const float pts[][2] = {{-1.76f, -0.03f}, {-1.65f, -0.04f}, {-1.86f, -0.005f}, {-1.62f, -0.02f}};
    if (esp_random() % 5 == 0)
    {
      fr.cx = -0.5f;
      fr.cy = -0.5f;
      fr.ancho = 3.6f;
    }
    else
    {
      int p = esp_random() % 4;
      fr.ancho = 0.4f / (float)(1 << (esp_random() % 5));
      fr.cx = pts[p][0] + (fracRnd() - 0.5f) * fr.ancho * 0.4f;
      fr.cy = pts[p][1] + (fracRnd() - 0.5f) * fr.ancho * 0.4f;
    }
    strcpy(fr.nombre, "BURNING SHIP");
    break;
  }
  case 3: // Tricorn
    fr.ancho = 0.4f + fracRnd() * 3.0f;
    fr.cx = -0.3f + (fracRnd() - 0.5f) * 1.0f;
    fr.cy = (fracRnd() - 0.5f) * 0.8f;
    strcpy(fr.nombre, "TRICORN");
    break;
  default: // Newton
    fr.ancho = 0.15f + fracRnd() * 3.5f;
    fr.cx = (fracRnd() - 0.5f) * 2.0f;
    fr.cy = (fracRnd() - 0.5f) * 1.4f;
    strcpy(fr.nombre, "NEWTON");
    break;
  }

  fr.iterMax = 60 + (int)(35.0f * log2f(3.4f / fr.ancho));
  if (fr.iterMax < 60)
    fr.iterMax = 60;
  if (fr.iterMax > 300)
    fr.iterMax = 300;
  fr.escala = 3.0f + fracRnd() * 6.0f;

  // Paleta nueva al azar ( 3 ondas de color con fase distinta )
  float pf[3], pd[3];
  for (int k = 0; k < 3; k++)
  {
    pf[k] = 1.0f + (float)(esp_random() % 2);
    pd[k] = fracRnd();
  }
  for (int i = 0; i < 256; i++)
  {
    float t = i / 255.0f;
    float r = 0.5f + 0.5f * cosf(6.2831853f * (pf[0] * t + pd[0]));
    float g = 0.5f + 0.5f * cosf(6.2831853f * (pf[1] * t + pd[1]));
    float b = 0.5f + 0.5f * cosf(6.2831853f * (pf[2] * t + pd[2]));
    fr.pal[i] = tft.color565((uint8_t)(r * 255), (uint8_t)(g * 255), (uint8_t)(b * 255));
  }
}

// Orden de pintado entrelazado: primero borroso ( bloques de 8 filas ), luego se afina hasta 1 fila
static void fracPaso(uint16_t s, int &y, int &h)
{
  if (s < 19)
  {
    y = s * 8;
    h = 8;
  }
  else if (s < 38)
  {
    y = (s - 19) * 8 + 4;
    h = 4;
  }
  else if (s < 76)
  {
    y = (s - 38) * 4 + 2;
    h = 2;
  }
  else
  {
    y = (s - 76) * 2 + 1;
    h = 1;
  }
}

static void fracPintaFila(int y, int h)
{
  float alto = fr.ancho * ((float)FRAC_H / (float)FRAC_W);
  float py = fr.cy + ((float)y / (float)FRAC_H - 0.5f) * alto;
  float dx = fr.ancho / (float)FRAC_W;
  float px = fr.cx - fr.ancho * 0.5f;
  for (int i = 0; i < FRAC_W; i++)
  {
    frLinea[i] = fracPunto(px, py);
    px += dx;
  }
  tft.startWrite();
  tft.setAddrWindow(0, y, FRAC_W, h);
  for (int k = 0; k < h; k++)
    tft.pushColors(frLinea, FRAC_W, true);
  tft.endWrite();
}

static void fracDrawHud(const char *izq, const char *der)
{
  tft.setFreeFont(NULL);
  tft.setTextFont(1);
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextPadding(150);
  tft.setTextDatum(TL_DATUM);
  tft.drawString(izq, 4, 153);
  tft.setTextDatum(TR_DATUM);
  tft.drawString(der, 316, 153);
  tft.setTextPadding(0);
  tft.setTextDatum(TL_DATUM);
}

void tDisplay_Fractal(unsigned long mElapsed)
{
  mineria = getMiningData(mElapsed);
  Serial.printf("M8AX - >>> Completados %s Share(s), %s Khashes, Prom. Hashrate %s KH/s %s°\n",
                mineria.completedShares.c_str(), mineria.totalKHashes.c_str(), mineria.currentHashRate.c_str(), mineria.temp.c_str());
  // Si hace rato que no nos llaman es que se ha visto otra pantalla: fractal nuevo y todo en negro
  bool limpiar = !fracInit || (millis() - fracUlt > 2500);
  fracInit = true;
  if (limpiar)
  {
    fracNuevo();
    tft.fillRect(0, 0, 320, 170, TFT_BLACK);
  }

  char hashStr[16];
  snprintf(hashStr, sizeof(hashStr), "%s", mineria.currentHashRate.c_str());
  char izq[24], der[24];
  char lastIzq[24] = "", lastDer[24] = "";

  unsigned long start = millis();
  unsigned long lastHud = 0;
  unsigned long lastMiningUpdate = 0;

  while (millis() - start < FRAC_SLICE_MS)
  {
    unsigned long now = millis();

    // ---- Fractal: una tanda de filas por vuelta ----
    if (fr.paso < FRAC_H)
    {
      int y, h;
      fracPaso(fr.paso, y, h);
      fracPintaFila(y, h);
      fr.paso++;
      if (fr.paso == FRAC_H) // terminado: a esperar
        fr.holdHasta = millis() + FRAC_HOLD_MS;
    }
    else if (now >= fr.holdHasta) // toca otro fractal nuevo
    {
      fracNuevo();
      tft.fillRect(0, 0, 320, FRAC_H, TFT_BLACK);
    }

    // ---- Hashrate (cada segundo) ----
    if (now - lastMiningUpdate >= 1000)
    {
      lastMiningUpdate = now;
      mineria = getMiningData(mElapsed);
      snprintf(hashStr, sizeof(hashStr), "%s", mineria.currentHashRate.c_str());
    }

    // ---- Textos de abajo ----
    if (now - lastHud >= 200)
    {
      lastHud = now;
      snprintf(izq, sizeof(izq), "%s", fr.nombre);
      snprintf(der, sizeof(der), "%s KH/s", hashStr);
      if (strcmp(izq, lastIzq) != 0 || strcmp(der, lastDer) != 0)
      {
        fracDrawHud(izq, der);
        strcpy(lastIzq, izq);
        strcpy(lastDer, der);
      }
    }
    delay(fr.paso < FRAC_H ? 1 : 10); // cede CPU al minero
  }

  tft.setTextPadding(0);
  tft.setTextDatum(TL_DATUM);
  fracUlt = millis();
}

/****************************************************************************************************************
 *
 *   Escrito por: M8AX
 *
 *   Descripción: GALAXIAS N CUERPOS - Pantalla para NerdminerV2 ( T-Display )
 *   -------------------------------------------------------------------------
 *
 *   Dos galaxias de 160 cuerpos cada una, con su núcleo pesado y sus estrellas en órbita, se acercan y chocan.
 *   Cada cuerpo atrae a todos los demás de verdad ( gravedad de Newton en 3D ), así que en cada paso el ESP32
 *   resuelve 51.040 interacciones. Los pasos se encadenan sin parar hasta que toca pintar el cuadro ( cada
 *   40 ms ), o sea que la CPU va a tope todo el rato.
 *
 *   Abajo a la izquierda sale cuántos millones de interacciones por segundo se calculan de verdad y a la
 *   derecha el hashrate. La cámara gira despacio. Una galaxia azul y la otra naranja; cuanto más rápida va
 *   una estrella, más brillante. Cada 5000 pasos nace otra pareja con inclinaciones y giros al azar.
 *
 *   Memoria: los unos 16 KB de los cuerpos se piden con malloc al entrar y se devuelven cuando se detecta que
 *   se ha salido a otra pantalla. Si no hay RAM, se salta la pantalla y lo dice por el puerto serie.
 *   Si aprieta la RAM, baja NBD_N ( 240 va de sobra y cuesta casi la mitad ).
 *
 *   Para activarla hay que pegar este bloque antes del array tDisplayCyclicScreens[] y meter tDisplay_Galaxias.
 *
 ****************************************************************************************************************/

#define NBD_N 320 // cuerpos: 2 núcleos + estrellas (par, y mejor que no baje de 8)
#define NBD_PARES ((NBD_N * (NBD_N - 1)) / 2)
#define NBD_DT 0.002f
#define NBD_EPS2 0.0064f // suavizado al cuadrado
#define NBD_EC2 0.36f    // suavizado al cuadrado entre los dos núcleos
#define NBD_MC 12.0f     // masa de cada núcleo
#define NBD_MS 0.02f     // masa de cada estrella
#define NBD_ESC 48.0f    // píxeles por unidad
#define NBD_ALTO 152
#define NBD_FRAME_MS 40
#define NBD_SLICE_MS 1000 // tandas de 1 s: el driver cambia de pantalla entre llamadas
#define NBD_PASOS_MAX 5000

struct NbdMem
{
  float x[NBD_N], y[NBD_N], z[NBD_N];
  float vx[NBD_N], vy[NBD_N], vz[NBD_N];
  float m[NBD_N];
  float ax[NBD_N], ay[NBD_N], az[NBD_N];
  int16_t px[NBD_N], py[NBD_N]; // dónde está pintado cada cuerpo (-1 = en ningún sitio)
  int16_t nx[NBD_N], ny[NBD_N];
  uint8_t gal[NBD_N];
  uint32_t pasos;
  float giro;
};
static NbdMem *nbd = NULL;
static float nbdMint = 0;        // millones de interacciones por segundo ( suavizado )
static unsigned long nbdUlt = 0; // cuándo terminó la última tanda

static const uint16_t NBD_COL_A[8] = {0x0010, 0x001F, 0x041F, 0x07FF, 0x4FFF, 0x8FFF, 0xCFFF, 0xFFFF};
static const uint16_t NBD_COL_B[8] = {0x4000, 0x8000, 0xC000, 0xF800, 0xFC00, 0xFD20, 0xFFE0, 0xFFFF};

static float nbdRnd() { return (esp_random() >> 8) / 16777216.0f; }

static inline float nbdRsqrt(float x)
{
  float h = 0.5f * x;
  uint32_t i;
  memcpy(&i, &x, 4);
  i = 0x5f3759df - (i >> 1);
  memcpy(&x, &i, 4);
  return x * (1.5f - h * x * x);
}

static void nbdGalaxia(int nucleo, int ini, int fin, float cx, float cy, float cz, float vx, float vy, float vz, float incl, float sentido)
{
  float si = sinf(incl), co = cosf(incl);
  nbd->x[nucleo] = cx;
  nbd->y[nucleo] = cy;
  nbd->z[nucleo] = cz;
  nbd->vx[nucleo] = vx;
  nbd->vy[nucleo] = vy;
  nbd->vz[nucleo] = vz;
  nbd->m[nucleo] = NBD_MC;
  for (int i = ini; i < fin; i++)
  {
    float r = 0.25f + 0.75f * powf(nbdRnd(), 0.7f);
    float a = nbdRnd() * 6.2831853f;
    float v = sqrtf(NBD_MC / r) * (0.97f + 0.06f * nbdRnd()) * sentido; // velocidad circular
    float px = r * cosf(a), pz = r * sinf(a), py = (nbdRnd() - 0.5f) * 0.04f;
    float qx = -sinf(a) * v, qz = cosf(a) * v;
    nbd->x[i] = cx + px;
    nbd->y[i] = cy + py * co - pz * si;
    nbd->z[i] = cz + py * si + pz * co;
    nbd->vx[i] = vx + qx;
    nbd->vy[i] = vy - qz * si;
    nbd->vz[i] = vz + qz * co;
    nbd->m[i] = NBD_MS;
  }
}

static void nbdNueva()
{
  memset(nbd, 0, sizeof(NbdMem));
  float off = (nbdRnd() < 0.5f ? -1.0f : 1.0f) * (1.8f + 0.6f * nbdRnd());
  float vv = 1.0f + 0.2f * nbdRnd();
  int mitad = 2 + (NBD_N - 2) / 2;
  nbdGalaxia(0, 2, mitad, -1.6f, 0, -off / 2, vv, 0, 0, nbdRnd() * 3.1416f, nbdRnd() < 0.5f ? 1.0f : -1.0f);
  nbdGalaxia(1, mitad, NBD_N, 1.6f, 0, off / 2, -vv, 0, 0, nbdRnd() * 3.1416f, nbdRnd() < 0.5f ? 1.0f : -1.0f);
  for (int i = 0; i < NBD_N; i++)
  {
    nbd->gal[i] = (i == 0 || (i >= 2 && i < mitad)) ? 0 : 1;
    nbd->px[i] = nbd->py[i] = -1;
  }
}

static void nbdPaso()
{
  NbdMem *m = nbd;
  for (int i = 0; i < NBD_N; i++)
    m->ax[i] = m->ay[i] = m->az[i] = 0;

  for (int i = 0; i < NBD_N - 1; i++)
  {
    float xi = m->x[i], yi = m->y[i], zi = m->z[i], mi = m->m[i];
    float axi = 0, ayi = 0, azi = 0;
    for (int j = (i == 0 ? 2 : i + 1); j < NBD_N; j++) // el par de núcleos (0,1) va aparte
    {
      float dx = m->x[j] - xi, dy = m->y[j] - yi, dz = m->z[j] - zi;
      float inv = nbdRsqrt(dx * dx + dy * dy + dz * dz + NBD_EPS2);
      float f = inv * inv * inv;
      float fi = f * m->m[j], fj = f * mi;
      axi += dx * fi;
      ayi += dy * fi;
      azi += dz * fi;
      m->ax[j] -= dx * fj;
      m->ay[j] -= dy * fj;
      m->az[j] -= dz * fj;
    }
    m->ax[i] += axi;
    m->ay[i] += ayi;
    m->az[i] += azi;
  }

  float dx = m->x[1] - m->x[0], dy = m->y[1] - m->y[0], dz = m->z[1] - m->z[0];
  float inv = nbdRsqrt(dx * dx + dy * dy + dz * dz + NBD_EC2);
  float f = inv * inv * inv * NBD_MC;
  m->ax[0] += dx * f;
  m->ay[0] += dy * f;
  m->az[0] += dz * f;
  m->ax[1] -= dx * f;
  m->ay[1] -= dy * f;
  m->az[1] -= dz * f;

  for (int i = 0; i < NBD_N; i++)
  {
    m->vx[i] += m->ax[i] * NBD_DT;
    m->vy[i] += m->ay[i] * NBD_DT;
    m->vz[i] += m->az[i] * NBD_DT;
    m->x[i] += m->vx[i] * NBD_DT;
    m->y[i] += m->vy[i] * NBD_DT;
    m->z[i] += m->vz[i] * NBD_DT;
  }
}

static void nbdDibuja()
{
  NbdMem *m = nbd;
  float cg = cosf(m->giro), sg = sinf(m->giro);
  const float ct = 0.58f, st = 0.81f; // la cámara mira desde arriba, inclinada unos 55 grados

  for (int i = 0; i < NBD_N; i++)
  {
    float x1 = m->x[i] * cg - m->z[i] * sg;
    float z1 = m->x[i] * sg + m->z[i] * cg;
    float y1 = m->y[i] * ct - z1 * st;
    int sx = 160 + (int)(x1 * NBD_ESC);
    int sy = NBD_ALTO / 2 + (int)(y1 * NBD_ESC);
    if (sx < 3 || sx > 316 || sy < 3 || sy > NBD_ALTO - 4)
      sx = sy = -1;
    m->nx[i] = sx;
    m->ny[i] = sy;
  }

  for (int i = 0; i < NBD_N; i++)
  {
    if (m->px[i] >= 0 && (m->px[i] != m->nx[i] || m->py[i] != m->ny[i]))
    {
      if (i < 2)
        tft.fillCircle(m->px[i], m->py[i], 3, TFT_BLACK);
      else
        tft.drawPixel(m->px[i], m->py[i], TFT_BLACK);
    }
  }

  for (int i = 2; i < NBD_N; i++)
  {
    if (m->nx[i] < 0)
      continue;
    int b = (int)((m->vx[i] * m->vx[i] + m->vy[i] * m->vy[i] + m->vz[i] * m->vz[i]) * 0.025f);
    if (b > 7)
      b = 7;
    tft.drawPixel(m->nx[i], m->ny[i], m->gal[i] ? NBD_COL_B[b] : NBD_COL_A[b]);
  }
  if (m->nx[0] >= 0)
    tft.fillCircle(m->nx[0], m->ny[0], 3, 0x5FFF);
  if (m->nx[1] >= 0)
    tft.fillCircle(m->nx[1], m->ny[1], 3, TFT_YELLOW);

  memcpy(m->px, m->nx, sizeof(m->px));
  memcpy(m->py, m->ny, sizeof(m->py));
}

static void nbdDrawHud(const char *izq, const char *der)
{
  tft.setFreeFont(NULL);
  tft.setTextFont(1);
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextPadding(150);
  tft.setTextDatum(TL_DATUM);
  tft.drawString(izq, 4, 153);
  tft.setTextDatum(TR_DATUM);
  tft.drawString(der, 316, 153);
  tft.setTextPadding(0);
  tft.setTextDatum(TL_DATUM);
}

void tDisplay_Galaxias(unsigned long mElapsed)
{
  mineria = getMiningData(mElapsed);
  Serial.printf("M8AX - >>> Completados %s Share(s), %s Khashes, Prom. Hashrate %s KH/s %s°\n",
                mineria.completedShares.c_str(), mineria.totalKHashes.c_str(), mineria.currentHashRate.c_str(), mineria.temp.c_str());

  // Si hace rato que no nos llaman es que se ha visto otra pantalla: borrar antes de pintar
  bool limpiar = (millis() - nbdUlt > 2500);
  if (!nbd)
  {
    nbd = (NbdMem *)malloc(sizeof(NbdMem));
    if (!nbd)
    {
      Serial.printf("M8AX - ### Galaxias: Sin RAM ( Libre %u, Mayor Bloque %u ) ###\n", (unsigned)ESP.getFreeHeap(), (unsigned)ESP.getMaxAllocHeap());
      delay(1000);
      return;
    }
    nbdNueva();
    nbdMint = 0;
    limpiar = true;
  }
  const int nbdMiPantalla = currentDisplayDriver->current_cyclic_screen;
  if (limpiar)
  {
    tft.fillRect(0, 0, 320, 170, TFT_BLACK);
    for (int i = 0; i < NBD_N; i++)
      nbd->px[i] = nbd->py[i] = -1;
  }

  char hashStr[16];
  snprintf(hashStr, sizeof(hashStr), "%s", mineria.currentHashRate.c_str());
  char izq[24], der[24];
  char lastIzq[24] = "", lastDer[24] = "";

  unsigned long start = millis();
  unsigned long lastMiningUpdate = 0;

  while (millis() - start < NBD_SLICE_MS)
  {
    // ---- Física a tope hasta que toque pintar ----
    unsigned long tf = millis();
    uint32_t k = 0;
    do
    {
      nbdPaso();
      nbd->pasos++;
      k++;
    } while (millis() - tf < NBD_FRAME_MS);

    nbd->giro += 0.012f;
    nbdDibuja();

    if (nbd->pasos >= NBD_PASOS_MAX)
    {
      nbdNueva();
      tft.fillRect(0, 0, 320, NBD_ALTO, TFT_BLACK);
    }

    unsigned long now = millis();
    float inst = k * (float)NBD_PARES / ((now - tf) * 1000.0f);
    nbdMint = (nbdMint == 0) ? inst : nbdMint * 0.7f + inst * 0.3f;

    if (now - lastMiningUpdate >= 1000)
    {
      lastMiningUpdate = now;
      mineria = getMiningData(mElapsed);
      snprintf(hashStr, sizeof(hashStr), "%s", mineria.currentHashRate.c_str());
    }

    snprintf(izq, sizeof(izq), "%.1fM INT/S", nbdMint);
    snprintf(der, sizeof(der), "%s KH/s", hashStr);
    if (strcmp(izq, lastIzq) != 0 || strcmp(der, lastDer) != 0)
    {
      nbdDrawHud(izq, der);
      strcpy(lastIzq, izq);
      strcpy(lastDer, der);
    }
    delay(1);
  }

  tft.setTextPadding(0);
  tft.setTextDatum(TL_DATUM);
  nbdUlt = millis();

  // Si el driver ya ha cambiado de pantalla, se devuelven los 16 KB
  if (currentDisplayDriver->current_cyclic_screen != nbdMiPantalla)
  {
    free(nbd);
    nbd = NULL;
  }
}

/****************************************************************************************************************
 *
 *   Escrito por: M8AX
 *
 *   Descripción: GALAXIAN CPU - Pantalla para NerdminerV2 ( T-Display )
 *   --------------------------------------------------------------------
 *
 *   La CPU juega sola al Galaxian: una formación de 8x5 marcianos que se mueve de lado a lado y de vez en
 *   cuando se lanzan en picado contra la nave, disparando por el camino. La nave decide ella sola: si hay un
 *   disparo o un marciano en picado cerca se aparta, y si no se coloca debajo del marciano que más le
 *   conviene ( adelantándose a donde estará cuando llegue la bala ) y dispara. Cada oleada es un poco más
 *   rápida y pica más a menudo, y como la CPU esquiva peor cuanto más avanza el juego, tarde o temprano la
 *   matan. Tiene 3 vidas; al perder la última sale GAME OVER y empieza otra partida xD...
 *
 *   Arriba a la izquierda la hora y arriba a la derecha el hashrate, como en las demás. Abajo van los
 *   puntos, la oleada, las vidas y el récord. Sin LEDs: esta placa no los tiene.
 *
 *   Memoria: las tablas ( poco más de 1 KB ) se piden con malloc al entrar y se devuelven cuando se detecta
 *   que se ha salido a otra pantalla. Solo quedan fijos el récord y las partidas ( 8 bytes ).
 *
 *   Para activarla hay que pegar este bloque antes del array tDisplayCyclicScreens[] y meter tDisplay_Galaxian.
 *
 ****************************************************************************************************************/

#define GAL_COLS 8
#define GAL_ROWS 5
#define GAL_N (GAL_COLS * GAL_ROWS)
#define GAL_DX 26
#define GAL_DY 12
#define GAL_AW 12
#define GAL_AH 7
#define GAL_FY 36
#define GAL_FXMIN 4
#define GAL_FXMAX 122
#define GAL_TOP 28
#define GAL_SHIPY 146
#define GAL_SW 13
#define GAL_SH 8
#define GAL_FRAME_MS 25
#define GAL_FIN_MS 3000
#define GAL_SLICE_MS 1000 // tandas de 1 s: el driver cambia de pantalla entre llamadas
#define GAL_VIDAS 3
#define GAL_MAXB 3
#define GAL_MAXE 5
#define GAL_MAXX 6

struct GalAlien
{
  int x, y;   // donde está
  int dx, dy; // donde se pintó la última vez
  int x0;
  float ph;
  uint8_t vivo, modo, amp; // modo: 0 formación, 1 picando, 2 volviendo
};

struct GalBala
{
  int x, y;
  bool on;
};

struct GalExp
{
  int x, y, w, h, ttl;
};

struct GalMem
{
  GalAlien al[GAL_N];
  GalBala bp[GAL_MAXB]; // las de la nave
  GalBala be[GAL_MAXE]; // las de los marcianos
  GalExp ex[GAL_MAXX];
  int fx, fdir;
  int nx, nxPintada;
  uint16_t nCol;
  uint8_t estado; // 0 jugando, 1 nave explotando, 2 game over, 3 oleada limpia
  uint8_t vidas, nivel, vivos;
  bool anim, sucio;
  uint32_t puntos, fr;
  unsigned long finAt, disparoAt, picadoAt, animAt, inmuneHasta;
};
static GalMem *galM = NULL;
static uint32_t galRecord = 0;
static uint32_t galPartidas = 0;
static unsigned long galUlt = 0; // cuándo terminó la última tanda
static const uint16_t GAL_COL[GAL_ROWS] = {TFT_MAGENTA, TFT_ORANGE, TFT_CYAN, TFT_GREEN, TFT_SKYBLUE};

static void galDibujaAlien(int x, int y, uint16_t c, bool f)
{
  tft.fillRect(x + 2, y, 8, 2, c);
  tft.fillRect(x, y + 2, 12, 3, c);
  tft.fillRect(x + 3, y + 2, 2, 1, TFT_BLACK);
  tft.fillRect(x + 7, y + 2, 2, 1, TFT_BLACK);
  if (f)
  {
    tft.fillRect(x, y + 5, 2, 2, c);
    tft.fillRect(x + 10, y + 5, 2, 2, c);
  }
  else
  {
    tft.fillRect(x + 2, y + 5, 2, 2, c);
    tft.fillRect(x + 8, y + 5, 2, 2, c);
  }
}

static void galDibujaNave(int x, uint16_t c)
{
  tft.fillRect(x + 5, GAL_SHIPY, 3, 2, c);
  tft.fillRect(x + 3, GAL_SHIPY + 2, 7, 2, c);
  tft.fillRect(x, GAL_SHIPY + 4, 13, 4, c);
}

static void galStatus()
{
  char b[64];
  uint32_t rec = (galM->puntos > galRecord) ? galM->puntos : galRecord;
  snprintf(b, sizeof(b), "PTS %06lu   NIV %02u   VIDAS %u   REC %06lu",
           (unsigned long)galM->puntos, galM->nivel, galM->vidas, (unsigned long)rec);
  tft.setFreeFont(NULL);
  tft.setTextFont(1);
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextDatum(TC_DATUM);
  tft.setTextPadding(312);
  tft.drawString(b, 160, 160);
  tft.setTextPadding(0);
  tft.setTextDatum(TL_DATUM);
}

static void galTextoFin()
{
  tft.setFreeFont(NULL);
  tft.setTextFont(1);
  tft.setTextSize(3);
  tft.setTextColor(TFT_RED, TFT_BLACK);
  tft.setTextDatum(MC_DATUM);
  tft.drawString("GAME OVER", 160, 95);
  tft.setTextDatum(TL_DATUM);
  tft.setTextSize(1);
}

static void galExplota(int x, int y, int w, int h, int ttl)
{
  for (int i = 0; i < GAL_MAXX; i++)
    if (!galM->ex[i].ttl)
    {
      galM->ex[i] = {x, y, w, h, ttl};
      return;
    }
}

static void galExplosiones()
{
  for (int i = 0; i < GAL_MAXX; i++)
  {
    GalExp *e = &galM->ex[i];
    if (!e->ttl)
      continue;
    if (--e->ttl == 0)
    {
      tft.fillRect(e->x, e->y, e->w, e->h, TFT_BLACK);
      continue;
    }
    uint16_t c = (e->ttl & 1) ? TFT_YELLOW : TFT_RED;
    tft.fillRect(e->x + e->w / 3, e->y, e->w / 3, e->h, c);
    tft.fillRect(e->x, e->y + e->h / 3, e->w, e->h / 3, c);
  }
}

static void galOleada()
{
  galM->fx = 60;
  galM->fdir = 1;
  for (int i = 0; i < GAL_N; i++)
  {
    int hx = galM->fx + (i % GAL_COLS) * GAL_DX;
    int hy = GAL_FY + (i / GAL_COLS) * GAL_DY;
    galM->al[i] = {hx, hy, hx, hy, 0, 0.0f, 1, 0, 0};
  }
  for (int i = 0; i < GAL_MAXB; i++)
    galM->bp[i].on = false;
  for (int i = 0; i < GAL_MAXE; i++)
    galM->be[i].on = false;
  galM->vivos = GAL_N;
  galM->picadoAt = millis() + 1500;
}

static void galNueva()
{
  memset(galM, 0, sizeof(GalMem));
  galM->vidas = GAL_VIDAS;
  galM->nivel = 1;
  galM->nx = 150;
  galM->nxPintada = 150;
  galM->sucio = true;
  galOleada();
}

static void galPintaTodo()
{
  tft.fillRect(0, GAL_TOP, 320, 170 - GAL_TOP, TFT_BLACK);
  for (int i = 0; i < GAL_N; i++)
  {
    GalAlien *a = &galM->al[i];
    if (!a->vivo)
      continue;
    galDibujaAlien(a->x, a->y, GAL_COL[i / GAL_COLS], galM->anim);
    a->dx = a->x;
    a->dy = a->y;
  }
  if (galM->estado == 0 || galM->estado == 3)
  {
    galDibujaNave(galM->nx, TFT_WHITE);
    galM->nxPintada = galM->nx;
    galM->nCol = TFT_WHITE;
  }
  galStatus();
  if (galM->estado == 2)
    galTextoFin();
}

static void galDisparo()
{
  for (int i = 0; i < GAL_MAXB; i++)
    if (!galM->bp[i].on)
    {
      galM->bp[i] = {galM->nx + 6, GAL_SHIPY - 6, true};
      galM->disparoAt = millis() + 230;
      return;
    }
}

static void galDisparoEnemigo(GalAlien *a)
{
  for (int i = 0; i < GAL_MAXE; i++)
    if (!galM->be[i].on)
    {
      galM->be[i] = {a->x + 5, a->y + GAL_AH, true};
      return;
    }
}

static void galMata(int i, bool puntua)
{
  GalAlien *a = &galM->al[i];
  tft.fillRect(a->dx, a->dy, GAL_AW, GAL_AH, TFT_BLACK);
  galExplota(a->x, a->y, GAL_AW, GAL_AH, 6);
  a->vivo = 0;
  if (galM->vivos)
    galM->vivos--;
  if (puntua)
  {
    uint32_t pts = 10 * (GAL_ROWS - i / GAL_COLS);
    if (a->modo != 0)
      pts *= 3;
    galM->puntos += pts;
    galM->sucio = true;
  }
}

static void galNaveMuere()
{
  tft.fillRect(galM->nxPintada, GAL_SHIPY, GAL_SW, GAL_SH, TFT_BLACK);
  galExplota(galM->nxPintada - 4, GAL_SHIPY - 3, 21, 14, 16);
  galM->estado = 1;
  galM->finAt = millis();
  if (galM->vidas)
    galM->vidas--;
  galM->sucio = true;
}

static void galFin()
{
  galM->estado = 2;
  galM->finAt = millis();
  if (galM->puntos > galRecord)
    galRecord = galM->puntos;
  galPartidas++;
  Serial.printf("M8AX - Galaxian: Partida %lu | Puntos %lu | Oleada %u | Record %lu\n",
                (unsigned long)galPartidas, (unsigned long)galM->puntos, galM->nivel, (unsigned long)galRecord);
  galStatus();
  galTextoFin();
}

// Aquí decide la CPU: esquivar si hay peligro, y si no, apuntar y disparar
static void galIA(unsigned long now)
{
  int cx = galM->nx + 6;
  int ventana = 46 - galM->nivel * 3;
  if (ventana < 22)
    ventana = 22;
  int yPeor = -1, dPeor = 0;
  for (int i = 0; i < GAL_MAXE; i++)
  {
    GalBala *b = &galM->be[i];
    if (b->on && b->y > GAL_SHIPY - ventana && b->y < GAL_SHIPY + 4 && abs(b->x + 1 - cx) < 14 && b->y > yPeor)
    {
      yPeor = b->y;
      dPeor = b->x + 1 - cx;
    }
  }
  for (int i = 0; i < GAL_N; i++)
  {
    GalAlien *a = &galM->al[i];
    if (a->vivo && a->modo == 1 && a->y > GAL_SHIPY - ventana - 8 && abs(a->x + 6 - cx) < 20 && a->y > yPeor)
    {
      yPeor = a->y;
      dPeor = a->x + 6 - cx;
    }
  }

  int dir = 0, paso = 3;
  if (yPeor >= 0)
  {
    dir = (dPeor > 0) ? -1 : 1;
    if (dPeor == 0)
      dir = (cx > 160) ? -1 : 1;
    if ((galM->nx <= 3 && dir < 0) || (galM->nx >= 320 - GAL_SW - 3 && dir > 0))
      dir = -dir;
    paso = 4;
  }
  else
  {
    int mejor = -1, mejorD = 9999;
    for (int i = 0; i < GAL_N; i++)
    {
      GalAlien *a = &galM->al[i];
      if (!a->vivo)
        continue;
      int d = abs(a->x + 6 - cx);
      if (a->modo == 1 && a->y > 50)
        d -= 400;
      if (d < mejorD)
      {
        mejorD = d;
        mejor = i;
      }
    }
    if (mejor >= 0)
    {
      GalAlien *a = &galM->al[mejor];
      int lead = 0;
      if (a->modo == 0)
        lead = galM->fdir * ((galM->nivel > 3) ? 2 : 1) * ((GAL_SHIPY - a->y) / 6);
      int d = a->x + 6 + lead - cx;
      if (abs(d) > 3)
        dir = (d > 0) ? 1 : -1;
      if (abs(d) <= 5 && now >= galM->disparoAt)
        galDisparo();
    }
  }
  galM->nx += dir * paso;
  if (galM->nx < 2)
    galM->nx = 2;
  if (galM->nx > 320 - GAL_SW - 2)
    galM->nx = 320 - GAL_SW - 2;
}

static void galFrame(unsigned long now)
{
  galM->fr++;

  if (galM->estado == 2)
  {
    if (now - galM->finAt >= GAL_FIN_MS)
    {
      galNueva();
      galPintaTodo();
    }
    return;
  }
  if (galM->estado == 1 && now - galM->finAt >= 1200)
  {
    if (galM->vidas > 0)
    {
      galM->estado = 0;
      galM->nx = 150;
      galM->nxPintada = 150;
      galM->nCol = 0;
      galM->inmuneHasta = now + 1500;
    }
    else
    {
      galFin();
      return;
    }
  }
  if (galM->estado == 3 && now - galM->finAt >= 1200)
  {
    galM->nivel++;
    galOleada();
    galM->estado = 0;
    galM->sucio = true;
    galPintaTodo();
  }

  bool cambio = false;
  if (now - galM->animAt >= 350)
  {
    galM->anim = !galM->anim;
    galM->animAt = now;
    cambio = true;
  }

  int paso = (galM->nivel > 3) ? 2 : 1;
  galM->fx += galM->fdir * paso;
  if (galM->fx >= GAL_FXMAX)
  {
    galM->fx = GAL_FXMAX;
    galM->fdir = -1;
  }
  else if (galM->fx <= GAL_FXMIN)
  {
    galM->fx = GAL_FXMIN;
    galM->fdir = 1;
  }

  int cx = galM->nx + 6;
  bool vulnerable = (galM->estado == 0 && now >= galM->inmuneHasta);

  for (int i = 0; i < GAL_N; i++)
  {
    GalAlien *a = &galM->al[i];
    if (!a->vivo)
      continue;
    int hx = galM->fx + (i % GAL_COLS) * GAL_DX;
    int hy = GAL_FY + (i / GAL_COLS) * GAL_DY;
    if (a->modo == 0)
    {
      a->x = hx;
      a->y = hy;
    }
    else if (a->modo == 1)
    {
      a->y += (galM->nivel > 2) ? 3 : 2;
      a->ph += 0.11f;
      if (galM->nivel > 1 && (galM->fr & 1))
        a->x0 += (cx > a->x0 + 6) ? 1 : -1;
      int xx = a->x0 + (int)(a->amp * sinf(a->ph));
      if (xx < 0)
        xx = 0;
      if (xx > 308)
        xx = 308;
      a->x = xx;
      if (a->y > 98 && a->y < GAL_SHIPY - 24 && (int)(esp_random() % 1000) < 7 + galM->nivel * 2)
        galDisparoEnemigo(a);
      if (a->y >= GAL_SHIPY + 2)
      {
        a->modo = 2;
        a->x = hx;
        a->y = GAL_TOP;
      }
    }
    else
    {
      a->x = hx;
      a->y += 2;
      if (a->y >= hy)
      {
        a->y = hy;
        a->modo = 0;
      }
    }
    if (a->modo == 1 && vulnerable && a->x + GAL_AW > galM->nx && a->x < galM->nx + GAL_SW && a->y + GAL_AH > GAL_SHIPY)
    {
      galMata(i, false);
      galNaveMuere();
      vulnerable = false;
      continue;
    }
    if (cambio || a->x != a->dx || a->y != a->dy)
    {
      tft.fillRect(a->dx, a->dy, GAL_AW, GAL_AH, TFT_BLACK);
      galDibujaAlien(a->x, a->y, GAL_COL[i / GAL_COLS], galM->anim);
      a->dx = a->x;
      a->dy = a->y;
    }
  }

  if (galM->estado != 3 && now >= galM->picadoAt)
  {
    int picando = 0;
    for (int i = 0; i < GAL_N; i++)
      if (galM->al[i].vivo && galM->al[i].modo == 1)
        picando++;
    int maxPicando = 1 + galM->nivel / 2;
    if (maxPicando > 4)
      maxPicando = 4;
    if (picando < maxPicando)
    {
      for (int t = 0; t < 10; t++)
      {
        GalAlien *a = &galM->al[esp_random() % GAL_N];
        if (a->vivo && a->modo == 0)
        {
          a->modo = 1;
          a->x0 = a->x;
          a->ph = 0.0f;
          a->amp = 18 + esp_random() % 24;
          break;
        }
      }
    }
    int base = 1700 - galM->nivel * 170;
    if (base < 450)
      base = 450;
    galM->picadoAt = now + base + esp_random() % 500;
  }

  if (galM->estado == 0 || galM->estado == 3)
  {
    if (galM->estado == 0)
      galIA(now);
    uint16_t col = (now < galM->inmuneHasta) ? TFT_CYAN : TFT_WHITE;
    if (galM->nx != galM->nxPintada || col != galM->nCol)
    {
      tft.fillRect(galM->nxPintada, GAL_SHIPY, GAL_SW, GAL_SH, TFT_BLACK);
      galDibujaNave(galM->nx, col);
      galM->nxPintada = galM->nx;
      galM->nCol = col;
    }
  }

  for (int i = 0; i < GAL_MAXB; i++)
  {
    GalBala *b = &galM->bp[i];
    if (!b->on)
      continue;
    tft.fillRect(b->x, b->y, 2, 6, TFT_BLACK);
    b->y -= 6;
    if (b->y < GAL_TOP)
    {
      b->on = false;
      continue;
    }
    for (int j = 0; j < GAL_N; j++)
    {
      GalAlien *a = &galM->al[j];
      if (a->vivo && b->x + 1 >= a->x && b->x <= a->x + GAL_AW - 1 && b->y <= a->y + GAL_AH - 1 && b->y + 5 >= a->y)
      {
        galMata(j, true);
        b->on = false;
        break;
      }
    }
    if (b->on)
      tft.fillRect(b->x, b->y, 2, 6, TFT_YELLOW);
  }

  for (int i = 0; i < GAL_MAXE; i++)
  {
    GalBala *b = &galM->be[i];
    if (!b->on)
      continue;
    tft.fillRect(b->x, b->y, 2, 5, TFT_BLACK);
    b->y += (galM->nivel > 3) ? 4 : 3;
    if (b->y > GAL_SHIPY + GAL_SH)
    {
      b->on = false;
      continue;
    }
    if (vulnerable && b->x + 1 >= galM->nx && b->x <= galM->nx + GAL_SW - 1 && b->y + 4 >= GAL_SHIPY)
    {
      b->on = false;
      galNaveMuere();
      vulnerable = false;
      continue;
    }
    tft.fillRect(b->x, b->y, 2, 5, TFT_RED);
  }

  galExplosiones();

  if (galM->vivos == 0 && galM->estado == 0)
  {
    galM->estado = 3;
    galM->finAt = now;
  }
  if (galM->sucio)
  {
    galStatus();
    galM->sucio = false;
  }
}

// Hora arriba a la izquierda, hashrate arriba a la derecha ( igual que Tetris, Ajedrez y Primos )
static void galDrawHud(const char *hash, const char *hora)
{
  tft.setFreeFont(NULL);
  tft.setTextFont(1);
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextPadding(100);
  tft.setTextDatum(TL_DATUM);
  tft.drawString(hora, 4, 9);
  char b[24];
  snprintf(b, sizeof(b), "%s KH/s", hash);
  tft.setTextPadding(150);
  tft.setTextDatum(TR_DATUM);
  tft.drawString(b, 316, 9);
  tft.setTextPadding(0);
  tft.setTextDatum(TL_DATUM);
}

void tDisplay_Galaxian(unsigned long mElapsed)
{
  mineria = getMiningData(mElapsed);
  Serial.printf("M8AX - >>> Completados %s Share(s), %s Khashes, Prom. Hashrate %s KH/s %s°\n",
                mineria.completedShares.c_str(), mineria.totalKHashes.c_str(), mineria.currentHashRate.c_str(), mineria.temp.c_str());

  // Si hace rato que no nos llaman es que se ha visto otra pantalla: borrar antes de pintar
  bool limpiar = (millis() - galUlt > 2500);
  if (!galM)
  {
    galM = (GalMem *)malloc(sizeof(GalMem));
    if (!galM)
    {
      Serial.printf("M8AX - ### Galaxian: Sin RAM ( Libre %u, Mayor Bloque %u ) ###\n", (unsigned)ESP.getFreeHeap(), (unsigned)ESP.getMaxAllocHeap());
      delay(1000);
      return;
    }
    galNueva();
    limpiar = true;
  }
  const int galMiPantalla = currentDisplayDriver->current_cyclic_screen;
  if (limpiar)
  {
    tft.fillRect(0, 0, 320, 170, TFT_BLACK);
    galPintaTodo();
  }

  char horaStr[9] = "";
  char lastHora[9] = "";
  char hashStr[16];
  char lastHash[16] = "";

  unsigned long start = millis();
  unsigned long lastMiningUpdate = 0;
  unsigned long siguiente = start;

  while (millis() - start < GAL_SLICE_MS)
  {
    unsigned long now = millis();

    if (now >= siguiente)
    {
      galFrame(now);
      siguiente = millis() + GAL_FRAME_MS;
    }

    // ---- Hora ( izquierda ) y hashrate ( derecha ), arriba como en las demás ----
    if (now - lastMiningUpdate >= 1000)
    {
      lastMiningUpdate = now;
      mineria = getMiningData(mElapsed);
      snprintf(hashStr, sizeof(hashStr), "%s", mineria.currentHashRate.c_str());
      time_t epoch = (time_t)timeClient.getEpochTime();
      struct tm *ti = localtime(&epoch);
      snprintf(horaStr, sizeof(horaStr), "%02d:%02d:%02d", ti->tm_hour, ti->tm_min, ti->tm_sec);
      if (strcmp(horaStr, lastHora) != 0 || strcmp(hashStr, lastHash) != 0)
      {
        galDrawHud(hashStr, horaStr);
        strcpy(lastHora, horaStr);
        strcpy(lastHash, hashStr);
      }
    }
    delay(2);
  }

  tft.setTextPadding(0);
  tft.setTextDatum(TL_DATUM);
  galUlt = millis();

  // Si el driver ya ha cambiado de pantalla, se devuelve la RAM
  if (currentDisplayDriver->current_cyclic_screen != galMiPantalla)
  {
    free(galM);
    galM = NULL;
  }
}

/****************************************************************************************************************
 *
 *   Escrito por: M8AX
 *
 *   Descripción: ASTEROIDES M8AX - Los asteroides de toda la vida, pero jugados por la CPU del ESP32 para NerdminerV2 ( T-Display )
 *   ---------------------------------------------------------------------------------------------------------------------------------
 *
 *   La nave se pilota sola. Cada pocos fotogramas el ESP32 se inventa 144 futuros distintos para los próximos
 *   segundos ( gira a un lado, gira al otro, acelera, deja de acelerar, durante más o menos tiempo y encadenando
 *   dos maniobras ), los simula uno por uno con todas las rocas moviéndose, y se queda con el que menos
 *   choca y mejor deja la puntería. Al disparar no apunta a donde está la roca, sino a donde va a estar cuando
 *   llegue la bala. Si la cosa se pone fea y tarda mucho en pensar, se toma más fotogramas entre una decisión y
 *   otra para que la animación no se arrastre.
 *
 *   La pantalla se pinta entera cada fotograma en un buffer de 4 bits por píxel ( 16 colores ) y se manda de
 *   golpe al TFT, así que no parpadea. Antes de pintar, todo lo que había se apaga un escalón en su rampa de
 *   color, por eso las balas, las llamas y las explosiones dejan estela como en las pantallas vectoriales
 *   de las recreativas. Hay estrellas en tres capas que se mueven al revés de la nave, partículas, ondas
 *   de choque y temblor de pantalla cuando revienta algo grande.
 *
 *   Arriba a la izquierda la hora y arriba a la derecha el hashrate, como en las demás. Abajo van los puntos,
 *   la ola y las vidas. Si la nave pierde las tres vidas, empieza otra partida sola. Sin LEDs: esta placa no
 *   los tiene.
 *
 *   Memoria: unos 22 KB en el heap ( casi todo es el buffer de pantalla ), que se piden al entrar en la pantalla
 *   y se devuelven cuando se detecta que se ha salido a otra. Si no hay RAM, se salta la pantalla sin pintar nada.
 *
 *   Para activarla: pegar este bloque justo debajo de tDisplay_Galaxian ( antes del comentario de analiCadaSegundo ),
 *   meter tDisplay_Asteroides en el array tDisplayCyclicScreens[] y añadir en analiCadaSegundo, debajo del if de galM,
 *   este otro:
 *   if (astM && millis() - astUlt > 5000) { free(astM); astM = NULL; }
 *
 ****************************************************************************************************************/

// >>> ASTEROIDES MOTOR
#define AST_W 320
#define AST_H 120
#define AST_Y0 28 // el HUD de arriba ocupa hasta la 26 y el de abajo empieza en la 150
#define AST_FRAME_MS 20
#define AST_SLICE_MS 1000 // tandas de 1 s: el driver cambia de pantalla entre llamadas
#define AST_MAXA 20
#define AST_MAXB 5
#define AST_MAXP 64
#define AST_NSTAR 32
#define AST_NV 9
#define AST_HORIZ 36 // fotogramas que mira hacia delante al pensar
#define AST_ROT 0.13f
#define AST_EMPUJE 0.19f
#define AST_ROZA 0.992f
#define AST_VMAX 3.4f
#define AST_BVEL 6.2f
#define AST_BVIDA 32
#define AST_RANGO2 22500.0f
#define AST_PI 3.14159265f
#define AST_RGB(r, g, b) ((uint16_t)((((r) & 0xF8) << 8) | (((g) & 0xFC) << 3) | ((b) >> 3)))

// 0 negro | 1-2 estrellas | 3-6 blanco y sus grises | 7-10 fuego | 11-13 cian | 14-15 verde
static const uint16_t AST_PAL[16] = {
    AST_RGB(0, 0, 0), AST_RGB(24, 28, 64), AST_RGB(80, 90, 140), AST_RGB(255, 255, 255),
    AST_RGB(205, 205, 215), AST_RGB(135, 135, 155), AST_RGB(62, 62, 78), AST_RGB(255, 240, 120),
    AST_RGB(255, 150, 30), AST_RGB(230, 50, 20), AST_RGB(110, 20, 10), AST_RGB(120, 255, 255),
    AST_RGB(40, 170, 200), AST_RGB(10, 70, 100), AST_RGB(80, 255, 120), AST_RGB(20, 110, 50)};
// a qué color pasa cada uno en el siguiente fotograma
static const uint8_t AST_DEC[16] = {0, 0, 1, 4, 5, 6, 0, 8, 9, 10, 0, 12, 13, 0, 15, 0};
static const float AST_RAD[4] = {0.0f, 5.0f, 9.5f, 16.0f};
static const uint16_t AST_PTS[4] = {0, 100, 50, 20};
static const float AST_VAL[4] = {0.0f, 1.4f, 1.1f, 0.9f};

struct AstRoca
{
  float x, y, vx, vy, ang, vang;
  uint8_t tam, seed, flash; // tam: 0 libre, 1 chica, 2 media, 3 grande
};

struct AstBala
{
  float x, y, vx, vy;
  uint8_t vida;
};

struct AstPart
{
  float x, y, vx, vy;
  uint8_t vida, tipo; // 0 fuego, 1 blanca, 2 cian
};

struct AstOnda
{
  float x, y;
  uint8_t r, vida;
};

struct AstEstr
{
  float x, y;
  uint8_t capa;
};

struct AstMem
{
  uint8_t fb[AST_W * AST_H / 2];
  uint16_t linea[AST_W];
  AstRoca ro[AST_MAXA];
  AstBala ba[AST_MAXB];
  AstPart pt[AST_MAXP];
  AstOnda on[3];
  AstEstr es[AST_NSTAR];
  float nx, ny, nvx, nvy, nang;
  int8_t ctlRot, shx, shy, shk;
  uint8_t ctlEmp, empuje, viva, vidas, cadencia, planCnt, planCada, ipt;
  uint16_t inmune, reaparece, pausaOla, finTimer, ola;
  uint32_t puntos, mejor, frame, ledOff;
  uint8_t ledPrio;
  char tIzq[16], tCen[12], tDer[12];
};
static AstMem *astM = NULL;
static unsigned long astUlt = 0; // cuándo terminó la última tanda

struct AstCtx
{
  int n;
  float nx, ny, nvx, nvy, hx, hy, cs, sn;
  float x[AST_MAXA], y[AST_MAXA], vx[AST_MAXA], vy[AST_MAXA];
  float lim2[AST_MAXA], cer2[AST_MAXA], aim2[AST_MAXA], val[AST_MAXA];
};

static inline uint32_t astRnd(uint32_t n) { return esp_random() % n; }
static inline float astRf() { return (esp_random() & 0xFFFF) * (1.0f / 65535.0f); }

static inline float astDel(float d, float L)
{
  if (d > L * 0.5f)
    d -= L;
  else if (d < -L * 0.5f)
    d += L;
  return d;
}

static inline void astPix(int x, int y, uint8_t c)
{
  if ((unsigned)x >= AST_W || (unsigned)y >= AST_H)
    return;
  uint8_t *p = &astM->fb[y * (AST_W / 2) + (x >> 1)];
  if (x & 1)
    *p = (*p & 0x0F) | (c << 4);
  else
    *p = (*p & 0xF0) | c;
}

static void astLinea(int x0, int y0, int x1, int y1, uint8_t c)
{
  int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
  int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
  int err = dx + dy;
  for (;;)
  {
    astPix(x0, y0, c);
    if (x0 == x1 && y0 == y1)
      break;
    int e2 = 2 * err;
    if (e2 >= dy)
    {
      err += dy;
      x0 += sx;
    }
    if (e2 <= dx)
    {
      err += dx;
      y0 += sy;
    }
  }
}

static void astCirculo(int cx, int cy, int r, uint8_t c)
{
  int x = r, y = 0, d = 1 - r;
  while (x >= y)
  {
    astPix(cx + x, cy + y, c);
    astPix(cx - x, cy + y, c);
    astPix(cx + x, cy - y, c);
    astPix(cx - x, cy - y, c);
    astPix(cx + y, cy + x, c);
    astPix(cx - y, cy + x, c);
    astPix(cx + y, cy - x, c);
    astPix(cx - y, cy - x, c);
    y++;
    if (d < 0)
      d += 2 * y + 1;
    else
    {
      x--;
      d += 2 * (y - x) + 1;
    }
  }
}

// copias del dibujo que hacen falta cuando algo se sale por un borde y entra por el contrario
static int astOffs(float x, float y, float R, float *ox, float *oy)
{
  float xs[2] = {0, 0}, ys[2] = {0, 0};
  int nx = 1, ny = 1, n = 0;
  if (x - R < 0)
    xs[nx++] = AST_W;
  else if (x + R >= AST_W)
    xs[nx++] = -AST_W;
  if (y - R < 0)
    ys[ny++] = AST_H;
  else if (y + R >= AST_H)
    ys[ny++] = -AST_H;
  for (int a = 0; a < nx; a++)
    for (int b = 0; b < ny; b++)
    {
      ox[n] = xs[a];
      oy[n] = ys[b];
      n++;
    }
  return n;
}

static inline float astRadioV(uint8_t seed, int i)
{
  uint32_t h = (uint32_t)(seed + 7) * 2654435761u;
  h ^= (uint32_t)(i + 3) * 40503u;
  h *= 2246822519u;
  h ^= h >> 15;
  return 0.68f + ((h >> 4) & 255) * (0.40f / 255.0f);
}

static void astDibRoca(const AstRoca &r, uint8_t col)
{
  float R = AST_RAD[r.tam];
  float lx[AST_NV], ly[AST_NV], ox[4], oy[4];
  for (int i = 0; i < AST_NV; i++)
  {
    float a = r.ang + i * (2.0f * AST_PI / AST_NV);
    float rr = R * astRadioV(r.seed, i);
    lx[i] = cosf(a) * rr;
    ly[i] = sinf(a) * rr;
  }
  int n = astOffs(r.x, r.y, R + 1.0f, ox, oy);
  for (int k = 0; k < n; k++)
  {
    float cx = r.x + ox[k] + astM->shx, cy = r.y + oy[k] + astM->shy;
    for (int i = 0; i < AST_NV; i++)
    {
      int j = (i + 1) % AST_NV;
      astLinea((int)(cx + lx[i] + 0.5f), (int)(cy + ly[i] + 0.5f), (int)(cx + lx[j] + 0.5f), (int)(cy + ly[j] + 0.5f), col);
    }
  }
}

static void astDibNave(float x, float y, float ang, uint8_t col, bool llama, bool escudo)
{
  static const float P[4][2] = {{9.0f, 0.0f}, {-6.0f, -5.5f}, {-3.0f, 0.0f}, {-6.0f, 5.5f}};
  float ca = cosf(ang), sa = sinf(ang), ox[4], oy[4];
  int n = astOffs(x, y, 14.0f, ox, oy);
  for (int k = 0; k < n; k++)
  {
    float cx = x + ox[k] + astM->shx, cy = y + oy[k] + astM->shy;
    int X[4], Y[4];
    for (int i = 0; i < 4; i++)
    {
      X[i] = (int)(cx + P[i][0] * ca - P[i][1] * sa + 0.5f);
      Y[i] = (int)(cy + P[i][0] * sa + P[i][1] * ca + 0.5f);
    }
    for (int i = 0; i < 4; i++)
    {
      int j = (i + 1) & 3;
      astLinea(X[i], Y[i], X[j], Y[j], col);
    }
    if (llama)
    {
      float L = 5.0f + astRnd(8), s = (astRf() - 0.5f) * 3.0f;
      float fx = -3.0f - L, fy = s;
      astLinea((int)(cx - 3.0f * ca + 0.5f), (int)(cy - 3.0f * sa + 0.5f),
               (int)(cx + fx * ca - fy * sa + 0.5f), (int)(cy + fx * sa + fy * ca + 0.5f), 8);
      fx = -3.0f - L * 0.55f;
      fy = s * 0.4f;
      astLinea((int)(cx - 3.0f * ca + 0.5f), (int)(cy - 3.0f * sa + 0.5f),
               (int)(cx + fx * ca - fy * sa + 0.5f), (int)(cy + fx * sa + fy * ca + 0.5f), 7);
    }
    if (escudo)
      astCirculo((int)(cx + 0.5f), (int)(cy + 0.5f), 11 + (astM->frame & 3), 14);
  }
}

// Esta placa no tiene LEDs: el motor sigue llamando a astLed pero no hace nada
static void astLed(uint8_t prio, bool r, bool g, bool b, uint16_t ms, uint32_t now)
{
  (void)prio;
  (void)r;
  (void)g;
  (void)b;
  (void)ms;
  (void)now;
}

static void astChispas(float x, float y, int n, float vel, uint8_t tipo)
{
  AstMem &m = *astM;
  for (int k = 0; k < n; k++)
  {
    int idx = -1;
    for (int i = 0; i < AST_MAXP; i++)
    {
      uint8_t j = (m.ipt + i) % AST_MAXP;
      if (!m.pt[j].vida)
      {
        idx = j;
        m.ipt = (j + 1) % AST_MAXP;
        break;
      }
    }
    if (idx < 0)
      return;
    float a = astRf() * 2.0f * AST_PI, v = vel * (0.25f + astRf());
    AstPart &p = m.pt[idx];
    p.x = x;
    p.y = y;
    p.vx = cosf(a) * v;
    p.vy = sinf(a) * v;
    p.vida = 14 + astRnd(24);
    p.tipo = tipo;
  }
}

static void astOnda(float x, float y)
{
  AstMem &m = *astM;
  for (int i = 0; i < 3; i++)
    if (!m.on[i].vida)
    {
      m.on[i].x = x;
      m.on[i].y = y;
      m.on[i].r = 2;
      m.on[i].vida = 18;
      return;
    }
}

static void astNave()
{
  AstMem &m = *astM;
  m.nx = AST_W / 2;
  m.ny = AST_H / 2;
  m.nvx = m.nvy = 0;
  m.nang = -AST_PI / 2;
  m.ctlRot = 0;
  m.ctlEmp = 0;
  m.empuje = 0;
  m.cadencia = 0;
  m.planCnt = 0;
  m.viva = 1;
  m.inmune = 110;
}

static void astNuevaOla()
{
  AstMem &m = *astM;
  m.ola++;
  int n = 3 + m.ola;
  if (n > 10)
    n = 10;
  float sf = 1.0f + 0.10f * m.ola;
  if (sf > 2.2f)
    sf = 2.2f;
  for (int k = 0; k < n; k++)
  {
    int idx = -1;
    for (int i = 0; i < AST_MAXA; i++)
      if (!m.ro[i].tam)
      {
        idx = i;
        break;
      }
    if (idx < 0)
      break;
    AstRoca &r = m.ro[idx];
    for (int t = 0; t < 24; t++)
    {
      r.x = astRnd(AST_W);
      r.y = astRnd(AST_H);
      float dx = astDel(r.x - m.nx, AST_W), dy = astDel(r.y - m.ny, AST_H);
      if (dx * dx + dy * dy > 3600.0f)
        break;
    }
    float a = astRf() * 2.0f * AST_PI, v = (0.35f + astRf() * 0.4f) * sf;
    r.vx = cosf(a) * v;
    r.vy = sinf(a) * v;
    r.ang = astRf() * 6.28f;
    r.vang = (astRf() - 0.5f) * 0.08f;
    r.tam = 3;
    r.seed = astRnd(256);
    r.flash = 0;
  }
  astOnda(m.nx, m.ny);
}

static void astNuevaPartida()
{
  AstMem &m = *astM;
  memset(m.ro, 0, sizeof(m.ro));
  memset(m.ba, 0, sizeof(m.ba));
  memset(m.pt, 0, sizeof(m.pt));
  memset(m.on, 0, sizeof(m.on));
  m.puntos = 0;
  m.ola = 0;
  m.vidas = 3;
  m.pausaOla = 0;
  m.finTimer = 0;
  m.reaparece = 0;
  astNave();
  astNuevaOla();
}

static void astInit()
{
  AstMem &m = *astM;
  memset(m.fb, 0, sizeof(m.fb));
  m.mejor = 0;
  m.planCada = 4;
  m.frame = 0;
  m.ledOff = 0;
  m.ledPrio = 0;
  m.shk = m.shx = m.shy = 0;
  m.tIzq[0] = m.tCen[0] = m.tDer[0] = 0;
  for (int i = 0; i < AST_NSTAR; i++)
  {
    m.es[i].x = astRnd(AST_W);
    m.es[i].y = astRnd(AST_H);
    m.es[i].capa = i % 3;
  }
  astNuevaPartida();
}

// rompe la roca i: puntos, chispas y los dos trozos en que se parte
static void astRomper(int i, uint32_t now)
{
  AstMem &m = *astM;
  AstRoca r = m.ro[i];
  m.ro[i].tam = 0;
  m.puntos += AST_PTS[r.tam];
  if (m.puntos > m.mejor)
    m.mejor = m.puntos;
  astChispas(r.x, r.y, 6 + r.tam * 6, 1.0f + r.tam * 0.5f, 0);
  astChispas(r.x, r.y, 3 + r.tam * 2, 1.6f, 1);
  if (r.tam == 3)
  {
    astOnda(r.x, r.y);
    if (m.shk < 3)
      m.shk = 3;
  }
  astLed(2, false, true, false, 40, now);
  if (r.tam > 1)
  {
    for (int h = 0; h < 2; h++)
    {
      int idx = -1;
      for (int k = 0; k < AST_MAXA; k++)
        if (!m.ro[k].tam)
        {
          idx = k;
          break;
        }
      if (idx < 0)
        return;
      AstRoca &c = m.ro[idx];
      float a = astRf() * 2.0f * AST_PI, v = 0.5f + astRf() * 0.7f;
      c.x = r.x;
      c.y = r.y;
      c.vx = r.vx * 0.5f + cosf(a) * v;
      c.vy = r.vy * 0.5f + sinf(a) * v;
      c.ang = astRf() * 6.28f;
      c.vang = (astRf() - 0.5f) * 0.14f;
      c.tam = r.tam - 1;
      c.seed = astRnd(256);
      c.flash = 6;
    }
  }
}

static void astMuereNave(uint32_t now)
{
  AstMem &m = *astM;
  m.viva = 0;
  m.empuje = 0;
  astChispas(m.nx, m.ny, 22, 2.6f, 2);
  astChispas(m.nx, m.ny, 26, 2.2f, 0);
  astChispas(m.nx, m.ny, 14, 3.0f, 1);
  astOnda(m.nx, m.ny);
  m.shk = 7;
  astLed(3, true, true, true, 140, now);
  if (m.vidas)
    m.vidas--;
  if (m.vidas)
    m.reaparece = 60;
  else
    m.finTimer = 110;
}

// simula un futuro de la nave: dos maniobras seguidas y luego dejarse llevar. Cuanto mayor, mejor
static float astSimular(const AstCtx &C, int r1, int e1, int h1, int r2, int e2, int h2)
{
  float x = C.nx, y = C.ny, vx = C.nvx, vy = C.nvy, hx = C.hx, hy = C.hy;
  float px[AST_MAXA], py[AST_MAXA], sc = 0.0f;
  const int n = C.n;
  for (int i = 0; i < n; i++)
  {
    px[i] = C.x[i];
    py[i] = C.y[i];
  }
  for (int t = 1; t <= AST_HORIZ; t++)
  {
    int rot, emp;
    if (t <= h1)
    {
      rot = r1;
      emp = e1;
    }
    else if (t <= h1 + h2)
    {
      rot = r2;
      emp = e2;
    }
    else
    {
      rot = 0;
      emp = 0;
    }
    if (rot)
    {
      float nhx = hx * C.cs - rot * C.sn * hy;
      float nhy = hy * C.cs + rot * C.sn * hx;
      hx = nhx;
      hy = nhy;
    }
    if (emp)
    {
      vx += hx * AST_EMPUJE;
      vy += hy * AST_EMPUJE;
    }
    vx *= AST_ROZA;
    vy *= AST_ROZA;
    float v2 = vx * vx + vy * vy;
    if (v2 > AST_VMAX * AST_VMAX)
    {
      float k = AST_VMAX / sqrtf(v2);
      vx *= k;
      vy *= k;
    }
    x += vx;
    y += vy;
    if (x < 0)
      x += AST_W;
    else if (x >= AST_W)
      x -= AST_W;
    if (y < 0)
      y += AST_H;
    else if (y >= AST_H)
      y -= AST_H;
    const bool mirar = (t % 3) == 0;
    for (int i = 0; i < n; i++)
    {
      float ax = px[i] + C.vx[i], ay = py[i] + C.vy[i];
      if (ax < 0)
        ax += AST_W;
      else if (ax >= AST_W)
        ax -= AST_W;
      if (ay < 0)
        ay += AST_H;
      else if (ay >= AST_H)
        ay -= AST_H;
      px[i] = ax;
      py[i] = ay;
      float dx = ax - x, dy = ay - y;
      if (dx > AST_W * 0.5f)
        dx -= AST_W;
      else if (dx < -AST_W * 0.5f)
        dx += AST_W;
      if (dy > AST_H * 0.5f)
        dy -= AST_H;
      else if (dy < -AST_H * 0.5f)
        dy += AST_H;
      float d2 = dx * dx + dy * dy;
      if (d2 < C.lim2[i])
        return sc - 1000.0f - 25.0f * (AST_HORIZ - t);
      if (d2 < C.cer2[i])
        sc -= (C.cer2[i] - d2) * 0.01f;
      if (mirar && d2 < AST_RANGO2)
      {
        float tau = (fabsf(dx) + fabsf(dy)) * 0.12f;
        float lx = dx + (C.vx[i] - vx) * tau, ly = dy + (C.vy[i] - vy) * tau;
        if (hx * lx + hy * ly > 0)
        {
          float cr = hx * ly - hy * lx;
          if (cr * cr < C.aim2[i])
            sc += C.val[i] * 3.0f * (1.0f - d2 * (1.0f / AST_RANGO2));
        }
      }
    }
  }
  sc -= (vx * vx + vy * vy) * 0.5f;
  return sc;
}

static void astPlanear()
{
  AstMem &m = *astM;
  AstCtx C;
  C.n = 0;
  for (int i = 0; i < AST_MAXA; i++)
  {
    const AstRoca &r = m.ro[i];
    if (!r.tam)
      continue;
    int k = C.n++;
    float R = AST_RAD[r.tam];
    C.x[k] = r.x;
    C.y[k] = r.y;
    C.vx[k] = r.vx;
    C.vy[k] = r.vy;
    float l = R * 0.85f + 5.0f + 0.5f;
    C.lim2[k] = l * l;
    l += 10.0f;
    C.cer2[k] = l * l;
    l = R * 0.9f + 1.5f;
    C.aim2[k] = l * l;
    C.val[k] = AST_VAL[r.tam];
  }
  if (!C.n)
  {
    m.ctlRot = 0;
    m.ctlEmp = 0;
    return;
  }
  C.nx = m.nx;
  C.ny = m.ny;
  C.nvx = m.nvx;
  C.nvy = m.nvy;
  C.hx = cosf(m.nang);
  C.hy = sinf(m.nang);
  C.cs = cosf(AST_ROT);
  C.sn = sinf(AST_ROT);
  static const int8_t RR[3] = {-1, 0, 1};
  static const uint8_t HH[2] = {3, 9};
  float mejor = -1e30f;
  int mr = 0, me = 0;
  for (int r1 = 0; r1 < 3; r1++)
    for (int e1 = 0; e1 < 2; e1++)
      for (int a = 0; a < 2; a++)
        for (int r2 = 0; r2 < 3; r2++)
          for (int e2 = 0; e2 < 2; e2++)
            for (int b = 0; b < 2; b++)
            {
              float s = astSimular(C, RR[r1], e1, HH[a], RR[r2], e2, HH[b]);
              if (s > mejor)
              {
                mejor = s;
                mr = RR[r1];
                me = e1;
              }
            }
  m.ctlRot = mr;
  m.ctlEmp = me;
}

// dispara si la bala, sumando la velocidad de la nave, va a pillar a alguna roca antes de que se apague
static bool astQuieroDisparar()
{
  AstMem &m = *astM;
  float hx = cosf(m.nang), hy = sinf(m.nang);
  const float B2 = AST_BVEL * AST_BVEL;
  for (int i = 0; i < AST_MAXA; i++)
  {
    const AstRoca &r = m.ro[i];
    if (!r.tam)
      continue;
    float dx = astDel(r.x - m.nx, AST_W), dy = astDel(r.y - m.ny, AST_H);
    float ux = r.vx - m.nvx, uy = r.vy - m.nvy;
    float A = ux * ux + uy * uy - B2, Bq = dx * ux + dy * uy, Cq = dx * dx + dy * dy;
    float t;
    if (fabsf(A) < 1e-4f)
    {
      if (fabsf(Bq) < 1e-6f)
        continue;
      t = -Cq / (2.0f * Bq);
    }
    else
    {
      float disc = Bq * Bq - A * Cq;
      if (disc < 0)
        continue;
      float sq = sqrtf(disc);
      float t1 = (-Bq - sq) / A, t2 = (-Bq + sq) / A;
      t = (t1 > 0 && (t1 < t2 || t2 <= 0)) ? t1 : t2;
    }
    if (t <= 0 || t > AST_BVIDA - 2)
      continue;
    float ix = dx + ux * t, iy = dy + uy * t;
    if (hx * ix + hy * iy <= 0)
      continue;
    float cr = hx * iy - hy * ix, R = AST_RAD[r.tam] * 0.8f + 1.0f;
    if (cr * cr < R * R)
      return true;
  }
  return false;
}

static void astActualizar(uint32_t now)
{
  AstMem &m = *astM;

  // ---- La nave ----
  if (m.viva)
  {
    if (m.planCnt == 0)
    {
      uint32_t t0 = micros();
      astPlanear();
      uint32_t dt = micros() - t0;
      uint32_t c = dt / 4000UL;
      m.planCada = c < 3 ? 3 : (c > 8 ? 8 : c);
      m.planCnt = m.planCada;
    }
    m.planCnt--;
    m.nang += m.ctlRot * AST_ROT;
    m.empuje = m.ctlEmp;
    if (m.empuje)
    {
      m.nvx += cosf(m.nang) * AST_EMPUJE;
      m.nvy += sinf(m.nang) * AST_EMPUJE;
    }
    m.nvx *= AST_ROZA;
    m.nvy *= AST_ROZA;
    float v2 = m.nvx * m.nvx + m.nvy * m.nvy;
    if (v2 > AST_VMAX * AST_VMAX)
    {
      float k = AST_VMAX / sqrtf(v2);
      m.nvx *= k;
      m.nvy *= k;
    }
    m.nx += m.nvx;
    m.ny += m.nvy;
    if (m.nx < 0)
      m.nx += AST_W;
    else if (m.nx >= AST_W)
      m.nx -= AST_W;
    if (m.ny < 0)
      m.ny += AST_H;
    else if (m.ny >= AST_H)
      m.ny -= AST_H;
    if (m.inmune)
      m.inmune--;
    if (m.cadencia)
      m.cadencia--;
    else if (astQuieroDisparar())
    {
      for (int i = 0; i < AST_MAXB; i++)
        if (!m.ba[i].vida)
        {
          float hx = cosf(m.nang), hy = sinf(m.nang);
          m.ba[i].x = m.nx + hx * 9.0f;
          m.ba[i].y = m.ny + hy * 9.0f;
          m.ba[i].vx = m.nvx + hx * AST_BVEL;
          m.ba[i].vy = m.nvy + hy * AST_BVEL;
          m.ba[i].vida = AST_BVIDA;
          m.cadencia = 7;
          astLed(1, true, false, false, 25, now);
          break;
        }
    }
  }
  else
  {
    if (m.vidas)
    {
      if (m.reaparece)
        m.reaparece--;
      if (!m.reaparece)
      {
        bool libre = true;
        for (int i = 0; i < AST_MAXA; i++)
          if (m.ro[i].tam)
          {
            float dx = astDel(m.ro[i].x - AST_W / 2, AST_W), dy = astDel(m.ro[i].y - AST_H / 2, AST_H);
            float l = AST_RAD[m.ro[i].tam] + 38.0f;
            if (dx * dx + dy * dy < l * l)
              libre = false;
          }
        if (libre)
          astNave();
        else
          m.reaparece = 4;
      }
    }
    else if (m.finTimer)
    {
      if (--m.finTimer == 0)
        astNuevaPartida();
    }
  }

  // ---- Balas ----
  for (int i = 0; i < AST_MAXB; i++)
  {
    AstBala &b = m.ba[i];
    if (!b.vida)
      continue;
    b.vida--;
    b.x += b.vx;
    b.y += b.vy;
    if (b.x < 0)
      b.x += AST_W;
    else if (b.x >= AST_W)
      b.x -= AST_W;
    if (b.y < 0)
      b.y += AST_H;
    else if (b.y >= AST_H)
      b.y -= AST_H;
    for (int k = 0; k < AST_MAXA; k++)
    {
      if (!m.ro[k].tam)
        continue;
      float dx = astDel(m.ro[k].x - b.x, AST_W), dy = astDel(m.ro[k].y - b.y, AST_H);
      float l = AST_RAD[m.ro[k].tam] * 0.9f + 1.5f;
      if (dx * dx + dy * dy < l * l)
      {
        b.vida = 0;
        astRomper(k, now);
        break;
      }
    }
  }

  // ---- Rocas ----
  int vivas = 0;
  for (int i = 0; i < AST_MAXA; i++)
  {
    AstRoca &r = m.ro[i];
    if (!r.tam)
      continue;
    vivas++;
    r.x += r.vx;
    r.y += r.vy;
    r.ang += r.vang;
    if (r.flash)
      r.flash--;
    if (r.x < 0)
      r.x += AST_W;
    else if (r.x >= AST_W)
      r.x -= AST_W;
    if (r.y < 0)
      r.y += AST_H;
    else if (r.y >= AST_H)
      r.y -= AST_H;
    if (m.viva && !m.inmune)
    {
      float dx = astDel(r.x - m.nx, AST_W), dy = astDel(r.y - m.ny, AST_H);
      float l = AST_RAD[r.tam] * 0.82f + 4.0f;
      if (dx * dx + dy * dy < l * l)
        astMuereNave(now);
    }
  }
  if (!vivas)
  {
    if (!m.pausaOla)
      m.pausaOla = 45;
    else if (--m.pausaOla == 0)
      astNuevaOla();
  }

  // ---- Partículas, ondas y estrellas ----
  for (int i = 0; i < AST_MAXP; i++)
  {
    AstPart &p = m.pt[i];
    if (!p.vida)
      continue;
    p.vida--;
    p.x += p.vx;
    p.y += p.vy;
    p.vx *= 0.96f;
    p.vy *= 0.96f;
  }
  for (int i = 0; i < 3; i++)
    if (m.on[i].vida)
    {
      m.on[i].vida--;
      m.on[i].r += 3;
    }
  for (int i = 0; i < AST_NSTAR; i++)
  {
    static const float F[3] = {0.10f, 0.25f, 0.50f};
    AstEstr &e = m.es[i];
    e.x -= m.nvx * F[e.capa];
    e.y -= m.nvy * F[e.capa];
    if (e.x < 0)
      e.x += AST_W;
    else if (e.x >= AST_W)
      e.x -= AST_W;
    if (e.y < 0)
      e.y += AST_H;
    else if (e.y >= AST_H)
      e.y -= AST_H;
  }
}

static void astDibujar()
{
  AstMem &m = *astM;
  uint8_t *f = m.fb;
  for (int i = 0; i < AST_W * AST_H / 2; i++)
  {
    uint8_t b = f[i];
    f[i] = (AST_DEC[b >> 4] << 4) | AST_DEC[b & 15];
  }
  for (int i = 0; i < AST_NSTAR; i++)
  {
    const AstEstr &e = m.es[i];
    uint8_t c = e.capa == 0 ? 1 : 2;
    if (astRnd(60) == 0)
      c = 3;
    astPix((int)e.x + m.shx, (int)e.y + m.shy, c);
  }
  for (int i = 0; i < AST_MAXA; i++)
    if (m.ro[i].tam)
      astDibRoca(m.ro[i], m.ro[i].flash ? 3 : (m.ro[i].tam == 1 ? 3 : 4));
  for (int i = 0; i < AST_MAXB; i++)
    if (m.ba[i].vida)
    {
      const AstBala &b = m.ba[i];
      astLinea((int)b.x + m.shx, (int)b.y + m.shy, (int)(b.x - b.vx * 0.7f) + m.shx, (int)(b.y - b.vy * 0.7f) + m.shy, 14);
    }
  for (int i = 0; i < AST_MAXP; i++)
    if (m.pt[i].vida)
    {
      const AstPart &p = m.pt[i];
      uint8_t v = p.vida, c;
      if (p.tipo == 0)
        c = v > 24 ? 7 : (v > 14 ? 8 : (v > 6 ? 9 : 10));
      else if (p.tipo == 1)
        c = v > 20 ? 3 : (v > 12 ? 4 : (v > 5 ? 5 : 6));
      else
        c = v > 18 ? 11 : (v > 8 ? 12 : 13);
      int px = (int)p.x + m.shx, py = (int)p.y + m.shy;
      astPix(px, py, c);
      if (v > 16)
      {
        astPix(px + 1, py, c);
        astPix(px, py + 1, c);
      }
    }
  for (int i = 0; i < 3; i++)
    if (m.on[i].vida)
    {
      uint8_t v = m.on[i].vida;
      astCirculo((int)m.on[i].x + m.shx, (int)m.on[i].y + m.shy, m.on[i].r, v > 12 ? 3 : (v > 7 ? 4 : (v > 3 ? 5 : 6)));
    }
  if (m.viva)
  {
    bool visible = !m.inmune || (m.frame & 2);
    if (visible)
      astDibNave(m.nx, m.ny, m.nang, m.inmune ? 3 : 11, m.empuje, m.inmune > 0);
  }
}

static void astEmpujar()
{
  AstMem &m = *astM;
  tft.startWrite();
  tft.setAddrWindow(0, AST_Y0, AST_W, AST_H);
  for (int y = 0; y < AST_H; y++)
  {
    const uint8_t *s = &m.fb[y * (AST_W / 2)];
    uint16_t *d = m.linea;
    for (int x = 0; x < AST_W / 2; x++)
    {
      uint8_t b = s[x];
      d[0] = AST_PAL[b & 15];
      d[1] = AST_PAL[b >> 4];
      d += 2;
    }
    tft.pushColors(m.linea, AST_W, true);
  }
  tft.endWrite();
}

static void astFrame(uint32_t now)
{
  AstMem &m = *astM;
  m.frame++;
  if (m.shk > 0)
  {
    m.shx = (int8_t)astRnd(2 * m.shk + 1) - m.shk;
    m.shy = (int8_t)astRnd(2 * m.shk + 1) - m.shk;
    if ((m.frame & 1) == 0)
      m.shk--;
  }
  else
    m.shx = m.shy = 0;
  astActualizar(now);
  astDibujar();
  astEmpujar();
}

// los tres textos de abajo, solo se repintan si cambian
static void astTextos(bool forzar)
{
  AstMem &m = *astM;
  char izq[16], cen[12], der[12];
  snprintf(izq, sizeof(izq), "Pts %lu", (unsigned long)m.puntos);
  snprintf(cen, sizeof(cen), "Ola %u", (unsigned)m.ola);
  snprintf(der, sizeof(der), "Vidas %u", (unsigned)m.vidas);
  bool a = forzar || strcmp(izq, m.tIzq) != 0, b = forzar || strcmp(cen, m.tCen) != 0, c = forzar || strcmp(der, m.tDer) != 0;
  if (!a && !b && !c)
    return;
  tft.setFreeFont(NULL);
  tft.setTextFont(1);
  tft.setTextSize(2);
  if (a)
  {
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.setTextPadding(120);
    tft.setTextDatum(TL_DATUM);
    tft.drawString(izq, 4, 151);
    strcpy(m.tIzq, izq);
  }
  if (b)
  {
    tft.setTextColor(TFT_CYAN, TFT_BLACK);
    tft.setTextPadding(72);
    tft.setTextDatum(TC_DATUM);
    tft.drawString(cen, 160, 151);
    strcpy(m.tCen, cen);
  }
  if (c)
  {
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.setTextPadding(110);
    tft.setTextDatum(TR_DATUM);
    tft.drawString(der, 316, 151);
    strcpy(m.tDer, der);
  }
  tft.setTextPadding(0);
  tft.setTextDatum(TL_DATUM);
}
// <<< ASTEROIDES MOTOR

// Hora arriba a la izquierda, hashrate arriba a la derecha ( igual que Tetris, Ajedrez, Primos y Galaxian )
static void astDrawHud(const char *hash, const char *hora)
{
  tft.setFreeFont(NULL);
  tft.setTextFont(1);
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextPadding(100);
  tft.setTextDatum(TL_DATUM);
  tft.drawString(hora, 4, 9);
  char b[24];
  snprintf(b, sizeof(b), "%s KH/s", hash);
  tft.setTextPadding(150);
  tft.setTextDatum(TR_DATUM);
  tft.drawString(b, 316, 9);
  tft.setTextPadding(0);
  tft.setTextDatum(TL_DATUM);
}

void tDisplay_Asteroides(unsigned long mElapsed)
{
  mineria = getMiningData(mElapsed);
  Serial.printf("M8AX - >>> Completados %s Share(s), %s Khashes, Prom. Hashrate %s KH/s %s°\n",
                mineria.completedShares.c_str(), mineria.totalKHashes.c_str(), mineria.currentHashRate.c_str(), mineria.temp.c_str());

  // Si hace rato que no nos llaman es que se ha visto otra pantalla: borrar antes de pintar
  bool limpiar = (millis() - astUlt > 2500);
  if (!astM)
  {
    astM = (AstMem *)malloc(sizeof(AstMem));
    if (!astM)
    {
      Serial.printf("M8AX - ### Asteroides: Sin RAM ( Libre %u, Mayor Bloque %u ) ###\n", (unsigned)ESP.getFreeHeap(), (unsigned)ESP.getMaxAllocHeap());
      delay(1000);
      return;
    }
    astInit();
    limpiar = true;
  }
  const int astMiPantalla = currentDisplayDriver->current_cyclic_screen;
  if (limpiar)
  {
    tft.fillRect(0, 0, 320, 170, TFT_BLACK);
    astEmpujar();
    astTextos(true);
  }

  char horaStr[9] = "";
  char lastHora[9] = "";
  char hashStr[16];
  char lastHash[16] = "";

  unsigned long start = millis();
  unsigned long lastMiningUpdate = 0;
  unsigned long siguiente = start;

  while (millis() - start < AST_SLICE_MS)
  {
    unsigned long now = millis();

    if (now >= siguiente)
    {
      astFrame(now);
      astTextos(false);
      siguiente = millis() + AST_FRAME_MS;
    }

    // ---- Hora ( izquierda ) y hashrate ( derecha ), arriba como en las demás ----
    if (now - lastMiningUpdate >= 1000)
    {
      lastMiningUpdate = now;
      mineria = getMiningData(mElapsed);
      snprintf(hashStr, sizeof(hashStr), "%s", mineria.currentHashRate.c_str());
      time_t epoch = (time_t)timeClient.getEpochTime();
      struct tm *ti = localtime(&epoch);
      snprintf(horaStr, sizeof(horaStr), "%02d:%02d:%02d", ti->tm_hour, ti->tm_min, ti->tm_sec);
      if (strcmp(horaStr, lastHora) != 0 || strcmp(hashStr, lastHash) != 0)
      {
        astDrawHud(hashStr, horaStr);
        strcpy(lastHora, horaStr);
        strcpy(lastHash, hashStr);
      }
    }
    delay(2);
  }

  tft.setTextPadding(0);
  tft.setTextDatum(TL_DATUM);
  astUlt = millis();

  // Si el driver ya ha cambiado de pantalla, se devuelve la RAM
  if (currentDisplayDriver->current_cyclic_screen != astMiPantalla)
  {
    free(astM);
    astM = NULL;
  }
}

/**
 * Analiza y actualiza datos cada segundo.
 *
 * Esta función realiza múltiples tareas en cada ejecución:
 *
 * 1. Obtiene la hora actual en formato UNIX (epoch) y la convierte a una estructura de tiempo local.
 * 2. Si el sistema ya ha arrancado (startTime > 0), actualiza los valores máximos y mínimos de:
 *    - Hashrate (KH/s)
 *    - Temperatura del minero
 *    - Cuenta las veces que la CPU pasa de los 70°C
 * 3. Ejecuta una animación de nieve en pantalla si es Navidad (20 de diciembre - 6 de enero).
 * 4. Muestra mensajes motivacionales en la pantalla en ciertos momentos aleatorios.
 * 5. Inicializa los datos del bot de Telegram al inicio y registra la hora de arranque.
 * 6. Envía un mensaje a Telegram cada 2 horas si ya pasó el tiempo mínimo de arranque.
 *
 * La función también controla la variable `ContadorEspecial` y reinicia algunos parámetros
 * cuando se activan los eventos especiales (Navidad, motivación).
 *
 * Si la temperatura de la CPU sobrepasa los 80 grados, la CPU entrará en modo deep sleep
 * durante 10 minutos, minutos en los cuales la CPU bajará la temperatura. Pasados
 * los 10 minutos, el NerdMinerV2 rearrancará solo y comenzará a minar con la CPU Más Fría... ( VIGILAR )
 *
 * Miramos tambien si hay que cambiar a horario de verano o no.
 *
 * @param frame Número de fotograma actual (usado en ciertas animaciones o cálculos).
 */

void analiCadaSegundo(unsigned long frame)
{
  unsigned long epochTime = timeClient.getEpochTime();
  struct tm *t = localtime((time_t *)&epochTime);
  int dia = t->tm_mday;
  int mes = t->tm_mon + 1;
  int anio = t->tm_year + 1900;
  int horita = t->tm_hour;
  int minutitos = t->tm_min;
  int segundos = t->tm_sec;

  if (nbd && millis() - nbdUlt > 5000)
  {
    free(nbd);
    nbd = NULL;
  }

  if (galM && millis() - galUlt > 5000)
  {
    free(galM);
    galM = NULL;
  }

  if (astM && millis() - astUlt > 5000)
  {
    free(astM);
    astM = NULL;
  }

  if (startTime > 0 && uncontadormas > 50)
  {
    int temp = mineria.temp.toInt();
    float hashRate = mineria.currentHashRate.toFloat();

    if (hashRate > maxkh)
    {
      maxkh = hashRate;
    }

    if (hashRate < minkh)
    {
      minkh = hashRate;
    }

    if (temp > maxtemp)
    {
      maxtemp = temp;
    }

    if (temp < mintemp)
    {
      mintemp = temp;
    }

    if (ContadorEspecial % 300 == 0 && temp > 70)
    {
      Serial.println("M8AX - Temperatura De CPU, Ha Superado Los 70°C...");
      alertatemp++;
    }

    if (ContadorEspecial % 30 == 0 && temp > 80)
    {
      Serial.println("M8AX - ¡ Temperatura Muy Alta ! 80°C Superados. Entrando En Deep Sleep Por 10 Minutos, Para Enfriar La CPU...");
      esp_sleep_enable_timer_wakeup(600e6);
      esp_deep_sleep_start();
    }
  }

  // Felicitar La Navidad O El Año Nuevo

  if (anio != 1970 && ((mes == 12 && dia >= 20) || (mes == 1 && dia <= 6)))
  {
    if (minutitos == 30 && (horita % 2 == 0) &&
        ((horita >= 8 && horita <= 15) || (horita >= 19 && horita <= 23) || (horita <= 2)) &&
        segundos == 0 && dia % 2 == 0)
    {
      if (mes == 12)
      {
        nevar();
        Serial.println("M8AX - Felicitando La Navidad...");
      }
      else if (mes == 1)
      {
        nevar2();
        Serial.println("M8AX - Felicitando El Año Nuevo...");
      }
      ContadorEspecial = 0;
    }
  }

  if (startTime == 0)
  {
    BOT_TOKEN = Settings.botTelegram;
    CHAT_ID = Settings.ChanelIDTelegram;
    startTime = (epochTime > 1700000000UL && epochTime < 4102444800UL) ? epochTime : 0;
    diadecambios = dia;
    anterBTC = monedilla.btcPrice.toFloat();
  }
  else
  {
    if (segundos == 0 && !mensajeEnviado)
    {
      char buffer[50];
      sprintf(buffer, "M8AX - %02d/%02d/%d %02d:%02d:%02d - M8AX",
              dia, mes, anio, horita, minutitos, segundos);
      Serial.println(buffer);
      mensajeEnviado = true;
      if (millis() - arrankito <= 300000)
      {
        ajustarZonaHoraria();
      }
    }
    else if (segundos != 0)
    {
      mensajeEnviado = false;
    }
  }

  if (anterBTC <= 0.0)
  {
    anterBTC = monedilla.btcPrice.toFloat();
  }
  if (dia != diadecambios)
  {
    diadecambios = dia;
    float variacion = (anterBTC > 0) ? ((monedilla.btcPrice.toFloat() - anterBTC) / anterBTC) * 100 : 0;
    subebaja = (anterBTC > 0 && monedilla.btcPrice.toFloat() > 0) ? ((variacion >= 0) ? "+" : "-") + String(fabs(variacion), 5) + "%" : ". ERROR .";
    if (monedilla.btcPrice.toFloat() > 0)
      anterBTC = monedilla.btcPrice.toFloat();
  }

  // Si ya ha pasado el tiempo de arranque mínimo (por ejemplo, 10 minutos) y han pasado 2 horas desde el último mensaje de Telegram

  if (startTime > 0 && epochTime - startTime >= minStartupTime && epochTime - lastTelegramEpochTime >= interval)
  {

    // Ajustar la zona horaria solo en meses de cambio de hora y de madrugada

    if ((mes == 3 || mes == 4 || mes == 10 || mes == 11) && horita >= 0 && horita <= 5)
      ajustarZonaHoraria();

    // Verificar si los datos de Telegram están configurados

    static int fallosSeguidos = 0;
    bool enviado = true;
    if (BOT_TOKEN != "NO CONFIGURADO" && CHAT_ID != "NO CONFIGURADO")
    {
      enviado = recopilaTelegram(); // Envia el mensaje a Telegram
    }
    if (enviado)
    {
      fallosSeguidos = 0;
      lastTelegramEpochTime = epochTime; // Actualiza el tiempo de la última ejecución
    }
    else if (++fallosSeguidos < 4)
    {
      lastTelegramEpochTime = epochTime - interval + 300; // falló: reintenta en 5 minutos (hasta 3 veces)
    }
    else
    {
      fallosSeguidos = 0;
      lastTelegramEpochTime = epochTime; // 4 fallos seguidos: espera las 2 horas normales
    }
  }

  // Dando Esperanzas Al Dueño Del NerdminerV2: Lo Conseguirás... Ostia!!! xD

  rndnumero = esp_random();
  if (rndnumero <= 10031977 && segundos <= 20 && segundos % 2 == 0 && dia % 2 == 0)
  {
    Serial.printf(">>> M8AX-NerdMinerV2 Dando Ánimos Y Esperanza Al Usuario...\n");
    actualizarcalen = actualizarc = actual = actuanot = 0;
    uncontadormas = 51;
    ContadorEspecial = correccion = 0;
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(colors[colorIndex]);
    tft.setTextSize(3);
    tft.setCursor(0, 0);
    const char *mensaje1 = (rndnumero % 2 == 0) ? "La Esperanza Es\nLo Ultimo Que\nSe Pierde...\nLo Conseguiremos!\n\n     VAMOS !" : "Hope Is The Last\nThing To Lose, We\nWill Archieve It!\n\n    COME ON !";
    tft.println(mensaje1);
    tft.setTextSize(1);
    tft.println("");
    tft.setTextSize(2);
    tft.setTextColor(TFT_WHITE), tft.println("     ... By M8AX ..."), delay(3000);
  }
}

// Función encargada de gestionar la animación de la pantalla actual.
// 1. Incrementa el contador especial (ContadorEspecial) en cada llamada.
// 2. Si el contador es divisible por 5, llama a la función 'analiCadaSegundo' para realizar acciones basadas en el tiempo.
//    Esta función se encarga de verificar el tiempo, felicitar eventos especiales (Navidad/Año Nuevo),
//    mostrar mensajes motivacionales y realizar tareas de Telegram en intervalos específicos de tiempo.
// 3. La animación de la pantalla o las actualizaciones relacionadas con el frame también se manejan dentro de esta función.
//    Sin embargo, el código para las animaciones no se incluye explícitamente en esta función.

void tDisplay_AnimateCurrentScreen(unsigned long frame)
{
  ContadorEspecial++;
  uncontadormas++;
  if (ContadorEspecial % 5 == 0)
  {
    analiCadaSegundo(frame);
  }
}

// Aún sin implementación

void tDisplay_DoLedStuff(unsigned long frame)
{
}

// Definición de la estructura `tDisplayDriver` que encapsula las funciones necesarias para controlar las pantallas y sus interacciones. Esta estructura contiene las siguientes funciones:
// - `tDisplay_Init`: Inicializa la pantalla y otros componentes necesarios.
// - `tDisplay_AlternateScreenState`: Permite alternar entre diferentes estados de la pantalla.
// - `tDisplay_AlternateRotation`: Maneja la rotación de la pantalla.
// - `tDisplay_LoadingScreen`: Muestra la pantalla de carga con efectos aleatorios.
// - `tDisplay_SetupScreen`: Muestra la pantalla de configuración.
// - `tDisplayCyclicScreens`: Array de funciones cíclicas que se muestran de manera periódica, permitiendo la transición entre pantallas, como la pantalla del minero, cotización de BTC, hora, etc.
// - `tDisplay_AnimateCurrentScreen`: Actualiza las pantallas de forma cíclica y con animaciones, como la del minero o las cotizaciones.
// - `tDisplay_DoLedStuff`: Función vacía en este momento, reservada para gestionar efectos relacionados con LEDs.
// Además, contiene detalles de la pantalla, como el tamaño y el número total de pantallas definidas en `tDisplayCyclicScreens` (calculado con `SCREENS_ARRAY_SIZE`). La estructura `tDisplayDriver` centraliza el control de todas las operaciones relacionadas con la pantalla, permitiendo la ejecución secuencial o cíclica de diferentes pantallas en el dispositivo.

CyclicScreenFunction tDisplayCyclicScreens[] = {tDisplay_MinerScreen, tDisplay_GlobalHashScreen, tDisplay_BTCprice, tDisplay_m8axScreen15, tDisplay_m8axScreen2, datoTextPlano, tDisplay_m8axScreen5, tDisplay_m8axScreen16, tDisplay_ClockScreen, tDisplay_m8axScreen1, tDisplay_m8axScreen7, RelojDeNumeros, tDisplay_m8axScreen9, tDisplay_m8axScreen10, tDisplay_m8axScreen11, tDisplay_m8axScreen18, tDisplay_m8axScreen3, tDisplay_m8axScreen13, tDisplay_m8axScreen14, tDisplay_m8axScreen8, monedaYdado, tDisplay_Primos, tDisplay_Tetris, tDisplay_Ajedrez, tDisplay_Galaxian, tDisplay_Asteroides, tDisplay_Galaxias, tDisplay_Fractal, tDisplay_m8axvida, tDisplay_m8axScreen4, tDisplay_m8axScreen6, tDisplay_m8axScreen17, tDisplay_m8axScreen12};

DisplayDriver tDisplayDriver = {
    tDisplay_Init,
    tDisplay_AlternateScreenState,
    tDisplay_AlternateRotation,
    tDisplay_LoadingScreen,
    tDisplay_SetupScreen,
    tDisplayCyclicScreens,
    tDisplay_AnimateCurrentScreen,
    tDisplay_DoLedStuff,
    SCREENS_ARRAY_SIZE(tDisplayCyclicScreens),
    0,
    WIDTH,
    HEIGHT};
#endif

/***********************************************************************************************************************************
 *
 *                        ===============================================================================
 *
 *                                                  F   I   N          D   E
 *
 *                                                P   R   O   G   R   A   M   A
 *
 *                        ===============================================================================
 *
 *                                       ¡   H   A   S   T   A          O   T   R   A   !
 *
 *                        ===============================================================================
 *                                         ___  ___   _     _   _   _   _       ___  __    __
 *                                        /   |/   | | |   / / | | | | | |     /   | \ \  / /
 *                                       / /|   /| | | |  / /  | | | | | |    / /| |  \ \/ /
 *                                      / / |__/ | | | | / /   | | | | | |   / / | |   )  (
 *                                     / /       | | | |/ /    | | | | | |  / / -| |  / /\ \
 *                                    /_/        |_| |___/     |_| |_| |_| /_/   |_| /_/  \_\
 *
 *                        ===============================================================================
 *
 ***********************************************************************************************************************************/