/****************************************************************************************************************
 *
 *   Escrito por: M8AX
 *
 *   Descripción: ESP23_2432S028R Con Logo Trump
 *   -------------------------------------------
 *
 *   Expansión de las pantallas del NerdMinerV2 con aún más datos y mucho más "nerd"
 *   de lo que ya era xD...
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
 *   ------------------------------------------------------------------------------------------------------
 *                                  LEDS RGB DEL MINERO - COMPORTAMIENTO
 *   ------------------------------------------------------------------------------------------------------
 *
 *   MODO NORMAL ( según el hash rate ):
 *
 *   1. **Cuando el hash rate es mayor de 300 kH/s**:
 *      - Los LEDs **rojo**, **verde** y **azul** se encienden de uno en uno, en ese orden, y vuelta a empezar.
 *
 *   2. **Cuando el hash rate es mayor de 0 kH/s pero menor o igual a 300 kH/s**:
 *      - El LED **azul** está encendido de forma constante.
 *      - Los LEDs **rojo** y **verde** parpadean a la vez.
 *
 *   3. **Cuando el hash rate es 0**:
 *      - El LED **azul** está encendido de forma constante.
 *      - El LED **rojo** parpadea.
 *
 *   AJUSTES SEGÚN HORA Y DÍA ( se aplican a los 3 casos de arriba ):
 *
 *   4. **De noche, de 20:00 a 08:00**:
 *      - Hacen lo mismo, pero con menos brillo ( LED_BRILLO_NOCHE ), para no molestar en la oscuridad.
 *
 *   5. **Viernes, sábado y domingo**:
 *      - Hacen lo mismo, pero más rápido ( LED_MS_FINDE en vez de LED_MS_NORMAL ).
 *
 *   6. **Fin de semana + de noche**:
 *      - Las dos cosas a la vez: más rápido y más tenue.
 *
 *   MODOS ESPECIALES ( mandan sobre todo lo anterior, siempre a tope de brillo ):
 *
 *   7. **¡ BLOQUE MINADO !**:
 *      - Cuando el contador de bloques válidos SUBE con el minero encendido, fiesta de LEDs durante 12 horas:
 *        combinaciones de colores al azar sin parar, sea de día, de noche o fin de semana.
 *      - Si se reinicia el minero, la fiesta se corta ( no se graba nada aparte ).
 *
 *   8. **10 de marzo ( el cumple de M8AX )**:
 *      - Todo el día, de 00:00 a 23:59, los 3 LEDs parpadean a la vez y cambian de color al azar.
 *      - Sin noche, sin fin de semana, sin tenue. Siempre a tope. ¡ Felicidades ! xD...
 *
 *   NOTA: este comportamiento solo se ve en las pantallas normales ( minero, precio, reloj, banners...).
 *   Los juegos y animaciones ( Pong, Tetris, Doom, Asteroides, Cifras, etc. ) hacen sus propios destellos
 *   a tope de brillo, sea la hora y el día que sea.
 *
 *
 *           Un minero de Bitcoin es un dispositivo o software que realiza cálculos
 *           matemáticos complejos para verificar y validar transacciones en la red.
 *           Los mineros compiten para resolver estos problemas y añadir un bloque
 *           a la cadena. A cambio, reciben bitcoins recién creados como recompensa.
 *
 *
 *                                 HASHRATE - 380 / 390 KH/S
 *
 *                     Tmp. De Programación 19H - 23545 Líneas De Código
 *                     -------------------------------------------------
 *
 ****************************************************************************************************************/

// Invocando las poderosas librerías que hacen posible esta obra maestra del minado nerd

#include "displayDriver.h"
#if defined(ESP32_2432S028R) || defined(ESP32_2432S028_2USB)
#include <TFT_eSPI.h>
#include <TFT_eTouch.h>
#include "media/images_320_170_tr.h"
#include "media/images_bottom_320_70.h"
#include "media/myFonts.h"
#include "media/Free_Fonts.h"
#include "version.h"
#include "monitor.h"
#include "OpenFontRender.h"
#include <HTTPClient.h>
#include "moonPhase.h"
#include <ArduinoJson.h>
#include <SPI.h>
#include <SD.h>
#include "esp_vfs_fat.h"
#include <qrcode.h>
#include "rotation.h"
#include <clientntp.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <time.h>
#include <ctime>
#include <WiFi.h>
#include <urlencode.h>
#include "drivers/storage/nvMemory.h"
#include "drivers/storage/storage.h"

#define WIDTH 130
#define HEIGHT 170
#define MAX_RESULT_LENGTH 500

extern nvMemory nvMem;
extern monitor_data mMonitor;
extern pool_data pData;
extern DisplayDriver *currentDisplayDriver;
extern bool invertColors;
extern TSettings Settings;
bool hasChangedScreen = true;

OpenFontRender render;
TFT_eSPI tft = TFT_eSPI();                  // Invoke library, pins defined in platformio.ini
TFT_eSprite background = TFT_eSprite(&tft); // Invoke library sprite
SPIClass hSPI(HSPI);
TFT_eTouch<TFT_eSPI> touch(tft, ETOUCH_CS, 0xFF, hSPI);

int colorI = 0, colorM8 = 0, colorIndex = 0, maxtemp = 0, mintemp = 1000, limite = 0, zonilla, solounavez = 0, solouna = 0, sumatele = 1;
uint16_t colors[] = {TFT_WHITE, TFT_RED, TFT_GREEN, TFT_BLUE, TFT_YELLOW, TFT_CYAN, TFT_MAGENTA, TFT_ORANGE, TFT_GREENYELLOW, TFT_PINK, TFT_LIGHTGREY, TFT_SKYBLUE, TFT_OLIVE, TFT_GOLD, TFT_SILVER};
uint32_t rndnumero, uncontadormas = 0, refresca = 0, cuentita = 0, numnotis = 0, numfrases = 0, detectar = 0, detectar2 = 0;
unsigned long lastTelegramEpochTime = 0;       // Guarda el tiempo de la última ejecución (en segundos desde Epoch)
unsigned long startTime = 0;                   // Para guardar Epoch de inicio
const unsigned long interval = 60 * 2 * 60;    // 2 horas en segundos (2 horas * 60 minutos * 60 segundos)
const unsigned long minStartupTime = interval; // Segundos para que no envíe mensaje a telegram si esta configurado, nada más arrancar
float maxkh = 0.00, minkh = 1000.00;
char result[MAX_RESULT_LENGTH];
const char *apiURL = "https://api.coingecko.com/api/v3/coins/markets?vs_currency=usd&ids=bitcoin&price_change_percentage=24h,7d", *serverName = "https://dummyjson.com/quotes/random";
const char *urlsm8ax[] = {
    "YT - https://youtube.com/m8ax",
    "OS - https://opensea.io/es/m8ax",
    "OC - https://oncyber.io/m8ax",
    "FW - https://m8ax.github.io/MvIiIaX-NerdMiner_V2-DeV",
    "OD - https://odysee.com/@m8ax",
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
String BOT_TOKEN, CHAT_ID, cadenanoti = "", prebitco = "", alturabloque = "", variacion1dia, variacion1semana;
std::pair<String, String> Tresultado;
mining_data mineria;
moonPhase mymoonPhase;

void getChipInfo(void)
{
  Serial.print("\nM8AX - Chip: ");
  Serial.print(ESP.getChipModel());
  Serial.print("\nM8AX - Revisión Del Chip: ");
  Serial.print(ESP.getChipRevision());
  Serial.print("\nM8AX - Tamaño De PsRam: ");
  Serial.print(ESP.getPsramSize() / 1024);
  Serial.println("KB");
  Serial.print("M8AX - Tamaño De Flash: ");
  Serial.print(ESP.getFlashChipSize() / 1024);
  Serial.println("KB");
  Serial.print("M8AX - Frecuencia De CPU: ");
  Serial.print(ESP.getCpuFreqMHz());
  Serial.println("MHz\n");
}

bool esBisiesto(int anio)
{
  return ((anio % 4 == 0) && (anio % 100 != 0)) || (anio % 400 == 0);
}

bool obtenerVariacionesBTC(String &variacion1dia, String &variacion1semana)
{
  HTTPClient http;
  http.begin(apiURL);
  int httpCode = http.GET();
  if (httpCode > 0)
  {
    String payload = http.getString();
    DynamicJsonDocument doc(2048);
    DeserializationError error = deserializeJson(doc, payload);
    if (!error)
    {
      float cambio24h = doc[0]["price_change_percentage_24h_in_currency"];
      float cambio7d = doc[0]["price_change_percentage_7d_in_currency"];
      variacion1dia = String(cambio24h, 2) + "%";
      variacion1semana = String(cambio7d, 2) + "%";
      http.end();
      return true;
    }
    else
    {
      Serial.println("M8AX - Error Al Parsear JSON.");
    }
  }
  else
  {
    Serial.println("M8AX - Error Al Hacer La Petición HTTP.");
  }
  http.end();
  return false;
}

String alturab()
{
  WiFiClientSecure client;
  client.setInsecure(); // Desactiva la verificación SSL para facilitar la conexión
  HTTPClient https;
  const char *url = "https://blockstream.info/api/blocks/tip/height";
  if (https.begin(client, url))
  {
    int httpCode = https.GET();
    if (httpCode == 200)
    {
      String payload = https.getString();
      https.end();
      return payload; // Devuelve directamente el string con la altura del bloque
    }
    else
    {
      Serial.printf("M8AX - Error HTTP: %d\n", httpCode);
    }
    https.end();
  }
  else
  {
    Serial.println("M8AX - Conexión HTTPS Fallida");
  }
  return "ERROR"; // Devuelve "error" si algo falla
}

String preciob(void)
{
  String bitcoin_price = "";
  if (WiFi.status() != WL_CONNECTED)
    return String("---") + "$";
  HTTPClient http;
  try
  {
    http.begin(getBTCAPI);
    int httpCode = http.GET();
    if (httpCode == HTTP_CODE_OK)
    {
      String payload = http.getString();
      DynamicJsonDocument doc(1024);
      deserializeJson(doc, payload);
      if (doc.containsKey("data") && doc["data"].containsKey("amount"))
        bitcoin_price = String(doc["data"]["amount"].as<const char *>());
      doc.clear();
    }
    http.end();
  }
  catch (...)
  {
    http.end();
  }
  return String((int)bitcoin_price.toFloat()) + "$";
}

/**************************************************************************
 *
 *   LEDs DEL MINERO - Noche tenue, finde rápido, fiesta de bloque y cumple
 *   ----------------------------------------------------------------------
 *
 *   Las pantallas normales solo avisan del hashrate ( manejandoLeds ), y
 *   el parpadeo lo hace actualizaLeds() desde DoLedStuff.
 *
 *   Prioridad: 10 de marzo > bloque minado > noche / fin de semana.
 *
 *   Ajustes: LED_BRILLO_NOCHE ( 0-255 ), LED_MS_NORMAL, LED_MS_FINDE y
 *   LED_MS_FIESTA ( ms entre cambios ). DoLedStuff se llama cada 100 ms,
 *   así que por debajo de eso no se nota diferencia.
 *
 **************************************************************************/

#define LED_BRILLO_DIA 255
#define LED_BRILLO_NOCHE 15             // 0-255, más bajo = más tenue (20:00 a 08:00)
#define LED_MS_NORMAL 1000              // ms entre cambios entre semana
#define LED_MS_FINDE 500                // ms entre cambios vie, sáb y dom
#define LED_MS_FIESTA 80                // ms entre cambios en la fiesta
#define LED_FIESTA_BLOQUE_MS 43200000UL // 12 horas

static float ledHashRate = 0;
static unsigned long ledUltimoManejo = 0;
static unsigned long fiestaBloqueHasta = 0;
static bool fiestaBloqueActiva = false;
static int validosPrev = -1;
// Aviso en pantalla de BLOQUE MINADO: parpadea cada 5 s ( ES / EN ) hasta que se toque la pantalla
#define BLOQUE_AVISO_CICLO_MS 5000   // cada cuántos ms sale el aviso ( 5000 = 5 s )
#define BLOQUE_AVISO_VISIBLE_MS 3000 // cuánto tiempo se ve el aviso en cada parpadeo
static bool bloqueAvisoActivo = false;
static bool bloqueAvisoIngles = false;
static unsigned long bloqueAvisoUltimo = 0;

// Igual que touch.getXY, pero si hay toque apaga el aviso de bloque minado
static bool toqueXY(int16_t &x, int16_t &y)
{
  bool p = touch.getXY(x, y);
  if (p)
    bloqueAvisoActivo = false;
  return p;
}

/***********************************************************************
 *
 *   PWM de los LEDs: se configura UNA sola vez y los 3 canales quedan
 *   enganchados mientras estés en pantallas normales. ledsLibres() los
 *   suelta al cambiar de pantalla para que los juegos usen digitalWrite.
 *   brillo 0 = apagado, 255 = a tope ( LEDs activos en LOW ).
 *
 ***********************************************************************/

static bool ledPwmListo = false;

static void ledSet(uint8_t pin, uint8_t canal, uint8_t brillo)
{
  if (!ledPwmListo)
  {
    for (uint8_t c = 1; c <= 3; c++)
      ledcSetup(c, 5000, 8);
    ledcAttachPin(LED_PIN, 1);
    ledcAttachPin(LED_PIN_G, 2);
    ledcAttachPin(LED_PIN_B, 3);
    for (uint8_t c = 1; c <= 3; c++)
      ledcWrite(c, 255); // los 3 apagados antes de nada
    ledPwmListo = true;
  }
  ledcWrite(canal, 255 - brillo);
}

static void ledsLibres()
{
  ledcDetachPin(LED_PIN);
  ledcDetachPin(LED_PIN_G);
  ledcDetachPin(LED_PIN_B);
  ledPwmListo = false;
  pinMode(LED_PIN, OUTPUT);
  pinMode(LED_PIN_G, OUTPUT);
  pinMode(LED_PIN_B, OUTPUT);
  digitalWrite(LED_PIN, HIGH);
  digitalWrite(LED_PIN_G, HIGH);
  digitalWrite(LED_PIN_B, HIGH);
}

// Las pantallas normales solo avisan del hashrate; el parpadeo lo hace actualizaLeds()
void manejandoLeds(float currentHashRate)
{
  ledHashRate = currentHashRate;
  ledUltimoManejo = millis();
}

// mascara: bit0 rojo, bit1 verde, bit2 azul
static void ledMascara(uint8_t m)
{
  ledSet(LED_PIN, 1, (m & 1) ? 255 : 0);
  ledSet(LED_PIN_G, 2, (m & 2) ? 255 : 0);
  ledSet(LED_PIN_B, 3, (m & 4) ? 255 : 0);
}

static void actualizaLeds()
{
  unsigned long ahora = millis();

  // Detectar bloque minado: solo si el contador SUBE con el minero encendido
  int v = mineria.valids.toInt();
  if (validosPrev >= 0 && v > validosPrev)
  {
    fiestaBloqueActiva = true;
    fiestaBloqueHasta = ahora + LED_FIESTA_BLOQUE_MS;
    bloqueAvisoActivo = true; // aviso en pantalla hasta que se toque
    bloqueAvisoIngles = false;
    bloqueAvisoUltimo = 0;
    Serial.println("M8AX - BLOQUE MINADO! Fiesta De LEDs 12 Horas");
  }
  validosPrev = v;
  if (fiestaBloqueActiva && (long)(fiestaBloqueHasta - ahora) <= 0)
    fiestaBloqueActiva = false;

  // Si ninguna pantalla normal ha avisado hace poco (estamos en un juego), no tocar nada
  if (ledUltimoManejo == 0 || ahora - ledUltimoManejo > 2500)
    return;

  bool noche = false, finde = false, cumple = false;
  time_t epoch = (time_t)timeClient.getEpochTime();
  if (epoch > 1700000000)
  {
    struct tm ti;
    localtime_r(&epoch, &ti);
    noche = (ti.tm_hour >= 20 || ti.tm_hour < 8);
    finde = (ti.tm_wday == 0 || ti.tm_wday == 5 || ti.tm_wday == 6); // dom, vie, sáb
    cumple = (ti.tm_mon == 2 && ti.tm_mday == 10);                   // 10 de marzo
  }

  static unsigned long ultimo = 0;
  static uint8_t paso = 0;
  static uint8_t color = 7;

  // 10 de marzo: los 3 LEDs a la vez, a tope, cambiando de color
  if (cumple)
  {
    if (ahora - ultimo < LED_MS_FIESTA)
      return;
    ultimo = ahora;
    paso++;
    if (paso % 8 == 0)
      color = 1 + esp_random() % 7;
    ledMascara((paso % 2 == 0) ? color : 0);
    return;
  }

  // Bloque minado: locura total durante 12 horas
  if (fiestaBloqueActiva)
  {
    if (ahora - ultimo < LED_MS_FIESTA)
      return;
    ultimo = ahora;
    ledMascara(esp_random() % 8);
    return;
  }

  // Modo normal: noche más tenue, fin de semana más rápido
  uint8_t br = noche ? LED_BRILLO_NOCHE : LED_BRILLO_DIA;
  unsigned long ms = finde ? LED_MS_FINDE : LED_MS_NORMAL;
  static unsigned long dbg = 0;
  if (ahora - dbg > 10000)
  {
    dbg = ahora;
    Serial.printf("M8AX - LEDs Noche=%d Finde=%d Br=%d Hash=%.1f\n", noche, finde, br, ledHashRate);
  }
  if (ahora - ultimo < ms)
    return;
  ultimo = ahora;
  paso++;

  if (ledHashRate > 300)
  {
    // Rojo, verde, azul, uno cada vez
    uint8_t p = paso % 3;
    ledSet(LED_PIN, 1, p == 0 ? br : 0);
    ledSet(LED_PIN_G, 2, p == 1 ? br : 0);
    ledSet(LED_PIN_B, 3, p == 2 ? br : 0);
  }
  else if (ledHashRate > 0)
  {
    // Azul fijo, rojo y verde parpadean a la vez
    bool on = (paso % 2 == 0);
    ledSet(LED_PIN_B, 3, br);
    ledSet(LED_PIN, 1, on ? br : 0);
    ledSet(LED_PIN_G, 2, on ? br : 0);
  }
  else
  {
    // Azul fijo, rojo parpadea
    ledSet(LED_PIN_B, 3, br);
    ledSet(LED_PIN_G, 2, 0);
    ledSet(LED_PIN, 1, (paso % 2 == 0) ? br : 0);
  }
}

String capitalizar(String palabra)
{
  if (palabra.length() > 0)
  {
    palabra[0] = toupper(palabra[0]);
  }
  return palabra;
}

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

String quitarAcentos(String str)
{
  // Sustituir Letra De Izquierda Por Letra Derecha
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

double moonPhase::_fhour(const struct tm &timeinfo)
{
  return timeinfo.tm_hour + ((timeinfo.tm_min * 60) + timeinfo.tm_sec) / 3600.0;
}

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

// ===== Salida Y Puesta De Sol (cálculo local, sin internet) =====
static char salidaypuesta[48] = "";

static void solHora(char *o, double t)
{
  t = fmod(t, 24.0);
  if (t < 0)
    t += 24.0;
  int h = (int)t;
  int mi = (int)((t - h) * 60.0 + 0.5);
  if (mi >= 60)
  {
    mi = 0;
    h = (h + 1) % 24;
  }
  snprintf(o, 6, "%02d:%02d", h, mi);
}

static void calcSol(double lat, double lng)
{
  salidaypuesta[0] = 0;
  time_t epoch = (time_t)timeClient.getEpochTime();
  if (epoch < 1700000000) // reloj sin poner
    return;
  struct tm ti;
  localtime_r(&epoch, &ti);
  int year = ti.tm_year + 1900, month = ti.tm_mon + 1, day = ti.tm_mday;
  int a = (14 - month) / 12;
  int y = year + 4800 - a;
  int m = month + 12 * a - 3;
  double jd = day + (153 * m + 2) / 5 + 365.0 * y + y / 4 - y / 100 + y / 400 - 32045;
  double T = (jd - 2451545.0) / 36525.0;
  double L0 = fmod(280.46646 + T * (36000.76983 + T * 0.0003032), 360.0);
  double M = 357.52911 + T * (35999.05029 - 0.0001537 * T);
  double e = 0.016708634 - T * (0.000042037 + 0.0000001267 * T);
  double Mr = M * DEG_TO_RAD;
  double C = sin(Mr) * (1.914602 - T * (0.004817 + 0.000014 * T)) + sin(2 * Mr) * (0.019993 - 0.000101 * T) + sin(3 * Mr) * 0.000289;
  double omega = 125.04 - 1934.136 * T;
  double appLong = L0 + C - 0.00569 - 0.00478 * sin(omega * DEG_TO_RAD);
  double seconds = 21.448 - T * (46.815 + T * (0.00059 - T * 0.001813));
  double eps = 23.0 + (26.0 + seconds / 60.0) / 60.0 + 0.00256 * cos(omega * DEG_TO_RAD);
  double decl = asin(sin(eps * DEG_TO_RAD) * sin(appLong * DEG_TO_RAD));
  double yy = tan(eps / 2.0 * DEG_TO_RAD);
  yy = yy * yy;
  double L0r = L0 * DEG_TO_RAD;
  double eot = 4.0 * RAD_TO_DEG * (yy * sin(2 * L0r) - 2 * e * sin(Mr) + 4 * e * yy * sin(Mr) * cos(2 * L0r) - 0.5 * yy * yy * sin(4 * L0r) - 1.25 * e * e * sin(2 * Mr));
  double cosH = cos(90.833 * DEG_TO_RAD) / (cos(lat * DEG_TO_RAD) * cos(decl)) - tan(lat * DEG_TO_RAD) * tan(decl);
  if (cosH > 1)
  {
    strcpy(salidaypuesta, "El Sol No Sale Hoy ( Noche Polar )");
    return;
  }
  if (cosH < -1)
  {
    strcpy(salidaypuesta, "Sol Todo El Día ( Día Polar )");
    return;
  }
  double H = acos(cosH) * RAD_TO_DEG / 15.0;
  double noon = (720.0 - 4.0 * lng - eot) / 60.0;
  char s1[6], s2[6];
  solHora(s1, noon - H + Settings.Timezone);
  solHora(s2, noon + H + Settings.Timezone);
  snprintf(salidaypuesta, sizeof(salidaypuesta), "Sol ^ - %s | Sol v - %s", s1, s2);
}

std::pair<String, String> obtenerCiudadYTemperatura(const String &ip)
{
  String ciudad = "ERROR", temperatura = "ERROR";
  String latitud = "", longitud = "";
  HTTPClient http;
  String urlGeo = "http://ip-api.com/json/" + ip + "?fields=city,lat,lon";
  http.begin(urlGeo);
  int httpCode = http.GET();
  if (httpCode == HTTP_CODE_OK)
  {
    String payload = http.getString();
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, payload);
    payload = "";
    payload.reserve(0);
    if (error)
    {
      Serial.println("M8AX - Error Al Parsear JSON De Geolocalización");
      http.end();
      return std::make_pair(ciudad, temperatura);
    }
    ciudad = doc["city"].as<String>();
    latitud = String(doc["lat"].as<float>(), 6);
    longitud = String(doc["lon"].as<float>(), 6);
    if (!doc["lat"].isNull() && !doc["lon"].isNull())
      calcSol(doc["lat"].as<double>(), doc["lon"].as<double>());
    Serial.println("M8AX - Obtenida Ciudad");
  }
  else
  {
    Serial.println("M8AX - Error Al Obtener Datos De Geolocalización");
    http.end();
    return std::make_pair(ciudad, temperatura);
  }
  http.end();
  if (latitud != "" && longitud != "")
  {
    WiFiClientSecure client;
    client.setInsecure();
    String urlTemp = "https://wttr.in/" + latitud + "," + longitud + "?format=%t";
    http.begin(client, urlTemp);
    httpCode = http.GET();
    urlTemp.reserve(0);
    if (httpCode == HTTP_CODE_OK)
    {
      temperatura = http.getString();
      Serial.println("M8AX - Obtenida Temperatura");
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
  return std::make_pair(ciudad, temperatura);
}

void obtenerLocYTemp()
{
  salidaypuesta[0] = 0;
  Tresultado.first.clear();
  Tresultado.second.clear();
  Tresultado.first.reserve(0);
  Tresultado.second.reserve(0);
  Tresultado = obtenerCiudadYTemperatura(getPublicIP());
}

void dibujaAnalogKH(float khs)
{
  int centerX = 160;
  int centerY = 85, radius = 60;
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
        tft.print("400"); // arriba ( 12h, 0° ): 400 KH/s, es la vuelta completa
      else if (i == 90)
      {
        tft.setCursor(textX - 11, textY - 5);
        tft.print("100"); // derecha ( 3h, 90° ): 100 KH/s
      }
      else if (i == 180)
        tft.print("200"); // abajo ( 6h, 180° ): 200 KH/s
      else if (i == 270)
        tft.print("300"); // izquierda ( 9h, 270° ): 300 KH/s
    }
  }

  // No dibujar la aguja si khs es 0
  if (khs == 0)
  {
    return;
  }

  // Escala: 400 KH/s = 360° ( vuelta completa ), o sea 0.9° por cada KH/s
  //   0 KH/s =   0° desde las 12 ( arriba )                  -> -90° en pantalla
  // 100 KH/s =  90° ( las 3, derecha )                       ->   0° en pantalla
  // 200 KH/s = 180° ( las 6, abajo )                         ->  90° en pantalla
  // 300 KH/s = 270° ( las 9, izquierda )                     -> 180° en pantalla
  // 400 KH/s = 360° ( vuelta completa, otra vez arriba )     -> 270° en pantalla
  float angle = -90.0f + constrain(khs, 0.0f, 400.0f) * 0.9f; // en pantalla 0° es la derecha y se gira en sentido horario

  // Convertir el ángulo a radianes
  float radianes = angle * DEG_TO_RAD;

  // Coordenadas de la punta de la aguja (calculada a partir del ángulo)
  int agujaX = centerX + radius * cos(radianes), agujaY = centerY + radius * sin(radianes);

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

const char *evaluarRSSI(const String &rssiStr)
{
  int rssi = rssiStr.toInt();
  if (rssi >= -50)
  {
    return "******";
  }
  else if (rssi >= -60)
  {
    return "*****";
  }
  else if (rssi >= -70)
  {
    return "****";
  }
  else if (rssi >= -80)
  {
    return "***";
  }
  else if (rssi >= -90)
  {
    return "**";
  }
  else
  {
    return "*";
  }
}

void relojAnalogicoM8AX(int hours, int minutes, int seconds)
{
  // Definir área específica para el reloj (parte superior)
  const int clockWidth = 320;
  const int clockHeight = 170;
  const int clockAreaX = (tft.width() - clockWidth) / 2; // Centrado horizontal
  const int clockAreaY = 0;                              // Parte superior

  // Calcular centro y radio del reloj dentro del área designada
  const int centerX = clockAreaX + clockWidth / 2;
  const int centerY = clockAreaY + clockHeight / 2;
  const int clockRadius = min(clockWidth, clockHeight) * 0.45; // 45% del área

  // Función interna para dibujar manecillas
  auto drawHand = [&](float angle, int length, uint16_t color, int width)
  {
    int endX = centerX + length * cos(angle);
    int endY = centerY + length * sin(angle);

    if (width > 1)
    {
      float perpAngle = angle + PI / 2;
      float dx = (width / 2) * cos(perpAngle);
      float dy = (width / 2) * sin(perpAngle);

      tft.fillTriangle(
          centerX + dx, centerY + dy,
          centerX - dx, centerY - dy,
          endX - dx, endY - dy,
          color);
      tft.fillTriangle(
          centerX + dx, centerY + dy,
          endX - dx, endY - dy,
          endX + dx, endY + dy,
          color);
    }
    else
    {
      tft.drawLine(centerX, centerY, endX, endY, color);
    }
  };

  // Limpiar solo el área del reloj
  tft.fillRect(clockAreaX, clockAreaY, clockWidth, clockHeight, TFT_BLACK);

  // Dibujar círculos del reloj
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.drawCircle(centerX, centerY, clockRadius, colors[colorI]);
  colorIndex = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.drawCircle(centerX, centerY, clockRadius * 0.4, colors[colorIndex]);

  // Dibujar marcas de horas (12)
  for (int i = 0; i < 12; i++)
  {
    float angle = i * PI / 6;
    int markLength = (i % 3 == 0) ? 12 : 8; // Marcas más largas cada 3 horas
    tft.drawLine(
        centerX + (clockRadius - markLength) * cos(angle),
        centerY + (clockRadius - markLength) * sin(angle),
        centerX + clockRadius * cos(angle),
        centerY + clockRadius * sin(angle),
        TFT_WHITE);
  }

  // Dibujar marcas de minutos (60)
  for (int i = 0; i < 60; i++)
  {
    if (i % 5 != 0)
    { // Saltar donde ya hay marcas de hora
      float angle = i * PI / 30;
      tft.drawPixel(
          centerX + (clockRadius - 5) * cos(angle),
          centerY + (clockRadius - 5) * sin(angle),
          TFT_WHITE);
    }
  }

  // Dibujar marcas de 24 horas
  for (int i = 0; i < 24; i++)
  {
    float angle = i * PI / 12;
    if (i % 6 == 0)
    { // Marcas más visibles cada 6 horas
      tft.drawLine(
          centerX + (clockRadius * 0.4 - 6) * cos(angle),
          centerY + (clockRadius * 0.4 - 6) * sin(angle),
          centerX + clockRadius * 0.4 * cos(angle),
          centerY + clockRadius * 0.4 * sin(angle),
          TFT_CYAN);
    }
    else
    {
      tft.drawPixel(
          centerX + (clockRadius * 0.4 - 4) * cos(angle),
          centerY + (clockRadius * 0.4 - 4) * sin(angle),
          TFT_CYAN);
    }
  }

  // Calcular ángulos con movimiento suave
  float hourAngle = (hours % 12 + minutes / 60.0) * PI / 6 - PI / 2;
  float minuteAngle = (minutes + seconds / 60.0) * PI / 30 - PI / 2;
  float secondAngle = seconds * PI / 30 - PI / 2;
  float hour24Angle = hours * PI / 12 - PI / 2;

  // Dibujar manecillas
  drawHand(hour24Angle, clockRadius * 0.35, TFT_CYAN, 3); // Manecilla 24h azul
  drawHand(hourAngle, clockRadius * 0.5, TFT_WHITE, 5);   // Hora blanca
  drawHand(minuteAngle, clockRadius * 0.7, TFT_WHITE, 3); // Minuto blanco
  drawHand(secondAngle, clockRadius * 0.85, TFT_RED, 1);  // Segundo rojo

  // Dibujar centro del reloj
  tft.fillCircle(centerX, centerY, 5, TFT_RED);
}

const char *obtenerEstadoTemperatura(int temperatura)
{
  if (temperatura <= 45)
  {
    return "Temperatura Baja";
  }
  else if (temperatura >= 46 && temperatura <= 65)
  {
    return "Temperatura Normal";
  }
  else if (temperatura >= 66 && temperatura <= 75)
  {
    return "Temperatura Alta";
  }
  else
  {
    return "Temperatura Muy Alta";
  }
}

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
    tft.setCursor(80, 90);
    (numeroSaludo % 2 == 0) ? tft.print("HOLA") : tft.print("EYEY");
  }
  else
  {
    tft.setCursor(60, 90);
    (numeroSaludo % 2 == 0) ? tft.print("HELLO") : tft.print("KAIXO");
  }

  tft.setTextSize(1);
}

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

void cortinas()
{
  tft.fillScreen(TFT_BLACK); // Fondo negro
  cortinas2();
  drawCenteredText("M 8 A X", 50, 100);
  drawCenteredText("NerdMiner V2", 90, 100);
  drawCenteredText("V  10 . 03 . 77", 130, 100);
  delay(500);
  television();
  delay(1000);
  tft.setTextSize(1);
}

void M8AXTicker()
{
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(colors[colorI], TFT_BLACK);
  tft.setTextSize(2);
  drawCenteredText("M v I i I a X", 50, 100);
  drawCenteredText("NerdMiner V2", 90, 100);
  drawCenteredText("V  10 . 03 . 77", 130, 100);
  delay(500);
  television();
  delay(1000);
  tft.setTextSize(1);
}

void nevar3()
{
  tft.fillScreen(TFT_BLACK);             // Fondo negro (puedes cambiarlo)
  const int NUM_COPOS = 500;             // Número de copos de nieve en pantalla
  static int x[NUM_COPOS], y[NUM_COPOS]; // Coordenadas de los copos

  // Inicializa copos en posiciones aleatorias
  for (int i = 0; i < NUM_COPOS; i++)
  {
    x[i] = esp_random() % 536;
    y[i] = esp_random() % 240;
  }
  unsigned long startTime = millis();
  while (millis() - startTime < 2000)
  {
    for (int i = 0; i < NUM_COPOS; i++)
    {
      tft.drawPixel(x[i], y[i], TFT_BLACK);
      y[i] += (esp_random() % 5) + 1;
      if (y[i] > 240)
      {
        y[i] = 0;
        x[i] = esp_random() % 536;
      }
      colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
      tft.drawPixel(x[i], y[i], colors[colorI]);
    }
    int nnumeroAleatorio = esp_random() % 20 + 1;
    delay(nnumeroAleatorio);
  }
  M8AXTicker();
}

void M8AXTicker2()
{
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(colors[colorI], TFT_BLACK);
  tft.setTextSize(2);
  drawCenteredText("B T C  M I N E R", 50, 100);
  drawCenteredText("NerdMiner V2", 90, 100);
  drawCenteredText("V  10 . 03 . 77", 130, 100);
  delay(500);
  television();
  delay(1000);
  tft.setTextSize(1);
}

void M8AXTicker3()
{
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(colors[colorI], TFT_BLACK);
  tft.setTextSize(2);
  drawCenteredText("M I N E  T E C H", 50, 100);
  drawCenteredText("NerdMiner V2", 90, 100);
  drawCenteredText("V  10 . 03 . 77", 130, 100);
  delay(500);
  television();
  delay(1000);
  tft.setTextSize(1);
}

void M8AXTicker4()
{
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(colors[colorI], TFT_BLACK);
  tft.setTextSize(2);
  drawCenteredText("I M O D   T E C H", 50, 100);
  drawCenteredText("NerdMiner V2", 90, 100);
  drawCenteredText("V  10 . 03 . 77", 130, 100);
  delay(500);
  television();
  delay(1000);
  tft.setTextSize(1);
}

void M8AXTicker5()
{
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(colors[colorI], TFT_BLACK);
  tft.setTextSize(2);
  drawCenteredText("E H D  -  M D D D", 50, 100);
  drawCenteredText("NerdMiner V2", 90, 100);
  drawCenteredText("V  10 . 03 . 77", 130, 100);
  delay(500);
  television();
  delay(1000);
  tft.setTextSize(1);
}

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

void nevar()
{
  tft.fillScreen(TFT_BLACK); // Fondo negro (puedes cambiarlo)
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  const int NUM_COPOS = 400;             // Número de copos de nieve en pantalla
  static int x[NUM_COPOS], y[NUM_COPOS]; // Coordenadas de los copos
  // Inicializa copos en posiciones aleatorias
  for (int i = 0; i < NUM_COPOS; i++)
  {
    x[i] = (esp_random() % 536); // Genera un número entre 0 y 319
    y[i] = (esp_random() % 240); // Genera un número entre 0 y 169
  }

  unsigned long startTime = millis();

  while (millis() - startTime < 5000)
  {
    for (int i = 0; i < NUM_COPOS; i++)
    {
      tft.drawPixel(x[i], y[i], TFT_BLACK);
      y[i] += (esp_random() % 4) + 1;
      if (y[i] > 240)
      {
        y[i] = 0;
        x[i] = esp_random() % 536;
      }
      tft.drawPixel(x[i], y[i], colors[colorI]);
    }
    delay(35);
  }

  tft.fillScreen(TFT_BLACK);
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.setTextColor(colors[colorI], TFT_BLACK);
  tft.setTextDatum(MC_DATUM);                // Centra el texto
  tft.setFreeFont(FSB18);                    // Fuente grande (cambia si es necesario)
  tft.drawString("VAMOS A MINAR", 159, 100); // Texto en el centro
  delay(1500);
  tft.setFreeFont(NULL);
}

void nevar2()
{
  tft.fillScreen(TFT_BLACK); // Fondo negro (puedes cambiarlo)
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  const int NUM_COPOS = 600;             // Número de copos de nieve en pantalla
  static int x[NUM_COPOS], y[NUM_COPOS]; // Coordenadas de los copos

  // Inicializa copos en posiciones aleatorias
  for (int i = 0; i < NUM_COPOS; i++)
  {
    x[i] = esp_random() % 536;
    y[i] = esp_random() % 240;
  }

  unsigned long startTime = millis();

  while (millis() - startTime < 5000)
  {
    for (int i = 0; i < NUM_COPOS; i++)
    {
      tft.drawPixel(x[i], y[i], TFT_BLACK);
      y[i] += (esp_random() % 5) + 1;
      if (y[i] > 240)
      {
        y[i] = 0;
        x[i] = esp_random() % 536;
      }
      tft.drawPixel(x[i], y[i], colors[colorI]);
    }
    delay(25);
  }

  tft.fillScreen(TFT_BLACK);
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.setTextColor(colors[colorI], TFT_BLACK);
  tft.setTextDatum(MC_DATUM);                  // Centra el texto
  tft.setFreeFont(FSB18);                      // Fuente grande (cambia si es necesario)
  tft.drawString("LIBERTY BITCOIN", 159, 100); // Texto en el centro
  delay(1500);
  tft.setFreeFont(NULL);
}

// Nieve + felicitación ( Navidad y Año Nuevo ). Copos 100 / 200 como en el tDisplay
void nevarFelicita(const char *texto, int copos, int pausaMs)
{
  if (copos > 200)
    copos = 200;
  static int x[200], y[200];
  tft.fillScreen(TFT_BLACK);
  for (int i = 0; i < copos; i++)
  {
    x[i] = esp_random() % 320;
    y[i] = esp_random() % 240;
  }
  int vel = (copos > 100) ? 5 : 4;
  unsigned long inicio = millis();
  while (millis() - inicio < 5000)
  {
    for (int i = 0; i < copos; i++)
    {
      tft.fillRect(x[i], y[i], 2, 2, TFT_BLACK);
      y[i] += (esp_random() % vel) + 1;
      if (y[i] >= 240)
      {
        y[i] = 0;
        x[i] = esp_random() % 318;
      }
      tft.fillRect(x[i], y[i], 2, 2, TFT_WHITE);
    }
    delay(pausaMs);
  }
  tft.fillScreen(TFT_BLACK);
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.setTextColor(colors[colorI], TFT_BLACK);
  tft.setTextDatum(MC_DATUM);
  tft.setFreeFont(FSB18);
  tft.drawString(texto, 160, 85);
  delay(1500);
  tft.setFreeFont(NULL);
  tft.setTextDatum(TL_DATUM);
  hasChangedScreen = true;
}

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
    return "Mes Invalido";
  }
}

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
    tft.setTextColor(TFT_WHITE);
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
  }
  colorIndex = esp_random() % (sizeof(colors) / sizeof(colors[0]));
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

const char *factorize(uint32_t number)
{
  char buffer[20];       // Buffer temporal para formatear factores
  uint32_t factors[16];  // Almacena factores primos
  uint8_t exponents[16]; // Almacena exponentes
  uint8_t count = 0;     // Número de factores encontrados
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

uint32_t floatToUint32(float num)
{
  if (num < 0)
    return 0;
  if (num > UINT32_MAX)
    return UINT32_MAX;
  return static_cast<uint32_t>(round(num));
}

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

// Codifica un byte para URL ( igual que urlEncode, el espacio sale como + ): escribe 1 o 3 caracteres en o
static inline size_t tgCodifica(uint8_t c, char *o)
{
  static const char hexd[] = "0123456789ABCDEF";
  if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '-' || c == '_' || c == '.' || c == '~')
  {
    o[0] = (char)c;
    return 1;
  }
  if (c == ' ')
  {
    o[0] = '+';
    return 1;
  }
  o[0] = '%';
  o[1] = hexd[c >> 4];
  o[2] = hexd[c & 15];
  return 3;
}

bool enviarMensajeATelegram(const String &mensaje)
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

  // Se cuenta cuánto ocupará el cuerpo ya codificado ( hace falta para Content-Length )
  const char *m = mensaje.c_str();
  const size_t n = mensaje.length();
  const String cab = "chat_id=" + CHAT_ID + "&text=";
  char t[3];
  size_t largo = cab.length();
  for (size_t i = 0; i < n; i++)
    largo += tgCodifica((uint8_t)m[i], t);
  String head = "POST /bot" + BOT_TOKEN + "/sendMessage HTTP/1.1\r\nHost: api.telegram.org\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: " + String((unsigned)largo) + "\r\nConnection: close\r\n\r\n";
  const size_t total = head.length() + largo;

  WiFiClientSecure client;
  client.setInsecure();
  Serial.println("M8AX - Enviando Mensaje A Telegram...");

  if (client.connect("api.telegram.org", 443))
  {
    // Un solo bloque con cabecera + cuerpo, creado DESPUÉS de conectar y enviado de una vez
    uint8_t *pet = (uint8_t *)malloc(total);
    if (!pet)
    {
      Serial.printf("M8AX - Telegram: No Hay RAM Para El Mensaje ( %u Bytes )\n", (unsigned)total);
      client.stop();
      return false;
    }
    size_t p = head.length();
    memcpy(pet, head.c_str(), p);
    head = "";
    memcpy(pet + p, cab.c_str(), cab.length());
    p += cab.length();
    for (size_t i = 0; i < n; i++)
      p += tgCodifica((uint8_t)m[i], (char *)pet + p);

    bool fallo = client.write(pet, p) != p;
    free(pet);

    // Telegram a veces tarda en contestar ( la red está ocupada con el minero ): se espera hasta 12 s
    unsigned long t0 = millis();
    while (!client.available() && client.connected() && millis() - t0 < 12000)
    {
      delay(10);
    }
    String estado = client.readStringUntil('\n');
    bool sinRespuesta = !fallo && estado.length() == 0; // se envió todo pero no contestó a tiempo
    bool acepto = !fallo && estado.indexOf(" 200") > 0;
    String resto;
    if (!acepto && !sinRespuesta)
    {
      // Solo si falla de verdad: lee un trozo de la respuesta para ver el motivo ( máx. 600 bytes )
      unsigned long t1 = millis();
      while ((client.available() || client.connected()) && millis() - t1 < 1500 && resto.length() < 600)
      {
        while (client.available() && resto.length() < 600)
          resto += (char)client.read();
        delay(5);
      }
    }
    client.stop();
    if (acepto)
    {
      Serial.println("M8AX - Mensaje Enviado A Telegram...");
      sumatele += 1;
      ok = true;
    }
    else if (sinRespuesta)
    {
      // El mensaje se mandó entero; Telegram lo suele recibir aunque no conteste a tiempo.
      // Se da por enviado para no reintentar a los 5 minutos y que no salga repetido.
      Serial.println("M8AX - Mensaje Enviado A Telegram ( Sin Confirmación A Tiempo )...");
      sumatele += 1;
      ok = true;
    }
    else
    {
      estado.trim();
      if (fallo)
        Serial.println("M8AX - Se Cortó La Conexión Mientras Se Enviaba El Mensaje...");
      Serial.println("M8AX - Telegram No Lo Aceptó - Respuesta: [" + estado + "]");
      if (resto.length())
        Serial.println("M8AX - Detalle: " + resto);
    }
  }
  else
  {
    Serial.println("M8AX - Error De Conexión, Mensaje A Telegram Falló...");
    client.flush();
    client.stop();
  }
  return ok;
}

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

String lunitaporc(String currentDate, String currentTime)
{
  moonData_t moon;
  int dia = currentDate.substring(0, 2).toInt();
  int mes = currentDate.substring(3, 5).toInt();
  int anio = currentDate.substring(6, 10).toInt();
  int hora = currentTime.substring(0, 2).toInt();
  int minuto = currentTime.substring(3, 5).toInt();
  unsigned long segundo = timeClient.getSeconds();

  struct tm timeinfo;
  timeinfo.tm_year = anio - 1900; // Año desde 1900
  timeinfo.tm_mon = mes - 1;      // Mes (0 = enero)
  timeinfo.tm_mday = dia;         // Día del mes
  timeinfo.tm_hour = hora;        // Hora
  timeinfo.tm_min = minuto;       // Minutos
  timeinfo.tm_sec = segundo;      // Segundos
  timeinfo.tm_isdst = -1;         // Automático DST

  // Convertir a time_t
  time_t cadenaDeTiempo = mktime(&timeinfo);

  // Obtener fase de la luna
  moon = mymoonPhase.getPhase(cadenaDeTiempo);
  double porcentajeIluminado = moon.percentLit * 100;

  // Formatear texto
  char porcentajeTexto[10];
  snprintf(porcentajeTexto, sizeof(porcentajeTexto), "%.2f%%", porcentajeIluminado);

  return "Luna Iluminada Al - " + String(porcentajeTexto);
}

#include <new>
#define SD_CS 5
#define SD_SCK 18
#define SD_MISO 19
#define SD_MOSI 23
#define SD_BUS HSPI // la pantalla va por VSPI, así que la SD va por HSPI. Si no monta, prueba con VSPI

static SPIClass sdSPI(SD_BUS); // bus aparte del de la pantalla (pines 18/19/23/5 de la placa)
static bool sdOk = false;
static bool sdGuardadoCiclo = false;
static unsigned long sdFalloEn = 0;
static bool sdSinTarjeta = false; // true = la ranura está vacía ( no tiene sentido reintentar )
#define SD_COLA_MAX 2             // mensajes que esperan a la SD en RAM ( si la SD no vuelve, se pierde el más antiguo )
static String *sdCola[SD_COLA_MAX] = {nullptr, nullptr};
static uint8_t sdColaN = 0;
static unsigned long sdColaUltimo = 0;
static uint8_t sdUltimoCMD0 = 0xFF; // última respuesta de la tarjeta al CMD0 ( 0x01 = bien, 0xFF = no contesta )
static uint8_t sdFallosN = 0;       // fallos seguidos de "no hay tarjeta": solo tras 3 se da por ranura vacía

/**************************************************************************
 *
 *   Pone el reloj interno del ESP32 con la hora de NTP antes de tocar la
 *   SD. Sin esto todo lo que se crea sale con fecha de 1980 en la tarjeta
 *   y el servidor web no la enseña.
 *
 **************************************************************************/
static void sdPonHora()
{
  time_t ahora = (time_t)timeClient.getEpochTime();
  if (ahora < 1700000000)
    return;
  struct timeval tv = {ahora, 0};
  settimeofday(&tv, NULL);
}

// Prueba rápida: manda CMD0 a la tarjeta. Si contesta 0x01 hay tarjeta; si no, la ranura está vacía
static bool sdHayTarjeta()
{
  sdSPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);
  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, HIGH);
  sdSPI.beginTransaction(SPISettings(400000, MSBFIRST, SPI_MODE0));
  for (int i = 0; i < 10; i++)
    sdSPI.transfer(0xFF); // 80 pulsos con CS alto
  digitalWrite(SD_CS, LOW);
  // si la tarjeta sigue ocupada escribiendo, MISO está a 0: espero a que suelte ( máx. 500 ms )
  uint32_t t0 = millis();
  while (sdSPI.transfer(0xFF) != 0xFF && millis() - t0 < 500)
    ;
  const uint8_t cmd0[6] = {0x40, 0x00, 0x00, 0x00, 0x00, 0x95};
  for (int i = 0; i < 6; i++)
    sdSPI.transfer(cmd0[i]);
  uint8_t r = 0xFF;
  for (int i = 0; i < 100 && (r & 0x80); i++) // un R1 válido tiene el bit 7 a 0
    r = sdSPI.transfer(0xFF);
  digitalWrite(SD_CS, HIGH);
  sdSPI.transfer(0xFF);
  sdSPI.endTransaction();
  sdSPI.end();
  sdUltimoCMD0 = r;
  return r == 0x01;
}

static bool sdMonta(bool forzar = false)
{
  sdPonHora();
  if (sdOk)
    return true;
  if (!forzar && sdFalloEn && millis() - sdFalloEn < 60000UL)
    return false;
  uint32_t antes = ESP.getFreeHeap();
  hSPI.end(); // libera el bus del táctil
  bool hay = false;
  for (int k = 0; k < 5 && !hay; k++) // 5 pruebas por si una falla de casualidad
  {
    hay = sdHayTarjeta();
    if (!hay)
      delay(50);
  }
  if (!hay)
  {
    Serial.printf("M8AX - SD: No Hay Tarjeta En La Ranura ( CMD0 Respuesta 0x%02X )\n", sdUltimoCMD0);
    if (++sdFallosN >= 3) // un fallo suelto no cuenta como ranura vacía
      sdSinTarjeta = true;
    sdFalloEn = millis() | 1;
    hSPI.begin(TOUCH_CLK, TOUCH_MISO, TOUCH_MOSI, ETOUCH_CS);
    pinMode(SD_CS, OUTPUT);
    digitalWrite(SD_CS, HIGH); // devuelve el bus al táctil
    return false;
  }
  sdSinTarjeta = false; // hay tarjeta: si falla el montaje, sí se reintenta
  sdFallosN = 0;
  // 3 intentos, cada uno más despacio: a 20 MHz alguna tarjeta o cable falla de vez en cuando
  static const uint32_t VEL_RAPIDA[3] = {20000000, 10000000, 4000000}; // servidor web ( sdMonta() sin forzar ): descargas rápidas
  static const uint32_t VEL_LENTA[3] = {4000000, 4000000, 1000000};    // grabar Telegram ( sdMonta(true) ): pocos KB, manda la fiabilidad
  const uint32_t *VEL = forzar ? VEL_LENTA : VEL_RAPIDA;
  for (int i = 0; i < 3 && !sdOk; i++)
  {
    esp_vfs_fat_unregister_path("/sd");
    sdSPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);
    sdOk = SD.begin(SD_CS, sdSPI, VEL[i]);
    if (!sdOk)
    {
      Serial.printf("M8AX - SD: Intento %d A %u MHz Falló ( Heap Libre %u | Max Bloque %u )\n", i + 1, (unsigned)(VEL[i] / 1000000), (unsigned)ESP.getFreeHeap(), (unsigned)ESP.getMaxAllocHeap());
      SD.end();
      esp_vfs_fat_unregister_path("/sd");
      sdSPI.end();
      delay(60);
    }
  }
  if (!sdOk)
  {
    sdFalloEn = millis() | 1;
    hSPI.begin(TOUCH_CLK, TOUCH_MISO, TOUCH_MOSI, ETOUCH_CS);
    pinMode(SD_CS, OUTPUT);
    digitalWrite(SD_CS, HIGH);
  }
  else
    sdFalloEn = 0;
  if (sdOk && !SD.exists("/-M8AX-NerdMinerV2-PG-M8AX-Logs"))
    SD.mkdir("/-M8AX-NerdMinerV2-PG-M8AX-Logs");
  Serial.printf("M8AX - SD %s (RAM Usada: %d Bytes)\n", sdOk ? "Montada" : "No Encontrada", (int)(antes - ESP.getFreeHeap()));
  return sdOk;
}

String sdEspacioTexto()
{
  if (!sdMonta(true))
    return "Espacio Libre En La MicroSD - SIN TARJETA";
  uint32_t t0 = millis();
  uint64_t total = SD.totalBytes();
  uint64_t usado = SD.usedBytes();
  Serial.printf("M8AX - SD UsedBytes Tardó %lu ms\n", (unsigned long)(millis() - t0));
  uint64_t libre = total > usado ? total - usado : 0;
  String r = "Espacio Libre En La MicroSD - " + String((unsigned long)(libre / 1048576ULL)) + "/" + String((unsigned long)(total / 1048576ULL)) + " MB";
  SD.end(); // se desmonta antes de enviar a Telegram: no ocupa RAM durante la conexión segura
  sdSPI.end();
  hSPI.begin(TOUCH_CLK, TOUCH_MISO, TOUCH_MOSI, ETOUCH_CS);
  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, HIGH); // devuelve el bus al táctil
  sdOk = false;
  return r;
}

bool sdGuardaTelegram(const String &texto)
{
  time_t epoch = (time_t)timeClient.getEpochTime();
  if (epoch < 1700000000) // aun sin hora de internet, no se guarda
  {
    Serial.println("M8AX - SD: Aún Sin Hora De Internet, No Se Guarda...");
    return false;
  }
  struct tm *ti = localtime(&epoch);
  const int anio = ti->tm_year + 1900, mes = ti->tm_mon, dia = ti->tm_mday;
  static const char *MESES[12] = {"Enero", "Febrero", "Marzo", "Abril", "Mayo", "Junio", "Julio", "Agosto", "Septiembre", "Octubre", "Noviembre", "Diciembre"};
  const size_t esperado = texto.length() + 2;
  bool ok = false;
  // Hasta 3 intentos completos: cada uno desmonta, monta de nuevo ( forzado, sin esperar el minuto de castigo ) y escribe
  const int maxInt = sdSinTarjeta ? 1 : 3; // la última vez la ranura estaba vacía: un solo intento, sin parones
  for (int intento = 1; intento <= maxInt && !ok; intento++)
  {
    if (!sdMonta(true))
    {
      if (sdSinTarjeta && intento == maxInt) // ranura vacía
        return false;
      Serial.printf("M8AX - SD: No Monta ( Intento %d De 3 )\n", intento);
      delay(200);
      continue;
    }
    char carpetaAnio[64];
    snprintf(carpetaAnio, sizeof(carpetaAnio), "/-M8AX-NerdMinerV2-PG-M8AX-Logs/M8AX-%04d", anio);
    char carpeta[96];
    snprintf(carpeta, sizeof(carpeta), "%s/M8AX-%s", carpetaAnio, MESES[mes]);
    bool carpetasOk = (SD.exists(carpetaAnio) || SD.mkdir(carpetaAnio)) && (SD.exists(carpeta) || SD.mkdir(carpeta));
    char nombre[128];
    nombre[0] = 0;
    File w; // solo se abre si las carpetas existen: en la raíz de la tarjeta no se escribe nada
    if (carpetasOk)
    {
      snprintf(nombre, sizeof(nombre), "%s/M8AX-Telegram-%04d%02d%02d.TxT", carpeta, anio, mes + 1, dia);
      w = SD.open(nombre, FILE_APPEND);
    }
    if (w)
    {
      size_t escritos = w.print(texto);
      escritos += w.print("\n\n");
      w.flush();
      w.close();
      if (escritos == esperado)
      {
        ok = true;
        Serial.println("M8AX - Telegram Guardado En La SD...");
      }
      else
        Serial.printf("M8AX - SD: Escritura Incompleta ( %u De %u Bytes ) - Intento %d De 3\n", (unsigned)escritos, (unsigned)esperado, intento);
    }
    else
      Serial.printf("M8AX - SD: No Se Pudo Crear La Carpeta O Abrir El Archivo ( Intento %d De 3 )\n", intento);
    SD.end();
    sdSPI.end();
    hSPI.begin(TOUCH_CLK, TOUCH_MISO, TOUCH_MOSI, ETOUCH_CS);
    pinMode(SD_CS, OUTPUT);
    digitalWrite(SD_CS, HIGH); // devuelve el bus al táctil
    sdOk = false;
    if (!ok)
      delay(200);
  }
  if (!ok)
    Serial.printf("M8AX - ###### NO SE PUDO GUARDAR EN LA SD ###### ( Heap Libre %u | Max Bloque %u )\n", (unsigned)ESP.getFreeHeap(), (unsigned)ESP.getMaxAllocHeap());
  return ok;
}

// Cola de mensajes que esperan a la SD. Cada mensaje vive en el heap solo hasta que se graba
static void sdEncola(const String &t)
{
  if (sdColaN >= SD_COLA_MAX)
  {
    Serial.println("M8AX - SD: Cola Llena, Se Descarta El Mensaje Más Antiguo...");
    delete sdCola[0];
    for (uint8_t i = 1; i < SD_COLA_MAX; i++)
      sdCola[i - 1] = sdCola[i];
    sdCola[--sdColaN] = nullptr;
  }
  String *p = new (std::nothrow) String(t);
  if (!p || p->length() != t.length())
  {
    Serial.println("M8AX - SD: Sin RAM Para La Cola, Mensaje Perdido...");
    delete p;
    return;
  }
  sdCola[sdColaN++] = p;
}

// Graba lo que haya en cola, el más antiguo primero. Se para en el primer fallo y se reintenta luego
static void sdVuelcaCola()
{
  while (sdColaN)
  {
    if (!sdGuardaTelegram(*sdCola[0]))
    {
      sdColaUltimo = millis();
      Serial.printf("M8AX - SD: %u Mensaje(s) En Cola, Se Reintentará...\n", (unsigned)sdColaN);
      return;
    }
    delete sdCola[0];
    for (uint8_t i = 1; i < sdColaN; i++)
      sdCola[i - 1] = sdCola[i];
    sdCola[--sdColaN] = nullptr;
  }
}

/***************************************************************************
 *  SERVIDOR WEB DE LA MICROSD
 *
 *  Monta la SD solo cuando entra una petición y la desmonta al terminar,
 *  así el bus SPI vuelve al táctil. Lista carpetas primero y archivos
 *  después, con tamaño en B / KB / MB / GB, fecha de modificación,
 *  migas de pan para navegar y un pie con espacio usado y libre de la
 *  tarjeta y datos del ESP32. El CSS vive en flash y todo se manda por
 *  trozos directamente desde buffers de pila, sin String ni copias.
 ***************************************************************************/
static WebServer web(80);

#define WEB_USER "M8AX-Minero"
#define WEB_PASS "MinaTuPasta"
#define WEB_RAM_MIN 30000

static const char WEB_CSS[] PROGMEM =
    "<!doctype html><meta charset=utf-8>"
    "<meta name=viewport content='width=device-width,initial-scale=1'>"
    "<title>M8AX-NerdminerV2-PG-M8AX &middot; MicroSD</title>"
    "<link id=ic rel=icon>"
    "<style>"
    "body{margin:0;font:15px system-ui,sans-serif;background:#0f1419;color:#d7dde4;position:relative;overflow-x:hidden}"
    "#network{position:fixed;inset:0;width:100%;height:100%;pointer-events:none;z-index:0}"
    "main{max-width:760px;margin:auto;padding:14px;position:relative;z-index:1}"
    "h1{font-size:18px;margin:0 0 10px;color:#f7931a}"
    "a{color:#6cb6ff;text-decoration:none}"
    ".m{padding:8px 10px;background:#1a212b;border-radius:8px;margin-bottom:8px;word-break:break-all}"
    ".r{display:flex;align-items:center;gap:8px;padding:9px 10px;border-bottom:1px solid #222b36}"
    ".r:hover{background:#1a212b}"
    ".b{display:inline-block;padding:6px 12px;background:#f7931a;color:#0f1419;border-radius:6px;cursor:pointer;margin-right:8px}"
    ".n{flex:1;min-width:0;overflow:hidden;text-overflow:ellipsis;white-space:nowrap}"
    ".s{color:#8b98a5;font-size:12px;white-space:nowrap}"
    "@media(max-width:560px){"
    ".r{flex-wrap:wrap}"
    ".n{flex:1 1 calc(100% - 60px)}"
    ".n+.s{margin-left:28px}"
    "}"
    ".f{margin-top:12px;color:#8b98a5;font-size:12px;line-height:1.7}"
    ".h{display:flex;align-items:center;gap:12px;margin-bottom:10px}"
    ".h img{height:42px}.h h1{margin:0}"
    ".h a{display:flex;align-items:center;cursor:pointer}"
    "</style>"
    "<canvas id=network></canvas>"
    "<main><div class=h>"
    "<a id=ml target=_blank rel=noopener>"
    "<img id=lg onerror=\"this.style.display='none'\">"
    "</a>"
    "<h1>M8AX-NerdminerV2-PG-M8AX &middot; MicroSD</h1></div>"
    "<script>"

    /* LOGOTIPO Y FAVICON */
    "var u='https://m8ax.github.io/MvIiIaX-NerdMiner_V2-DeV/webflash/m8axweb'+(1+Math.floor(Math.random()*2))+'.png';"
    "document.getElementById('lg').src=u;document.getElementById('ic').href=u;"

    /* WEBS ALEATORIAS */
    "var webs=["
    "'https://youtube.com/m8ax',"
    "'https://opensea.io/es/m8ax',"
    "'https://oncyber.io/m8ax',"
    "'https://m8ax.github.io/MvIiIaX-NerdMiner_V2-DeV/',"
    "'https://github.com/m8ax',"
    "'https://odysee.com/@m8ax'"
    "];"
    "document.getElementById('ml').href=webs[Math.floor(Math.random()*webs.length)];"
    "function bx(a,f){"
    "var n=a.parentNode.querySelector('a.n'),p=new URL(n.href).searchParams.get(f?'d':'p');"
    "if(!confirm((f?'Borrar Carpeta Y Todo Su Contenido:\\n':'Borrar Archivo:\\n')+p))return false;"
    "fetch('/x?p='+encodeURIComponent(p),{method:'POST'})"
    ".then(function(r){return r.text().then(function(t){if(!r.ok)alert(t);location.reload()})})"
    ".catch(function(){alert('Error')});"
    "return false}"
    "function sb(i){"
    "var fs=i.files,t=fs.length,n=0,d=i.closest('.m').dataset.d,u=document.getElementById('us');"
    "if(!t)return;"
    "function s(){"
    "if(n>=t){location.reload();return}"
    "u.textContent='Subiendo '+(n+1)+'/'+t+' ...';"
    "var f=fs[n],fd=new FormData();fd.append('f',f,f.name);"
    "fetch('/u?d='+d+'&r='+encodeURIComponent(f.webkitRelativePath||f.name),{method:'POST',body:fd})"
    ".then(function(r){if(r.ok){n++;s()}else r.text().then(function(m){alert(m);location.reload()})})"
    ".catch(function(){alert('Error');location.reload()})}"
    "s()}"
    "function rm(){"
    "if(!confirm('¿Reiniciar El Minero?'))return;"
    "fetch('/r',{method:'POST'})"
    ".then(function(){document.body.innerHTML='<p style=padding:20px>Reiniciando... Recarga La Página En Unos 30 Segundos...<br><br><br>Mientras... Si Te Apetece, Puedes Ver Mi Canal De Youtube Y Suscribirte xD - <a href=\"https://youtube.com/m8ax\" target=\"_blank\" rel=\"noopener noreferrer\">https://youtube.com/m8ax</a></p>'})"
    ".catch(function(){alert('Error')})}"

    /* NODOS ANIMADOS */
    "(()=>{"
    "const c=document.getElementById('network');"
    "const ctx=c.getContext('2d');"
    "let w=0,h=0,nodes=[];"
    "const colors=['#f7931a','#6cb6ff','#00e5ff','#a78bfa'];"
    "const reduced=window.matchMedia('(prefers-reduced-motion: reduce)').matches;"
    "function resize(){"
    "const dpr=Math.min(window.devicePixelRatio||1,1.5);"
    "w=window.innerWidth;h=window.innerHeight;"
    "c.width=w*dpr;c.height=h*dpr;"
    "ctx.setTransform(dpr,0,0,dpr,0,0);"
    "const count=Math.min(40,Math.max(15,Math.floor(w*h/18000)));"
    "nodes=Array.from({length:count},()=>({"
    "x:Math.random()*w,y:Math.random()*h,"
    "vx:(Math.random()-.5)*.35,vy:(Math.random()-.5)*.35,"
    "r:1.3+Math.random()*1.5,"
    "color:colors[Math.floor(Math.random()*colors.length)]"
    "}));"
    "}"
    "function draw(){"
    "ctx.clearRect(0,0,w,h);"
    "for(let i=0;i<nodes.length;i++){"
    "let a=nodes[i];"
    "a.x+=a.vx;a.y+=a.vy;"
    "if(a.x<0||a.x>w)a.vx*=-1;"
    "if(a.y<0||a.y>h)a.vy*=-1;"
    "for(let j=i+1;j<nodes.length;j++){"
    "let b=nodes[j];"
    "let dx=a.x-b.x,dy=a.y-b.y;"
    "let dist=Math.sqrt(dx*dx+dy*dy);"
    "if(dist<145){"
    "ctx.beginPath();"
    "ctx.moveTo(a.x,a.y);ctx.lineTo(b.x,b.y);"
    "ctx.strokeStyle='rgba(108,182,255,'+(0.20*(1-dist/145))+')';"
    "ctx.lineWidth=.7;ctx.stroke();"
    "}"
    "}"
    "ctx.beginPath();ctx.arc(a.x,a.y,a.r,0,Math.PI*2);"
    "ctx.fillStyle=a.color;ctx.globalAlpha=.75;ctx.fill();ctx.globalAlpha=1;"
    "}"
    "if(!reduced)requestAnimationFrame(draw);"
    "}"
    "resize();draw();"
    "window.addEventListener('resize',resize,{passive:true});"
    "})();"
    "</script>";

static bool rutaOk(const char *p) { return p[0] == '/' && !strstr(p, ".."); }

static void webEnv(const char *s) { web.sendContent(s, strlen(s)); }

static void webSdCierra()
{
  SD.end();
  sdSPI.end();
  hSPI.begin(TOUCH_CLK, TOUCH_MISO, TOUCH_MOSI, ETOUCH_CS);
  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, HIGH); // devuelve el bus al táctil
  sdOk = false;
}

static void webTam(char *o, size_t n, uint64_t b)
{
  if (b < 1024)
    snprintf(o, n, "%u B", (unsigned)b);
  else if (b < 1048576ULL)
    snprintf(o, n, "%.1f KB", b / 1024.0);
  else if (b < 1073741824ULL)
    snprintf(o, n, "%.2f MB", b / 1048576.0);
  else
    snprintf(o, n, "%.2f GB", b / 1073741824.0);
}

static void webFecha(char *o, size_t n, time_t t)
{
  o[0] = 0;
  if (t < 1000000000)
    return; // reloj sin poner
  struct tm tm;
  localtime_r(&t, &tm);
  strftime(o, n, "%d/%m/%y %H:%M", &tm);
}

static void webEsc(char *o, size_t n, const char *s)
{
  size_t i = 0;
  for (; *s && i + 7 < n; s++)
  {
    const char *r = *s == '&' ? "&amp;" : *s == '<' ? "&lt;"
                                      : *s == '>'   ? "&gt;"
                                      : *s == '\''  ? "&#39;"
                                                    : nullptr;
    if (r)
    {
      strcpy(o + i, r);
      i += strlen(r);
    }
    else
      o[i++] = *s;
  }
  o[i] = 0;
}

static void webUrl(char *o, size_t n, const char *s)
{
  size_t i = 0;
  for (; *s && i + 4 < n; s++)
  {
    if (isalnum((uint8_t)*s) || strchr("-_./", *s))
      o[i++] = *s;
    else
      i += snprintf(o + i, 4, "%%%02X", (uint8_t)*s);
  }
  o[i] = 0;
}

static bool webBorraTodo(char *p, size_t n)
{
  size_t base = strlen(p);
  for (;;)
  {
    File d = SD.open(p);
    if (!d || !d.isDirectory())
    {
      if (d)
        d.close();
      return false;
    }
    File f = d.openNextFile();
    if (!f)
    {
      d.close();
      if (!SD.rmdir(p))
        return false;
      if (strlen(p) <= base)
        return true;
      *strrchr(p, '/') = 0;
      continue;
    }
    bool esDir = f.isDirectory();
    const char *fn = f.name();
    const char *bn = strrchr(fn, '/');
    bn = bn ? bn + 1 : fn;
    size_t a = strlen(p);
    if (a + 1 + strlen(bn) >= n)
    {
      f.close();
      d.close();
      return false;
    }
    snprintf(p + a, n - a, "/%s", bn);
    f.close();
    d.close();
    if (esDir)
      continue;
    if (!SD.remove(p))
      return false;
    p[a] = 0;
    yield();
  }
}

static void webBorrar()
{
  if (!web.authenticate(WEB_USER, WEB_PASS))
    return web.requestAuthentication();
  char p[128];
  strlcpy(p, web.arg("p").c_str(), sizeof(p));
  size_t l = strlen(p);
  while (l > 1 && p[l - 1] == '/')
    p[--l] = 0;
  if (!rutaOk(p) || l < 2)
    return web.send(400, "text/plain; charset=utf-8", "Ruta No Valida");
  if (!sdMonta())
    return web.send(503, "text/plain; charset=utf-8", "No Hay MicroSD");
  File f = SD.open(p);
  if (!f)
  {
    webSdCierra();
    return web.send(404, "text/plain; charset=utf-8", "No Existe");
  }
  bool esDir = f.isDirectory();
  f.close();
  bool ok = esDir ? webBorraTodo(p, sizeof(p)) : SD.remove(p);
  webSdCierra();
  web.send(ok ? 200 : 500, "text/plain; charset=utf-8", ok ? "OK" : "No Se Ha Podido Borrar");
}

static File webSubF;
static bool webSubFallo = false;

static bool webSubRuta(char *o, size_t n, const char *fn)
{
  char dir[128] = "/", rel[128];
  if (web.hasArg("d"))
    strlcpy(dir, web.arg("d").c_str(), sizeof(dir));
  if (!rutaOk(dir))
    return false;
  size_t dl = strlen(dir);
  while (dl > 1 && dir[dl - 1] == '/')
    dir[--dl] = 0;
  strlcpy(rel, web.hasArg("r") ? web.arg("r").c_str() : fn, sizeof(rel));
  for (char *s = rel; *s; s++)
    if (*s == '\\')
      *s = '/';
  const char *r = rel;
  while (*r == '/')
    r++;
  if (!*r || r[strlen(r) - 1] == '/' || strstr(r, "//") || strpbrk(r, "\"*:<>?|"))
    return false;
  for (const char *s = r; *s;)
  {
    const char *e = strchr(s, '/');
    size_t ln = e ? (size_t)(e - s) : strlen(s);
    if ((ln == 1 && s[0] == '.') || (ln == 2 && s[0] == '.' && s[1] == '.'))
      return false;
    s = e ? e + 1 : s + ln;
  }
  int w = snprintf(o, n, "%s%s%s", dir, dl > 1 ? "/" : "", r);
  return w > 0 && (size_t)w < n;
}

static void webSubida()
{
  HTTPUpload &up = web.upload();
  char ruta[128];
  if (up.status == UPLOAD_FILE_START)
  {
    if (webSubF)
      webSubF.close();
    if (!web.authenticate(WEB_USER, WEB_PASS) || !sdMonta() || !webSubRuta(ruta, sizeof(ruta), up.filename.c_str()))
    {
      webSubFallo = true;
      return;
    }
    for (char *s = ruta + 1; *s; s++)
      if (*s == '/')
      {
        *s = 0;
        if (!SD.exists(ruta))
          SD.mkdir(ruta);
        *s = '/';
      }
    webSubF = SD.open(ruta, FILE_WRITE);
    if (!webSubF)
      webSubFallo = true;
  }
  else if (up.status == UPLOAD_FILE_WRITE)
  {
    if (webSubF && webSubF.write(up.buf, up.currentSize) != up.currentSize)
    {
      webSubFallo = true;
      webSubF.close();
      if (webSubRuta(ruta, sizeof(ruta), up.filename.c_str()))
        SD.remove(ruta);
    }
  }
  else if (up.status == UPLOAD_FILE_END)
  {
    if (webSubF)
      webSubF.close();
  }
  else if (up.status == UPLOAD_FILE_ABORTED)
  {
    webSubFallo = true;
    if (webSubF)
    {
      webSubF.close();
      if (webSubRuta(ruta, sizeof(ruta), up.filename.c_str()))
        SD.remove(ruta);
    }
    if (sdOk)
      webSdCierra(); // si se corta la subida, devuelve el bus SPI al táctil
  }
}

static void webSubidaFin()
{
  bool fallo = webSubFallo;
  webSubFallo = false;
  if (webSubF)
    webSubF.close();
  if (sdOk)
    webSdCierra();
  if (!web.authenticate(WEB_USER, WEB_PASS))
    return web.requestAuthentication();
  if (fallo)
    return web.send(500, "text/plain; charset=utf-8", "Fallo Al Subir ( Nombre No Valido, SD Llena O Sin SD )");
  web.send(200, "text/plain; charset=utf-8", "OK");
}

static void webListar()
{
  if (!web.authenticate(WEB_USER, WEB_PASS))
    return web.requestAuthentication();
  if (!sdMonta())
    return web.send(200, "text/plain; charset=utf-8", "No Hay MicroSD");
  char dir[128] = "/";
  if (web.hasArg("d"))
    strlcpy(dir, web.arg("d").c_str(), sizeof(dir));
  if (!rutaOk(dir))
  {
    webSdCierra();
    return web.send(400, "text/plain; charset=utf-8", "Ruta No Válida");
  }
  size_t dl = strlen(dir);
  if (dl > 1 && dir[dl - 1] == '/')
    dir[dl - 1] = 0;
  File root = SD.open(dir);
  if (!root || !root.isDirectory())
  {
    if (root)
      root.close();
    webSdCierra();
    return web.send(404, "text/plain; charset=utf-8", "No Existe");
  }
  web.setContentLength(CONTENT_LENGTH_UNKNOWN);
  web.send(200, "text/html; charset=utf-8", "");
  web.sendContent(WEB_CSS, sizeof(WEB_CSS) - 1);

  char ruta[128], enc[192], nom[64], tam[16], tar[16], u[16], v[16], l[600];

  webEnv("<div class=m><a href='/'>&#128190; SD</a>");
  ruta[0] = 0; // aquí hace de acumulador de la miga
  for (const char *p = dir + 1; *p;)
  {
    const char *e = strchr(p, '/');
    size_t ln = e ? (size_t)(e - p) : strlen(p);
    if (ln)
    {
      char seg[40];
      strlcpy(seg, p, ln + 1 < sizeof(seg) ? ln + 1 : sizeof(seg));
      size_t a = strlen(ruta);
      snprintf(ruta + a, sizeof(ruta) - a, "/%s", seg);
      webUrl(enc, sizeof(enc), ruta);
      webEsc(nom, sizeof(nom), seg);
      snprintf(l, sizeof(l), " / <a href='/?d=%s'>%s</a>", enc, nom);
      webEnv(l);
    }
    p = e ? e + 1 : p + ln;
  }
  webEnv("</div>");
  webUrl(enc, sizeof(enc), dir);
  snprintf(l, sizeof(l), "<div class=m data-d='%s'>", enc);
  webEnv(l);
  webEnv("<span class=b style='float:right;margin:0;background:#d32f2f;color:#fff' onclick='return rm()'>Reiniciar Minero</span>");
  webEnv("<label class=b>Subir Archivos<input type=file multiple hidden onchange='sb(this)'></label>"
         "<label class=b>Subir Carpeta<input type=file webkitdirectory hidden onchange='sb(this)'></label>"
         "<b id=us></b></div>");

  if (strcmp(dir, "/"))
  {
    strlcpy(ruta, dir, sizeof(ruta));
    char *s = strrchr(ruta, '/');
    if (s == ruta)
      ruta[1] = 0;
    else
      *s = 0;
    webUrl(enc, sizeof(enc), ruta);
    snprintf(l, sizeof(l), "<div class=r><span>&#11014;</span><a class=n href='/?d=%s'>..</a></div>", enc);
    webEnv(l);
  }

  int nd = 0, nf = 0;
  uint64_t tot = 0;
  for (int pasada = 0; pasada < 2; pasada++) // primero carpetas, luego archivos
  {
    root.rewindDirectory();
    while (nd + nf < 200)
    {
      File f = root.openNextFile();
      if (!f)
        break;
      bool d = f.isDirectory();
      if (d == (pasada == 0))
      {
        snprintf(ruta, sizeof(ruta), "%s%s%s", dir, strcmp(dir, "/") ? "/" : "", f.name());
        webUrl(enc, sizeof(enc), ruta);
        webEsc(nom, sizeof(nom), f.name());
        webFecha(u, sizeof(u), f.getLastWrite());
        if (d)
        {
          nd++;
          snprintf(l, sizeof(l),
                   "<div class=r><span>&#128193;</span><a class=n href='/?d=%s'>%s</a>"
                   "<span class=s>%s</span>"
                   "<a href='#' title=Borrar onclick=\"return bx(this,1)\">&#128465;</a></div>",
                   enc, nom, u);
        }
        else
        {
          nf++;
          tot += f.size();
          webTam(tam, sizeof(tam), f.size());
          snprintf(l, sizeof(l),
                   "<div class=r><span>&#128196;</span><a class=n href='/f?p=%s'>%s</a>"
                   "<span class=s>%s</span><span class=s>%s</span>"
                   "<a href='/f?p=%s&dl=1' title=Descargar>&#11015;</a>"
                   "<a href='#' title=Borrar onclick=\"return bx(this,0)\">&#128465;</a></div>",
                   enc, nom, u, tam, enc);
        }
        webEnv(l);
      }
      f.close();
    }
  }
  root.close();

  uint64_t tb = SD.totalBytes(), ub = SD.usedBytes();
  webTam(tam, sizeof(tam), tot);
  webTam(tar, sizeof(tar), tb);
  webTam(u, sizeof(u), ub);
  webTam(v, sizeof(v), tb > ub ? tb - ub : 0);
  const char *tipo = SD.cardType() == CARD_MMC ? "MMC" : SD.cardType() == CARD_SD ? "SDSC"
                                                                                  : "SDHC/SDXC";
  unsigned long min = (unsigned long)(esp_timer_get_time() / 60000000ULL);
  snprintf(l, sizeof(l),
           "<div class=f>%d Carpetas &middot; %d Archivos &middot; %s En Esta Carpeta<br>"
           "Tarjeta %s %s &middot; Usado %s &middot; Libre %s<br>"
           "RAM Libre %u KB &middot; WiFi %d dBm &middot; Activo %lud %02luh %02lum</div></main>",
           nd, nf, tam, tipo, tar, u, v, (unsigned)(ESP.getFreeHeap() / 1024), WiFi.RSSI(), min / 1440, (min / 60) % 24, min % 60);
  webEnv(l);
  web.sendContent("");
  webSdCierra();
}

static void webArchivo()
{
  if (!web.authenticate(WEB_USER, WEB_PASS))
    return web.requestAuthentication();
  if (!sdMonta())
    return web.send(200, "text/plain; charset=utf-8", "No Hay MicroSD");
  char p[128];
  strlcpy(p, web.arg("p").c_str(), sizeof(p));
  if (!rutaOk(p))
  {
    webSdCierra();
    return web.send(400, "text/plain; charset=utf-8", "Ruta No Válida");
  }
  File f = SD.open(p, FILE_READ);
  if (!f || f.isDirectory())
  {
    if (f)
      f.close();
    webSdCierra();
    return web.send(404, "text/plain; charset=utf-8", "No Existe");
  }
  if (web.hasArg("dl"))
  {
    char cab[160];
    snprintf(cab, sizeof(cab), "attachment; filename=\"%s\"", strrchr(p, '/') + 1);
    web.sendHeader("Content-Disposition", cab);
    web.streamFile(f, "application/octet-stream");
  }
  else
    web.streamFile(f, "text/plain; charset=utf-8");
  f.close();
  webSdCierra();
}

static void webReinicia()
{
  if (!web.authenticate(WEB_USER, WEB_PASS))
    return web.requestAuthentication();
  web.send(200, "text/plain; charset=utf-8", "Reiniciando");
  delay(500); // da tiempo a que la respuesta llegue al navegador
  ESP.restart();
}

void webAtender()
{
  if (ESP.getMaxAllocHeap() < WEB_RAM_MIN)
    return;
  static bool iniciado = false;
  if (!iniciado)
  {
    if (WiFi.status() != WL_CONNECTED)
      return;
    web.on("/", webListar);
    web.on("/f", webArchivo);
    web.on("/x", HTTP_POST, webBorrar);
    web.on("/u", HTTP_POST, webSubidaFin, webSubida);
    web.on("/r", HTTP_POST, webReinicia);
    web.begin();
    iniciado = true;
    Serial.printf("\nM8AX - Servidor Web De La SD En http://%s\n", WiFi.localIP().toString().c_str());
  }
  web.handleClient();
}

String bloqueCriptos();

void wxHistMuestrea(void);

String cifrasTelegram();

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
  int indice = esp_random() % 6;
  // Formatear la hora en "00:00:00"
  char horaFormateada[9];
  sprintf(horaFormateada, "%02d:%02d:%02d", horita, minutitos, segundos);
  // Formatear la fecha en "dia/mes/año"
  char fechaFormateada[11];
  sprintf(fechaFormateada, "%02d/%02d/%04d", dia, mes, anio);
  String LUNAP = lunitaporc(fechaFormateada, horaFormateada);
  prebitco = preciob().c_str();
  alturabloque = alturab().c_str();
  int altura = 1;
  int bloquesPorHalving = 210000;
  int bloquesCompletados = 1;
  int bloquesRestantes = 1;
  float porcentaje = 0.00;
  if (alturabloque.length() > 0)
  {
    altura = alturabloque.toInt();
    bloquesCompletados = altura % bloquesPorHalving;
    porcentaje = (bloquesCompletados * 100.0) / bloquesPorHalving;
  }
  // Extraer los últimos 4 dígitos de la mac
  uint8_t mac[6];
  WiFi.macAddress(mac);
  char u4digits[5];
  sprintf(u4digits, "%02X%02X", mac[4], mac[5]);
  std::string quediase = obtenerDiaSemana(std::string(fechaFormateada));
  String cadenaEnvio;
  cadenaEnvio.reserve(4000);
  cadenaEnvio = "";
  cadenaEnvio = F("------------------------------------------------------------------------------------------------\n");
  cadenaEnvio += "----------------------- M8AX - NerdMinerV2PG-" + String(u4digits) + " DATOS DE MINERÍA - M8AX ----------------------\n";
  cadenaEnvio += "----------------------------------- " + String(fechaFormateada) + " " + quediase.c_str() + " - " + horaFormateada + " ----------------------------------\n";
  cadenaEnvio += F("------------------------------------------------------------------------------------------------\n");
  quediase.clear(), quediase.shrink_to_fit();
  if (sumatele <= 3999)
  {
    cadenaEnvio += "Mensaje Número - " + convertirARomanos(sumatele) + " | F.C.FW - " + String(__DATE__) + " A Las " + String(__TIME__) + " | Hace - " + String(convertirTiempo(segundosDesdeCompilacion())).c_str() + "\n";
  }
  else
  {
    cadenaEnvio += "Mensaje Número - " + String(sumatele) + " | F.C.FW - " + String(__DATE__) + " A Las " + String(__TIME__) + " | Hace - " + String(convertirTiempo(segundosDesdeCompilacion())).c_str() + "\n";
  }
  if (Tresultado.first != "" && Tresultado.second != "")
  {
    Serial.println("M8AX - Ciudad: " + Tresultado.first);
    Serial.println("M8AX - Temperatura: " + Tresultado.second);
  }
  else
  {
    Tresultado.first = "Quien Sabe Donde";
    Tresultado.second = "xxC";
    Serial.println("M8AX - No Se Pudo Obtener La Ciudad O La Temperatura.");
  }
  cadenaEnvio += quitarAcentos(Tresultado.first) + ", " + Tresultado.second;
  if (salidaypuesta[0])
  {
    cadenaEnvio += String(" | ") + salidaypuesta;
    Serial.printf("M8AX - %s\n", salidaypuesta);
  }
  cadenaEnvio += String(" | UTC ") + ((zonilla >= 0) ? "+" : "") + String(zonilla) + " | " + LUNAP + "\n";
  cadenaEnvio += "Tiempo Minando - " + (mineria.timeMining.substring(0, mineria.timeMining.indexOf(" ")).length() == 1 ? "0" + mineria.timeMining.substring(0, mineria.timeMining.indexOf(" ")) : mineria.timeMining.substring(0, mineria.timeMining.indexOf(" "))) + " Días" + mineria.timeMining.substring(mineria.timeMining.indexOf(" ") + 1) + "\n";
  cadenaEnvio += "HR Actual - " + mineria.currentHashRate + " KH/s ( MAX - " + String(maxkh) + " | MIN - " + String(minkh) + " )\n";
  cadenaEnvio += "Temp. De CPU - " + mineria.temp + "° ( MAX - " + String(maxtemp) + "° | MIN - " + String(mintemp) + "° ) | " + obtenerEstadoTemperatura(mineria.temp.toInt()) + "\n";
  cadenaEnvio += "RAM Libre - " + String(ESP.getFreeHeap()) + " Bytes\n";
  cadenaEnvio += sdEspacioTexto() + "\n";
  cadenaEnvio += "Plantillas De Bloque - " + mineria.templates + "\n";
  cadenaEnvio += "Shares Enviados A La Pool - " + mineria.completedShares + "\n";
  cadenaEnvio += "Mejor Dificultad Alcanzada - " + mineria.bestDiff + "\n";
  cadenaEnvio += "Cómputo Total - " + mineria.totalKHashes + " KH - ( " + String(atof(mineria.totalKHashes.c_str()) / 1000, 3) + " MH )\n";
  cadenaEnvio += "Pool De Minería - " + Settings.PoolAddress + "\n";
  cadenaEnvio += "Puerto Del Pool - " + String(Settings.PoolPort) + "\n";
  cadenaEnvio += "Tu Wallet De BTC - " + String(Settings.BtcWallet) + "\n";
  cadenaEnvio += "Precio De BTC - " + String(prebitco) + " | Var. 1 Sem -> " + variacion1semana + " | Var. 1 Día -> " + variacion1dia + "\n";
  cadenaEnvio += "Bloques Minados - " + String(alturabloque) + "\n";
  bloquesCompletados = altura % bloquesPorHalving;
  bloquesRestantes = bloquesPorHalving - bloquesCompletados;
  cadenaEnvio += "Bloques Restantes Para Halving - " + String(bloquesRestantes) + "\n";
  cadenaEnvio += "Progreso Hacia El Halving - " + String(210000 - bloquesRestantes) + " Bloques | " + String(porcentaje, 2) + "% Completado\n";
  cadenaEnvio += "Tu IP - " + getPublicIP() + "\n";
  cadenaEnvio += "WiFi RSSI " + String(WiFi.RSSI()) + " | Señal 1 A 6 - " + evaluarRSSI(String(WiFi.RSSI())) + "\n";
  cadenaEnvio += urlsm8ax[indice];
  cadenaEnvio += F("\n------------------------------------------------------------------------------------------------\n");
  cadenaEnvio += bloqueCriptos();
  cadenaEnvio += F("------------------------------------------------------------------------------------------------\n");
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
  rndnumero = esp_random();
  cadenaEnvio += "Factorización De Número - " + String(rndnumero) + " -> " + factorize(rndnumero) + "\n";
  rndnumero = esp_random();
  cadenaEnvio += "Factorización De Número - " + String(rndnumero) + " -> " + factorize(rndnumero);
  cadenaEnvio += "\n------------------------------------------------------------------------------------------------\n";
  cadenaEnvio += cifrasTelegram();
  cadenaEnvio += "\n";
  cadenaEnvio += "------------------------------------------------------------------------------------------------\n                                       By M8AX Corp. " + convertirARomanos(anio);
  cadenaEnvio += F("\n------------------------------------------------------------------------------------------------");
  bool botOk = (BOT_TOKEN != "NO CONFIGURADO" && CHAT_ID != "NO CONFIGURADO");
  bool enviado = true;
  if (!sdGuardadoCiclo) // el mensaje del ciclo entra en la cola una sola vez: ya no se pierde aunque la SD falle
  {
    sdEncola(cadenaEnvio);
    sdGuardadoCiclo = true;
  }
  sdVuelcaCola();
  Serial.printf("M8AX - Tamaño Del Mensaje: %u Caracteres\n", (unsigned)cadenaEnvio.length());
  if (botOk)
    enviado = enviarMensajeATelegram(cadenaEnvio);
  else
    sumatele += 1;
  // segunda oportunidad para la SD, con la RAM ya liberada de Telegram: solo si antes falló
  if (sdColaN && !sdSinTarjeta) // ranura vacía: un solo intento por ciclo
    sdVuelcaCola();
  cadenaEnvio = "", cadenaEnvio.reserve(0); // ahora sí se libera, ya sin hacer falta
  return enviado;
}

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
    http.useHTTP10(true);
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

void ajustarZonaHoraria()
{
  int nueva = obtenHoraPorIP(getPublicIP());
  if (nueva == 1000)
    return;
  if (abs(Settings.Timezone - nueva) <= 1 && Settings.Timezone != nueva)
  {
    Settings.Timezone = nueva;
    nvMem.saveConfig(&Settings);
    Serial.println(String("M8AX - Cambiando TimeZone... Actualmente Es UTC ") + (Settings.Timezone >= 0 ? "+" : "") + String(Settings.Timezone));
    timeClient.setTimeOffset(3600 * Settings.Timezone);
  }
  zonilla = Settings.Timezone;
  Serial.println("M8AX - Combrobación De Ajuste De Hora ( TimeZone ), Correcta...");
}

void esp32_2432S028R_Init(void)
{
  getChipInfo();
  tft.init();
  if (nvMem.loadConfig(&Settings))
  {
    // Serial.print("Invert Colors: ");
    // Serial.println(Settings.invertColors);
    // invertColors= false; Si Pantallas Raras...
    // invertColors = Settings.invertColors; Si Pantallas Normales...
    invertColors = Settings.invertColors;
  }
  tft.invertDisplay(invertColors);
  tft.setRotation(1);
  // Swap the colour byte order when rendering
  tft.setSwapBytes(true);
  if (invertColors)
  {
    tft.writecommand(ILI9341_GAMMASET);
    tft.writedata(2);
    delay(120);
    // Gamma curve selected
    tft.writecommand(ILI9341_GAMMASET);
    tft.writedata(1);
  }
  hSPI.begin(TOUCH_CLK, TOUCH_MISO, TOUCH_MOSI, ETOUCH_CS);
  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, HIGH);
  touch.init();
  TFT_eTouchBase::Calibation calibation = {233, 3785, 3731, 120, 2};
  touch.setCalibration(calibation);
  // Configuring screen backlight brightness using ledcontrol channel 0.
  // Using 5000Hz in 8bit resolution, which gives 0-255 possible duty cycle setting.
  ledcSetup(0, 5000, 8);
  ledcAttachPin(TFT_BL, 0);
  ledcWrite(0, Settings.Brightness);
  // background.createSprite(WIDTH, HEIGHT); // Background Sprite
  // background.setSwapBytes(true);
  // render.setDrawer(background);  // Link drawing object to background instance (so font will be rendered on background)
  // render.setLineSpaceRatio(0.9); // Espaciado entre texto
  // Load the font and check it can be read OK
  // if (render.loadFont(NotoSans_Bold, sizeof(NotoSans_Bold)))
  if (render.loadFont(DigitalNumbers, sizeof(DigitalNumbers)))
  {
    Serial.println("M8AX - Inicialización Errónea");
    return;
  }
  pinMode(LED_PIN, OUTPUT);
  pinMode(LED_PIN_B, OUTPUT);
  pinMode(LED_PIN_G, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  digitalWrite(LED_PIN_B, HIGH);
  digitalWrite(LED_PIN_G, HIGH);
  pData.bestDifficulty = "0";
  pData.workersHash = "0";
  pData.workersCount = 0;
  // Serial.println("=========== Fim Display ==============") ;
}

void esp32_2432S028R_AlternateScreenState(void)
{
  Serial.println("M8AX - Cambiando Estado De Pantalla");
  int screen_state_duty = ledcRead(0);
  // Switching the duty cycle for the ledc channel, where the TFT_BL pin is attached.
  if (screen_state_duty > 0)
  {
    ledcWrite(0, 0);
  }
  else
  {
    ledcWrite(0, Settings.Brightness);
  }
}

void esp32_2432S028R_AlternateRotation(void)
{
  tft.setRotation(flipRotation(tft.getRotation()));
  hasChangedScreen = true;
}

bool bottomScreenBlue = true;

void printheap()
{
  Serial.printf("M8AX - $$ Free Heap: %u  Max Bloque: %u\n", ESP.getFreeHeap(), ESP.getMaxAllocHeap());
}

bool createBackgroundSprite(int16_t wdt, int16_t hgt)
{
  // Si ya hay un sprite de otro tamaño, se libera. Si es del mismo tamaño, se reutiliza.
  if (background.created() && (background.width() != wdt || background.height() != hgt))
    background.deleteSprite();

  // Si no hay RAM contigua, reintenta hasta ~3 s (deja que termine el TLS de otra tarea)
  for (int intento = 0; intento < 30 && !background.created(); intento++)
  {
    background.setColorDepth(16); // antes de crear, nunca con el sprite ya creado
    background.createSprite(wdt, hgt);
    if (!background.created())
      vTaskDelay(pdMS_TO_TICKS(100));
  }

  if (background.created())
  {
    background.setSwapBytes(true);
    render.setDrawer(background);
    render.setLineSpaceRatio(0.9);
  }
  else
  {
    Serial.println("M8AX - #### Sprite Error ####");
    Serial.printf("M8AX - Size W:%d H:%d \n", wdt, hgt);
    printheap();
  }
  return background.created();
}

extern unsigned long mPoolUpdate;

struct Cripto
{
  const char *par;     // par de Binance, ej. BTCUSDT
  const char *simbolo; // texto que se ve en pantalla
  float precio;        // último precio (0 = sin datos)
  int8_t tendencia;    // 0 = sin datos, 1 = sube, -1 = baja
};

const int NUM_CRIPTOS = 18; // 3 páginas de 6
int paginaCriptos = 0;      // página que se está enseñando (0, 1 o 2)

// Cada página tiene 6 monedas: [recuadro 1: 2] [recuadro 2: 2] [recuadro 3: 2]
Cripto criptos[NUM_CRIPTOS] = {
    // Página 0
    {"BTCUSDT", "BTC", 0},
    {"ETHUSDT", "ETH", 0},
    {"XRPUSDT", "XRP", 0},
    {"ADAUSDT", "ADA", 0},
    {"SOLUSDT", "SOL", 0},
    {"BNBUSDT", "BNB", 0},
    // Página 1
    {"DOGEUSDT", "DOGE", 0},
    {"TRXUSDT", "TRX", 0},
    {"AVAXUSDT", "AVAX", 0},
    {"LINKUSDT", "LINK", 0},
    {"SUIUSDT", "SUI", 0},
    {"DOTUSDT", "DOT", 0},
    // Página 2
    {"LTCUSDT", "LTC", 0},
    {"BCHUSDT", "BCH", 0},
    {"XLMUSDT", "XLM", 0},
    {"UNIUSDT", "UNI", 0},
    {"NEARUSDT", "NEAR", 0},
    {"ATOMUSDT", "ATOM", 0}};

unsigned long ultimaActualizacionCriptos = 0;
unsigned long ultimoOkCriptos = 0;

bool obtenerPreciosCriptos()
{
  if (WiFi.status() != WL_CONNECTED)
    return false;

  String query = "/api/v3/ticker/price?symbols=%5B";
  for (int i = 0; i < NUM_CRIPTOS; i++)
  {
    query += "%22";
    query += criptos[i].par;
    query += "%22";
    if (i < NUM_CRIPTOS - 1)
      query += ",";
  }
  query += "%5D";

  const char *hosts[] = {"https://api.binance.com", "https://data-api.binance.vision"};
  for (int h = 0; h < 2; h++)
  {
    WiFiClientSecure client;
    client.setInsecure();
    HTTPClient http;
    http.setTimeout(2500);
    http.setConnectTimeout(2500);
    if (!http.begin(client, String(hosts[h]) + query))
      continue;
    int httpCode = http.GET();
    if (httpCode == HTTP_CODE_OK)
    {
      String payload = http.getString();
      http.end();
      DynamicJsonDocument doc(4096);
      if (!deserializeJson(doc, payload))
      {
        bool ok = false;
        for (JsonObject o : doc.as<JsonArray>())
        {
          const char *sym = o["symbol"];
          const char *pr = o["price"];
          if (!sym || !pr)
            continue;
          for (int j = 0; j < NUM_CRIPTOS; j++)
          {
            if (strcmp(sym, criptos[j].par) == 0)
            {
              float nuevo = atof(pr);
              if (criptos[j].precio > 0)
              {
                if (nuevo > criptos[j].precio)
                  criptos[j].tendencia = 1;
                else if (nuevo < criptos[j].precio)
                  criptos[j].tendencia = -1;
              }
              criptos[j].precio = nuevo;
              ok = true;
            }
          }
        }
        if (ok)
        {
          ultimoOkCriptos = millis();
          Serial.println("M8AX - Precios De Criptos Actualizados (Binance)");
          for (int j = 0; j < NUM_CRIPTOS; j++)
          {
            const char *flecha = criptos[j].tendencia > 0 ? "SUBE" : (criptos[j].tendencia < 0 ? "BAJA" : "----");
            Serial.printf("  %-5s %12.4f USDT  %s\n", criptos[j].simbolo, criptos[j].precio, flecha);
          }
          return true;
        }
      }
    }
    else
    {
      Serial.printf("M8AX - Error HTTP Binance (%s): %d\n", hosts[h], httpCode);
      http.end();
    }
  }
  return false;
}

String formatoPrecio(float p)
{
  if (p <= 0)
    return "---";
  char buf[16];
  if (p >= 1000)
    snprintf(buf, sizeof(buf), "%.0f", p);
  else if (p >= 100)
    snprintf(buf, sizeof(buf), "%.1f", p);
  else if (p >= 10)
    snprintf(buf, sizeof(buf), "%.2f", p);
  else if (p >= 1)
    snprintf(buf, sizeof(buf), "%.3f", p);
  else
    snprintf(buf, sizeof(buf), "%.4f", p);
  return String(buf);
}

// Bloque de precios para Telegram y la SD: 6 columnas x 3 filas, todas las celdas del mismo ancho y centradas en 96 caracteres
String bloqueCriptos()
{
  const int ANCHO = 96;
  const int COLS = 6;
  const int CELDA = ANCHO / COLS; // 16 caracteres por moneda
  int wSim = 0, wPre = 0;
  for (int i = 0; i < NUM_CRIPTOS; i++)
  {
    int a = strlen(criptos[i].simbolo);
    int b = formatoPrecio(criptos[i].precio).length();
    if (a > wSim)
      wSim = a;
    if (b > wPre)
      wPre = b;
  }
  int wCelda = wSim + 1 + wPre;
  if ((CELDA - wCelda) % 2) // que sobre lo mismo a izquierda y derecha
  {
    wPre++;
    wCelda++;
  }
  int margen = (CELDA - wCelda) / 2;
  if (margen < 0)
    margen = 0;

  String r;
  r.reserve(450);
  String t = " M8AX - PRECIOS CRIPTO (USDT) - M8AX ";
  int izq = (ANCHO - (int)t.length()) / 2;
  for (int i = 0; i < izq; i++)
    r += '-';
  r += t;
  for (int i = 0; i < ANCHO - izq - (int)t.length(); i++)
    r += '-';
  r += "\n------------------------------------------------------------------------------------------------\n";
  for (int fila = 0; fila < NUM_CRIPTOS / COLS; fila++)
  {
    for (int col = 0; col < COLS; col++)
    {
      Cripto &c = criptos[fila * COLS + col];
      String precio = formatoPrecio(c.precio);
      for (int i = 0; i < margen; i++)
        r += ' ';
      r += c.simbolo;
      for (int i = strlen(c.simbolo); i < wSim; i++) // símbolo pegado a la izquierda
        r += ' ';
      r += ' ';
      for (int i = precio.length(); i < wPre; i++) // precio pegado a la derecha
        r += ' ';
      r += precio;
      for (int i = 0; i < margen; i++)
        r += ' ';
    }
    r += '\n';
  }
  if (ultimoOkCriptos == 0)
    r += "AVISO - Binance No Ha Contestado Desde Que Arrancó El Minero, Aún No Hay Precios\n";
  else if ((millis() - ultimoOkCriptos) / 60000UL >= 10)
    r += "AVISO - Binance No Contesta Desde Hace " + String((millis() - ultimoOkCriptos) / 60000UL) + " Min, Los Precios De Arriba Son De Entonces\n";
  return r;
}

const int BARRA_CX[3] = {54, 159, 266}; // centro horizontal de cada recuadro
const int BARRA_MEDIO_ANCHO = 44;       // mitad del ancho útil dentro de cada recuadro
const int BARRA_SPR_Y = 186;            // y real donde empieza el sprite de texto
const int BARRA_SPR_H = 42;             // alto del sprite
const int BARRA_FILA_Y[2] = {190, 204}; // y real de cada moneda (arriba, abajo)
const int BARRA_PIE_Y = 220;            // y real de la línea de abajo (webs / By M8AX)
const int BARRA_PRECIO_TAM = 8;         // tamaño de la letra digital del precio
const bool BARRA_MOSTRAR_USDT = true;   // false = quita el "USDT" de cada línea
const int BARRA_ETIQ_DY = 3;            // bajada extra de símbolo y USDT para alinearlos con el precio
// Textos de abajo de cada recuadro (máx. 14 letras para que quepan).
const uint16_t BARRA_COLOR_SUBE = 0x07E0; // verde brillante
const uint16_t BARRA_COLOR_BAJA = 0xFC00; // naranja brillante
const char *BARRA_PIE[3] = {"opensea.io/m8ax", " youtube.com/m8ax", "\\\\\\ By M8AX ///"};
const unsigned long BARRA_REFRESCO_MS = 60000; // cada cuántos ms se repinta la barra y se piden precios (60000 = 60 s)

/***********************************************************************************************
 *
 *   Carita de ánimo: ojo guiñando ( guiña y abre ) o cara sacando la lengua ( sale y entra ).
 *   Cara grande centrada en la pantalla de 320 x 240. Dura 3 segundos en total.
 *
 ***********************************************************************************************/
static void animoOjo(int x, int y)
{
  tft.fillCircle(x, y, 20, TFT_WHITE);
  tft.drawCircle(x, y, 20, TFT_BLACK);
  tft.fillCircle(x + 4, y + 3, 9, TFT_BLACK);
  tft.fillCircle(x + 7, y - 1, 3, TFT_WHITE); // brillo
}

static void animoArco(int cx, int cy, int r, int a0, int a1, int grosor, uint16_t color)
{
  for (int a = a0; a <= a1; a += 3)
  {
    float rad = a * 0.0174533f;
    tft.fillCircle(cx + (int)lroundf(r * cosf(rad)), cy + (int)lroundf(r * sinf(rad)), grosor, color);
  }
}

static void animoSonrisa()
{
  animoArco(160, 100, 62, 25, 155, 4, TFT_BLACK);
}

static void animoOjoDcho(bool abierto)
{
  tft.fillCircle(198, 84, 24, TFT_YELLOW); // borra lo que hubiera
  if (abierto)
    animoOjo(198, 84);
  else
    animoArco(198, 96, 18, 200, 340, 4, TFT_BLACK); // ojo cerrado
}

static void animoLengua(bool fuera)
{
  tft.fillRect(134, 148, 52, 48, TFT_YELLOW); // borra lengua y sonrisa de esa zona
  animoSonrisa();
  if (fuera)
  {
    tft.fillRoundRect(140, 152, 40, 42, 16, TFT_RED);
    tft.drawFastVLine(160, 160, 24, TFT_MAROON);
  }
}

static void animoCarita(bool guino)
{
  tft.fillScreen(TFT_BLACK);
  tft.fillCircle(160, 106, 102, TFT_ORANGE);
  tft.fillCircle(160, 106, 98, TFT_YELLOW);
  animoOjo(122, 84);
  animoOjo(198, 84);
  animoSonrisa();
  tft.setTextDatum(TC_DATUM);
  tft.setTextPadding(0);
  tft.setFreeFont(NULL);
  tft.setTextFont(1);
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE);
  tft.drawString("... By M8AX ...", 160, 214);
  if (guino)
  {
    delay(600);
    animoOjoDcho(false);
    delay(600);
    animoOjoDcho(true);
    delay(500);
    animoOjoDcho(false);
    delay(600);
    animoOjoDcho(true);
    delay(700);
  }
  else
  {
    delay(500);
    animoLengua(true);
    delay(800);
    animoLengua(false);
    delay(400);
    animoLengua(true);
    delay(800);
    animoLengua(false);
    delay(500);
  }
}

/***********************************************************************************************
 *
 *   Mensaje de ánimo: 6 variantes al azar ( 4 textos centrados + ojo guiñando + lengua ).
 *   Siempre dura 3 segundos. El color del texto se elige al azar cada vez.
 *
 ***********************************************************************************************/
static void mensajeAnimo(uint32_t n)
{
  uint8_t v = n % 6;
  if (v >= 4)
  {
    animoCarita(v == 4);
    tft.setTextDatum(TL_DATUM);
    tft.setTextSize(1);
    return;
  }
  const char *lineas[6];
  int nl;
  if (v == 0)
  {
    lineas[0] = "La Esperanza Es";
    lineas[1] = "Lo Ultimo Que";
    lineas[2] = "Se Pierde...";
    lineas[3] = "Lo Conseguiremos!";
    lineas[4] = "";
    lineas[5] = "VAMOS !";
    nl = 6;
  }
  else if (v == 1)
  {
    lineas[0] = "Hope Is The Last";
    lineas[1] = "Thing To Lose, We";
    lineas[2] = "Will Achieve It!";
    lineas[3] = "";
    lineas[4] = "COME ON !";
    nl = 5;
  }
  else if (v == 2)
  {
    lineas[0] = "Paciencia, Que Un";
    lineas[1] = "Dia Seremos Ricos";
    lineas[2] = "Y Lo Celebraremos";
    lineas[3] = "";
    lineas[4] = "ANIMO !";
    nl = 5;
  }
  else
  {
    lineas[0] = "Someday We Will";
    lineas[1] = "Be Rich, Keep";
    lineas[2] = "Mining, Friend!";
    lineas[3] = "";
    lineas[4] = "NEVER GIVE UP !";
    nl = 5;
  }
  int y = (240 - (nl * 24 + 8 + 16)) / 2;
  tft.fillScreen(TFT_BLACK);
  tft.setTextDatum(TC_DATUM);
  tft.setTextPadding(0);
  tft.setFreeFont(NULL);
  tft.setTextFont(1);
  tft.setTextSize(3);
  tft.setTextColor(colors[esp_random() % (sizeof(colors) / sizeof(colors[0]))]);
  for (int i = 0; i < nl; i++, y += 24)
    tft.drawString(lineas[i], 160, y);
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE);
  tft.drawString("... By M8AX ...", 160, y + 8);
  delay(3000);
  tft.setTextDatum(TL_DATUM);
  tft.setTextSize(1);
}

static bool mensajeBloque(bool ingles)
{
  const char *lineas[3];
  if (ingles)
  {
    lineas[0] = "YOU MINED A";
    lineas[1] = "BLOCK CHAMPION";
    lineas[2] = "CONGRATULATIONS!";
  }
  else
  {
    lineas[0] = "HAS MINADO UN";
    lineas[1] = "BLOQUE CAMPEON";
    lineas[2] = "ENHORABUENA!";
  }
  int y = (240 - (3 * 24 + 40 + 32)) / 2;
  tft.fillScreen(TFT_BLACK);
  tft.setTextDatum(TC_DATUM);
  tft.setTextPadding(0);
  tft.setFreeFont(NULL);
  tft.setTextFont(1);
  tft.setTextSize(3);
  tft.setTextColor(colors[esp_random() % (sizeof(colors) / sizeof(colors[0]))]);
  for (int i = 0; i < 3; i++, y += 24)
    tft.drawString(lineas[i], 160, y);
  y += 40; // doble espacio
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE);
  tft.drawString("... By M8AX ...", 160, y);
  tft.drawString("youtube.com/m8ax", 160, y + 20);

  bool tocado = false;
  unsigned long t0 = millis();
  int16_t tx, ty;
  while (millis() - t0 < BLOQUE_AVISO_VISIBLE_MS)
  {
    if (toqueXY(tx, ty))
    {
      tocado = true;
      break;
    }
    delay(30);
  }
  tft.setTextDatum(TL_DATUM);
  tft.setTextSize(1);
  return tocado;
}

/***********************************************************************************************
 *
 *   Pinta un recuadro de la barra de abajo ( 2 monedas + texto del pie ). Vale para un sprite
 *   o para la pantalla directa: si no hay RAM para el sprite, el texto sale igual sin él.
 *   ox, oy = esquina del recuadro donde empieza a dibujar ( 0, 0 dentro del sprite ).
 *
 ***********************************************************************************************/
template <typename T>
static void barraRecuadro(T &d, int ox, int oy, int col)
{
  const int MED = 54;
  render.setDrawer(d);
  render.setLineSpaceRatio(1);
  render.setAlignment(Align::TopLeft);
  render.setFontSize(BARRA_PRECIO_TAM);
  d.setFreeFont(NULL);
  d.setTextFont(1);
  d.setTextSize(1);

  for (int fila = 0; fila < 2; fila++)
  {
    Cripto &c = criptos[paginaCriptos * 6 + col * 2 + fila];
    int y = oy + BARRA_FILA_Y[fila] - BARRA_SPR_Y;
    int xl = ox + MED - BARRA_MEDIO_ANCHO;
    int xr = ox + MED + BARRA_MEDIO_ANCHO;
    int ini = xl + strlen(c.simbolo) * 6 + 1;
    int fin = xr;
    d.setTextDatum(TL_DATUM);
    d.setTextColor(TFT_YELLOW);
    d.drawString(c.simbolo, xl, y + BARRA_ETIQ_DY);
    if (BARRA_MOSTRAR_USDT)
    {
      d.setTextDatum(TR_DATUM);
      d.setTextColor(TFT_CYAN);
      d.drawString("USDT", xr, y + BARRA_ETIQ_DY);
      fin = xr - 24 - 1;
    }
    uint16_t colorPrecio = TFT_WHITE;
    if (c.tendencia > 0)
      colorPrecio = BARRA_COLOR_SUBE;
    else if (c.tendencia < 0)
      colorPrecio = BARRA_COLOR_BAJA;
    render.cdrawString(formatoPrecio(c.precio).c_str(), (ini + fin) / 2, y, colorPrecio);
  }
  d.setTextDatum(TC_DATUM);
  colorM8 = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  d.setTextColor(colors[colorM8]);
  d.drawString(BARRA_PIE[col], ox + MED, oy + BARRA_PIE_Y - BARRA_SPR_Y);
  d.setTextDatum(TL_DATUM);
}

/***********************************************************************************************
 *
 *   Barra de precios de abajo ( función completa ). Un sprite pequeño por recuadro; si no hay
 *   RAM para alguno, ese recuadro se escribe directo en la pantalla.
 *
 ***********************************************************************************************/
void printPoolData()
{
  if ((hasChangedScreen) || (mPoolUpdate == 0) || (millis() - mPoolUpdate > BARRA_REFRESCO_MS))
  {
    if (Settings.PoolAddress != "tn.vkbit.com")
    {
      background.deleteSprite(); // libera un sprite viejo que haya dejado otra pantalla
      // Precios: como mucho 1 petición cada 60 segundos. Cada petición cambia de página.
      if (ultimaActualizacionCriptos == 0 || millis() - ultimaActualizacionCriptos > BARRA_REFRESCO_MS)
      {
        bool ok = obtenerPreciosCriptos();
        Serial.printf("M8AX - Stack Libre Minimo: %u Bytes\n", (unsigned)uxTaskGetStackHighWaterMark(NULL));
        if (ok && ultimaActualizacionCriptos != 0) // solo cambia de página si llegaron precios nuevos
          paginaCriptos = (paginaCriptos + 1) % (NUM_CRIPTOS / 6);
        ultimaActualizacionCriptos = millis();
      }
      mPoolUpdate = millis();

      // 1) Fondo completo de la barra (borra todo lo anterior)
      if (bottomScreenBlue)
        tft.pushImage(0, 170, 320, 70, bottonPoolScreen);
      else
        tft.pushImage(0, 170, 320, 70, bottonPoolScreen_g);

      // 2) Un sprite pequeño por recuadro; si no hay RAM, ese recuadro va directo a la pantalla
      for (int col = 0; col < 3; col++)
      {
        const int x0 = BARRA_CX[col] - 54;
        background.setColorDepth(16);
        background.createSprite(108, BARRA_SPR_H);
        if (background.created())
        {
          background.setSwapBytes(true);
          if (bottomScreenBlue)
            background.pushImage(-x0, -(BARRA_SPR_Y - 170), 320, 70, bottonPoolScreen);
          else
            background.pushImage(-x0, -(BARRA_SPR_Y - 170), 320, 70, bottonPoolScreen_g);
          barraRecuadro(background, 0, 0, col);
          background.pushSprite(x0, BARRA_SPR_Y);
          background.deleteSprite();
        }
        else
        {
          Serial.println("M8AX - Barra: Sin RAM Para El Sprite, Dibujo Directo");
          barraRecuadro(tft, x0, BARRA_SPR_Y, col);
        }
      }
      render.setDrawer(background);
    }
    else
    {
      pData.bestDifficulty = "TESTNET";
      pData.workersHash = "TESTNET";
      pData.workersCount = 1;
      tft.fillRect(0, 170, 320, 70, TFT_DARKGREEN);
      background.deleteSprite(); // libera un sprite viejo que haya dejado otra pantalla
      background.createSprite(320, 40);
      background.fillSprite(TFT_DARKGREEN);
      if (!background.created())
      {
        Serial.println("M8AX - ###### POOL SPRITE ERROR ######");
        printheap();
      }
      background.setFreeFont(FF24);
      background.setTextDatum(TL_DATUM);
      background.setTextSize(1);
      background.setTextColor(TFT_WHITE, TFT_DARKGREEN);
      background.drawString("TESTNET", 50, 0, GFXFF);
      background.pushSprite(0, 185);
      mPoolUpdate = millis();
      Serial.println("Testnet");
      background.deleteSprite();
    }
  }
}

void esp32_2432S028R_MinerScreen(unsigned long mElapsed)
{
  mineria = getMiningData(mElapsed);
  detectar = 0;
  unsigned long epochTime = timeClient.getEpochTime(); // Obtener segundos desde 1970
  time_t epoch = (time_t)epochTime;                    // Convertir a time_t
  struct tm *timeinfo = localtime(&epoch);             // Convertir a una estructura de tiempo local
  int dia = timeinfo->tm_mday;                         // Día del mes (1 a 31)
  int mes = timeinfo->tm_mon + 1;                      // Mes (1 a 12)
  int anio = timeinfo->tm_year + 1900;                 // Año (por defecto es desde 1900)
  int horita = timeinfo->tm_hour;                      // Hora
  int minutitos = timeinfo->tm_min;                    // Minutos
  int segundos = timeinfo->tm_sec;                     // Segundos
  int wday = timeinfo->tm_wday;                        // Día semana 0 - 6
  int diaSemana = wday == 0 ? 7 : wday;                // Si es domingo (0), lo cambiamos a 7
  int diaDelAno = timeinfo->tm_yday;                   // Día del año 0 - 366
  int segundosDelDia = horita * 3600 + minutitos * 60 + segundos;
  uint32_t segundosDelAno = diaDelAno * 86400 + segundosDelDia, segundosTotalesDelAno = esBisiesto(anio) ? 366 * 86400 : 365 * 86400;
  tft.setTextFont(1);
  tft.setTextSize(1);
  printPoolData();
  if (hasChangedScreen)
    tft.pushImage(0, 0, initWidth, initHeight, MinerScreen);
  hasChangedScreen = false;
  int wdtOffset = 190;
  // Recreate sprite to the right side of the screen
  createBackgroundSprite(WIDTH - 5, HEIGHT - 7);
  // Print background screen
  background.pushImage(-190, 0, MinerWidth, MinerHeight, MinerScreen);
  // Total hashes
  render.setFontSize(14);
  render.rdrawString(mineria.totalMHashes.c_str(), 273 - wdtOffset, 143, TFT_BLACK);
  // Block templates
  render.setFontSize(16);
  render.setAlignment(Align::TopLeft);
  render.drawString(mineria.templates.c_str(), 188 - wdtOffset, 21, 0xDEDB);
  // Best diff
  render.drawString(mineria.bestDiff.c_str(), 188 - wdtOffset, 48, 0xDEDB);
  // 32Bit shares
  render.setFontSize(18);
  render.drawString(mineria.completedShares.c_str(), 188 - wdtOffset, 74, 0xDEDB);
  // Hores
  render.setFontSize(12);
  render.rdrawString(mineria.timeMining.c_str(), 310 - wdtOffset, 106, 0xDEDB);
  // Valid Blocks
  render.setFontSize(24);
  render.setAlignment(Align::TopCenter);
  render.drawString(mineria.valids.c_str(), 287 - wdtOffset, 56, (mineria.valids.toInt() == 0) ? 0xF00F : 0x07E0);
  // Print Temp
  render.setFontSize(10);
  render.rdrawString(mineria.temp.c_str(), 239 - wdtOffset, 1, TFT_BLACK);
  render.setFontSize(4);
  render.rdrawString(String(0).c_str(), 244 - wdtOffset, 3, TFT_BLACK);
  // Print Hour
  render.setFontSize(10);
  render.rdrawString(mineria.currentTime.c_str(), 286 - wdtOffset, 1, TFT_BLACK);
  // Push prepared background to screen
  background.pushSprite(190, 0);
  // Delete sprite to free the memory heap
  background.deleteSprite();
  // printheap();
  // Serial.println("=========== Mining Display ==============") ;
  // Create background sprite to print data at once
  createBackgroundSprite(WIDTH - 7, HEIGHT - 100); // initHeight); //Background Sprite
  // Print background screen
  background.pushImage(0, -90, MinerWidth, MinerHeight, MinerScreen);
  // Hashrate
  render.setFontSize(30);
  render.setCursor(24, 121);
  render.setFontColor(TFT_BLACK);
  render.rdrawString(mineria.currentHashRate.c_str(), 121, 118 - 90, TFT_BLACK);
  // Push prepared background to screen
  background.pushSprite(0, 90);
  // Delete sprite to free the memory heap
  background.deleteSprite();
  tft.setTextColor(colors[colorI]);
  tft.setCursor(33, 107);
  tft.print("             ");
  String pool = Settings.PoolAddress;
  const char *prefijos[] = {"stratum+tcp://", "pool.", "web.", "eu.", "stratum."};
  for (int i = 0; i < 5; i++)
    if (pool.startsWith(prefijos[i]))
      pool = pool.substring(strlen(prefijos[i]));
  if (pool.length() > 15)
    pool = pool.substring(0, 15);
  String extra = "";
  int ciclo = segundos % 20;
  if (ciclo < 5 && pool.length() > 0)
    extra = pool; // 5 s la pool
  else if (ciclo >= 10 && ciclo < 15)
    extra = "Heap " + String(ESP.getFreeHeap() / 1024) + "/" + String(ESP.getMaxAllocHeap() / 1024) + "KB"; // 5 s la RAM

  if (extra.length() > 0)
  {
    tft.setCursor(69 - (int)extra.length() * 3, 107); // centrado, mismo hueco
    tft.print(extra);
  }
  else
  {
    tft.setCursor(33, 107);
    tft.print(segundos % 2 == 0 ? "- EHD-MDDD -" : "- MvIiIaX -");
  }
  int X_INICIO = 155;
  int X_FINAL = 310;
  int Y_POS = 104;
  int longitud_total = X_FINAL - X_INICIO;
  int longitud_pintada = (longitud_total * segundosDelDia) / 86400;
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.fillRect(X_INICIO, Y_POS - 1, longitud_total + 1, 3, TFT_DARKGREY);
  tft.drawLine(X_INICIO, Y_POS, X_INICIO + longitud_pintada, Y_POS, colors[colorI]);
  X_INICIO = 155;
  X_FINAL = 310;
  Y_POS = 124;
  longitud_total = X_FINAL - X_INICIO;
  longitud_pintada = (longitud_total * segundos) / 59;
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.fillRect(X_INICIO, Y_POS - 1, longitud_total + 1, 3, TFT_DARKGREY);
  tft.drawLine(X_INICIO, Y_POS, X_INICIO + longitud_pintada, Y_POS, colors[colorI]);
  X_INICIO = 3;
  X_FINAL = 167;
  Y_POS = 161;
  longitud_total = X_FINAL - X_INICIO;
  longitud_pintada = (longitud_total * diaSemana) / 7;
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.fillRect(X_INICIO, Y_POS - 1, longitud_total + 1, 3, TFT_DARKGREY);
  tft.drawLine(X_INICIO, Y_POS, X_INICIO + longitud_pintada, Y_POS, colors[colorI]);
  X_INICIO = 173;
  X_FINAL = 316;
  Y_POS = 137;
  longitud_total = X_FINAL - X_INICIO;
  longitud_pintada = (longitud_total * mineria.currentHashRate.toInt()) / 400;
  if (longitud_pintada > longitud_total)
    longitud_pintada = longitud_total;
  if (longitud_pintada < 0)
    longitud_pintada = 0;
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.fillRect(X_INICIO, Y_POS - 1, longitud_total + 1, 3, TFT_DARKGREY);
  tft.drawLine(X_INICIO, Y_POS, X_INICIO + longitud_pintada, Y_POS, colors[colorI]);
  X_INICIO = 110;
  X_FINAL = 212;
  Y_POS = 11;
  longitud_total = X_FINAL - X_INICIO;
  longitud_pintada = (longitud_total * minutitos) / 59;
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.fillRect(X_INICIO, Y_POS - 1, longitud_total + 1, 3, TFT_DARKGREY);
  tft.drawLine(X_INICIO, Y_POS, X_INICIO + longitud_pintada, Y_POS, colors[colorI]);
  X_INICIO = 184;
  X_FINAL = 316;
  Y_POS = 165;
  longitud_total = X_FINAL - X_INICIO;
  longitud_pintada = (longitud_total * (segundosDelAno / 60)) / (segundosTotalesDelAno / 60);
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.fillRect(X_INICIO, Y_POS - 1, longitud_total + 1, 3, TFT_DARKGREY);
  tft.drawLine(X_INICIO, Y_POS, X_INICIO + longitud_pintada, Y_POS, colors[colorI]);
  cuentita++;
  if (cuentita == 15)
  {
    ajustarZonaHoraria();
    obtenerLocYTemp();
    obtenerVariacionesBTC(variacion1dia, variacion1semana);
  }
  Serial.printf("M8AX - %s >>> Completados %s Share(s), %s Khashes, Prom. Hashrate %s KH/s %s°\n",
                mineria.currentTime.c_str(), mineria.completedShares.c_str(), mineria.totalKHashes.c_str(), mineria.currentHashRate.c_str(), mineria.temp.c_str());
  manejandoLeds(mineria.currentHashRate.toFloat());
#ifdef DEBUG_MEMORY
  // Print heap
  printheap();
#endif
}

void esp32_2432S028R_ClockScreen(unsigned long mElapsed)
{
  mineria = getMiningData(mElapsed);
  unsigned long epochTime = timeClient.getEpochTime(); // Obtener segundos desde 1970
  time_t epoch = (time_t)epochTime;                    // Convertir a time_t
  struct tm *timeinfo = localtime(&epoch);             // Convertir a una estructura de tiempo local
  int dia = timeinfo->tm_mday;                         // Día del mes (1 a 31)
  int mes = timeinfo->tm_mon + 1;                      // Mes (1 a 12)
  int anio = timeinfo->tm_year + 1900;                 // Año (por defecto es desde 1900)
  int diaDelAno = timeinfo->tm_yday;                   // 0-365
  int horita = timeinfo->tm_hour;                      // Hora
  int minutitos = timeinfo->tm_min;                    // Minutos
  int segundos = timeinfo->tm_sec;                     // Segundos
  int segundosDelDia = horita * 3600 + minutitos * 60 + segundos;
  uint32_t segundosDelAno = diaDelAno * 86400 + segundosDelDia, segundosTotalesDelAno = esBisiesto(anio) ? 366 * 86400 : 365 * 86400;
  // Formatear la hora en "00:00:00"
  char horaFormateada[9];
  sprintf(horaFormateada, "%02d:%02d:%02d", horita, minutitos, segundos);
  // Formatear la fecha en "dia/mes/año"
  char fechaFormateada[11];
  sprintf(fechaFormateada, "%02d/%02d/%04d", dia, mes, anio);
  if (hasChangedScreen)
    tft.pushImage(0, 0, minerClockWidth, minerClockHeight, minerClockScreen);
  detectar = 0;
  printPoolData();
  if (detectar2 == 0)
  {
    prebitco = preciob().c_str();
    alturabloque = alturab().c_str();
  }
  detectar2++;
  if (detectar2 % 60 == 0)
  {
    prebitco = preciob().c_str();
    alturabloque = alturab().c_str();
  }
  hasChangedScreen = false;
  // Create background sprite to print data at once
  createBackgroundSprite(270, 36);
  // Print background screen
  background.pushImage(0, -130, minerClockWidth, minerClockHeight, minerClockScreen);
  // Hashrate
  render.setFontSize(22);
  render.setFontColor(TFT_BLACK);
  render.rdrawString(mineria.currentHashRate.c_str(), 95, 0, TFT_BLACK);
  // Print BlockHeight
  render.setFontSize(18);
  render.rdrawString(alturabloque.c_str(), 254, 9, TFT_BLACK);
  // Push prepared background to screen
  background.pushSprite(0, 130);
  // Delete sprite to free the memory heap
  background.deleteSprite();
  createBackgroundSprite(169, 105);
  // Print background screen
  background.pushImage(-130, -3, minerClockWidth, minerClockHeight, minerClockScreen);
  // Print BTC Price
  background.setFreeFont(FSSB9);
  background.setTextSize(1);
  background.setTextDatum(TL_DATUM);
  background.setTextColor(TFT_BLACK);
  background.drawString(prebitco, 202 - 130, 0, GFXFF);
  // Print Hour
  background.setFreeFont(FF23);
  background.setTextSize(1);
  background.setTextColor(TFT_WHITE);
  background.drawString(horaFormateada, 15, 39, GFXFF);
  background.drawString(fechaFormateada, -1, 77, GFXFF);
  // Push prepared background to screen
  background.pushSprite(130, 3);
  // Delete sprite to free the memory heap
  background.deleteSprite();
  Serial.printf("M8AX - %s >>> Completados %s Share(s), %s Khashes, Prom. Hashrate %s KH/s %s°\n",
                mineria.currentTime.c_str(), mineria.completedShares.c_str(), mineria.totalKHashes.c_str(), mineria.currentHashRate.c_str(), mineria.temp.c_str());
  cuentita++;
  if (cuentita == 15)
  {
    ajustarZonaHoraria();
    obtenerLocYTemp();
    obtenerVariacionesBTC(variacion1dia, variacion1semana);
  }
  int X_INICIO = 124;
  int X_FINAL = 312;
  int Y_POS = 117;
  int longitud_total = X_FINAL - X_INICIO;
  int longitud_pintada = (longitud_total * (segundosDelAno / 60)) / (segundosTotalesDelAno / 60);
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.fillRect(X_INICIO, Y_POS - 1, longitud_total + 1, 4, TFT_DARKGREY);
  tft.fillRect(X_INICIO, Y_POS - 1, longitud_pintada, 4, colors[colorI]);
  X_INICIO = 124;
  X_FINAL = 312;
  Y_POS = 32;
  longitud_total = X_FINAL - X_INICIO;
  longitud_pintada = (longitud_total * segundos) / 59;
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.fillRect(X_INICIO, Y_POS - 1, longitud_total + 1, 4, TFT_DARKGREY);
  tft.fillRect(X_INICIO, Y_POS - 1, longitud_pintada, 4, colors[colorI]);
  manejandoLeds(mineria.currentHashRate.toFloat());
#ifdef DEBUG_MEMORY
  // Print heap
  printheap();
#endif
}

void esp32_2432S028R_m8axScreen2(unsigned long mElapsed)
{
  mineria = getMiningData(mElapsed);
  unsigned long epochTime = timeClient.getEpochTime(); // Obtener segundos desde 1970
  time_t epoch = (time_t)epochTime;                    // Convertir a time_t
  struct tm *timeinfo = localtime(&epoch);             // Convertir a estructura de tiempo local
  int dia = timeinfo->tm_mday;                         // Día del mes (1 a 31)
  int mes = timeinfo->tm_mon + 1;                      // Mes (0 a 11) -> +1
  int anio = timeinfo->tm_year + 1900;                 // Año desde 1900
  int horita = timeinfo->tm_hour;                      // Hora
  int minutitos = timeinfo->tm_min;                    // Minutos
  // Formatear fecha dd/mm/aaaa
  char fecha[11]; // "dd/mm/aaaa" + null terminator
  sprintf(fecha, "%02d/%02d/%04d", dia, mes, anio);
  // Formatear hora hh:mm
  char hora[6]; // "hh:mm" + null terminator
  sprintf(hora, "%02d:%02d", horita, minutitos);
  // Obtener dígitos individuales de hora y minutos
  int num1 = horita / 10;
  int num2 = horita % 10;
  int num3 = minutitos / 10;
  int num4 = minutitos % 10;
  if (hasChangedScreen)
    tft.pushImage(0, 0, initWidth, initHeight, M8AXQuote1);
  printPoolData();
  refresca++;
  if (refresca > 4)
  {
    tft.pushImage(0, 0, initWidth, initHeight, M8AXQuote1);
    mostrarCalendario(dia, mes, anio, num1, num2, num3, num4);
    refresca = 0;
  }
  hasChangedScreen = false;
  int wdtOffset = 190;
  Serial.printf("M8AX - %s >>> Completados %s Share(s), %s Khashes, Prom. Hashrate %s KH/s %s°\n",
                mineria.currentTime.c_str(), mineria.completedShares.c_str(), mineria.totalKHashes.c_str(), mineria.currentHashRate.c_str(), mineria.temp.c_str());
  cuentita++;
  if (cuentita == 15)
  {
    ajustarZonaHoraria();
    obtenerLocYTemp();
    obtenerVariacionesBTC(variacion1dia, variacion1semana);
  }
  manejandoLeds(mineria.currentHashRate.toFloat());
#ifdef DEBUG_MEMORY
  // Print heap
  printheap();
#endif
}

void RelojDeNumeros(unsigned long mElapsed)
{
  mineria = getMiningData(mElapsed);
  unsigned long epochTime = timeClient.getEpochTime(); // Obtener segundos desde 1970
  time_t epoch = (time_t)epochTime;                    // Convertir a time_t
  struct tm *timeinfo = localtime(&epoch);             // Convertir a estructura de tiempo local
  int dia = timeinfo->tm_mday;                         // Día del mes (1 a 31)
  int mes = timeinfo->tm_mon + 1;                      // Mes (0 a 11) -> +1
  int anio = timeinfo->tm_year + 1900;                 // Año desde 1900
  int horas = timeinfo->tm_hour;                       // Hora
  int minutos = timeinfo->tm_min;                      // Minutos
  int segundos = timeinfo->tm_sec;                     // Segundos
  detectar2 = 0;
  // Formatear fecha dd/mm/aaaa
  char fecha[11]; // "dd/mm/aaaa" + null terminator
  sprintf(fecha, "%02d/%02d/%04d", dia, mes, anio);
  // Formatear hora hh:mm
  char hora[6]; // "hh:mm" + null terminator
  sprintf(hora, "%02d:%02d", horas, minutos);
  // Obtener dígitos individuales de hora y minutos
  int millonario = atoi(mineria.valids.c_str());
  int horis1 = horas / 10;       // Primer dígito de las horas
  int horis2 = horas % 10;       // Segundo dígito de las horas
  int minutis1 = minutos / 10;   // Primer dígito de los minutos
  int minutis2 = minutos % 10;   // Segundo dígito de los minutos
  int segundis1 = segundos / 10; // Primer dígito de los segundos
  int segundis2 = segundos % 10; // Segundo dígito de los segundos
  std::string quediase = obtenerDiaSemana(std::string(fecha));
  int horis = horas;
  int mins = minutos;
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
  String TempCPU = String(mineria.temp.c_str());
  if (hasChangedScreen)
    tft.pushImage(0, 0, initWidth, initHeight, ImagenM8AX);
  printPoolData();
  refresca++;
  if (refresca > 0)
  {
    tft.pushImage(0, 0, initWidth, initHeight, ImagenM8AX);
    refresca = 0;
  }
  hasChangedScreen = false;
  int wdtOffset = 190;
  Serial.printf("M8AX - %s >>> Completados %s Share(s), %s Khashes, Prom. Hashrate %s KH/s %s°\n",
                mineria.currentTime.c_str(), mineria.completedShares.c_str(), mineria.totalKHashes.c_str(), mineria.currentHashRate.c_str(), mineria.temp.c_str());
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
  tft.print(String(mineria.currentHashRate.c_str()) + " KH/s");
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
  tft.print("400 KH/s");
  int grosor = 5;
  int X_INICIO = 5;                                                                // Inicio de la barra ( izquierda )
  int X_FINAL = 242;                                                               // Final de la barra ( derecha )
  int Y_POS = 43;                                                                  // Altura de la barra
  int longitud_total = X_FINAL - X_INICIO;                                         // Longitud total de la barra (237 píxeles)
  int longitud_pintada = (longitud_total * mineria.currentHashRate.toInt()) / 400; // Parte de la barra según el hashrate ( 400 KH/s = llena )
  if (longitud_pintada > longitud_total)
    longitud_pintada = longitud_total;
  if (longitud_pintada < 0)
    longitud_pintada = 0;
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
  X_INICIO = 4;                                                 // Inicio de la barra ( izquierda )
  X_FINAL = 242;                                                // Final de la barra ( derecha )
  Y_POS = 117;                                                  // Altura de la barra
  longitud_total = X_FINAL - X_INICIO;                          // Longitud total de la barra (238 píxeles)
  longitud_pintada = (longitud_total * segundosDelDia) / 86400; // Porcentaje de la barra según los segundos
  tft.fillRect(X_INICIO, Y_POS - (grosor / 2), longitud_total + 1, grosor, TFT_BLACK);
  tft.fillRect(X_INICIO, Y_POS - (grosor / 2), longitud_pintada, grosor, colors[colorI]);
  cuentita++;
  if (cuentita == 15)
  {
    ajustarZonaHoraria();
    obtenerLocYTemp();
    obtenerVariacionesBTC(variacion1dia, variacion1semana);
  }
  manejandoLeds(mineria.currentHashRate.toFloat());
#ifdef DEBUG_MEMORY
  // Print heap
  printheap();
#endif
}

void esp32_2432S028R_m8axScreen1(unsigned long mElapsed)
{
  mineria = getMiningData(mElapsed);
  unsigned long epochTime = timeClient.getEpochTime(); // Obtener segundos desde 1970
  time_t epoch = (time_t)epochTime;                    // Convertir a time_t
  struct tm *timeinfo = localtime(&epoch);             // Convertir a estructura de tiempo local
  int dia = timeinfo->tm_mday;
  int mes = timeinfo->tm_mon + 1;
  int anio = timeinfo->tm_year + 1900;
  int hora = timeinfo->tm_hour;
  int minuto = timeinfo->tm_min;
  int segundo = timeinfo->tm_sec;
  int millonario = atoi(mineria.valids.c_str());
  int segundosDelDia = (hora * 3600) + (minuto * 60) + segundo;
  moonData_t moon;
  time_t cadenaDeTiempo = mktime(timeinfo);
  moon = mymoonPhase.getPhase(cadenaDeTiempo);
  double porcentajeIluminado = moon.percentLit * 100;
  char porcentajeTexto[10];
  snprintf(porcentajeTexto, sizeof(porcentajeTexto), "%.2f%%", porcentajeIluminado);
  String textoFinal = String(porcentajeTexto);
  int horis = hora;
  int minutis = minuto;
  int secondis = segundo;
  if (hasChangedScreen)
    tft.fillRect(0, 0, 320, 170, TFT_BLACK);
  printPoolData();
  refresca++;
  if (refresca > 0)
  {
    tft.fillRect(0, 0, 320, 170, TFT_BLACK);
    refresca = 0;
  }
  hasChangedScreen = false;
  int wdtOffset = 190;
  Serial.printf("M8AX - %s >>> Completados %s Share(s), %s Khashes, Prom. Hashrate %s KH/s %s°\n",
                mineria.currentTime.c_str(), mineria.completedShares.c_str(), mineria.totalKHashes.c_str(), mineria.currentHashRate.c_str(), mineria.temp.c_str());
  colorIndex = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  relojAnalogicoM8AX(horis, minutis, secondis);
  tft.setCursor(4, 42);
  tft.setTextColor(colors[colorIndex]);
  tft.setTextSize(3);
  tft.print(dia);
  tft.setCursor(4, 74);
  String mesecillo = obtenerNombreMes(mes).substring(0, 3);
  tft.print(mesecillo);
  tft.setCursor(4, 106);
  char fechaFormateada[11];
  sprintf(fechaFormateada, "%02d/%02d/%04d", dia, mes, anio);
  std::string quediase = obtenerDiaSemana(std::string(fechaFormateada));
  tft.print(String(quediase.c_str()));
  quediase.clear();
  quediase.shrink_to_fit();
  tft.setCursor(4, 138);
  tft.print(anio);
  tft.setCursor(230, 150);
  tft.setTextSize(2);
  tft.print(mineria.currentHashRate);
  tft.setCursor(250, 126);
  tft.print("KH/s");
  tft.setCursor(250, 37);
  tft.print("RSSI");
  tft.setCursor(250, 69);
  tft.print(String(WiFi.RSSI()) + "dB");
  tft.setCursor(240, 101);
  tft.print(evaluarRSSI(String(WiFi.RSSI())));
  if (millonario == 0)
  {
    tft.setCursor(4, 5);
    tft.setTextSize(1);
    tft.print("MvIiIaX - M8AX - NR");
  }
  else
  {
    tft.setCursor(4, 5);
    tft.setTextSize(1);
    tft.print("MvIiIaX - M8AX - SR");
  }
  if (Tresultado.first != "" && Tresultado.second != "")
  {
    Serial.println("M8AX - Ciudad: " + Tresultado.first);
    Serial.println("M8AX - Temperatura: " + Tresultado.second);
    tft.setCursor(4, 17);
    tft.print(quitarAcentos(Tresultado.first));
    tft.setCursor(4, 27);
    Tresultado.second.replace("°", "");
    tft.print(Tresultado.second);
    tft.setCursor(38, 27);
    tft.print(textoFinal);
  }
  else
  {
    Serial.println("M8AX - No Se Pudo Obtener La Ciudad O La Temperatura.");
    tft.setCursor(4, 17);
    tft.print("Quien Sabe Donde");
    tft.setCursor(4, 27);
    tft.print("xxC");
    tft.setCursor(38, 27);
    tft.print(textoFinal);
    obtenerLocYTemp();
  }
  tft.setCursor(215, 5);
  tft.print("youtube.com/m8ax");
  tft.setCursor(215, 15);
  tft.print("paypal.me/m8ax/5");
  cuentita++;
  dibujarPorcentajeLunar(61, 57, 16, porcentajeIluminado);
  if (cuentita == 15)
  {
    ajustarZonaHoraria();
    obtenerLocYTemp();
    obtenerVariacionesBTC(variacion1dia, variacion1semana);
  }
  if (cuentita % 1800 == 0)
  {
    obtenerLocYTemp();
  }
  int X_INICIO = 1, X_FINAL = 320, Y_POS = 1, longitud_total = X_FINAL - X_INICIO, longitud_pintada = (longitud_total * segundosDelDia) / 86400;
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.fillRect(X_INICIO, Y_POS - 1, longitud_total + 1, 3, TFT_DARKCYAN);
  tft.drawLine(X_INICIO, Y_POS, X_INICIO + longitud_pintada, Y_POS, colors[colorIndex]);
  manejandoLeds(mineria.currentHashRate.toFloat());
#ifdef DEBUG_MEMORY
  // Print heap
  printheap();
#endif
}

void tDisplay_m8axScreen4(unsigned long mElapsed)
{
  mineria = getMiningData(mElapsed);
  if (hasChangedScreen)
    tft.pushImage(0, 0, initWidth, initHeight, M8AXRelojLunar);
  printPoolData();
  refresca++;
  hasChangedScreen = false;
  int wdtOffset = 190;
  // Print background screen
  if (refresca > 0)
  {
    refresca = 0;
    tft.fillRect(0, 0, 320, 170, TFT_BLACK);
  }
  Serial.printf("M8AX - %s >>> Completados %s Share(s), %s Khashes, Prom. Hashrate %s KH/s %s°\n",
                mineria.currentTime.c_str(), mineria.completedShares.c_str(), mineria.totalKHashes.c_str(), mineria.currentHashRate.c_str(), mineria.temp.c_str());
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  if (colorI % 2 == 0)
  {
    limite = esp_random() % 15000; // Rango: 0 a 14999
  }
  else
  {
    limite = esp_random() % 5000; // Rango: 0 a 4999
  }
  tft.setCursor(30, 60);
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.setTextColor(colors[colorI]);
  tft.setTextSize(7);
  tft.print(String(mineria.currentHashRate.c_str()));
  tft.setCursor(25, 15);
  tft.setTextSize(3);
  tft.print("... GRACIAS ...");
  tft.setCursor(16, 118);
  tft.setTextSize(2);
  tft.print("https://youtube.com/m8ax");
  tft.setCursor(16, 145);
  tft.setTextSize(2);
  tft.print("https://paypal.me/m8ax/5");
  for (int i = 0; i < limite; i++)
  {
    colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
    int ancho = esp_random() % 341; // Rango: 0 a 340
    int alto = esp_random() % 171;  // Rango: 0 a 170
    tft.drawPixel(ancho, alto, colors[colorI]);
  }
  cuentita++;
  if (cuentita == 15)
  {
    ajustarZonaHoraria();
    obtenerLocYTemp();
    obtenerVariacionesBTC(variacion1dia, variacion1semana);
  }
  manejandoLeds(mineria.currentHashRate.toFloat());
#ifdef DEBUG_MEMORY
  // Print heap
  printheap();
#endif
}

void tDisplay_m8axScreen5(unsigned long mElapsed)
{
  mineria = getMiningData(mElapsed);
  unsigned long epochTime = timeClient.getEpochTime(); // Obtener segundos desde 1970
  time_t epoch = (time_t)epochTime;                    // Convertir a time_t
  struct tm *timeinfo = localtime(&epoch);             // Convertir a estructura de tiempo local
  int dia = timeinfo->tm_mday;                         // Día del mes (1 a 31)
  int mes = timeinfo->tm_mon + 1;                      // Mes (0 a 11) -> +1
  int anio = timeinfo->tm_year + 1900;                 // Año desde 1900
  int horita = timeinfo->tm_hour;                      // Hora
  int minutitos = timeinfo->tm_min;                    // Minutos
  // Formatear fecha dd/mm/aaaa
  char fecha[11]; // "dd/mm/aaaa" + null terminator
  sprintf(fecha, "%02d/%02d/%04d", dia, mes, anio);
  // Formatear hora hh:mm
  char hora[6]; // "hh:mm" + null terminator
  sprintf(hora, "%02d:%02d", horita, minutitos);
  solouna = 0;
  Serial.printf("M8AX - %s >>> Completados %s Share(s), %s Khashes, Prom. Hashrate %s KH/s %s°\n",
                mineria.currentTime.c_str(), mineria.completedShares.c_str(), mineria.totalKHashes.c_str(), mineria.currentHashRate.c_str(), mineria.temp.c_str());
  if (hasChangedScreen)
    tft.pushImage(0, 0, initWidth, initHeight, M8AXRelojLunar);
  printPoolData();
  refresca++;
  hasChangedScreen = false;
  int wdtOffset = 190;
  if (solounavez == 0)
  {
    numnotis++;
    refresca = 0;
    tft.fillRect(0, 0, 320, 170, TFT_BLACK);
    solounavez = 1;
    obtenerNoticias();
    tft.setTextColor(TFT_WHITE);
    tft.setCursor(112, 160);
    tft.setTextSize(1);
    tft.print("M8AX-Noticias Num-" + String(numnotis));
    tft.setTextColor(TFT_WHITE);
    tft.drawLine(0, 20, 320, 20, colors[colorI]); // Dibujar línea de (0, y) a (320, y)
    tft.setCursor(1, 6);
    tft.setTextSize(1);
    int millonario = atoi(mineria.valids.c_str());
    if (millonario == 0)
    {
      tft.print(String(hora) + " - " + String(fecha) + " - " + mineria.currentHashRate + " KH/s - " + mineria.temp + " Grados." + " NO RICO");
    }
    else
    {
      tft.print(String(hora) + " - " + String(fecha) + " - " + mineria.currentHashRate + " KH/s - " + mineria.temp + " Grados." + " SI RICO");
    }
  }
  if (refresca > 60)
  {
    refresca = 0;
    tft.fillRect(0, 0, 320, 170, TFT_BLACK);
    numnotis++;
    obtenerNoticias();
    tft.setTextColor(TFT_WHITE);
    tft.setCursor(112, 160);
    tft.setTextSize(1);
    tft.print("M8AX-Noticias Num-" + String(numnotis));
    tft.setTextColor(TFT_WHITE);
    tft.drawLine(0, 20, 320, 20, colors[colorI]); // Dibujar línea de (0, y) a (320, y)
    tft.setCursor(1, 6);
    tft.setTextSize(1);
    int millonario = atoi(mineria.valids.c_str());
    if (millonario == 0)
    {
      tft.print(String(hora) + " - " + String(fecha) + " - " + mineria.currentHashRate + " KH/s - " + mineria.temp + " Grados." + " NO RICO");
    }
    else
    {
      tft.print(String(hora) + " - " + String(fecha) + " - " + mineria.currentHashRate + " KH/s - " + mineria.temp + " Grados." + " SI RICO");
    }
  }
  cuentita++;
  if (cuentita == 15)
  {
    ajustarZonaHoraria();
    obtenerLocYTemp();
    obtenerVariacionesBTC(variacion1dia, variacion1semana);
  }
  manejandoLeds(mineria.currentHashRate.toFloat());
#ifdef DEBUG_MEMORY
  // Print heap
  printheap();
#endif
}

void tDisplay_m8axScreen6(unsigned long mElapsed)
{
  mineria = getMiningData(mElapsed);
  unsigned long epochTime = timeClient.getEpochTime(); // Obtener segundos desde 1970
  time_t epoch = (time_t)epochTime;                    // Convertir a time_t
  struct tm *timeinfo = localtime(&epoch);             // Convertir a estructura de tiempo local
  int dia = timeinfo->tm_mday;                         // Día del mes (1 a 31)
  int mes = timeinfo->tm_mon + 1;                      // Mes (0 a 11) -> +1
  int anio = timeinfo->tm_year + 1900;                 // Año desde 1900
  // Formatear fecha dd/mm/aaaa
  char fecha[11]; // "dd/mm/aaaa" + null terminator
  sprintf(fecha, "%02d/%02d/%04d", dia, mes, anio);
  solounavez = 0;
  Serial.printf("M8AX - %s >>> Completados %s Share(s), %s Khashes, Prom. Hashrate %s KH/s %s°\n",
                mineria.currentTime.c_str(), mineria.completedShares.c_str(), mineria.totalKHashes.c_str(), mineria.currentHashRate.c_str(), mineria.temp.c_str());
  if (hasChangedScreen)
    tft.pushImage(0, 0, initWidth, initHeight, M8AXRelojLunar);
  printPoolData();
  refresca++;
  hasChangedScreen = false;
  int wdtOffset = 190;
  if (solouna == 0)
  {
    numfrases++;
    solouna = 1;
    refresca = 0;
    tft.fillRect(0, 0, 320, 170, TFT_BLACK);
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
    int millonario = atoi(mineria.valids.c_str());
    if (millonario == 0)
    {
      tft.print(mineria.currentTime + " - " + String(fecha) + " - " + mineria.currentHashRate + " KH/s - " + mineria.temp + " Grados." + " NO RICO");
    }
    else
    {
      tft.print(mineria.currentTime + " - " + String(fecha) + " - " + mineria.currentHashRate + " KH/s - " + mineria.temp + " Grados." + " SI RICO");
    }
  }
  if (refresca > 10)
  {
    refresca = 0;
    tft.fillRect(0, 0, 320, 170, TFT_BLACK);
    numfrases++;
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
    int millonario = atoi(mineria.valids.c_str());
    if (millonario == 0)
    {
      tft.print(mineria.currentTime + " - " + String(fecha) + " - " + mineria.currentHashRate + " KH/s - " + mineria.temp + " Grados." + " NO RICO");
    }
    else
    {
      tft.print(mineria.currentTime + " - " + String(fecha) + " - " + mineria.currentHashRate + " KH/s - " + mineria.temp + " Grados." + " SI RICO");
    }
  }
  cuentita++;
  if (cuentita == 15)
  {
    ajustarZonaHoraria();
    obtenerLocYTemp();
    obtenerVariacionesBTC(variacion1dia, variacion1semana);
  }
  manejandoLeds(mineria.currentHashRate.toFloat());
#ifdef DEBUG_MEMORY
  // Print heap
  printheap();
#endif
}

void tDisplay_m8axScreen7(unsigned long mElapsed)
{
  mineria = getMiningData(mElapsed);
  unsigned long epochTime = timeClient.getEpochTime(); // Obtener segundos desde 1970
  time_t epoch = (time_t)epochTime;                    // Convertir a time_t
  struct tm *timeinfo = localtime(&epoch);             // Convertir a estructura de tiempo local
  int dia = timeinfo->tm_mday;                         // Día del mes (1 a 31)
  int mes = timeinfo->tm_mon + 1;                      // Mes (0 a 11) -> +1
  int anio = timeinfo->tm_year + 1900;                 // Año desde 1900
  int horita = timeinfo->tm_hour;                      // Hora
  int minutitos = timeinfo->tm_min;                    // Minutos
  int segundos = timeinfo->tm_sec;                     // Segundos
  Serial.printf("M8AX - %s >>> Completados %s Share(s), %s Khashes, Prom. Hashrate %s KH/s %s°\n",
                mineria.currentTime.c_str(), mineria.completedShares.c_str(), mineria.totalKHashes.c_str(), mineria.currentHashRate.c_str(), mineria.temp.c_str());
  if (hasChangedScreen)
    tft.pushImage(0, 0, initWidth, initHeight, M8AXRelojLunar);
  printPoolData();
  refresca++;
  hasChangedScreen = false;
  int wdtOffset = 190;
  if (refresca > 0)
  {
    refresca = 0;
    tft.fillRect(0, 0, 320, 170, TFT_BLACK);
  }
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
  tft.setTextSize(2);
  tft.setCursor(3, 92);
  int numero = random(1, 51);
  if (numero == 1)
  {
    tft.print("Por Muchas Vueltas Que Demos, Siempre Tendremos El Culo Atras...");
  }
  else if (numero == 2)
  {
    tft.print("El Futuro No Esta Establecido, No Hay Destino, Solo Existe El Que Nosotros Hacemos...");
  }
  else if (numero == 3)
  {
    tft.print("Oro Digital: Minado Con Codigo, Asegurado Con Blockchain...");
  }
  else if (numero == 4)
  {
    tft.print("Quien Controla El Dinero, Controla La Sociedad. Bitcoin Le Devuelve El Poder A La Gente.");
  }
  else if (numero == 5)
  {
    tft.print("No Confies, Verifica. La Cadena De Bloques No Miente, Pero Los Gobiernos Si.");
  }
  else if (numero == 6)
  {
    tft.print("El Dinero Fuerte Crea Civilizaciones Fuertes; La Inflacion Es El Impuesto De Los Tiranos.");
  }
  else if (numero == 7)
  {
    tft.print("El Oro Fue Dinero Durante Siglos, Bitcoin Lo Es En La Era Digital.");
  }
  else if (numero == 8)
  {
    tft.print("Las Reglas Del Sistema Financiero Tradicional Benefician A Pocos; Bitcoin Cambia El Juego.");
  }
  else if (numero == 9)
  {
    tft.print("El Tiempo Es La Moneda Mas Valiosa. Bitcoin Es La Unica Que No Se Devalua.");
  }
  else if (numero == 10)
  {
    tft.print("Los Bancos Imprimen Dinero Sin Control, Bitcoin Solo 21 Millones. Haz Los Calculos.");
  }
  else if (numero == 11)
  {
    tft.print("Blockchain No Duerme, No Cierra, No Pide Permiso.");
  }
  else if (numero == 12)
  {
    tft.print("Si Tu Dinero No Es Tuyo, Es Del Banco.");
  }
  else if (numero == 13)
  {
    tft.print("Bitcoin No Se Quema, No Se Devalua, No Se Congela.");
  }
  else if (numero == 14)
  {
    tft.print("Los Billeteros Pierden Llaves, Bitcoin No Pierde Valor.");
  }
  else if (numero == 15)
  {
    tft.print("Fiat Es Promesa, Bitcoin Es Codigo.");
  }
  else if (numero == 16)
  {
    tft.print("Las Ballenas Compran En La Caida, Los Novatos Venden.");
  }
  else if (numero == 17)
  {
    tft.print("Bitcoin Es Paciencia, No Loteria.");
  }
  else if (numero == 18)
  {
    tft.print("El Precio Sube Y Baja, Pero Bitcoin Se Queda.");
  }
  else if (numero == 19)
  {
    tft.print("Si No Controlas Tus Llaves, No Controlas Tu Dinero.");
  }
  else if (numero == 20)
  {
    tft.print("El Dinero Bueno Expulsa Al Malo, Bitcoin Gana.");
  }
  else if (numero == 21)
  {
    tft.print("Bitcoin No Pide Permiso, Se Usa.");
  }
  else if (numero == 22)
  {
    tft.print("Las Reglas De Bitcoin Son Claras, Las Del Banco Cambian Diario.");
  }
  else if (numero == 23)
  {
    tft.print("Si Puedes Imprimirlo, No Es Escaso.");
  }
  else if (numero == 24)
  {
    tft.print("Tu Banco Puede Cerrar, Tu Wallet No.");
  }
  else if (numero == 25)
  {
    tft.print("El Mercado Es Volatil, Pero La Inflacion Es Segura.");
  }
  else if (numero == 26)
  {
    tft.print("Minar Es Convertir Electricidad En Dinero.");
  }
  else if (numero == 27)
  {
    tft.print("Blockchain No Pregunta Quien Eres, Solo Si Tienes La Llave.");
  }
  else if (numero == 28)
  {
    tft.print("El Futuro Financiero Es Descentralizado.");
  }
  else if (numero == 29)
  {
    tft.print("Bitcoin No Es Anonimo, Pero Es Libre.");
  }
  else if (numero == 30)
  {
    tft.print("El Primer Bitcoin Que Compras Es El Mas Dificil.");
  }
  else if (numero == 31)
  {
    tft.print("Tu Abuela No Entiende Bitcoin, Pero Tampoco Entiende SWIFT.");
  }
  else if (numero == 32)
  {
    tft.print("El Dinero Facil Se Imprime, Bitcoin Se Gana.");
  }
  else if (numero == 33)
  {
    tft.print("El Verdadero Hodler No Se Preocupa Por La Caida.");
  }
  else if (numero == 34)
  {
    tft.print("Bitcoin No Te Hace Rico Rapido, Pero Te Protege De Hacerte Pobre.");
  }
  else if (numero == 35)
  {
    tft.print("Si La Luz Se Apaga, El Efectivo No Funciona; Bitcoin Si.");
  }
  else if (numero == 36)
  {
    tft.print("Los Bancos Se Rescatan, Los Bitcoiners Se Adaptan.");
  }
  else if (numero == 37)
  {
    tft.print("La Revolucion No Sera Televisada, Sera Minada.");
  }
  else if (numero == 38)
  {
    tft.print("Bitcoin No Se Imprime, Se Descubre.");
  }
  else if (numero == 39)
  {
    tft.print("La Verdad Es Inmutable, Como La Blockchain.");
  }
  else if (numero == 40)
  {
    tft.print("Bitcoin No Es Para Todos, Solo Para Quienes Lo Entienden.");
  }
  else if (numero == 41)
  {
    tft.print("Tu Banco Te Cobra Por Guardar Tu Dinero, Bitcoin No.");
  }
  else if (numero == 42)
  {
    tft.print("Las Transacciones Son Baratas Hasta Que El Banco Cobra Comision.");
  }
  else if (numero == 43)
  {
    tft.print("Bitcoin No Tiene Fronteras, Pero Los Bancos Si.");
  }
  else if (numero == 44)
  {
    tft.print("No Es Demasiado Tarde, Solo Es Mas Caro Que Ayer.");
  }
  else if (numero == 45)
  {
    tft.print("Bitcoin No Necesita Rescates, Solo Tiempo.");
  }
  else if (numero == 46)
  {
    tft.print("El Precio No Es Importante, La Adopcion Lo Es.");
  }
  else if (numero == 47)
  {
    tft.print("El Efecto Red Hace Bitcoin Mas Fuerte Cada Dia.");
  }
  else if (numero == 48)
  {
    tft.print("El Dinero Centralizado Es Herramienta De Control.");
  }
  else if (numero == 49)
  {
    tft.print("Bitcoin Es Un Experimento Que Funciona.");
  }
  else if (numero == 50)
  {
    tft.print("No Es Magia, Es Matematica.");
  }
  tft.setCursor(128, 160);
  tft.setTextSize(1);
  tft.print(mineria.currentHashRate + " KH/s");
  cuentita++;
  if (cuentita == 15)
  {
    ajustarZonaHoraria();
    obtenerLocYTemp();
    obtenerVariacionesBTC(variacion1dia, variacion1semana);
  }
  manejandoLeds(mineria.currentHashRate.toFloat());
#ifdef DEBUG_MEMORY
  // Print heap
  printheap();
#endif
}

void tDisplay_m8axScreen3(unsigned long mElapsed)
{
  mineria = getMiningData(mElapsed);
  solounavez = 0;
  solouna = 0;
  String btcm8 = "bitcoin:bc1qljq00pm2plq2l9jxzdzt0xc8t79j9wcmu7r8em";
  Serial.printf("M8AX - %s >>> Completados %s Share(s), %s Khashes, Prom. Hashrate %s KH/s %s°\n",
                mineria.currentTime.c_str(), mineria.completedShares.c_str(), mineria.totalKHashes.c_str(), mineria.currentHashRate.c_str(), mineria.temp.c_str());
  if (hasChangedScreen)
    tft.pushImage(0, 0, initWidth, initHeight, M8AXRelojLunar);
  printPoolData();
  refresca++;
  hasChangedScreen = false;
  int wdtOffset = 190;
  if (refresca > 0)
  {
    refresca = 0;
    tft.fillRect(0, 0, 320, 170, TFT_BLACK);
  }
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
  tft.setCursor(128, 160);
  tft.setTextSize(1);
  tft.print(mineria.currentHashRate + " KH/s");
  uint16_t colorss[] = {TFT_WHITE, TFT_YELLOW, TFT_CYAN, TFT_GREENYELLOW}; // Array de colores
  int colorrr = esp_random() % 4;
  dibujaQR(btcm8, (320 - 150) / 2, (170 - 150) / 2, 150, colorss[colorrr]); // Dibuja el QR centrado en la pantalla 320x170
  cuentita++;
  if (cuentita == 15)
  {
    ajustarZonaHoraria();
    obtenerLocYTemp();
    obtenerVariacionesBTC(variacion1dia, variacion1semana);
  }
  manejandoLeds(mineria.currentHashRate.toFloat());
#ifdef DEBUG_MEMORY
  // Print heap
  printheap();
#endif
}

void tDisplay_m8axScreenMegaNerd(unsigned long mElapsed)
{
  mineria = getMiningData(mElapsed);
  unsigned long epochTime = timeClient.getEpochTime(); // Obtener segundos desde 1970
  time_t epoch = (time_t)epochTime;                    // Convertir a time_t
  struct tm *timeinfo = localtime(&epoch);             // Convertir a estructura de tiempo local
  int dia = timeinfo->tm_mday;                         // Día del mes (1 a 31)
  int mes = timeinfo->tm_mon + 1;                      // Mes (0 a 11) -> +1
  int anio = timeinfo->tm_year + 1900;                 // Año desde 1900
  int horita = timeinfo->tm_hour;                      // Hora
  int minutitos = timeinfo->tm_min;                    // Minutos
  int segundos = timeinfo->tm_sec;                     // Segundos
  // Formatear fecha dd/mm/aaaa
  char fecha[11]; // "dd/mm/aaaa" + null terminator
  sprintf(fecha, "%02d/%02d/%04d", dia, mes, anio);
  solounavez = 0;
  int horas = horita, minutos = minutitos;
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  Serial.printf("M8AX - %s >>> Completados %s Share(s), %s Khashes, Prom. Hashrate %s KH/s %s°\n",
                mineria.currentTime.c_str(), mineria.completedShares.c_str(), mineria.totalKHashes.c_str(), mineria.currentHashRate.c_str(), mineria.temp.c_str());
  printPoolData();
  refresca++;
  if (refresca > 0)
  {
    tft.pushImage(0, 0, initWidth, initHeight, M8AXRelojLunar);
    refresca = 0;
  }
  hasChangedScreen = false;
  int wdtOffset = 190;
  const char *hashRateStr = mineria.currentHashRate.c_str(); // Obtener la cadena
  float hashRa = atof(hashRateStr);                          // Convertirla a flotante (float)
  dibujaAnalogKH(hashRa);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(1);
  uint32_t rndnumero2 = esp_random();
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
  int millonario = atoi(mineria.valids.c_str());
  if (millonario == 0)
  {
    tft.print(mineria.currentTime + " - " + String(fecha) + " - " + mineria.currentHashRate + " KH/s - " + mineria.temp + " Grados." + " NO RICO");
  }
  else
  {
    tft.print(mineria.currentTime + " - " + String(fecha) + " - " + mineria.currentHashRate + " KH/s - " + mineria.temp + " Grados." + " SI RICO");
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
  uint64_t opsseg = (uint64_t)(hashRa * 1000) * 2560;
  tft.setCursor(4, 75);
  tft.print((unsigned long long)opsseg);
  tft.setCursor(4, 90);
  tft.setTextColor(colors[colorI]);
  tft.print("GH/Dia");
  tft.setTextColor(TFT_WHITE);
  float ghdia = hashRa * 86400.0f / 1000000.0f;
  tft.setCursor(4, 100);
  char buffer[20];              // Buffer para almacenar el número formateado
  dtostrf(ghdia, 1, 3, buffer); // (valor, ancho mínimo, decimales, buffer)
  tft.print(buffer);
  tft.setCursor(4, 115);
  tft.setTextColor(colors[colorI]);
  tft.print("GOPS");
  tft.setTextColor(TFT_WHITE);
  float gops = ((hashRa * 1000) * 2560) / 1e9f;
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
  float Tops = ((hashRa * 1000) * 2560) / 1e12f;
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
  tft.print(mineria.temp);
  tft.setTextSize(1);
  tft.setCursor(283, 103);
  tft.print("o");
  tft.setCursor(208, 138);
  tft.print("HMS");
  std::string quediase = obtenerDiaSemana(std::string(fecha));
  tft.setCursor(153, 68);
  tft.setTextColor(colors[colorI]);
  tft.print(quediase.c_str());
  tft.setTextColor(TFT_WHITE);
  int X_INICIO = 230;                                      // Inicio de la barra ( izquierda )
  int X_FINAL = 310;                                       // Final de la barra ( derecha )
  int Y_POS = 147;                                         // Altura de la barra
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
  cuentita++;
  if (cuentita == 15)
  {
    ajustarZonaHoraria();
    obtenerLocYTemp();
    obtenerVariacionesBTC(variacion1dia, variacion1semana);
  }
  manejandoLeds(mineria.currentHashRate.toFloat());
#ifdef DEBUG_MEMORY
  // Print heap
  printheap();
#endif
}

void esp32_2432S028R_BTCprice(unsigned long mElapsed)
{
  mineria = getMiningData(mElapsed);
  unsigned long epochTime = timeClient.getEpochTime(); // Obtener segundos desde 1970
  time_t epoch = (time_t)epochTime;                    // Convertir a time_t
  struct tm *timeinfo = localtime(&epoch);             // Convertir a estructura de tiempo local
  int dia = timeinfo->tm_mday;                         // Día del mes (1 a 31)
  int mes = timeinfo->tm_mon + 1;                      // Mes (0 a 11) -> +1
  int anio = timeinfo->tm_year + 1900;                 // Año desde 1900
  int hora = timeinfo->tm_hour;                        // Hora
  int minuto = timeinfo->tm_min;                       // Minutos
  int segundo = timeinfo->tm_sec;                      // Segundos
  int segundosDelDia = hora * 3600 + minuto * 60 + segundo;
  detectar2 = 0;
  tft.setTextFont(1);
  tft.setTextSize(1);
  if (detectar == 0)
  {
    prebitco = preciob().c_str();
    alturabloque = alturab().c_str();
  }
  detectar++;
  if (detectar % 60 == 0)
  {
    prebitco = preciob().c_str();
    alturabloque = alturab().c_str();
  }
  // Formatear la hora
  char hora_formateada[9]; // "HH:MM:SS" + null
  snprintf(hora_formateada, sizeof(hora_formateada), "%02d:%02d:%02d", hora, minuto, segundo);
  // Formatear fecha dd/mm/aaaa
  char fecha[11]; // "dd/mm/aaaa" + null terminator
  sprintf(fecha, "%02d/%02d/%04d", dia, mes, anio);
  if (hasChangedScreen)
    tft.pushImage(0, 0, priceScreenWidth, priceScreenHeight, priceScreen);
  printPoolData();
  hasChangedScreen = false;
  // Create background sprite to print data at once
  createBackgroundSprite(270, 36);
  // Print background screen
  background.pushImage(0, -130, priceScreenWidth, priceScreenHeight, priceScreen);
  // Hashrate
  render.setFontSize(23);
  render.setFontColor(TFT_BLACK);
  render.rdrawString(mineria.currentHashRate.c_str(), 95, 0, TFT_BLACK);
  // Print BlockHeight
  render.setFontSize(18);
  render.rdrawString(alturabloque.c_str(), 254, 9, TFT_WHITE);
  // Push prepared background to screen
  background.pushSprite(0, 130);
  // Delete sprite to free the memory heap
  background.deleteSprite();
  createBackgroundSprite(169, 105);
  // Print background screen
  background.pushImage(-130, -3, priceScreenWidth, priceScreenHeight, priceScreen);
  // Print Hour
  background.setFreeFont(FSSB9);
  background.setTextSize(1);
  background.setTextDatum(TL_DATUM);
  background.setTextColor(TFT_BLACK);
  background.drawString(fecha, 202 - 147, 0, GFXFF);
  // Print BTC Price
  background.setFreeFont(FF23);
  background.setTextDatum(TL_DATUM);
  background.setTextColor(TFT_WHITE);
  background.drawString(prebitco, 0, 40, GFXFF);
  background.setTextDatum(TL_DATUM);
  background.setTextColor(TFT_WHITE);
  background.drawString(hora_formateada, 0, 78, GFXFF);
  // Push prepared background to screen
  background.pushSprite(130, 3);
  // Delete sprite to free the memory heap
  background.deleteSprite();
  Serial.printf("M8AX - %s >>> Completados %s Share(s), %s Khashes, Prom. Hashrate %s KH/s %s°\n",
                mineria.currentTime.c_str(), mineria.completedShares.c_str(), mineria.totalKHashes.c_str(), mineria.currentHashRate.c_str(), mineria.temp.c_str());
  cuentita++;
  if (cuentita == 15)
  {
    ajustarZonaHoraria();
    obtenerLocYTemp();
    obtenerVariacionesBTC(variacion1dia, variacion1semana);
  }
  int X_INICIO = 124;
  int X_FINAL = 312;
  int Y_POS = 117;
  int longitud_total = X_FINAL - X_INICIO;
  int longitud_pintada = (longitud_total * segundosDelDia) / 86400;
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.fillRect(X_INICIO, Y_POS - 1, longitud_total + 1, 4, TFT_DARKGREY);
  tft.fillRect(X_INICIO, Y_POS - 1, longitud_pintada, 4, colors[colorI]);
  X_INICIO = 124, X_FINAL = 312, Y_POS = 32;
  longitud_total = X_FINAL - X_INICIO;
  longitud_pintada = (longitud_total * segundo) / 59;
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.fillRect(X_INICIO, Y_POS - 1, longitud_total + 1, 4, TFT_DARKGREY);
  tft.fillRect(X_INICIO, Y_POS - 1, longitud_pintada, 4, colors[colorI]);
  manejandoLeds(mineria.currentHashRate.toFloat());
#ifdef DEBUG_MEMORY
  // Print heap
  printheap();
#endif
}

void tDisplay_granHash(unsigned long mElapsed)
{
  mineria = getMiningData(mElapsed);
  unsigned long epochTime = timeClient.getEpochTime(); // Obtener segundos desde 1970
  time_t epoch = (time_t)epochTime;                    // Convertir a time_t
  struct tm *timeinfo = localtime(&epoch);             // Convertir a estructura de tiempo local
  int dia = timeinfo->tm_mday;                         // Día del mes (1 a 31)
  int mes = timeinfo->tm_mon + 1;                      // Mes (0 a 11) -> +1
  int diaDelAno = timeinfo->tm_yday;                   // 0-365
  int anio = timeinfo->tm_year + 1900;                 // Año desde 1900
  int hora = timeinfo->tm_hour;                        // Hora
  int minuto = timeinfo->tm_min;                       // Minutos
  int segundo = timeinfo->tm_sec;                      // Segundos
  int segundosDelDia = hora * 3600 + minuto * 60 + segundo;
  uint32_t segundosDelAno = diaDelAno * 86400 + segundosDelDia, segundosTotalesDelAno = esBisiesto(anio) ? 366 * 86400 : 365 * 86400;
  // Formatear la hora
  char hora_formateada[9]; // "HH:MM:SS" + null
  snprintf(hora_formateada, sizeof(hora_formateada), "%02d:%02d:%02d", hora, minuto, segundo);
  // Formatear fecha dd/mm/aaaa
  char fecha[11]; // "dd/mm/aaaa" + null terminator
  sprintf(fecha, "%02d/%02d/%04d", dia, mes, anio);
  detectar = 0;
  Serial.printf("M8AX - %s >>> Completados %s Share(s), %s Khashes, Prom. Hashrate %s KH/s %s°\n",
                mineria.currentTime.c_str(), mineria.completedShares.c_str(), mineria.totalKHashes.c_str(), mineria.currentHashRate.c_str(), mineria.temp.c_str());
  if (hasChangedScreen)
    tft.pushImage(0, 0, initWidth, initHeight, M8AXQuote2);
  printPoolData();
  refresca++;
  if (refresca > 0)
  {
    tft.pushImage(0, 0, initWidth, initHeight, M8AXQuote2);
    refresca = 0;
  }
  hasChangedScreen = false;
  int wdtOffset = 190;
  tft.setFreeFont(FF24);
  tft.setTextSize(2);
  tft.setCursor(18, 115);
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.setTextColor(colors[colorI]);
  tft.print(mineria.currentHashRate);
  tft.setTextFont(2);
  tft.setTextSize(2);
  tft.setCursor(10, 1);
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.setTextColor(colors[colorI]);
  tft.print(String(hora_formateada) + " - " + fecha);
  tft.setCursor(107, 138);
  tft.print(String(prebitco));
  tft.setTextFont(1);
  tft.setTextSize(1);
  tft.setCursor(1, 125);
  tft.print("PRECIO 1 DIA");
  tft.setCursor(210, 125);
  tft.print("   PRECIO 1 SEMANA");
  tft.setCursor(23, 149);
  tft.print(variacion1dia);
  tft.setCursor(255, 149);
  tft.print(variacion1semana);
  tft.setCursor(28, 125);
  tft.print("          BARRA DE HASH/S 0-400");
  cuentita++;
  if (cuentita == 15)
  {
    ajustarZonaHoraria();
    obtenerLocYTemp();
    obtenerVariacionesBTC(variacion1dia, variacion1semana);
  }
  if (cuentita % 60 == 0)
  {
    prebitco = preciob().c_str();
    obtenerVariacionesBTC(variacion1dia, variacion1semana);
  }
  int X_INICIO = 228;
  int X_FINAL = 316;
  int Y_POS = 135;
  int longitud_total = X_FINAL - X_INICIO;
  int longitud_pintada = (longitud_total * segundosDelDia) / 86400;
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.fillRect(X_INICIO, Y_POS - 1, longitud_total + 1, 3, TFT_DARKGREY);
  tft.drawLine(X_INICIO, Y_POS, X_INICIO + longitud_pintada, Y_POS, colors[colorI]);
  X_INICIO = 1;
  X_FINAL = 72;
  Y_POS = 135;
  longitud_total = X_FINAL - X_INICIO;
  longitud_pintada = (longitud_total * segundo) / 59;
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.fillRect(X_INICIO, Y_POS - 1, longitud_total + 1, 3, TFT_DARKGREY);
  tft.drawLine(X_INICIO, Y_POS, X_INICIO + longitud_pintada, Y_POS, colors[colorI]);
  X_INICIO = 10;
  X_FINAL = 308;
  Y_POS = 35;
  longitud_total = X_FINAL - X_INICIO;
  longitud_pintada = (longitud_total * (segundosDelAno / 60)) / (segundosTotalesDelAno / 60);
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.fillRect(X_INICIO, Y_POS - 1, longitud_total + 1, 7, TFT_DARKGREY);
  tft.fillRect(X_INICIO, Y_POS - 1, longitud_pintada, 7, colors[colorI]);
  X_INICIO = 87;
  X_FINAL = 213;
  Y_POS = 135;
  longitud_total = X_FINAL - X_INICIO;
  longitud_pintada = (longitud_total * mineria.currentHashRate.toDouble()) / 400.00;
  if (longitud_pintada > longitud_total)
    longitud_pintada = longitud_total;
  if (longitud_pintada < 0)
    longitud_pintada = 0;
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.fillRect(X_INICIO, Y_POS - 1, longitud_total + 1, 3, TFT_DARKGREY);
  tft.drawLine(X_INICIO, Y_POS, X_INICIO + longitud_pintada, Y_POS, colors[colorI]);
  manejandoLeds(mineria.currentHashRate.toFloat());
#ifdef DEBUG_MEMORY
  // Print heap
  printheap();
#endif
}

const int HASHG_S = 18;                 // lado de cada bloque (3 cifras = 17 columnas x 18 = 306 px, cabe en 320)
const bool HASHG_CEROS = true;          // true = 005 (siempre 3 dígitos), false = 5
const bool HASHG_MOSTRAR_UNIDAD = true; // false = quita el "KH/s" pequeñito

// Mismas cifras 5x7 que el banner (el bit 0 es la fila de arriba)
static const uint8_t HASHG_FONT[10][5] = {
    {0x3E, 0x51, 0x49, 0x45, 0x3E}, // 0
    {0x00, 0x42, 0x7F, 0x40, 0x00}, // 1
    {0x72, 0x49, 0x49, 0x49, 0x46}, // 2
    {0x21, 0x41, 0x49, 0x4D, 0x33}, // 3
    {0x18, 0x14, 0x12, 0x7F, 0x10}, // 4
    {0x27, 0x45, 0x45, 0x45, 0x39}, // 5
    {0x3C, 0x4A, 0x49, 0x49, 0x31}, // 6
    {0x41, 0x21, 0x11, 0x09, 0x07}, // 7
    {0x36, 0x49, 0x49, 0x49, 0x36}, // 8
    {0x46, 0x49, 0x49, 0x29, 0x1E}  // 9
};
static const uint16_t HASHG_COLORES[8] = {TFT_RED, TFT_GREEN, TFT_CYAN, TFT_YELLOW, TFT_MAGENTA, TFT_ORANGE, TFT_WHITE, TFT_SKYBLUE};
static int hashgUltimo = -1;
static unsigned long hashgSeg = 0xFFFFFFFF;
static uint8_t hashgColor = 0;

void tDisplay_hashGigante(unsigned long mElapsed)
{
  mineria = getMiningData(mElapsed);
  detectar = 0;

  // Barra de criptos de abajo (ella sola decide cuándo repintarse)
  printPoolData();

  bool repintar = hasChangedScreen;
  hasChangedScreen = false;

  // Hashrate sin decimales, máximo 3 dígitos
  int hr = (int)mineria.currentHashRate.toFloat();
  if (hr < 0)
    hr = 0;
  if (hr > 999)
    hr = 999;

  unsigned long seg = millis() / 1000;

  // Repinta si cambió el valor, si cambió el segundo (color nuevo) o si acabamos de entrar
  if (repintar || hr != hashgUltimo || seg != hashgSeg)
  {
    if (seg != hashgSeg)
      hashgColor = (hashgColor + 1) % 8;
    hashgSeg = seg;
    hashgUltimo = hr;

    char txt[4];
    snprintf(txt, sizeof(txt), HASHG_CEROS ? "%03d" : "%d", hr);
    int n = strlen(txt);

    const int S = HASHG_S;
    int ancho = (n * 6 - 1) * S;
    int x0 = (320 - ancho) / 2; // centrado en horizontal
    int y0 = (170 - 7 * S) / 2; // centrado en vertical

    if (repintar)
    {
      tft.fillRect(0, 0, 320, 170, TFT_BLACK);
      if (HASHG_MOSTRAR_UNIDAD)
      {
        tft.setTextFont(1);
        tft.setTextSize(1);
        tft.setTextColor(TFT_DARKGREY, TFT_BLACK);
        tft.setCursor(290, 160);
        tft.print("KH/s");
      }
    }
    else
      tft.fillRect(0, y0, 320, 7 * S, TFT_BLACK);

    uint16_t color = HASHG_COLORES[hashgColor];
    for (int d = 0; d < n; d++)
    {
      int g = txt[d] - '0';
      int xd = x0 + d * 6 * S;
      for (int f = 0; f < 7; f++)
      {
        int k = 0;
        while (k < 5)
        {
          if (!(HASHG_FONT[g][k] & (1 << f)))
          {
            k++;
            continue;
          }
          int ini = k;
          while (k < 5 && (HASHG_FONT[g][k] & (1 << f)))
            k++;
          tft.fillRect(xd + ini * S, y0 + f * S, (k - ini) * S, S, color);
        }
      }
    }
  }
  cuentita++;
  if (cuentita == 15)
  {
    ajustarZonaHoraria();
    obtenerLocYTemp();
    obtenerVariacionesBTC(variacion1dia, variacion1semana);
  }
  manejandoLeds(mineria.currentHashRate.toFloat());
}

void esp32_2432S028R_LoadingScreen(void)
{
  int effect = esp_random() % 10;
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
  case 8:
    nevar();
    break;
  case 9:
    nevar2();
    break;
  }
  tft.fillScreen(TFT_BLACK);
  tft.pushImage(0, 33, initWidth, initHeight, initScreen);
  tft.setTextColor(TFT_BLACK);
  tft.drawString(CURRENT_VERSION, 24, 182, FONT2);
  // delay(2000);
  // tft.fillScreen(TFT_BLACK);
  // tft.pushImage(0, 0, initWidth, initHeight, MinerScreen);
}

void esp32_2432S028R_SetupScreen(void)
{
  tft.fillScreen(TFT_BLACK);
  tft.pushImage(0, 33, setupModeWidth, setupModeHeight, setupModeScreen);
}

extern unsigned long previousTouchMillis; // se define mas abajo, la reutilizo para no duplicar toques

#define PONG_YT 34  // linea superior del campo
#define PONG_YB 148 // linea inferior del campo
#define PONG_PW 4   // ancho de pala
#define PONG_PH 24  // alto de pala
#define PONG_BS 5   // tamaño de la bola
#define PONG_LX 8   // x de la pala izquierda (CPU2)
#define PONG_RX 308 // x de la pala derecha (CPU1)
#define PONG_SPEED0 2.4f
#define PONG_SPEEDMAX 6.0f
#define PONG_PADDLE_SPEED 3.0f
#define PONG_SLICE_MS 5000 // tandas largas = menos cortes entre tandas (el tactil se atiende dentro)

struct PongState
{
  bool init;
  float bx, by, vx, vy; // bola (esquina superior izquierda)
  float lp, rp;         // y (arriba) de cada pala
  float lerr, rerr;     // error de punteria de cada CPU en este golpe
  int sL, sR;           // sL = CPU2 (izquierda), sR = CPU1 (derecha)
  unsigned long serveAt;
};
static PongState pg = {false};

// Donde llegara la bola (centro) a la x indicada, contando rebotes en las paredes
static float pongPredict(float bx, float by, float vx, float vy, float tx)
{
  float top = PONG_YT + 2;
  float h = (PONG_YB - PONG_BS) - top;
  float t = (tx - bx) / vx;
  float y = by + vy * t - top;
  float r = fmodf(y, 2.0f * h);
  if (r < 0)
    r += 2.0f * h;
  if (r > h)
    r = 2.0f * h - r;
  return top + r + PONG_BS / 2.0f;
}

// Error de punteria: mas rapida la bola, mas impreciso; de vez en cuando se despista del todo
static float pongNewErr()
{
  float sp = fabsf(pg.vx);
  float amp = 5.0f + (sp - PONG_SPEED0) * 2.0f;
  if (amp > 11.0f)
    amp = 11.0f;
  float e = ((int)(esp_random() % 2001) - 1000) / 1000.0f * amp;
  if (esp_random() % 100 < 9)
    e += ((esp_random() & 1) ? 1.0f : -1.0f) * 20.0f;
  return e;
}

static void pongServe(int dir)
{
  pg.bx = 160 - PONG_BS / 2;
  pg.by = (PONG_YT + PONG_YB) / 2 - PONG_BS / 2 + (int)(esp_random() % 41) - 20;
  float ang = ((int)(esp_random() % 101) - 50) / 50.0f; // -1..1
  pg.vx = PONG_SPEED0 * dir;
  pg.vy = ang * 1.8f;
  pg.lerr = pongNewErr();
  pg.rerr = pongNewErr();
  pg.serveAt = millis() + 700;
}

static void pongCenterLine()
{
  for (int y = PONG_YT + 2; y < PONG_YB; y += 8)
    tft.fillRect(159, y, 2, 4, TFT_WHITE);
}

static void pongDrawField()
{
  tft.fillRect(0, 0, 320, 170, TFT_BLACK);
  tft.fillRect(0, PONG_YT, 320, 2, TFT_WHITE);
  tft.fillRect(0, PONG_YB, 320, 2, TFT_WHITE);
  pongCenterLine();
}

static void pongDrawScores()
{
  char b[16];
  tft.setFreeFont(NULL);
  tft.setTextFont(1);
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextPadding(144);
  tft.setTextDatum(TL_DATUM);
  snprintf(b, sizeof(b), "CPU-1 %06d", pg.sL);
  tft.drawString(b, 4, 153);
  tft.setTextDatum(TR_DATUM);
  snprintf(b, sizeof(b), "CPU-2 %06d", pg.sR);
  tft.drawString(b, 316, 153);
  tft.setTextPadding(0);
  tft.setTextDatum(TL_DATUM);
}

static void pongDrawHud(const char *hash, const char *hora)
{
  tft.setFreeFont(NULL);
  tft.setTextFont(1);
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  // Hora arriba a la izquierda
  tft.setTextPadding(100);
  tft.setTextDatum(TL_DATUM);
  tft.drawString(hora, 4, 9);
  // Hashrate arriba a la derecha
  char b[24];
  snprintf(b, sizeof(b), "%s KH/s", hash);
  tft.setTextPadding(150);
  tft.setTextDatum(TR_DATUM);
  tft.drawString(b, 316, 9);
  tft.setTextPadding(0);
  tft.setTextDatum(TL_DATUM);
}

// Mueve una pala repintando solo lo que cambia (sin parpadeo)
static void pongMovePaddle(int x, int oldY, int newY)
{
  if (oldY == newY)
    return;
  int d = abs(newY - oldY);
  if (d >= PONG_PH)
  {
    tft.fillRect(x, oldY, PONG_PW, PONG_PH, TFT_BLACK);
    tft.fillRect(x, newY, PONG_PW, PONG_PH, TFT_WHITE);
  }
  else if (newY > oldY)
  {
    tft.fillRect(x, oldY, PONG_PW, d, TFT_BLACK);
    tft.fillRect(x, oldY + PONG_PH, PONG_PW, d, TFT_WHITE);
  }
  else
  {
    tft.fillRect(x, newY + PONG_PH, PONG_PW, d, TFT_BLACK);
    tft.fillRect(x, newY, PONG_PW, d, TFT_WHITE);
  }
}

static inline float pongClampPaddle(float p)
{
  if (p < PONG_YT + 2)
    return PONG_YT + 2;
  if (p > PONG_YB - PONG_PH)
    return PONG_YB - PONG_PH;
  return p;
}

// IA de una pala. toward = la bola viene hacia ella
static void pongMoveAI(float &p, bool toward, float contactX, float err, float dt)
{
  float target;
  float spd;
  if (toward)
  {
    float c = pongPredict(pg.bx, pg.by, pg.vx, pg.vy, contactX);
    target = c + err - PONG_PH / 2.0f;
    spd = PONG_PADDLE_SPEED;
  }
  else
  {
    target = (PONG_YT + 2 + PONG_YB - PONG_PH) / 2.0f; // vuelve al centro
    spd = 1.2f;
  }
  target = pongClampPaddle(target);
  float d = target - p;
  float m = spd * dt;
  if (d > m)
    d = m;
  else if (d < -m)
    d = -m;
  p = pongClampPaddle(p + d);
}

// Un paso de fisica (dt <= 1)
static bool pongStep(float dt)
{
  bool scored = false;
  pg.bx += pg.vx * dt;
  pg.by += pg.vy * dt;

  // Paredes
  float ymin = PONG_YT + 2, ymax = PONG_YB - PONG_BS;
  if (pg.by < ymin)
  {
    pg.by = ymin + (ymin - pg.by);
    pg.vy = -pg.vy;
  }
  else if (pg.by > ymax)
  {
    pg.by = ymax - (pg.by - ymax);
    pg.vy = -pg.vy;
  }

  float c = pg.by + PONG_BS / 2.0f;

  // Pala izquierda (CPU2)
  if (pg.vx < 0 && pg.bx <= PONG_LX + PONG_PW && pg.bx + PONG_BS >= PONG_LX &&
      c >= pg.lp - 1 && c <= pg.lp + PONG_PH + 1)
  {
    float off = (c - (pg.lp + PONG_PH / 2.0f)) / (PONG_PH / 2.0f);
    float sp = fabsf(pg.vx) * 1.04f;
    if (sp > PONG_SPEEDMAX)
      sp = PONG_SPEEDMAX;
    pg.bx = PONG_LX + PONG_PW;
    pg.vx = sp;
    pg.vy = pg.vy * 0.4f + off * 3.0f;
    pg.rerr = pongNewErr();
  }
  // Pala derecha (CPU1)
  else if (pg.vx > 0 && pg.bx + PONG_BS >= PONG_RX && pg.bx <= PONG_RX + PONG_PW &&
           c >= pg.rp - 1 && c <= pg.rp + PONG_PH + 1)
  {
    float off = (c - (pg.rp + PONG_PH / 2.0f)) / (PONG_PH / 2.0f);
    float sp = fabsf(pg.vx) * 1.04f;
    if (sp > PONG_SPEEDMAX)
      sp = PONG_SPEEDMAX;
    pg.bx = PONG_RX - PONG_BS;
    pg.vx = -sp;
    pg.vy = pg.vy * 0.4f + off * 3.0f;
    pg.lerr = pongNewErr();
  }
  else
  {
    // Evita tiros demasiado planos y limita el angulo
    if (fabsf(pg.vy) > 3.4f)
      pg.vy = (pg.vy > 0) ? 3.4f : -3.4f;
    if (pg.bx + PONG_BS < 0) // se le escapa a CPU2 -> punto de CPU1
    {
      pg.sR++;
      scored = true;
      pongServe(-1);
    }
    else if (pg.bx > 320) // se le escapa a CPU1 -> punto de CPU2
    {
      pg.sL++;
      scored = true;
      pongServe(1);
    }
  }
  if (fabsf(pg.vy) > 3.4f)
    pg.vy = (pg.vy > 0) ? 3.4f : -3.4f;
  if (fabsf(pg.vy) < 0.35f)
    pg.vy = (esp_random() & 1) ? 0.8f : -0.8f;
  return scored;
}

#define PONG_LED_MS 80 // cuanto dura el destello (ms)
static void pongLeds(bool encendidos)
{
  digitalWrite(LED_PIN, encendidos ? LOW : HIGH);
  digitalWrite(LED_PIN_G, encendidos ? LOW : HIGH);
  digitalWrite(LED_PIN_B, encendidos ? LOW : HIGH);
}

static void pongLedRojo(bool encendido)
{
  digitalWrite(LED_PIN, encendido ? LOW : HIGH);
  digitalWrite(LED_PIN_G, HIGH);
  digitalWrite(LED_PIN_B, HIGH);
}

#define LED_FLASH_MS 20
static void ledFlash(bool r, bool g, bool b, uint16_t ms)
{
  digitalWrite(LED_PIN, r ? LOW : HIGH);
  digitalWrite(LED_PIN_G, g ? LOW : HIGH);
  digitalWrite(LED_PIN_B, b ? LOW : HIGH);
  delay(ms);
  digitalWrite(LED_PIN, HIGH);
  digitalWrite(LED_PIN_G, HIGH);
  digitalWrite(LED_PIN_B, HIGH);
}

void esp32_2432S028R_PongScreen(unsigned long mElapsed)
{
  mineria = getMiningData(mElapsed);
  Serial.printf("M8AX - %s >>> Completados %s Share(s), %s Khashes, Prom. Hashrate %s KH/s %s°\n",
                mineria.currentTime.c_str(), mineria.completedShares.c_str(), mineria.totalKHashes.c_str(), mineria.currentHashRate.c_str(), mineria.temp.c_str());

  bool redraw = hasChangedScreen;
  if (!pg.init)
  {
    pg.init = true;
    pg.lp = pg.rp = (PONG_YT + 2 + PONG_YB - PONG_PH) / 2.0f;
    pg.sL = pg.sR = 0;
    pongServe((esp_random() & 1) ? 1 : -1);
    redraw = true;
  }

  if (redraw)
    pongDrawField();
  printPoolData(); // pie de las criptos (solo se repinta cuando toca)
  hasChangedScreen = false;

  char horaStr[9] = "";
  char lastHora[9] = "";
  char hashStr[16];
  char lastHash[16] = "";
  int lastSL = -1, lastSR = -1;
  if (redraw)
  {
    lastHora[0] = 0;
    lastHash[0] = 0;
  }

  int olx = (int)pg.lp, orx = (int)pg.rp;
  int obx = (int)pg.bx, oby = (int)pg.by;
  // Palas y bola en su sitio al empezar cada tanda
  tft.fillRect(PONG_LX, olx, PONG_PW, PONG_PH, TFT_WHITE);
  tft.fillRect(PONG_RX, orx, PONG_PW, PONG_PH, TFT_WHITE);
  tft.fillRect(obx, oby, PONG_BS, PONG_BS, TFT_WHITE);

  unsigned long start = millis();
  unsigned long last = start;
  unsigned long lastHud = 0;
  unsigned long lastMiningUpdate = 0;
  unsigned long lastTouchChk = 0;
  unsigned long ledApagarEn = 0; // 0 = LEDs apagados
  bool golpePala = false;
  pongLeds(false); // empieza con los LEDs apagados

  while (millis() - start < PONG_SLICE_MS)
  {
    unsigned long now = millis();
    float dt = (now - last) / 16.667f;
    if (dt > 3.0f)
      dt = 3.0f;
    last = now;

    // ---- Tactil (misma logica que esp32_2432S028R_DoLedStuff) ----
    if (now - lastTouchChk >= 40 && now - previousTouchMillis >= 500)
    {
      lastTouchChk = now;
      int16_t t_x, t_y;
      if (toqueXY(t_x, t_y))
      {
        previousTouchMillis = now;
        if (((t_x > 109) && (t_x < 211)) && ((t_y > 185) && (t_y < 241)))
        {
          bottomScreenBlue ^= true;
          hasChangedScreen = true;
        }
        else if ((t_x > 235) && ((t_y > 0) && (t_y < 16)))
        {
          esp32_2432S028R_AlternateScreenState();
        }
        else if (t_x > 160)
        {
          currentDisplayDriver->current_cyclic_screen = (currentDisplayDriver->current_cyclic_screen + 1) % currentDisplayDriver->num_cyclic_screens;
        }
        else
        {
          currentDisplayDriver->current_cyclic_screen = currentDisplayDriver->current_cyclic_screen - 1;
          if (currentDisplayDriver->current_cyclic_screen < 0)
            currentDisplayDriver->current_cyclic_screen = currentDisplayDriver->num_cyclic_screens - 1;
        }
        break;
      }
    }

    // ---- IA ----
    bool serving = (now < pg.serveAt);
    pongMoveAI(pg.lp, !serving && pg.vx < 0, PONG_LX + PONG_PW, pg.lerr, dt);
    pongMoveAI(pg.rp, !serving && pg.vx > 0, PONG_RX - PONG_BS, pg.rerr, dt);

    // ---- Fisica (subpasos para que no atraviese las palas) ----
    bool scored = false;
    if (!serving)
    {
      int steps = (int)ceilf(dt * fabsf(pg.vx) / 3.0f);
      if (steps < 1)
        steps = 1;
      for (int i = 0; i < steps; i++)
      {
        float vxAntes = pg.vx;
        bool punto = pongStep(dt / steps);
        // Si la bola cambia de sentido y no hay punto, ha rebotado en una pala
        if (!punto && ((vxAntes < 0) != (pg.vx < 0)))
          golpePala = true;
        if (punto)
        {
          scored = true;
          break;
        }
      }
    }
    if (pg.sL > 999999 || pg.sR > 999999)
    {
      pg.sL = pg.sR = 0;
      scored = true;
    }

    // ---- LEDs: golpe de pala -> todos a tope y se apagan enseguida ----
    if (ledApagarEn && now >= ledApagarEn)
    {
      pongLeds(false);
      ledApagarEn = 0;
    }
    if (golpePala)
    {
      pongLeds(true);
      ledApagarEn = now + PONG_LED_MS;
      golpePala = false;
    }

    // ---- Dibujo ----
    int nlx = (int)(pg.lp + 0.5f), nrx = (int)(pg.rp + 0.5f);
    int nbx = (int)pg.bx, nby = (int)pg.by;

    pongMovePaddle(PONG_LX, olx, nlx);
    pongMovePaddle(PONG_RX, orx, nrx);
    olx = nlx;
    orx = nrx;

    if (scored)
    {
      // La bola vieja se borra y se repinta el campo entre las lineas
      tft.fillRect(0, PONG_YT + 2, 320, PONG_YB - PONG_YT - 2, TFT_BLACK);
      pongCenterLine();
      tft.fillRect(PONG_LX, olx, PONG_PW, PONG_PH, TFT_WHITE);
      tft.fillRect(PONG_RX, orx, PONG_PW, PONG_PH, TFT_WHITE);
      tft.fillRect(nbx, nby, PONG_BS, PONG_BS, TFT_WHITE);
      obx = nbx;
      oby = nby;
    }
    else if (nbx != obx || nby != oby)
    {
      tft.fillRect(obx, oby, PONG_BS, PONG_BS, TFT_BLACK);
      if (abs(obx - 160) < 10 || abs(nbx - 160) < 10)
        pongCenterLine();
      if (obx <= PONG_LX + PONG_PW + 6 || obx + PONG_BS >= PONG_RX - 6)
      {
        tft.fillRect(PONG_LX, olx, PONG_PW, PONG_PH, TFT_WHITE);
        tft.fillRect(PONG_RX, orx, PONG_PW, PONG_PH, TFT_WHITE);
      }
      tft.fillRect(nbx, nby, PONG_BS, PONG_BS, TFT_WHITE);
      obx = nbx;
      oby = nby;
    }

    // ---- Marcadores ----
    if (pg.sL != lastSL || pg.sR != lastSR || redraw)
    {
      pongDrawScores();
      lastSL = pg.sL;
      lastSR = pg.sR;
      redraw = false;
    }

    // ---- Hashrate y hora (arriba a la derecha) ----
    if (now - lastMiningUpdate >= 1000)
    {
      lastMiningUpdate = now;

      // Consultar nuevamente los datos de mineria
      mineria = getMiningData(mElapsed);

      // Actualizar hashrate
      snprintf(hashStr, sizeof(hashStr), "%s",
               mineria.currentHashRate.c_str());

      // Actualizar hora
      time_t epoch = (time_t)timeClient.getEpochTime();
      struct tm *ti = localtime(&epoch);

      snprintf(horaStr, sizeof(horaStr), "%02d:%02d:%02d",
               ti->tm_hour, ti->tm_min, ti->tm_sec);

      if (strcmp(horaStr, lastHora) != 0 ||
          strcmp(hashStr, lastHash) != 0)
      {
        pongDrawHud(hashStr, horaStr);

        strcpy(lastHora, horaStr);
        strcpy(lastHash, hashStr);
      }
    }
    webAtender();
    delay(10);
  }

  pongLeds(false); // por si salimos con los LEDs encendidos
  tft.setTextPadding(0);
  tft.setTextDatum(TL_DATUM);

  cuentita++;
  if (cuentita == 15)
  {
    ajustarZonaHoraria();
    obtenerLocYTemp();
    obtenerVariacionesBTC(variacion1dia, variacion1semana);
  }
}

#define LIFE_CS 4          // tamano de celda en pixeles
#define LIFE_W 80          // 320 / 4
#define LIFE_H 38          // 152 / 4 (abajo quedan 18 px para los textos)
#define LIFE_STEP_MS 15    // ms entre generaciones
#define LIFE_SLICE_MS 5000 // tandas largas, como en el Pong
#define LIFE_HIST 24       // generaciones recordadas para detectar repeticion
#define LIFE_MAX_GEN 3000  // reinicia aunque no se repita
#define LIFE_PAUSE_MS 1000 // pausa mostrando el dibujo final antes de reiniciar

struct LifeState
{
  bool init;
  bool needSeed;
  uint8_t age[LIFE_W * LIFE_H];   // 0 = muerta, 1..255 = edad
  uint8_t shown[LIFE_W * LIFE_H]; // banda de color pintada ahora (0 = negro)
  uint32_t gen;
  uint32_t hist[LIFE_HIST];
  uint8_t histN;
  unsigned long pauseUntil;
};
static LifeState lf = {false};

static const uint16_t LIFE_COLORS[7] = {0x0000, 0xFFFF, 0x07FF, 0x07E0, 0xFFE0, 0xFD20, 0xF81F};

static inline uint8_t lifeBand(uint8_t a)
{
  if (a == 0)
    return 0;
  if (a == 1)
    return 1;
  if (a <= 3)
    return 2;
  if (a <= 8)
    return 3;
  if (a <= 20)
    return 4;
  if (a <= 60)
    return 5;
  return 6;
}

// Semilla simetrica: se rellena un cuarto al azar y se refleja en los otros tres
static void lifeSeed()
{
  memset(lf.age, 0, sizeof(lf.age));
  for (int y = 0; y < LIFE_H / 2; y++)
  {
    for (int x = 0; x < LIFE_W / 2; x++)
    {
      if (esp_random() % 100 < 25)
      {
        int x2 = LIFE_W - 1 - x;
        int y2 = LIFE_H - 1 - y;
        lf.age[y * LIFE_W + x] = 1;
        lf.age[y * LIFE_W + x2] = 1;
        lf.age[y2 * LIFE_W + x] = 1;
        lf.age[y2 * LIFE_W + x2] = 1;
      }
    }
  }
  lf.gen = 0;
  lf.histN = 0;
  memset(lf.hist, 0, sizeof(lf.hist));
}

// Una generacion (mundo toroidal: lo que sale por un borde entra por el contrario).
// Devuelve cuantas celdas hay vivas y un hash del estado para detectar repeticiones.
static int lifeStep(uint32_t &hash)
{
  static uint8_t nx[LIFE_W * LIFE_H];
  int alive = 0;
  hash = 2166136261u;
  for (int y = 0; y < LIFE_H; y++)
  {
    int yu = ((y + LIFE_H - 1) % LIFE_H) * LIFE_W;
    int yc = y * LIFE_W;
    int yd = ((y + 1) % LIFE_H) * LIFE_W;
    for (int x = 0; x < LIFE_W; x++)
    {
      int xl = (x + LIFE_W - 1) % LIFE_W;
      int xr = (x + 1) % LIFE_W;
      int n = (lf.age[yu + xl] > 0) + (lf.age[yu + x] > 0) + (lf.age[yu + xr] > 0) +
              (lf.age[yc + xl] > 0) + (lf.age[yc + xr] > 0) +
              (lf.age[yd + xl] > 0) + (lf.age[yd + x] > 0) + (lf.age[yd + xr] > 0);
      uint8_t a = lf.age[yc + x];
      uint8_t r;
      if (a)
        r = (n == 2 || n == 3) ? (a < 255 ? a + 1 : 255) : 0;
      else
        r = (n == 3) ? 1 : 0;
      nx[yc + x] = r;
      if (r)
        alive++;
      hash = (hash ^ (r ? 1u : 0u)) * 16777619u;
    }
  }
  memcpy(lf.age, nx, sizeof(nx));
  return alive;
}

// Pinta solo las celdas cuyo color ha cambiado (sin parpadeo)
static void lifeDrawCells(bool force)
{
  if (force)
  {
    tft.fillRect(0, 0, 320, LIFE_H * LIFE_CS, TFT_BLACK);
    memset(lf.shown, 0, sizeof(lf.shown));
  }
  for (int y = 0; y < LIFE_H; y++)
  {
    for (int x = 0; x < LIFE_W; x++)
    {
      int i = y * LIFE_W + x;
      uint8_t b = lifeBand(lf.age[i]);
      if (b != lf.shown[i])
      {
        tft.fillRect(x * LIFE_CS, y * LIFE_CS, LIFE_CS - 1, LIFE_CS - 1, LIFE_COLORS[b]);
        lf.shown[i] = b;
      }
    }
  }
}

static void lifeDrawHud(const char *izq, const char *der)
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

void esp32_2432S028R_LifeScreen(unsigned long mElapsed)
{
  mineria = getMiningData(mElapsed);
  Serial.printf("M8AX - %s >>> Completados %s Share(s), %s Khashes, Prom. Hashrate %s KH/s %s°\n",
                mineria.currentTime.c_str(), mineria.completedShares.c_str(), mineria.totalKHashes.c_str(), mineria.currentHashRate.c_str(), mineria.temp.c_str());

  bool redraw = hasChangedScreen;
  if (!lf.init)
  {
    lf.init = true;
    lf.needSeed = false;
    lifeSeed();
    redraw = true;
  }

  if (redraw)
  {
    tft.fillRect(0, 0, 320, 170, TFT_BLACK);
    lifeDrawCells(true);
  }
  printPoolData(); // pie de las criptos (solo se repinta cuando toca)
  hasChangedScreen = false;

  char hashStr[16];
  snprintf(hashStr, sizeof(hashStr), "%s", mineria.currentHashRate.c_str());
  char izq[24], der[24];
  char lastIzq[24] = "";
  char lastDer[24] = "";

  unsigned long start = millis();
  unsigned long lastStep = 0;
  unsigned long lastHud = 0;
  unsigned long lastMiningUpdate = 0;
  unsigned long lastTouchChk = 0;
  unsigned long ledApagarEn = 0; // 0 = LEDs apagados
  pongLeds(false);

  while (millis() - start < LIFE_SLICE_MS)
  {
    unsigned long now = millis();

    // ---- Tactil (misma logica que esp32_2432S028R_DoLedStuff) ----
    if (now - lastTouchChk >= 40 && now - previousTouchMillis >= 500)
    {
      lastTouchChk = now;
      int16_t t_x, t_y;
      if (toqueXY(t_x, t_y))
      {
        previousTouchMillis = now;
        if (((t_x > 109) && (t_x < 211)) && ((t_y > 185) && (t_y < 241)))
        {
          bottomScreenBlue ^= true;
          hasChangedScreen = true;
        }
        else if ((t_x > 235) && ((t_y > 0) && (t_y < 16)))
        {
          esp32_2432S028R_AlternateScreenState();
        }
        else if (t_x > 160)
        {
          currentDisplayDriver->current_cyclic_screen = (currentDisplayDriver->current_cyclic_screen + 1) % currentDisplayDriver->num_cyclic_screens;
        }
        else
        {
          currentDisplayDriver->current_cyclic_screen = currentDisplayDriver->current_cyclic_screen - 1;
          if (currentDisplayDriver->current_cyclic_screen < 0)
            currentDisplayDriver->current_cyclic_screen = currentDisplayDriver->num_cyclic_screens - 1;
        }
        break;
      }
    }

    // ---- LEDs: destello corto cuando se siembra una vida nueva ----
    if (ledApagarEn && now >= ledApagarEn)
    {
      pongLeds(false);
      ledApagarEn = 0;
    }

    // ---- Reinicio: tras la pausa se siembra otra vida ----
    if (lf.needSeed && now >= lf.pauseUntil)
    {
      lifeSeed();
      lifeDrawCells(true);
      lf.needSeed = false;
      pongLeds(true);
      ledApagarEn = now + PONG_LED_MS;
    }

    // ---- Una generacion ----
    if (!lf.needSeed && now - lastStep >= LIFE_STEP_MS)
    {
      lastStep = now;
      uint32_t h;
      int alive = lifeStep(h);

      // Si el estado ya ha aparecido en las ultimas generaciones, esta estancado u oscila
      bool repetido = false;
      for (int i = 0; i < lf.histN; i++)
      {
        if (lf.hist[i] == h)
        {
          repetido = true;
          break;
        }
      }
      lf.hist[lf.gen % LIFE_HIST] = h;
      if (lf.histN < LIFE_HIST)
        lf.histN++;
      lf.gen++;

      lifeDrawCells(false);

      if (alive == 0 || repetido || lf.gen >= LIFE_MAX_GEN)
      {
        lf.needSeed = true;
        lf.pauseUntil = now + LIFE_PAUSE_MS;
      }
    }

    // ---- Hashrate (cada segundo) ----
    if (now - lastMiningUpdate >= 1000)
    {
      lastMiningUpdate = now;
      mineria = getMiningData(mElapsed);
      snprintf(hashStr, sizeof(hashStr), "%s", mineria.currentHashRate.c_str());
    }

    // ---- Textos de abajo: generacion y hashrate ----
    if (now - lastHud >= 200)
    {
      lastHud = now;
      snprintf(izq, sizeof(izq), "GEN %lu", (unsigned long)lf.gen);
      snprintf(der, sizeof(der), "%s KH/s", hashStr);
      if (strcmp(izq, lastIzq) != 0 || strcmp(der, lastDer) != 0)
      {
        lifeDrawHud(izq, der);
        strcpy(lastIzq, izq);
        strcpy(lastDer, der);
      }
    }
    webAtender();
    delay(10);
  }

  pongLeds(false); // por si salimos con los LEDs encendidos
  tft.setTextPadding(0);
  tft.setTextDatum(TL_DATUM);

  cuentita++;
  if (cuentita == 15)
  {
    ajustarZonaHoraria();
    obtenerLocYTemp();
    obtenerVariacionesBTC(variacion1dia, variacion1semana);
  }
}

#define BRK_COLS 16
#define BRK_ROWS 7
#define BRK_W 20          // ancho de ladrillo (16 * 20 = 320)
#define BRK_H 8           // alto de ladrillo
#define BRK_Y0 12         // y donde empiezan los ladrillos
#define BRK_PY 140        // y de la pala
#define BRK_PH 5          // alto de la pala
#define BRK_PW 40         // ancho de la pala
#define BRK_BS 4          // tamano de la bola
#define BRK_SLICE_MS 5000 // tandas largas, como en el Pong
#define BRK_LIVES 3

struct BrkState
{
  bool init;
  uint8_t br[BRK_ROWS * BRK_COLS]; // 0 = vacio, 1 = ladrillo, 2 = ladrillo duro
  float bx, by, vx, vy;            // bola (esquina superior izquierda)
  float px;                        // x (izquierda) de la pala
  float aim, err;                  // punteria de la CPU para el proximo golpe
  int lives, level, left;
  unsigned long score;
  unsigned long serveAt;
  unsigned long clearUntil; // 0 = jugando; si no, pausa de nivel completado
};
static BrkState bk = {false};

static const uint16_t BRK_COLORS[BRK_ROWS] = {0xF800, 0xFD20, 0xFFE0, 0x07E0, 0x07FF, 0x041F, 0xF81F};
#define BRK_HARD_COLOR 0xC618

static float brkSpeed()
{
  float s = 2.6f + 0.18f * (bk.level - 1);
  return s > 4.6f ? 4.6f : s;
}

// Dibujo de cada nivel (se repite cada 6 niveles, pero mas rapido)
static uint8_t brkPattern(int level, int r, int c)
{
  int cm = c < BRK_COLS - 1 - c ? c : BRK_COLS - 1 - c; // columna espejada
  bool on;
  switch ((level - 1) % 6)
  {
  case 0: // lleno
    on = true;
    break;
  case 1: // ajedrez
    on = ((r + c) % 2) == 0;
    break;
  case 2: // piramide invertida
    on = (c >= r && c < BRK_COLS - r);
    break;
  case 3: // columnas con huecos
    on = (c % 4) != 3;
    break;
  case 4: // marco + bloque central
    on = (r == 0 || r == BRK_ROWS - 1 || c == 0 || c == BRK_COLS - 1 ||
          (r >= 2 && r <= 4 && c >= 5 && c <= 10));
    break;
  default: // simetrico irregular
    on = ((r * 7 + cm * 13 + level * 5) % 5) != 0;
    break;
  }
  if (!on)
    return 0;
  if (level >= 8 && r <= 1)
    return 2;
  if (level >= 4 && r == 0)
    return 2;
  return 1;
}

static void brkDrawBrick(int r, int c)
{
  int x = c * BRK_W, y = BRK_Y0 + r * BRK_H;
  uint8_t h = bk.br[r * BRK_COLS + c];
  if (h == 0)
    tft.fillRect(x, y, BRK_W, BRK_H, TFT_BLACK);
  else
    tft.fillRect(x + 1, y + 1, BRK_W - 2, BRK_H - 2, h == 2 ? BRK_HARD_COLOR : BRK_COLORS[r]);
}

static void brkDrawAllBricks()
{
  for (int r = 0; r < BRK_ROWS; r++)
    for (int c = 0; c < BRK_COLS; c++)
      if (bk.br[r * BRK_COLS + c])
        brkDrawBrick(r, c);
}

static void brkBuildLevel()
{
  bk.left = 0;
  for (int r = 0; r < BRK_ROWS; r++)
    for (int c = 0; c < BRK_COLS; c++)
    {
      uint8_t h = brkPattern(bk.level, r, c);
      bk.br[r * BRK_COLS + c] = h;
      if (h)
        bk.left++;
    }
}

static void brkNewAim()
{
  bk.aim = ((int)(esp_random() % 1401) - 700) / 1000.0f;                    // -0.7..0.7
  bk.err = ((int)(esp_random() % 1001) - 500) / 1000.0f * (BRK_PW * 0.16f); // fallo pequeno
  if (esp_random() % 100 < 2)                                               // a veces se despista
    bk.err = ((esp_random() & 1) ? 1.0f : -1.0f) * BRK_PW * 0.85f;
}

// Bola pegada a la pala esperando para salir
static void brkServe()
{
  bk.bx = bk.px + BRK_PW / 2.0f - BRK_BS / 2.0f;
  bk.by = BRK_PY - BRK_BS;
  float sp = brkSpeed();
  float a = ((int)(esp_random() % 1001) - 500) / 1000.0f * 0.9f; // -0.45..0.45 rad
  bk.vx = sp * sinf(a);
  bk.vy = -sp * cosf(a);
  bk.serveAt = millis() + 700;
  brkNewAim();
}

// Comprueba las 4 esquinas de la bola; si toca un ladrillo lo golpea y devuelve true
static bool brkHitBricks(float x, float y)
{
  float cx[4] = {x, x + BRK_BS - 1, x, x + BRK_BS - 1};
  float cy[4] = {y, y, y + BRK_BS - 1, y + BRK_BS - 1};
  for (int i = 0; i < 4; i++)
  {
    int py = (int)cy[i] - BRK_Y0;
    int px = (int)cx[i];
    if (py < 0 || px < 0 || px >= 320)
      continue;
    int r = py / BRK_H, c = px / BRK_W;
    if (r >= BRK_ROWS)
      continue;
    uint8_t &h = bk.br[r * BRK_COLS + c];
    if (h)
    {
      h--;
      if (h == 0)
      {
        bk.left--;
        bk.score += (BRK_ROWS - r) * 10;
        if (bk.score > 999999)
          bk.score = 0;
      }
      brkDrawBrick(r, c);
      return true;
    }
  }
  return false;
}

// Donde llegara el centro de la bola a la altura de la pala (con rebotes en las paredes)
static float brkPredict()
{
  float w = 320 - BRK_BS;
  float t = (BRK_PY - BRK_BS - bk.by) / bk.vy;
  float x = bk.bx + bk.vx * t;
  float r = fmodf(x, 2.0f * w);
  if (r < 0)
    r += 2.0f * w;
  if (r > w)
    r = 2.0f * w - r;
  return r + BRK_BS / 2.0f;
}

#define BRK_EV_PALA 1
#define BRK_EV_LADRILLO 2
#define BRK_EV_PERDIDA 4

static int brkStep(float dt)
{
  int ev = 0;
  float ox = bk.bx, oy = bk.by;

  // Eje X
  bk.bx += bk.vx * dt;
  if (bk.bx < 0)
  {
    bk.bx = -bk.bx;
    bk.vx = -bk.vx;
  }
  else if (bk.bx > 320 - BRK_BS)
  {
    bk.bx = 2 * (320 - BRK_BS) - bk.bx;
    bk.vx = -bk.vx;
  }
  if (brkHitBricks(bk.bx, bk.by))
  {
    bk.bx = ox;
    bk.vx = -bk.vx;
    ev |= BRK_EV_LADRILLO;
  }

  // Eje Y
  bk.by += bk.vy * dt;
  if (bk.by < 0)
  {
    bk.by = -bk.by;
    bk.vy = -bk.vy;
  }
  if (brkHitBricks(bk.bx, bk.by))
  {
    bk.by = oy;
    bk.vy = -bk.vy;
    ev |= BRK_EV_LADRILLO;
  }

  // Pala
  if (bk.vy > 0 && bk.by + BRK_BS >= BRK_PY && bk.by <= BRK_PY + BRK_PH &&
      bk.bx + BRK_BS >= bk.px && bk.bx <= bk.px + BRK_PW)
  {
    float off = (bk.bx + BRK_BS / 2.0f - (bk.px + BRK_PW / 2.0f)) / (BRK_PW / 2.0f);
    if (off > 1)
      off = 1;
    if (off < -1)
      off = -1;
    float sp = sqrtf(bk.vx * bk.vx + bk.vy * bk.vy) * 1.012f;
    if (sp > 5.2f)
      sp = 5.2f;
    float ang = off * 1.05f; // hasta ~60 grados
    bk.vx = sp * sinf(ang);
    bk.vy = -sp * cosf(ang);
    bk.by = BRK_PY - BRK_BS;
    brkNewAim();
    ev |= BRK_EV_PALA;
  }
  else if (bk.by > BRK_PY + BRK_PH + 1)
  {
    ev |= BRK_EV_PERDIDA;
  }
  return ev;
}

// Mueve la pala repintando solo lo que cambia (sin parpadeo)
static void brkMovePaddle(int oldX, int newX)
{
  if (oldX == newX)
    return;
  int d = abs(newX - oldX);
  if (d >= BRK_PW)
  {
    tft.fillRect(oldX, BRK_PY, BRK_PW, BRK_PH, TFT_BLACK);
    tft.fillRect(newX, BRK_PY, BRK_PW, BRK_PH, TFT_WHITE);
  }
  else if (newX > oldX)
  {
    tft.fillRect(oldX, BRK_PY, d, BRK_PH, TFT_BLACK);
    tft.fillRect(oldX + BRK_PW, BRK_PY, d, BRK_PH, TFT_WHITE);
  }
  else
  {
    tft.fillRect(newX + BRK_PW, BRK_PY, d, BRK_PH, TFT_BLACK);
    tft.fillRect(newX, BRK_PY, d, BRK_PH, TFT_WHITE);
  }
}

static void brkDrawHud(const char *izq, const char *der)
{
  tft.setFreeFont(NULL);
  tft.setTextFont(1);
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextPadding(160);
  tft.setTextDatum(TL_DATUM);
  tft.drawString(izq, 4, 153);
  tft.setTextPadding(150);
  tft.setTextDatum(TR_DATUM);
  tft.drawString(der, 316, 153);
  tft.setTextPadding(0);
  tft.setTextDatum(TL_DATUM);
}

static void brkNewGame()
{
  bk.level = 1;
  bk.lives = BRK_LIVES;
  bk.score = 0;
  bk.px = (320 - BRK_PW) / 2.0f;
  bk.clearUntil = 0;
  brkBuildLevel();
  brkServe();
}

void esp32_2432S028R_KrakoutScreen(unsigned long mElapsed)
{
  mineria = getMiningData(mElapsed);
  Serial.printf("M8AX - %s >>> Completados %s Share(s), %s Khashes, Prom. Hashrate %s KH/s %s°\n",
                mineria.currentTime.c_str(), mineria.completedShares.c_str(), mineria.totalKHashes.c_str(), mineria.currentHashRate.c_str(), mineria.temp.c_str());

  bool redraw = hasChangedScreen;
  if (!bk.init)
  {
    bk.init = true;
    brkNewGame();
    redraw = true;
  }

  if (redraw)
  {
    tft.fillRect(0, 0, 320, 170, TFT_BLACK);
    brkDrawAllBricks();
  }
  printPoolData(); // pie de las criptos (solo se repinta cuando toca)
  hasChangedScreen = false;

  char hashStr[16];
  snprintf(hashStr, sizeof(hashStr), "%s", mineria.currentHashRate.c_str());
  char izq[28], der[28];
  char lastIzq[28] = "", lastDer[28] = "";

  int opx = (int)(bk.px + 0.5f);
  int obx = (int)bk.bx, oby = (int)bk.by;
  // Pala y bola en su sitio al empezar cada tanda
  tft.fillRect(opx, BRK_PY, BRK_PW, BRK_PH, TFT_WHITE);
  tft.fillRect(obx, oby, BRK_BS, BRK_BS, TFT_WHITE);

  unsigned long start = millis();
  unsigned long last = start;
  unsigned long lastHud = 0;
  unsigned long lastMiningUpdate = 0;
  unsigned long lastTouchChk = 0;
  unsigned long ledApagarEn = 0; // 0 = LEDs apagados
  pongLeds(false);

  while (millis() - start < BRK_SLICE_MS)
  {
    unsigned long now = millis();
    float dt = (now - last) / 16.667f;
    if (dt > 3.0f)
      dt = 3.0f;
    last = now;

    // ---- Tactil (misma logica que esp32_2432S028R_DoLedStuff) ----
    if (now - lastTouchChk >= 40 && now - previousTouchMillis >= 500)
    {
      lastTouchChk = now;
      int16_t t_x, t_y;
      if (toqueXY(t_x, t_y))
      {
        previousTouchMillis = now;
        if (((t_x > 109) && (t_x < 211)) && ((t_y > 185) && (t_y < 241)))
        {
          bottomScreenBlue ^= true;
          hasChangedScreen = true;
        }
        else if ((t_x > 235) && ((t_y > 0) && (t_y < 16)))
        {
          esp32_2432S028R_AlternateScreenState();
        }
        else if (t_x > 160)
        {
          currentDisplayDriver->current_cyclic_screen = (currentDisplayDriver->current_cyclic_screen + 1) % currentDisplayDriver->num_cyclic_screens;
        }
        else
        {
          currentDisplayDriver->current_cyclic_screen = currentDisplayDriver->current_cyclic_screen - 1;
          if (currentDisplayDriver->current_cyclic_screen < 0)
            currentDisplayDriver->current_cyclic_screen = currentDisplayDriver->num_cyclic_screens - 1;
        }
        break;
      }
    }

    // ---- LEDs: se apagan solos tras el destello ----
    if (ledApagarEn && now >= ledApagarEn)
    {
      pongLeds(false);
      ledApagarEn = 0;
    }

    bool serving = (now < bk.serveAt);

    // ---- Nivel completado: pausa y nivel nuevo ----
    if (bk.clearUntil)
    {
      if (now >= bk.clearUntil)
      {
        bk.clearUntil = 0;
        bk.level++;
        if (bk.lives < BRK_LIVES)
          bk.lives++; // premio: una vida extra (hasta 3)
        tft.fillRect(0, 0, 320, 152, TFT_BLACK);
        brkBuildLevel();
        brkDrawAllBricks();
        brkServe();
        serving = true;
        opx = -1000; // fuerza repintar la pala
        obx = (int)bk.bx;
        oby = (int)bk.by;
        tft.fillRect(obx, oby, BRK_BS, BRK_BS, TFT_WHITE);
      }
    }

    // ---- IA de la pala ----
    {
      float tc, spd;
      if (serving || bk.clearUntil)
      {
        tc = 160;
        spd = 2.0f;
      }
      else if (bk.vy > 0)
      {
        tc = brkPredict() - bk.aim * (BRK_PW / 2.0f) + bk.err;
        spd = 5.0f;
      }
      else
      {
        tc = bk.bx + BRK_BS / 2.0f; // la bola sube: sigue su x con calma
        spd = 2.2f;
      }
      float tx = tc - BRK_PW / 2.0f;
      if (tx < 0)
        tx = 0;
      if (tx > 320 - BRK_PW)
        tx = 320 - BRK_PW;
      float d = tx - bk.px;
      float m = spd * dt;
      if (d > m)
        d = m;
      else if (d < -m)
        d = -m;
      bk.px += d;
      if (bk.px < 0)
        bk.px = 0;
      if (bk.px > 320 - BRK_PW)
        bk.px = 320 - BRK_PW;
    }

    // ---- Fisica ----
    if (serving)
    {
      // la bola va pegada a la pala hasta que sale
      bk.bx = bk.px + BRK_PW / 2.0f - BRK_BS / 2.0f;
      bk.by = BRK_PY - BRK_BS;
    }
    else if (!bk.clearUntil)
    {
      float sp = sqrtf(bk.vx * bk.vx + bk.vy * bk.vy);
      int steps = (int)ceilf(dt * sp / 2.0f);
      if (steps < 1)
        steps = 1;
      for (int i = 0; i < steps; i++)
      {
        int ev = brkStep(dt / steps);
        if (ev & BRK_EV_LADRILLO)
        {
          pongLedRojo(true);
          ledApagarEn = now + PONG_LED_MS;
        }
        if (ev & BRK_EV_PALA)
        {
          pongLeds(true);
          ledApagarEn = now + PONG_LED_MS;
        }
        if (ev & BRK_EV_PERDIDA)
        {
          bk.lives--;
          if (bk.lives < 0)
          {
            // sin vidas: empieza otra partida desde el nivel 1
            bk.level = 1;
            bk.lives = BRK_LIVES;
            bk.score = 0;
            tft.fillRect(0, 0, 320, 152, TFT_BLACK);
            brkBuildLevel();
            brkDrawAllBricks();
            opx = -1000;
          }
          brkServe();
          break;
        }
        if (bk.left <= 0)
        {
          bk.clearUntil = now + 1200;
          pongLeds(true); // destello largo al limpiar el nivel
          ledApagarEn = now + 400;
          break;
        }
      }
    }

    // ---- Dibujo ----
    int npx = (int)(bk.px + 0.5f);
    int nbx = (int)bk.bx, nby = (int)bk.by;
    if (opx == -1000)
    {
      tft.fillRect(npx, BRK_PY, BRK_PW, BRK_PH, TFT_WHITE);
      opx = npx;
    }
    else
    {
      brkMovePaddle(opx, npx);
      opx = npx;
    }
    if (nbx != obx || nby != oby)
    {
      tft.fillRect(obx, oby, BRK_BS, BRK_BS, TFT_BLACK);
      if (oby + BRK_BS >= BRK_PY - 2) // la bola borrada pudo rozar la pala
        tft.fillRect(npx, BRK_PY, BRK_PW, BRK_PH, TFT_WHITE);
      tft.fillRect(nbx, nby, BRK_BS, BRK_BS, TFT_WHITE);
      obx = nbx;
      oby = nby;
    }

    // ---- Hashrate (cada segundo) ----
    if (now - lastMiningUpdate >= 1000)
    {
      lastMiningUpdate = now;
      mineria = getMiningData(mElapsed);
      snprintf(hashStr, sizeof(hashStr), "%s", mineria.currentHashRate.c_str());
    }

    // ---- Textos de abajo: nivel, vidas, puntos y hashrate ----
    if (now - lastHud >= 200)
    {
      lastHud = now;
      int vidas = bk.lives < 0 ? 0 : bk.lives;
      snprintf(izq, sizeof(izq), "N%02d V%d %lu", bk.level, vidas, bk.score);
      snprintf(der, sizeof(der), "%s KH/s", hashStr);
      if (strcmp(izq, lastIzq) != 0 || strcmp(der, lastDer) != 0)
      {
        brkDrawHud(izq, der);
        strcpy(lastIzq, izq);
        strcpy(lastDer, der);
      }
    }
    webAtender();
    delay(10);
  }

  pongLeds(false); // por si salimos con los LEDs encendidos
  tft.setTextPadding(0);
  tft.setTextDatum(TL_DATUM);

  cuentita++;
  if (cuentita == 15)
  {
    ajustarZonaHoraria();
    obtenerLocYTemp();
    obtenerVariacionesBTC(variacion1dia, variacion1semana);
  }
}

/********************************************************************************************
 *  PANTALLA CIFRAS - Juego de cifras que se resuelve solo, en bucle, mientras el minero sigue
 *
 *  Cada vuelta saca 6 numeros al azar (1-100) y un objetivo (1-1000), y busca la mejor
 *  cuenta posible con + - * / usando los numeros como piezas. No hace falta usarlos todos
 *  y solo valen divisiones exactas, sin negativos ni fracciones. Si no hay cuenta exacta,
 *  se queda con la que mas se acerca al objetivo.
 *
 *  La busqueda es por fuerza bruta con poda de numeros repetidos y esta limitada en tiempo
 *  (CIF_BUDGET_MS). Cada cierto numero de nodos cede CPU con vTaskDelay, asi no ahoga al
 *  minero ni al watchdog. Al terminar se descartan los pasos que no intervienen en el
 *  resultado final, de modo que solo se pinta la resolucion que de verdad hace falta.
 *
 *  En pantalla: arriba la hora y el hashrate, en el centro los numeros, el objetivo, la
 *  velocidad (OBJ/S) y la resolucion paso a paso con su resultado (EXACTO en verde, DIF en
 *  naranja), y en el pie los aciertos, los fallos y el porcentaje. Abajo se mantiene la
 *  barra de precios de criptos. Cada cuenta hace parpadear el LED y se vuelca por Serial.
 *
 *  Tactil igual que Pong/Life/Krakout: zona central inferior cambia el fondo, esquina
 *  superior derecha alterna pantalla y los laterales pasan a la siguiente o a la anterior.
 *
 *  Ajustes: CIF_BUDGET_MS (tiempo maximo por cuenta), CIF_PAUSA_MS (pausa entre cuentas)
 *  y CIF_SLICE_MS (duracion de cada tanda antes de devolver el control).
 ********************************************************************************************/

#define CIF_MAX_NUMBERS 6
#define CIF_MAX_STEPS (CIF_MAX_NUMBERS - 1)
#define CIF_BUDGET_MS 1200 // tiempo maximo buscando la mejor cuenta (baja a 800 si lo notas lento)
#define CIF_PAUSA_MS 0     // pausa entre cuenta y cuenta (0 = a toda pastilla)
#define CIF_SLICE_MS 5000  // tandas largas, como en Pong/Life/Krakout

struct CifPaso
{
  int a;
  char op;
  int b;
  int r;
  int ia; // -1 = numero original, si no indice del paso que lo produjo
  int ib;
};

struct CifCtx
{
  int target;
  int bestVal;
  int bestDiff;
  int bestCount;
  CifPaso best[CIF_MAX_STEPS];
  CifPaso cur[CIF_MAX_STEPS];
  uint32_t nodes;
  uint32_t deadline;
  bool done;
};

struct CifState
{
  bool init;
  bool tiene; // ya hay una cuenta calculada para pintar
  int nums[CIF_MAX_NUMBERS];
  int target;
  CifPaso pasos[CIF_MAX_STEPS]; // solo los pasos que de verdad se usan
  int nPasos;
  int valor;
  bool exacto;
  int ac, fa;
  unsigned long nextAt;
};
static CifState cf = {false};
static uint32_t cifWinN = 0;
static unsigned long cifWinT = 0;
static float cifRate = 0.0f;
static unsigned long cifSolveUs = 0;
static uint32_t cifSolveN = 0;

static inline void cif_candidate(CifCtx &c, int value, int steps)
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

static void cif_search(CifCtx &c, const int *vals, const int *ids, int n, int depth)
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
      int rest[CIF_MAX_NUMBERS];
      int restId[CIF_MAX_NUMBERS];
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
          vTaskDelay(1); // cede CPU al minero / watchdog
        }
        c.cur[depth] = {x, op, y, r, ix, iy};
        cif_candidate(c, r, depth + 1);
        if (c.done)
          return;
        if (m >= 1)
        {
          rest[m] = r;
          restId[m] = depth;
          cif_search(c, rest, restId, m + 1, depth + 1);
        }
      }
    }
  }
}

// Genera numeros y objetivo nuevos, busca la mejor cuenta y deja todo en cf
static void cifNuevaCuenta()
{
  static CifCtx c;
  for (int i = 0; i < CIF_MAX_NUMBERS; i++)
    cf.nums[i] = 1 + (esp_random() % 100);
  cf.target = 1 + (esp_random() % 1000);

  c.target = cf.target;
  c.bestVal = cf.nums[0];
  c.bestDiff = abs(cf.nums[0] - cf.target);
  c.bestCount = 0;
  c.nodes = 0;
  c.done = false;
  c.deadline = millis() + CIF_BUDGET_MS;
  for (int i = 0; i < CIF_MAX_NUMBERS && c.bestDiff != 0; i++)
  {
    int d = abs(cf.nums[i] - cf.target);
    if (d < c.bestDiff)
    {
      c.bestDiff = d;
      c.bestVal = cf.nums[i];
    }
  }
  if (c.bestDiff == 0)
    c.done = true;
  if (!c.done)
  {
    int ids0[CIF_MAX_NUMBERS];
    for (int k = 0; k < CIF_MAX_NUMBERS; k++)
      ids0[k] = -1;
    cif_search(c, cf.nums, ids0, CIF_MAX_NUMBERS, 0);
  }

  // Quedarse solo con los pasos que de verdad se usan
  bool usado[CIF_MAX_STEPS];
  for (int k = 0; k < CIF_MAX_STEPS; k++)
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
  cf.nPasos = 0;
  for (int k = 0; k < c.bestCount; k++)
    if (usado[k])
      cf.pasos[cf.nPasos++] = c.best[k];

  cf.valor = c.bestVal;
  cf.exacto = (c.bestVal == cf.target);
  if (cf.exacto)
    cf.ac++;
  else
    cf.fa++;
  cf.tiene = true;
}

static int tgCifJuego = 0, tgCifAc = 0, tgCifFa = 0; // juegos enviados a Telegram, aciertos y fallos

// Juega una cuenta del juego de las cifras y devuelve el texto para Telegram
String cifrasTelegram()
{
  CifState guardado = cf; // la pantalla de Cifras no se entera
  cifNuevaCuenta();
  tgCifJuego++;
  if (cf.exacto)
    tgCifAc++;
  else
    tgCifFa++;
  int total = tgCifAc + tgCifFa;
  float pAc = tgCifAc * 100.0f / total;
  float pFa = tgCifFa * 100.0f / total;

  String res = "... JUEGO " + String(tgCifJuego) + " DE LAS CIFRAS - ( 1 CADA 2H ) ...\n";
  res += "Números -> ";
  for (int i = 0; i < CIF_MAX_NUMBERS; i++)
  {
    res += String(cf.nums[i]);
    if (i < CIF_MAX_NUMBERS - 1)
      res += "  ";
  }
  res += "\nObjetivo -> " + String(cf.target) + "\n";
  for (int k = 0; k < cf.nPasos; k++)
  {
    const CifPaso &p = cf.pasos[k];
    char linea[40];
    snprintf(linea, sizeof(linea), "%d. %d %c %d = %d\n", k + 1, p.a, (p.op == '*') ? 'x' : p.op, p.b, p.r);
    res += linea;
  }
  if (cf.exacto)
    res += "¡ EXACTO !\n";
  else
    res += "No Exacto, Lo Más Cercano -> " + String(cf.valor) + " ( Dif " + String(abs(cf.valor - cf.target)) + " )\n";
  res += "Aciertos -> " + String(tgCifAc) + " ( " + String(pAc, 1) + "% )\n";
  res += "Fallos -> " + String(tgCifFa) + " ( " + String(pFa, 1) + "% )";
  cf = guardado;
  return res;
}

static void cifDrawRate()
{
  char b[12];
  if (cifRate >= 100.0f)
    snprintf(b, sizeof(b), "%.0f", cifRate);
  else
    snprintf(b, sizeof(b), "%.1f", cifRate);
  tft.setFreeFont(NULL);
  tft.setTextFont(1);
  tft.setTextDatum(TR_DATUM);
  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.setTextSize(2);
  tft.setTextPadding(72);
  tft.drawString(b, 316, 67);
  tft.setTextPadding(0);
  tft.setTextSize(1);
  tft.drawString("OBJ/S", 316, 83);
  tft.setTextDatum(TL_DATUM);
}

// Cuerpo de la pantalla (numeros, objetivo y resolucion). Deja libre la linea de arriba y el pie.
static void cifDrawAll()
{
  tft.fillRect(0, 23, 320, 129, TFT_BLACK);
  tft.setFreeFont(NULL);
  tft.setTextFont(1);
  tft.setTextPadding(0);
  tft.drawFastHLine(0, 22, 320, TFT_DARKGREY);

  // Numeros seleccionados
  tft.setTextSize(1);
  tft.setTextDatum(TL_DATUM);
  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.drawString("NUMEROS", 4, 27);
  tft.setTextSize(2);
  tft.setTextDatum(MC_DATUM);
  for (int i = 0; i < CIF_MAX_NUMBERS; i++)
  {
    int x = 4 + i * 52;
    tft.drawRect(x, 38, 48, 24, TFT_CYAN);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawString(String(cf.nums[i]), x + 24, 51);
  }

  // Objetivo
  tft.setTextDatum(TL_DATUM);
  tft.setTextSize(2);
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.drawString("OBJETIVO", 4, 70);
  tft.setTextSize(3);
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.drawString(String(cf.target), 150, 67);
  cifDrawRate();
  tft.drawFastHLine(0, 92, 320, TFT_DARKGREY);

  // Resolucion paso a paso (2 columnas)
  tft.setTextSize(1);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  if (cf.nPasos == 0)
  {
    tft.drawString("Sin Operaciones: Ya Hay Un Numero Asi", 4, 98);
  }
  else
  {
    char b[32];
    for (int i = 0; i < cf.nPasos; i++)
    {
      snprintf(b, sizeof(b), "%d) %d %c %d = %d", i + 1, cf.pasos[i].a, cf.pasos[i].op, cf.pasos[i].b, cf.pasos[i].r);
      tft.drawString(b, 4 + (i % 2) * 160, 98 + (i / 2) * 12);
    }
  }

  // Resultado
  char r[40];
  tft.setTextSize(2);
  if (cf.exacto)
  {
    snprintf(r, sizeof(r), "= %d  EXACTO", cf.valor);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
  }
  else
  {
    snprintf(r, sizeof(r), "= %d  DIF %d", cf.valor, abs(cf.valor - cf.target));
    tft.setTextColor(TFT_ORANGE, TFT_BLACK);
  }
  tft.drawString(r, 4, 134);
  tft.setTextSize(1);
}

// Linea de arriba: hora a la izquierda, hashrate a la derecha
static void cifDrawTop(const char *hora, const char *hash)
{
  tft.setFreeFont(NULL);
  tft.setTextFont(1);
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextPadding(110);
  tft.setTextDatum(TL_DATUM);
  tft.drawString(hora, 4, 4);
  char b[24];
  snprintf(b, sizeof(b), "%s KH/s", hash);
  tft.setTextPadding(190);
  tft.setTextDatum(TR_DATUM);
  tft.drawString(b, 316, 4);
  tft.setTextPadding(0);
  tft.setTextDatum(TL_DATUM);
}

// Pie: aciertos / fallos y porcentaje
static void cifDrawFoot()
{
  char izq[28], der[16];
  int tot = cf.ac + cf.fa;
  float pct = tot > 0 ? (cf.ac * 100.0f) / tot : 0.0f;
  snprintf(izq, sizeof(izq), "AC %d FA %d", cf.ac, cf.fa);
  snprintf(der, sizeof(der), "%.1f%%", pct);
  tft.setFreeFont(NULL);
  tft.setTextFont(1);
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextPadding(200);
  tft.setTextDatum(TL_DATUM);
  tft.drawString(izq, 4, 153);
  tft.setTextPadding(100);
  tft.setTextDatum(TR_DATUM);
  tft.drawString(der, 316, 153);
  tft.setTextPadding(0);
  tft.setTextDatum(TL_DATUM);
}

// Misma logica tactil que Pong/Life/Krakout. Devuelve true si hay que salir de la tanda.
static bool cifTouch(unsigned long now)
{
  int16_t t_x, t_y;
  if (!toqueXY(t_x, t_y))
    return false;
  previousTouchMillis = now;
  if (((t_x > 109) && (t_x < 211)) && ((t_y > 185) && (t_y < 241)))
  {
    bottomScreenBlue ^= true;
    hasChangedScreen = true;
  }
  else if ((t_x > 235) && ((t_y > 0) && (t_y < 16)))
  {
    esp32_2432S028R_AlternateScreenState();
  }
  else if (t_x > 160)
  {
    currentDisplayDriver->current_cyclic_screen = (currentDisplayDriver->current_cyclic_screen + 1) % currentDisplayDriver->num_cyclic_screens;
  }
  else
  {
    currentDisplayDriver->current_cyclic_screen = currentDisplayDriver->current_cyclic_screen - 1;
    if (currentDisplayDriver->current_cyclic_screen < 0)
      currentDisplayDriver->current_cyclic_screen = currentDisplayDriver->num_cyclic_screens - 1;
  }
  return true;
}

void esp32_2432S028R_CifrasScreen(unsigned long mElapsed)
{
  mineria = getMiningData(mElapsed);
  Serial.printf("M8AX - %s >>> Completados %s Share(s), %s Khashes, Prom. Hashrate %s KH/s %s°\n",
                mineria.currentTime.c_str(), mineria.completedShares.c_str(), mineria.totalKHashes.c_str(), mineria.currentHashRate.c_str(), mineria.temp.c_str());

  bool redraw = hasChangedScreen;
  if (!cf.init)
  {
    cf.init = true;
    cf.tiene = false;
    cf.ac = cf.fa = 0;
    cf.nextAt = 0;
    cifRate = 0.0f;
    cifWinN = 0;
    cifSolveUs = 0;
    cifSolveN = 0;
    redraw = true;
  }

  if (redraw)
  {
    tft.fillRect(0, 0, 320, 170, TFT_BLACK);
    if (cf.tiene)
      cifDrawAll();
  }
  printPoolData(); // barra de criptos de abajo
  hasChangedScreen = false;

  char hashStr[16];
  char horaStr[9];
  snprintf(hashStr, sizeof(hashStr), "%s", mineria.currentHashRate.c_str());
  {
    time_t ep = (time_t)timeClient.getEpochTime();
    struct tm *ti = localtime(&ep);
    snprintf(horaStr, sizeof(horaStr), "%02d:%02d:%02d", ti->tm_hour, ti->tm_min, ti->tm_sec);
  }
  cifDrawTop(horaStr, hashStr);
  cifDrawFoot();

  unsigned long start = millis();
  cifWinT = start;
  cifWinN = 0;
  unsigned long lastMining = start;
  unsigned long lastTouchChk = 0;
  pongLeds(false);

  while (millis() - start < CIF_SLICE_MS)
  {
    unsigned long now = millis();

    // ---- Tactil ----
    if (now - lastTouchChk >= 40 && now - previousTouchMillis >= 500)
    {
      lastTouchChk = now;
      if (cifTouch(now))
        break;
    }

    // ---- Cuenta nueva (la busqueda cede CPU con vTaskDelay, no bloquea al minero) ----
    if (now >= cf.nextAt)
    {
      unsigned long t0 = micros();
      cifNuevaCuenta();
      cifSolveUs += micros() - t0;
      cifSolveN++;
      cifWinN++;
      cifDrawAll();
      cifDrawFoot();
      ledFlash(!cf.exacto, cf.exacto, false, LED_FLASH_MS);
      // ---- INFORMACION DE LA CUENTA POR SERIAL ----
      Serial.printf("M8AX - Cifras %d %d %d %d %d %d -> OBJETIVO %d\n",
                    cf.nums[0], cf.nums[1], cf.nums[2],
                    cf.nums[3], cf.nums[4], cf.nums[5],
                    cf.target);

      // Mostrar operaciones realizadas
      if (cf.nPasos == 0)
      {
        Serial.printf("M8AX - Sin Operaciones: Resultado %d\n", cf.valor);
      }
      else
      {
        for (int i = 0; i < cf.nPasos; i++)
        {
          Serial.printf("M8AX - %d) %d %c %d = %d\n",
                        i + 1,
                        cf.pasos[i].a,
                        cf.pasos[i].op,
                        cf.pasos[i].b,
                        cf.pasos[i].r);
        }
      }
      Serial.printf("M8AX - RESULTADO %d (%s)\n",
                    cf.valor,
                    cf.exacto ? "EXACTO" : "DIF");
      cf.nextAt = millis() + CIF_PAUSA_MS;
    }

    // ---- Hora y hashrate cada segundo ----
    if (now - lastMining >= 1000)
    {
      lastMining = now;
      mineria = getMiningData(mElapsed);
      snprintf(hashStr, sizeof(hashStr), "%s", mineria.currentHashRate.c_str());
      time_t ep = (time_t)timeClient.getEpochTime();
      struct tm *ti = localtime(&ep);
      snprintf(horaStr, sizeof(horaStr), "%02d:%02d:%02d", ti->tm_hour, ti->tm_min, ti->tm_sec);
      cifDrawTop(horaStr, hashStr);
      unsigned long dtW = now - cifWinT;
      cifRate = cifWinN * 1000.0f / (float)dtW;
      Serial.printf("M8AX - Cifras: %.1f Obj/s Reales | %.1f Obj/s Solo Busqueda\n",
                    cifRate,
                    cifSolveUs ? (float)cifSolveN * 1000000.0f / (float)cifSolveUs : 0.0f);
      cifWinN = 0;
      cifSolveN = 0;
      cifSolveUs = 0;
      cifWinT = now;
      cifDrawRate();
    }
    webAtender();
    delay(10);
  }

  pongLeds(false);
  tft.setTextPadding(0);
  tft.setTextDatum(TL_DATUM);

  cuentita++;
  if (cuentita == 15)
  {
    ajustarZonaHoraria();
    obtenerLocYTemp();
    obtenerVariacionesBTC(variacion1dia, variacion1semana);
  }
}

/********************************************************************************************
 *  PANTALLA LLUVIA MATRIX (HEX) - Columnas de digitos hexadecimales cayendo como en Matrix
 ********************************************************************************************/
#define RAIN_COLS 40       // 320 / 8
#define RAIN_ROWS 19       // 152 / 8 ( abajo quedan 18 px para los textos )
#define RAIN_CW 8          // ancho de celda en pixeles
#define RAIN_CH 8          // alto de celda en pixeles
#define RAIN_SLICE_MS 5000 // tandas largas, como en Life / Pong
#define RAIN_MIN_LEN 8     // longitud minima de la estela ( no bajar de 8 )
#define RAIN_MAX_LEN 17    // longitud maxima de la estela ( no pasar de 19 )
#define RAIN_STEP_MIN 70   // ms por paso de la gota mas rapida
#define RAIN_STEP_VAR 150  // hasta cuantos ms mas lenta puede ser ( mas numero = mas calma )

struct RainCol
{
  bool active;
  int16_t head;    // fila de la cabeza ( -1 = todavia no ha entrado, puede pasarse del final )
  uint8_t len;     // longitud de la estela
  uint16_t stepMs; // ms entre pasos ( velocidad de esta gota )
  uint32_t nextAt; // cuando toca el siguiente paso
};

static RainCol rainCols[RAIN_COLS];
static char rainCells[RAIN_ROWS][RAIN_COLS];
static bool rainInit = false;
static const char RAIN_HEX[] = "0123456789ABCDEF";
static const uint16_t RAIN_COLORS[] = {0xFFFF, 0xAFE5, 0x07E0, 0x03E0, 0x0200}; // blanco, lima, verde, verde medio, verde oscuro

static uint16_t rainColor(int off)
{
  if (off <= 0)
    return RAIN_COLORS[0];
  if (off == 1)
    return RAIN_COLORS[1];
  if (off <= 3)
    return RAIN_COLORS[2];
  if (off <= 6)
    return RAIN_COLORS[3];
  return RAIN_COLORS[4];
}

static void rainDrawCell(int row, int col, uint16_t color)
{
  if (row < 0 || row >= RAIN_ROWS)
    return;
  char s[2] = {rainCells[row][col], 0};
  tft.setTextColor(color, TFT_BLACK);
  tft.drawString(s, col * RAIN_CW + 1, row * RAIN_CH);
}

static void rainEraseCell(int row, int col)
{
  if (row < 0 || row >= RAIN_ROWS)
    return;
  tft.fillRect(col * RAIN_CW, row * RAIN_CH, RAIN_CW, RAIN_CH, TFT_BLACK);
}

static void rainSpawn(int c, uint32_t now)
{
  RainCol &d = rainCols[c];
  d.active = true;
  d.head = -1;
  d.len = RAIN_MIN_LEN + esp_random() % (RAIN_MAX_LEN - RAIN_MIN_LEN + 1);
  d.stepMs = RAIN_STEP_MIN + esp_random() % RAIN_STEP_VAR;
  d.nextAt = now;
}

// Un paso de una gota: solo se repintan las celdas que cambian de color ( 5 ) y la que se borra ( 1 )
static void rainStep(int c)
{
  RainCol &d = rainCols[c];
  d.head++;
  int h = d.head;
  if (h >= 0 && h < RAIN_ROWS)
    rainCells[h][c] = RAIN_HEX[esp_random() & 15];

  tft.setFreeFont(NULL);
  tft.setTextFont(1);
  tft.setTextSize(1);
  tft.setTextDatum(TL_DATUM);
  tft.setTextPadding(0);

  static const int offs[] = {0, 1, 2, 4, 7};
  for (int k = 0; k < 5; k++)
  {
    int off = offs[k];
    if (off >= d.len)
      break;
    int row = h - off;
    if (row < 0 || row >= RAIN_ROWS)
      continue;
    if (off == 1 && (esp_random() % 5) == 0)
      rainCells[row][c] = RAIN_HEX[esp_random() & 15]; // pequeño centelleo
    rainDrawCell(row, c, rainColor(off));
  }
  rainEraseCell(h - d.len, c);
}

void esp32_2432S028R_RainScreen(unsigned long mElapsed)
{
  mineria = getMiningData(mElapsed);
  Serial.printf("M8AX - %s >>> Completados %s Share(s), %s Khashes, Prom. Hashrate %s KH/s %s°\n",
                mineria.currentTime.c_str(), mineria.completedShares.c_str(), mineria.totalKHashes.c_str(), mineria.currentHashRate.c_str(), mineria.temp.c_str());

  bool redraw = hasChangedScreen;
  if (!rainInit)
  {
    rainInit = true;
    redraw = true;
  }

  if (redraw)
  {
    tft.fillRect(0, 0, 320, 170, TFT_BLACK);
    memset(rainCells, ' ', sizeof(rainCells));
    unsigned long t0 = millis();
    for (int c = 0; c < RAIN_COLS; c++)
    {
      rainCols[c].active = false;
      rainCols[c].nextAt = t0 + esp_random() % 3000; // entran escalonadas, la lluvia "se va llenando"
    }
  }
  printPoolData(); // pie de las criptos (solo se repinta cuando toca)
  hasChangedScreen = false;

  char hashStr[16];
  snprintf(hashStr, sizeof(hashStr), "%s", mineria.currentHashRate.c_str());
  char izq[24], der[24];
  char lastIzq[24] = "", lastDer[24] = "";

  unsigned long start = millis();
  unsigned long lastHud = 0;
  unsigned long lastMiningUpdate = 0;
  unsigned long lastTouchChk = 0;
  pongLeds(false); // LEDs apagados para dormir
  unsigned long ledApagarEn = 0;
  unsigned long ledUlt = 0;

  while (millis() - start < RAIN_SLICE_MS)
  {
    unsigned long now = millis();

    // ---- Tactil (misma logica que esp32_2432S028R_DoLedStuff) ----
    if (now - lastTouchChk >= 40 && now - previousTouchMillis >= 500)
    {
      lastTouchChk = now;
      int16_t t_x, t_y;
      if (toqueXY(t_x, t_y))
      {
        previousTouchMillis = now;
        if (((t_x > 109) && (t_x < 211)) && ((t_y > 185) && (t_y < 241)))
        {
          bottomScreenBlue ^= true;
          hasChangedScreen = true;
        }
        else if ((t_x > 235) && ((t_y > 0) && (t_y < 16)))
        {
          esp32_2432S028R_AlternateScreenState();
        }
        else if (t_x > 160)
        {
          currentDisplayDriver->current_cyclic_screen = (currentDisplayDriver->current_cyclic_screen + 1) % currentDisplayDriver->num_cyclic_screens;
        }
        else
        {
          currentDisplayDriver->current_cyclic_screen = currentDisplayDriver->current_cyclic_screen - 1;
          if (currentDisplayDriver->current_cyclic_screen < 0)
            currentDisplayDriver->current_cyclic_screen = currentDisplayDriver->num_cyclic_screens - 1;
        }
        break;
      }
    }

    if (ledApagarEn && now >= ledApagarEn)
    {
      pongLeds(false);
      ledApagarEn = 0;
    }

    // ---- Lluvia: cada columna avanza a su ritmo ----
    for (int c = 0; c < RAIN_COLS; c++)
    {
      RainCol &d = rainCols[c];
      if (now < d.nextAt)
        continue;
      if (!d.active)
      {
        rainSpawn(c, now); // el primer paso se da en la vuelta siguiente
        continue;
      }
      rainStep(c);
      if (now - ledUlt >= 80)
      {
        pongLeds(false);
        digitalWrite(LED_PIN_G, LOW);
        ledApagarEn = now + 15;
        ledUlt = now;
      }
      d.nextAt = now + d.stepMs;
      if (d.head - d.len >= RAIN_ROWS) // la estela ya ha salido entera por abajo
      {
        d.active = false;
        d.nextAt = now + 300 + esp_random() % 3000; // descansa un rato antes de volver a caer
      }
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
      snprintf(izq, sizeof(izq), "LLUVIA M.HEX");
      snprintf(der, sizeof(der), "%s KH/s", hashStr);
      if (strcmp(izq, lastIzq) != 0 || strcmp(der, lastDer) != 0)
      {
        lifeDrawHud(izq, der);
        strcpy(lastIzq, izq);
        strcpy(lastDer, der);
      }
    }
    webAtender();
    delay(10);
  }

  pongLeds(false); // por si salimos con los LEDs encendidos
  tft.setTextPadding(0);
  tft.setTextDatum(TL_DATUM);

  cuentita++;
  if (cuentita == 15)
  {
    ajustarZonaHoraria();
    obtenerLocYTemp();
    obtenerVariacionesBTC(variacion1dia, variacion1semana);
  }
}

/****************************************************************************************************************
 *
 *   Escrito por: M8AX
 *
 *   Descripción: TETRIS CPU - Pantalla para ESP32_2432S028R
 *   -------------------------------------------------------
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
 *   Para activarla hay que pegar este bloque antes del array esp32_2432S028RCyclicScreens[] (encima de
 *   analiCadaSegundo vale) y meter esp32_2432S028R_TetrisScreen en ese array.
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
    ledFlash(true, false, false, 15);
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
    ledFlash(true, true, true, 250);
    ttLimpia(b);
    ttDibujaCampo(b, false);
  }
  else
  {
    ttDibujaPieza(tt.pieza + 1);
    ledFlash(false, true, false, 40);
  }
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

// Tandas de 5 s como el Pong, el tactil se atiende dentro
#define TT_SLICE_MS 5000

void esp32_2432S028R_TetrisScreen(unsigned long mElapsed)
{
  mineria = getMiningData(mElapsed);
  Serial.printf("M8AX - %s >>> Completados %s Share(s), %s Khashes, Prom. Hashrate %s KH/s %s°\n",
                mineria.currentTime.c_str(), mineria.completedShares.c_str(), mineria.totalKHashes.c_str(), mineria.currentHashRate.c_str(), mineria.temp.c_str());

  uint8_t b[TT_ROWS][TT_COLS]; // el campo vive aqui, en la pila, solo mientras dura la tanda
  if (!tt.init)
  {
    tt.init = 1;
    tt.record = 0;
    tt.partidas = 0;
    ttNueva(b);
    hasChangedScreen = true;
  }
  else
    ttCarga(b);
  if (hasChangedScreen)
    tft.fillRect(0, 0, 320, 170, TFT_BLACK);
  ttPintaTodo(b);
  printPoolData(); // barra de abajo, solo se repinta cuando toca
  hasChangedScreen = false;

  char horaStr[9] = "";
  char lastHora[9] = "";
  char hashStr[16];
  char lastHash[16] = "";

  unsigned long start = millis();
  unsigned long lastMiningUpdate = 0;
  unsigned long lastTouchChk = 0;
  unsigned long siguiente = start + ttPasoMs();

  while (millis() - start < TT_SLICE_MS)
  {
    unsigned long now = millis();

    // ---- Táctil (misma lógica que el Pong) ----
    if (now - lastTouchChk >= 40 && now - previousTouchMillis >= 500)
    {
      lastTouchChk = now;
      int16_t t_x, t_y;
      if (toqueXY(t_x, t_y))
      {
        previousTouchMillis = now;
        if (((t_x > 109) && (t_x < 211)) && ((t_y > 185) && (t_y < 241)))
        {
          bottomScreenBlue ^= true;
          hasChangedScreen = true;
        }
        else if ((t_x > 235) && ((t_y > 0) && (t_y < 16)))
        {
          esp32_2432S028R_AlternateScreenState();
        }
        else if (t_x > 160)
        {
          currentDisplayDriver->current_cyclic_screen = (currentDisplayDriver->current_cyclic_screen + 1) % currentDisplayDriver->num_cyclic_screens;
        }
        else
        {
          currentDisplayDriver->current_cyclic_screen = currentDisplayDriver->current_cyclic_screen - 1;
          if (currentDisplayDriver->current_cyclic_screen < 0)
            currentDisplayDriver->current_cyclic_screen = currentDisplayDriver->num_cyclic_screens - 1;
        }
        break;
      }
    }

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

    // ---- Hora (izquierda) y hashrate (derecha), arriba como en las demás ----
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
        pongDrawHud(hashStr, horaStr);
        strcpy(lastHora, horaStr);
        strcpy(lastHash, hashStr);
      }
    }
    webAtender();
    delay(5);
  }

  ttGuarda(b); // la partida se guarda comprimida y el campo de trabajo desaparece con la pila
  tft.setTextPadding(0);
  tft.setTextDatum(TL_DATUM);

  cuentita++;
  if (cuentita == 15)
  {
    ajustarZonaHoraria();
    obtenerLocYTemp();
    obtenerVariacionesBTC(variacion1dia, variacion1semana);
  }
}

/****************************************************************************************************************
 *
 *   Escrito por: M8AX
 *
 *   Descripción: PRIMOS A SACO - Pantalla para NerdminerV2
 *   ------------------------------------------------------
 *
 *   Un número enorme en el centro y debajo de él los primos anteriores, uno encima de otro y cada vez más
 *   apagados. Salen todos los primos seguidos (2 3 5 7 11 13...), sin saltarse ninguno, hasta llegar al
 *   mayor primo que cabe en 64 bits, 18446744073709551557 (2^64 - 59). Ahí se para y se queda mostrándolo.
 *
 *   El número grande es de 7 segmentos y se ajusta solo al ancho: con una cifra ocupa media pantalla y con
 *   las 20 cifras de un uint64 llena la pantalla entera de lado a lado, en una sola línea. Los primos no
 *   se sacan de una tabla, se calculan de verdad: es Miller-Rabin con las 12 primeras bases, que es exacto
 *   para cualquier número de 64 bits (la multiplicación grande se hace sumando y duplicando, así no hace
 *   falta nada de 128 bits). Con números pequeños el cálculo es casi instantáneo y lo que limita es el
 *   dibujo; por encima de 2^32 cada primo cuesta bastante más.
 *
 *   Arriba a la izquierda la hora y arriba a la derecha el hashrate, como en las demás. Abajo una barra con
 *   cuántos de los 64 bits lleva usados el número y el 2^64-1 de referencia.
 *
 *   Memoria: no pide NADA al heap. Guarda solo unos 100 bytes de estado entre tandas, el resto vive en la
 *   pila mientras corre la pantalla.
 *
 *   Para activarla hay que pegar este bloque antes del array tDisplayCyclicScreens[] y meter tDisplay_Primos
 *   en ese array. El táctil funciona como en el Pong: los lados cambian de pantalla, la zona de arriba a la
 *   derecha alterna el estado de la pantalla y el centro de abajo cambia la barra. Como la pantalla puede
 *   cambiar en cualquier momento, el estado se guarda y sigue donde se quedó cuando vuelve a salir.
 *
 ****************************************************************************************************************/

// ---------------------------------------------------------------------------------------------------------
//  PRIMOS
// ---------------------------------------------------------------------------------------------------------

#define PM_MAXPRIMO 18446744073709551557ULL // el primo mas grande que cabe en un uint64 (2^64 - 59)
#define PM_PASO_SEC_MS 0                    // ms entre primos mientras salen seguidos
#define PM_BANDA_Y 76                       // la banda del numero grande
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
  uint8_t init, fase; // fase: 0 contando, 2 mayor primo (parado)
  uint64_t actual;
  uint64_t hist[4]; // los 4 anteriores, el mas reciente primero
  uint8_t nHist;
  uint32_t k; // numero de orden mientras salen seguidos
  unsigned long ultLlamada;
  char vista[21]; // lo que hay dibujado en la banda (para repintar solo las cifras que cambian)
};
static PmEstado pm = {0};
// Medicion de primos calculados por segundo
static uint32_t pmCuentaVelocidad = 0;
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

// Siguiente primo de la lista: todos los consecutivos, sin saltarse ninguno, hasta el mayor de 64 bits
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

static inline unsigned long pmPasoMs() { return PM_PASO_SEC_MS; }

// Un paso de la animacion
static void pmPaso()
{
  pmAvanza();
  pmPintaPila();
  pmPintaNumero(false);
  pmPintaInfo();
  ledFlash(true, false, false, LED_FLASH_MS);
}

// Tandas de 1 s para que el cambio de pantalla (táctil o automático) responda enseguida
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
  Serial.printf("M8AX - %s >>> Completados %s Share(s), %s Khashes, Prom. Hashrate %s KH/s %s°\n",
                mineria.currentTime.c_str(), mineria.completedShares.c_str(), mineria.totalKHashes.c_str(), mineria.currentHashRate.c_str(), mineria.temp.c_str());

  // Si hace rato que no nos llaman es que se ha visto otra pantalla: borrar antes de pintar
  bool limpiar = !pm.init || (millis() - pm.ultLlamada > 2500) || hasChangedScreen;
  if (!pm.init)
  {
    pm.init = 1;
    pmReinicia();
  }
  if (limpiar)
  {
    tft.fillRect(0, 0, 320, 170, TFT_BLACK);
    pmCuentaVelocidad = 0;
    pmTiempoVelocidad = millis();
  }
  pmPintaTodo();
  printPoolData(); // barra de abajo, solo se repinta cuando toca
  hasChangedScreen = false;

  char horaStr[9] = "";
  char lastHora[9] = "";
  char hashStr[16];
  char lastHash[16] = "";

  unsigned long start = millis();
  unsigned long lastMiningUpdate = 0;
  unsigned long lastTouchChk = 0;
  unsigned long siguiente = start + pmPasoMs();

  // Inicializar el cronometro solamente la primera vez
  if (pmTiempoVelocidad == 0)
    pmTiempoVelocidad = start;

  while (millis() - start < PM_SLICE_MS)
  {
    unsigned long now = millis();

    // ---- Táctil (misma lógica que el Pong) ----
    if (now - lastTouchChk >= 40 && now - previousTouchMillis >= 500)
    {
      lastTouchChk = now;
      int16_t t_x, t_y;
      if (toqueXY(t_x, t_y))
      {
        previousTouchMillis = now;
        if (((t_x > 109) && (t_x < 211)) && ((t_y > 185) && (t_y < 241)))
        {
          bottomScreenBlue ^= true;
          hasChangedScreen = true;
        }
        else if ((t_x > 235) && ((t_y > 0) && (t_y < 16)))
        {
          esp32_2432S028R_AlternateScreenState();
        }
        else if (t_x > 160)
        {
          currentDisplayDriver->current_cyclic_screen = (currentDisplayDriver->current_cyclic_screen + 1) % currentDisplayDriver->num_cyclic_screens;
        }
        else
        {
          currentDisplayDriver->current_cyclic_screen = currentDisplayDriver->current_cyclic_screen - 1;
          if (currentDisplayDriver->current_cyclic_screen < 0)
            currentDisplayDriver->current_cyclic_screen = currentDisplayDriver->num_cyclic_screens - 1;
        }
        break;
      }
    }

    // Calcular primos encontrados por segundo
    if (now - pmTiempoVelocidad >= 1000)
    {
      pmVelocidadMostrada = pmCuentaVelocidad;
      pmCuentaVelocidad = 0;
      pmTiempoVelocidad = now;
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
    webAtender();
    delay(5);
  }

  tft.setTextPadding(0);
  tft.setTextDatum(TL_DATUM);

  cuentita++;
  if (cuentita == 15)
  {
    ajustarZonaHoraria();
    obtenerLocYTemp();
    obtenerVariacionesBTC(variacion1dia, variacion1semana);
  }

  pm.ultLlamada = millis(); // al final, para que lo que tarde la red no cuente como "hace rato que no nos llaman"
}

/****************************************************************************************************************
 *
 *   Escrito por: M8AX
 *
 *   Descripción: AJEDREZ CPU VS CPU - Pantalla para ESP32_2432S028R
 *   ---------------------------------------------------------------
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
 *   Para activarla solo hay que meter esp32_2432S028R_AjedrezScreen en el array
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
#define CH_SLICE_MS 5000  // tandas de 5 s como el Pong, el táctil se atiende dentro
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
  unsigned long siguiente, finAt;
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

static void chPintaTodo()
{
  tft.fillRect(0, 0, 320, 170, TFT_BLACK);
  tft.drawRect(CH_BX - 1, CH_BY - 1, CH_SQ * 8 + 2, CH_SQ * 8 + 2, CH_GRIS);
  memset(ch.shown, 0xFF, sizeof(ch.shown));
  chPintaTablero(true);
  chPintaPanel(1);
  chPintaPanel(2);
  chPintaMarcador();
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
  const bool juegaBlanca = (g.side == 1);
  const bool hayCaptura = (g.sq[m.to] != 0) || (m.flag & CHF_CAP) || (m.flag & CHF_EP);

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
  if (hayCaptura)
    ledFlash(true, true, true, 120);
  else if (juegaBlanca)
    ledFlash(false, true, false, 60);
  else
    ledFlash(true, false, false, 60);
  ch.siguiente = millis() + CH_PAUSA_MS;
}

void esp32_2432S028R_AjedrezScreen(unsigned long mElapsed)
{
  mineria = getMiningData(mElapsed);
  Serial.printf("M8AX - %s >>> Completados %s Share(s), %s Khashes, Prom. Hashrate %s KH/s %s°\n",
                mineria.currentTime.c_str(), mineria.completedShares.c_str(), mineria.totalKHashes.c_str(), mineria.currentHashRate.c_str(), mineria.temp.c_str());

  if (!chMemAlloc()) // sin RAM libre no se juega (y no se rompe nada de las otras pantallas)
  {
    Serial.println("M8AX - ### Ajedrez: Sin RAM Para El Motor ###");
    printheap();
    delay(1000);
    return;
  }
  const int chMiPantalla = currentDisplayDriver->current_cyclic_screen;

  bool redraw = hasChangedScreen;
  if (!ch.init)
  {
    ch.init = true;
    chNuevaPartida(true);
    redraw = true;
  }
  if (redraw)
    chPintaTodo();
  printPoolData(); // barra de abajo, solo se repinta cuando toca
  hasChangedScreen = false;

  char horaStr[9] = "";
  char lastHora[9] = "";
  char hashStr[16];
  char lastHash[16] = "";

  unsigned long start = millis();
  unsigned long lastMiningUpdate = 0;
  unsigned long lastTouchChk = 0;

  while (millis() - start < CH_SLICE_MS)
  {
    unsigned long now = millis();

    // ---- Táctil (misma lógica que el Pong) ----
    if (now - lastTouchChk >= 40 && now - previousTouchMillis >= 500)
    {
      lastTouchChk = now;
      int16_t t_x, t_y;
      if (toqueXY(t_x, t_y))
      {
        previousTouchMillis = now;
        if (((t_x > 109) && (t_x < 211)) && ((t_y > 185) && (t_y < 241)))
        {
          bottomScreenBlue ^= true;
          hasChangedScreen = true;
        }
        else if ((t_x > 235) && ((t_y > 0) && (t_y < 16)))
        {
          esp32_2432S028R_AlternateScreenState();
        }
        else if (t_x > 160)
        {
          currentDisplayDriver->current_cyclic_screen = (currentDisplayDriver->current_cyclic_screen + 1) % currentDisplayDriver->num_cyclic_screens;
        }
        else
        {
          currentDisplayDriver->current_cyclic_screen = currentDisplayDriver->current_cyclic_screen - 1;
          if (currentDisplayDriver->current_cyclic_screen < 0)
            currentDisplayDriver->current_cyclic_screen = currentDisplayDriver->num_cyclic_screens - 1;
        }
        break;
      }
    }

    // ---- Partida ----
    if (ch.estado == 0)
    {
      if (now >= ch.siguiente)
      {
        ChMove m;
        if (chElegir(ch.g, m, CH_PROF, CH_TIEMPO_MS, ch.hist, ch.nHist))
          chJuega(m);
        else
          chFin(2, 0); // no debería pasar, pero así no se cuelga
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
      chPintaTodo();
    }

    // ---- Hora (izquierda) y hashrate (derecha), arriba como en las demás ----
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
        pongDrawHud(hashStr, horaStr);
        strcpy(lastHora, horaStr);
        strcpy(lastHash, hashStr);
      }
    }
    webAtender();
    delay(10);
  }

  tft.setTextPadding(0);
  tft.setTextDatum(TL_DATUM);

  // Si nos vamos a otra pantalla (con el tactil) se devuelve toda la RAM del ajedrez

  if (currentDisplayDriver->current_cyclic_screen != chMiPantalla)
    chMemLibera();

  cuentita++;
  if (cuentita == 15)
  {
    ajustarZonaHoraria();
    obtenerLocYTemp();
    obtenerVariacionesBTC(variacion1dia, variacion1semana);
  }
}

/****************************************************************************************************************
 *
 *   Escrito por: M8AX
 *
 *   Descripción: SNAKE CPU - Pantalla para ESP32_2432S028R
 *   ------------------------------------------------------
 *
 *   La CPU juega sola a la serpiente en un campo de 40x15 casillas. Cada vez que tiene que moverse busca el
 *   camino más corto hasta la comida, pero antes de seguirlo comprueba, imaginando que ya se la ha comido,
 *   que la cabeza todavía podría llegar hasta su propia cola. Si es así, va a por ella; si no, se queda
 *   dando vueltas por el sitio libre, alejándose de la comida, hasta que se abre un camino seguro. Gracias
 *   a eso aguanta mucho más de lo que parece y suele pasar de 100 casillas sin encerrarse.
 *
 *   Para que no juegue perfecta eternamente, de vez en cuando (más a menudo cuanto más larga es) se mueve
 *   a una casilla libre cualquiera sin mirar si es segura, y tarde o temprano se encierra sola, sale GAME
 *   OVER y empieza otra partida xD... También se rinde si pasa demasiado tiempo sin comer, por si se pone
 *   a dar vueltas sin fin.
 *
 *   Arriba a la izquierda la hora y arriba a la derecha el hashrate, como en las demás. Abajo a la
 *   izquierda la longitud y el récord, y a la derecha las partidas jugadas. Cada comida hace un destello
 *   verde en el LED y cada GAME OVER uno rojo.
 *
 *   Memoria: al arrancar el minero no ocupa NADA. Las tablas (unos 6 KB: la serpiente, el mapa y las de la
 *   búsqueda de caminos) se piden con malloc al entrar en la pantalla y se liberan al salir a otra, como en
 *   el ajedrez. Solo quedan fijos el récord y las partidas (8 bytes). Al volver a la pantalla empieza una
 *   partida nueva. Si no hubiera RAM para las tablas, se salta la pantalla sin pintar nada.
 *
 *   Para activarla hay que pegar este bloque antes de analiCadaSegundo y meter esp32_2432S028R_SnakeScreen
 *   en el array esp32_2432S028RCyclicScreens[].
 *
 ****************************************************************************************************************/

#define SNK_C 8 // lado de cada casilla
#define SNK_COLS 40
#define SNK_ROWS 15
#define SNK_N (SNK_COLS * SNK_ROWS)
#define SNK_FX 0
#define SNK_FY 29
#define SNK_LARGO0 3
#define SNK_PASO_MS 40
#define SNK_FIN_MS 3000    // lo que se queda el GAME OVER antes de empezar otra partida
#define SNK_SLICE_MS 5000  // tandas largas, como en las demás
#define SNK_ERR 20         // fallos por mil en cada paso (se le suma largo / 2)
#define SNK_SIN_COMER 1200 // pasos seguidos sin comer antes de rendirse

struct SnkMem
{
  uint8_t estado; // 0 jugando, 1 partida terminada
  bool gana;
  uint16_t cuerpo[SNK_N]; // anillo: de la cola a la cabeza
  uint16_t cola;
  uint16_t largo;
  uint16_t comida;
  uint16_t sinComer;
  uint8_t ocupa[SNK_N];
  unsigned long finAt;
  uint16_t fila[SNK_N];
  uint16_t dist[SNK_N];
  uint16_t camino[SNK_N];
  uint8_t virt[SNK_N];
};
static SnkMem *snkM = NULL;
static uint32_t snkRecord = 0;
static uint32_t snkPartidas = 0;

static inline int snkElem(int i)
{
  return snkM->cuerpo[(snkM->cola + i) % SNK_N];
}

static int snkVecino(int c, int d)
{
  int x = c % SNK_COLS, y = c / SNK_COLS;
  switch (d)
  {
  case 0:
    y--;
    break;
  case 1:
    x++;
    break;
  case 2:
    y++;
    break;
  default:
    x--;
    break;
  }
  if (x < 0 || x >= SNK_COLS || y < 0 || y >= SNK_ROWS)
    return -1;
  return y * SNK_COLS + x;
}

// Distancias desde una casilla sin cruzar las bloqueadas. Las que no se alcanzan quedan en 0xFFFF
static void snkBfs(const uint8_t *bloq, int desde)
{
  for (int i = 0; i < SNK_N; i++)
    snkM->dist[i] = 0xFFFF;
  int ini = 0, fin = 0;
  snkM->fila[fin++] = desde;
  snkM->dist[desde] = 0;
  while (ini < fin)
  {
    int c = snkM->fila[ini++];
    for (int d = 0; d < 4; d++)
    {
      int n = snkVecino(c, d);
      if (n < 0 || snkM->dist[n] != 0xFFFF || bloq[n])
        continue;
      snkM->dist[n] = snkM->dist[c] + 1;
      snkM->fila[fin++] = n;
    }
  }
}

// Reconstruye en snkM->camino el camino hasta una casilla, tras un snkBfs. Devuelve los pasos, -1 si no llega
static int snkCaminoA(int hasta)
{
  if (snkM->dist[hasta] == 0xFFFF)
    return -1;
  int L = snkM->dist[hasta];
  int c = hasta;
  for (int k = L - 1; k >= 0; k--)
  {
    snkM->camino[k] = c;
    for (int d = 0; d < 4; d++)
    {
      int n = snkVecino(c, d);
      if (n >= 0 && snkM->dist[n] == k)
      {
        c = n;
        break;
      }
    }
  }
  return L;
}

// Tras seguir el camino de L pasos y comerse la comida, ¿la cabeza todavía llega a la cola?
static bool snkSegura(int L)
{
  int largo = snkM->largo;
  memset(snkM->virt, 0, sizeof(snkM->virt));
  for (int i = L - 1; i < largo + L; i++)
    snkM->virt[i < largo ? snkElem(i) : snkM->camino[i - largo]] = 1;
  int cabeza = snkM->camino[L - 1];
  int cola = (L - 1 < largo) ? snkElem(L - 1) : snkM->camino[L - 1 - largo];
  snkM->virt[cola] = 0;
  snkBfs(snkM->virt, cabeza);
  return snkM->dist[cola] != 0xFFFF;
}

// Lo mismo para un solo paso sin comer
static bool snkSalidaLibre(int n)
{
  if (snkM->largo < 2)
    return true;
  memset(snkM->virt, 0, sizeof(snkM->virt));
  for (int i = 1; i < snkM->largo; i++)
    snkM->virt[snkElem(i)] = 1;
  snkM->virt[n] = 1;
  int cola = snkElem(1);
  snkM->virt[cola] = 0;
  snkBfs(snkM->virt, n);
  return snkM->dist[cola] != 0xFFFF;
}

// Casilla a la que va la cabeza. -1 = no tiene a dónde ir
static int snkElige()
{
  int cabeza = snkElem(snkM->largo - 1);
  int cola = snkElem(0);
  int cand[4], nc = 0;
  for (int d = 0; d < 4; d++)
  {
    int n = snkVecino(cabeza, d);
    if (n >= 0 && (!snkM->ocupa[n] || n == cola))
      cand[nc++] = n;
  }
  if (!nc)
    return -1;

  if ((int)(esp_random() % 1000) < SNK_ERR + snkM->largo / 2)
    return cand[esp_random() % nc];

  memcpy(snkM->virt, snkM->ocupa, sizeof(snkM->virt));
  snkBfs(snkM->virt, cabeza);
  int L = snkCaminoA(snkM->comida);
  if (L > 0 && snkSegura(L))
    return snkM->camino[0];

  int mejor = -1, mejorD = -1;
  for (int i = 0; i < nc; i++)
  {
    if (!snkSalidaLibre(cand[i]))
      continue;
    int d = abs(cand[i] % SNK_COLS - snkM->comida % SNK_COLS) + abs(cand[i] / SNK_COLS - snkM->comida / SNK_COLS);
    if (d > mejorD)
    {
      mejorD = d;
      mejor = cand[i];
    }
  }
  return mejor >= 0 ? mejor : cand[0];
}

static void snkCelda(int c, uint16_t col)
{
  tft.fillRect(SNK_FX + (c % SNK_COLS) * SNK_C, SNK_FY + (c / SNK_COLS) * SNK_C, SNK_C - 1, SNK_C - 1, col);
}

static void snkFoot()
{
  char izq[24], der[16];
  uint32_t rec = snkRecord > snkM->largo ? snkRecord : snkM->largo;
  snprintf(izq, sizeof(izq), "LON %u REC %lu", snkM->largo, (unsigned long)rec);
  snprintf(der, sizeof(der), "PART %lu", (unsigned long)snkPartidas);
  tft.setFreeFont(NULL);
  tft.setTextFont(1);
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextPadding(190);
  tft.setTextDatum(TL_DATUM);
  tft.drawString(izq, 4, 153);
  tft.setTextPadding(120);
  tft.setTextDatum(TR_DATUM);
  tft.drawString(der, 316, 153);
  tft.setTextPadding(0);
  tft.setTextDatum(TL_DATUM);
}

static void snkTextoFin()
{
  tft.setFreeFont(NULL);
  tft.setTextFont(1);
  tft.setTextSize(3);
  tft.setTextPadding(0);
  tft.setTextDatum(MC_DATUM);
  tft.setTextColor(snkM->gana ? TFT_GREEN : TFT_RED, TFT_BLACK);
  tft.drawString(snkM->gana ? "LLENO!" : "GAME OVER", 160, SNK_FY + SNK_ROWS * SNK_C / 2);
  tft.setTextDatum(TL_DATUM);
}

static void snkPintaTodo()
{
  tft.fillRect(0, 26, 320, 126, TFT_BLACK);
  tft.drawFastHLine(0, 27, 320, TFT_DARKGREY);
  tft.drawFastHLine(0, SNK_FY + SNK_ROWS * SNK_C, 320, TFT_DARKGREY);
  for (int i = 0; i < snkM->largo - 1; i++)
    snkCelda(snkElem(i), TFT_GREEN);
  snkCelda(snkElem(snkM->largo - 1), TFT_WHITE);
  snkCelda(snkM->comida, TFT_RED);
  if (snkM->estado)
    snkTextoFin();
  snkFoot();
}

static void snkNuevaComida()
{
  int k = esp_random() % (SNK_N - snkM->largo);
  for (int c = 0; c < SNK_N; c++)
  {
    if (!snkM->ocupa[c] && k-- == 0)
    {
      snkM->comida = c;
      return;
    }
  }
}

static void snkNueva()
{
  memset(snkM->ocupa, 0, sizeof(snkM->ocupa));
  snkM->largo = SNK_LARGO0;
  snkM->cola = 0;
  for (int i = 0; i < snkM->largo; i++)
  {
    int c = (SNK_ROWS / 2) * SNK_COLS + 10 + i;
    snkM->cuerpo[i] = c;
    snkM->ocupa[c] = 1;
  }
  snkM->estado = 0;
  snkM->gana = false;
  snkM->sinComer = 0;
  snkNuevaComida();
}

static void snkFin(bool gana)
{
  snkM->estado = 1;
  snkM->gana = gana;
  snkM->finAt = millis();
  if (snkM->largo > snkRecord)
    snkRecord = snkM->largo;
  snkPartidas++;
  snkTextoFin();
  snkFoot();
  Serial.printf("M8AX - Snake: Partida %lu | Longitud %u | Record %lu\n",
                (unsigned long)snkPartidas, snkM->largo, (unsigned long)snkRecord);
  ledFlash(!gana, gana, false, 150);
}

static void snkPaso()
{
  int n = snkElige();
  if (n < 0)
  {
    snkFin(false);
    return;
  }
  int cabeza = snkElem(snkM->largo - 1);
  bool come = (n == snkM->comida);
  int pos = (snkM->cola + snkM->largo) % SNK_N;
  int libre = -1;
  if (come)
    snkM->largo++;
  else
  {
    libre = snkM->cuerpo[snkM->cola];
    snkM->ocupa[libre] = 0;
    snkM->cola = (snkM->cola + 1) % SNK_N;
  }
  snkM->cuerpo[pos] = n;
  snkM->ocupa[n] = 1;

  snkCelda(cabeza, TFT_GREEN);
  if (libre >= 0 && libre != n)
    snkCelda(libre, TFT_BLACK);
  snkCelda(n, TFT_WHITE);

  if (come)
  {
    snkM->sinComer = 0;
    if (snkM->largo >= SNK_N - 1)
    {
      snkFin(true);
      return;
    }
    snkNuevaComida();
    snkCelda(snkM->comida, TFT_RED);
    snkFoot();
    ledFlash(false, true, false, LED_FLASH_MS);
  }
  else if (++snkM->sinComer > SNK_SIN_COMER)
    snkFin(false);
}

void esp32_2432S028R_SnakeScreen(unsigned long mElapsed)
{
  mineria = getMiningData(mElapsed);
  Serial.printf("M8AX - %s >>> Completados %s Share(s), %s Khashes, Prom. Hashrate %s KH/s %s°\n",
                mineria.currentTime.c_str(), mineria.completedShares.c_str(), mineria.totalKHashes.c_str(), mineria.currentHashRate.c_str(), mineria.temp.c_str());

  bool redraw = hasChangedScreen;
  if (!snkM)
  {
    snkM = (SnkMem *)malloc(sizeof(SnkMem));
    if (!snkM)
    {
      Serial.println("M8AX - Snake: No Hay RAM Libre");
      delay(1000);
      return;
    }
    memset(snkM, 0, sizeof(SnkMem));
    snkNueva();
    redraw = true;
  }
  const int snkMiPantalla = currentDisplayDriver->current_cyclic_screen;
  if (redraw)
  {
    tft.fillRect(0, 0, 320, 170, TFT_BLACK);
    snkPintaTodo();
  }
  printPoolData(); // barra de abajo, solo se repinta cuando toca
  hasChangedScreen = false;

  char horaStr[9] = "";
  char lastHora[9] = "";
  char hashStr[16];
  char lastHash[16] = "";

  unsigned long start = millis();
  unsigned long lastMiningUpdate = 0;
  unsigned long lastTouchChk = 0;
  unsigned long siguiente = start;
  pongLeds(false);

  while (millis() - start < SNK_SLICE_MS)
  {
    unsigned long now = millis();

    // ---- Táctil (misma lógica que el Pong) ----
    if (now - lastTouchChk >= 40 && now - previousTouchMillis >= 500)
    {
      lastTouchChk = now;
      int16_t t_x, t_y;
      if (toqueXY(t_x, t_y))
      {
        previousTouchMillis = now;
        if (((t_x > 109) && (t_x < 211)) && ((t_y > 185) && (t_y < 241)))
        {
          bottomScreenBlue ^= true;
          hasChangedScreen = true;
        }
        else if ((t_x > 235) && ((t_y > 0) && (t_y < 16)))
        {
          esp32_2432S028R_AlternateScreenState();
        }
        else if (t_x > 160)
        {
          currentDisplayDriver->current_cyclic_screen = (currentDisplayDriver->current_cyclic_screen + 1) % currentDisplayDriver->num_cyclic_screens;
        }
        else
        {
          currentDisplayDriver->current_cyclic_screen = currentDisplayDriver->current_cyclic_screen - 1;
          if (currentDisplayDriver->current_cyclic_screen < 0)
            currentDisplayDriver->current_cyclic_screen = currentDisplayDriver->num_cyclic_screens - 1;
        }
        break;
      }
    }
    // ---- Partida ----
    if (!snkM->estado)
    {
      if (now >= siguiente)
      {
        snkPaso();
        siguiente = millis() + SNK_PASO_MS;
      }
    }
    else if (now - snkM->finAt >= SNK_FIN_MS)
    {
      snkNueva();
      snkPintaTodo();
      siguiente = millis() + SNK_PASO_MS;
    }

    // ---- Hora (izquierda) y hashrate (derecha), arriba como en las demás ----
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
        pongDrawHud(hashStr, horaStr);
        strcpy(lastHora, horaStr);
        strcpy(lastHash, hashStr);
      }
    }
    webAtender();
    delay(5);
  }

  pongLeds(false);
  tft.setTextPadding(0);
  tft.setTextDatum(TL_DATUM);

  if (currentDisplayDriver->current_cyclic_screen != snkMiPantalla)
  {
    free(snkM);
    snkM = NULL;
  }

  cuentita++;
  if (cuentita == 15)
  {
    ajustarZonaHoraria();
    obtenerLocYTemp();
    obtenerVariacionesBTC(variacion1dia, variacion1semana);
  }
}

/****************************************************************************************************************
 *
 *   Escrito por: M8AX
 *
 *   Descripción: GALAXIAN CPU - Pantalla para ESP32_2432S028R
 *   ----------------------------------------------------------
 *
 *   La CPU juega sola al Galaxian: una formación de 8x5 marcianos que se mueve de lado a lado y de vez en
 *   cuando se lanzan en picado contra la nave, disparando por el camino. La nave decide ella sola: si hay un
 *   disparo o un marciano en picado cerca se aparta, y si no se coloca debajo del marciano que más le
 *   conviene (adelantándose a donde estará cuando llegue la bala) y dispara. Cada oleada es un poco más
 *   rápida y pica más a menudo, y como la CPU esquiva peor cuanto más avanza el juego, tarde o temprano la
 *   matan. Tiene 3 vidas; al perder la última sale GAME OVER y empieza otra partida xD...
 *
 *   Arriba a la izquierda la hora y arriba a la derecha el hashrate, como en las demás. Abajo van los
 *   puntos, la oleada, las vidas y el récord.
 *
 *   LED: disparo = rojo corto, marciano destruido = verde, y si matan a la nave se encienden los tres a la vez.
 *
 *   Memoria: al arrancar el minero no ocupa NADA. Las tablas (poco más de 1 KB) se piden con malloc al entrar
 *   en la pantalla y se liberan al salir a otra, como en el ajedrez. Solo quedan fijos el récord y las
 *   partidas (8 bytes). Si no hubiera RAM, se salta la pantalla sin pintar nada.
 *
 *   Para activarla hay que pegar este bloque antes de analiCadaSegundo y meter
 *   esp32_2432S028R_GalaxianScreen en el array esp32_2432S028RCyclicScreens[].
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
#define GAL_SLICE_MS 5000 // tandas largas, como en las demás
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
  uint8_t vidas, nivel, vivos, ledPrio;
  bool anim, sucio;
  uint32_t puntos, fr;
  unsigned long finAt, disparoAt, picadoAt, animAt, inmuneHasta, ledOff;
};
static GalMem *galM = NULL;
static uint32_t galRecord = 0;
static uint32_t galPartidas = 0;
static const uint16_t GAL_COL[GAL_ROWS] = {TFT_MAGENTA, TFT_ORANGE, TFT_CYAN, TFT_GREEN, TFT_SKYBLUE};

static void galLed(bool r, bool g, bool b, uint16_t ms, uint8_t prio)
{
  unsigned long now = millis();
  if (galM->ledOff && now < galM->ledOff && prio < galM->ledPrio)
    return;
  digitalWrite(LED_PIN, r ? LOW : HIGH);
  digitalWrite(LED_PIN_G, g ? LOW : HIGH);
  digitalWrite(LED_PIN_B, b ? LOW : HIGH);
  galM->ledOff = now + ms;
  galM->ledPrio = prio;
}

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
  digitalWrite(LED_PIN, HIGH);
  digitalWrite(LED_PIN_G, HIGH);
  digitalWrite(LED_PIN_B, HIGH);
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
      galLed(true, false, false, 45, 0);
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
    galLed(false, true, false, 110, 1);
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
  galLed(true, true, true, 450, 2);
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

void esp32_2432S028R_GalaxianScreen(unsigned long mElapsed)
{
  mineria = getMiningData(mElapsed);
  Serial.printf("M8AX - %s >>> Completados %s Share(s), %s Khashes, Prom. Hashrate %s KH/s %s°\n",
                mineria.currentTime.c_str(), mineria.completedShares.c_str(), mineria.totalKHashes.c_str(), mineria.currentHashRate.c_str(), mineria.temp.c_str());

  bool redraw = hasChangedScreen;
  if (!galM)
  {
    galM = (GalMem *)malloc(sizeof(GalMem));
    if (!galM)
    {
      Serial.println("M8AX - Galaxian: No Hay RAM Libre");
      delay(1000);
      return;
    }
    galNueva();
    redraw = true;
  }
  const int galMiPantalla = currentDisplayDriver->current_cyclic_screen;
  if (redraw)
  {
    tft.fillRect(0, 0, 320, 170, TFT_BLACK);
    galPintaTodo();
  }
  printPoolData(); // barra de abajo, solo se repinta cuando toca
  hasChangedScreen = false;

  char horaStr[9] = "";
  char lastHora[9] = "";
  char hashStr[16];
  char lastHash[16] = "";

  unsigned long start = millis();
  unsigned long lastMiningUpdate = 0;
  unsigned long lastTouchChk = 0;
  unsigned long siguiente = start;
  pongLeds(false);
  galM->ledOff = 0;

  while (millis() - start < GAL_SLICE_MS)
  {
    unsigned long now = millis();

    // ---- Táctil (misma lógica que el Pong) ----
    if (now - lastTouchChk >= 40 && now - previousTouchMillis >= 500)
    {
      lastTouchChk = now;
      int16_t t_x, t_y;
      if (toqueXY(t_x, t_y))
      {
        previousTouchMillis = now;
        if (((t_x > 109) && (t_x < 211)) && ((t_y > 185) && (t_y < 241)))
        {
          bottomScreenBlue ^= true;
          hasChangedScreen = true;
        }
        else if ((t_x > 235) && ((t_y > 0) && (t_y < 16)))
        {
          esp32_2432S028R_AlternateScreenState();
        }
        else if (t_x > 160)
        {
          currentDisplayDriver->current_cyclic_screen = (currentDisplayDriver->current_cyclic_screen + 1) % currentDisplayDriver->num_cyclic_screens;
        }
        else
        {
          currentDisplayDriver->current_cyclic_screen = currentDisplayDriver->current_cyclic_screen - 1;
          if (currentDisplayDriver->current_cyclic_screen < 0)
            currentDisplayDriver->current_cyclic_screen = currentDisplayDriver->num_cyclic_screens - 1;
        }
        break;
      }
    }
    if (galM->ledOff && now >= galM->ledOff)
    {
      pongLeds(false);
      galM->ledOff = 0;
      galM->ledPrio = 0;
    }

    if (now >= siguiente)
    {
      galFrame(now);
      siguiente = millis() + GAL_FRAME_MS;
    }

    // ---- Hora (izquierda) y hashrate (derecha), arriba como en las demás ----
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
        pongDrawHud(hashStr, horaStr);
        strcpy(lastHora, horaStr);
        strcpy(lastHash, hashStr);
      }
    }
    webAtender();
    delay(2);
  }

  pongLeds(false);
  tft.setTextPadding(0);
  tft.setTextDatum(TL_DATUM);

  if (currentDisplayDriver->current_cyclic_screen != galMiPantalla)
  {
    free(galM);
    galM = NULL;
  }

  cuentita++;
  if (cuentita == 15)
  {
    ajustarZonaHoraria();
    obtenerLocYTemp();
    obtenerVariacionesBTC(variacion1dia, variacion1semana);
  }
}

/****************************************************************************************************************
 *
 *   Escrito por: M8AX
 *
 *   Descripción: BANNER M8AX - Pantalla para ESP32_2432S028R
 *   --------------------------------------------------------
 *
 *   Un letrero con letras gigantes que entra por la derecha, cruza la pantalla hasta salir por la izquierda y
 *   vuelta a empezar, cada pasada de un color distinto. El mensaje es:
 *
 *       M8AX - FECHA - HORA - HASHRATE KH/s
 *
 *   y se vuelve a montar al empezar cada pasada, así que fecha, hora y hashrate van siempre al día.
 *
 *   LED: cambia cada 150 ms, rojo, verde, azul y otra vez rojo, uno solo encendido cada vez.
 *
 *   La librería de la pantalla no deja pasar del tamaño 7 de letra, así que aquí las letras se dibujan a mano:
 *   cada píxel de una letra de 5x7 es un bloque de BAN_S x BAN_S píxeles de pantalla. Con BAN_S 24 las letras
 *   ocupan toda la altura (168 píxeles). Para moverlas no se repinta nada: en cada paso solo se tapa la
 *   franja que se queda atrás y se pinta la que entra por delante, así que no parpadea.
 *
 *   Memoria: al arrancar el minero no ocupa NADA. La estructura del mensaje (unos 100 bytes) se pide al entrar
 *   en la pantalla y se libera al salir a otra, como en el ajedrez. Si no hubiera RAM, se salta la pantalla
 *   sin pintar nada.
 *
 *   Ajustes: BAN_S (tamaño de los bloques, la letra mide 6 x BAN_S de ancho y 7 x BAN_S de alto; se centra
 *   sola en vertical) y BAN_PXS (velocidad).
 *
 *   Para activarla hay que pegar este bloque antes de analiCadaSegundo y meter
 *   esp32_2432S028R_BannerScreen en el array esp32_2432S028RCyclicScreens[].
 *
 ****************************************************************************************************************/

#define BAN_S 24                      // lado de cada bloque
#define BAN_Y ((170 - 7 * BAN_S) / 2) // la letra queda centrada en vertical
#define BAN_MAXC (320 / BAN_S + 4)    // columnas que pueden verse a la vez
#define BAN_PXS 500                   // píxeles por segundo
#define BAN_FRAME_MS 20
#define BAN_LED_MS 150
#define BAN_SLICE_MS 5000                                              // tandas largas, como en las demás
#define BAN_USDT 1                                                     // 1 = pone " USDT" detrás de cada precio
#define BAN_POR_GRUPO 6                                                // criptos por tanda
#define BAN_GRUPOS ((NUM_CRIPTOS + BAN_POR_GRUPO - 1) / BAN_POR_GRUPO) // 18 criptos = 3 tandas
#define BAN_HASH_TODOS 0                                               // 1 = el hashrate sale en las 3 tandas, 0 = solo en la primera
#define BAN_MSG_MAX 480                                                // sitio de sobra para fecha, día, hora, hashrate y 6 criptos

struct BanMem
{
  char msg[BAN_MSG_MAX];
  int len;
  int x;
  uint8_t color;
  uint8_t led;
  uint8_t grupo; // tanda actual: 0, 1 o 2
  unsigned long ledAt;
  unsigned long t;
};
static BanMem *banM = NULL;
static const uint16_t BAN_COLORS[8] = {TFT_RED, TFT_GREEN, TFT_CYAN, TFT_YELLOW, TFT_MAGENTA, TFT_ORANGE, TFT_WHITE, TFT_SKYBLUE};

// Letras de 5x7: el primer byte es el carácter y los otros cinco las columnas (el bit 0 es la fila de arriba)
static const uint8_t BAN_FONT[][6] = {
    {' ', 0x00, 0x00, 0x00, 0x00, 0x00},
    {'-', 0x08, 0x08, 0x08, 0x08, 0x08},
    {'.', 0x00, 0x60, 0x60, 0x00, 0x00},
    {'/', 0x20, 0x10, 0x08, 0x04, 0x02},
    {'0', 0x3E, 0x51, 0x49, 0x45, 0x3E},
    {'1', 0x00, 0x42, 0x7F, 0x40, 0x00},
    {'2', 0x72, 0x49, 0x49, 0x49, 0x46},
    {'3', 0x21, 0x41, 0x49, 0x4D, 0x33},
    {'4', 0x18, 0x14, 0x12, 0x7F, 0x10},
    {'5', 0x27, 0x45, 0x45, 0x45, 0x39},
    {'6', 0x3C, 0x4A, 0x49, 0x49, 0x31},
    {'7', 0x41, 0x21, 0x11, 0x09, 0x07},
    {'8', 0x36, 0x49, 0x49, 0x49, 0x36},
    {'9', 0x46, 0x49, 0x49, 0x29, 0x1E},
    {':', 0x00, 0x36, 0x36, 0x00, 0x00},
    {'A', 0x7C, 0x12, 0x11, 0x12, 0x7C},
    {'H', 0x7F, 0x08, 0x08, 0x08, 0x7F},
    {'K', 0x7F, 0x08, 0x14, 0x22, 0x41},
    {'M', 0x7F, 0x02, 0x0C, 0x02, 0x7F},
    {'X', 0x63, 0x14, 0x08, 0x14, 0x63},
    {'B', 0x7F, 0x49, 0x49, 0x49, 0x36},
    {'C', 0x3E, 0x41, 0x41, 0x41, 0x22},
    {'D', 0x7F, 0x41, 0x41, 0x22, 0x1C},
    {'E', 0x7F, 0x49, 0x49, 0x49, 0x41},
    {'G', 0x3E, 0x41, 0x49, 0x49, 0x7A},
    {'I', 0x00, 0x41, 0x7F, 0x41, 0x00},
    {'J', 0x20, 0x40, 0x41, 0x3F, 0x01},
    {'L', 0x7F, 0x40, 0x40, 0x40, 0x40},
    {'N', 0x7F, 0x04, 0x08, 0x10, 0x7F},
    {'O', 0x3E, 0x41, 0x41, 0x41, 0x3E},
    {'R', 0x7F, 0x09, 0x19, 0x29, 0x46},
    {'S', 0x46, 0x49, 0x49, 0x49, 0x31},
    {'T', 0x01, 0x01, 0x7F, 0x01, 0x01},
    {'U', 0x3F, 0x40, 0x40, 0x40, 0x3F},
    {'V', 0x1F, 0x20, 0x40, 0x20, 0x1F},
    {'P', 0x7F, 0x09, 0x09, 0x09, 0x06},
    {'Y', 0x07, 0x08, 0x70, 0x08, 0x07},
    {'s', 0x48, 0x54, 0x54, 0x54, 0x24}};

static void banLed(uint8_t k)
{
  digitalWrite(LED_PIN, k == 0 ? LOW : HIGH);
  digitalWrite(LED_PIN_G, k == 1 ? LOW : HIGH);
  digitalWrite(LED_PIN_B, k == 2 ? LOW : HIGH);
}

static const char *BAN_DIAS[7] = {"DOMINGO", "LUNES", "MARTES", "MIERCOLES", "JUEVES", "VIERNES", "SABADO"};

static void banMensaje(unsigned long mElapsed)
{
  mineria = getMiningData(mElapsed);
  time_t epoch = (time_t)timeClient.getEpochTime();
  struct tm *ti = localtime(&epoch);
  int n = snprintf(banM->msg, BAN_MSG_MAX, "M8AX - %02d/%02d/%04d - %s - %02d:%02d:%02d",
                   ti->tm_mday, ti->tm_mon + 1, ti->tm_year + 1900, BAN_DIAS[ti->tm_wday],
                   ti->tm_hour, ti->tm_min, ti->tm_sec);
  if (n >= BAN_MSG_MAX)
    n = BAN_MSG_MAX - 1;
  // el hashrate solo sale en la primera tanda (BAN_HASH_TODOS 1 = en las tres)
  if (banM->grupo == 0 || BAN_HASH_TODOS)
  {
    n += snprintf(banM->msg + n, BAN_MSG_MAX - n, " - %s KH/s", mineria.currentHashRate.c_str());
    if (n >= BAN_MSG_MAX)
      n = BAN_MSG_MAX - 1;
  }
  // solo las 6 criptos que tocan en esta tanda (0-5, 6-11, 12-17)
  int ini = banM->grupo * BAN_POR_GRUPO;
  int fin = ini + BAN_POR_GRUPO;
  if (fin > NUM_CRIPTOS)
    fin = NUM_CRIPTOS;
  for (int k = ini; k < fin && n < BAN_MSG_MAX - 1; k++)
  {
    n += snprintf(banM->msg + n, BAN_MSG_MAX - n, " - %s %s%s",
                  criptos[k].simbolo, formatoPrecio(criptos[k].precio).c_str(), BAN_USDT ? " USDT" : "");
    if (n >= BAN_MSG_MAX)
      n = BAN_MSG_MAX - 1;
  }
  banM->len = strlen(banM->msg);
  banM->x = 320;
}

// Columna c del mensaje (6 por letra: 5 de la letra y una vacía de separación)
static uint8_t banColumna(int c)
{
  if (c < 0 || c >= banM->len * 6 || c % 6 == 5)
    return 0;
  char ch = banM->msg[c / 6];
  for (unsigned i = 0; i < sizeof(BAN_FONT) / sizeof(BAN_FONT[0]); i++)
    if (BAN_FONT[i][0] == (uint8_t)ch)
      return BAN_FONT[i][1 + c % 6];
  return 0;
}

static void banRect(int x, int y, int w, int h, uint16_t c)
{
  if (x < 0)
  {
    w += x;
    x = 0;
  }
  if (x + w > 320)
    w = 320 - x;
  if (w > 0 && h > 0)
    tft.fillRect(x, y, w, h, c);
}

// av = lo que se ha movido el texto hacia la izquierda desde el último dibujo (0 = pintarlo entero)
static void banPaso(int av)
{
  int t = -banM->x - av;
  int c0 = (t < 0) ? 0 : t / BAN_S;
  int r = 320 - banM->x;
  int c1 = (r <= 0) ? -1 : (r + BAN_S - 1) / BAN_S - 1;
  if (c1 > banM->len * 6 - 1)
    c1 = banM->len * 6 - 1;
  if (c1 - c0 + 1 > BAN_MAXC)
    c1 = c0 + BAN_MAXC - 1;
  int n = c1 - c0 + 1;
  if (n <= 0)
    return;

  uint8_t col[BAN_MAXC];
  for (int k = 0; k < n; k++)
    col[k] = banColumna(c0 + k);

  uint16_t color = BAN_COLORS[banM->color];
  for (int f = 0; f < 7; f++)
  {
    int y = BAN_Y + f * BAN_S;
    int k = 0;
    while (k < n)
    {
      if (!(col[k] & (1 << f)))
      {
        k++;
        continue;
      }
      int ini = k;
      while (k < n && (col[k] & (1 << f)))
        k++;
      int xi = banM->x + (c0 + ini) * BAN_S;
      int w = (k - ini) * BAN_S;
      if (av > 0)
      {
        banRect(xi + w, y, av, BAN_S, TFT_BLACK); // la franja que se queda atrás
        banRect(xi, y, av, BAN_S, color);         // la que entra por delante
      }
      else
        banRect(xi, y, w, BAN_S, color);
    }
  }
}

void esp32_2432S028R_BannerScreen(unsigned long mElapsed)
{
  mineria = getMiningData(mElapsed);
  Serial.printf("M8AX - %s >>> Completados %s Share(s), %s Khashes, Prom. Hashrate %s KH/s %s°\n",
                mineria.currentTime.c_str(), mineria.completedShares.c_str(), mineria.totalKHashes.c_str(), mineria.currentHashRate.c_str(), mineria.temp.c_str());

  bool redraw = hasChangedScreen;
  if (!banM)
  {
    banM = (BanMem *)malloc(sizeof(BanMem));
    if (!banM)
    {
      Serial.println("M8AX - Banner: No Hay RAM Libre");
      delay(1000);
      return;
    }
    memset(banM, 0, sizeof(BanMem));
    banM->color = esp_random() % 8;
    banMensaje(mElapsed);
    banM->t = millis();
    banM->ledAt = millis();
    redraw = true;
  }
  const int banMiPantalla = currentDisplayDriver->current_cyclic_screen;
  if (redraw)
  {
    tft.fillRect(0, 0, 320, 170, TFT_BLACK);
    banPaso(0);
  }
  printPoolData(); // barra de abajo, solo se repinta cuando toca
  hasChangedScreen = false;

  unsigned long start = millis();
  unsigned long lastTouchChk = 0;
  banLed(banM->led);

  while (millis() - start < BAN_SLICE_MS)
  {
    unsigned long now = millis();

    // ---- Táctil (misma lógica que el Pong) ----
    if (now - lastTouchChk >= 40 && now - previousTouchMillis >= 500)
    {
      lastTouchChk = now;
      int16_t t_x, t_y;
      if (toqueXY(t_x, t_y))
      {
        previousTouchMillis = now;
        if (((t_x > 109) && (t_x < 211)) && ((t_y > 185) && (t_y < 241)))
        {
          bottomScreenBlue ^= true;
          hasChangedScreen = true;
        }
        else if ((t_x > 235) && ((t_y > 0) && (t_y < 16)))
        {
          esp32_2432S028R_AlternateScreenState();
        }
        else if (t_x > 160)
        {
          currentDisplayDriver->current_cyclic_screen = (currentDisplayDriver->current_cyclic_screen + 1) % currentDisplayDriver->num_cyclic_screens;
        }
        else
        {
          currentDisplayDriver->current_cyclic_screen = currentDisplayDriver->current_cyclic_screen - 1;
          if (currentDisplayDriver->current_cyclic_screen < 0)
            currentDisplayDriver->current_cyclic_screen = currentDisplayDriver->num_cyclic_screens - 1;
        }
        break;
      }
    }
    if (now - banM->ledAt >= BAN_LED_MS)
    {
      banM->led = (banM->led + 1) % 3;
      banM->ledAt = now;
      banLed(banM->led);
    }

    if (now - banM->t >= BAN_FRAME_MS)
    {
      int avance = (int)((now - banM->t) * BAN_PXS / 1000);
      if (avance < 1)
        avance = 1;
      if (avance > BAN_S)
        avance = BAN_S;
      banM->t = now;
      banM->x -= avance;
      banPaso(avance);
      if (banM->x + banM->len * 6 * BAN_S <= 0)
      {
        banM->color = (banM->color + 1) % 8;
        banM->grupo = (banM->grupo + 1) % BAN_GRUPOS; // siguiente tanda; tras la última vuelve a empezar
        banMensaje(mElapsed);
      }
    }
    webAtender();
    delay(2);
  }

  pongLeds(false);
  tft.setTextPadding(0);
  tft.setTextDatum(TL_DATUM);

  if (currentDisplayDriver->current_cyclic_screen != banMiPantalla)
  {
    free(banM);
    banM = NULL;
  }

  cuentita++;
  if (cuentita == 15)
  {
    ajustarZonaHoraria();
    obtenerLocYTemp();
    obtenerVariacionesBTC(variacion1dia, variacion1semana);
  }
}

/********************************************************************************************
 *  PANTALLA FRACTALES - Mandelbrot, Julia, Burning Ship, Tricorn y Newton, calculados por el ESP32
 ********************************************************************************************/
#define FRAC_W 320
#define FRAC_H 152
#define FRAC_SLICE_MS 5000 // tandas de 5 s como las demás
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

/****************************************************************************************************************
 *
 *   LEDs De La Pantalla Fractal
 *   ---------------------------
 *   Mientras se pinta parpadean cambiando de color según lo fino que va el dibujo: rojo en el borroso,
 *   luego amarillo, verde y cian al afinar. Al terminar, destello blanco. Cuando sale un fractal nuevo,
 *   destello magenta. Sin delay, el apagado lo hace el bucle con ledApagarEn.
 *
 ****************************************************************************************************************/

static void fracLed(bool r, bool g, bool b)
{
  digitalWrite(LED_PIN, r ? LOW : HIGH);
  digitalWrite(LED_PIN_G, g ? LOW : HIGH);
  digitalWrite(LED_PIN_B, b ? LOW : HIGH);
}

static void fracLedEtapa(uint16_t paso)
{
  if (paso < 19)
    fracLed(true, false, false);
  else if (paso < 38)
    fracLed(true, true, false);
  else if (paso < 76)
    fracLed(false, true, false);
  else
    fracLed(false, true, true);
}

void esp32_2432S028R_FractalScreen(unsigned long mElapsed)
{
  mineria = getMiningData(mElapsed);
  Serial.printf("M8AX - %s >>> Completados %s Share(s), %s Khashes, Prom. Hashrate %s KH/s %s°\n",
                mineria.currentTime.c_str(), mineria.completedShares.c_str(), mineria.totalKHashes.c_str(), mineria.currentHashRate.c_str(), mineria.temp.c_str());

  bool redraw = hasChangedScreen;
  if (!fracInit)
  {
    fracInit = true;
    redraw = true;
  }

  if (redraw) // cada vez que se entra en la pantalla sale un fractal distinto
  {
    fracNuevo();
    tft.fillRect(0, 0, 320, 170, TFT_BLACK);
  }
  printPoolData();
  hasChangedScreen = false;

  char hashStr[16];
  snprintf(hashStr, sizeof(hashStr), "%s", mineria.currentHashRate.c_str());
  char izq[24], der[24];
  char lastIzq[24] = "", lastDer[24] = "";

  unsigned long start = millis();
  unsigned long lastHud = 0;
  unsigned long lastMiningUpdate = 0;
  unsigned long lastTouchChk = 0;
  pongLeds(false);
  unsigned long ledApagarEn = 0;
  unsigned long ledSigue = 0;

  while (millis() - start < FRAC_SLICE_MS)
  {
    unsigned long now = millis();

    // ---- Tactil (misma logica que esp32_2432S028R_DoLedStuff) ----
    if (now - lastTouchChk >= 40 && now - previousTouchMillis >= 500)
    {
      lastTouchChk = now;
      int16_t t_x, t_y;
      if (toqueXY(t_x, t_y))
      {
        previousTouchMillis = now;
        if (((t_x > 109) && (t_x < 211)) && ((t_y > 185) && (t_y < 241)))
        {
          bottomScreenBlue ^= true;
          hasChangedScreen = true;
        }
        else if ((t_x > 235) && ((t_y > 0) && (t_y < 16)))
        {
          esp32_2432S028R_AlternateScreenState();
        }
        else if (t_x > 160)
        {
          currentDisplayDriver->current_cyclic_screen = (currentDisplayDriver->current_cyclic_screen + 1) % currentDisplayDriver->num_cyclic_screens;
        }
        else
        {
          currentDisplayDriver->current_cyclic_screen = currentDisplayDriver->current_cyclic_screen - 1;
          if (currentDisplayDriver->current_cyclic_screen < 0)
            currentDisplayDriver->current_cyclic_screen = currentDisplayDriver->num_cyclic_screens - 1;
        }
        break;
      }
    }

    if (ledApagarEn && now >= ledApagarEn)
    {
      pongLeds(false);
      ledApagarEn = 0;
    }
    if (fr.paso < FRAC_H && !ledApagarEn && now >= ledSigue)
    {
      fracLedEtapa(fr.paso);
      ledApagarEn = now + 60;
      ledSigue = now + 260;
    }

    // ---- Fractal: una tanda de filas por vuelta ----
    if (fr.paso < FRAC_H)
    {
      int y, h;
      fracPaso(fr.paso, y, h);
      fracPintaFila(y, h);
      fr.paso++;
      if (fr.paso == FRAC_H) // terminado: destello y a esperar
      {
        fr.holdHasta = millis() + FRAC_HOLD_MS;
        fracLed(true, true, true);
        ledApagarEn = millis() + 150;
      }
    }
    else if (now >= fr.holdHasta) // toca otro fractal nuevo
    {
      fracNuevo();
      tft.fillRect(0, 0, 320, FRAC_H, TFT_BLACK);
      fracLed(true, false, true);
      ledApagarEn = millis() + 100;
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
        lifeDrawHud(izq, der);
        strcpy(lastIzq, izq);
        strcpy(lastDer, der);
      }
    }
    webAtender();
    delay(fr.paso < FRAC_H ? 1 : 10); // cede CPU al minero
  }

  pongLeds(false);
  tft.setTextPadding(0);
  tft.setTextDatum(TL_DATUM);

  cuentita++;
  if (cuentita == 15)
  {
    ajustarZonaHoraria(), obtenerLocYTemp();
    obtenerVariacionesBTC(variacion1dia, variacion1semana);
  }
}

/****************************************************************************************************************
 *
 *   Escrito por: M8AX
 *
 *   Descripción: GALAXIAS N CUERPOS - Pantalla para ESP32_2432S028R
 *   ----------------------------------------------------------------
 *
 *   Dos galaxias de 160 cuerpos cada una, con su núcleo pesado y sus estrellas en órbita, se acercan y chocan.
 *   Cada cuerpo atrae a todos los demás de verdad (gravedad de Newton en 3D, sin trampas), así que en cada paso
 *   el ESP32 resuelve 51.040 interacciones. Los pasos se encadenan sin parar hasta que toca pintar el cuadro
 *   (cada 40 ms), o sea que la CPU va a tope todo el rato, y aun así se atiende el táctil, el servidor web y
 *   el minero igual que en las demás pantallas.
 *
 *   Abajo a la izquierda sale cuántos millones de interacciones por segundo se están calculando de verdad
 *   y a la derecha el hashrate. La cámara gira despacio alrededor de las galaxias.
 *
 *   Matemáticas: paso de integración simpléctico (Euler semi-implícito), suavizado de la fuerza para que
 *   no se disparen los encuentros cercanos y raíz inversa rápida (1/sqrt(x) con un truco de bits y una
 *   iteración de Newton) para no gastar divisiones. Los dos núcleos se atraen con un suavizado mayor, así
 *   no forman una pareja dura que lance las estrellas fuera de la pantalla.
 *
 *   Colores: una galaxia azul y la otra naranja; cuanto más rápida va una estrella, más brillante.
 *   LED: destello corto cuando nace una pareja nueva y cada vez que los dos núcleos se cruzan.
 *
 *   Cada 5000 pasos (más o menos un minuto) nace otra pareja de galaxias con inclinaciones y giros al azar.
 *
 *   Memoria: al arrancar el minero no ocupa NADA. Los unos 16 KB de los cuerpos se piden al entrar en la
 *   pantalla y se liberan al salir a otra, como en el ajedrez. Si no hubiera RAM, se salta la pantalla.
 *
 *   Ajustes: NBD_N (cuerpos, el coste sube con el cuadrado: 240 va más de sobra, 400 ya ahoga), NBD_DT (paso
 *   de tiempo), NBD_ESC (zoom) y NBD_PASOS_MAX (cuánto dura cada pareja).
 *
 *   Para activarla hay que pegar este bloque antes de analiCadaSegundo y meter
 *   esp32_2432S028R_GalaxiasScreen en el array esp32_2432S028RCyclicScreens[].
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
#define NBD_SLICE_MS 5000
#define NBD_PASOS_MAX 5000
#define NBD_LED_MS 80

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
  bool cerca;
};
static NbdMem *nbd = NULL;

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

void esp32_2432S028R_GalaxiasScreen(unsigned long mElapsed)
{
  mineria = getMiningData(mElapsed);
  Serial.printf("M8AX - %s >>> Completados %s Share(s), %s Khashes, Prom. Hashrate %s KH/s %s°\n",
                mineria.currentTime.c_str(), mineria.completedShares.c_str(), mineria.totalKHashes.c_str(), mineria.currentHashRate.c_str(), mineria.temp.c_str());

  bool redraw = hasChangedScreen;
  if (!nbd)
  {
    nbd = (NbdMem *)malloc(sizeof(NbdMem));
    if (!nbd)
    {
      Serial.println("M8AX - Galaxias: No Hay RAM Libre");
      delay(1000);
      return;
    }
    nbdNueva();
    redraw = true;
  }
  const int nbdMiPantalla = currentDisplayDriver->current_cyclic_screen;
  if (redraw)
  {
    tft.fillRect(0, 0, 320, 170, TFT_BLACK);
    for (int i = 0; i < NBD_N; i++)
      nbd->px[i] = nbd->py[i] = -1;
  }
  printPoolData(); // barra de abajo, solo se repinta cuando toca
  hasChangedScreen = false;

  char hashStr[16];
  snprintf(hashStr, sizeof(hashStr), "%s", mineria.currentHashRate.c_str());
  char izq[24], der[24];
  char lastIzq[24] = "", lastDer[24] = "";

  unsigned long start = millis();
  unsigned long tSeg = start;
  unsigned long lastTouchChk = 0;
  unsigned long lastMiningUpdate = 0;
  unsigned long ledApagarEn = 0;
  unsigned long ledSigue = 0;
  uint32_t pasosSeg = 0;
  float mint = 0;
  pongLeds(false);

  while (millis() - start < NBD_SLICE_MS)
  {
    unsigned long now = millis();

    // ---- Táctil (misma lógica que el Pong) ----
    if (now - lastTouchChk >= 40 && now - previousTouchMillis >= 500)
    {
      lastTouchChk = now;
      int16_t t_x, t_y;
      if (toqueXY(t_x, t_y))
      {
        previousTouchMillis = now;
        if (((t_x > 109) && (t_x < 211)) && ((t_y > 185) && (t_y < 241)))
        {
          bottomScreenBlue ^= true;
          hasChangedScreen = true;
        }
        else if ((t_x > 235) && ((t_y > 0) && (t_y < 16)))
        {
          esp32_2432S028R_AlternateScreenState();
        }
        else if (t_x > 160)
        {
          currentDisplayDriver->current_cyclic_screen = (currentDisplayDriver->current_cyclic_screen + 1) % currentDisplayDriver->num_cyclic_screens;
        }
        else
        {
          currentDisplayDriver->current_cyclic_screen = currentDisplayDriver->current_cyclic_screen - 1;
          if (currentDisplayDriver->current_cyclic_screen < 0)
            currentDisplayDriver->current_cyclic_screen = currentDisplayDriver->num_cyclic_screens - 1;
        }
        break;
      }
    }

    // ---- LEDs: apagado del destello que toque ----
    if (ledApagarEn && now >= ledApagarEn)
    {
      pongLeds(false);
      ledApagarEn = 0;
    }

    // ---- LED latido: destello corto cada 400 ms, color según lo avanzada que va la simulación ----
    // azul (primer tercio), verde (segundo), amarillo (último). El LED es activo en LOW.
    if (!ledApagarEn && now >= ledSigue)
    {
      int fase = (int)((uint64_t)nbd->pasos * 3 / NBD_PASOS_MAX);
      if (fase > 2)
        fase = 2;
      digitalWrite(LED_PIN, fase == 2 ? LOW : HIGH);
      digitalWrite(LED_PIN_G, fase >= 1 ? LOW : HIGH);
      digitalWrite(LED_PIN_B, fase == 0 ? LOW : HIGH);
      ledApagarEn = now + 60;
      ledSigue = now + 400;
    }

    // ---- Física a tope hasta que toque pintar ----
    unsigned long t0 = millis();
    do
    {
      nbdPaso();
      nbd->pasos++;
      pasosSeg++;
    } while (millis() - t0 < NBD_FRAME_MS);

    float ddx = nbd->x[1] - nbd->x[0], ddy = nbd->y[1] - nbd->y[0], ddz = nbd->z[1] - nbd->z[0];
    float d2 = ddx * ddx + ddy * ddy + ddz * ddz;
    if (!nbd->cerca && d2 < 0.64f)
    {
      nbd->cerca = true;
      pongLeds(true); // destello blanco: pasan muy cerca
      ledApagarEn = millis() + NBD_LED_MS;
    }
    else if (nbd->cerca && d2 > 1.7f)
      nbd->cerca = false;

    nbd->giro += 0.012f;
    nbdDibuja();

    if (nbd->pasos >= NBD_PASOS_MAX)
    {
      nbdNueva();
      tft.fillRect(0, 0, 320, NBD_ALTO, TFT_BLACK);
      digitalWrite(LED_PIN, LOW); // destello rojo: simulación nueva
      digitalWrite(LED_PIN_G, HIGH);
      digitalWrite(LED_PIN_B, HIGH);
      ledApagarEn = millis() + NBD_LED_MS;
    }

    now = millis();
    if (now - tSeg >= 1000)
    {
      mint = pasosSeg * (float)NBD_PARES / ((now - tSeg) * 1000.0f);
      tSeg = now;
      pasosSeg = 0;
    }
    if (now - lastMiningUpdate >= 1000)
    {
      lastMiningUpdate = now;
      mineria = getMiningData(mElapsed);
      snprintf(hashStr, sizeof(hashStr), "%s", mineria.currentHashRate.c_str());
    }

    snprintf(izq, sizeof(izq), "%.1fM INT/S", mint);
    snprintf(der, sizeof(der), "%s KH/s", hashStr);
    if (strcmp(izq, lastIzq) != 0 || strcmp(der, lastDer) != 0)
    {
      nbdDrawHud(izq, der);
      strcpy(lastIzq, izq);
      strcpy(lastDer, der);
    }
    webAtender();
    delay(1);
  }

  pongLeds(false);
  tft.setTextPadding(0);
  tft.setTextDatum(TL_DATUM);

  if (currentDisplayDriver->current_cyclic_screen != nbdMiPantalla)
  {
    free(nbd);
    nbd = NULL;
  }

  cuentita++;
  if (cuentita == 15)
  {
    ajustarZonaHoraria();
    obtenerLocYTemp();
    obtenerVariacionesBTC(variacion1dia, variacion1semana);
  }
}

/****************************************************************************************************************
 *
 *   Escrito por: M8AX
 *
 *   Descripción: DOOM M8AX - Demo 3D estilo Doom para ESP32_2432S028R
 *   -----------------------------------------------------------------
 *
 *   Un raycaster ( la técnica de los primeros shooters ) calculado por el propio ESP32. La CPU juega sola:
 *   recorre un mapa de salas y pasillos, apunta a los demonios que ve, les dispara hasta matarlos y a los
 *   pocos segundos reaparecen. Todo es propio y se genera por código: texturas, demonios y arma.
 *
 *   Se dibuja por columnas de 2 píxeles, sin buffer de pantalla entero, así que no parpadea.
 *
 *   LEDs: destello ROJO al disparar, BLANCO al matar a un demonio y VERDE al llegar a un punto del recorrido.
 *
 *   Memoria: el mapa va en flash. El estado ( unos 4 KB ) se pide al entrar en la pantalla y se libera al
 *   salir a otra, como el Banner y las Galaxias. Si no hay RAM, se salta la pantalla sin pintar nada.
 *
 *   Para activarla hay que pegar este bloque antes de analiCadaSegundo y meter
 *   esp32_2432S028R_DoomScreen en el array esp32_2432S028RCyclicScreens[].
 *
 ****************************************************************************************************************/

// >>> DOOM MOTOR
#define DM_W 320
#define DM_H 152 // el HUD de abajo ocupa el resto, como en los fractales
#define DM_COLW 2
#define DM_COLS (DM_W / DM_COLW)
#define DM_N 16
#define DM_NEN 8
#define DM_NWP 9
#define DM_SLICE_MS 5000
#define DM_RGB(r, g, b) ((uint16_t)((((r) & 0xF8) << 8) | (((g) & 0xFC) << 3) | ((b) >> 3)))

// 0 = vacío; 1 ladrillo, 2 metal, 3 piedra, 4 carne
static const uint8_t DM_MAP[DM_N][DM_N] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 1},
    {1, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 3, 0, 0, 0, 0, 3, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 3, 0, 4, 4, 0, 3, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 3, 0, 4, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 3, 0, 0, 4, 4, 3, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 3, 0, 0, 0, 0, 3, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 1},
    {1, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};

// Recorrido de ida y vuelta: pasillos, puerta este de la sala central, puerta oeste y vuelta
static const float DM_WP[DM_NWP][2] = {{2.5f, 2.5f}, {13.5f, 2.5f}, {13.5f, 7.5f}, {10.5f, 7.5f}, {7.5f, 8.5f}, {2.5f, 8.5f}, {2.5f, 13.5f}, {13.5f, 13.5f}, {13.5f, 9.5f}};
static const float DM_SPAWN[DM_NEN][2] = {{6.5f, 1.5f}, {10.5f, 3.5f}, {12.5f, 5.5f}, {8.5f, 7.5f}, {3.5f, 10.5f}, {6.5f, 12.5f}, {11.5f, 12.5f}, {12.5f, 10.5f}};

struct DmEn
{
  float x, y;
  int8_t hp;
  uint8_t estado; // 0 vivo, 1 cayendo, 2 muerto esperando reaparecer
  uint32_t t;
  uint32_t flash;
};

struct DmMem
{
  float px, py, ang;
  uint8_t wp;
  int8_t wpDir;
  uint16_t kills;
  float paso;
  uint32_t disparoSig, armaHasta, ultimo;
  uint8_t evDisparo, evKill, evWp;
  DmEn en[DM_NEN];
  uint8_t tex[4][256];
  uint16_t pal[4][64];
  uint32_t techo[DM_H], suelo[DM_H]; // cada uno son dos píxeles iguales ( columna de 2 px )
  uint32_t col[DM_H];
};
static DmMem *dmM = NULL;

struct DmSpr
{
  int8_t i;
  uint8_t tipo, sombra;
  bool blanco, sangre;
  int cl, cr, top, h;
  float dist;
};

static void dmInit()
{
  DmMem &d = *dmM;
  memset(&d, 0, sizeof(DmMem));

  // Texturas de 16x16 ( intensidad 0..255 ) generadas por código
  for (int y = 0; y < 16; y++)
    for (int x = 0; x < 16; x++)
    {
      int n = (x * 7 + y * 13 + x * y) & 31;
      int v;
      if ((y & 7) == 0 || ((x + ((y >> 3) & 1) * 8) & 15) == 0) // ladrillo
        v = 70;
      else
        v = 175 + n;
      d.tex[0][y * 16 + x] = v;

      v = 170 + (n >> 2); // metal con remaches
      if (x == 0 || y == 0)
        v = 80;
      else if (y == 8)
        v = 125;
      if ((x == 3 || x == 12) && (y == 3 || y == 12))
        v = 250;
      d.tex[1][y * 16 + x] = v;

      v = 140 + n * 2; // piedra con juntas
      if (y == 7 || y == 15 || (x == 7 && y < 7) || (x == 3 && y > 7))
        v = 75;
      d.tex[2][y * 16 + x] = v;

      v = 110 + ((x ^ y) & 15) * 8; // carne con venas
      if (x % 5 == 0)
        v = v * 2 / 3;
      d.tex[3][y * 16 + x] = v;
    }
  static const uint8_t base[4][3] = {{175, 105, 70}, {100, 120, 150}, {125, 125, 112}, {170, 38, 32}};
  for (int t = 0; t < 4; t++)
    for (int l = 0; l < 64; l++)
      d.pal[t][l] = DM_RGB(base[t][0] * l / 63, base[t][1] * l / 63, base[t][2] * l / 63);

  // Techo oscuro y suelo marrón, más claros cuanto más cerca
  for (int y = 0; y < DM_H; y++)
  {
    uint32_t c = 0;
    if (y < DM_H / 2)
    {
      int v = 10 + y * 34 / (DM_H / 2);
      uint16_t p = DM_RGB(v, v, v + v / 3);
      d.techo[y] = p | ((uint32_t)p << 16);
    }
    else
    {
      int v = 22 + (y - DM_H / 2) * 70 / (DM_H / 2);
      uint16_t p = DM_RGB(v, v * 7 / 10, v / 2);
      d.suelo[y] = p | ((uint32_t)p << 16);
    }
    (void)c;
  }

  for (int i = 0; i < DM_NEN; i++)
  {
    d.en[i].x = DM_SPAWN[i][0];
    d.en[i].y = DM_SPAWN[i][1];
    d.en[i].hp = 3;
  }
  d.px = DM_WP[0][0];
  d.py = DM_WP[0][1];
  d.ang = 0;
  d.wp = 1;
  d.wpDir = 1;
}

// ¿Hay línea de visión libre entre dos puntos?
static bool dmLibre(float x1, float y1, float x2, float y2)
{
  float dx = x2 - x1, dy = y2 - y1;
  int n = (int)(sqrtf(dx * dx + dy * dy) / 0.12f) + 1;
  for (int k = 1; k < n; k++)
  {
    float t = (float)k / n;
    if (DM_MAP[(int)(y1 + dy * t)][(int)(x1 + dx * t)])
      return false;
  }
  return true;
}

static float dmNorm(float a)
{
  while (a > 3.14159265f)
    a -= 6.2831853f;
  while (a < -3.14159265f)
    a += 6.2831853f;
  return a;
}

// La CPU juega: apunta, dispara, o sigue el recorrido
static void dmUpdate(uint32_t now, float dt)
{
  DmMem &d = *dmM;

  for (int i = 0; i < DM_NEN; i++)
  {
    DmEn &e = d.en[i];
    if (e.estado == 1 && (int32_t)(now - e.t) >= 0)
    {
      e.estado = 2;
      e.t = now + 6000 + (esp_random() % 4000);
    }
    else if (e.estado == 2 && (int32_t)(now - e.t) >= 0)
    {
      e.estado = 0;
      e.hp = 3;
    }
  }

  // Objetivo: el demonio vivo más cercano que se ve
  int obj = -1;
  float mejorD = 99.0f, dif = 0;
  for (int i = 0; i < DM_NEN; i++)
  {
    DmEn &e = d.en[i];
    if (e.estado != 0)
      continue;
    float ex = e.x - d.px, ey = e.y - d.py;
    float dist = sqrtf(ex * ex + ey * ey);
    if (dist > 8.0f || dist >= mejorD)
      continue;
    float a = dmNorm(atan2f(ey, ex) - d.ang);
    if (fabsf(a) > 0.8f || !dmLibre(d.px, d.py, e.x, e.y))
      continue;
    obj = i;
    mejorD = dist;
    dif = a;
  }

  float giro = 3.2f * dt, vel = 0;
  if (obj >= 0)
  {
    d.ang += (dif > giro) ? giro : (dif < -giro ? -giro : dif);
    if (fabsf(dif) < 0.07f && (int32_t)(now - d.disparoSig) >= 0)
    {
      d.disparoSig = now + 300 + (esp_random() & 127);
      d.armaHasta = now + 90;
      d.evDisparo = 1;
      DmEn &e = d.en[obj];
      e.hp--;
      e.flash = now + 90;
      if (e.hp <= 0)
      {
        e.estado = 1;
        e.t = now + 350;
        d.kills++;
        d.evKill = 1;
      }
    }
    if (mejorD > 2.6f)
      vel = 1.2f;
  }
  else
  {
    float tx = DM_WP[d.wp][0] - d.px, ty = DM_WP[d.wp][1] - d.py;
    float a = dmNorm(atan2f(ty, tx) - d.ang);
    d.ang += (a > giro) ? giro : (a < -giro ? -giro : a);
    if (fabsf(a) < 0.35f)
      vel = 2.0f;
    if (tx * tx + ty * ty < 0.09f)
    {
      int nw = d.wp + d.wpDir;
      if (nw < 0 || nw >= DM_NWP)
      {
        d.wpDir = -d.wpDir;
        nw = d.wp + d.wpDir;
      }
      d.wp = nw;
      d.evWp = 1;
    }
  }
  if (d.ang > 100.0f || d.ang < -100.0f)
    d.ang = dmNorm(d.ang);

  if (vel > 0)
  {
    float cs = cosf(d.ang), sn = sinf(d.ang);
    float nx = d.px + cs * vel * dt, ny = d.py + sn * vel * dt;
    if (!DM_MAP[(int)d.py][(int)(nx + (cs > 0 ? 0.2f : -0.2f))])
      d.px = nx;
    if (!DM_MAP[(int)(ny + (sn > 0 ? 0.2f : -0.2f))][(int)d.px])
      d.py = ny;
    d.paso += dt * 9.0f;
  }
}

// Un píxel de demonio ( rejilla de 32x32 ). Devuelve 0 si es transparente.
static uint16_t dmSprPix(uint8_t tipo, int ux, int vy, int sombra, bool blanco, bool sangre)
{
  static const uint8_t col[3][3] = {{165, 100, 55}, {205, 115, 125}, {195, 45, 40}};
  int r = 0, g = 0, b = 0;
  bool on = false;
  int l = 220;
  if (tipo == 2) // cacodemonio: una bola con un ojo enorme
  {
    int dx = ux - 16, dy = vy - 15;
    int d2 = dx * dx + dy * dy;
    if (d2 <= 225)
    {
      on = true;
      l = 255 - d2 / 2;
      r = col[2][0];
      g = col[2][1];
      b = col[2][2];
      if (dy > 5 && dy < 12 && abs(dx) < 9 && ((ux + vy) & 3) == 0) // dientes
        r = g = b = 230;
      if (d2 <= 30)
      {
        r = g = b = 240;
        l = 255;
        if (d2 <= 8)
        {
          r = 30;
          g = 210;
          b = 40;
        }
      }
    }
  }
  else // humanoide: cabeza, cuerpo, brazos y dos patas
  {
    bool cab = ((ux - 16) * (ux - 16) + (vy - 7) * (vy - 7)) <= 28;
    bool cuerpo = (ux >= 10 && ux <= 21 && vy >= 12) && !(vy > 24 && (ux == 15 || ux == 16));
    bool brazo = (vy >= 13 && vy <= 24) && ((ux >= 5 && ux <= 9) || (ux >= 22 && ux <= 26));
    if (cab || cuerpo || brazo)
    {
      on = true;
      l = 235 - abs(ux - 16) * 5 + ((ux * 5 + vy * 3) & 15);
      r = col[tipo][0];
      g = col[tipo][1];
      b = col[tipo][2];
      if (cab && vy == 6 && (ux == 14 || ux == 18))
      {
        r = 255;
        g = 235;
        b = 40;
        l = 255;
      }
      if (tipo == 0 && vy < 4 && (ux == 13 || ux == 19)) // cuernos
        r = g = b = 210;
    }
  }
  if (!on)
    return 0;
  if (blanco)
    r = g = b = 255;
  else if (sangre)
  {
    r = 210;
    g = 25;
    b = 10;
  }
  if (l > 255)
    l = 255;
  if (l < 0)
    l = 0;
  r = (r * l >> 8) * sombra >> 8;
  g = (g * l >> 8) * sombra >> 8;
  b = (b * l >> 8) * sombra >> 8;
  uint16_t c = DM_RGB(r, g, b);
  return c ? c : 1;
}

// Dibuja todo el fotograma, columna a columna
static void dmRender(uint32_t now)
{
  DmMem &d = *dmM;
  float dx = cosf(d.ang), dy = sinf(d.ang);
  float plx = -dy * 0.66f, ply = dx * 0.66f;

  // Demonios visibles, proyectados y ordenados del más lejano al más cercano
  DmSpr sp[DM_NEN];
  int ns = 0;
  float inv = 1.0f / (plx * dy - dx * ply);
  for (int i = 0; i < DM_NEN; i++)
  {
    DmEn &e = d.en[i];
    if (e.estado == 2)
      continue;
    float ex = e.x - d.px, ey = e.y - d.py;
    float tx = inv * (dy * ex - dx * ey);
    float ty = inv * (-ply * ex + plx * ey);
    if (ty < 0.25f)
      continue;
    int hh = (int)(DM_H * 0.8f / ty);
    float suelo = DM_H / 2 + DM_H * 0.5f / ty;
    if (e.estado == 1) // cayendo: se encoge mientras se pone rojo
    {
      int resta = (int)(e.t - now);
      if (resta < 0)
        resta = 0;
      if (resta > 350)
        resta = 350;
      hh = (int)(hh * (0.35f + 0.65f * resta / 350.0f));
    }
    int wc = hh / DM_COLW;
    int sx = (int)((DM_COLS / 2) * (1.0f + tx / ty));
    if (wc < 2 || hh < 2 || sx + wc / 2 <= 0 || sx - wc / 2 >= DM_COLS)
      continue;
    DmSpr s;
    s.i = i;
    s.tipo = i % 3;
    int sh = 255 - (int)(ty * 22.0f);
    s.sombra = sh < 50 ? 50 : sh;
    s.blanco = (int32_t)(e.flash - now) > 0;
    s.sangre = e.estado == 1;
    s.cl = sx - wc / 2;
    s.cr = s.cl + wc;
    s.h = hh;
    s.top = (int)suelo - hh;
    s.dist = ty;
    int k = ns++;
    while (k > 0 && sp[k - 1].dist < s.dist) // inserción: lejos primero
    {
      sp[k] = sp[k - 1];
      k--;
    }
    sp[k] = s;
  }

  int bob = (int)(sinf(d.paso) * 3.0f);
  int gTop = DM_H - 58 + (bob < 0 ? -bob : bob) + ((int32_t)(d.armaHasta - now) > 0 ? 7 : 0);
  bool fuego = (int32_t)(d.armaHasta - now) > 0;

  tft.startWrite();
  for (int c = 0; c < DM_COLS; c++)
  {
    float cam = 2.0f * c / DM_COLS - 1.0f;
    float rdx = dx + plx * cam, rdy = dy + ply * cam;
    int mx = (int)d.px, my = (int)d.py;
    float ddx = (rdx == 0) ? 1e30f : fabsf(1.0f / rdx);
    float ddy = (rdy == 0) ? 1e30f : fabsf(1.0f / rdy);
    int stx, sty;
    float sdx, sdy;
    if (rdx < 0)
    {
      stx = -1;
      sdx = (d.px - mx) * ddx;
    }
    else
    {
      stx = 1;
      sdx = (mx + 1.0f - d.px) * ddx;
    }
    if (rdy < 0)
    {
      sty = -1;
      sdy = (d.py - my) * ddy;
    }
    else
    {
      sty = 1;
      sdy = (my + 1.0f - d.py) * ddy;
    }
    int lado = 0, tipo = 1;
    for (int it = 0; it < 32; it++)
    {
      if (sdx < sdy)
      {
        sdx += ddx;
        mx += stx;
        lado = 0;
      }
      else
      {
        sdy += ddy;
        my += sty;
        lado = 1;
      }
      if (DM_MAP[my][mx])
      {
        tipo = DM_MAP[my][mx];
        break;
      }
    }
    float perp = (lado == 0) ? sdx - ddx : sdy - ddy;
    if (perp < 0.1f)
      perp = 0.1f;
    float wx = (lado == 0) ? d.py + perp * rdy : d.px + perp * rdx;
    wx -= floorf(wx);
    int tx = (int)(wx * 16.0f) & 15;
    int lineH = (int)(DM_H / perp);
    if (lineH < 1)
      lineH = 1;
    int top = DM_H / 2 - lineH / 2;
    int bot = top + lineH;
    int sh = 255 - (int)(perp * 26.0f);
    if (sh < 30)
      sh = 30;
    if (lado == 1)
      sh = sh * 3 / 4;

    int y0 = top < 0 ? 0 : top;
    int y1 = bot > DM_H ? DM_H : bot;
    for (int y = 0; y < y0; y++)
      d.col[y] = d.techo[y];
    uint32_t step = (16u << 16) / (uint32_t)lineH;
    uint32_t tp = (uint32_t)(y0 - top) * step;
    const uint8_t *tex = d.tex[tipo - 1];
    const uint16_t *pl = d.pal[tipo - 1];
    for (int y = y0; y < y1; y++)
    {
      int lv = (tex[((tp >> 16) & 15) * 16 + tx] * sh) >> 9;
      tp += step;
      if (lv > 63)
        lv = 63;
      uint32_t cc = pl[lv];
      d.col[y] = cc | (cc << 16);
    }
    for (int y = y1; y < DM_H; y++)
      d.col[y] = d.suelo[y];

    // Demonios por delante de la pared
    for (int k = 0; k < ns; k++)
    {
      DmSpr &s = sp[k];
      if (c < s.cl || c >= s.cr || s.dist >= perp)
        continue;
      int ux = ((c - s.cl) * 32) / (s.cr - s.cl);
      int ys = s.top < 0 ? 0 : s.top;
      int ye = s.top + s.h;
      if (ye > DM_H)
        ye = DM_H;
      int vstep = (32 << 8) / s.h;
      int va = (ys - s.top) * vstep;
      for (int y = ys; y < ye; y++, va += vstep)
      {
        int vy = va >> 8;
        uint16_t cc = dmSprPix(s.tipo, ux, vy > 31 ? 31 : vy, s.sombra, s.blanco, s.sangre);
        if (cc)
          d.col[y] = cc | ((uint32_t)cc << 16);
      }
    }

    // Arma abajo en el centro, con fogonazo al disparar
    int gx = c - DM_COLS / 2;
    if (gx > -13 && gx < 13)
    {
      int ax = gx < 0 ? -gx : gx;
      for (int y = gTop; y < DM_H; y++)
      {
        int mitad = 2 + (y - gTop) / 7;
        if (ax <= mitad)
        {
          int g = 60 + (13 - ax) * 6 - ((y - gTop) < 3 ? 25 : 0);
          if (ax == mitad)
            g = 28;
          uint16_t cc = DM_RGB(g, g, g + 12);
          d.col[y] = cc | ((uint32_t)cc << 16);
        }
      }
      if (fuego)
        for (int k = 1; k < 24; k++)
        {
          int hw = (k < 24 - k ? k : 24 - k) * 10 / 12;
          int y = gTop - k;
          if (y >= 0 && ax <= hw)
          {
            uint16_t cc = (ax <= hw / 2) ? DM_RGB(255, 235, 90) : DM_RGB(255, 140, 20);
            d.col[y] = cc | ((uint32_t)cc << 16);
          }
        }
    }

    tft.setAddrWindow(c * DM_COLW, 0, DM_COLW, DM_H);
    tft.pushColors((uint16_t *)d.col, DM_H * DM_COLW, true);
  }
  tft.endWrite();
}
// <<< DOOM MOTOR

void esp32_2432S028R_DoomScreen(unsigned long mElapsed)
{
  mineria = getMiningData(mElapsed);
  Serial.printf("M8AX - %s >>> Completados %s Share(s), %s Khashes, Prom. Hashrate %s KH/s %s°\n",
                mineria.currentTime.c_str(), mineria.completedShares.c_str(), mineria.totalKHashes.c_str(), mineria.currentHashRate.c_str(), mineria.temp.c_str());

  bool redraw = hasChangedScreen;
  if (!dmM)
  {
    dmM = (DmMem *)malloc(sizeof(DmMem));
    if (!dmM)
    {
      Serial.println("M8AX - Doom: No Hay RAM Libre");
      delay(1000);
      return;
    }
    dmInit();
    redraw = true;
  }
  const int dmMiPantalla = currentDisplayDriver->current_cyclic_screen;
  if (redraw)
    tft.fillRect(0, 0, 320, 170, TFT_BLACK);
  printPoolData(); // barra de abajo, solo se repinta cuando toca
  hasChangedScreen = false;

  char hashStr[16];
  snprintf(hashStr, sizeof(hashStr), "%s", mineria.currentHashRate.c_str());
  char izq[24], der[24];
  char lastIzq[24] = "", lastDer[24] = "";

  unsigned long start = millis();
  unsigned long tFps = start;
  unsigned long lastHud = 0;
  unsigned long lastMiningUpdate = 0;
  unsigned long lastTouchChk = 0;
  unsigned long ledApagarEn = 0;
  uint32_t frames = 0;
  static int fps = 0;
  dmM->ultimo = start;
  pongLeds(false);

  while (millis() - start < DM_SLICE_MS)
  {
    unsigned long now = millis();

    // ---- Táctil (misma lógica que el Pong) ----
    if (now - lastTouchChk >= 40 && now - previousTouchMillis >= 500)
    {
      lastTouchChk = now;
      int16_t t_x, t_y;
      if (toqueXY(t_x, t_y))
      {
        previousTouchMillis = now;
        if (((t_x > 109) && (t_x < 211)) && ((t_y > 185) && (t_y < 241)))
        {
          bottomScreenBlue ^= true;
          hasChangedScreen = true;
        }
        else if ((t_x > 235) && ((t_y > 0) && (t_y < 16)))
        {
          esp32_2432S028R_AlternateScreenState();
        }
        else if (t_x > 160)
        {
          currentDisplayDriver->current_cyclic_screen = (currentDisplayDriver->current_cyclic_screen + 1) % currentDisplayDriver->num_cyclic_screens;
        }
        else
        {
          currentDisplayDriver->current_cyclic_screen = currentDisplayDriver->current_cyclic_screen - 1;
          if (currentDisplayDriver->current_cyclic_screen < 0)
            currentDisplayDriver->current_cyclic_screen = currentDisplayDriver->num_cyclic_screens - 1;
        }
        break;
      }
    }

    // ---- LEDs: apagado del destello que toque ----
    if (ledApagarEn && now >= ledApagarEn)
    {
      pongLeds(false);
      ledApagarEn = 0;
    }

    // ---- La CPU juega un paso ----
    float dt = (now - dmM->ultimo) / 1000.0f;
    if (dt > 0.1f)
      dt = 0.1f;
    dmM->ultimo = now;
    dmUpdate(now, dt);

    // ---- LEDs: blanco al matar, verde al llegar a un punto, rojo al disparar ----
    if (dmM->evKill)
    {
      pongLeds(true);
      ledApagarEn = now + 90;
    }
    else if (dmM->evWp)
    {
      digitalWrite(LED_PIN, HIGH);
      digitalWrite(LED_PIN_G, LOW);
      digitalWrite(LED_PIN_B, HIGH);
      ledApagarEn = now + 70;
    }
    else if (dmM->evDisparo)
    {
      digitalWrite(LED_PIN, LOW);
      digitalWrite(LED_PIN_G, HIGH);
      digitalWrite(LED_PIN_B, HIGH);
      ledApagarEn = now + 35;
    }
    dmM->evKill = dmM->evWp = dmM->evDisparo = 0;

    dmRender(now);
    frames++;

    now = millis();
    if (now - tFps >= 1000)
    {
      fps = (int)(frames * 1000UL / (now - tFps));
      frames = 0;
      tFps = now;
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
      snprintf(izq, sizeof(izq), "M8AX.DOOM %dFPS K:%u", fps, (unsigned)dmM->kills);
      snprintf(der, sizeof(der), "%s KH/s", hashStr);
      if (strcmp(izq, lastIzq) != 0 || strcmp(der, lastDer) != 0)
      {
        tft.setFreeFont(NULL);
        tft.setTextFont(1);
        tft.setTextSize(1);
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        tft.setTextPadding(150);
        tft.setTextDatum(TL_DATUM);
        tft.drawString(izq, 4, 158);
        tft.setTextDatum(TR_DATUM);
        tft.drawString(der, 316, 158);
        tft.setTextPadding(0);
        tft.setTextDatum(TL_DATUM);
        tft.setTextSize(2);
        strcpy(lastIzq, izq);
        strcpy(lastDer, der);
      }
    }
    webAtender();
    delay(1);
  }

  pongLeds(false);
  tft.setTextPadding(0);
  tft.setTextDatum(TL_DATUM);

  if (currentDisplayDriver->current_cyclic_screen != dmMiPantalla)
  {
    free(dmM);
    dmM = NULL;
  }

  cuentita++;
  if (cuentita == 15)
  {
    ajustarZonaHoraria();
    obtenerLocYTemp();
    obtenerVariacionesBTC(variacion1dia, variacion1semana);
  }
}

/****************************************************************************************************************
 *
 *   Escrito por: M8AX
 *
 *   Descripción: ASTEROIDES M8AX - Los asteroides de toda la vida, pero jugados por la CPU del ESP32 para ESP32_2432S028R
 *   --------------------------------------------------------------------------------------------------------------------
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
 *   LEDs: destello ROJO corto al disparar, VERDE al romper una roca y los TRES a la vez si destruyen la nave.
 *   Abajo van los puntos, la ola y las vidas. Si la nave pierde las tres vidas, empieza otra partida sola.
 *
 *   Memoria: unos 22 KB en el heap ( casi todo es el buffer de pantalla ), que se piden al entrar en la pantalla
 *   y se liberan al salir a otra, como el ajedrez y el Doom. Si no hay RAM, se salta la pantalla sin pintar nada.
 *
 *   Para activarla: pegar este bloque justo antes de analiCadaSegundo, meter esp32_2432S028R_AsteroidesScreen en el
 *   array esp32_2432S028RCyclicScreens[] y añadir en analiCadaSegundo, debajo del if de galM, este otro:
 *   if (astM && pantallaActual != esp32_2432S028R_AsteroidesScreen) { free(astM); astM = NULL; }
 *
 ****************************************************************************************************************/

// >>> ASTEROIDES MOTOR
#define AST_W 320
#define AST_H 120
#define AST_Y0 28 // el HUD de arriba ocupa hasta la 26 y el de abajo empieza en la 150
#define AST_FRAME_MS 20
#define AST_SLICE_MS 5000
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

static void astLed(uint8_t prio, bool r, bool g, bool b, uint16_t ms, uint32_t now)
{
  if (astM->ledOff && prio < astM->ledPrio)
    return;
  digitalWrite(LED_PIN, r ? LOW : HIGH);
  digitalWrite(LED_PIN_G, g ? LOW : HIGH);
  digitalWrite(LED_PIN_B, b ? LOW : HIGH);
  astM->ledOff = now + ms;
  astM->ledPrio = prio;
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

void esp32_2432S028R_AsteroidesScreen(unsigned long mElapsed)
{
  mineria = getMiningData(mElapsed);
  Serial.printf("M8AX - %s >>> Completados %s Share(s), %s Khashes, Prom. Hashrate %s KH/s %s°\n",
                mineria.currentTime.c_str(), mineria.completedShares.c_str(), mineria.totalKHashes.c_str(), mineria.currentHashRate.c_str(), mineria.temp.c_str());

  bool redraw = hasChangedScreen;
  if (!astM)
  {
    astM = (AstMem *)malloc(sizeof(AstMem));
    if (!astM)
    {
      Serial.println("M8AX - Asteroides: No Hay RAM Libre");
      delay(1000);
      return;
    }
    astInit();
    redraw = true;
  }
  const int astMiPantalla = currentDisplayDriver->current_cyclic_screen;
  if (redraw)
  {
    tft.fillRect(0, 0, 320, 170, TFT_BLACK);
    astEmpujar();
    astTextos(true);
  }
  printPoolData(); // barra de abajo, solo se repinta cuando toca
  hasChangedScreen = false;

  char horaStr[9] = "";
  char lastHora[9] = "";
  char hashStr[16];
  char lastHash[16] = "";

  unsigned long start = millis();
  unsigned long lastMiningUpdate = 0;
  unsigned long lastTouchChk = 0;
  unsigned long siguiente = start;
  pongLeds(false);
  astM->ledOff = 0;

  while (millis() - start < AST_SLICE_MS)
  {
    unsigned long now = millis();

    // ---- Táctil (misma lógica que el Pong) ----
    if (now - lastTouchChk >= 40 && now - previousTouchMillis >= 500)
    {
      lastTouchChk = now;
      int16_t t_x, t_y;
      if (toqueXY(t_x, t_y))
      {
        previousTouchMillis = now;
        if (((t_x > 109) && (t_x < 211)) && ((t_y > 185) && (t_y < 241)))
        {
          bottomScreenBlue ^= true;
          hasChangedScreen = true;
        }
        else if ((t_x > 235) && ((t_y > 0) && (t_y < 16)))
        {
          esp32_2432S028R_AlternateScreenState();
        }
        else if (t_x > 160)
        {
          currentDisplayDriver->current_cyclic_screen = (currentDisplayDriver->current_cyclic_screen + 1) % currentDisplayDriver->num_cyclic_screens;
        }
        else
        {
          currentDisplayDriver->current_cyclic_screen = currentDisplayDriver->current_cyclic_screen - 1;
          if (currentDisplayDriver->current_cyclic_screen < 0)
            currentDisplayDriver->current_cyclic_screen = currentDisplayDriver->num_cyclic_screens - 1;
        }
        break;
      }
    }
    if (astM->ledOff && now >= astM->ledOff)
    {
      pongLeds(false);
      astM->ledOff = 0;
      astM->ledPrio = 0;
    }

    if (now >= siguiente)
    {
      astFrame(now);
      astTextos(false);
      siguiente = millis() + AST_FRAME_MS;
    }

    // ---- Hora (izquierda) y hashrate (derecha), arriba como en las demás ----
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
        pongDrawHud(hashStr, horaStr);
        strcpy(lastHora, horaStr);
        strcpy(lastHash, hashStr);
      }
    }
    webAtender();
    delay(2);
  }

  pongLeds(false);
  tft.setTextPadding(0);
  tft.setTextDatum(TL_DATUM);

  if (currentDisplayDriver->current_cyclic_screen != astMiPantalla)
  {
    free(astM);
    astM = NULL;
  }

  cuentita++;
  if (cuentita == 15)
  {
    ajustarZonaHoraria();
    obtenerLocYTemp();
    obtenerVariacionesBTC(variacion1dia, variacion1semana);
  }
}

/****************************************************************************************************************
 *
 *   Escrito por: M8AX
 *
 *   Descripción: ATRACTORES M8AX - Atractores extraños, matemáticas a lo bruto para ESP32_2432S028R
 *   --------------------------------------------------------------------------------------------------
 *
 *   Se coge un punto, se le aplica una fórmula con senos y cosenos, sale otro punto, se le aplica la misma
 *   fórmula... y así millones de veces. Cada punto se apunta en un mapa de densidad, y lo que va saliendo, si los
 *   parámetros tienen suerte, son figuras con una pinta increíble. Hay tres familias: Clifford, De Jong y
 *   Svensson. Los parámetros se sacan al azar, y como la mayoría no dan nada ( se quedan en un punto o en
 *   una vuelta que se repite ), cada atractor se comprueba contando cuántas casillas del mapa llena, y si
 *   sale pobre se tira y se prueba otro. Los buenos duran 24 segundos y se pasa al siguiente.
 *
 *   El ESP32 no hace otra cosa que iterar todo el tiempo que puede y repintar unas 6 veces por segundo. Abajo
 *   se ve el nombre del atractor, cuántos miles de puntos calcula por segundo y los millones que lleva. El
 *   color depende de cuántas veces ha pasado un punto por cada casilla, con escala logarítmica, y cada
 *   atractor cambia de tono. Cuando una casilla se llena, todo el mapa se divide a la mitad para no perder detalle.
 *
 *   LEDs: los tres destellan un momento cada vez que arranca un atractor nuevo.
 *
 *   Memoria: unos 10 KB en el heap ( casi todo es el mapa de densidad ), que se piden al entrar en la pantalla
 *   y se liberan al salir a otra. Si no hay RAM, se salta la pantalla sin pintar nada.
 *
 *   Para activarla: pegar este bloque justo antes de analiCadaSegundo, meter esp32_2432S028R_AtractoresScreen en el
 *   array esp32_2432S028RCyclicScreens[] y añadir en analiCadaSegundo, debajo del if de astM, este otro:
 *   if (atrM && pantallaActual != esp32_2432S028R_AtractoresScreen) { free(atrM); atrM = NULL; }
 *
 ****************************************************************************************************************/

// >>> ATRACTORES MOTOR
#define ATR_W 160
#define ATR_H 60
#define ATR_Y0 28 // el HUD de arriba ocupa hasta la 26 y el de abajo empieza en la 150
#define ATR_SLICE_MS 5000
#define ATR_REFRESCO_MS 160
#define ATR_CAMBIO_MS 24000
#define ATR_LOTE 256
#define ATR_MEDIDA 12000 // puntos que se miran para saber el tamaño del atractor
#define ATR_PRUEBA 30000 // puntos tras los cuales se decide si merece la pena
#define ATR_MIN_CASILLAS 450
#define ATR_ESTIRA 1.6f

struct AtrMem
{
  uint8_t d[ATR_W * ATR_H];
  uint16_t lut[256];
  uint16_t linea[ATR_W * 2];
  float x, y, a, b, c, e;
  float mnx, mxx, mny, mxy, sx, sy;
  uint8_t tipo, fase, llena, validado, nuevo;
  uint32_t nMed, nAcu, iter, iterPrev, tCambio, tPush, tSeg, vel, ledOff;
  char tNom[12], tVel[20], tIt[20];
};
static AtrMem *atrM = NULL;

static inline float atrRf() { return (esp_random() & 0xFFFF) * (1.0f / 65535.0f); }

static void atrColores(float tono)
{
  AtrMem &m = *atrM;
  for (int i = 0; i < 256; i++)
  {
    float t = logf(1.0f + i) / logf(256.0f);
    float h = tono + 70.0f * t, s = 1.0f - 0.85f * t * t * t, v = t * 1.5f;
    if (v > 1.0f)
      v = 1.0f;
    h = fmodf(h, 360.0f) / 60.0f;
    int k = (int)h;
    float f = h - k, p = v * (1.0f - s), q = v * (1.0f - s * f), u = v * (1.0f - s * (1.0f - f)), r, g, b;
    switch (k)
    {
    case 0:
      r = v;
      g = u;
      b = p;
      break;
    case 1:
      r = q;
      g = v;
      b = p;
      break;
    case 2:
      r = p;
      g = v;
      b = u;
      break;
    case 3:
      r = p;
      g = q;
      b = v;
      break;
    case 4:
      r = u;
      g = p;
      b = v;
      break;
    default:
      r = v;
      g = p;
      b = q;
      break;
    }
    m.lut[i] = (uint16_t)((((int)(r * 255) & 0xF8) << 8) | (((int)(g * 255) & 0xFC) << 3) | ((int)(b * 255) >> 3));
  }
}

// elige tipo y parámetros nuevos y vuelve a empezar desde cero
static void atrNuevo()
{
  AtrMem &m = *atrM;
  m.tipo = (m.tipo + 1 + esp_random() % 2) % 3;
  switch (m.tipo)
  {
  case 0: // Clifford
    m.a = (atrRf() - 0.5f) * 4.4f;
    m.b = (atrRf() - 0.5f) * 4.4f;
    m.c = (atrRf() - 0.5f) * 4.4f;
    m.e = (atrRf() - 0.5f) * 4.4f;
    strcpy(m.tNom, "Clifford");
    break;
  case 1: // De Jong
    m.a = (atrRf() - 0.5f) * 6.0f;
    m.b = (atrRf() - 0.5f) * 6.0f;
    m.c = (atrRf() - 0.5f) * 6.0f;
    m.e = (atrRf() - 0.5f) * 6.0f;
    strcpy(m.tNom, "De Jong");
    break;
  default: // Svensson
    m.a = (atrRf() - 0.5f) * 6.0f;
    m.b = (atrRf() - 0.5f) * 6.0f;
    m.c = 0.5f + atrRf();
    m.e = 0.5f + atrRf();
    strcpy(m.tNom, "Svensson");
    break;
  }
  m.x = atrRf() - 0.5f;
  m.y = atrRf() - 0.5f;
  m.mnx = m.mny = 1e9f;
  m.mxx = m.mxy = -1e9f;
  m.fase = 0;
  m.llena = 0;
  m.validado = 0;
  m.nMed = 0;
  m.nAcu = 0;
  m.iter = 0;
  m.iterPrev = 0;
  m.nuevo = 1;
  memset(m.d, 0, sizeof(m.d));
  atrColores(atrRf() * 360.0f);
  Serial.printf("M8AX - Atractor: %s A:%.3f B:%.3f C:%.3f D:%.3f\n", m.tNom, m.a, m.b, m.c, m.e);
}

static void atrInit()
{
  AtrMem &m = *atrM;
  memset(m.d, 0, sizeof(m.d));
  m.tipo = esp_random() % 3;
  m.tPush = 0;
  m.tSeg = 0;
  m.vel = 0;
  m.ledOff = 0;
  m.tNom[0] = m.tVel[0] = m.tIt[0] = 0;
  atrNuevo();
  m.tCambio = millis();
}

// aplica la fórmula del atractor a (x,y) y deja el resultado en (nx,ny)
#define ATR_PASO(nx, ny)                \
  if (t == 0)                           \
  {                                     \
    nx = sinf(a * y) + c * cosf(a * x); \
    ny = sinf(b * x) + e * cosf(b * y); \
  }                                     \
  else if (t == 1)                      \
  {                                     \
    nx = sinf(a * y) - cosf(b * x);     \
    ny = sinf(c * x) - cosf(e * y);     \
  }                                     \
  else                                  \
  {                                     \
    nx = e * sinf(a * x) - sinf(b * y); \
    ny = c * cosf(a * x) + cosf(b * y); \
  }

static void atrLote(int n)
{
  AtrMem &m = *atrM;
  float x = m.x, y = m.y, nx, ny;
  const float a = m.a, b = m.b, c = m.c, e = m.e;
  const int t = m.tipo;
  if (m.fase == 0)
  {
    for (int i = 0; i < n; i++)
    {
      ATR_PASO(nx, ny)
      x = nx;
      y = ny;
      if (m.nMed + i < 200) // los primeros puntos todavía no están en el atractor
        continue;
      if (x < m.mnx)
        m.mnx = x;
      if (x > m.mxx)
        m.mxx = x;
      if (y < m.mny)
        m.mny = y;
      if (y > m.mxy)
        m.mxy = y;
    }
    m.nMed += n;
  }
  else
  {
    const float mnx = m.mnx, mny = m.mny, sx = m.sx, sy = m.sy;
    uint8_t *d = m.d;
    for (int i = 0; i < n; i++)
    {
      ATR_PASO(nx, ny)
      x = nx;
      y = ny;
      int cx = (int)((x - mnx) * sx), cy = (int)((y - mny) * sy);
      if ((unsigned)cx < ATR_W && (unsigned)cy < ATR_H)
      {
        uint8_t *p = &d[cy * ATR_W + cx];
        if (*p < 255)
          (*p)++;
        else
          m.llena = 1;
      }
    }
    m.nAcu += n;
  }
  m.x = x;
  m.y = y;
  m.iter += n;
}

static void atrTrabajar(uint32_t us)
{
  AtrMem &m = *atrM;
  uint32_t t0 = micros();
  while (micros() - t0 < us)
  {
    atrLote(ATR_LOTE);
    if (m.fase == 0)
    {
      if (m.nMed >= ATR_MEDIDA)
      {
        float dx = m.mxx - m.mnx, dy = m.mxy - m.mny;
        if (!(dx > 0.3f && dy > 0.3f) || !(dx < 1e6f && dy < 1e6f))
        {
          atrNuevo();
          return;
        }
        // un poco de margen, y el eje x se estira para aprovechar que la pantalla es ancha
        float sy = (ATR_H - 2) / dy, sx = (ATR_W - 2) / dx;
        if (sx > sy * ATR_ESTIRA)
          sx = sy * ATR_ESTIRA;
        m.mnx -= ((ATR_W - 1) / sx - dx) * 0.5f;
        m.mny -= ((ATR_H - 1) / sy - dy) * 0.5f;
        m.sx = sx;
        m.sy = sy;
        memset(m.d, 0, sizeof(m.d));
        m.fase = 1;
      }
    }
    else
    {
      if (m.llena)
      {
        for (int i = 0; i < ATR_W * ATR_H; i++)
          m.d[i] >>= 1;
        m.llena = 0;
      }
      if (!m.validado && m.nAcu >= ATR_PRUEBA)
      {
        int casillas = 0;
        for (int i = 0; i < ATR_W * ATR_H; i++)
          if (m.d[i])
            casillas++;
        if (casillas < ATR_MIN_CASILLAS)
        {
          atrNuevo();
          return;
        }
        m.validado = 1;
        m.tCambio = millis();
      }
    }
  }
}

static void atrPintar()
{
  AtrMem &m = *atrM;
  tft.startWrite();
  tft.setAddrWindow(0, ATR_Y0, ATR_W * 2, ATR_H * 2);
  for (int y = 0; y < ATR_H; y++)
  {
    const uint8_t *s = &m.d[y * ATR_W];
    uint16_t *l = m.linea;
    for (int x = 0; x < ATR_W; x++)
    {
      uint16_t c = m.lut[s[x]];
      l[0] = c;
      l[1] = c;
      l += 2;
    }
    tft.pushColors(m.linea, ATR_W * 2, true);
    tft.pushColors(m.linea, ATR_W * 2, true);
  }
  tft.endWrite();
}

// los tres textos de abajo, solo se repintan si cambian
static void atrTextos(bool forzar)
{
  AtrMem &m = *atrM;
  char vel[20], it[20];
  snprintf(vel, sizeof(vel), "%lu K/s", (unsigned long)m.vel);
  snprintf(it, sizeof(it), "%lu.%02lu M", (unsigned long)(m.iter / 1000000UL), (unsigned long)((m.iter / 10000UL) % 100UL));
  bool a = forzar || m.nuevo, b = forzar || strcmp(vel, m.tVel) != 0, c = forzar || strcmp(it, m.tIt) != 0;
  if (!a && !b && !c)
    return;
  tft.setFreeFont(NULL);
  tft.setTextFont(1);
  tft.setTextSize(2);
  if (a)
  {
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.setTextPadding(110);
    tft.setTextDatum(TL_DATUM);
    tft.drawString(m.tNom, 4, 151);
    m.nuevo = 0;
  }
  if (b)
  {
    tft.setTextColor(TFT_CYAN, TFT_BLACK);
    tft.setTextPadding(84);
    tft.setTextDatum(TC_DATUM);
    tft.drawString(vel, 160, 151);
    strcpy(m.tVel, vel);
  }
  if (c)
  {
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.setTextPadding(110);
    tft.setTextDatum(TR_DATUM);
    tft.drawString(it, 316, 151);
    strcpy(m.tIt, it);
  }
  tft.setTextPadding(0);
  tft.setTextDatum(TL_DATUM);
}
// <<< ATRACTORES MOTOR

void esp32_2432S028R_AtractoresScreen(unsigned long mElapsed)
{
  mineria = getMiningData(mElapsed);
  Serial.printf("M8AX - %s >>> Completados %s Share(s), %s Khashes, Prom. Hashrate %s KH/s %s°\n",
                mineria.currentTime.c_str(), mineria.completedShares.c_str(), mineria.totalKHashes.c_str(), mineria.currentHashRate.c_str(), mineria.temp.c_str());

  bool redraw = hasChangedScreen;
  if (!atrM)
  {
    atrM = (AtrMem *)malloc(sizeof(AtrMem));
    if (!atrM)
    {
      Serial.println("M8AX - Atractores: No Hay RAM Libre");
      delay(1000);
      return;
    }
    atrInit();
    redraw = true;
  }
  const int atrMiPantalla = currentDisplayDriver->current_cyclic_screen;
  if (redraw)
  {
    tft.fillRect(0, 0, 320, 170, TFT_BLACK);
    atrPintar();
    atrTextos(true);
  }
  printPoolData(); // barra de abajo, solo se repinta cuando toca
  hasChangedScreen = false;

  char horaStr[9] = "";
  char lastHora[9] = "";
  char hashStr[16];
  char lastHash[16] = "";

  unsigned long start = millis();
  unsigned long lastMiningUpdate = 0;
  unsigned long lastTouchChk = 0;
  pongLeds(false);
  atrM->ledOff = 0;

  while (millis() - start < ATR_SLICE_MS)
  {
    unsigned long now = millis();

    // ---- Táctil (misma lógica que el Pong) ----
    if (now - lastTouchChk >= 40 && now - previousTouchMillis >= 500)
    {
      lastTouchChk = now;
      int16_t t_x, t_y;
      if (toqueXY(t_x, t_y))
      {
        previousTouchMillis = now;
        if (((t_x > 109) && (t_x < 211)) && ((t_y > 185) && (t_y < 241)))
        {
          bottomScreenBlue ^= true;
          hasChangedScreen = true;
        }
        else if ((t_x > 235) && ((t_y > 0) && (t_y < 16)))
        {
          esp32_2432S028R_AlternateScreenState();
        }
        else if (t_x > 160)
        {
          currentDisplayDriver->current_cyclic_screen = (currentDisplayDriver->current_cyclic_screen + 1) % currentDisplayDriver->num_cyclic_screens;
        }
        else
        {
          currentDisplayDriver->current_cyclic_screen = currentDisplayDriver->current_cyclic_screen - 1;
          if (currentDisplayDriver->current_cyclic_screen < 0)
            currentDisplayDriver->current_cyclic_screen = currentDisplayDriver->num_cyclic_screens - 1;
        }
        break;
      }
    }
    if (atrM->ledOff && now >= atrM->ledOff)
    {
      pongLeds(false);
      atrM->ledOff = 0;
    }

    // ---- A calcular, que para eso estamos ----
    atrTrabajar(8000);
    if (atrM->validado && millis() - atrM->tCambio >= ATR_CAMBIO_MS)
    {
      atrNuevo();
      digitalWrite(LED_PIN, LOW);
      digitalWrite(LED_PIN_G, LOW);
      digitalWrite(LED_PIN_B, LOW);
      atrM->ledOff = now + 120;
    }
    if (now - atrM->tSeg >= 1000)
    {
      atrM->vel = (atrM->iter - atrM->iterPrev) / 1000UL;
      atrM->iterPrev = atrM->iter;
      atrM->tSeg = now;
    }
    if (now - atrM->tPush >= ATR_REFRESCO_MS)
    {
      atrM->tPush = now;
      atrPintar();
      atrTextos(false);
    }

    // ---- Hora (izquierda) y hashrate (derecha), arriba como en las demás ----
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
        pongDrawHud(hashStr, horaStr);
        strcpy(lastHora, horaStr);
        strcpy(lastHash, hashStr);
      }
    }
    webAtender();
    delay(1);
  }

  pongLeds(false);
  tft.setTextPadding(0);
  tft.setTextDatum(TL_DATUM);

  if (currentDisplayDriver->current_cyclic_screen != atrMiPantalla)
  {
    free(atrM);
    atrM = NULL;
  }

  cuentita++;
  if (cuentita == 15)
  {
    ajustarZonaHoraria();
    obtenerLocYTemp();
    obtenerVariacionesBTC(variacion1dia, variacion1semana);
  }
}

/****************************************************************************************************************
 *
 *   Escrito por: M8AX
 *
 *   Descripción: EL DADO Y LA MONEDA DE LA SUERTE - Pantalla para ESP32_2432S028R
 *   -------------------------------------------------------------------------------
 *
 *   La de siempre del T-Display, adaptada a la 2432S028R: cada vez que se pinta tira un dado ( 1 a 6 ) y una
 *   moneda ( cara o cruz ) y los dibuja encima de una imagen de fondo que cambia sola cada 30 segundos entre
 *   cuatro. También muestra el hashrate, la hora, la temperatura y el hashrate escrito con letras.
 *
 *   Es una pantalla de las antiguas: no tiene bucle propio. El táctil, el botón de abajo y el cambio automático
 *   cada minuto los hace esp32_2432S028R_DoLedStuff, así que ES LA MISMA para la versión normal y para la Auto.
 *
 *   Para activarla: pegar este bloque justo antes del array esp32_2432S028RCyclicScreens[] y meter
 *   esp32_2432S028R_monedaYdado en ese array.
 *
 ****************************************************************************************************************/

// Dado de 60x60 centrado en ( x, y ) con los puntos según el número
void dibujarDado(int numero, int x, int y)
{
  tft.drawRect(x - 30, y - 30, 60, 60, TFT_WHITE);
  int r = 5; // radio de los puntos
  switch (numero)
  {
  case 1:
    tft.fillCircle(x, y, r, TFT_WHITE);
    break;
  case 2:
    tft.fillCircle(x - 15, y - 15, r, TFT_WHITE);
    tft.fillCircle(x + 15, y + 15, r, TFT_WHITE);
    break;
  case 3:
    tft.fillCircle(x, y, r, TFT_WHITE);
    tft.fillCircle(x - 15, y - 15, r, TFT_WHITE);
    tft.fillCircle(x + 15, y + 15, r, TFT_WHITE);
    break;
  case 4:
    tft.fillCircle(x - 15, y - 15, r, TFT_WHITE);
    tft.fillCircle(x + 15, y - 15, r, TFT_WHITE);
    tft.fillCircle(x - 15, y + 15, r, TFT_WHITE);
    tft.fillCircle(x + 15, y + 15, r, TFT_WHITE);
    break;
  case 5:
    tft.fillCircle(x, y, r, TFT_WHITE);
    tft.fillCircle(x - 15, y - 15, r, TFT_WHITE);
    tft.fillCircle(x + 15, y - 15, r, TFT_WHITE);
    tft.fillCircle(x - 15, y + 15, r, TFT_WHITE);
    tft.fillCircle(x + 15, y + 15, r, TFT_WHITE);
    break;
  case 6:
    tft.fillCircle(x - 15, y - 15, r, TFT_WHITE);
    tft.fillCircle(x + 15, y - 15, r, TFT_WHITE);
    tft.fillCircle(x - 15, y, r, TFT_WHITE);
    tft.fillCircle(x + 15, y, r, TFT_WHITE);
    tft.fillCircle(x - 15, y + 15, r, TFT_WHITE);
    tft.fillCircle(x + 15, y + 15, r, TFT_WHITE);
    break;
  }
}

// Moneda de radio 45 centrada en ( x, y ): 0 = cara ( C verde ), 1 = cruz ( X roja )
void dibujarMoneda(int moneda, int x, int y)
{
  int radio = 45;
  tft.drawCircle(x, y, radio, TFT_WHITE);

  tft.setFreeFont(NULL); // por si otra pantalla dejó una fuente libre puesta
  tft.setTextFont(1);    // la fuente 1 siempre está cargada
  tft.setTextSize(7);    // celda 42x56, pero la letra real ocupa 35x49

  // La letra mide 35x49 px, así que la colocamos a mano desde la esquina
  // superior izquierda para que quede exactamente en el centro del círculo
  tft.setCursor(x - 17, y - 24);

  if (moneda == 0)
  {
    tft.setTextColor(TFT_GREEN);
    tft.print("C");
  }
  else
  {
    tft.setTextColor(TFT_RED);
    tft.print("X");
  }

  tft.setTextSize(1);
}

void esp32_2432S028R_monedaYdado(unsigned long mElapsed)
{
  mineria = getMiningData(mElapsed);
  unsigned long epochTime = timeClient.getEpochTime();
  time_t epoch = (time_t)epochTime;
  struct tm *timeinfo = localtime(&epoch);
  char horaStr[9]; // HH:MM:SS
  snprintf(horaStr, sizeof(horaStr), "%02d:%02d:%02d", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
  int segundo = timeinfo->tm_sec;

  // Las pantallas antiguas se avisan entre sí para repetir lo que hacen una sola vez al entrar
  solounavez = 0;
  solouna = 0;

  Serial.printf("M8AX - %s >>> Completados %s Share(s), %s Khashes, Prom. Hashrate %s KH/s %s°\n",
                mineria.currentTime.c_str(), mineria.completedShares.c_str(), mineria.totalKHashes.c_str(), mineria.currentHashRate.c_str(), mineria.temp.c_str());

  // Imagen de fondo: al entrar la primera, y cada 30 segundos una al azar entre las cuatro
  static uint8_t fondoDado = 0;
  static int ultCambio = -1;
  if (hasChangedScreen)
    fondoDado = 0;
  if (segundo % 30 == 0)
  {
    if (segundo != ultCambio)
    {
      fondoDado = esp_random() % 4;
      ultCambio = segundo;
    }
  }
  else
    ultCambio = -1;

  switch (fondoDado)
  {
  case 0:
    tft.pushImage(0, 0, initWidth, initHeight, ImagenM8AX);
    break;
  case 1:
    tft.pushImage(0, 0, initWidth, initHeight, M8AXRelojLunar);
    break;
  case 2:
    tft.pushImage(0, 0, initWidth, initHeight, M8AXQuote1);
    break;
  default:
    tft.pushImage(0, 0, initWidth, initHeight, M8AXQuote2);
    break;
  }
  printPoolData(); // barra de abajo, solo se repinta cuando toca
  hasChangedScreen = false;

  int dado = esp_random() % 6 + 1; // del 1 al 6
  int moneda = esp_random() % 2;   // 0 cara, 1 cruz

  tft.setFreeFont(NULL);
  tft.setTextFont(1);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(1);
  tft.setCursor(108, 88);
  tft.print("DADO - ");
  tft.print(dado);
  tft.setCursor(108, 99);
  tft.print("MONEDA - ");
  tft.print(moneda == 0 ? "CARA" : "CRUZ");
  tft.setCursor(108, 110);
  tft.print(horaStr);
  tft.setCursor(108, 121);
  tft.print("TEMP - ");
  tft.print(mineria.temp.c_str());
  tft.print(" G");
  tft.setTextSize(2);
  tft.setCursor(46, 4);
  tft.print("EL DADO Y LA MONEDA");
  tft.setCursor(46, 27);
  tft.print("DE LA SUERTE");

  // Hashrate con un decimal para escribirlo con letras: "350.12" -> parte entera 350 y decimales 12
  String cadena = mineria.currentHashRate;
  cadena.replace(',', '.');
  int punto = cadena.indexOf('.');
  int parteEntera = (punto < 0) ? cadena.toInt() : cadena.substring(0, punto).toInt();
  int parteDecimal = (punto < 0) ? 0 : cadena.substring(punto + 1).toInt();

  dibujarDado(dado, 60, 85);
  dibujarMoneda(moneda, 250, 80);

  // Hashrate en letras: fuente 2 tamaño 1 ( más pequeña ) y 2 px más arriba ( y = 130 ) para no pisar la barra de abajo ( y = 186 )
  tft.setFreeFont(NULL);
  tft.setTextFont(2);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(1);
  tft.setCursor(0, 130);
  tft.print(quitarAcentos(numeroAEscrito(parteEntera, false)) + " Con ");
  tft.print(quitarAcentos(numeroAEscrito(parteDecimal, true)) + " KH/s.");

  // Hashrate numérico ( igual que antes )
  tft.setTextFont(1);
  tft.setTextSize(2);
  tft.setCursor(115, 50);
  tft.print(cadena);
  tft.setCursor(114, 69);
  tft.print(" KH/s");
  tft.setTextSize(1);
  tft.setTextDatum(TL_DATUM);

  cuentita++;
  if (cuentita == 15)
  {
    ajustarZonaHoraria();
    obtenerLocYTemp();
    obtenerVariacionesBTC(variacion1dia, variacion1semana);
  }
  manejandoLeds(mineria.currentHashRate.toFloat());
}

/****************************************************************************************************************
 *
 *   Escrito por: M8AX
 *
 *   Descripción: DEMOSCENE M8AX - Siete efectos a lo bruto calculados pixel a pixel por la CPU del ESP32
 *   ----------------------------------------------------------------------------------------------------
 *
 *   Como las demos de los 90, pero en un chip que además está minando Bitcoin. Cada 15 segundos cambia el
 *   efecto, y todos se calculan de verdad, nada de imágenes ni vídeos guardados:
 *
 *     1. PLASMA      - cuatro ondas de seno sumadas pixel a pixel y pintadas con una paleta que gira.
 *     2. TUNEL       - para cada pixel se calcula su distancia y su ángulo al centro, y de ahí sale la
 *                      profundidad y la textura: un túnel infinito que avanza y gira.
 *     3. ROTOZOOM    - una textura XOR que rota y hace zoom, con matriz de rotación por cada pixel.
 *     4. FUEGO       - autómata de calor: cada celda es la media de las de abajo menos un poco de enfriamiento.
 *     5. METABOLAS   - cinco gotas que se atraen como lava: se suma el campo 1/d2 de todas en cada pixel.
 *     6. TESERACTO   - un hipercubo de 4 dimensiones girando en 4 planos a la vez, con doble proyección
 *                      ( 4D a 3D y 3D a 2D ), 16 vértices y 32 aristas, borrando y pintando solo lo que cambia.
 *     7. WARP        - un campo de 90 estrellas en 3D que se viene encima con estela y un giro de cámara.
 *
 *   Encima de los cinco primeros va un scroller con las letras ondulando, dibujado a mano con la misma
 *   fuente 5x7 del banner, con sombra para que se lea sobre cualquier fondo.
 *
 *   Los efectos de pixeles se pintan fila a fila directamente al TFT ( sin buffer de pantalla ), así que no
 *   parpadean. Abajo sale el nombre del efecto con sus fotogramas por segundo, y a la derecha el hashrate.
 *
 *   LEDs: destello blanco al cambiar de efecto y latido corto de un color distinto para cada efecto.
 *
 *   Memoria: unos 6,5 KB en el heap ( paleta, tabla de senos, fuego, estrellas ), que se piden al entrar en la
 *   pantalla y se liberan al salir a otra, como el ajedrez y el Doom. Si no hay RAM, se salta la pantalla.
 *
 ****************************************************************************************************************/

// >>> DEMOSCENE MOTOR
#define DEM_NEF 7
#define DEM_SLICE_MS 5000
#define DEM_EFECTO_MS 15000
#define DEM_FW 80
#define DEM_FH 38
#define DEM_NSTAR 90
#define DEM_TY 108 // altura del scroller
#define DEM_ALTO 152

struct DemMem
{
  int16_t sn[256]; // seno * 127
  uint16_t pal[256];
  uint16_t lr[160]; // fila logica
  uint16_t ln[320]; // fila de pantalla
  uint8_t fire[DEM_FW * (DEM_FH + 2)];
  uint8_t cols[110];
  int16_t mbx[5], mby[5];
  int16_t tpx[16], tpy[16];
  int16_t ex[DEM_NSTAR], ey[DEM_NSTAR], ez[DEM_NSTAR];
  int16_t q1x[DEM_NSTAR], q1y[DEM_NSTAR], q2x[DEM_NSTAR], q2y[DEM_NSTAR];
  int scroll, c0, msgCols;
  uint8_t efecto;
  bool tpOk;
  uint32_t t;
  unsigned long desde;
};
static DemMem *demM = NULL;
static int demFps = 0;

static const char DEM_MSG[] = "M8AX - YOUTUBE.COM/M8AX - OPENSEA.IO/M8AX - PAYPAL.ME/M8AX/5 - GRACIAS - ";
static const char *const DEM_NOM[DEM_NEF] = {"PLASMA", "TUNEL", "ROTOZOOM", "FUEGO", "METABOLAS", "TESERACTO", "WARP"};
static const uint16_t DEM_COLB[4] = {TFT_CYAN, TFT_MAGENTA, TFT_YELLOW, TFT_GREEN};
static const bool DEM_LEDC[DEM_NEF][3] = {{1, 0, 1}, {0, 1, 1}, {1, 1, 0}, {1, 0, 0}, {0, 1, 0}, {1, 1, 1}, {0, 0, 1}};

static void demPaleta(int tipo)
{
  DemMem &d = *demM;
  for (int i = 0; i < 256; i++)
  {
    int r, g, b;
    if (tipo == 0) // arcoiris
    {
      r = 128 + d.sn[i];
      g = 128 + d.sn[(i + 85) & 255];
      b = 128 + d.sn[(i + 170) & 255];
    }
    else if (tipo == 1) // fuego
    {
      if (i < 64)
      {
        r = i * 4;
        g = 0;
        b = 0;
      }
      else if (i < 128)
      {
        r = 255;
        g = (i - 64) * 4;
        b = 0;
      }
      else if (i < 192)
      {
        r = 255;
        g = 255;
        b = (i - 128) * 4;
      }
      else
        r = g = b = 255;
    }
    else // electrico
    {
      r = (i * i) >> 8;
      g = (i >> 1) + ((i * i) >> 9);
      b = i;
    }
    if (r > 255)
      r = 255;
    if (g > 255)
      g = 255;
    if (b > 255)
      b = 255;
    d.pal[i] = (uint16_t)(((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3));
  }
}

// Angulo aproximado en 0..255 por vuelta ( puede salir negativo, se usa con & 255 )
static inline int demAng(int y, int x)
{
  int ax = abs(x), ay = abs(y);
  if (!ax && !ay)
    return 0;
  float a = (ax >= ay) ? (float)ay / (float)ax : (float)ax / (float)ay;
  float r = a * (0.7853982f + 0.273f * (1.0f - a));
  if (ax < ay)
    r = 1.5707963f - r;
  if (x < 0)
    r = 3.1415927f - r;
  if (y < 0)
    r = -r;
  return (int)(r * 40.7437f);
}

static void demEstrella(int i, bool lejos)
{
  DemMem &d = *demM;
  d.ex[i] = (int16_t)((int)(esp_random() % 2400) - 1200);
  d.ey[i] = (int16_t)((int)(esp_random() % 1200) - 600);
  d.ez[i] = (int16_t)(lejos ? 100 + (int)(esp_random() % 924) : 900 + (int)(esp_random() % 124));
  d.q1x[i] = -1;
  d.q2x[i] = -1;
}

static void demEntra(int ef)
{
  DemMem &d = *demM;
  d.efecto = ef;
  d.desde = millis();
  d.tpOk = false;
  if (ef == 3)
  {
    demPaleta(1);
    memset(d.fire, 0, sizeof(d.fire));
  }
  else if (ef == 4)
    demPaleta(2);
  else if (ef < 3)
    demPaleta(0);
  if (ef >= 5)
  {
    tft.fillRect(0, 0, 320, DEM_ALTO, TFT_BLACK);
    for (int i = 0; i < DEM_NSTAR; i++)
      demEstrella(i, true);
  }
}

static void demInit()
{
  DemMem &d = *demM;
  memset(&d, 0, sizeof(DemMem));
  for (int i = 0; i < 256; i++)
    d.sn[i] = (int16_t)(sinf(i * 6.2831853f / 256.0f) * 127.0f);
  d.msgCols = (int)(sizeof(DEM_MSG) - 1) * 6;
  d.efecto = esp_random() % DEM_NEF;
}

// ---- Efectos de pixeles: cada uno rellena la fila logica d.lr ( 160 pixeles, o 80 en el fuego ) ----
static void demPlasma(int ly)
{
  DemMem &d = *demM;
  int t = (int)d.t;
  int dy = ly - 38, dy2 = dy * dy;
  int yv = d.sn[(ly * 4 + t * 2) & 255];
  int ys = ly * 2 + t * 3;
  for (int x = 0; x < 160; x++)
  {
    int dx = x - 80;
    int v = d.sn[(x * 3 + t) & 255] + yv + d.sn[(x * 2 + ys) & 255] + d.sn[(((dx * dx + dy2) >> 4) + t) & 255];
    d.lr[x] = d.pal[(((v + 508) >> 2) + t) & 255];
  }
}

static void demTunel(int ly)
{
  DemMem &d = *demM;
  int t = (int)d.t;
  int dy = ly - 38;
  for (int x = 0; x < 160; x++)
  {
    int dx = x - 80;
    float dist = sqrtf((float)(dx * dx + dy * dy)) + 0.5f;
    if (dist < 4.0f)
    {
      d.lr[x] = 0;
      continue;
    }
    int di = (int)(2600.0f / dist) + t * 3;
    int ai = demAng(dy, dx) + (t >> 1) + (d.sn[(di >> 1) & 255] >> 4);
    int chk = ((di >> 3) ^ (ai >> 3)) & 1;
    int idx = ((di * 3 + ai) & 127) + chk * 128;
    d.lr[x] = d.pal[(idx + t) & 255];
  }
}

static void demRotozoom(int ly)
{
  DemMem &d = *demM;
  int t = (int)d.t;
  int a = t * 2;
  int ca = d.sn[(a + 64) & 255], sa = d.sn[a & 255];
  int z = 160 + d.sn[(t * 2) & 255];
  int dy = ly - 38;
  int bu = -dy * sa, bv = dy * ca;
  for (int x = 0; x < 160; x++)
  {
    int dx = x - 80;
    int u = ((dx * ca + bu) * z) >> 12;
    int v = ((dx * sa + bv) * z) >> 12;
    d.lr[x] = d.pal[(((u ^ v) << 2) + t) & 255];
  }
}

static void demFuegoUpd()
{
  DemMem &d = *demM;
  for (int x = 0; x < DEM_FW; x++)
  {
    uint32_t r = esp_random();
    uint8_t v = (r & 7) ? (uint8_t)(255 - ((r >> 3) & 63)) : 0;
    d.fire[DEM_FH * DEM_FW + x] = v;
    d.fire[(DEM_FH + 1) * DEM_FW + x] = v;
  }
  for (int y = 0; y < DEM_FH; y++)
  {
    const uint8_t *r1 = &d.fire[(y + 1) * DEM_FW];
    const uint8_t *r2 = &d.fire[(y + 2) * DEM_FW];
    uint8_t *o = &d.fire[y * DEM_FW];
    for (int x = 0; x < DEM_FW; x++)
    {
      int xl = x > 0 ? x - 1 : x, xr = x < DEM_FW - 1 ? x + 1 : x;
      int s = r1[xl] + r1[x] + r1[xr] + r2[x];
      o[x] = (uint8_t)((s * 120) >> 9);
    }
  }
}

static void demFuegoFila(int fy)
{
  DemMem &d = *demM;
  const uint8_t *f = &d.fire[fy * DEM_FW];
  for (int x = 0; x < DEM_FW; x++)
    d.lr[x] = d.pal[f[x]];
}

static void demMetabolas(int ly)
{
  DemMem &d = *demM;
  int t = (int)d.t;
  int d2y[5];
  for (int k = 0; k < 5; k++)
  {
    int dyy = ly - d.mby[k];
    d2y[k] = dyy * dyy + 1;
  }
  for (int x = 0; x < 160; x++)
  {
    int f = 0;
    for (int k = 0; k < 5; k++)
    {
      int dxx = x - d.mbx[k];
      f += 30000 / (dxx * dxx + d2y[k]);
    }
    int v = f > 255 ? 255 : f;
    d.lr[x] = (v < 40) ? 0 : d.pal[(v * 2 + t) & 255];
  }
}

// ---- Scroller con letras ondulantes ----
static uint8_t demCol(int ca)
{
  char ch = DEM_MSG[ca / 6];
  int k = ca % 6;
  if (k == 5)
    return 0;
  for (unsigned i = 0; i < sizeof(BAN_FONT) / sizeof(BAN_FONT[0]); i++)
    if (BAN_FONT[i][0] == (uint8_t)ch)
      return BAN_FONT[i][1 + k];
  return 0;
}

static inline int demBit(int x, int y)
{
  if (x < 0)
    return 0;
  DemMem &d = *demM;
  int wob = (d.sn[(x + (int)d.t * 3) & 255] * 8) >> 7;
  int ry = y - DEM_TY - wob;
  if (ry < 0 || ry >= 21)
    return 0;
  int ci = ((x + d.scroll) / 3) - d.c0;
  return (d.cols[ci] >> (ry / 3)) & 1;
}

static void demTexto(int y)
{
  DemMem &d = *demM;
  for (int x = 0; x < 320; x++)
  {
    if (demBit(x, y))
      d.ln[x] = 0xFFFF;
    else if (demBit(x - 2, y - 2))
      d.ln[x] = 0x0000;
  }
}

static void demPixeles(int ef)
{
  DemMem &d = *demM;
  int t = (int)d.t;
  if (ef == 3)
    demFuegoUpd();
  if (ef == 4)
    for (int k = 0; k < 5; k++)
    {
      d.mbx[k] = 80 + ((d.sn[(t * 3 + k * 51) & 255] * 55) >> 7);
      d.mby[k] = 38 + ((d.sn[(t * 2 + k * 37 + 64) & 255] * 26) >> 7);
    }
  d.scroll = (t * 4) % (d.msgCols * 3);
  d.c0 = d.scroll / 3;
  for (int ci = 0; ci < 110; ci++)
    d.cols[ci] = demCol((d.c0 + ci) % d.msgCols);

  const int xs = (ef == 3) ? 2 : 1;
  const int ys = (ef == 3) ? 4 : 2;
  const int filas = (ef == 3) ? DEM_FH : 76;

  tft.startWrite();
  tft.setAddrWindow(0, 0, 320, DEM_ALTO);
  int y = 0;
  for (int ly = 0; ly < filas; ly++)
  {
    switch (ef)
    {
    case 0:
      demPlasma(ly);
      break;
    case 1:
      demTunel(ly);
      break;
    case 2:
      demRotozoom(ly);
      break;
    case 3:
      demFuegoFila(ly);
      break;
    default:
      demMetabolas(ly);
      break;
    }
    for (int k = 0; k < ys; k++, y++)
    {
      for (int x = 0; x < 320; x++)
        d.ln[x] = d.lr[x >> xs];
      if (y >= DEM_TY - 8 && y <= DEM_TY + 30)
        demTexto(y);
      tft.pushColors(d.ln, 320, true);
    }
  }
  tft.endWrite();
}

// ---- Teseracto: hipercubo 4D girando en 4 planos ----
static void demTeseracto()
{
  DemMem &d = *demM;
  float tt = d.t * 0.022f;
  float c1 = cosf(tt), s1 = sinf(tt);
  float c2 = cosf(tt * 0.73f), s2 = sinf(tt * 0.73f);
  float c3 = cosf(tt * 1.31f), s3 = sinf(tt * 1.31f);
  float c4 = cosf(tt * 0.47f), s4 = sinf(tt * 0.47f);
  int16_t nx[16], ny[16];
  for (int i = 0; i < 16; i++)
  {
    float x = (i & 1) ? 1.0f : -1.0f, y = (i & 2) ? 1.0f : -1.0f;
    float z = (i & 4) ? 1.0f : -1.0f, w = (i & 8) ? 1.0f : -1.0f;
    float a;
    a = x * c1 - w * s1;
    w = x * s1 + w * c1;
    x = a;
    a = y * c2 - z * s2;
    z = y * s2 + z * c2;
    y = a;
    a = z * c3 - w * s3;
    w = z * s3 + w * c3;
    z = a;
    a = x * c4 - y * s4;
    y = x * s4 + y * c4;
    x = a;
    float k = 1.0f / (2.6f - w);
    float m = 1.0f / (3.2f - z * k);
    int sx = 160 + (int)(x * k * m * 150.0f);
    int sy = 75 + (int)(y * k * m * 150.0f);
    nx[i] = (int16_t)constrain(sx, 3, 316);
    ny[i] = (int16_t)constrain(sy, 3, 147);
  }
  if (d.tpOk)
  {
    for (int i = 0; i < 16; i++)
      for (int b = 0; b < 4; b++)
        if (!(i & (1 << b)))
        {
          int j = i | (1 << b);
          tft.drawLine(d.tpx[i], d.tpy[i], d.tpx[j], d.tpy[j], TFT_BLACK);
        }
    for (int i = 0; i < 16; i++)
      tft.fillCircle(d.tpx[i], d.tpy[i], 2, TFT_BLACK);
  }
  for (int i = 0; i < 16; i++)
    for (int b = 0; b < 4; b++)
      if (!(i & (1 << b)))
      {
        int j = i | (1 << b);
        tft.drawLine(nx[i], ny[i], nx[j], ny[j], DEM_COLB[b]);
      }
  for (int i = 0; i < 16; i++)
  {
    tft.fillCircle(nx[i], ny[i], 2, TFT_WHITE);
    d.tpx[i] = nx[i];
    d.tpy[i] = ny[i];
  }
  d.tpOk = true;
}

// ---- Warp: estrellas en 3D con estela y giro de camara ----
static void demWarp()
{
  DemMem &d = *demM;
  int t = (int)d.t;
  int rot = d.sn[(t >> 1) & 255] >> 3;
  int vel = 10 + ((d.sn[t & 255] + 127) >> 4);
  for (int i = 0; i < DEM_NSTAR; i++)
  {
    if (d.q2x[i] >= 0 && d.q1x[i] >= 0)
      tft.drawLine(d.q2x[i], d.q2y[i], d.q1x[i], d.q1y[i], TFT_BLACK);
    int x = d.ex[i], y = d.ey[i], z = d.ez[i] - vel;
    int nx = x + ((y * rot) >> 10);
    int ny = y - ((x * rot) >> 10);
    d.ex[i] = (int16_t)nx;
    d.ey[i] = (int16_t)ny;
    d.ez[i] = (int16_t)z;
    if (z < 24)
    {
      demEstrella(i, false);
      continue;
    }
    int sx = 160 + (nx * 128) / z;
    int sy = 75 + (ny * 128) / z;
    if (sx < 1 || sx > 318 || sy < 1 || sy > 148)
    {
      demEstrella(i, false);
      continue;
    }
    uint16_t col = z > 640 ? 0x7BEF : (z > 320 ? 0x07FF : 0xFFFF);
    if (d.q1x[i] >= 0)
      tft.drawLine(d.q1x[i], d.q1y[i], sx, sy, col);
    else
      tft.drawPixel(sx, sy, col);
    if (z < 200)
      tft.drawPixel(sx + 1, sy, col);
    d.q2x[i] = d.q1x[i];
    d.q2y[i] = d.q1y[i];
    d.q1x[i] = (int16_t)sx;
    d.q1y[i] = (int16_t)sy;
  }
}

static void demFrame()
{
  DemMem &d = *demM;
  d.t++;
  if (d.efecto == 5)
    demTeseracto();
  else if (d.efecto == 6)
    demWarp();
  else
    demPixeles(d.efecto);
}

static void demHud(const char *izq, const char *der)
{
  tft.setFreeFont(NULL);
  tft.setTextFont(1);
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextPadding(160);
  tft.setTextDatum(TL_DATUM);
  tft.drawString(izq, 4, 153);
  tft.setTextPadding(150);
  tft.setTextDatum(TR_DATUM);
  tft.drawString(der, 316, 153);
  tft.setTextPadding(0);
  tft.setTextDatum(TL_DATUM);
}
// <<< DEMOSCENE MOTOR

void esp32_2432S028R_DemoScreen(unsigned long mElapsed)
{
  mineria = getMiningData(mElapsed);
  Serial.printf("M8AX - %s >>> Completados %s Share(s), %s Khashes, Prom. Hashrate %s KH/s %s°\n",
                mineria.currentTime.c_str(), mineria.completedShares.c_str(), mineria.totalKHashes.c_str(), mineria.currentHashRate.c_str(), mineria.temp.c_str());

  bool redraw = hasChangedScreen;
  if (!demM)
  {
    demM = (DemMem *)malloc(sizeof(DemMem));
    if (!demM)
    {
      Serial.println("M8AX - Demoscene: No Hay RAM Libre");
      delay(1000);
      return;
    }
    demInit();
    redraw = true;
  }
  const int demMiPantalla = currentDisplayDriver->current_cyclic_screen;
  if (redraw)
  {
    ledsLibres();
    tft.fillRect(0, 0, 320, 170, TFT_BLACK);
    demEntra(demM->efecto);
  }
  printPoolData(); // barra de abajo, solo se repinta cuando toca
  hasChangedScreen = false;

  char hashStr[16];
  snprintf(hashStr, sizeof(hashStr), "%s", mineria.currentHashRate.c_str());
  char izq[24], der[24];
  char lastIzq[24] = "", lastDer[24] = "";

  unsigned long start = millis();
  unsigned long tFps = start;
  unsigned long lastHud = 0;
  unsigned long lastMiningUpdate = 0;
  unsigned long lastTouchChk = 0;
  unsigned long ledApagarEn = 0;
  unsigned long ledSigue = 0;
  uint32_t frames = 0;
  pongLeds(false);

  while (millis() - start < DEM_SLICE_MS)
  {
    unsigned long now = millis();

    // ---- Táctil (misma lógica que el Pong) ----
    if (now - lastTouchChk >= 40 && now - previousTouchMillis >= 500)
    {
      lastTouchChk = now;
      int16_t t_x, t_y;
      if (toqueXY(t_x, t_y))
      {
        previousTouchMillis = now;

        if (((t_x > 109) && (t_x < 211)) && ((t_y > 185) && (t_y < 241)))
        {
          bottomScreenBlue ^= true;
          hasChangedScreen = true;
        }
        else if ((t_x > 235) && ((t_y > 0) && (t_y < 16)))
        {
          esp32_2432S028R_AlternateScreenState();
        }
        else if (t_x > 160)
        {
          currentDisplayDriver->current_cyclic_screen = (currentDisplayDriver->current_cyclic_screen + 1) % currentDisplayDriver->num_cyclic_screens;
        }
        else
        {
          currentDisplayDriver->current_cyclic_screen = currentDisplayDriver->current_cyclic_screen - 1;
          if (currentDisplayDriver->current_cyclic_screen < 0)
            currentDisplayDriver->current_cyclic_screen = currentDisplayDriver->num_cyclic_screens - 1;
        }
        break;
      }
    }

    // ---- LEDs: latido del color de cada efecto ----
    if (ledApagarEn && now >= ledApagarEn)
    {
      pongLeds(false);
      ledApagarEn = 0;
    }
    if (!ledApagarEn && now >= ledSigue)
    {
      digitalWrite(LED_PIN, DEM_LEDC[demM->efecto][0] ? LOW : HIGH);
      digitalWrite(LED_PIN_G, DEM_LEDC[demM->efecto][1] ? LOW : HIGH);
      digitalWrite(LED_PIN_B, DEM_LEDC[demM->efecto][2] ? LOW : HIGH);
      ledApagarEn = now + 50;
      ledSigue = now + 150;
    }

    // ---- Siguiente efecto ----
    if (now - demM->desde >= DEM_EFECTO_MS)
    {
      demEntra((demM->efecto + 1) % DEM_NEF);
      pongLeds(true); // destello blanco
      ledApagarEn = now + 120;
    }

    // ---- Un fotograma ----
    demFrame();
    frames++;
    if (ledApagarEn && millis() >= ledApagarEn) // apagar sin esperar a la siguiente vuelta
    {
      pongLeds(false);
      ledApagarEn = 0;
    }

    now = millis();
    if (now - tFps >= 1000)
    {
      demFps = (int)(frames * 1000UL / (now - tFps));
      frames = 0;
      tFps = now;
    }

    // ---- Hashrate (cada segundo) ----
    if (now - lastMiningUpdate >= 1000)
    {
      lastMiningUpdate = now;
      mineria = getMiningData(mElapsed);
      snprintf(hashStr, sizeof(hashStr), "%s", mineria.currentHashRate.c_str());
    }

    // ---- Textos de abajo ----
    if (now - lastHud >= 250)
    {
      lastHud = now;
      snprintf(izq, sizeof(izq), "%s %dF", DEM_NOM[demM->efecto], demFps);
      snprintf(der, sizeof(der), "%s KH/s", hashStr);
      if (strcmp(izq, lastIzq) != 0 || strcmp(der, lastDer) != 0)
      {
        demHud(izq, der);
        strcpy(lastIzq, izq);
        strcpy(lastDer, der);
      }
    }
    webAtender();
    delay(1);
  }

  pongLeds(false);
  tft.setTextPadding(0);
  tft.setTextDatum(TL_DATUM);

  if (currentDisplayDriver->current_cyclic_screen != demMiPantalla)
  {
    free(demM);
    demM = NULL;
  }

  cuentita++;
  if (cuentita == 15)
  {
    ajustarZonaHoraria();
    obtenerLocYTemp();
    obtenerVariacionesBTC(variacion1dia, variacion1semana);
  }
}

/****************************************************************************************************************
 *
 *   Escrito por: M8AX
 *
 *   Descripción: CALCULADORA M8AX - Pantalla táctil para ESP32_2432S028R ( versión normal )
 *   -----------------------------------------------------------------------------------------------------------
 *
 *   Calculadora de verdad con teclado táctil de 5x5 teclas a pantalla completa ( 320x240, sin barra de criptos
 *   para que las teclas sean grandes ). Entiende expresiones enteras, no va tecla a tecla:
 *
 *       +  -  *  /  ^ ( potencia )  % ( porcentaje, 50% = 0.5 )  paréntesis  sqrt( )  y ANS ( último resultado )
 *
 *   Mientras escribes sale en gris el resultado provisional, y al pulsar = sale en verde. Si falta cerrar
 *   paréntesis, se cierran solos. Si pulsas un operador justo después de un resultado, sigue la cuenta con él.
 *   División por cero, raíz de negativo o expresión rara = ERROR en rojo.
 *
 *   LEDs: cada tecla = destello ROJO corto. Resultado correcto con = destello VERDE largo. ERROR = rojo largo.
 *
 *   Arriba: [<] pantalla anterior, [>] pantalla siguiente, el hashrate en el centro y [LUZ] apaga/enciende la
 *   luz de la pantalla. Todo lo demás son teclas.
 *
 *   Memoria: no pide nada al heap. El estado ( unos 100 bytes ) es estático y la cuenta se conserva al salir y
 *   volver a la pantalla.
 *
 *   Para activarla: pegar este bloque justo antes de analiCadaSegundo y meter esp32_2432S028R_CalcScreen en el
 *   array esp32_2432S028RCyclicScreens[].
 *
 ****************************************************************************************************************/

#define CAL_SLICE_MS 5000
#define CAL_MAXLEN 60
#define CAL_HH 20 // alto de la cabecera
#define CAL_KY 78 // y donde empiezan las teclas
#define CAL_KW 64
#define CAL_KH 32

#define CAL_C_NUM 0x2965
#define CAL_C_OP 0xFD20
#define CAL_C_FN 0x0416
#define CAL_C_CLR 0xF800
#define CAL_C_EQ 0x07E0

struct CalEstado
{
  bool init, done, err;
  int len;
  char expr[CAL_MAXLEN + 1];
  char ans[24];
};
static CalEstado calS = {false};

struct CalTecla
{
  const char *lab;
  const char *ins;
  uint16_t bg, fg;
};

static const CalTecla CAL_T[5][5] = {
    {{"C", "C", CAL_C_CLR, TFT_WHITE}, {"DEL", "DEL", CAL_C_FN, TFT_WHITE}, {"(", "(", CAL_C_FN, TFT_WHITE}, {")", ")", CAL_C_FN, TFT_WHITE}, {"/", "/", CAL_C_OP, TFT_BLACK}},
    {{"7", "7", CAL_C_NUM, TFT_WHITE}, {"8", "8", CAL_C_NUM, TFT_WHITE}, {"9", "9", CAL_C_NUM, TFT_WHITE}, {"x^2", "^2", CAL_C_FN, TFT_WHITE}, {"x", "*", CAL_C_OP, TFT_BLACK}},
    {{"4", "4", CAL_C_NUM, TFT_WHITE}, {"5", "5", CAL_C_NUM, TFT_WHITE}, {"6", "6", CAL_C_NUM, TFT_WHITE}, {"sqrt", "sqrt(", CAL_C_FN, TFT_WHITE}, {"-", "-", CAL_C_OP, TFT_BLACK}},
    {{"1", "1", CAL_C_NUM, TFT_WHITE}, {"2", "2", CAL_C_NUM, TFT_WHITE}, {"3", "3", CAL_C_NUM, TFT_WHITE}, {"%", "%", CAL_C_FN, TFT_WHITE}, {"+", "+", CAL_C_OP, TFT_BLACK}},
    {{"^", "^", CAL_C_FN, TFT_WHITE}, {"0", "0", CAL_C_NUM, TFT_WHITE}, {".", ".", CAL_C_NUM, TFT_WHITE}, {"ANS", "ANS", CAL_C_FN, TFT_WHITE}, {"=", "=", CAL_C_EQ, TFT_BLACK}}};

// ---- Intérprete: + - * / ^ % ( ) sqrt( ) y menos unario ----
static const char *calP;
static bool calErr;
static double calExpr();

static double calPri()
{
  if (*calP == '(')
  {
    calP++;
    double v = calExpr();
    if (*calP == ')')
      calP++;
    else if (*calP)
      calErr = true; // si se acaba la cuenta, el paréntesis se cierra solo
    return v;
  }
  if (!strncmp(calP, "sqrt(", 5))
  {
    calP += 5;
    double v = calExpr();
    if (*calP == ')')
      calP++;
    else if (*calP)
      calErr = true;
    if (v < 0)
      calErr = true;
    return calErr ? 0 : sqrt(v);
  }
  if ((*calP >= '0' && *calP <= '9') || *calP == '.')
  {
    char *e;
    double v = strtod(calP, &e);
    if (e == calP)
    {
      calErr = true;
      return 0;
    }
    calP = e;
    return v;
  }
  calErr = true;
  return 0;
}

static double calPost()
{
  double v = calPri();
  while (*calP == '%')
  {
    calP++;
    v /= 100.0;
  }
  return v;
}

static double calUna();

static double calPot()
{
  double b = calPost();
  if (*calP == '^')
  {
    calP++;
    return pow(b, calUna());
  }
  return b;
}

static double calUna()
{
  if (*calP == '-')
  {
    calP++;
    return -calUna();
  }
  if (*calP == '+')
  {
    calP++;
    return calUna();
  }
  return calPot();
}

static double calTer()
{
  double v = calUna();
  while (*calP == '*' || *calP == '/')
  {
    char o = *calP++;
    double w = calUna();
    if (o == '*')
      v *= w;
    else
    {
      if (w == 0)
      {
        calErr = true;
        return 0;
      }
      v /= w;
    }
  }
  return v;
}

static double calExpr()
{
  double v = calTer();
  while (*calP == '+' || *calP == '-')
  {
    char o = *calP++;
    double w = calTer();
    v = (o == '+') ? v + w : v - w;
  }
  return v;
}

static bool calEval(const char *s, double &out)
{
  if (!*s)
    return false;
  calP = s;
  calErr = false;
  double v = calExpr();
  if (calErr || *calP || isnan(v) || isinf(v))
    return false;
  out = v;
  return true;
}

static void calFmt(double v, char *o, size_t n)
{
  if (fabs(v) < 1e-12)
    v = 0;
  snprintf(o, n, "%.10g", v);
}

static bool calEsOp(char c)
{
  return c == '+' || c == '-' || c == '*' || c == '/' || c == '^' || c == '%';
}

static void calBorra()
{
  calS.len = 0;
  calS.expr[0] = 0;
  calS.done = false;
  calS.err = false;
}

// Devuelve 0 = tecla normal, 1 = resultado correcto, 2 = error
static int calPulsa(const char *ins)
{
  CalEstado &s = calS;
  if (!strcmp(ins, "C"))
  {
    calBorra();
    return 0;
  }
  if (!strcmp(ins, "DEL"))
  {
    if (s.done || s.err)
      calBorra();
    else if (s.len >= 5 && !strcmp(s.expr + s.len - 5, "sqrt("))
    {
      s.len -= 5;
      s.expr[s.len] = 0;
    }
    else if (s.len > 0)
      s.expr[--s.len] = 0;
    return 0;
  }
  if (!strcmp(ins, "="))
  {
    double v;
    if (s.len == 0)
      return 0;
    if (calEval(s.expr, v))
    {
      calFmt(v, s.ans, sizeof(s.ans));
      s.done = true;
      s.err = false;
      return 1;
    }
    s.err = true;
    s.done = false;
    return 2;
  }
  bool esAns = !strcmp(ins, "ANS");
  if (esAns)
  {
    if (!s.ans[0])
      return 0;
    ins = s.ans;
  }
  if (s.err)
    calBorra();
  if (s.done)
  {
    if (!esAns && calEsOp(ins[0]))
    {
      strcpy(s.expr, s.ans);
      s.len = strlen(s.expr);
    }
    else
    {
      s.len = 0;
      s.expr[0] = 0;
    }
    s.done = false;
  }
  if (!strcmp(ins, "."))
  {
    int i = s.len - 1;
    bool hay = false;
    while (i >= 0 && ((s.expr[i] >= '0' && s.expr[i] <= '9') || s.expr[i] == '.'))
    {
      if (s.expr[i] == '.')
        hay = true;
      i--;
    }
    if (hay)
      return 0;
    if (s.len == 0 || !(s.expr[s.len - 1] >= '0' && s.expr[s.len - 1] <= '9'))
      ins = "0.";
  }
  int l = strlen(ins);
  if (s.len + l > CAL_MAXLEN)
    return 0;
  memcpy(s.expr + s.len, ins, l + 1);
  s.len += l;
  return 0;
}

// ---- Dibujo ----
static void calPintaTecla(int r, int c, bool pulsada)
{
  const CalTecla &k = CAL_T[r][c];
  int x = c * CAL_KW, y = CAL_KY + r * CAL_KH;
  uint16_t bg = pulsada ? TFT_WHITE : k.bg, fg = pulsada ? TFT_BLACK : k.fg;
  tft.fillRect(x, y, CAL_KW, CAL_KH, TFT_BLACK);
  tft.fillRoundRect(x + 2, y + 2, CAL_KW - 4, CAL_KH - 4, 6, bg);
  tft.setFreeFont(NULL);
  tft.setTextFont(1);
  tft.setTextPadding(0);
  tft.setTextDatum(MC_DATUM);
  tft.setTextSize(strlen(k.lab) == 1 ? 3 : 2);
  tft.setTextColor(fg, bg);
  tft.drawString(k.lab, x + CAL_KW / 2, y + CAL_KH / 2);
  tft.setTextDatum(TL_DATUM);
}

static void calPintaPantalla()
{
  CalEstado &s = calS;
  char vis[28], res[28];
  uint16_t colRes = TFT_WHITE;
  tft.fillRect(1, 23, 318, 50, TFT_BLACK);

  int n = s.len;
  const char *p = s.expr;
  if (n > 25)
  {
    p += n - 25;
    n = 25;
  }
  memcpy(vis, p, n);
  vis[n] = 0;
  tft.setFreeFont(NULL);
  tft.setTextFont(1);
  tft.setTextPadding(0);
  tft.setTextDatum(TR_DATUM);
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString(vis, 314, 26);

  double v;
  if (s.err)
  {
    strcpy(res, "ERROR");
    colRes = TFT_RED;
  }
  else if (s.done)
  {
    strcpy(res, s.ans);
    colRes = TFT_GREEN;
  }
  else if (s.len == 0)
  {
    strcpy(res, "0");
    colRes = 0x4208;
  }
  else if (calEval(s.expr, v))
  {
    calFmt(v, res, sizeof(res));
    colRes = 0xBDF7;
  }
  else
    res[0] = 0;
  tft.setTextSize(strlen(res) <= 16 ? 3 : 2);
  tft.setTextColor(colRes, TFT_BLACK);
  tft.drawString(res, 314, 46);
  tft.setTextDatum(TL_DATUM);
}

static void calHud(const char *hash)
{
  char b[24];
  snprintf(b, sizeof(b), "%s KH/s", hash);
  tft.setFreeFont(NULL);
  tft.setTextFont(1);
  tft.setTextSize(1);
  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.setTextPadding(150);
  tft.setTextDatum(TC_DATUM);
  tft.drawString(b, 175, 7);
  tft.setTextPadding(0);
  tft.setTextDatum(TL_DATUM);
}

static void calPintaTodo()
{
  tft.fillRect(0, 0, 320, 240, TFT_BLACK);
  tft.setFreeFont(NULL);
  tft.setTextFont(1);
  tft.setTextPadding(0);
  tft.setTextDatum(MC_DATUM);
  tft.setTextSize(2);
  tft.fillRoundRect(2, 1, 44, 18, 4, CAL_C_FN);
  tft.setTextColor(TFT_WHITE, CAL_C_FN);
  tft.drawString("<", 24, 10);
  tft.fillRoundRect(50, 1, 44, 18, 4, CAL_C_FN);
  tft.drawString(">", 72, 10);
  tft.fillRoundRect(256, 1, 62, 18, 4, 0x4208);
  tft.setTextColor(TFT_WHITE, 0x4208);
  tft.drawString("LUZ", 287, 10);
  tft.setTextDatum(TL_DATUM);
  tft.drawRect(0, 22, 320, 52, TFT_DARKCYAN);
  for (int r = 0; r < 5; r++)
    for (int c = 0; c < 5; c++)
      calPintaTecla(r, c, false);
  calPintaPantalla();
}

void esp32_2432S028R_CalcScreen(unsigned long mElapsed)
{
  mineria = getMiningData(mElapsed);
  Serial.printf("M8AX - %s >>> Completados %s Share(s), %s Khashes, Prom. Hashrate %s KH/s %s°\n",
                mineria.currentTime.c_str(), mineria.completedShares.c_str(), mineria.totalKHashes.c_str(), mineria.currentHashRate.c_str(), mineria.temp.c_str());

  bool redraw = hasChangedScreen;
  if (!calS.init)
  {
    memset(&calS, 0, sizeof(calS));
    calS.init = true;
    redraw = true;
  }
  hasChangedScreen = false;
  if (redraw)
    calPintaTodo();

  char hashStr[16];
  char lastHash[16] = "";
  snprintf(hashStr, sizeof(hashStr), "%s", mineria.currentHashRate.c_str());
  calHud(hashStr);
  strcpy(lastHash, hashStr);

  int16_t t_x, t_y;
  bool wasDown = toqueXY(t_x, t_y); // si venimos de un toque, no cuenta como tecla
  bool salir = false;
  unsigned long start = millis();
  unsigned long lastTouchChk = 0;
  unsigned long lastMiningUpdate = 0;
  pongLeds(false);

  while (!salir && millis() - start < CAL_SLICE_MS)
  {
    unsigned long now = millis();

    // ---- Táctil: solo cuenta al pulsar ( flanco ), no mientras se mantiene ----
    if (now - lastTouchChk >= 15)
    {
      lastTouchChk = now;
      bool down = toqueXY(t_x, t_y);
      if (down && !wasDown)
      {
        previousTouchMillis = now;
        if (t_y < CAL_HH)
        {
          if (t_x < 48)
          {
            currentDisplayDriver->current_cyclic_screen = currentDisplayDriver->current_cyclic_screen - 1;
            if (currentDisplayDriver->current_cyclic_screen < 0)
              currentDisplayDriver->current_cyclic_screen = currentDisplayDriver->num_cyclic_screens - 1;
            salir = true;
          }
          else if (t_x < 96)
          {
            currentDisplayDriver->current_cyclic_screen = (currentDisplayDriver->current_cyclic_screen + 1) % currentDisplayDriver->num_cyclic_screens;
            salir = true;
          }
          else if (t_x >= 250)
          {
            esp32_2432S028R_AlternateScreenState();
          }
        }
        else if (t_x >= 0 && t_y >= CAL_KY && t_y < CAL_KY + 5 * CAL_KH)
        {
          int c = t_x / CAL_KW;
          int r = (t_y - CAL_KY) / CAL_KH;
          if (c > 4)
            c = 4;
          calPintaTecla(r, c, true);
          int res = calPulsa(CAL_T[r][c].ins);
          calPintaPantalla();
          if (res == 1)
            ledFlash(false, true, false, 180); // resultado: verde
          else if (res == 2)
            ledFlash(true, false, false, 180); // error: rojo largo
          else
            ledFlash(true, false, false, 35); // tecla: rojo corto
          calPintaTecla(r, c, false);
        }
      }
      wasDown = down;
    }

    // ---- Hashrate arriba, cada segundo ----
    if (now - lastMiningUpdate >= 1000)
    {
      lastMiningUpdate = now;
      mineria = getMiningData(mElapsed);
      snprintf(hashStr, sizeof(hashStr), "%s", mineria.currentHashRate.c_str());
      if (strcmp(hashStr, lastHash) != 0)
      {
        calHud(hashStr);
        strcpy(lastHash, hashStr);
      }
    }
    webAtender();
    delay(8);
  }

  pongLeds(false);
  tft.setTextPadding(0);
  tft.setTextDatum(TL_DATUM);
  previousTouchMillis = millis(); // que DoLedStuff no se coma un toque justo entre tandas

  cuentita++;
  if (cuentita == 15)
  {
    ajustarZonaHoraria();
    obtenerLocYTemp();
    obtenerVariacionesBTC(variacion1dia, variacion1semana);
  }
}

/****************************************************************************************************************
 *
 *   FLAPPY HASH - El Flappy Bird que juega la CPU sola, y el pajaro ... ES TU HASHRATE ( ESP32_2432S028R )
 *
 *   El "pajaro" es una moneda dorada con alas que lleva escrito el hashrate REAL del minero ( se actualiza cada
 *   segundo ). Los tubos son RACKS de ASICs con ranuras y LEDs parpadeando, con una abertura por la que hay que
 *   pasar. Detras hay dos capas de rascacielos con ventanas ( parallax ), estrellas, y abajo una CADENA DE
 *   BLOQUES con un nonce dorado en cada uno.
 *
 *   La CPU juega sola y cada poco "se despista", se la pega y explota. Con la puntuacion el juego se acelera y
 *   las aberturas se estrechan. Tras el golpe empieza otra partida sola. El record se conserva mientras el
 *   minero siga encendido.
 *
 *   Memoria: sin buffer de pantalla, unos 1,5 KB en el heap. Se piden al entrar y se devuelven al salir a otra.
 *   Si no hay RAM, se salta la pantalla sin pintar nada.
 *
 *   Arriba la hora y el hashrate ( pongDrawHud ), abajo puntos, record y numero de partida, y la barra de
 *   cripto de siempre ( printPoolData ).
 *
 *   Para activarla: pegar este bloque justo antes de analiCadaSegundo ( detras de la ultima pantalla ) y meter
 *   esp32_2432S028R_FlappyHashScreen en el array esp32_2432S028RCyclicScreens[]. No hace falta tocar nada mas.
 *
 ****************************************************************************************************************/

// >>> FLAPPYHASH MOTOR
#define FH_W 320
#define FH_H 120
#define FH_Y0 28 // el HUD de arriba ocupa hasta la 26 y el de abajo empieza en la 150
#define FH_FRAME_MS 30
#define FH_SLICE_MS 5000 // tandas de 5 s, como en las demas
#define FH_NR 4          // racks vivos a la vez
#define FH_NP 32         // particulas
#define FH_NS 18         // estrellas
#define FH_PW 28         // ancho del rack
#define FH_CAPH 6        // alto de la tapa del rack
#define FH_ESP 112       // distancia entre racks
#define FH_SUELO 10      // alto de la cadena de bloques
#define FH_BX 72         // x fija del pajaro
#define FH_BH 14         // alto del pajaro
#define FH_SEGF 18       // ancho de los edificios lejanos
#define FH_SEGN 30       // ancho de los edificios cercanos
#define FH_G 0.30f
#define FH_SALTO (-3.3f)
#define FH_RGB(r, g, b) ((uint16_t)((((r) & 0xF8) << 8) | (((g) & 0xFC) << 3) | ((b) >> 3)))

// Digitos 3x5 ( 3 bits por fila, de arriba a abajo )
static const uint16_t FH_DIG[10] = {0x7B6F, 0x2C97, 0x73E7, 0x73CF, 0x5BC9, 0x79CF, 0x79EF, 0x7249, 0x7BEF, 0x7BCF};
// Cuanto se recorta cada fila del pajaro por los lados para que quede en forma de pastilla
static const uint8_t FH_PILL[FH_BH] = {4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4};
// Colores de las particulas segun lo que les queda de vida ( 4 niveles )
static const uint16_t FH_PCOL[4][4] = {
    {FH_RGB(90, 30, 0), FH_RGB(200, 90, 0), FH_RGB(255, 170, 20), FH_RGB(255, 230, 120)},
    {FH_RGB(60, 60, 80), FH_RGB(120, 120, 150), FH_RGB(190, 190, 210), FH_RGB(255, 255, 255)},
    {FH_RGB(110, 20, 10), FH_RGB(230, 50, 20), FH_RGB(255, 150, 30), FH_RGB(255, 240, 120)},
    {FH_RGB(10, 80, 40), FH_RGB(30, 170, 80), FH_RGB(80, 255, 120), FH_RGB(200, 255, 220)}};

struct FhRack
{
  float x;
  int16_t gy; // centro de la abertura
  uint8_t gap, flash, vivo, pasado;
  uint16_t id;
};

struct FhPart
{
  float x, y, vx, vy;
  uint8_t vida, tipo; // 0 dorada, 1 blanca, 2 explosion, 3 verde
};

struct FhStar
{
  float x;
  uint8_t y, capa;
};

struct FhMem
{
  uint16_t linea[FH_W]; // la primera: alineada a 4 para rellenar de 2 en 2 pixeles
  FhRack rk[FH_NR];
  FhPart pt[FH_NP];
  FhStar es[FH_NS];
  float y, vy, vel, scF, scN, scS, sigX;
  int16_t ultGy;
  uint8_t nd, dg[4], estado, aleteo, flash, ipt, record;
  uint16_t despiste, finTimer, sigId;
  uint32_t puntos, frame;
  char tIzq[16], tCen[14], tDer[16];
};
static FhMem *fhM = NULL;
static uint32_t fhMejor = 0; // record ( fuera del heap: sobrevive a salir de la pantalla )
static uint32_t fhPartidas = 0;

static inline uint32_t fhRnd(uint32_t n) { return esp_random() % n; }
static inline float fhRf() { return (esp_random() & 0xFFFF) * (1.0f / 65535.0f); }

static inline uint32_t fhHash(uint32_t v)
{
  v ^= v >> 16;
  v *= 0x7feb352dU;
  v ^= v >> 15;
  v *= 0x846ca68bU;
  v ^= v >> 16;
  return v;
}

static inline void fhSpan(uint16_t *L, int xa, int xb, uint16_t c)
{
  if (xa < 0)
    xa = 0;
  if (xb > FH_W)
    xb = FH_W;
  while (xa < xb)
    L[xa++] = c;
}

static inline void fhPx(uint16_t *L, int x, uint16_t c)
{
  if ((unsigned)x < FH_W)
    L[x] = c;
}

// El hashrate pasa a ser el numero que lleva el pajaro ( de 1 a 4 cifras )
static void fhPonHash(int v)
{
  FhMem &m = *fhM;
  if (v < 0)
    v = 0;
  if (v > 9999)
    v = 9999;
  char b[8];
  int n = snprintf(b, sizeof(b), "%d", v);
  if (n < 1)
    n = 1;
  if (n > 4)
    n = 4;
  m.nd = (uint8_t)n;
  for (int i = 0; i < n; i++)
    m.dg[i] = (uint8_t)(b[i] - '0');
}

static inline int fhAncho() { return 8 * fhM->nd + 6; }

static void fhChispa(float x, float y, float vx, float vy, uint8_t vida, uint8_t tipo)
{
  FhMem &m = *fhM;
  for (int k = 0; k < FH_NP; k++)
  {
    FhPart &p = m.pt[m.ipt];
    m.ipt = (m.ipt + 1) % FH_NP;
    if (!p.vida)
    {
      p.x = x;
      p.y = y;
      p.vx = vx;
      p.vy = vy;
      p.vida = vida;
      p.tipo = tipo;
      return;
    }
  }
}

static void fhNuevoRack()
{
  FhMem &m = *fhM;
  for (int i = 0; i < FH_NR; i++)
  {
    FhRack &r = m.rk[i];
    if (r.vivo)
      continue;
    int gap = 56 - (int)(m.puntos / 2);
    if (gap < 44)
      gap = 44;
    int lo = gap / 2 + 6, hi = FH_H - FH_SUELO - gap / 2 - 6;
    int gy = m.ultGy + (int)fhRnd(89) - 44; // cambia como mucho 44 px respecto al anterior
    if (gy < lo)
      gy = lo;
    if (gy > hi)
      gy = hi;
    r.x = m.sigX;
    r.gy = gy;
    r.gap = (uint8_t)gap;
    r.flash = 0;
    r.vivo = 1;
    r.pasado = 0;
    r.id = m.sigId++;
    m.ultGy = gy;
    return;
  }
}

static void fhNuevaPartida()
{
  FhMem &m = *fhM;
  m.y = 52.0f;
  m.vy = 0;
  m.vel = 1.9f;
  m.estado = 0;
  m.aleteo = 0;
  m.flash = 0;
  m.despiste = 0;
  m.finTimer = 0;
  m.record = 0;
  m.puntos = 0;
  m.ultGy = 58;
  m.sigX = 250.0f;
  for (int i = 0; i < FH_NR; i++)
    m.rk[i].vivo = 0;
  for (int i = 0; i < FH_NP; i++)
    m.pt[i].vida = 0;
  fhPartidas++;
}

static void fhInit()
{
  FhMem &m = *fhM;
  memset(&m, 0, sizeof(FhMem));
  m.nd = 3;
  m.dg[0] = 3;
  m.dg[1] = 9;
  m.dg[2] = 0;
  for (int i = 0; i < FH_NS; i++)
  {
    m.es[i].x = (float)fhRnd(FH_W);
    m.es[i].y = (uint8_t)fhRnd(70);
    m.es[i].capa = (uint8_t)fhRnd(3);
  }
  m.scF = (float)fhRnd(500);
  m.scN = (float)fhRnd(500);
  fhNuevaPartida();
}

// La CPU decide si aletea: se mantiene rebotando en la parte baja de la abertura del siguiente rack
static bool fhIA()
{
  FhMem &m = *fhM;
  if (m.despiste)
  {
    m.despiste--;
    return false;
  }
  int izq = FH_BX - fhAncho() / 2;
  int tgy = 58;
  float mx = 1e9f;
  for (int i = 0; i < FH_NR; i++)
    if (m.rk[i].vivo && m.rk[i].x + FH_PW + 2 > izq && m.rk[i].x < mx)
    {
      mx = m.rk[i].x;
      tgy = m.rk[i].gy;
    }
  return (m.y + m.vy + 0.15f) > (float)(tgy + 9);
}

static bool fhChoca()
{
  FhMem &m = *fhM;
  int bw = fhAncho();
  int l = FH_BX - bw / 2 + 3, rr = FH_BX + bw / 2 - 3;
  int t = (int)m.y - 6, b = (int)m.y + 6;
  if (b >= FH_H - FH_SUELO)
    return true;
  for (int i = 0; i < FH_NR; i++)
  {
    const FhRack &r = m.rk[i];
    if (!r.vivo)
      continue;
    int x0 = (int)r.x, gt = r.gy - r.gap / 2, gb = r.gy + r.gap / 2;
    bool cuerpo = rr > x0 && l < x0 + FH_PW;
    bool tapa = rr > x0 - 2 && l < x0 + FH_PW + 2;
    if (t < gt && (cuerpo || (b > gt - FH_CAPH && tapa)))
      return true;
    if (b > gb && (cuerpo || (t < gb + FH_CAPH && tapa)))
      return true;
  }
  return false;
}

static void fhExplota()
{
  FhMem &m = *fhM;
  m.estado = 1;
  m.finTimer = 75;
  m.flash = 3;
  for (int i = 0; i < 36; i++)
  {
    float a = fhRf() * 6.2832f, v = 0.4f + fhRf() * 2.8f;
    fhChispa((float)FH_BX, m.y, cosf(a) * v, sinf(a) * v, 14 + fhRnd(18), 2);
  }
  for (int i = 0; i < 8; i++)
    fhChispa((float)FH_BX, m.y, (fhRf() - 0.5f) * 2.0f, -1.0f - fhRf() * 2.0f, 20 + fhRnd(10), 1);
  if (m.puntos > fhMejor)
  {
    fhMejor = m.puntos;
    m.record = 1;
  }
  Serial.printf("M8AX - FlappyHash: Partida %lu | Racks %lu | Record %lu\n",
                (unsigned long)fhPartidas, (unsigned long)m.puntos, (unsigned long)fhMejor);
}

static void fhActualizar()
{
  FhMem &m = *fhM;
  m.frame++;
  if (m.aleteo)
    m.aleteo--;
  if (m.flash)
    m.flash--;

  // Parallax y estrellas
  m.scF += m.vel * 0.25f;
  m.scN += m.vel * 0.5f;
  m.scS += m.vel;
  for (int i = 0; i < FH_NS; i++)
  {
    FhStar &s = m.es[i];
    s.x -= (0.1f + 0.1f * s.capa) * (m.vel + 0.3f);
    if (s.x < 0)
    {
      s.x += FH_W;
      s.y = (uint8_t)fhRnd(70);
    }
  }

  if (m.estado == 0)
  {
    int pt = m.puntos > 40 ? 40 : (int)m.puntos;
    m.vel += ((1.9f + pt * 0.03f) - m.vel) * 0.05f;

    // Se despista cada tanto ( mas a menudo cuanto mas lejos llega )
    if (!m.despiste && fhRnd(100000) < 60 + m.puntos * 4)
      m.despiste = 10 + fhRnd(18);

    if (fhIA())
    {
      m.vy = FH_SALTO;
      m.aleteo = 6;
      for (int i = 0; i < 3; i++)
        fhChispa((float)FH_BX - 4 + fhRnd(9), m.y + 7, -m.vel * 0.6f + (fhRf() - 0.5f), 0.5f + fhRf() * 0.8f, 8 + fhRnd(6), 1);
    }
    m.vy += FH_G;
    m.y += m.vy;
    if (m.y < 7.0f)
    {
      m.y = 7.0f;
      if (m.vy < 0)
        m.vy = 0;
    }

    // Estela dorada
    fhChispa((float)(FH_BX - fhAncho() / 2), m.y + (fhRf() - 0.5f) * 6.0f, -m.vel - 0.3f, (fhRf() - 0.5f) * 0.3f, 12 + fhRnd(10), 0);

    // Racks
    m.sigX -= m.vel;
    if (m.sigX <= FH_W + 2)
    {
      fhNuevoRack();
      m.sigX += FH_ESP;
    }
    int izq = FH_BX - fhAncho() / 2;
    for (int i = 0; i < FH_NR; i++)
    {
      FhRack &r = m.rk[i];
      if (!r.vivo)
        continue;
      r.x -= m.vel;
      if (r.flash)
        r.flash--;
      if (r.x + FH_PW + 4 < 0)
      {
        r.vivo = 0;
        continue;
      }
      if (!r.pasado && r.x + FH_PW + 2 < izq + 2)
      {
        r.pasado = 1;
        r.flash = 12;
        m.puntos++;
        for (int k = 0; k < 10; k++)
        {
          float a = k * 0.6283f, v = 0.8f + fhRf() * 1.2f;
          fhChispa(r.x + FH_PW / 2, (float)r.gy, cosf(a) * v - m.vel * 0.5f, sinf(a) * v, 14 + fhRnd(10), 3);
        }
      }
    }

    if (fhChoca())
      fhExplota();
  }
  else
  {
    m.vel *= 0.9f;
    if (m.finTimer && --m.finTimer == 0)
      fhNuevaPartida();
  }

  // Particulas
  for (int i = 0; i < FH_NP; i++)
  {
    FhPart &p = m.pt[i];
    if (!p.vida)
      continue;
    p.x += p.vx;
    p.y += p.vy;
    if (p.tipo == 2)
      p.vy += 0.05f;
    p.vida--;
    if (p.x < -2 || p.x > FH_W + 2 || p.y < -2 || p.y > FH_H)
      p.vida = 0;
  }
}

// ---- Pintado: una linea cada vez ----

static void fhRackLinea(uint16_t *L, int y, const FhRack &r, uint32_t frame)
{
  int gt = r.gy - r.gap / 2, gb = r.gy + r.gap / 2, x0 = (int)r.x;
  bool arriba = y < gt, abajo = (y >= gb) && (y < FH_H - FH_SUELO);
  if (!arriba && !abajo)
    return;
  bool tapa = arriba ? (y >= gt - FH_CAPH) : (y < gb + FH_CAPH);
  bool filoGap = arriba ? (y == gt - 1) : (y == gb);
  bool filoExt = tapa && (arriba ? (y == gt - FH_CAPH) : (y == gb + FH_CAPH - 1));
  int xa = tapa ? x0 - 2 : x0, xb = tapa ? x0 + FH_PW + 2 : x0 + FH_PW;
  if (xb <= 0 || xa >= FH_W)
    return;
  uint16_t borde = r.flash ? FH_RGB(110, 255, 150) : (tapa ? FH_RGB(140, 170, 230) : FH_RGB(96, 120, 170));
  uint16_t cuerpo = r.flash ? FH_RGB(40, 90, 66) : (tapa ? FH_RGB(52, 64, 96) : FH_RGB(34, 42, 64));
  if (filoGap || filoExt)
  {
    fhSpan(L, xa, xb, borde);
    return;
  }
  fhSpan(L, xa, xb, borde);
  fhSpan(L, xa + 1, xb - 1, cuerpo);
  if (tapa)
    return;
  int f = y & 7;
  if (f == 0)
    fhSpan(L, xa + 1, xb - 1, FH_RGB(16, 20, 34)); // ranura entre servidores
  else if (f == 3 || f == 4)
  {
    int row = y >> 3;
    for (int k = 0; k < 2; k++)
    {
      uint32_t s = (r.id * 5 + row * 7 + k * 3 + (frame >> 4) * (k + 2)) & 7;
      uint16_t c = s < 5 ? FH_RGB(40, 255, 100) : (s < 7 ? FH_RGB(255, 170, 30) : FH_RGB(255, 50, 40));
      int lx = x0 + 5 + k * 5;
      fhSpan(L, lx, lx + 2, c);
    }
    if (f == 3)
      for (int vx = x0 + 17; vx < x0 + FH_PW - 4; vx += 2)
        fhPx(L, vx, FH_RGB(70, 84, 118)); // rejilla de ventilacion
  }
}

static void fhPajaroLinea(uint16_t *L, int y)
{
  FhMem &m = *fhM;
  if (m.estado)
    return;
  int bw = fhAncho(), izq = FH_BX - bw / 2, top = (int)(m.y + 0.5f) - FH_BH / 2;
  int r = y - top;
  if (r >= -4 && r < 0 && m.aleteo)
  {
    static const uint8_t wd[4] = {1, 3, 5, 6};
    fhSpan(L, izq + 4, izq + 4 + wd[r + 4], FH_RGB(215, 240, 255));
  }
  if (r >= FH_BH && r < FH_BH + 4 && !m.aleteo)
  {
    static const uint8_t wd[4] = {6, 5, 3, 1};
    fhSpan(L, izq + 4, izq + 4 + wd[r - FH_BH], FH_RGB(215, 240, 255));
  }
  if (r < 0 || r >= FH_BH)
    return;
  int ins = FH_PILL[r], a = izq + ins, b = izq + bw - ins;
  uint16_t borde = FH_RGB(140, 60, 0);
  uint16_t cuerpo = r < 4 ? FH_RGB(255, 225, 90) : (r < 10 ? FH_RGB(255, 190, 25) : FH_RGB(235, 135, 10));
  if (r == 0 || r == FH_BH - 1)
    fhSpan(L, a, b, borde);
  else
  {
    fhSpan(L, a, b, cuerpo);
    fhPx(L, a, borde);
    fhPx(L, b - 1, borde);
  }
  if (r >= 2 && r < 12)
  {
    int dr = (r - 2) >> 1, dx = izq + 4;
    for (int k = 0; k < m.nd; k++)
    {
      uint16_t g = FH_DIG[m.dg[k]];
      for (int c = 0; c < 3; c++)
        if ((g >> ((4 - dr) * 3 + (2 - c))) & 1)
          fhSpan(L, dx + k * 8 + c * 2, dx + k * 8 + c * 2 + 2, FH_RGB(70, 25, 0));
    }
  }
}

static void fhLinea(int y, int offF, int offN, int offS, const uint8_t *hF, const uint8_t *hN)
{
  FhMem &m = *fhM;
  uint16_t *L = m.linea;
  const int suelo = FH_H - FH_SUELO;

  // Cielo nocturno con degradado
  uint16_t cielo = m.flash ? FH_RGB(200, 200, 225) : FH_RGB(6 + y * 50 / 110, 8 + y * 14 / 110, 30 + y * 50 / 110);
  uint32_t cc = ((uint32_t)cielo << 16) | cielo;
  uint32_t *d = (uint32_t *)L;
  for (int i = 0; i < FH_W / 2; i++)
    d[i] = cc;

  // Estrellas
  if (!m.flash)
    for (int i = 0; i < FH_NS; i++)
      if (m.es[i].y == y)
      {
        uint16_t c = m.es[i].capa == 0 ? FH_RGB(60, 70, 130) : (m.es[i].capa == 1 ? FH_RGB(130, 140, 190) : FH_RGB(255, 255, 255));
        if (m.es[i].capa == 2 && ((m.frame + i * 7) & 31) < 3)
          c = FH_RGB(120, 120, 160);
        fhPx(L, (int)m.es[i].x, c);
      }

  // Rascacielos lejanos
  {
    int base = -(offF % FH_SEGF);
    for (int s = 0; s < 24 && base + s * FH_SEGF < FH_W; s++)
      if (y >= suelo - hF[s] && y < suelo)
        fhSpan(L, base + s * FH_SEGF, base + (s + 1) * FH_SEGF - 1, FH_RGB(28, 22, 62));
  }

  // Rascacielos cercanos con ventanas
  {
    int base = -(offN % FH_SEGN), s0 = offN / FH_SEGN;
    for (int s = 0; s < 24 && base + s * FH_SEGN < FH_W; s++)
    {
      int t = suelo - hN[s];
      if (y < t || y >= suelo)
        continue;
      int x0 = base + s * FH_SEGN;
      fhSpan(L, x0, x0 + FH_SEGN - 2, FH_RGB(13, 11, 34));
      int ry = y - t;
      if (ry >= 4 && (ry % 7) < 2)
        for (int k = 0; k < 3; k++)
          if ((fhHash((s0 + s) * 5 + k + (ry / 7) * 131) & 3) == 0)
            fhSpan(L, x0 + 4 + k * 8, x0 + 8 + k * 8, FH_RGB(255, 210, 110));
    }
  }

  // Cadena de bloques
  if (y >= suelo)
  {
    int gr = y - suelo, u = offS % 28;
    for (int x = 0; x < FH_W; x++)
    {
      uint16_t c;
      if (u < 24)
      {
        if (gr == 0)
          c = FH_RGB(110, 140, 220);
        else if (gr == 1)
          c = FH_RGB(70, 95, 170);
        else if (gr == FH_SUELO - 1)
          c = FH_RGB(20, 26, 60);
        else if ((gr == 4 || gr == 5) && u >= 5 && u < 10)
          c = FH_RGB(255, 190, 20); // el nonce
        else
          c = FH_RGB(36, 48, 96);
      }
      else
        c = (gr == 4 || gr == 5) ? FH_RGB(90, 120, 200) : FH_RGB(8, 8, 20);
      L[x] = c;
      if (++u == 28)
        u = 0;
    }
  }

  // Racks
  for (int i = 0; i < FH_NR; i++)
    if (m.rk[i].vivo)
      fhRackLinea(L, y, m.rk[i], m.frame);

  // Particulas
  for (int i = 0; i < FH_NP; i++)
  {
    const FhPart &p = m.pt[i];
    if (!p.vida)
      continue;
    int py = (int)p.y;
    bool grande = p.tipo == 2 && p.vida > 16;
    if (py == y || (grande && py + 1 == y))
    {
      uint16_t c = FH_PCOL[p.tipo][p.vida >> 3];
      fhPx(L, (int)p.x, c);
      if (grande)
        fhPx(L, (int)p.x + 1, c);
    }
  }

  fhPajaroLinea(L, y);
}

static void fhPintar()
{
  FhMem &m = *fhM;
  int offF = (int)m.scF, offN = (int)m.scN, offS = (int)m.scS;
  uint8_t hF[24], hN[24];
  int sF = offF / FH_SEGF, sN = offN / FH_SEGN;
  for (int s = 0; s < 24; s++)
  {
    hF[s] = (uint8_t)(14 + fhHash(sF + s) % 34);
    hN[s] = (uint8_t)(10 + fhHash((sN + s) * 3 + 1) % 30);
  }
  tft.startWrite();
  tft.setAddrWindow(0, FH_Y0, FH_W, FH_H);
  for (int y = 0; y < FH_H; y++)
  {
    fhLinea(y, offF, offN, offS, hF, hN);
    tft.pushColors(m.linea, FH_W, true);
  }
  tft.endWrite();
}

static void fhFrame()
{
  fhActualizar();
  fhPintar();
}

// los tres textos de abajo, solo se repintan si cambian
static void fhTextos(bool forzar)
{
  FhMem &m = *fhM;
  char izq[16], cen[14], der[16];
  snprintf(izq, sizeof(izq), "Pts %lu", (unsigned long)m.puntos);
  if (m.estado && m.record)
    snprintf(cen, sizeof(cen), "RECORD!");
  else if (m.estado)
    snprintf(cen, sizeof(cen), "CRASH!");
  else
    snprintf(cen, sizeof(cen), "Rec %lu", (unsigned long)fhMejor);
  snprintf(der, sizeof(der), "Part %lu", (unsigned long)fhPartidas);
  bool a = forzar || strcmp(izq, m.tIzq) != 0, b = forzar || strcmp(cen, m.tCen) != 0, c = forzar || strcmp(der, m.tDer) != 0;
  if (!a && !b && !c)
    return;
  tft.setFreeFont(NULL);
  tft.setTextFont(1);
  tft.setTextSize(2);
  if (a)
  {
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.setTextPadding(100);
    tft.setTextDatum(TL_DATUM);
    tft.drawString(izq, 4, 151);
    strcpy(m.tIzq, izq);
  }
  if (b)
  {
    tft.setTextColor(m.estado ? TFT_ORANGE : TFT_CYAN, TFT_BLACK);
    tft.setTextPadding(90);
    tft.setTextDatum(TC_DATUM);
    tft.drawString(cen, 160, 151);
    strcpy(m.tCen, cen);
  }
  if (c)
  {
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.setTextPadding(96);
    tft.setTextDatum(TR_DATUM);
    tft.drawString(der, 316, 151);
    strcpy(m.tDer, der);
  }
  tft.setTextPadding(0);
  tft.setTextDatum(TL_DATUM);
}
// <<< FLAPPYHASH MOTOR

void esp32_2432S028R_FlappyHashScreen(unsigned long mElapsed)
{
  mineria = getMiningData(mElapsed);
  Serial.printf("M8AX - %s >>> Completados %s Share(s), %s Khashes, Prom. Hashrate %s KH/s %s°\n",
                mineria.currentTime.c_str(), mineria.completedShares.c_str(), mineria.totalKHashes.c_str(), mineria.currentHashRate.c_str(), mineria.temp.c_str());

  bool redraw = hasChangedScreen;
  if (!fhM)
  {
    fhM = (FhMem *)malloc(sizeof(FhMem));
    if (!fhM)
    {
      Serial.println("M8AX - FlappyHash: No Hay RAM Libre");
      delay(1000);
      return;
    }
    fhInit();
    redraw = true;
  }
  const int fhMiPantalla = currentDisplayDriver->current_cyclic_screen;
  fhPonHash((int)(mineria.currentHashRate.toFloat() + 0.5f));
  if (redraw)
  {
    tft.fillRect(0, 0, 320, 170, TFT_BLACK);
    fhTextos(true);
  }
  printPoolData(); // barra de abajo, solo se repinta cuando toca
  hasChangedScreen = false;

  char horaStr[9] = "";
  char lastHora[9] = "";
  char hashStr[16];
  char lastHash[16] = "";

  unsigned long start = millis();
  unsigned long lastMiningUpdate = 0;
  unsigned long lastTouchChk = 0;
  unsigned long siguiente = start;
  pongLeds(false);
  unsigned long ledApagarEn = 0;

  while (millis() - start < FH_SLICE_MS)
  {
    unsigned long now = millis();

    // ---- LEDs: se apagan solos tras el destello ----
    if (ledApagarEn && now >= ledApagarEn)
    {
      pongLeds(false);
      ledApagarEn = 0;
    }

    // ---- Táctil (misma lógica que el Pong) ----
    if (now - lastTouchChk >= 40 && now - previousTouchMillis >= 500)
    {
      lastTouchChk = now;
      int16_t t_x, t_y;
      if (toqueXY(t_x, t_y))
      {
        previousTouchMillis = now;
        if (((t_x > 109) && (t_x < 211)) && ((t_y > 185) && (t_y < 241)))
        {
          bottomScreenBlue ^= true;
          hasChangedScreen = true;
        }
        else if ((t_x > 235) && ((t_y > 0) && (t_y < 16)))
        {
          esp32_2432S028R_AlternateScreenState();
        }
        else if (t_x > 160)
        {
          currentDisplayDriver->current_cyclic_screen = (currentDisplayDriver->current_cyclic_screen + 1) % currentDisplayDriver->num_cyclic_screens;
        }
        else
        {
          currentDisplayDriver->current_cyclic_screen = currentDisplayDriver->current_cyclic_screen - 1;
          if (currentDisplayDriver->current_cyclic_screen < 0)
            currentDisplayDriver->current_cyclic_screen = currentDisplayDriver->num_cyclic_screens - 1;
        }
        break;
      }
    }

    if (now >= siguiente)
    {
      uint32_t ptsAntes = fhM->puntos;
      uint8_t estAntes = fhM->estado;
      fhFrame();

      if (estAntes == 0 && fhM->estado == 1) // muerte: LED rojo
      {
        pongLeds(false);
        digitalWrite(LED_PIN, LOW);
        ledApagarEn = millis() + 300;
      }
      else if (fhM->puntos > ptsAntes) // pasa un edificio: LED verde
      {
        pongLeds(false);
        digitalWrite(LED_PIN_G, LOW);
        ledApagarEn = millis() + 80;
      }

      fhTextos(false);
      siguiente = millis() + FH_FRAME_MS;
    }

    // ---- Hora (izquierda) y hashrate (derecha), arriba como en las demás ----
    if (now - lastMiningUpdate >= 1000)
    {
      lastMiningUpdate = now;
      mineria = getMiningData(mElapsed);
      snprintf(hashStr, sizeof(hashStr), "%s", mineria.currentHashRate.c_str());
      fhPonHash((int)(mineria.currentHashRate.toFloat() + 0.5f)); // el pajaro ES el hashrate
      time_t epoch = (time_t)timeClient.getEpochTime();
      struct tm *ti = localtime(&epoch);
      snprintf(horaStr, sizeof(horaStr), "%02d:%02d:%02d", ti->tm_hour, ti->tm_min, ti->tm_sec);
      if (strcmp(horaStr, lastHora) != 0 || strcmp(hashStr, lastHash) != 0)
      {
        pongDrawHud(hashStr, horaStr);
        strcpy(lastHora, horaStr);
        strcpy(lastHash, hashStr);
      }
    }
    webAtender();
    delay(2);
  }

  pongLeds(false);
  tft.setTextPadding(0);
  tft.setTextDatum(TL_DATUM);

  if (currentDisplayDriver->current_cyclic_screen != fhMiPantalla)
  {
    free(fhM);
    fhM = NULL;
  }

  cuentita++;
  if (cuentita == 15)
  {
    ajustarZonaHoraria();
    obtenerLocYTemp();
    obtenerVariacionesBTC(variacion1dia, variacion1semana);
  }
}

/****************************************************************************************************************
 *
 *   Escrito por: M8AX
 *
 *   Descripción: ECG HASHRATE - Pantalla para ESP32_2432S028R
 *   -----------------------------------------------------------------------
 *
 *   Un electrocardiograma con los últimos 30 minutos de hashrate. Cada píxel de ancho es una muestra
 *   ( 294 muestras x 6,122 s = 30 min ) y el trazo entra por la derecha y se va desplazando a la izquierda.
 *
 *   - Rejilla de papel de ECG: una línea vertical cada 5 minutos y líneas horizontales con su escala en KH/s
 *     ( la escala se ajusta sola a lo que haya minado en los últimos 30 minutos ).
 *   - Línea ámbar discontinua en ECG_UMBRAL ( 350 KH/s ). Por debajo es zona roja.
 *   - Trazo verde por encima del umbral y rojo por debajo, con relleno degradado debajo.
 *   - Barrido tipo monitor: un destello blanco con cola recorre el trazo de izquierda a derecha.
 *   - Punto en la última muestra que late, y un corazón arriba con los BPM ( BPM = KH/s x ECG_BPM_K,
 *     así que 350 KH/s son 70 BPM ).
 *   - Diagnóstico abajo: RITMO SINUSAL ( > umbral ), ARRITMIA! ( por debajo ) o ASISTOLIA!! ( 0 KH/s ).
 *   - Máximo y mínimo de los últimos 30 minutos.
 *
 *   LEDs: verde, un parpadeo cada segundo, mientras el hashrate esté por encima de ECG_UMBRAL. Por debajo,
 *   rojo con doble latido "lub-dub" cada segundo. Con 0 KH/s, rojo fijo ( electro plano, pitido continuo ).
 *
 *   El historial se guarda en ecgMuestrea(), que mide solo cada ECG_MS. Si además la llamas desde
 *   analiCadaSegundo, el historial se sigue llenando aunque estés en otra pantalla.
 *
 *   Para activarla hay que pegar este bloque antes de analiCadaSegundo y meter
 *   esp32_2432S028R_ECGScreen en el array esp32_2432S028RCyclicScreens[].
 *
 *   Ajustes: ECG_UMBRAL, ECG_BPM_K, ECG_SWEEP_MS ( velocidad del barrido ) y los colores ECG_C_*.
 *
 ****************************************************************************************************************/

#define ECG_N 294         // muestras en pantalla = columnas del trazo ( 294 = 6 bloques de 49 px, 5 min cada uno )
#define ECG_MS 6122       // ms entre muestras ( 294 x 6,122 s = 30 min )
#define ECG_UMBRAL 350.0f // KH/s: por encima es "sano" ( verde ), por debajo "arritmia" ( rojo )
#define ECG_BPM_K 0.2f    // BPM = KH/s x esto ( 350 KH/s -> 70 BPM )
#define ECG_PX 26         // x donde empieza el trazo ( a la izquierda va la escala )
#define ECG_PY 20         // y donde empieza el trazo ( encima va la cabecera )
#define ECG_PH 124        // alto del trazo
#define ECG_SLICE_MS 5000 // tandas largas, como en las demás
#define ECG_SWEEP_MS 25   // ms entre pasos del barrido
#define ECG_SWEEP_PASO 3  // píxeles que avanza el barrido en cada paso
#define ECG_COLA 8        // largo de la cola del barrido

#define ECG_C_VERDE 0x5FEA // trazo bueno ( verde fósforo )
#define ECG_C_ROJO 0xFA28  // trazo malo
#define ECG_C_AMBAR 0xFD20 // línea del umbral
#define ECG_C_GRIDH 0x01A0 // rejilla horizontal
#define ECG_C_GRIDV 0x0260 // rejilla vertical ( cada 5 min )
#define ECG_C_ZONA 0x1800  // fondo de la zona por debajo del umbral
#define ECG_C_GRIS 0x7BEF  // textos pequeños

static float ecgH[ECG_N];        // historial ( anillo ), KH/s
static uint16_t ecgHead = 0;     // donde se escribe la próxima muestra
static uint16_t ecgCount = 0;    // cuántas muestras hay ( máx. ECG_N )
static uint32_t ecgSeq = 0;      // sube con cada muestra nueva
static unsigned long ecgUlt = 0; // cuándo se tomó la última
static bool ecgInit = false;
static uint32_t ecgSeqVisto = 0;

static int16_t ecgY[ECG_N]; // fila de cada columna ( 0 = arriba ), -1 si todavía no hay muestra
static bool ecgOk[ECG_N];   // true si esa muestra supera el umbral
static int16_t ecgYUmb = 0; // fila del umbral
static int16_t ecgGY[10];   // filas de la rejilla horizontal
static float ecgGV[10];     // valor ( KH/s ) de cada fila
static uint8_t ecgNG = 0;
static float ecgMin = 0, ecgMax = 0;
static int ecgSw = 0;       // columna del barrido ( negativo = en pausa )
static bool ecgLat = false; // el punto de la última muestra está "latiendo"

// Toma una muestra cada ECG_MS. Se puede llamar todo lo seguido que se quiera.
static void ecgMuestrea()
{
  unsigned long ahora = millis();
  if (ecgUlt != 0 && ahora - ecgUlt < ECG_MS)
    return;
  if (mineria.currentHashRate.length() == 0)
    return;
  float h = mineria.currentHashRate.toFloat();
  if (h <= 0 && ecgCount == 0)
    return; // el historial empieza con el primer hashrate real
  ecgUlt = ahora;
  ecgH[ecgHead] = h;
  ecgHead = (ecgHead + 1) % ECG_N;
  if (ecgCount < ECG_N)
    ecgCount++;
  ecgSeq++;
}

// Muestra de la columna c ( 0 = la más vieja, ECG_N - 1 = la más nueva ). -1 si todavía no hay.
static float ecgMuestra(int c)
{
  int edad = ECG_N - 1 - c;
  if (edad >= ecgCount)
    return -1.0f;
  return ecgH[((int)ecgHead - 1 - edad + 2 * ECG_N) % ECG_N];
}

static uint16_t ecgMezcla(uint16_t a, uint16_t b, uint8_t t) // t = 0 -> a, 255 -> b
{
  int ra = (a >> 11) & 31, ga = (a >> 5) & 63, ba = a & 31;
  int rb = (b >> 11) & 31, gb = (b >> 5) & 63, bb = b & 31;
  int r = ra + ((rb - ra) * t) / 255;
  int g = ga + ((gb - ga) * t) / 255;
  int bl = ba + ((bb - ba) * t) / 255;
  return (uint16_t)((r << 11) | (g << 5) | bl);
}

static int ecgFila(float v, float lo, float hi)
{
  int y = (int)lroundf((ECG_PH - 1) - (v - lo) / (hi - lo) * (ECG_PH - 1));
  return y < 0 ? 0 : (y > ECG_PH - 1 ? ECG_PH - 1 : y);
}

// Calcula la escala ( redondeada para que las etiquetas no bailen ), la rejilla y la fila de cada columna
static void ecgEscala()
{
  float dmin = 1e9f, dmax = -1e9f;
  for (int c = 0; c < ECG_N; c++)
  {
    float v = ecgMuestra(c);
    if (v < 0)
      continue;
    if (v < dmin)
      dmin = v;
    if (v > dmax)
      dmax = v;
  }
  bool hay = (dmax >= 0);
  ecgMin = hay ? dmin : 0;
  ecgMax = hay ? dmax : 0;
  if (!hay)
  {
    dmin = ECG_UMBRAL - 50;
    dmax = ECG_UMBRAL + 50;
  }

  float lo = dmin < ECG_UMBRAL ? dmin : ECG_UMBRAL; // el umbral siempre se ve
  float hi = dmax > ECG_UMBRAL ? dmax : ECG_UMBRAL;
  if (hi - lo < 30)
  {
    float m = (lo + hi) / 2;
    lo = m - 15;
    hi = m + 15;
  }
  float margen = (hi - lo) * 0.12f;
  lo -= margen;
  hi += margen;
  if (lo < 0)
    lo = 0;

  static const float pasos[] = {1, 2, 5, 10, 20, 25, 50, 100, 200, 500, 1000, 2000, 5000};
  float paso = 5000;
  for (unsigned i = 0; i < sizeof(pasos) / sizeof(pasos[0]); i++)
    if ((hi - lo) / pasos[i] <= 6)
    {
      paso = pasos[i];
      break;
    }
  lo = floorf(lo / paso) * paso;
  hi = ceilf(hi / paso) * paso;
  if (hi - lo < paso)
    hi = lo + paso;

  ecgNG = 0;
  for (float g = lo; g <= hi + 0.01f && ecgNG < 10; g += paso)
  {
    ecgGV[ecgNG] = g;
    ecgGY[ecgNG] = ecgFila(g, lo, hi);
    ecgNG++;
  }
  ecgYUmb = ecgFila(ECG_UMBRAL, lo, hi);

  for (int c = 0; c < ECG_N; c++)
  {
    float v = ecgMuestra(c);
    ecgY[c] = (v >= 0) ? ecgFila(v, lo, hi) : -1;
    ecgOk[c] = (v >= ECG_UMBRAL);
  }
}

// Pinta una columna entera ( fondo, relleno, rejilla, umbral, trazo y punto ) y la manda de un golpe
static void ecgPintaCol(int c)
{
  uint16_t col[ECG_PH];
  const int yc = ecgY[c];
  const int dist = ecgSw - c; // distancia al barrido ( 0 = justo debajo )

  for (int i = 0; i < ECG_PH; i++)
    col[i] = (dist >= 0 && dist <= 1) ? 0x0220 : ((i > ecgYUmb) ? ECG_C_ZONA : 0x0000);

  if (yc >= 0) // relleno degradado debajo del trazo
  {
    uint16_t A, B, C;
    if (ecgOk[c])
    {
      A = 0x03A0;
      B = 0x0240;
      C = 0x0120;
    }
    else
    {
      A = 0x4800;
      B = 0x3000;
      C = 0x1800;
    }
    for (int i = yc; i < ECG_PH; i++)
      col[i] = (i - yc < 8) ? A : ((i - yc < 28) ? B : C);
  }

  if ((c % 3) == 0)
    for (int g = 0; g < ecgNG; g++)
      col[ecgGY[g]] = ECG_C_GRIDH;
  if ((c % 49) == 0)
    for (int i = 0; i < ECG_PH; i += 3)
      col[i] = ECG_C_GRIDV;
  if ((c % 6) < 3)
    col[ecgYUmb] = ECG_C_AMBAR; // umbral discontinuo

  if (yc >= 0) // trazo: tramo vertical que une con las columnas vecinas, así la línea no tiene huecos
  {
    int y0 = yc, y1 = yc;
    if (c > 0 && ecgY[c - 1] >= 0)
      y0 = (ecgY[c - 1] + yc) / 2;
    if (c < ECG_N - 1 && ecgY[c + 1] >= 0)
      y1 = (yc + ecgY[c + 1]) / 2;
    int a = y0 < y1 ? y0 : y1;
    int b = y0 > y1 ? y0 : y1;
    if (yc < a)
      a = yc;
    if (yc > b)
      b = yc;
    uint16_t core = ecgOk[c] ? ECG_C_VERDE : ECG_C_ROJO;
    int hl = (dist >= 0 && dist <= ECG_COLA) ? ECG_COLA - dist : 0; // destello del barrido
    if (hl)
      core = ecgMezcla(core, 0xFFFF, hl * 28);
    uint16_t brillo = ecgMezcla(core, 0x0000, 170);
    if (hl >= 3)
    {
      a--;
      b++;
    }
    for (int i = a - 1; i <= b + 1; i++)
      if (i >= 0 && i < ECG_PH)
        col[i] = (i >= a && i <= b) ? core : brillo;
  }

  const int hy = ecgY[ECG_N - 1]; // punto de la última muestra ( late con los BPM )
  if (hy >= 0 && c >= ECG_N - 7)
  {
    const int R = ecgLat ? 4 : 2;
    const int dx = c - (ECG_N - 1);
    uint16_t halo = ecgMezcla(ecgOk[ECG_N - 1] ? ECG_C_VERDE : ECG_C_ROJO, 0x0000, 110);
    for (int i = hy - 6; i <= hy + 6; i++)
    {
      if (i < 0 || i >= ECG_PH)
        continue;
      int d2 = dx * dx + (i - hy) * (i - hy);
      if (d2 <= R * R)
        col[i] = 0xFFFF;
      else if (ecgLat && d2 <= (R + 2) * (R + 2))
        col[i] = halo;
    }
  }

  tft.startWrite();
  tft.setAddrWindow(ECG_PX + c, ECG_PY, 1, ECG_PH);
  tft.pushColors(col, ECG_PH, true);
  tft.endWrite();
}

// Escala a la izquierda, marcas de tiempo y máx / mín debajo del trazo
static void ecgEjes()
{
  char b[16];
  tft.fillRect(0, ECG_PY, ECG_PX, ECG_PH, TFT_BLACK);
  tft.fillRect(0, ECG_PY + ECG_PH + 1, 320, 8, TFT_BLACK);
  tft.setFreeFont(NULL);
  tft.setTextFont(1);
  tft.setTextSize(1);
  tft.setTextPadding(0);

  tft.setTextDatum(MR_DATUM);
  tft.setTextColor(ECG_C_GRIS, TFT_BLACK);
  for (int g = 0; g < ecgNG; g++)
  {
    int y = ECG_PY + ecgGY[g];
    if (abs(y - (ECG_PY + ecgYUmb)) < 8)
      continue; // no pisar la etiqueta del umbral
    snprintf(b, sizeof(b), "%d", (int)(ecgGV[g] + 0.5f));
    tft.drawString(b, ECG_PX - 3, y);
  }
  snprintf(b, sizeof(b), "%d", (int)ECG_UMBRAL);
  tft.setTextColor(ECG_C_AMBAR, TFT_BLACK);
  tft.drawString(b, ECG_PX - 3, ECG_PY + ecgYUmb);

  const int yT = ECG_PY + ECG_PH + 1;
  tft.setTextDatum(TL_DATUM);
  tft.setTextColor(ECG_C_GRIS, TFT_BLACK);
  tft.drawString("-30m", ECG_PX, yT);
  tft.drawString("-15m", ECG_PX + (ECG_N / 2) - 12, yT);

  tft.setTextDatum(TR_DATUM);
  if (ecgCount > 0)
  {
    snprintf(b, sizeof(b), "MIN %d", (int)(ecgMin + 0.5f));
    tft.setTextColor(ECG_C_ROJO, TFT_BLACK);
    tft.drawString(b, 319, yT);
    int w = 6 * (int)strlen(b) + 8;
    snprintf(b, sizeof(b), "MAX %d", (int)(ecgMax + 0.5f));
    tft.setTextColor(ECG_C_VERDE, TFT_BLACK);
    tft.drawString(b, 319 - w, yT);
  }
  else
  {
    tft.setTextDatum(MC_DATUM);
    tft.setTextColor(ECG_C_AMBAR, TFT_BLACK);
    tft.drawString("ESPERANDO HASHRATE...", ECG_PX + ECG_N / 2, ECG_PY + ECG_PH / 2);
  }
  tft.setTextDatum(TL_DATUM);
}

// Repinta todo el trazo ( cuando llega una muestra nueva o al entrar en la pantalla )
static void ecgPintaTodo()
{
  ecgEscala();
  for (int c = 0; c < ECG_N; c++)
    ecgPintaCol(c);
  ecgEjes();
}

static void ecgCorazon(bool grande, uint16_t color)
{
  const int cx = 214;
  const int r = grande ? 5 : 4;
  const int base = 6 + (r >> 1);
  tft.fillRect(cx - 13, 0, 26, 19, TFT_BLACK);
  tft.fillCircle(cx - r, 6, r, color);
  tft.fillCircle(cx + r, 6, r, color);
  tft.fillTriangle(cx - 2 * r, base, cx + 2 * r, base, cx, base + 2 * r, color);
}

static void ecgCabecera(const char *bpm, uint16_t col)
{
  tft.setFreeFont(NULL);
  tft.setTextFont(1);
  tft.setTextSize(2);
  tft.setTextPadding(0);
  tft.setTextDatum(TL_DATUM);
  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.drawString("ECG HASHRATE 30M", 4, 2);
  tft.setTextDatum(TR_DATUM);
  tft.setTextColor(col, TFT_BLACK);
  tft.setTextPadding(84);
  tft.drawString(bpm, 316, 2);
  tft.setTextPadding(0);
  tft.setTextDatum(TL_DATUM);
  tft.drawFastHLine(0, 19, 320, 0x0410);
}

static void ecgHud(const char *izq, uint16_t colIzq, const char *der)
{
  tft.setFreeFont(NULL);
  tft.setTextFont(1);
  tft.setTextSize(2);
  tft.setTextPadding(150);
  tft.setTextDatum(TL_DATUM);
  tft.setTextColor(colIzq, TFT_BLACK);
  tft.drawString(izq, 4, 153);
  tft.setTextDatum(TR_DATUM);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString(der, 316, 153);
  tft.setTextPadding(0);
  tft.setTextDatum(TL_DATUM);
}

// bit0 = rojo, bit1 = verde. fase = ms dentro del segundo actual
static uint8_t ecgLedMask(int clase, unsigned long fase)
{
  if (clase == 2) // sinusal: un destello verde cada segundo
    return (fase < 120) ? 2 : 0;
  if (clase == 1) // arritmia: rojo, doble latido lub-dub
    return (fase < 70 || (fase >= 170 && fase < 240)) ? 1 : 0;
  return 1; // asistolia: rojo fijo
}

void esp32_2432S028R_ECGScreen(unsigned long mElapsed)
{
  mineria = getMiningData(mElapsed);
  Serial.printf("M8AX - %s >>> Completados %s Share(s), %s Khashes, Prom. Hashrate %s KH/s %s°\n",
                mineria.currentTime.c_str(), mineria.completedShares.c_str(), mineria.totalKHashes.c_str(), mineria.currentHashRate.c_str(), mineria.temp.c_str());

  bool redraw = hasChangedScreen;
  if (!ecgInit)
  {
    ecgInit = true;
    redraw = true;
  }

  ecgMuestrea();
  if (redraw)
  {
    tft.fillRect(0, 0, 320, 170, TFT_BLACK);
    ecgSw = 0;
    ecgLat = false;
    ecgSeqVisto = ecgSeq;
    ecgPintaTodo();
  }
  printPoolData(); // pie de las criptos (solo se repinta cuando toca)
  hasChangedScreen = false;

  float hash = mineria.currentHashRate.toFloat();
  char hashStr[16];
  snprintf(hashStr, sizeof(hashStr), "%s", mineria.currentHashRate.c_str());
  char izq[24], der[24], bpmS[16];
  char lastIzq[24] = "", lastDer[24] = "", lastBpm[16] = "";
  int lastClase = -1, lastBeat = -1;
  uint8_t lastLed = 255;

  unsigned long start = millis();
  unsigned long lastTxt = 0;
  unsigned long lastSweep = 0;
  unsigned long lastMiningUpdate = 0;
  unsigned long lastTouchChk = 0;
  pongLeds(false); // LEDs apagados para empezar

  while (millis() - start < ECG_SLICE_MS)
  {
    unsigned long now = millis();

    // ---- Tactil (misma logica que esp32_2432S028R_DoLedStuff) ----
    if (now - lastTouchChk >= 40 && now - previousTouchMillis >= 500)
    {
      lastTouchChk = now;
      int16_t t_x, t_y;
      if (toqueXY(t_x, t_y))
      {
        previousTouchMillis = now;
        if (((t_x > 109) && (t_x < 211)) && ((t_y > 185) && (t_y < 241)))
        {
          bottomScreenBlue ^= true;
          hasChangedScreen = true;
        }
        else if ((t_x > 235) && ((t_y > 0) && (t_y < 16)))
        {
          esp32_2432S028R_AlternateScreenState();
        }
        else if (t_x > 160)
        {
          currentDisplayDriver->current_cyclic_screen = (currentDisplayDriver->current_cyclic_screen + 1) % currentDisplayDriver->num_cyclic_screens;
        }
        else
        {
          currentDisplayDriver->current_cyclic_screen = currentDisplayDriver->current_cyclic_screen - 1;
          if (currentDisplayDriver->current_cyclic_screen < 0)
            currentDisplayDriver->current_cyclic_screen = currentDisplayDriver->num_cyclic_screens - 1;
        }
        break;
      }
    }

    // ---- Muestra nueva: el trazo se desplaza un paso a la izquierda ----
    ecgMuestrea();
    if (ecgSeqVisto != ecgSeq)
    {
      ecgSeqVisto = ecgSeq;
      ecgPintaTodo();
    }

    // ---- Hashrate (cada segundo) ----
    if (now - lastMiningUpdate >= 1000)
    {
      lastMiningUpdate = now;
      mineria = getMiningData(mElapsed);
      hash = mineria.currentHashRate.toFloat();
      snprintf(hashStr, sizeof(hashStr), "%s", mineria.currentHashRate.c_str());
    }
    const int clase = (hash > ECG_UMBRAL) ? 2 : ((hash > 0) ? 1 : 0); // 2 sinusal, 1 arritmia, 0 asistolia
    const uint16_t colEstado = (clase == 2) ? ECG_C_VERDE : ECG_C_ROJO;

    // ---- LEDs: verde cada segundo si > umbral, si no rojo ----
    uint8_t led = ecgLedMask(clase, now % 1000);
    if (led != lastLed)
    {
      lastLed = led;
      digitalWrite(LED_PIN, (led & 1) ? LOW : HIGH);
      digitalWrite(LED_PIN_G, (led & 2) ? LOW : HIGH);
      digitalWrite(LED_PIN_B, HIGH);
    }

    // ---- Latido: corazón de la cabecera y punto de la última muestra ----
    int bpm = 0;
    if (hash > 0)
    {
      bpm = (int)(hash * ECG_BPM_K + 0.5f);
      bpm = bpm < 20 ? 20 : (bpm > 220 ? 220 : bpm);
    }
    int beat = 0;
    if (bpm > 0)
    {
      unsigned long per = 60000UL / bpm;
      unsigned long ph = (now - start) % per;
      beat = (ph < 100) || (ph >= 170 && ph < 240);
    }
    if (beat != lastBeat)
    {
      lastBeat = beat;
      ecgLat = beat;
      ecgCorazon(beat, bpm > 0 ? 0xF800 : 0x4208);
      if (ecgCount > 0)
        for (int c = ECG_N - 8; c < ECG_N; c++)
          ecgPintaCol(c);
    }

    // ---- Barrido: destello con cola que recorre el trazo ----
    if (ecgCount > 0 && now - lastSweep >= ECG_SWEEP_MS)
    {
      lastSweep = now;
      int desde = ecgSw - ECG_COLA;
      ecgSw += ECG_SWEEP_PASO;
      int hasta = ecgSw;
      if (ecgSw > ECG_N + ECG_COLA)
      {
        ecgSw = -20; // pequeña pausa y vuelta a empezar
        hasta = ECG_N - 1;
      }
      desde = desde < 0 ? 0 : desde;
      hasta = hasta > ECG_N - 1 ? ECG_N - 1 : hasta;
      for (int c = desde; c <= hasta; c++)
        ecgPintaCol(c);
    }

    // ---- Textos ( 5 veces por segundo, solo si cambian ) ----
    if (now - lastTxt >= 200)
    {
      lastTxt = now;
      snprintf(izq, sizeof(izq), "%s", clase == 2 ? "RITMO SINUSAL" : (clase == 1 ? "ARRITMIA!" : "ASISTOLIA!!"));
      snprintf(der, sizeof(der), "%s KH/s", hashStr);
      snprintf(bpmS, sizeof(bpmS), "%d BPM", bpm);
      if (clase != lastClase || strcmp(izq, lastIzq) != 0 || strcmp(der, lastDer) != 0)
      {
        ecgHud(izq, colEstado, der);
        strcpy(lastIzq, izq);
        strcpy(lastDer, der);
      }
      if (clase != lastClase || strcmp(bpmS, lastBpm) != 0)
      {
        ecgCabecera(bpmS, colEstado);
        strcpy(lastBpm, bpmS);
      }
      lastClase = clase;
    }

    webAtender();
    delay(5);
  }

  pongLeds(false); // por si salimos con los LEDs encendidos
  tft.setTextPadding(0);
  tft.setTextDatum(TL_DATUM);

  cuentita++;
  if (cuentita == 15)
  {
    ajustarZonaHoraria(), obtenerLocYTemp();
    obtenerVariacionesBTC(variacion1dia, variacion1semana);
  }
}

/****************************************************************************************************************
 *
 *   Escrito por: M8AX
 *
 *   Descripción: FRACTAL 3D M8AX - Fractales en tres dimensiones trazados rayo a rayo por el ESP32_2432S028R
 *   --------------------------------------------------------------------------------------------------------
 *
 *   Para cada punto de la pantalla se lanza un rayo desde una cámara que da vueltas alrededor de un fractal
 *   que no existe en ningún sitio: solo hay una fórmula que dice a qué distancia está la superficie desde
 *   cualquier punto del espacio. El rayo avanza esa distancia, vuelve a preguntar, avanza otra vez... y así
 *   hasta que se pega a la superficie o se pierde. Con eso se calcula la normal ( cuatro preguntas más ), la
 *   luz, una oclusión ambiental gratis sacada de cuántos pasos ha costado llegar y un brillo de neón alrededor
 *   para los rayos que pasan rozando. Los colores salen de la órbita que ha hecho el punto dentro de la fórmula.
 *
 *   Hay cuatro vistas, de 26 segundos cada una: el Mandelbulb de potencia 8 en magenta y en oro ( con la fórmula
 *   sin trigonometría, que va unas cuatro veces más rápida ) y la esponja de Menger en azul hielo y en verde.
 *   Se pinta de arriba abajo, fila a fila, directamente a la pantalla: no hay framebuffer, ni una sola imagen
 *   en memoria, así que cada fotograma se ve como se va calculando. Ojo, que esto es tiempo real a lo bruto, no
 *   esperes 60 fps: cada fotograma son unos 10.000 rayos con decenas de evaluaciones cada uno.
 *
 *   Abajo: nombre de la vista, miles de evaluaciones de la fórmula por segundo y los millones que lleva.
 *
 *   LEDs: un destello al acabar cada fotograma con el color de la vista, y los tres a la vez al cambiar de vista.
 *
 *   Memoria: menos de 1 KB en el heap ( la línea que se manda a la pantalla y cuatro variables ), que se pide al
 *   entrar en la pantalla y se libera al salir. Si no hay RAM, se salta la pantalla sin pintar nada.
 *
 *   Para activarla: pegar este bloque justo antes de analiCadaSegundo, meter esp32_2432S028R_Fractal3DScreen en el
 *   array esp32_2432S028RCyclicScreens[] y añadir en analiCadaSegundo, debajo del if de atrM, este otro:
 *   if (f3M && pantallaActual != esp32_2432S028R_Fractal3DScreen) { free(f3M); f3M = NULL; }
 *
 ****************************************************************************************************************/

// >>> FRACTAL 3D MOTOR
#define F3_W 160
#define F3_H 60
#define F3_Y0 28 // el HUD de arriba ocupa hasta la 26 y el de abajo empieza en la 150
#define F3_SLICE_MS 5000
#define F3_VISTA_MS 26000
#define F3_MAXPASOS 56

struct F3Mem
{
  uint16_t linea[F3_W * 2];
  float ox, oy, oz, fx, fy, fz, rx, ry, rz, ux, uy, uz, lx, ly, lz;
  float ang, elev;
  uint8_t vista, nuevo;
  int fila;
  uint32_t fotograma, pasos, pasosPrev, vel, tVista, tSeg, ledOff;
  char tNom[12], tVel[20], tIt[20];
};
static F3Mem *f3M = NULL;

// por vista: escena ( 0 bulbo, 1 esponja ), paleta, nombre
static const uint8_t F3_ESC[4] = {0, 1, 0, 1};
static const uint8_t F3_PAL[4] = {0, 1, 2, 0};
static const char *const F3_NOM[4] = {"Bulbo 3D", "Menger 3D", "Bulbo Oro", "Esponja"};
// por escena: distancia de la cámara y radio de la esfera que lo envuelve
static const float F3_RCAM[2] = {3.7f, 4.9f};
static const float F3_RBOL[2] = {1.35f, 1.85f};
static const float F3_PO[3][3] = {{0.00f, 0.33f, 0.67f}, {0.00f, 0.12f, 0.25f}, {0.55f, 0.40f, 0.20f}};

static inline float f3Min(float a, float b) { return a < b ? a : b; }
static inline float f3Max(float a, float b) { return a > b ? a : b; }
static inline float f3Cl(float v, float a, float b) { return v < a ? a : (v > b ? b : v); }

// Mandelbulb de potencia 8 sin senos ni cosenos ( el polinomio ya desarrollado ), devuelve la distancia estimada
static float f3Bulbo(float px, float py, float pz, float *trap)
{
  float wx = px, wy = py, wz = pz;
  float m = wx * wx + wy * wy + wz * wz;
  float dz = 1.0f, tr = m;
  for (int i = 0; i < 4; i++)
  {
    dz = 8.0f * m * m * m * sqrtf(m) * dz + 1.0f;
    float x = wx, y = wy, z = wz;
    float x2 = x * x, y2 = y * y, z2 = z * z;
    float x4 = x2 * x2, y4 = y2 * y2, z4 = z2 * z2;
    float k3 = x2 + z2;
    float k2 = 1.0f / sqrtf(k3 * k3 * k3 * k3 * k3 * k3 * k3);
    float k1 = x4 + y4 + z4 - 6.0f * y2 * z2 - 6.0f * x2 * y2 + 2.0f * z2 * x2;
    float k4 = x2 - y2 + z2;
    wx = px + 64.0f * x * y * z * (x2 - z2) * k4 * (x4 - 6.0f * x2 * z2 + z4) * k1 * k2;
    wy = py - 16.0f * y2 * k3 * k4 * k4 + k1 * k1;
    wz = pz - 8.0f * y * k4 * (x4 * x4 - 28.0f * x4 * x2 * z2 + 70.0f * x4 * z4 - 28.0f * x2 * z2 * z4 + z4 * z4) * k1 * k2;
    m = wx * wx + wy * wy + wz * wz;
    if (m < tr)
      tr = m;
    if (m > 256.0f)
      break;
  }
  *trap = tr;
  return 0.25f * logf(m) * sqrtf(m) / dz;
}

static float f3Menger(float px, float py, float pz, float *trap)
{
  float ax = fabsf(px) - 1.0f, ay = fabsf(py) - 1.0f, az = fabsf(pz) - 1.0f;
  float ox = f3Max(ax, 0.0f), oy = f3Max(ay, 0.0f), oz = f3Max(az, 0.0f);
  float d = sqrtf(ox * ox + oy * oy + oz * oz) + f3Min(f3Max(ax, f3Max(ay, az)), 0.0f);
  float s = 1.0f, tr = 1e9f;
  for (int i = 0; i < 4; i++)
  {
    float qx = px * s, qy = py * s, qz = pz * s;
    qx = qx - 2.0f * floorf(qx * 0.5f) - 1.0f;
    qy = qy - 2.0f * floorf(qy * 0.5f) - 1.0f;
    qz = qz - 2.0f * floorf(qz * 0.5f) - 1.0f;
    s *= 3.0f;
    float rx = fabsf(1.0f - 3.0f * fabsf(qx)), ry = fabsf(1.0f - 3.0f * fabsf(qy)), rz = fabsf(1.0f - 3.0f * fabsf(qz));
    float da = f3Max(rx, ry), db = f3Max(ry, rz), dc = f3Max(rz, rx);
    float c = (f3Min(da, f3Min(db, dc)) - 1.0f) / s;
    if (c > d)
      d = c;
    float q2 = qx * qx + qy * qy + qz * qz;
    if (q2 < tr)
      tr = q2;
  }
  *trap = tr;
  return d;
}

static inline float f3DE(int esc, float x, float y, float z, float *trap)
{
  return esc == 0 ? f3Bulbo(x, y, z, trap) : f3Menger(x, y, z, trap);
}

static void f3Pal(float t, int p, float *r, float *g, float *b)
{
  const float DOSPI = 6.2831853f;
  *r = 0.5f + 0.5f * cosf(DOSPI * (t + F3_PO[p][0]));
  *g = 0.5f + 0.5f * cosf(DOSPI * (t + F3_PO[p][1]));
  *b = 0.5f + 0.5f * cosf(DOSPI * (t + F3_PO[p][2]));
}

static inline uint16_t f3Rgb(float r, float g, float b)
{
  r = f3Cl(r, 0.0f, 1.0f);
  g = f3Cl(g, 0.0f, 1.0f);
  b = f3Cl(b, 0.0f, 1.0f);
  return ((int)(r * 31.0f) << 11) | ((int)(g * 63.0f) << 5) | (int)(b * 31.0f);
}

// pone la cámara en órbita y deja calculada su base y la luz ( que viaja con la cámara )
static void f3Camara()
{
  F3Mem &m = *f3M;
  const float R = F3_RCAM[F3_ESC[m.vista]];
  float ce = cosf(m.elev);
  m.ox = R * ce * sinf(m.ang);
  m.oy = R * sinf(m.elev);
  m.oz = R * ce * cosf(m.ang);
  float l = 1.0f / sqrtf(m.ox * m.ox + m.oy * m.oy + m.oz * m.oz);
  m.fx = -m.ox * l;
  m.fy = -m.oy * l;
  m.fz = -m.oz * l;
  l = 1.0f / sqrtf(m.fx * m.fx + m.fz * m.fz);
  m.rx = -m.fz * l;
  m.ry = 0.0f;
  m.rz = m.fx * l;
  m.ux = m.ry * m.fz - m.rz * m.fy;
  m.uy = m.rz * m.fx - m.rx * m.fz;
  m.uz = m.rx * m.fy - m.ry * m.fx;
  float lx = -0.5f * m.rx + 0.8f * m.ux - 0.6f * m.fx;
  float ly = -0.5f * m.ry + 0.8f * m.uy - 0.6f * m.fy;
  float lz = -0.5f * m.rz + 0.8f * m.uz - 0.6f * m.fz;
  l = 1.0f / sqrtf(lx * lx + ly * ly + lz * lz);
  m.lx = lx * l;
  m.ly = ly * l;
  m.lz = lz * l;
}

// una fila entera: traza los rayos, la deja en linea[] y la manda dos veces a la pantalla ( cada punto es 2x2 )
static void f3Fila(int j)
{
  F3Mem &m = *f3M;
  const int esc = F3_ESC[m.vista], pal = F3_PAL[m.vista];
  const float rb2 = F3_RBOL[esc] * F3_RBOL[esc];
  const float c0 = m.ox * m.ox + m.oy * m.oy + m.oz * m.oz - rb2;
  const float sy = -((j + 0.5f) / F3_H * 2.0f - 1.0f) * 0.5f;
  const float foc = 1.7f;
  uint32_t pasos = 0;
  uint16_t *l = m.linea;
  for (int i = 0; i < F3_W; i++)
  {
    float sx = ((i + 0.5f) / F3_W * 2.0f - 1.0f) * 1.3333f;
    float dx = m.fx * foc + m.rx * sx + m.ux * sy;
    float dy = m.fy * foc + m.ry * sx + m.uy * sy;
    float dz = m.fz * foc + m.rz * sx + m.uz * sy;
    float il = 1.0f / sqrtf(dx * dx + dy * dy + dz * dz);
    dx *= il;
    dy *= il;
    dz *= il;
    float fondo = 0.5f + 0.5f * dy;
    float r = 0.02f + 0.05f * fondo, g = 0.02f, b = 0.06f + 0.10f * fondo;
    float bq = m.ox * dx + m.oy * dy + m.oz * dz;
    float h = bq * bq - c0;
    if (h > 0.0f)
    {
      h = sqrtf(h);
      float t = -bq - h, tmax = -bq + h;
      if (t < 0.0f)
        t = 0.0f;
      float minD = 1e9f, trap = 0.0f, px = 0.0f, py = 0.0f, pz = 0.0f;
      int n, hit = 0;
      for (n = 0; n < F3_MAXPASOS; n++)
      {
        px = m.ox + dx * t;
        py = m.oy + dy * t;
        pz = m.oz + dz * t;
        float d = f3DE(esc, px, py, pz, &trap);
        if (d < 0.0012f * t)
        {
          hit = 1;
          break;
        }
        if (d < minD)
          minD = d;
        t += d;
        if (t > tmax)
          break;
      }
      pasos += n + 1;
      if (hit)
      {
        // la normal sale de cuatro preguntas alrededor del punto ( tetraedro )
        static const float K[4][3] = {{1, -1, -1}, {-1, -1, 1}, {-1, 1, -1}, {1, 1, 1}};
        float e = 0.002f * t + 0.0004f, tt, nx = 0.0f, ny = 0.0f, nz = 0.0f;
        for (int q = 0; q < 4; q++)
        {
          float dd = f3DE(esc, px + K[q][0] * e, py + K[q][1] * e, pz + K[q][2] * e, &tt);
          nx += K[q][0] * dd;
          ny += K[q][1] * dd;
          nz += K[q][2] * dd;
        }
        pasos += 4;
        float nl = 1.0f / (sqrtf(nx * nx + ny * ny + nz * nz) + 1e-9f);
        nx *= nl;
        ny *= nl;
        nz *= nl;
        float nd = nx * m.lx + ny * m.ly + nz * m.lz;
        float dif = f3Max(nd, 0.0f);
        float ao = 1.0f - (float)n / F3_MAXPASOS;
        ao *= ao;
        float luz = (0.35f + 0.25f * ny) * ao + 0.9f * dif * (0.4f + 0.6f * ao);
        float sp = f3Max(2.0f * nd * (nx * dx + ny * dy + nz * dz) - (dx * m.lx + dy * m.ly + dz * m.lz), 0.0f);
        sp *= sp;
        sp *= sp;
        sp *= sp;
        float tc = esc == 1 ? 0.52f + 0.22f * ny + 0.12f * nx + 0.04f * t : sqrtf(trap) * 0.9f + 0.15f * t;
        float cr, cg, cb;
        f3Pal(tc, pal, &cr, &cg, &cb);
        float brillo = 0.35f * sp * ao;
        r = cr * luz + brillo;
        g = cg * luz + brillo;
        b = cb * luz + brillo;
      }
      else
      {
        // el rayo se ha ido, pero si ha pasado cerca del fractal se lleva un poco de neón
        float glow = expf(-minD * 14.0f) * 0.9f, gr, gg, gb;
        f3Pal(0.1f, pal, &gr, &gg, &gb);
        r += gr * glow;
        g += gg * glow;
        b += gb * glow;
      }
    }
    uint16_t c = f3Rgb(r, g, b);
    l[0] = c;
    l[1] = c;
    l += 2;
  }
  m.pasos += pasos;
  tft.startWrite();
  tft.setAddrWindow(0, F3_Y0 + j * 2, F3_W * 2, 2);
  tft.pushColors(m.linea, F3_W * 2, true);
  tft.pushColors(m.linea, F3_W * 2, true);
  tft.endWrite();
}

static void f3Init()
{
  F3Mem &m = *f3M;
  memset(m.linea, 0, sizeof(m.linea));
  m.vista = esp_random() % 4;
  m.ang = (esp_random() % 628) / 100.0f;
  m.elev = 0.3f;
  m.fila = 0;
  m.fotograma = 0;
  m.pasos = m.pasosPrev = m.vel = m.tSeg = m.ledOff = 0;
  m.nuevo = 1;
  m.tVel[0] = m.tIt[0] = 0;
  strcpy(m.tNom, F3_NOM[m.vista]);
  m.tVista = millis();
  f3Camara();
}

// destello de los LEDs, los pines son activos en LOW
static void f3Led(uint8_t vista, bool todos)
{
  static const uint8_t RGB[4][3] = {{1, 0, 1}, {0, 1, 1}, {1, 1, 0}, {0, 1, 0}};
  digitalWrite(LED_PIN, (todos || RGB[vista][0]) ? LOW : HIGH);
  digitalWrite(LED_PIN_G, (todos || RGB[vista][1]) ? LOW : HIGH);
  digitalWrite(LED_PIN_B, (todos || RGB[vista][2]) ? LOW : HIGH);
  f3M->ledOff = millis() + (todos ? 220 : 60);
}

// avanza la cámara cuando se acaba un fotograma y cambia de vista si ya toca
static void f3Siguiente()
{
  F3Mem &m = *f3M;
  m.fila = 0;
  m.fotograma++;
  m.ang += 0.045f;
  m.elev = 0.12f + 0.32f * sinf(m.fotograma * 0.037f);
  if (millis() - m.tVista >= F3_VISTA_MS)
  {
    m.vista = (m.vista + 1) % 4;
    m.tVista = millis();
    m.nuevo = 1;
    strcpy(m.tNom, F3_NOM[m.vista]);
    Serial.printf("M8AX - Fractal 3D: %s\n", m.tNom);
    f3Led(m.vista, true);
  }
  else
    f3Led(m.vista, false);
  f3Camara();
}

// los tres textos de abajo, solo se repintan si cambian
static void f3Textos(bool forzar)
{
  F3Mem &m = *f3M;
  char vel[20], it[20];
  snprintf(vel, sizeof(vel), "%lu K/s", (unsigned long)m.vel);
  snprintf(it, sizeof(it), "%lu.%02lu M", (unsigned long)(m.pasos / 1000000UL), (unsigned long)((m.pasos / 10000UL) % 100UL));
  bool a = forzar || m.nuevo, b = forzar || strcmp(vel, m.tVel) != 0, c = forzar || strcmp(it, m.tIt) != 0;
  if (!a && !b && !c)
    return;
  tft.setFreeFont(NULL);
  tft.setTextFont(1);
  tft.setTextSize(2);
  if (a)
  {
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.setTextPadding(110);
    tft.setTextDatum(TL_DATUM);
    tft.drawString(m.tNom, 4, 151);
    m.nuevo = 0;
  }
  if (b)
  {
    tft.setTextColor(TFT_CYAN, TFT_BLACK);
    tft.setTextPadding(84);
    tft.setTextDatum(TC_DATUM);
    tft.drawString(vel, 160, 151);
    strcpy(m.tVel, vel);
  }
  if (c)
  {
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.setTextPadding(110);
    tft.setTextDatum(TR_DATUM);
    tft.drawString(it, 316, 151);
    strcpy(m.tIt, it);
  }
  tft.setTextPadding(0);
  tft.setTextDatum(TL_DATUM);
}
// <<< FRACTAL 3D MOTOR

void esp32_2432S028R_Fractal3DScreen(unsigned long mElapsed)
{
  mineria = getMiningData(mElapsed);
  Serial.printf("M8AX - %s >>> Completados %s Share(s), %s Khashes, Prom. Hashrate %s KH/s %s°\n",
                mineria.currentTime.c_str(), mineria.completedShares.c_str(), mineria.totalKHashes.c_str(), mineria.currentHashRate.c_str(), mineria.temp.c_str());

  bool redraw = hasChangedScreen;
  if (!f3M)
  {
    f3M = (F3Mem *)malloc(sizeof(F3Mem));
    if (!f3M)
    {
      Serial.println("M8AX - Fractal 3D: No Hay RAM Libre");
      delay(1000);
      return;
    }
    f3Init();
    redraw = true;
  }
  const int f3MiPantalla = currentDisplayDriver->current_cyclic_screen;
  if (redraw)
  {
    tft.fillRect(0, 0, 320, 170, TFT_BLACK);
    f3Textos(true);
  }
  printPoolData(); // barra de abajo, solo se repinta cuando toca
  hasChangedScreen = false;

  char horaStr[9] = "";
  char lastHora[9] = "";
  char hashStr[16];
  char lastHash[16] = "";

  unsigned long start = millis();
  unsigned long lastMiningUpdate = 0;
  unsigned long lastTouchChk = 0;
  pongLeds(false);
  f3M->ledOff = 0;

  while (millis() - start < F3_SLICE_MS)
  {
    unsigned long now = millis();

    // ---- Táctil (misma lógica que el Pong) ----
    if (now - lastTouchChk >= 40 && now - previousTouchMillis >= 500)
    {
      lastTouchChk = now;
      int16_t t_x, t_y;
      if (toqueXY(t_x, t_y))
      {
        previousTouchMillis = now;
        if (((t_x > 109) && (t_x < 211)) && ((t_y > 185) && (t_y < 241)))
        {
          bottomScreenBlue ^= true;
          hasChangedScreen = true;
        }
        else if ((t_x > 235) && ((t_y > 0) && (t_y < 16)))
        {
          esp32_2432S028R_AlternateScreenState();
        }
        else if (t_x > 160)
        {
          currentDisplayDriver->current_cyclic_screen = (currentDisplayDriver->current_cyclic_screen + 1) % currentDisplayDriver->num_cyclic_screens;
        }
        else
        {
          currentDisplayDriver->current_cyclic_screen = currentDisplayDriver->current_cyclic_screen - 1;
          if (currentDisplayDriver->current_cyclic_screen < 0)
            currentDisplayDriver->current_cyclic_screen = currentDisplayDriver->num_cyclic_screens - 1;
        }
        break;
      }
    }

    if (f3M->ledOff && now >= f3M->ledOff)
    {
      pongLeds(false);
      f3M->ledOff = 0;
    }

    // ---- A trazar rayos, una fila por vuelta ----
    f3Fila(f3M->fila);
    f3M->fila++;
    if (f3M->fila >= F3_H)
      f3Siguiente();
    if (now - f3M->tSeg >= 1000)
    {
      f3M->vel = (f3M->pasos - f3M->pasosPrev) / 1000UL;
      f3M->pasosPrev = f3M->pasos;
      f3M->tSeg = now;
      f3Textos(false);
    }

    // ---- Hora (izquierda) y hashrate (derecha), arriba como en las demás ----
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
        pongDrawHud(hashStr, horaStr);
        strcpy(lastHora, horaStr);
        strcpy(lastHash, hashStr);
      }
    }
    webAtender();
    delay(1);
  }

  pongLeds(false);
  tft.setTextPadding(0);
  tft.setTextDatum(TL_DATUM);

  if (currentDisplayDriver->current_cyclic_screen != f3MiPantalla)
  {
    free(f3M);
    f3M = NULL;
  }

  cuentita++;
  if (cuentita == 15)
  {
    ajustarZonaHoraria();
    obtenerLocYTemp();
    obtenerVariacionesBTC(variacion1dia, variacion1semana);
  }
}

/********************************************************************************************
 *  PANTALLA ESTACION METEOROLOGICA - El tiempo de donde esta el minero + todo el hashrate
 *
 *  La localidad se saca sola de la IP publica (ip-api.com, sin clave) y el tiempo de
 *  Open-Meteo (https://open-meteo.com, gratis y sin clave). Se pide cada 10 minutos y la
 *  localizacion cada 6 horas, asi que si mueves el minero de sitio se entera solo.
 *
 *  Izquierda: icono dibujado con formas ( sol, luna, nubes, lluvia, nieve, tormenta, niebla ),
 *  temperatura, estado del cielo, sensacion, presion, punto de rocio, nubes, viento y rachas
 *  con direccion, lluvia y probabilidad, UV maximo, salida y puesta del sol y prevision de
 *  hoy y de los 2 dias siguientes ( max / min / prob. de lluvia ). Abajo a la izquierda va la luna
 *  con el mismo dibujo que las otras pantallas ( dibujarPorcentajeLunar ), su fase y el porcentaje
 *  iluminado con 3 decimales. La luna se calcula en local, funciona aunque no haya internet.
 *  La salida y la puesta del sol van en la cabecera, entre la localidad y la hora.
 *
 *  Derecha: hashrate actual en grande ( verde > 300, amarillo > 0, rojo = 0 ), maximo, minimo,
 *  media de los ultimos minutos, shares, bloques validos, mejor dificultad, total de KH,
 *  plantillas, temperatura de la CPU, tiempo minando y una grafica con el hashrate de los
 *  ultimos 10 minutos ( se sigue rellenando aunque estes en otra pantalla ).
 *
 *  Abajo se mantiene la barra de precios de criptos como en el resto de pantallas normales.
 *
 *  LEDs: igual que en la pantalla del minero ( manejandoLeds ), con todo su comportamiento:
 *  hashrate, noche, fin de semana, bloque minado y 10 de marzo.
 *
 *  RAM: nada de sprites. Se pinta directo con tft y solo se repinta cada linea si cambia su
 *  texto. Estado fijo total unos 450 bytes. La peticion HTTPS solo se lanza si hay memoria
 *  contigua suficiente ( WX_HEAP_MIN ) y todo lo temporal se libera al terminar.
 *
 *  Ajustes: WX_REFRESCO_MS, WX_GEO_REFRESCO_MS y, si la IP te sitúa mal ( pasa con algunos
 *  operadores ), WX_LAT_FIJA / WX_LON_FIJA / WX_CIUDAD_FIJA para fijar la localidad a mano.
 ********************************************************************************************/

#define WX_REFRESCO_MS 600000UL       // tiempo cada 10 minutos
#define WX_REINTENTO_MS 60000UL       // si falla, reintenta al minuto
#define WX_GEO_REFRESCO_MS 21600000UL // localidad cada 6 horas
#define WX_HEAP_MIN 36000             // bytes contiguos minimos para lanzar la peticion HTTPS
#define WX_HN 77                      // muestras de la grafica ( 2 px cada una )
#define WX_HIST_SEG 8                 // segundos entre muestras ( 77 x 8 s = unos 10 min )
#define WX_LAT_FIJA 999.0f            // 999 = automatico por IP. Pon tu latitud para fijarla
#define WX_LON_FIJA 999.0f
#define WX_CIUDAD_FIJA ""

struct WxDia
{
  int8_t tmax, tmin;
  uint8_t pp, code;
};

struct WxDatos
{
  bool ok;
  bool dia;
  uint8_t hum, nubes, code;
  int16_t wdir;
  float t, sens, pres, viento, racha, lluvia, uv;
  char sale[6], pone[6];
  WxDia d[3];
  unsigned long tFetch;
};

static WxDatos wxd = {};
static float wxLat = 0, wxLon = 0;
static char wxCiudad[24] = "";
static bool wxGeoOk = false, wxFalloUlt = false, wxPintadoOk = false, wxForce = false;
static unsigned long wxTGeo = 0, wxTIntento = 0;
static uint16_t wxHist[WX_HN]; // KH/s x 10
static uint8_t wxHead = 0, wxCount = 0;
static uint16_t wxSeq = 0, wxSeqPint = 0xFFFF;
static uint32_t wxHsh[32];
static char wxLunaTxt[12] = "";
static uint8_t wxIconKey = 255;

// Se llama cada segundo desde analiCadaSegundo(), asi la grafica se llena en cualquier pantalla
void wxHistMuestrea(void)
{
  static unsigned long ult = 0;
  unsigned long ahora = millis();
  if (ahora - ult < WX_HIST_SEG * 1000UL)
    return;
  ult = ahora;
  float h = mineria.currentHashRate.toFloat() * 10.0f;
  if (h < 0)
    h = 0;
  if (h > 65535)
    h = 65535;
  wxHist[wxHead] = (uint16_t)h;
  wxHead = (wxHead + 1) % WX_HN;
  if (wxCount < WX_HN)
    wxCount++;
  wxSeq++;
}

static void wxStats(uint16_t &lo, uint16_t &hi, float &avg)
{
  lo = 65535;
  hi = 0;
  uint32_t s = 0;
  for (uint8_t i = 0; i < wxCount; i++)
  {
    uint16_t v = wxHist[(wxHead + WX_HN - wxCount + i) % WX_HN];
    if (v < lo)
      lo = v;
    if (v > hi)
      hi = v;
    s += v;
  }
  if (wxCount)
    avg = s / (10.0f * wxCount);
  else
  {
    lo = 0;
    avg = 0;
  }
}

// UTF-8 a ASCII ( la fuente de la pantalla no tiene acentos )
static void wxAscii(char *dst, size_t n, const char *src)
{
  static const char up[] = "AAAAAAACEEEEIIIIDNOOOOOxOUUUUYTs";
  static const char lo[] = "aaaaaaaceeeeiiiidnooooo/ouuuuyty";
  size_t k = 0;
  for (size_t i = 0; src[i] && k < n - 1; i++)
  {
    uint8_t c = (uint8_t)src[i];
    if (c == 0xC3 && src[i + 1])
    {
      uint8_t d = (uint8_t)src[++i];
      c = (d >= 0x80 && d <= 0x9F) ? up[d - 0x80] : ((d >= 0xA0 && d <= 0xBF) ? lo[d - 0xA0] : '?');
    }
    else if (c >= 0x80)
      continue;
    dst[k++] = (char)toupper(c);
  }
  dst[k] = 0;
}

static bool wxGeo()
{
  if (WX_LAT_FIJA < 900.0f)
  {
    wxLat = WX_LAT_FIJA;
    wxLon = WX_LON_FIJA;
    wxAscii(wxCiudad, sizeof(wxCiudad), WX_CIUDAD_FIJA);
    wxGeoOk = true;
    wxTGeo = millis();
    return true;
  }
  bool ok = false;
  HTTPClient http;
  http.setConnectTimeout(5000);
  http.setTimeout(5000);
  http.begin("http://ip-api.com/json/?fields=status,city,lat,lon&lang=es"); // sin IP = la del que pregunta
  int code = http.GET();
  if (code == HTTP_CODE_OK)
  {
    StaticJsonDocument<384> doc;
    DeserializationError e = deserializeJson(doc, http.getString());
    if (!e && strcmp(doc["status"] | "", "success") == 0 && !doc["lat"].isNull() && !doc["lon"].isNull())
    {
      wxLat = doc["lat"].as<float>();
      wxLon = doc["lon"].as<float>();
      wxAscii(wxCiudad, sizeof(wxCiudad), doc["city"] | "");
      wxGeoOk = true;
      wxTGeo = millis();
      ok = true;
      Serial.printf("M8AX - Tiempo: Localidad %s ( %.3f, %.3f )\n", wxCiudad, wxLat, wxLon);
    }
  }
  else
    Serial.printf("M8AX - Tiempo: Error Geolocalizacion HTTP %d\n", code);
  http.end();
  return ok;
}

static int wxGet(const char *url, bool seguro, String &body)
{
  int code = -1;
  if (seguro)
  {
    WiFiClientSecure client;
    client.setInsecure();
    HTTPClient http;
    http.setConnectTimeout(6000);
    http.setTimeout(6000);
    if (!http.begin(client, url))
      return -1;
    code = http.GET();
    if (code == HTTP_CODE_OK)
      body = http.getString();
    http.end();
  }
  else
  {
    WiFiClient client;
    HTTPClient http;
    http.setConnectTimeout(6000);
    http.setTimeout(6000);
    if (!http.begin(client, url))
      return -1;
    code = http.GET();
    if (code == HTTP_CODE_OK)
      body = http.getString();
    http.end();
  }
  return code;
}

static bool wxFetch()
{
  String body;
  bool obtenido = false;
  char url[480];

  for (uint8_t intento = 0; intento < 2 && !obtenido; intento++)
  {
    bool seguro = (intento == 1);
    if (seguro && ESP.getMaxAllocHeap() < WX_HEAP_MIN)
    {
      Serial.printf("M8AX - Tiempo: Poca RAM Contigua ( %u ) Para HTTPS\n", (unsigned)ESP.getMaxAllocHeap());
      break;
    }
    snprintf(url, sizeof(url),
             "%s://api.open-meteo.com/v1/forecast?latitude=%.4f&longitude=%.4f"
             "&current=temperature_2m,relative_humidity_2m,apparent_temperature,is_day,precipitation,"
             "weather_code,cloud_cover,surface_pressure,wind_speed_10m,wind_direction_10m,wind_gusts_10m"
             "&daily=weather_code,temperature_2m_max,temperature_2m_min,precipitation_probability_max,"
             "uv_index_max,sunrise,sunset&timezone=auto&forecast_days=3",
             seguro ? "https" : "http", wxLat, wxLon);
    body = "";
    int code = wxGet(url, seguro, body);
    if (code == HTTP_CODE_OK && body.length() > 50)
      obtenido = true;
    else
      Serial.printf("M8AX - Tiempo: Error Open-Meteo %s HTTP %d\n", seguro ? "HTTPS" : "HTTP", code);
  }
  if (!obtenido)
    return false;

  StaticJsonDocument<96> filtro;
  filtro["current"] = true;
  filtro["daily"] = true;
  DynamicJsonDocument doc(2560);
  DeserializationError e = deserializeJson(doc, body, DeserializationOption::Filter(filtro));
  body = "";
  if (e)
  {
    Serial.printf("M8AX - Tiempo: Error Al Parsear JSON ( %s )\n", e.c_str());
    return false;
  }
  JsonObject c = doc["current"];
  JsonObject d = doc["daily"];
  if (c.isNull() || d.isNull())
    return false;
  wxd.t = c["temperature_2m"] | 0.0f;
  wxd.sens = c["apparent_temperature"] | 0.0f;
  wxd.hum = c["relative_humidity_2m"] | 0;
  wxd.dia = (c["is_day"] | 1) != 0;
  wxd.lluvia = c["precipitation"] | 0.0f;
  wxd.code = c["weather_code"] | 0;
  wxd.nubes = c["cloud_cover"] | 0;
  wxd.pres = c["surface_pressure"] | 0.0f;
  wxd.viento = c["wind_speed_10m"] | 0.0f;
  wxd.wdir = c["wind_direction_10m"] | 0;
  wxd.racha = c["wind_gusts_10m"] | 0.0f;
  wxd.uv = d["uv_index_max"][0] | 0.0f;
  for (int i = 0; i < 3; i++)
  {
    wxd.d[i].code = d["weather_code"][i] | 0;
    wxd.d[i].tmax = (int8_t)lroundf(d["temperature_2m_max"][i] | 0.0f);
    wxd.d[i].tmin = (int8_t)lroundf(d["temperature_2m_min"][i] | 0.0f);
    wxd.d[i].pp = d["precipitation_probability_max"][i] | 0;
  }
  const char *sr = d["sunrise"][0] | "";
  const char *ss = d["sunset"][0] | "";
  strlcpy(wxd.sale, strlen(sr) >= 16 ? sr + 11 : "--:--", sizeof(wxd.sale));
  strlcpy(wxd.pone, strlen(ss) >= 16 ? ss + 11 : "--:--", sizeof(wxd.pone));
  wxd.ok = true;
  wxd.tFetch = millis();
  Serial.printf("M8AX - Tiempo: %s %.1fC Hum %d%% Viento %.0f km/h Pres %.0f hPa\n", wxCiudad, wxd.t, wxd.hum, wxd.viento, wxd.pres);
  return true;
}

static uint8_t wxKind(uint8_t c)
{
  if (c == 0)
    return 0; // despejado
  if (c <= 2)
    return 1; // poco nuboso
  if (c == 3)
    return 2; // nublado
  if (c == 45 || c == 48)
    return 3; // niebla
  if ((c >= 51 && c <= 67) || (c >= 80 && c <= 82))
    return 4; // lluvia
  if ((c >= 71 && c <= 77) || c == 85 || c == 86)
    return 5; // nieve
  if (c >= 95)
    return 6; // tormenta
  return 2;
}

static const char *wxTxt(uint8_t c)
{
  switch (c)
  {
  case 0:
    return "DESPEJADO";
  case 1:
    return "POCO NUBOSO";
  case 2:
    return "PARC. NUBOSO";
  case 3:
    return "NUBLADO";
  case 45:
  case 48:
    return "NIEBLA";
  case 51:
  case 53:
  case 55:
    return "LLOVIZNA";
  case 56:
  case 57:
    return "LLOV. HELADA";
  case 61:
    return "LLUVIA DEBIL";
  case 63:
    return "LLUVIA";
  case 65:
    return "LLUVIA FUERTE";
  case 66:
  case 67:
    return "LLUVIA HELADA";
  case 71:
    return "NIEVE DEBIL";
  case 73:
    return "NIEVE";
  case 75:
    return "NIEVE FUERTE";
  case 77:
    return "GRANIZO FINO";
  case 80:
    return "CHUBASCOS";
  case 81:
  case 82:
    return "CHUBASC FUERT";
  case 85:
  case 86:
    return "CHUBASC NIEVE";
  case 95:
    return "TORMENTA";
  case 96:
  case 99:
    return "TORM+GRANIZO";
  }
  return "---";
}

static void wxNube(int x, int y, uint16_t c)
{
  tft.fillCircle(x + 14, y + 30, 8, c);
  tft.fillCircle(x + 25, y + 24, 11, c);
  tft.fillCircle(x + 36, y + 31, 7, c);
  tft.fillRect(x + 14, y + 31, 22, 8, c);
}

static void wxIcono(int x, int y, uint8_t kind, bool dia)
{
  tft.fillRect(x, y, 50, 48, TFT_BLACK);
  switch (kind)
  {
  case 0: // sol o luna
    if (dia)
    {
      tft.fillCircle(x + 24, y + 24, 10, TFT_YELLOW);
      for (int k = 0; k < 8; k++)
      {
        float a = k * 0.785398f;
        tft.drawLine(x + 24 + (int)(15 * cosf(a)), y + 24 + (int)(15 * sinf(a)),
                     x + 24 + (int)(21 * cosf(a)), y + 24 + (int)(21 * sinf(a)), TFT_YELLOW);
      }
    }
    else
    {
      tft.fillCircle(x + 24, y + 24, 13, TFT_LIGHTGREY);
      tft.fillCircle(x + 31, y + 18, 11, TFT_BLACK);
    }
    break;
  case 1: // sol/luna con nube
    if (dia)
      tft.fillCircle(x + 17, y + 15, 9, TFT_YELLOW);
    else
    {
      tft.fillCircle(x + 17, y + 15, 9, TFT_LIGHTGREY);
      tft.fillCircle(x + 22, y + 11, 8, TFT_BLACK);
    }
    wxNube(x + 4, y + 8, TFT_WHITE);
    break;
  case 2: // nublado
    wxNube(x, y + 4, TFT_LIGHTGREY);
    break;
  case 3: // niebla
    wxNube(x, y - 4, TFT_LIGHTGREY);
    for (int i = 0; i < 3; i++)
      tft.drawFastHLine(x + 6 + (i % 2) * 4, y + 36 + i * 4, 32, TFT_LIGHTGREY);
    break;
  case 4: // lluvia
    wxNube(x, y - 4, TFT_LIGHTGREY);
    for (int i = 0; i < 3; i++)
      tft.drawLine(x + 15 + i * 10, y + 37, x + 12 + i * 10, y + 46, TFT_CYAN);
    break;
  case 5: // nieve
    wxNube(x, y - 4, TFT_LIGHTGREY);
    for (int i = 0; i < 3; i++)
      tft.fillCircle(x + 14 + i * 10, y + 41 + (i % 2) * 4, 2, TFT_WHITE);
    break;
  default: // tormenta
    wxNube(x, y - 6, TFT_DARKGREY);
    tft.fillTriangle(x + 28, y + 28, x + 19, y + 40, x + 26, y + 40, TFT_YELLOW);
    tft.fillTriangle(x + 26, y + 40, x + 22, y + 47, x + 32, y + 33, TFT_YELLOW);
    break;
  }
}

static uint16_t wxColorT(float t)
{
  if (t < 5)
    return TFT_CYAN;
  if (t < 12)
    return TFT_SKYBLUE;
  if (t < 20)
    return TFT_GREENYELLOW;
  if (t < 28)
    return TFT_YELLOW;
  if (t < 35)
    return TFT_ORANGE;
  return TFT_RED;
}

// Fase de la luna a partir del % iluminado y de si esta creciendo o menguando
static const char *wxFase(double p, bool crece)
{
  if (p < 2.0)
    return "NUEVA";
  if (p >= 98.0)
    return "LLENA";
  if (p >= 45.0 && p <= 55.0)
    return crece ? "CUARTO CREC." : "CUARTO MENG.";
  if (p < 45.0)
    return crece ? "CRECIENTE" : "MENGUANTE";
  return crece ? "GIB.CRECIENTE" : "GIB.MENGUANTE";
}

// Pinta una linea solo si ha cambiado ( hash del texto ), sin parpadeo gracias al padding
static void wxPut(uint8_t i, int x, int y, int w, uint8_t sz, uint16_t col, const char *s, uint8_t datum = TL_DATUM)
{
  uint32_t h = 2166136261u ^ col ^ ((uint32_t)sz << 16);
  for (const char *p = s; *p; p++)
  {
    h ^= (uint8_t)*p;
    h *= 16777619u;
  }
  if (!wxForce && h == wxHsh[i])
    return;
  wxHsh[i] = h;
  tft.setFreeFont(NULL);
  tft.setTextFont(1);
  tft.setTextSize(sz);
  tft.setTextColor(col, TFT_BLACK);
  tft.setTextPadding(w);
  tft.setTextDatum(datum);
  tft.drawString(s, x, y);
  tft.setTextPadding(0);
  tft.setTextDatum(TL_DATUM);
}

void esp32_2432S028R_WeatherScreen(unsigned long mElapsed)
{
  mineria = getMiningData(mElapsed);
  if (hasChangedScreen)
  {
    tft.fillRect(0, 0, 320, 170, TFT_BLACK);
    tft.drawFastHLine(0, 17, 320, TFT_DARKGREY);
    tft.drawFastHLine(0, 155, 320, TFT_DARKGREY);
    tft.drawFastVLine(162, 18, 137, TFT_DARKGREY);
    wxForce = true;
    wxPintadoOk = false;
    wxIconKey = 255;
    wxSeqPint = 0xFFFF;
  }
  printPoolData();
  hasChangedScreen = false;

  unsigned long ahora = millis();
  time_t epoch = (time_t)timeClient.getEpochTime();
  struct tm ti;
  localtime_r(&epoch, &ti);
  char b[40];

  // Cabecera: localidad, antiguedad del dato y hora
  snprintf(b, sizeof(b), "%.10s", wxGeoOk ? wxCiudad : "BUSCANDO...");
  wxPut(0, 2, 1, 120, 2, TFT_YELLOW, b);
  if (wxd.ok)
    snprintf(b, sizeof(b), "SOL^%s v%s", wxd.sale, wxd.pone);
  else
    snprintf(b, sizeof(b), "SOL^--:-- v--:--");
  wxPut(27, 124, 0, 96, 1, TFT_GOLD, b);
  if (wxd.ok)
    snprintf(b, sizeof(b), "ACT %lum", (ahora - wxd.tFetch) / 60000UL);
  else
    snprintf(b, sizeof(b), "ACT --");
  wxPut(1, 126, 8, 90, 1, TFT_DARKGREY, b);
  snprintf(b, sizeof(b), "%02d:%02d:%02d", ti.tm_hour, ti.tm_min, ti.tm_sec);
  wxPut(2, 318, 1, 96, 2, TFT_WHITE, b, TR_DATUM);

  // Descarga: bloquea 2-4 s cada 10 minutos, se avisa en pantalla
  bool conectado = (WiFi.status() == WL_CONNECTED);
  unsigned long espera = (wxd.ok && !wxFalloUlt) ? WX_REFRESCO_MS : WX_REINTENTO_MS;
  if (conectado && (wxTIntento == 0 || ahora - wxTIntento >= espera))
  {
    wxPut(3, 2, 30, 156, 1, TFT_ORANGE, "ACTUALIZANDO TIEMPO...");
    wxTIntento = ahora;
    if (!wxGeoOk || ahora - wxTGeo >= WX_GEO_REFRESCO_MS)
      wxGeo();
    wxFalloUlt = !(wxGeoOk && wxFetch());
    wxHsh[3] = 0;
    wxPintadoOk = false; // el aviso tapo el icono: se repinta todo el panel del tiempo
  }

  if (!wxd.ok)
  {
    wxPut(3, 2, 30, 156, 1, TFT_ORANGE, conectado ? (wxTIntento ? "SIN DATOS DEL TIEMPO" : "CONECTANDO...") : "SIN WIFI");
  }
  else
  {
    if (!wxPintadoOk)
    {
      tft.fillRect(0, 18, 162, 137, TFT_BLACK);
      wxPintadoOk = true;
      wxForce = true;
      wxIconKey = 255;
    }
    uint8_t key = wxKind(wxd.code) | (wxd.dia ? 0x10 : 0);
    if (key != wxIconKey)
    {
      wxIcono(2, 18, wxKind(wxd.code), wxd.dia);
      wxIconKey = key;
    }
    snprintf(b, sizeof(b), "%.1fC", wxd.t);
    wxPut(4, 52, 22, 108, 3, wxColorT(wxd.t), b);
    snprintf(b, sizeof(b), "HUM %d%%", wxd.hum);
    wxPut(5, 52, 49, 108, 2, TFT_CYAN, b);
    wxPut(6, 2, 66, 156, 2, TFT_YELLOW, wxTxt(wxd.code));

    static const char *rosa[] = {"N", "NE", "E", "SE", "S", "SO", "O", "NO"};
    float tr = wxd.t, hr = wxd.hum > 0 ? wxd.hum : 1;
    float g = logf(hr / 100.0f) + 17.62f * tr / (243.12f + tr);
    float rocio = 243.12f * g / (17.62f - g);
    snprintf(b, sizeof(b), "SENS %.1fC  PRES %.0fhPa", wxd.sens, wxd.pres);
    wxPut(7, 2, 84, 156, 1, TFT_WHITE, b);
    snprintf(b, sizeof(b), "ROCIO %.1fC  NUBES %d%%", rocio, wxd.nubes);
    wxPut(8, 2, 94, 156, 1, TFT_WHITE, b);
    snprintf(b, sizeof(b), "VIENTO %.0f R%.0f km/h %s", wxd.viento, wxd.racha, rosa[((int)((wxd.wdir + 22.5f) / 45.0f)) & 7]);
    wxPut(9, 2, 104, 156, 1, TFT_GREENYELLOW, b);
    snprintf(b, sizeof(b), "LLUVIA %.1fmm %d%% UV %.1f", wxd.lluvia, wxd.d[0].pp, wxd.uv);
    wxPut(10, 2, 114, 156, 1, wxd.uv >= 6 ? TFT_ORANGE : TFT_CYAN, b);

    static const char *dias[] = {"DOM", "LUN", "MAR", "MIE", "JUE", "VIE", "SAB"};
    for (int i = 0; i < 3; i++)
    {
      snprintf(b, sizeof(b), "%s %d/%dC LL:%d%%", i == 0 ? "HOY" : dias[(ti.tm_wday + i) % 7], wxd.d[i].tmax, wxd.d[i].tmin, wxd.d[i].pp);
      if (strlen(b) * 6 > 104) // no cabe (negativas o 100 % con 2 digitos): version corta
        snprintf(b, sizeof(b), "%s %d/%d %d%%", i == 0 ? "HOY" : dias[(ti.tm_wday + i) % 7], wxd.d[i].tmax, wxd.d[i].tmin, wxd.d[i].pp);
      wxPut(14 + i, 2 + i * 106, 160, 104, 1, wxd.d[i].pp >= 50 ? TFT_CYAN : TFT_WHITE, b);
    }
  }

  // Luna: calculo local, no necesita internet. Mismo dibujo que en las otras pantallas
  {
    moonData_t luna = mymoonPhase.getPhase(epoch);
    moonData_t luna2 = mymoonPhase.getPhase(epoch + 3600); // dentro de 1 hora: sirve para saber si crece
    double pl = luna.percentLit * 100.0;
    bool crece = luna2.percentLit > luna.percentLit;
    char pct[12];
    snprintf(pct, sizeof(pct), "%.3f%%", pl);
    if (wxForce || strcmp(pct, wxLunaTxt) != 0)
    {
      strlcpy(wxLunaTxt, pct, sizeof(wxLunaTxt));
      tft.fillRect(0, 124, 33, 30, TFT_BLACK);
      auto ciGuardado = colorIndex;
      colorIndex = 14; // plateado, y se devuelve como estaba
      dibujarPorcentajeLunar(16, 139, 14, pl);
      colorIndex = ciGuardado;
    }
    snprintf(b, sizeof(b), "LUNA %s", wxFase(pl, crece));
    wxPut(28, 36, 125, 124, 1, TFT_SKYBLUE, b);
    wxPut(29, 36, 136, 124, 2, TFT_WHITE, pct);
  }

  // Columna del minero
  float hash = mineria.currentHashRate.toFloat();
  uint16_t chash = hash > 300 ? TFT_GREEN : (hash > 0 ? TFT_YELLOW : TFT_RED);
  snprintf(b, sizeof(b), "%.1f", hash);
  wxPut(17, 166, 20, 102, hash >= 1000 ? 2 : 3, chash, b);
  wxPut(18, 318, 28, 48, 2, TFT_WHITE, "KH/s", TR_DATUM);

  uint16_t lo, hi;
  float avg;
  wxStats(lo, hi, avg);
  char mn[12];
  if (minkh >= 1000.0f)
    strcpy(mn, "--");
  else
    snprintf(mn, sizeof(mn), "%.1f", minkh);
  snprintf(b, sizeof(b), "MAX %.1f  MIN %s", maxkh, mn);
  wxPut(19, 166, 48, 152, 1, TFT_WHITE, b);
  if (wxCount)
    snprintf(b, sizeof(b), "MEDIA %.1f (%dm)", avg, wxCount * WX_HIST_SEG / 60);
  else
    snprintf(b, sizeof(b), "MEDIA --");
  wxPut(20, 166, 58, 152, 1, TFT_WHITE, b);
  snprintf(b, sizeof(b), "SHARES %.8s  VALIDOS %.5s", mineria.completedShares.c_str(), mineria.valids.c_str());
  wxPut(21, 166, 68, 152, 1, TFT_CYAN, b);
  snprintf(b, sizeof(b), "MEJOR DIF %.14s", mineria.bestDiff.c_str());
  wxPut(22, 166, 78, 152, 1, TFT_GOLD, b);
  snprintf(b, sizeof(b), "TOTAL %.10s KH", mineria.totalKHashes.c_str());
  wxPut(23, 166, 88, 152, 1, TFT_WHITE, b);
  snprintf(b, sizeof(b), "PLANT %.6s  CPU %.4sC", mineria.templates.c_str(), mineria.temp.c_str());
  wxPut(24, 166, 98, 152, 1, TFT_ORANGE, b);
  snprintf(b, sizeof(b), "TIEMPO %.16s", mineria.timeMining.c_str());
  wxPut(25, 166, 108, 152, 1, TFT_GREENYELLOW, b);

  // Grafica de los ultimos minutos
  if (wxCount)
    snprintf(b, sizeof(b), "ULT %dm ^%.0f v%.0f", (wxCount * WX_HIST_SEG + 59) / 60, hi / 10.0f, lo / 10.0f);
  else
    snprintf(b, sizeof(b), "RECOGIENDO HASHRATE...");
  wxPut(26, 166, 118, 152, 1, TFT_SKYBLUE, b);
  if (wxForce || wxSeq != wxSeqPint)
  {
    wxSeqPint = wxSeq;
    tft.fillRect(166, 128, 154, 26, TFT_BLACK);
    uint32_t rango = hi > lo ? (uint32_t)(hi - lo) : 1;
    for (uint8_t i = 0; i < wxCount; i++)
    {
      uint16_t v = wxHist[(wxHead + WX_HN - wxCount + i) % WX_HN];
      int h = 2 + (int)(((uint32_t)(v - lo) * 22) / rango);
      int x = 166 + (WX_HN - wxCount + i) * 2;
      uint16_t col = (i == wxCount - 1) ? TFT_WHITE : ((v / 10.0f >= avg) ? TFT_GREEN : TFT_YELLOW);
      tft.fillRect(x, 154 - h, 2, h, col);
    }
  }

  wxForce = false;
  manejandoLeds(hash); // LEDs exactamente igual que en la pantalla del minero
#ifdef DEBUG_MEMORY
  printheap();
#endif
}

/********************************************************************************************
 *  PANTALLA HANOI - Torres de Hanoi con 4 postes que se resuelven solas, en bucle, mientras
 *  el minero sigue
 *
 *  Con 4 postes ya no vale la regla de siempre: para cada tamano hay que calcular donde
 *  partir la torre (Frame-Stewart) y ese calculo se hace de verdad en cada partida. Poste de
 *  salida, de llegada y orden de los auxiliares salen al azar, asi que no se repite nunca el
 *  mismo baile. Empieza con 4 discos y sube de uno en uno hasta 32, luego vuelve a 4.
 *  Con 32 discos son 1281 movimientos (con 3 postes serian mas de 4000 millones).
 *
 *  Cada disco sube, se desliza y baja, y al aterrizar destella el LED con un color distinto.
 *  Arriba la hora y el hashrate, abajo los movimientos y los discos, y mas abajo se mantiene
 *  la barra de precios. Tactil igual que Pong: zona central inferior cambia el fondo,
 *  esquina superior derecha alterna pantalla y los laterales pasan a la siguiente o a la
 *  anterior.
 *
 *  Memoria: todo es estatico (unos 1,8 KB), no pide nada con malloc ni crea sprites.
 *
 *  Ajustes: HN_MIN_DISCOS y HN_MAX_DISCOS (no pasar de 32, la lista de movimientos esta
 *  hecha para eso), HN_PAUSA_MS (lo que se queda el RESUELTO) y HN_SLICE_MS (cada tanda).
 ********************************************************************************************/

#define HN_MIN_DISCOS 5
#define HN_MAX_DISCOS 32
#define HN_MAX_MOVS 1400
#define HN_SLICE_MS 5000
#define HN_PAUSA_MS 2500
#define HN_BASE_Y 138
#define HN_POSTE_Y 50
#define HN_VUELO_Y 34
#define HN_ALEATORIO (HN_MIN_DISCOS + esp_random() % (HN_MAX_DISCOS - HN_MIN_DISCOS + 1))

struct HanoiState
{
  bool init;
  int n;
  uint8_t peg[4][HN_MAX_DISCOS];
  uint8_t cnt[4];
  uint32_t mov, total;
  uint8_t fase; // 0 elegir, 1 subir, 2 deslizar, 3 bajar, 4 pausa final
  int from, to, disco;
  int x, y;
  int paso, alto;
  unsigned long hasta;
  uint8_t ancho[HN_MAX_DISCOS];
  uint16_t col[HN_MAX_DISCOS];
  uint8_t lista[HN_MAX_MOVS];
};
static HanoiState hn = {false};

static const int HN_PX[4] = {40, 120, 200, 280};
static uint16_t hnT[HN_MAX_DISCOS + 1];
static uint8_t hnK[HN_MAX_DISCOS + 1];
static int hnW = 0;

static inline int hnYNivel(int nivel)
{
  return HN_BASE_Y - hn.paso * (nivel + 1) + 1;
}

static uint16_t hnColor(int d, int n)
{
  int h = (d * 360 / n) % 360;
  int s = h / 60, f = (h % 60) * 255 / 60;
  uint8_t r = 0, g = 0, b = 0;
  switch (s)
  {
  case 0:
    r = 255;
    g = f;
    break;
  case 1:
    r = 255 - f;
    g = 255;
    break;
  case 2:
    g = 255;
    b = f;
    break;
  case 3:
    g = 255 - f;
    b = 255;
    break;
  case 4:
    r = f;
    b = 255;
    break;
  default:
    r = 255;
    b = 255 - f;
    break;
  }
  return tft.color565(r, g, b);
}

static void hnDisco(int cx, int y, int d)
{
  tft.fillRoundRect(cx - hn.ancho[d] / 2, y, hn.ancho[d], hn.alto, hn.alto / 3, hn.col[d]);
}

static void hnMov(int a, int b)
{
  if (hnW < HN_MAX_MOVS)
    hn.lista[hnW++] = (a << 2) | b;
}

static void hnH3(int k, int s, int d, int x)
{
  if (k == 0)
    return;
  hnH3(k - 1, s, x, d);
  hnMov(s, d);
  hnH3(k - 1, x, d, s);
}

static void hnH4(int n, int s, int d, int a, int b)
{
  if (n == 0)
    return;
  if (n == 1)
  {
    hnMov(s, d);
    return;
  }
  int k = hnK[n];
  hnH4(n - k, s, a, d, b);
  hnH3(k, s, d, b);
  hnH4(n - k, a, d, s, b);
}

static void hnNueva(int n)
{
  hn.n = n;
  hn.paso = min(10, 80 / n);
  hn.alto = hn.paso - 1;
  for (int d = 0; d < n; d++)
  {
    hn.ancho[d] = 10 + d * 62 / (n - 1);
    hn.col[d] = hnColor(d, n);
  }

  hnT[0] = 0;
  hnT[1] = 1;
  for (int m = 2; m <= n; m++)
  {
    uint32_t mejor = 0xFFFFFFFF;
    for (int k = 1; k < m; k++)
    {
      uint32_t c = 2 * hnT[m - k] + (1UL << k) - 1;
      if (c < mejor)
      {
        mejor = c;
        hnK[m] = k;
      }
    }
    hnT[m] = mejor;
  }

  int s = esp_random() % 4;
  int d = (s + 1 + esp_random() % 3) % 4;
  int ax[2], j = 0;
  for (int p = 0; p < 4; p++)
    if (p != s && p != d)
      ax[j++] = p;
  if (esp_random() & 1)
  {
    int t = ax[0];
    ax[0] = ax[1];
    ax[1] = t;
  }

  hnW = 0;
  hnH4(n, s, d, ax[0], ax[1]);
  hn.total = hnW;

  for (int p = 0; p < 4; p++)
    hn.cnt[p] = 0;
  for (int i = n - 1; i >= 0; i--)
    hn.peg[s][hn.cnt[s]++] = i;
  hn.mov = 0;
  hn.fase = 0;
}

static void hnCampo()
{
  tft.fillRect(0, 0, 320, 170, TFT_BLACK);
  tft.fillRect(4, HN_BASE_Y, 312, 4, TFT_WHITE);
  for (int p = 0; p < 4; p++)
  {
    tft.fillRect(HN_PX[p] - 2, HN_POSTE_Y, 4, HN_BASE_Y - HN_POSTE_Y, TFT_LIGHTGREY);
    for (int i = 0; i < hn.cnt[p]; i++)
      hnDisco(HN_PX[p], hnYNivel(i), hn.peg[p][i]);
  }
  if (hn.fase >= 1 && hn.fase <= 3)
    hnDisco(hn.x, hn.y, hn.disco);
}

// Borra el disco en vuelo y repone solo el trozo de poste que tapaba
static void hnMueve(int nx, int ny)
{
  int w = hn.ancho[hn.disco];
  int x0 = hn.x - w / 2, x1 = x0 + w;
  tft.fillRect(x0, hn.y, w, hn.alto, TFT_BLACK);
  int yA = max(HN_POSTE_Y, hn.y), yB = min(HN_BASE_Y, hn.y + hn.alto);
  for (int p = 0; p < 4; p++)
    if (HN_PX[p] + 2 > x0 && HN_PX[p] - 2 < x1 && yB > yA)
      tft.fillRect(HN_PX[p] - 2, yA, 4, yB - yA, TFT_LIGHTGREY);
  hn.x = nx;
  hn.y = ny;
  hnDisco(hn.x, hn.y, hn.disco);
}

static void hnTextos()
{
  char b[16];
  tft.setFreeFont(NULL);
  tft.setTextFont(1);
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextPadding(144);
  tft.setTextDatum(TL_DATUM);
  snprintf(b, sizeof(b), "MOV %03u/%03u", (unsigned)hn.mov, (unsigned)hn.total);
  tft.drawString(b, 4, 153);
  tft.setTextDatum(TR_DATUM);
  snprintf(b, sizeof(b), "DISCOS %d", hn.n);
  tft.drawString(b, 316, 153);
  tft.setTextPadding(0);
  tft.setTextDatum(TL_DATUM);
}

static void hnHud(const char *hash, const char *hora)
{
  char b[24];
  tft.setFreeFont(NULL);
  tft.setTextFont(1);
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextPadding(100);
  tft.setTextDatum(TL_DATUM);
  tft.drawString(hora, 4, 9);
  snprintf(b, sizeof(b), "%s KH/s", hash);
  tft.setTextPadding(150);
  tft.setTextDatum(TR_DATUM);
  tft.drawString(b, 316, 9);
  tft.setTextPadding(0);
  tft.setTextDatum(TL_DATUM);
}

// Un paso de animacion. Devuelve true si el disco acaba de aterrizar
static bool hnPaso(unsigned long now)
{
  int vy = 3 + hn.n, vx = 6 + 3 * hn.n;
  switch (hn.fase)
  {
  case 0:
  {
    hn.from = hn.lista[hn.mov] >> 2;
    hn.to = hn.lista[hn.mov] & 3;
    hn.disco = hn.peg[hn.from][--hn.cnt[hn.from]];
    hn.x = HN_PX[hn.from];
    hn.y = hnYNivel(hn.cnt[hn.from]);
    hn.fase = 1;
    break;
  }
  case 1:
  {
    int ny = hn.y - vy;
    if (ny <= HN_VUELO_Y)
    {
      ny = HN_VUELO_Y;
      hn.fase = 2;
    }
    hnMueve(hn.x, ny);
    break;
  }
  case 2:
  {
    int dest = HN_PX[hn.to];
    int nx = (dest > hn.x) ? min(hn.x + vx, dest) : max(hn.x - vx, dest);
    if (nx == dest)
      hn.fase = 3;
    hnMueve(nx, hn.y);
    break;
  }
  case 3:
  {
    int dest = hnYNivel(hn.cnt[hn.to]);
    int ny = hn.y + vy;
    bool aterriza = (ny >= dest);
    if (aterriza)
      ny = dest;
    hnMueve(hn.x, ny);
    if (aterriza)
    {
      hn.peg[hn.to][hn.cnt[hn.to]++] = hn.disco;
      hn.mov++;
      if (hn.mov >= hn.total)
      {
        hn.fase = 4;
        hn.hasta = now + HN_PAUSA_MS;
      }
      else
        hn.fase = 0;
      return true;
    }
    break;
  }
  }
  return false;
}

void esp32_2432S028R_HanoiScreen(unsigned long mElapsed)
{
  mineria = getMiningData(mElapsed);
  Serial.printf("M8AX - %s >>> Completados %s Share(s), %s Khashes, Prom. Hashrate %s KH/s %s°\n",
                mineria.currentTime.c_str(), mineria.completedShares.c_str(), mineria.totalKHashes.c_str(), mineria.currentHashRate.c_str(), mineria.temp.c_str());

  bool redraw = hasChangedScreen;
  if (!hn.init)
  {
    hn.init = true;
    hnNueva(HN_ALEATORIO);
    redraw = true;
  }

  if (redraw)
    hnCampo();
  printPoolData();
  hasChangedScreen = false;

  char horaStr[9] = "";
  char lastHora[9] = "";
  char hashStr[16];
  char lastHash[16] = "";
  uint32_t lastMov = redraw ? 0xFFFFFFFF : hn.mov;
  int lastN = redraw ? -1 : hn.n;

  unsigned long start = millis();
  unsigned long lastMiningUpdate = 0;
  unsigned long lastTouchChk = 0;

  while (millis() - start < HN_SLICE_MS)
  {
    unsigned long now = millis();

    if (now - lastTouchChk >= 40 && now - previousTouchMillis >= 500)
    {
      lastTouchChk = now;
      int16_t t_x, t_y;
      if (toqueXY(t_x, t_y))
      {
        previousTouchMillis = now;
        if (((t_x > 109) && (t_x < 211)) && ((t_y > 185) && (t_y < 241)))
        {
          bottomScreenBlue ^= true;
          hasChangedScreen = true;
        }
        else if ((t_x > 235) && ((t_y > 0) && (t_y < 16)))
        {
          esp32_2432S028R_AlternateScreenState();
        }
        else if (t_x > 160)
        {
          currentDisplayDriver->current_cyclic_screen = (currentDisplayDriver->current_cyclic_screen + 1) % currentDisplayDriver->num_cyclic_screens;
        }
        else
        {
          currentDisplayDriver->current_cyclic_screen = currentDisplayDriver->current_cyclic_screen - 1;
          if (currentDisplayDriver->current_cyclic_screen < 0)
            currentDisplayDriver->current_cyclic_screen = currentDisplayDriver->num_cyclic_screens - 1;
        }
        break;
      }
    }

    if (hn.fase == 4)
    {
      if (now >= hn.hasta)
      {
        hnNueva(HN_ALEATORIO);
        hnCampo();
        lastMov = 0xFFFFFFFF;
      }
    }
    else if (hnPaso(now))
    {
      int c = hn.disco % 7 + 1;
      ledFlash(c & 1, c & 2, c & 4, LED_FLASH_MS);
      if (hn.fase == 4)
      {
        tft.setTextFont(1);
        tft.setTextSize(2);
        tft.setTextColor(TFT_GREEN, TFT_BLACK);
        tft.setTextDatum(TC_DATUM);
        tft.drawString("RESUELTO!", 160, 28);
        tft.setTextDatum(TL_DATUM);
      }
    }

    if (hn.mov != lastMov || hn.n != lastN)
    {
      hnTextos();
      lastMov = hn.mov;
      lastN = hn.n;
    }

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
        hnHud(hashStr, horaStr);
        strcpy(lastHora, horaStr);
        strcpy(lastHash, hashStr);
      }
    }
    webAtender();
    delay(10);
  }

  tft.setTextPadding(0);
  tft.setTextDatum(TL_DATUM);

  cuentita++;
  if (cuentita == 15)
  {
    ajustarZonaHoraria();
    obtenerLocYTemp();
    obtenerVariacionesBTC(variacion1dia, variacion1semana);
  }
}

/****************************************************************************************************************
 *
 *   Escrito por: M8AX
 *
 *   Descripción: NEMESIS ( GRADIUS ) - Pantalla para ESP32_2432S028R
 *   ----------------------------------------------------------------
 *
 *   Un matamarcianos de scroll horizontal como el Nemesis del MSX, jugado por la CPU. La nave ( Vic Viper )
 *   esquiva balas y paredes, recoge las capsulas naranjas y las gasta en la barra de abajo: SPEED, MISSL, DOUBL,
 *   LASER, OPTION y ?. Cada vez que coge una capsula se ilumina la siguiente casilla, y la CPU decide si la
 *   activa o espera a la siguiente, como haria un jugador. El ultimo bicho de cada oleada es naranja y suelta
 *   la capsula, asi que la CPU le da prioridad.
 *
 *   Enemigos: FANs en serpiente, GARUNs en onda, RUGALs que te persiguen y CANONES pegados al terreno ( suelo
 *   y techo ). Montañas y estalactitas con scroll y estrellas en tres capas. Al final de cada fase sale el
 *   BIG CORE: solo se le hace daño cuando abre el nucleo ( parpadea rojo y amarillo ). Cada fase cambia los
 *   colores del terreno y es un poco mas dificil. Si te matan pierdes todos los power-ups, igual que en el
 *   original. Tiene 3 vidas; al perder la ultima sale GAME OVER y empieza otra partida.
 *
 *   Arriba a la izquierda la hora y arriba a la derecha el hashrate, como en las demas. Abajo van los puntos,
 *   la fase, las vidas, el record y la barra de power-ups.
 *
 *   LED: capsula = azul, enemigo destruido = verde, y si matan a la nave se encienden los tres a la vez.
 *
 *   Memoria: al arrancar el minero no ocupa NADA. Unos 22 KB ( casi todo el buffer de pantalla de 4 bits ) se
 *   piden con malloc al entrar en la pantalla y se liberan al salir a otra, como el Asteroides. Si no hubiera
 *   RAM, se salta la pantalla sin pintar nada.
 *
 *   Para activarla: pegar este bloque justo antes de analiCadaSegundo, meter esp32_2432S028R_NemesisScreen en el
 *   array esp32_2432S028RCyclicScreens[] y añadir en analiCadaSegundo, debajo del if de galM, este otro:
 *   if (nemM && pantallaActual != esp32_2432S028R_NemesisScreen) { free(nemM); nemM = NULL; }
 *
 ****************************************************************************************************************/

// >>> NEMESIS MOTOR
#define NEM_W 320
#define NEM_H 112
#define NEM_Y0 28 // el HUD de arriba ocupa hasta la 26
#define NEM_FRAME_MS 30
#define NEM_SLICE_MS 5000
#define NEM_FIN_MS 3500
#define NEM_VIDAS 10
#define NEM_MAXE 18
#define NEM_MAXEB 16
#define NEM_MAXPB 28
#define NEM_MAXX 14
#define NEM_MAXC 3
#define NEM_NST 40
#define NEM_HIST 40
#define NEM_ESC 2        // pixeles que avanza el scroll por fotograma
#define NEM_FASE_PX 2600 // distancia hasta el jefe
#define NEM_BW 46
#define NEM_BH 36
#define NEM_BOSS_HP 50
#define NEM_RGB(r, g, b) ((uint16_t)((((r) & 0xF8) << 8) | (((g) & 0xFC) << 3) | ((b) >> 3)))

#define NEM_T_FAN 1
#define NEM_T_GARUN 2
#define NEM_T_RUGAL 3
#define NEM_T_DUCKER 4

// 0 negro | 1-2 estrellas | 3 blanco | 4 gris nave | 5 cian | 6 azul | 7 rojo | 8 naranja | 9 amarillo | 10 verde
// 11-13 terreno ( cambia por fase ) | 14 magenta | 15 rojo oscuro
static const uint16_t NEM_PAL0[16] = {
    NEM_RGB(0, 0, 0), NEM_RGB(30, 34, 80), NEM_RGB(110, 120, 190), NEM_RGB(255, 255, 255),
    NEM_RGB(185, 195, 225), NEM_RGB(60, 230, 255), NEM_RGB(40, 100, 255), NEM_RGB(255, 50, 40),
    NEM_RGB(255, 150, 30), NEM_RGB(255, 240, 80), NEM_RGB(60, 255, 100), NEM_RGB(90, 50, 30),
    NEM_RGB(150, 90, 40), NEM_RGB(215, 150, 70), NEM_RGB(230, 60, 220), NEM_RGB(130, 20, 20)};

static const uint16_t NEM_TER[4][3] = {
    {NEM_RGB(90, 50, 30), NEM_RGB(150, 90, 40), NEM_RGB(215, 150, 70)},
    {NEM_RGB(20, 60, 90), NEM_RGB(40, 110, 150), NEM_RGB(110, 200, 230)},
    {NEM_RGB(60, 20, 70), NEM_RGB(120, 40, 130), NEM_RGB(210, 110, 220)},
    {NEM_RGB(20, 70, 30), NEM_RGB(50, 130, 60), NEM_RGB(140, 230, 110)}};

// Sprites: cada caracter es un color de la paleta ( 0-9, a-f ), el punto es transparente
static const char *const NEM_SP_VIC[7] = {
    "..77............",
    "..4777..........",
    "...444444444....",
    "8644444455533339",
    "...444444444....",
    "..4777..........",
    "..77............"};
static const char *const NEM_SP_OPC[5] = {
    ".999.",
    "98889",
    "98389",
    "98889",
    ".999."};
static const char *const NEM_SP_CAP[7] = {
    ".88888.",
    "8899988",
    "8999998",
    "8999998",
    "8999998",
    "8899988",
    ".88888."};
static const char *const NEM_SP_FAN[7] = {
    "..66666..",
    ".6655566.",
    "665555566",
    "655333556",
    "665555566",
    ".6655566.",
    "..66666.."};
static const char *const NEM_SP_FAN2[7] = {
    "..66666..",
    ".6633366.",
    "663555366",
    "655555556",
    "663555366",
    ".6633366.",
    "..66666.."};
static const char *const NEM_SP_GAR[5] = {
    "7.......7",
    ".77...77.",
    "..77777..",
    "...797...",
    "....7...."};
static const char *const NEM_SP_RUG[5] = {
    "....e....",
    "..eeeee..",
    "eeeee9eee",
    "..eeeee..",
    "....e...."};
static const char *const NEM_SP_DUC[5] = {
    "...4...",
    "..444..",
    ".44744.",
    "4444444",
    "3.....3"};

struct NemEn
{
  int16_t x, y, yb;
  uint16_t t, ret;
  uint8_t tipo, hp, cap, flip;
};

struct NemBala
{
  float x, y, vx, vy;
  bool on;
};

// tipo: 0 normal, 1 diagonal ( DOUBLE ), 2 laser, 3 misil cayendo, 4 misil pegado al suelo
struct NemPb
{
  int16_t x, y;
  int8_t vx, vy;
  uint8_t tipo;
  bool on;
};

struct NemExp
{
  int16_t x, y;
  uint8_t t, tmax, r, on;
};

struct NemEst
{
  int16_t x;
  uint8_t y, capa;
};

struct NemCap
{
  float x, y;
  bool on;
};

struct NemJefe
{
  bool on;
  int16_t x, y, hp, fin;
  uint16_t t;
  uint8_t abierto;
};

struct NemMem
{
  uint8_t fb[NEM_W * NEM_H / 2];
  uint16_t linea[NEM_W];
  uint16_t pal[16];
  uint8_t techo[NEM_W], suelo[NEM_W];
  NemEn en[NEM_MAXE];
  NemBala eb[NEM_MAXEB];
  NemPb pb[NEM_MAXPB];
  NemExp ex[NEM_MAXX];
  NemEst es[NEM_NST];
  NemCap ca[NEM_MAXC];
  NemJefe jf;
  int16_t hx[NEM_HIST], hy[NEM_HIST];
  float sx, sy;
  int32_t dist, proxOla, mundo;
  int16_t tRest, tgT, tgS, curT, curS;
  uint16_t inmune, muerteT, cad, cadMis, retardoAct;
  uint8_t hi, estado, vidas, fase, vel, miss, doble, laser, opciones, escudo, meterPos, ola;
  bool sucio, sucioMeter;
  uint32_t puntos, fr, ledOff;
  uint8_t ledPrio;
  unsigned long finAt;
  char tTxt[64];
};
static NemMem *nemM = NULL;
static uint32_t nemRecord = 0;
static uint32_t nemPartidas = 0;

// la dificultad deja de subir a partir de la fase 10
static inline int nemDif() { return (nemM->fase > 10) ? 10 : nemM->fase; }

static inline int nemRnd(int n) { return (int)(esp_random() % (uint32_t)n); }

static inline void nemPx(int x, int y, uint8_t c)
{
  if ((unsigned)x >= (unsigned)NEM_W || (unsigned)y >= (unsigned)NEM_H)
    return;
  uint8_t *p = &nemM->fb[y * (NEM_W / 2) + (x >> 1)];
  if (x & 1)
    *p = (uint8_t)((*p & 0x0F) | (c << 4));
  else
    *p = (uint8_t)((*p & 0xF0) | c);
}

static void nemRect(int x, int y, int w, int h, uint8_t c)
{
  for (int j = 0; j < h; j++)
    for (int i = 0; i < w; i++)
      nemPx(x + i, y + j, c);
}

static void nemDisco(int cx, int cy, int r, uint8_t c)
{
  for (int dy = -r; dy <= r; dy++)
  {
    int dx = (int)sqrtf((float)(r * r - dy * dy) + 0.5f);
    for (int i = -dx; i <= dx; i++)
      nemPx(cx + i, cy + dy, c);
  }
}

static void nemSpr(const char *const *sp, int w, int h, int x, int y, bool flipY)
{
  for (int r = 0; r < h; r++)
  {
    const char *fila = sp[flipY ? h - 1 - r : r];
    for (int i = 0; i < w; i++)
    {
      char ch = fila[i];
      if (ch == '.')
        continue;
      nemPx(x + i, y + r, (uint8_t)((ch <= '9') ? (ch - '0') : (ch - 'a' + 10)));
    }
  }
}

static inline bool nemSolapa(int ax, int ay, int aw, int ah, int bx, int by, int bw, int bh)
{
  return ax < bx + bw && ax + aw > bx && ay < by + bh && ay + ah > by;
}

static void nemLed(bool r, bool g, bool b, uint16_t ms, uint8_t prio)
{
  unsigned long now = millis();
  if (nemM->ledOff && now < nemM->ledOff && prio < nemM->ledPrio)
    return;
  digitalWrite(LED_PIN, r ? LOW : HIGH);
  digitalWrite(LED_PIN_G, g ? LOW : HIGH);
  digitalWrite(LED_PIN_B, b ? LOW : HIGH);
  nemM->ledOff = now + ms;
  nemM->ledPrio = prio;
}

static void nemPaleta()
{
  NemMem &m = *nemM;
  memcpy(m.pal, NEM_PAL0, sizeof(m.pal));
  int t = (m.fase > 0 ? m.fase - 1 : 0) & 3;
  m.pal[11] = NEM_TER[t][0];
  m.pal[12] = NEM_TER[t][1];
  m.pal[13] = NEM_TER[t][2];
}

// ---------------------------------------------------------------- terreno
static void nemColumna(int x)
{
  NemMem &m = *nemM;
  if (m.tRest <= 0)
  {
    m.tRest = 30 + nemRnd(60);
    int modo = (m.jf.on || m.dist > NEM_FASE_PX - 220) ? 0 : nemRnd(5);
    int ext = 16 + nemDif() * 2;
    if (ext > 28)
      ext = 28;
    switch (modo)
    {
    case 1: // montañas
      m.tgS = 16 + nemRnd(ext);
      m.tgT = 2 + nemRnd(6);
      break;
    case 2: // estalactitas
      m.tgT = 16 + nemRnd(ext);
      m.tgS = 2 + nemRnd(6);
      break;
    case 3: // tunel
      m.tgT = 18 + nemRnd(12);
      m.tgS = 18 + nemRnd(12);
      break;
    default: // abierto
      m.tgT = 2 + nemRnd(8);
      m.tgS = 2 + nemRnd(8);
      break;
    }
  }
  m.tRest--;
  if (m.curT < m.tgT)
    m.curT++;
  else if (m.curT > m.tgT)
    m.curT--;
  if (m.curS < m.tgS)
    m.curS++;
  else if (m.curS > m.tgS)
    m.curS--;
  m.techo[x] = (uint8_t)m.curT;
  m.suelo[x] = (uint8_t)m.curS;
}

static void nemScroll()
{
  NemMem &m = *nemM;
  memmove(m.techo, m.techo + NEM_ESC, NEM_W - NEM_ESC);
  memmove(m.suelo, m.suelo + NEM_ESC, NEM_W - NEM_ESC);
  for (int i = 0; i < NEM_ESC; i++)
    nemColumna(NEM_W - NEM_ESC + i);
  m.dist += NEM_ESC;
  m.mundo += NEM_ESC;
}

// ---------------------------------------------------------------- explosiones y disparos
static void nemExplota(int x, int y, int r)
{
  NemMem &m = *nemM;
  for (int i = 0; i < NEM_MAXX; i++)
    if (!m.ex[i].on)
    {
      m.ex[i].x = (int16_t)x;
      m.ex[i].y = (int16_t)y;
      m.ex[i].t = 0;
      m.ex[i].tmax = (uint8_t)(6 + r / 2);
      m.ex[i].r = (uint8_t)r;
      m.ex[i].on = 1;
      return;
    }
}

static void nemPb(int x, int y, int vx, int vy, uint8_t tipo)
{
  NemMem &m = *nemM;
  for (int i = 0; i < NEM_MAXPB; i++)
    if (!m.pb[i].on)
    {
      m.pb[i].x = (int16_t)x;
      m.pb[i].y = (int16_t)y;
      m.pb[i].vx = (int8_t)vx;
      m.pb[i].vy = (int8_t)vy;
      m.pb[i].tipo = tipo;
      m.pb[i].on = true;
      return;
    }
}

// disparo desde la nave o desde una opcion
static void nemDispara(int x, int y)
{
  NemMem &m = *nemM;
  if (m.laser)
    nemPb(x + 15, y + 3, 9, 0, 2);
  else
    nemPb(x + 15, y + 3, 6, 0, 0);
  if (m.doble)
    nemPb(x + 9, y + 1, 5, -3, 1);
}

static void nemDisparoEn(float x, float y)
{
  NemMem &m = *nemM;
  if (m.estado != 0)
    return;
  float dx = (m.sx + 8.0f) - x, dy = (m.sy + 3.0f) - y;
  float len = sqrtf(dx * dx + dy * dy);
  if (len < 1.0f || dx > -20.0f)
    return;
  float v = 1.5f + nemDif() * 0.2f;
  if (v > 3.0f)
    v = 3.0f;
  for (int i = 0; i < NEM_MAXEB; i++)
    if (!m.eb[i].on)
    {
      m.eb[i].x = x;
      m.eb[i].y = y;
      m.eb[i].vx = dx / len * v;
      m.eb[i].vy = dy / len * v;
      m.eb[i].on = true;
      return;
    }
}

static void nemCapsula(float x, float y)
{
  NemMem &m = *nemM;
  for (int i = 0; i < NEM_MAXC; i++)
    if (!m.ca[i].on)
    {
      m.ca[i].x = x;
      m.ca[i].y = y;
      m.ca[i].on = true;
      return;
    }
}

// ---------------------------------------------------------------- enemigos
static NemEn *nemNuevoEn(uint8_t tipo, int x, int y, int ret)
{
  NemMem &m = *nemM;
  for (int i = 0; i < NEM_MAXE; i++)
    if (m.en[i].hp == 0)
    {
      NemEn &e = m.en[i];
      e.x = (int16_t)x;
      e.y = (int16_t)y;
      e.yb = (int16_t)y;
      e.t = 0;
      e.ret = (uint16_t)ret;
      e.tipo = tipo;
      e.hp = (tipo == NEM_T_DUCKER) ? 2 : 1;
      e.cap = 0;
      e.flip = 0;
      return &e;
    }
  return NULL;
}

// el cañon se pone sobre el terreno que hay justo al entrar en pantalla
static void nemColocaDucker(NemEn &e)
{
  NemMem &m = *nemM;
  int x0 = NEM_W - 8;
  int maxS = 0, maxT = 0;
  for (int i = 0; i < 7; i++)
  {
    if (m.suelo[x0 + i] > maxS)
      maxS = m.suelo[x0 + i];
    if (m.techo[x0 + i] > maxT)
      maxT = m.techo[x0 + i];
  }
  e.x = (int16_t)x0;
  if (maxS >= maxT)
  {
    e.flip = 0;
    e.y = (int16_t)(NEM_H - maxS - 5);
  }
  else
  {
    e.flip = 1;
    e.y = (int16_t)maxT;
  }
}

static void nemOla()
{
  NemMem &m = *nemM;
  int tipo = m.ola % 6;
  m.ola++;
  switch (tipo)
  {
  case 0:
  case 3:
  {
    int yb = 34 + nemRnd(NEM_H - 70);
    for (int i = 0; i < 6; i++)
    {
      NemEn *e = nemNuevoEn(NEM_T_FAN, NEM_W + 6, yb, i * 9);
      if (e && i == 5)
        e->cap = 1;
    }
    break;
  }
  case 1:
  {
    int yb = 36 + nemRnd(NEM_H - 72);
    for (int i = 0; i < 5; i++)
    {
      NemEn *e = nemNuevoEn(NEM_T_GARUN, NEM_W + 6, yb, i * 7);
      if (e && i == 4)
        e->cap = 1;
    }
    break;
  }
  case 2:
    for (int i = 0; i < 3; i++)
      nemNuevoEn(NEM_T_RUGAL, NEM_W + 6, 12 + nemRnd(NEM_H - 30), i * 16);
    break;
  case 4:
  {
    NemEn *a = nemNuevoEn(NEM_T_DUCKER, NEM_W - 8, 0, 1);
    NemEn *b = nemNuevoEn(NEM_T_DUCKER, NEM_W - 8, 0, 40);
    if (b)
      b->cap = 1;
    (void)a;
    break;
  }
  default:
  {
    int yb = 34 + nemRnd(NEM_H - 70);
    for (int i = 0; i < 4; i++)
    {
      NemEn *e = nemNuevoEn(NEM_T_FAN, NEM_W + 6, yb, i * 9);
      if (e && i == 3)
        e->cap = 1;
    }
    nemNuevoEn(NEM_T_RUGAL, NEM_W + 6, 12 + nemRnd(NEM_H - 30), 20);
    nemNuevoEn(NEM_T_RUGAL, NEM_W + 6, 12 + nemRnd(NEM_H - 30), 40);
    break;
  }
  }
}

static void nemMataEn(NemEn &e)
{
  NemMem &m = *nemM;
  static const uint16_t pts[5] = {0, 100, 150, 200, 200};
  e.hp = 0;
  m.puntos += pts[e.tipo] * m.fase;
  m.sucio = true;
  nemExplota(e.x + 4, e.y + 3, 7);
  nemLed(false, true, false, 70, 1);
  if (e.cap)
    nemCapsula(e.x, e.y);
}

static void nemEnemigos()
{
  NemMem &m = *nemM;
  for (int i = 0; i < NEM_MAXE; i++)
  {
    NemEn &e = m.en[i];
    if (!e.hp)
      continue;
    if (e.ret)
    {
      e.ret--;
      if (e.ret == 0 && e.tipo == NEM_T_DUCKER)
        nemColocaDucker(e);
      continue;
    }
    e.t++;
    switch (e.tipo)
    {
    case NEM_T_FAN:
      e.x -= 2;
      e.y = (int16_t)(e.yb + (int)(sinf(e.t * 0.11f) * 22.0f));
      break;
    case NEM_T_GARUN:
      e.x -= 3;
      e.y = (int16_t)(e.yb + (int)(sinf(e.t * 0.18f) * 26.0f));
      if (nemRnd(100 - nemDif() * 6) == 0)
        nemDisparoEn(e.x + 4, e.y + 2);
      break;
    case NEM_T_RUGAL:
      e.x -= 4;
      if (m.estado == 0 && e.x > m.sx + 40)
      {
        int dy = (int)(m.sy + 3.0f) - (e.y + 2);
        e.y += (dy > 0) - (dy < 0);
      }
      break;
    default: // cañon
      e.x -= NEM_ESC;
      if (e.x > m.sx + 40 && e.x < NEM_W - 10 && nemRnd(80 - nemDif() * 6) == 0)
        nemDisparoEn(e.x + 3, e.y + (e.flip ? 4 : 0));
      break;
    }
    if (e.x < -14)
      e.hp = 0;
  }
}

// ---------------------------------------------------------------- jefe
static void nemJefeEntra()
{
  NemMem &m = *nemM;
  m.jf.on = true;
  m.jf.x = NEM_W + 4;
  m.jf.y = (NEM_H - NEM_BH) / 2;
  m.jf.hp = (int16_t)(NEM_BOSS_HP + nemDif() * 10);
  m.jf.t = 0;
  m.jf.abierto = 0;
  m.jf.fin = 0;
}

static void nemJefeMuere()
{
  NemMem &m = *nemM;
  m.jf.fin = 80;
  m.puntos += 3000UL * m.fase;
  m.sucio = true;
  for (int i = 0; i < NEM_MAXEB; i++)
    m.eb[i].on = false;
  nemLed(true, true, true, 500, 2);
}

static void nemJefe()
{
  NemMem &m = *nemM;
  NemJefe &j = m.jf;
  if (!j.on)
    return;
  if (j.fin > 0)
  {
    j.fin--;
    if ((j.fin & 3) == 0)
      nemExplota(j.x + nemRnd(NEM_BW), j.y + nemRnd(NEM_BH), 8 + nemRnd(8));
    if (j.fin == 0)
    {
      j.on = false;
      m.fase++;
      if (m.fase > 99)
        m.fase = 99;
      m.dist = 0;
      m.proxOla = 160;
      nemPaleta();
      nemCapsula(j.x + 10, j.y + 4);
      nemCapsula(j.x + 10, j.y + 24);
      m.sucio = true;
    }
    return;
  }
  j.t++;
  if (j.x > 232)
  {
    j.x -= 2;
    return;
  }
  j.y = (int16_t)((NEM_H - NEM_BH) / 2 + (int)(sinf(j.t * 0.045f) * 30.0f));
  j.abierto = (j.t % 100) < 55;
  if (j.t % 45 == 0 && m.estado == 0)
  {
    float ox = (float)j.x, oy = (float)(j.y + NEM_BH / 2);
    float ang = atan2f((m.sy + 3.0f) - oy, (m.sx + 8.0f) - ox);
    float v = 1.7f + nemDif() * 0.2f;
    if (v > 3.0f)
      v = 3.0f;
    int n = (m.fase >= 3) ? 5 : 3;
    for (int k = 0; k < n; k++)
    {
      float a = ang + (k - n / 2) * 0.3f;
      for (int i = 0; i < NEM_MAXEB; i++)
        if (!m.eb[i].on)
        {
          m.eb[i].x = ox;
          m.eb[i].y = oy;
          m.eb[i].vx = cosf(a) * v;
          m.eb[i].vy = sinf(a) * v;
          m.eb[i].on = true;
          break;
        }
    }
  }
}

// ---------------------------------------------------------------- nave
static void nemNaveMuere()
{
  NemMem &m = *nemM;
  nemExplota((int)m.sx + 8, (int)m.sy + 3, 14);
  nemExplota((int)m.sx + 2, (int)m.sy + 2, 9);
  nemExplota((int)m.sx + 14, (int)m.sy + 4, 9);
  m.estado = 1;
  m.muerteT = 0;
  if (m.vidas)
    m.vidas--;
  m.vel = m.miss = m.doble = m.laser = m.opciones = m.escudo = m.meterPos = 0;
  m.sucio = true;
  m.sucioMeter = true;
  nemLed(true, true, true, 450, 2);
}

// Que casilla le interesa a la CPU ahora mismo, en este orden: SPEED, LASER, OPTION, MISSL, OPTION, DOUBL, SPEED, OPTION, ?
static int nemObjetivoPower()
{
  NemMem &m = *nemM;
  if (m.vel < 1)
    return 0;
  if (!m.laser)
    return 3;
  if (m.opciones < 1)
    return 4;
  if (!m.miss)
    return 1;
  if (m.opciones < 2)
    return 4;
  if (!m.doble)
    return 2;
  if (m.vel < 3)
    return 0;
  if (m.opciones < 3)
    return 4;
  if (!m.escudo)
    return 5;
  return -1;
}

static void nemActivaPower()
{
  NemMem &m = *nemM;
  if (m.meterPos == 0)
    return;
  if (m.retardoAct)
  {
    m.retardoAct--;
    return;
  }
  int s = m.meterPos - 1;
  if (s != nemObjetivoPower())
    return;
  switch (s)
  {
  case 0:
    m.vel++;
    break;
  case 1:
    m.miss = 1;
    break;
  case 2:
    m.doble = 1;
    break;
  case 3:
    m.laser = 1;
    break;
  case 4:
    m.opciones++;
    break;
  default:
    m.escudo = 6;
    break;
  }
  m.meterPos = 0;
  m.sucioMeter = true;
}

// Un peligro previsto: dentro de t fotogramas habra algo a la altura y, a menos de thr pixeles de la nave
struct NemPeligro
{
  float t, y, thr;
};

// Donde estara un enemigo dentro de dt fotogramas
static float nemEnY(const NemEn &e, float dt)
{
  switch (e.tipo)
  {
  case NEM_T_FAN:
    return e.yb + sinf((e.t + dt) * 0.11f) * 22.0f + 3.0f;
  case NEM_T_GARUN:
    return e.yb + sinf((e.t + dt) * 0.18f) * 26.0f + 2.0f;
  case NEM_T_RUGAL:
  {
    float d = (nemM->sy + 3.0f) - (e.y + 2.0f);
    if (d > dt)
      d = dt;
    if (d < -dt)
      d = -dt;
    return e.y + 2.0f + d;
  }
  default:
    return e.y + 2.5f;
  }
}

// Lista de peligros vistos desde una nave puesta en la columna sxc: cada bala o enemigo se mira cuando entra,
// a mitad de camino y cuando sale de la nave
static int nemPeligros(float sxc, NemPeligro *pel)
{
  NemMem &m = *nemM;
  int np = 0;
  for (int i = 0; i < NEM_MAXEB; i++)
  {
    NemBala &b = m.eb[i];
    if (!b.on || b.vx > -0.1f)
      continue;
    float tIn = (b.x - (sxc + 13.0f)) / -b.vx;
    float tOut = (b.x - (sxc + 3.0f)) / -b.vx;
    if (tOut < 0.0f || tIn > 45.0f)
      continue;
    if (tIn < 0.0f)
      tIn = 0.0f;
    float tm = (tIn + tOut) * 0.5f;
    pel[np++] = {tIn, b.y + b.vy * tIn, 8.0f};
    pel[np++] = {tm, b.y + b.vy * tm, 8.0f};
    pel[np++] = {tOut, b.y + b.vy * tOut, 8.0f};
  }
  for (int i = 0; i < NEM_MAXE; i++)
  {
    NemEn &e = m.en[i];
    if (!e.hp || e.ret)
      continue;
    float sp = (e.tipo == NEM_T_GARUN) ? 3.0f : (e.tipo == NEM_T_RUGAL ? 4.0f : 2.0f);
    float tIn = (e.x - (sxc + 13.0f)) / sp;
    float tOut = (e.x + 9.0f - (sxc + 3.0f)) / sp;
    if (tOut < 0.0f || tIn > 45.0f)
      continue;
    if (tIn < 0.0f)
      tIn = 0.0f;
    float tm = (tIn + tOut) * 0.5f;
    float thr = (e.tipo == NEM_T_FAN) ? 9.5f : 8.5f;
    pel[np++] = {tIn, nemEnY(e, tIn), thr};
    pel[np++] = {tm, nemEnY(e, tm), thr};
    pel[np++] = {tOut, nemEnY(e, tOut), thr};
  }
  return np;
}

// Aqui decide la CPU: elige la altura mas segura y que mejor apunta, y se mueve hacia ella.
// Los peligros se miran en el momento en que llegarian a la nave, con la nave ya movida hacia esa altura.
static void nemIA()
{
  NemMem &m = *nemM;
  int vel = 2 + m.vel;
  int sxi = (int)m.sx, syi = (int)m.sy;
  int tx = 18, ty = NEM_H / 2;

  int cap = -1;
  float cd = 1e9f;
  for (int c = 0; c < NEM_MAXC; c++)
    if (m.ca[c].on && m.ca[c].x > sxi - 4 && m.ca[c].x < 250 && m.ca[c].x < cd)
    {
      cd = m.ca[c].x;
      cap = c;
    }
  if (cap >= 0)
  {
    tx = (int)m.ca[cap].x - 6;
    if (tx > 170)
      tx = 170;
    ty = (int)m.ca[cap].y + 3;
  }
  else if (m.jf.on && m.jf.fin == 0)
  {
    // el jefe sube y baja: se apunta a donde estara cuando llegue la bala
    float vuelo = (m.jf.x - m.sx) / 6.0f;
    ty = (NEM_H - NEM_BH) / 2 + (int)(sinf((m.jf.t + vuelo) * 0.045f) * 30.0f) + NEM_BH / 2;
    tx = 100;
  }
  else
  {
    int best = -1, bd = 9999;
    for (int i = 0; i < NEM_MAXE; i++)
    {
      NemEn &e = m.en[i];
      if (e.hp && !e.ret && e.x > sxi + 14 && e.x < NEM_W)
      {
        int d = e.x - sxi - (e.cap ? 70 : 0);
        if (d < bd)
        {
          bd = d;
          best = i;
        }
      }
    }
    if (best >= 0)
      ty = (int)nemEnY(m.en[best], (m.en[best].x - sxi) / 8.0f);
  }

  NemPeligro pel[NEM_MAXEB * 3 + NEM_MAXE * 3];
  int mejorY = syi, mejorX = sxi;
  long mejorC = 2000000000L;
  int tope = (m.jf.on ? m.jf.x - 40 : 190);
  if (tope > 190)
    tope = 190;
  // tres columnas posibles: la actual, un paso atras y un paso adelante ( retroceder da tiempo para esquivar )
  for (int k = 0; k < 2; k++)
  {
    int cx = sxi + (k == 1 ? -14 : 0);
    if (cx < 4 || cx > tope)
      continue;
    int np = nemPeligros((float)cx, pel);
    for (int cy = 2; cy <= NEM_H - 9; cy += 3)
    {
      long c = 0;
      // terreno: lo de justo debajo y lo que viene
      for (int col = cx; col < cx + 46 && col < NEM_W; col += 3)
        if (cy < m.techo[col] + 3 || cy + 7 > NEM_H - m.suelo[col] - 3)
          c += 6000 - (col - cx) * 40;
      int dyc = cy - syi;
      for (int j = 0; j < np; j++)
      {
        int mov = vel * (int)pel[j].t;
        int ye = syi + (dyc > mov ? mov : (dyc < -mov ? -mov : dyc));
        float d = fabsf(pel[j].y - (ye + 3.5f));
        if (d < pel[j].thr)
          c += (long)((pel[j].thr - d) * 90.0f) + (long)((45.0f - pel[j].t) * 8.0f);
      }
      c += (long)abs(cy + 3 - ty) * 4;
      c += abs(cy - syi);
      c += (long)abs(cx - tx) * 2 + (k ? 6 : 0);
      if (c < mejorC)
      {
        mejorC = c;
        mejorY = cy;
        mejorX = cx;
      }
    }
  }
  int dy = mejorY - syi;
  if (dy > vel)
    dy = vel;
  if (dy < -vel)
    dy = -vel;
  m.sy += dy;
  int dx = mejorX - sxi;
  if (dx > vel)
    dx = vel;
  if (dx < -vel)
    dx = -vel;
  m.sx += dx;
  if (m.sx < 4)
    m.sx = 4;
  if (m.sx > 190)
    m.sx = 190;
  if (m.jf.on && m.sx > m.jf.x - 40)
    m.sx = (float)(m.jf.x - 40);
}

static void nemOpcion(int i, int &ox, int &oy)
{
  NemMem &m = *nemM;
  int idx = (m.hi + NEM_HIST - (i + 1) * 7) % NEM_HIST;
  ox = m.hx[idx];
  oy = m.hy[idx];
}

static void nemNueva()
{
  NemMem &m = *nemM;
  memset(&m, 0, sizeof(NemMem));
  m.vidas = NEM_VIDAS;
  m.fase = 1;
  nemPaleta();
  m.curT = m.tgT = 4;
  m.curS = m.tgS = 4;
  for (int x = 0; x < NEM_W; x++)
    nemColumna(x);
  m.dist = 0;
  m.proxOla = 200;
  for (int i = 0; i < NEM_NST; i++)
  {
    m.es[i].x = (int16_t)nemRnd(NEM_W);
    m.es[i].y = (uint8_t)nemRnd(NEM_H);
    m.es[i].capa = (uint8_t)nemRnd(3);
  }
  m.sx = 30;
  m.sy = NEM_H / 2 - 3;
  for (int i = 0; i < NEM_HIST; i++)
  {
    m.hx[i] = (int16_t)m.sx;
    m.hy[i] = (int16_t)m.sy;
  }
  m.inmune = 60;
  m.sucio = true;
  m.sucioMeter = true;
  m.tTxt[0] = 0;
  digitalWrite(LED_PIN, HIGH);
  digitalWrite(LED_PIN_G, HIGH);
  digitalWrite(LED_PIN_B, HIGH);
}

static void nemFin(unsigned long now)
{
  NemMem &m = *nemM;
  m.estado = 2;
  m.finAt = now;
  if (m.puntos > nemRecord)
    nemRecord = m.puntos;
  nemPartidas++;
  Serial.printf("M8AX - Nemesis: Partida %lu | Puntos %lu | Fase %u | Record %lu\n",
                (unsigned long)nemPartidas, (unsigned long)m.puntos, (unsigned)m.fase, (unsigned long)nemRecord);
}

// ---------------------------------------------------------------- un fotograma de logica
static void nemActualiza(unsigned long now)
{
  NemMem &m = *nemM;
  nemScroll();

  for (int i = 0; i < NEM_NST; i++)
  {
    m.es[i].x -= m.es[i].capa + 1;
    if (m.es[i].x < 0)
    {
      m.es[i].x = NEM_W - 1;
      m.es[i].y = (uint8_t)nemRnd(NEM_H);
    }
  }

  // oleadas y jefe
  if (!m.jf.on)
  {
    if (m.dist < NEM_FASE_PX)
    {
      if (m.dist >= m.proxOla)
      {
        nemOla();
        int gap = 190 - nemDif() * 18;
        if (gap < 90)
          gap = 90;
        m.proxOla = m.dist + gap + nemRnd(80);
      }
    }
    else
    {
      bool libre = true;
      for (int i = 0; i < NEM_MAXE; i++)
        if (m.en[i].hp)
          libre = false;
      if (libre)
        nemJefeEntra();
    }
  }

  // nave
  if (m.estado == 0)
  {
    nemIA();
    nemActivaPower();
    m.hi = (uint8_t)((m.hi + 1) % NEM_HIST);
    m.hx[m.hi] = (int16_t)m.sx;
    m.hy[m.hi] = (int16_t)m.sy;
    if (m.cad)
      m.cad--;
    else
    {
      m.cad = 7;
      nemDispara((int)m.sx, (int)m.sy);
      for (int i = 0; i < m.opciones; i++)
      {
        int ox, oy;
        nemOpcion(i, ox, oy);
        nemDispara(ox, oy);
      }
    }
    if (m.miss)
    {
      if (m.cadMis)
        m.cadMis--;
      else
      {
        m.cadMis = 24;
        nemPb((int)m.sx + 6, (int)m.sy + 6, 2, 2, 3);
        for (int i = 0; i < m.opciones; i++)
        {
          int ox, oy;
          nemOpcion(i, ox, oy);
          nemPb(ox + 6, oy + 6, 2, 2, 3);
        }
      }
    }
    if (m.inmune)
      m.inmune--;
    else
    {
      for (int col = (int)m.sx + 2; col <= (int)m.sx + 14; col += 2)
        if ((int)m.sy + 1 < m.techo[col] || (int)m.sy + 6 >= NEM_H - m.suelo[col])
        {
          nemNaveMuere();
          break;
        }
    }
  }
  else if (m.estado == 1)
  {
    if (++m.muerteT >= 50)
    {
      if (m.vidas > 0)
      {
        m.estado = 0;
        m.sx = 30;
        m.sy = NEM_H / 2 - 3;
        for (int i = 0; i < NEM_HIST; i++)
        {
          m.hx[i] = (int16_t)m.sx;
          m.hy[i] = (int16_t)m.sy;
        }
        m.inmune = 80;
        for (int i = 0; i < NEM_MAXEB; i++)
          m.eb[i].on = false;
      }
      else
        nemFin(now);
    }
  }

  // balas de la nave
  for (int i = 0; i < NEM_MAXPB; i++)
  {
    NemPb &b = m.pb[i];
    if (!b.on)
      continue;
    if (b.tipo == 3)
    {
      b.x += b.vx;
      b.y += b.vy;
      int col = b.x < 0 ? 0 : (b.x >= NEM_W ? NEM_W - 1 : b.x);
      if (b.y + 3 >= NEM_H - m.suelo[col])
      {
        b.tipo = 4;
        b.vx = 3;
        b.vy = 0;
      }
    }
    else if (b.tipo == 4)
    {
      b.x += b.vx;
      int col = b.x < 0 ? 0 : (b.x >= NEM_W ? NEM_W - 1 : b.x);
      b.y = (int16_t)(NEM_H - m.suelo[col] - 3);
    }
    else
    {
      b.x += b.vx;
      b.y += b.vy;
    }
    if (b.x > NEM_W + 24 || b.y < -4 || b.y > NEM_H)
      b.on = false;
  }

  nemEnemigos();
  nemJefe();

  // balas de los enemigos
  for (int i = 0; i < NEM_MAXEB; i++)
  {
    NemBala &b = m.eb[i];
    if (!b.on)
      continue;
    b.x += b.vx;
    b.y += b.vy;
    if (b.x < -4 || b.x > NEM_W + 4 || b.y < -4 || b.y > NEM_H + 4)
      b.on = false;
  }

  // capsulas
  for (int i = 0; i < NEM_MAXC; i++)
  {
    NemCap &c = m.ca[i];
    if (!c.on)
      continue;
    c.x -= 1.5f;
    if (c.x < -8)
    {
      c.on = false;
      continue;
    }
    if (m.estado == 0 && nemSolapa((int)c.x, (int)c.y, 7, 7, (int)m.sx, (int)m.sy, 16, 7))
    {
      c.on = false;
      m.meterPos = (uint8_t)(m.meterPos % 6 + 1);
      m.retardoAct = 14;
      m.puntos += 50;
      m.sucio = true;
      m.sucioMeter = true;
      nemLed(false, false, true, 120, 1);
    }
  }

  // balas de la nave contra enemigos y jefe
  for (int i = 0; i < NEM_MAXPB; i++)
  {
    NemPb &b = m.pb[i];
    if (!b.on)
      continue;
    int rx, ry, rw, rh;
    if (b.tipo == 2)
    {
      rx = b.x - 20;
      ry = b.y - 1;
      rw = 21;
      rh = 3;
    }
    else if (b.tipo == 1)
    {
      rx = b.x - 1;
      ry = b.y - 1;
      rw = 3;
      rh = 3;
    }
    else
    {
      rx = b.x - 3;
      ry = b.y - 1;
      rw = 4;
      rh = 3;
    }
    bool gastada = false;
    for (int k = 0; k < NEM_MAXE && !gastada; k++)
    {
      NemEn &e = m.en[k];
      if (!e.hp || e.ret)
        continue;
      int ew = (e.tipo == NEM_T_DUCKER) ? 7 : 9;
      int eh = (e.tipo == NEM_T_FAN) ? 7 : 5;
      if (nemSolapa(rx, ry, rw, rh, e.x, e.y, ew, eh))
      {
        e.hp--;
        if (e.hp == 0)
          nemMataEn(e);
        if (b.tipo != 2)
          gastada = true;
      }
    }
    if (!gastada && m.jf.on && m.jf.fin == 0 && nemSolapa(rx, ry, rw, rh, m.jf.x, m.jf.y, NEM_BW, NEM_BH))
    {
      int cy = ry + rh / 2;
      if (m.jf.abierto && cy >= m.jf.y + 11 && cy < m.jf.y + 25)
      {
        m.jf.hp--;
        m.puntos += 10;
        m.sucio = true;
        nemExplota(rx + 2, ry, 5);
        if (m.jf.hp <= 0)
          nemJefeMuere();
      }
      gastada = true;
    }
    if (gastada)
      b.on = false;
  }

  // la nave contra balas, enemigos y jefe
  if (m.estado == 0 && !m.inmune)
  {
    bool muere = false;
    for (int i = 0; i < NEM_MAXEB && !muere; i++)
    {
      NemBala &b = m.eb[i];
      if (!b.on)
        continue;
      if (m.escudo && nemSolapa((int)b.x - 1, (int)b.y - 1, 3, 3, (int)m.sx + 15, (int)m.sy - 3, 5, 13))
      {
        b.on = false;
        m.escudo--;
        m.sucioMeter = true;
        continue;
      }
      if (nemSolapa((int)b.x - 1, (int)b.y - 1, 3, 3, (int)m.sx + 3, (int)m.sy + 1, 10, 5))
      {
        b.on = false;
        muere = true;
      }
    }
    for (int k = 0; k < NEM_MAXE && !muere; k++)
    {
      NemEn &e = m.en[k];
      if (!e.hp || e.ret)
        continue;
      int ew = (e.tipo == NEM_T_DUCKER) ? 7 : 9;
      int eh = (e.tipo == NEM_T_FAN) ? 7 : 5;
      bool toca = nemSolapa(e.x, e.y, ew, eh, (int)m.sx + 3, (int)m.sy + 1, 10, 5);
      bool tocaEsc = m.escudo && nemSolapa(e.x, e.y, ew, eh, (int)m.sx + 15, (int)m.sy - 3, 5, 13);
      if (tocaEsc && !toca)
      {
        m.escudo--;
        m.sucioMeter = true;
        nemMataEn(e);
      }
      else if (toca)
        muere = true;
    }
    if (!muere && m.jf.on && m.jf.fin == 0 && nemSolapa(m.jf.x, m.jf.y - 5, NEM_BW, NEM_BH + 10, (int)m.sx + 3, (int)m.sy + 1, 10, 5))
      muere = true;
    if (muere)
      nemNaveMuere();
  }

  // explosiones
  for (int i = 0; i < NEM_MAXX; i++)
    if (m.ex[i].on && ++m.ex[i].t >= m.ex[i].tmax)
      m.ex[i].on = 0;
}

// ---------------------------------------------------------------- dibujo
static void nemJefeDib()
{
  NemMem &m = *nemM;
  NemJefe &j = m.jf;
  if (!j.on)
    return;
  int x = j.x, y = j.y;
  nemRect(x + NEM_BW, y + 8, 3 + (m.fr & 3), 4, 8);
  nemRect(x + NEM_BW, y + NEM_BH - 12, 3 + ((m.fr >> 1) & 3), 4, 8);
  nemRect(x + 14, y - 5, 26, 5, 4);
  nemRect(x + 14, y + NEM_BH, 26, 5, 4);
  nemRect(x + 6, y, NEM_BW - 6, NEM_BH, 4);
  nemRect(x + 6, y, NEM_BW - 6, 2, 3);
  nemRect(x + 6, y + NEM_BH - 2, NEM_BW - 6, 2, 3);
  nemRect(x + 20, y + 4, 4, NEM_BH - 8, 6);
  nemRect(x + 30, y + 4, 4, NEM_BH - 8, 6);
  nemRect(x, y, 8, 11, 6);
  nemRect(x, y + 25, 8, 11, 6);
  nemRect(x + 1, y + 1, 6, 2, 5);
  nemRect(x + 1, y + 26, 6, 2, 5);
  if (j.abierto && j.fin == 0)
  {
    nemRect(x + 2, y + 11, 12, 14, 15);
    nemRect(x + 4, y + 14, 8, 8, (m.fr & 2) ? 9 : 7);
  }
  else
  {
    nemRect(x, y + 11, 8, 14, 6);
    nemRect(x + 2, y + 13, 4, 10, 5);
  }
  if (j.fin > 0 && (m.fr & 2))
    nemRect(x + 10, y + 6, 22, 12, 7);
}

static void nemDibuja()
{
  NemMem &m = *nemM;
  memset(m.fb, 0, sizeof(m.fb));

  // estrellas
  for (int i = 0; i < NEM_NST; i++)
    nemPx(m.es[i].x, m.es[i].y, (uint8_t)(1 + m.es[i].capa));

  // terreno
  for (int x = 0; x < NEM_W; x++)
  {
    int t = m.techo[x], s = m.suelo[x];
    int w = x + m.mundo;
    for (int y = 0; y < t; y++)
      nemPx(x, y, (y == t - 1) ? 13 : (y == t - 2 ? 12 : ((((w * 7) + y * 13) & 15) == 0 ? 12 : 11)));
    for (int y = NEM_H - s; y < NEM_H; y++)
    {
      int d = y - (NEM_H - s);
      nemPx(x, y, (d == 0) ? 13 : (d == 1 ? 12 : ((((w * 7) + y * 13) & 15) == 0 ? 12 : 11)));
    }
  }

  // enemigos
  for (int i = 0; i < NEM_MAXE; i++)
  {
    NemEn &e = m.en[i];
    if (!e.hp || e.ret)
      continue;
    switch (e.tipo)
    {
    case NEM_T_FAN:
      nemSpr((m.fr & 4) ? NEM_SP_FAN : NEM_SP_FAN2, 9, 7, e.x, e.y, false);
      break;
    case NEM_T_GARUN:
      nemSpr(NEM_SP_GAR, 9, 5, e.x, e.y, false);
      break;
    case NEM_T_RUGAL:
      nemSpr(NEM_SP_RUG, 9, 5, e.x, e.y, false);
      break;
    default:
      nemSpr(NEM_SP_DUC, 7, 5, e.x, e.y, e.flip != 0);
      break;
    }
    if (e.cap && (m.fr & 4)) // el portador de capsula parpadea en naranja
      nemRect(e.x + 3, e.y + 1, 3, 2, 8);
  }

  nemJefeDib();

  // capsulas
  for (int i = 0; i < NEM_MAXC; i++)
    if (m.ca[i].on)
      nemSpr(NEM_SP_CAP, 7, 7, (int)m.ca[i].x, (int)m.ca[i].y, false);

  // balas enemigas
  for (int i = 0; i < NEM_MAXEB; i++)
    if (m.eb[i].on)
    {
      int bx = (int)m.eb[i].x, by = (int)m.eb[i].y;
      nemRect(bx - 1, by - 1, 3, 3, 8);
      nemPx(bx, by, 9);
    }

  // balas de la nave
  for (int i = 0; i < NEM_MAXPB; i++)
  {
    NemPb &b = m.pb[i];
    if (!b.on)
      continue;
    if (b.tipo == 2)
    {
      nemRect(b.x - 20, b.y - 1, 21, 3, 5);
      nemRect(b.x - 20, b.y, 21, 1, 3);
    }
    else if (b.tipo == 1)
      nemRect(b.x - 1, b.y - 1, 3, 3, 9);
    else if (b.tipo == 0)
      nemRect(b.x - 3, b.y - 1, 4, 2, 3);
    else
    {
      nemRect(b.x - 3, b.y - 1, 4, 3, 8);
      nemPx(b.x, b.y, 9);
    }
  }

  // opciones y nave
  if (m.estado == 0)
  {
    for (int i = 0; i < m.opciones; i++)
    {
      int ox, oy;
      nemOpcion(i, ox, oy);
      nemSpr(NEM_SP_OPC, 5, 5, ox + 5, oy + 1, false);
    }
    if (!m.inmune || (m.fr & 2))
    {
      nemSpr(NEM_SP_VIC, 16, 7, (int)m.sx, (int)m.sy, false);
      if (m.fr & 1)
        nemPx((int)m.sx - 1, (int)m.sy + 3, 8);
    }
    if (m.escudo && (m.escudo > 1 || (m.fr & 2)))
    {
      int ex = (int)m.sx + 17, ey = (int)m.sy;
      nemRect(ex, ey - 2, 2, 11, 5);
      nemRect(ex + 1, ey, 1, 7, 3);
      nemRect(ex - 2, ey - 3, 3, 1, 5);
      nemRect(ex - 2, ey + 9, 3, 1, 5);
    }
  }

  // explosiones
  for (int i = 0; i < NEM_MAXX; i++)
  {
    NemExp &x = m.ex[i];
    if (!x.on)
      continue;
    int rr = 2 + (int)x.t * x.r / x.tmax;
    uint8_t col = (x.t * 3 < x.tmax) ? 9 : ((x.t * 3 < x.tmax * 2) ? 8 : 7);
    nemDisco(x.x, x.y, rr, col);
    if (x.t * 3 < x.tmax)
      nemDisco(x.x, x.y, rr / 2, 3);
  }
}

// vuelca el buffer de 4 bits a la pantalla pasando por la paleta
static void nemEmpuja()
{
  NemMem &m = *nemM;
  tft.startWrite();
  tft.setAddrWindow(0, NEM_Y0, NEM_W, NEM_H);
  for (int y = 0; y < NEM_H; y++)
  {
    const uint8_t *s = &m.fb[y * (NEM_W / 2)];
    uint16_t *d = m.linea;
    for (int x = 0; x < NEM_W / 2; x++)
    {
      uint8_t b = s[x];
      d[0] = m.pal[b & 15];
      d[1] = m.pal[b >> 4];
      d += 2;
    }
    tft.pushColors(m.linea, NEM_W, true);
  }
  tft.endWrite();
}

static void nemTexto(bool forzar)
{
  NemMem &m = *nemM;
  char b[64];
  uint32_t rec = (m.puntos > nemRecord) ? m.puntos : nemRecord;
  snprintf(b, sizeof(b), "PTS %06lu   FASE %02u   VIDAS %u   REC %06lu",
           (unsigned long)m.puntos, (unsigned)m.fase, (unsigned)m.vidas, (unsigned long)rec);
  if (!forzar && strcmp(b, m.tTxt) == 0)
    return;
  strcpy(m.tTxt, b);
  tft.setFreeFont(NULL);
  tft.setTextFont(1);
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextDatum(TC_DATUM);
  tft.setTextPadding(312);
  tft.drawString(b, 160, 142);
  tft.setTextPadding(0);
  tft.setTextDatum(TL_DATUM);
}

// la barra de power-ups de abajo: SPEED, MISSL, DOUBL, LASER, OPTION, ?
static void nemMeter()
{
  NemMem &m = *nemM;
  static const char *const nom[6] = {"SPEED", "MISSL", "DOUBL", "LASER", "OPTION", "  ?  "};
  bool tiene[6] = {m.vel > 0, m.miss != 0, m.doble != 0, m.laser != 0, m.opciones > 0, m.escudo > 0};
  tft.setFreeFont(NULL);
  tft.setTextFont(1);
  tft.setTextSize(1);
  tft.setTextPadding(0);
  tft.setTextDatum(MC_DATUM);
  for (int i = 0; i < 6; i++)
  {
    int x = 4 + i * 52;
    bool sel = (m.meterPos == i + 1);
    uint16_t fondo = sel ? TFT_YELLOW : TFT_NAVY;
    uint16_t txt = sel ? TFT_BLACK : (tiene[i] ? TFT_GREEN : TFT_WHITE);
    tft.fillRect(x, 153, 50, 14, fondo);
    tft.drawRect(x, 153, 50, 14, sel ? TFT_WHITE : TFT_BLUE);
    tft.setTextColor(txt, fondo);
    tft.drawString(nom[i], x + 25, 160);
  }
  tft.setTextDatum(TL_DATUM);
}

static void nemTextoFin()
{
  tft.setFreeFont(NULL);
  tft.setTextFont(1);
  tft.setTextSize(3);
  tft.setTextColor(TFT_RED, TFT_BLACK);
  tft.setTextDatum(MC_DATUM);
  tft.drawString("GAME OVER", 160, NEM_Y0 + 56);
  tft.setTextDatum(TL_DATUM);
  tft.setTextSize(1);
}

static void nemPintaTodo()
{
  nemDibuja();
  nemEmpuja();
  nemTexto(true);
  nemMeter();
  nemM->sucioMeter = false;
  if (nemM->estado == 2)
    nemTextoFin();
}

static void nemFrame(unsigned long now)
{
  NemMem &m = *nemM;
  m.fr++;
  if (m.estado == 2)
  {
    if (now - m.finAt >= NEM_FIN_MS)
    {
      nemNueva();
      nemPintaTodo();
    }
    return;
  }
  nemActualiza(now);
  nemDibuja();
  nemEmpuja();
  nemTexto(false);
  if (m.sucioMeter)
  {
    nemMeter();
    m.sucioMeter = false;
  }
  if (m.estado == 2)
    nemTextoFin();
}
// <<< NEMESIS MOTOR

void esp32_2432S028R_NemesisScreen(unsigned long mElapsed)
{
  mineria = getMiningData(mElapsed);
  Serial.printf("M8AX - %s >>> Completados %s Share(s), %s Khashes, Prom. Hashrate %s KH/s %s°\n",
                mineria.currentTime.c_str(), mineria.completedShares.c_str(), mineria.totalKHashes.c_str(), mineria.currentHashRate.c_str(), mineria.temp.c_str());

  bool redraw = hasChangedScreen;
  if (!nemM)
  {
    nemM = (NemMem *)malloc(sizeof(NemMem));
    if (!nemM)
    {
      Serial.println("M8AX - Nemesis: No Hay RAM Libre");
      delay(1000);
      return;
    }
    nemNueva();
    redraw = true;
  }
  const int nemMiPantalla = currentDisplayDriver->current_cyclic_screen;
  if (redraw)
  {
    tft.fillRect(0, 0, 320, 170, TFT_BLACK);
    nemPintaTodo();
  }
  printPoolData(); // barra de abajo, solo se repinta cuando toca
  hasChangedScreen = false;

  char horaStr[9] = "";
  char lastHora[9] = "";
  char hashStr[16];
  char lastHash[16] = "";

  unsigned long start = millis();
  unsigned long lastMiningUpdate = 0;
  unsigned long lastTouchChk = 0;
  unsigned long siguiente = start;
  pongLeds(false);
  nemM->ledOff = 0;

  while (millis() - start < NEM_SLICE_MS)
  {
    unsigned long now = millis();

    // ---- Táctil (misma lógica que el Pong) ----
    if (now - lastTouchChk >= 40 && now - previousTouchMillis >= 500)
    {
      lastTouchChk = now;
      int16_t t_x, t_y;
      if (touch.getXY(t_x, t_y))
      {
        previousTouchMillis = now;
        bloqueAvisoActivo = false;
        if (((t_x > 109) && (t_x < 211)) && ((t_y > 185) && (t_y < 241)))
        {
          bottomScreenBlue ^= true;
          hasChangedScreen = true;
        }
        else if ((t_x > 235) && ((t_y > 0) && (t_y < 16)))
        {
          esp32_2432S028R_AlternateScreenState();
        }
        else if (t_x > 160)
        {
          currentDisplayDriver->current_cyclic_screen = (currentDisplayDriver->current_cyclic_screen + 1) % currentDisplayDriver->num_cyclic_screens;
        }
        else
        {
          currentDisplayDriver->current_cyclic_screen = currentDisplayDriver->current_cyclic_screen - 1;
          if (currentDisplayDriver->current_cyclic_screen < 0)
            currentDisplayDriver->current_cyclic_screen = currentDisplayDriver->num_cyclic_screens - 1;
        }
        break;
      }
    }
    if (nemM->ledOff && now >= nemM->ledOff)
    {
      pongLeds(false);
      nemM->ledOff = 0;
      nemM->ledPrio = 0;
    }

    if (now >= siguiente)
    {
      nemFrame(now);
      siguiente += NEM_FRAME_MS; // paso fijo: si un fotograma tarda, el siguiente no espera 30 ms mas
      if (millis() > siguiente + NEM_FRAME_MS)
        siguiente = millis(); // si va muy atrasado no acumula
    }

    // ---- Hora (izquierda) y hashrate (derecha), arriba como en las demás ----
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
        pongDrawHud(hashStr, horaStr);
        strcpy(lastHora, horaStr);
        strcpy(lastHash, hashStr);
      }
    }
    webAtender();
    delay(2);
  }

  pongLeds(false);
  tft.setTextPadding(0);
  tft.setTextDatum(TL_DATUM);

  if (currentDisplayDriver->current_cyclic_screen != nemMiPantalla)
  {
    free(nemM);
    nemM = NULL;
  }

  cuentita++;
  if (cuentita == 15)
  {
    ajustarZonaHoraria();
    obtenerLocYTemp();
    obtenerVariacionesBTC(variacion1dia, variacion1semana);
  }
}

/****************************************************************************************************************
 *
 *   Escrito por: M8AX
 *
 *   Descripción: CONECTA 4 M8AX - El 4 en raya de toda la vida, tú contra la CPU, para ESP32_2432S028R
 *   --------------------------------------------------------------------------------------------------
 *
 *   Tablero de 7 columnas por 6 filas. Tú juegas con las fichas rojas y la CPU con las amarillas. Tocas la
 *   columna donde quieres soltar la ficha y cae hasta el fondo. Gana el primero que junte cuatro seguidas,
 *   en horizontal, en vertical o en diagonal, y esas cuatro fichas parpadean. Si el tablero se llena sin
 *   que nadie gane, tablas. La primera partida la abres tú y después se van alternando.
 *
 *   A la derecha están los marcadores ( tú, CPU y tablas ) y dos botones: Nivel, que va rotando entre Facil,
 *   Normal y Dificil, y Nueva, para empezar otra partida cuando quieras. Cuando una partida ha terminado,
 *   tocar el tablero también empieza la siguiente. Arriba, igual que en la calculadora: < y > para cambiar
 *   de pantalla, LUZ para apagar y encender la pantalla, y junto a ellos la hora y el hashrate.
 *
 *   La CPU piensa con minimax y poda alfa-beta sobre bitboards ( cada jugador cabe en un número de 64 bits ).
 *   Va mirando una jugada más por delante cada vuelta y se para cuando se le acaba el tiempo, así que siempre
 *   mueve: 0,1 segundos en Facil ( y una de cada cuatro veces tira al azar ), 0,6 en Normal y 1,5 en Dificil.
 *   Si ve que puede ganar ya, gana, y si tú tienes una línea a punto de cerrarse, la tapa.
 *
 *   LEDs: rojo corto al soltar tú la ficha, azul corto cuando la suelta la CPU, verde si ganas, rojo largo si
 *   gana la CPU y los tres a la vez en las tablas.
 *
 *   Memoria: nada de heap, solo unos 60 bytes con la partida. Si cambias de pantalla y vuelves, la partida
 *   sigue donde la dejaste ( mientras no se reinicie el minero ).
 *
 *   Para activarla: pegar este bloque justo antes del array esp32_2432S028RCyclicScreens[] y meter
 *   esp32_2432S028R_Conecta4Screen en ese array. En analiCadaSegundo no hay que añadir nada porque no pide memoria.
 *
 ****************************************************************************************************************/

#define C4_SLICE_MS 5000
#define C4_HH 20 // alto de la cabecera
#define C4_CS 30 // lado de cada casilla
#define C4_X0 4
#define C4_Y0 24
#define C4_PX 222 // donde empieza el panel de la derecha
#define C4_PW 96
#define C4_AZUL 0x0016
#define C4_MARCO 0x041F

struct C4Estado
{
  bool init;
  uint8_t turno;   // 0 = tú, 1 = CPU
  uint8_t fin;     // 0 jugando, 1 ganas tú, 2 gana la CPU, 3 tablas
  uint8_t nivel;   // 0 facil, 1 normal, 2 dificil
  uint8_t empieza; // quién abrió esta partida
  uint8_t jugadas;
  uint16_t marc[3]; // tú, CPU, tablas
  uint64_t fich[2];
  uint64_t cuatro; // casillas de la línea ganadora
};
static C4Estado c4 = {false};

// >>> CONECTA 4 MOTOR
#define C4_GANA 100000
#define C4_INF 1000000

static uint32_t c4Nodos;
static unsigned long c4Limite;
static bool c4Corte;
static const uint8_t C4_ORDEN[7] = {3, 2, 4, 1, 5, 0, 6};

static inline uint64_t c4Fondo(int c) { return 1ULL << (c * 7); }
static inline uint64_t c4Col(int c) { return 0x3FULL << (c * 7); }

static bool c4Cuatro(uint64_t p)
{
  uint64_t m = p & (p >> 7);
  if (m & (m >> 14))
    return true;
  m = p & (p >> 6);
  if (m & (m >> 12))
    return true;
  m = p & (p >> 8);
  if (m & (m >> 16))
    return true;
  m = p & (p >> 1);
  return (m & (m >> 2)) != 0;
}

// todas las casillas que forman alguna linea de 4 ( para hacerlas parpadear )
static uint64_t c4Linea(uint64_t p)
{
  static const uint8_t D[4] = {1, 6, 7, 8};
  uint64_t r = 0;
  for (int i = 0; i < 4; i++)
  {
    int d = D[i];
    uint64_t m = p & (p >> d) & (p >> (2 * d)) & (p >> (3 * d));
    r |= m | (m << d) | (m << (2 * d)) | (m << (3 * d));
  }
  return r;
}

// tres fichas de p y un hueco libre en la misma linea de 4
static int c4Treses(uint64_t p, uint64_t libre)
{
  static const uint8_t D[4] = {1, 6, 7, 8};
  int n = 0;
  for (int i = 0; i < 4; i++)
  {
    int d = D[i];
    uint64_t p1 = p >> d, p2 = p >> (2 * d), p3 = p >> (3 * d);
    uint64_t l1 = libre >> d, l2 = libre >> (2 * d), l3 = libre >> (3 * d);
    n += __builtin_popcountll((p & p1 & p2 & l3) | (libre & p1 & p2 & p3) | (p & l1 & p2 & p3) | (p & p1 & l2 & p3));
  }
  return n;
}

// valor para quien mueve ( yo = sus fichas, todo = fichas de los dos )
static int c4Eval(uint64_t yo, uint64_t todo)
{
  uint64_t el = yo ^ todo;
  uint64_t libre = 0;
  for (int c = 0; c < 7; c++)
    libre |= c4Col(c);
  libre &= ~todo;
  const uint64_t cen = c4Col(3), lat = c4Col(2) | c4Col(4);
  int s = 24 * (c4Treses(yo, libre) - c4Treses(el, libre));
  s += 3 * (__builtin_popcountll(yo & cen) - __builtin_popcountll(el & cen));
  s += __builtin_popcountll(yo & lat) - __builtin_popcountll(el & lat);
  return s;
}

static int c4Neg(uint64_t pos, uint64_t todo, int prof, int alfa, int beta, int ply)
{
  if ((++c4Nodos & 511) == 0 && millis() >= c4Limite)
    c4Corte = true;
  if (c4Corte)
    return 0;
  if (prof <= 0)
    return c4Eval(pos, todo);

  uint64_t el = pos ^ todo;
  uint64_t nb[7];
  uint64_t forzada = 0;
  int n = 0, nf = 0;
  for (int i = 0; i < 7; i++)
  {
    int c = C4_ORDEN[i];
    if (todo & (1ULL << (c * 7 + 5)))
      continue;
    uint64_t b = (todo + c4Fondo(c)) & c4Col(c);
    if (c4Cuatro(pos | b))
      return C4_GANA - ply;
    if (c4Cuatro(el | b))
    {
      forzada = b;
      nf++;
    }
    nb[n++] = b;
  }
  if (n == 0)
    return 0;
  if (nf >= 2)
    return -(C4_GANA - ply - 1);
  if (nf == 1)
  {
    nb[0] = forzada;
    n = 1;
  }

  int mejor = -C4_INF;
  for (int i = 0; i < n; i++)
  {
    int v = -c4Neg(el, todo | nb[i], prof - 1, -beta, -alfa, ply + 1);
    if (c4Corte)
      return 0;
    if (v > mejor)
    {
      mejor = v;
      if (v > alfa)
      {
        alfa = v;
        if (alfa >= beta)
          break;
      }
    }
  }
  return mejor;
}

// columna que juega la CPU. nivel 0 facil, 1 normal, 2 dificil
static int c4Piensa(uint64_t yo, uint64_t el, int nivel)
{
  static const uint8_t PROF[3] = {2, 5, 14};
  static const uint16_t MS[3] = {120, 600, 1500};
  uint64_t todo = yo | el;
  int libres[7], nl = 0;
  for (int i = 0; i < 7; i++)
    if (!(todo & (1ULL << (C4_ORDEN[i] * 7 + 5))))
      libres[nl++] = C4_ORDEN[i];
  if (nl <= 1)
    return nl ? libres[0] : 3;
  if (nivel == 0 && (esp_random() & 3) == 0)
    return libres[esp_random() % nl];

  c4Limite = millis() + MS[nivel];
  c4Nodos = 0;
  c4Corte = false;
  int mejor = libres[0];
  for (int prof = 1; prof <= PROF[nivel]; prof++)
  {
    int ord[7], no = 0;
    ord[no++] = mejor;
    for (int i = 0; i < nl; i++)
      if (libres[i] != mejor)
        ord[no++] = libres[i];
    int mc = -1, mv = -C4_INF, alfa = -C4_INF;
    for (int k = 0; k < no; k++)
    {
      int c = ord[k];
      uint64_t b = (todo + c4Fondo(c)) & c4Col(c);
      if (c4Cuatro(yo | b))
        return c;
      int v = -c4Neg(el, todo | b, prof - 1, -C4_INF, -alfa, 1);
      if (c4Corte)
        break;
      if (v > mv)
      {
        mv = v;
        mc = c;
        if (v > alfa)
          alfa = v;
      }
    }
    if (c4Corte)
      break;
    mejor = mc;
    if (mv >= C4_GANA - 100 || mv <= -C4_GANA + 100)
      break;
  }
  return mejor;
}

static void c4Celda(int c, int r, int quien, bool resalta)
{
  int x = C4_X0 + c * C4_CS, y = C4_Y0 + (5 - r) * C4_CS;
  int cx = x + C4_CS / 2, cy = y + C4_CS / 2;
  tft.fillRect(x, y, C4_CS, C4_CS, C4_AZUL);
  if (quien == 0)
  {
    tft.fillCircle(cx, cy, 12, TFT_BLACK);
    return;
  }
  uint16_t col = quien == 1 ? TFT_RED : TFT_YELLOW;
  uint16_t borde = quien == 1 ? 0xA000 : 0xBD80;
  tft.fillCircle(cx, cy, 12, borde);
  tft.fillCircle(cx, cy, 10, col);
  tft.fillCircle(cx - 3, cy - 3, 3, quien == 1 ? 0xFC10 : 0xFFF0);
  if (resalta)
  {
    tft.drawCircle(cx, cy, 13, TFT_WHITE);
    tft.drawCircle(cx, cy, 14, TFT_WHITE);
  }
}

static int c4Dueno(int c, int r)
{
  uint64_t b = 1ULL << (c * 7 + r);
  if (c4.fich[0] & b)
    return 1;
  if (c4.fich[1] & b)
    return 2;
  return 0;
}

static void c4PintaTablero()
{
  tft.fillRect(C4_X0 - 3, C4_Y0 - 3, 7 * C4_CS + 6, 6 * C4_CS + 6, C4_MARCO);
  for (int c = 0; c < 7; c++)
    for (int r = 0; r < 6; r++)
      c4Celda(c, r, c4Dueno(c, r), false);
}

static void c4Ganadoras(bool on)
{
  for (int c = 0; c < 7; c++)
    for (int r = 0; r < 6; r++)
      if (c4.cuatro & (1ULL << (c * 7 + r)))
        c4Celda(c, r, c4Dueno(c, r), on);
}

static void c4Mensaje(const char *t, uint16_t col)
{
  tft.setFreeFont(NULL);
  tft.setTextFont(1);
  tft.setTextSize(2);
  tft.setTextColor(col, TFT_BLACK);
  tft.setTextDatum(MC_DATUM);
  tft.setTextPadding(320);
  tft.drawString(t, 160, 226);
  tft.setTextPadding(0);
  tft.setTextDatum(TL_DATUM);
}

static void c4Marcadores()
{
  char b[20];
  tft.setFreeFont(NULL);
  tft.setTextFont(1);
  tft.setTextDatum(TL_DATUM);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(3);
  tft.setTextPadding(C4_PW - 8);
  snprintf(b, sizeof(b), "%u", c4.marc[0]);
  tft.drawString(b, C4_PX + 6, 44);
  snprintf(b, sizeof(b), "%u", c4.marc[1]);
  tft.drawString(b, C4_PX + 6, 94);
  tft.setTextSize(1);
  tft.setTextPadding(C4_PW);
  snprintf(b, sizeof(b), "Tablas: %u", c4.marc[2]);
  tft.drawString(b, C4_PX + 6, 126);
  tft.setTextPadding(0);
}

static void c4BotonNivel()
{
  static const char *const N[3] = {"Facil", "Normal", "Dificil"};
  static const uint16_t C[3] = {0x03E0, 0xFD20, 0xB000};
  tft.fillRoundRect(C4_PX, 140, C4_PW, 26, 5, C[c4.nivel]);
  tft.setFreeFont(NULL);
  tft.setTextFont(1);
  tft.setTextSize(2);
  tft.setTextPadding(0);
  tft.setTextDatum(MC_DATUM);
  tft.setTextColor(TFT_WHITE, C[c4.nivel]);
  tft.drawString(N[c4.nivel], C4_PX + C4_PW / 2, 153);
  tft.setTextDatum(TL_DATUM);
}

static void c4Cab(const char *hash, const char *hora)
{
  char b[24];
  tft.setFreeFont(NULL);
  tft.setTextFont(1);
  tft.setTextSize(1);
  tft.setTextPadding(56);
  tft.setTextDatum(TL_DATUM);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString(hora, 102, 7);
  snprintf(b, sizeof(b), "%s KH/s", hash);
  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.setTextPadding(90);
  tft.setTextDatum(TR_DATUM);
  tft.drawString(b, 250, 7);
  tft.setTextPadding(0);
  tft.setTextDatum(TL_DATUM);
}

static void c4Hora(char *o)
{
  time_t epoch = (time_t)timeClient.getEpochTime();
  struct tm *ti = localtime(&epoch);
  snprintf(o, 9, "%02d:%02d:%02d", ti->tm_hour, ti->tm_min, ti->tm_sec);
}

static void c4TextoTurno()
{
  if (c4.fin == 1)
    c4Mensaje("Has Ganado! Toca Nueva", TFT_GREEN);
  else if (c4.fin == 2)
    c4Mensaje("Gana La CPU! Toca Nueva", TFT_RED);
  else if (c4.fin == 3)
    c4Mensaje("Empate! Toca Nueva", TFT_WHITE);
  else if (c4.turno == 0)
    c4Mensaje("Tu Turno: Elige Columna", TFT_CYAN);
  else
    c4Mensaje("La CPU Piensa...", TFT_YELLOW);
}

static void c4PintaTodo()
{
  tft.fillRect(0, 0, 320, 240, TFT_BLACK);
  tft.setFreeFont(NULL);
  tft.setTextFont(1);
  tft.setTextPadding(0);
  tft.setTextDatum(MC_DATUM);
  tft.setTextSize(2);
  tft.fillRoundRect(2, 1, 44, 18, 4, 0x0416);
  tft.setTextColor(TFT_WHITE, 0x0416);
  tft.drawString("<", 24, 10);
  tft.fillRoundRect(50, 1, 44, 18, 4, 0x0416);
  tft.drawString(">", 72, 10);
  tft.fillRoundRect(256, 1, 62, 18, 4, 0x4208);
  tft.setTextColor(TFT_WHITE, 0x4208);
  tft.drawString("LUZ", 287, 10);
  tft.setTextDatum(TL_DATUM);

  c4PintaTablero();

  tft.fillCircle(C4_PX + 10, 33, 8, TFT_RED);
  tft.fillCircle(C4_PX + 10, 83, 8, TFT_YELLOW);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.drawString("Tu", C4_PX + 26, 26);
  tft.drawString("CPU", C4_PX + 26, 76);
  c4Marcadores();
  c4BotonNivel();
  tft.fillRoundRect(C4_PX, 172, C4_PW, 26, 5, 0x0416);
  tft.setTextDatum(MC_DATUM);
  tft.setTextColor(TFT_WHITE, 0x0416);
  tft.drawString("Nueva", C4_PX + C4_PW / 2, 185);
  tft.setTextDatum(TL_DATUM);
  c4TextoTurno();
}

static void c4Nueva()
{
  c4.fich[0] = c4.fich[1] = 0;
  c4.cuatro = 0;
  c4.fin = 0;
  c4.jugadas = 0;
  c4.empieza ^= 1;
  c4.turno = c4.empieza;
}

static void c4Cae(int c, int rFin, int quien)
{
  for (int r = 5; r > rFin; r--)
  {
    c4Celda(c, r, quien, false);
    delay(20);
    c4Celda(c, r, 0, false);
  }
  c4Celda(c, rFin, quien, false);
}

// quien: 1 = tú, 2 = CPU. La columna tiene que tener hueco
static void c4Suelta(int c, int quien)
{
  uint64_t todo = c4.fich[0] | c4.fich[1];
  uint64_t nb = (todo + c4Fondo(c)) & c4Col(c);
  int r = __builtin_ctzll(nb) - c * 7;
  c4Cae(c, r, quien);
  c4.fich[quien - 1] |= nb;
  c4.jugadas++;
  if (quien == 1)
    ledFlash(true, false, false, 30);
  else
    ledFlash(false, false, true, 30);

  if (c4Cuatro(c4.fich[quien - 1]))
  {
    c4.fin = quien;
    c4.cuatro = c4Linea(c4.fich[quien - 1]);
    if (c4.marc[quien - 1] < 999)
      c4.marc[quien - 1]++;
    c4Marcadores();
    c4TextoTurno();
    if (quien == 1)
      ledFlash(false, true, false, 400);
    else
      ledFlash(true, false, false, 400);
  }
  else if (c4.jugadas >= 42)
  {
    c4.fin = 3;
    if (c4.marc[2] < 999)
      c4.marc[2]++;
    c4Marcadores();
    c4TextoTurno();
    ledFlash(true, true, true, 300);
  }
  else
  {
    c4.turno = quien == 1 ? 1 : 0;
    c4TextoTurno();
  }
}

void esp32_2432S028R_Conecta4Screen(unsigned long mElapsed)
{
  mineria = getMiningData(mElapsed);
  Serial.printf("M8AX - %s >>> Completados %s Share(s), %s Khashes, Prom. Hashrate %s KH/s %s°\n",
                mineria.currentTime.c_str(), mineria.completedShares.c_str(), mineria.totalKHashes.c_str(), mineria.currentHashRate.c_str(), mineria.temp.c_str());

  bool redraw = hasChangedScreen;
  if (!c4.init)
  {
    memset(&c4, 0, sizeof(c4));
    c4.init = true;
    c4.nivel = 1;
    c4.empieza = 1; // c4Nueva lo da la vuelta y abres tú
    c4Nueva();
    redraw = true;
  }
  hasChangedScreen = false;
  if (redraw)
    c4PintaTodo();

  char hashStr[16], horaStr[9];
  char lastHash[16] = "";
  snprintf(hashStr, sizeof(hashStr), "%s", mineria.currentHashRate.c_str());
  c4Hora(horaStr);
  c4Cab(hashStr, horaStr);
  strcpy(lastHash, hashStr);

  int16_t t_x, t_y;
  bool wasDown = toqueXY(t_x, t_y); // si venimos de un toque, no cuenta
  bool salir = false, parp = false;
  unsigned long start = millis();
  unsigned long lastTouchChk = 0;
  unsigned long lastMiningUpdate = 0;
  unsigned long lastParp = 0;
  pongLeds(false);

  while (!salir && millis() - start < C4_SLICE_MS)
  {
    unsigned long now = millis();

    // ---- Táctil: solo cuenta al pulsar, no mientras se mantiene ----
    if (now - lastTouchChk >= 15)
    {
      lastTouchChk = now;
      bool down = toqueXY(t_x, t_y);
      if (down && !wasDown)
      {
        previousTouchMillis = now;
        if (t_y < C4_HH)
        {
          if (t_x < 48)
          {
            currentDisplayDriver->current_cyclic_screen = currentDisplayDriver->current_cyclic_screen - 1;
            if (currentDisplayDriver->current_cyclic_screen < 0)
              currentDisplayDriver->current_cyclic_screen = currentDisplayDriver->num_cyclic_screens - 1;
            salir = true;
          }
          else if (t_x < 96)
          {
            currentDisplayDriver->current_cyclic_screen = (currentDisplayDriver->current_cyclic_screen + 1) % currentDisplayDriver->num_cyclic_screens;
            salir = true;
          }
          else if (t_x >= 250)
          {
            esp32_2432S028R_AlternateScreenState();
          }
        }
        else if (t_x >= C4_PX && t_y >= 140 && t_y < 168)
        {
          c4.nivel = (c4.nivel + 1) % 3;
          c4BotonNivel();
        }
        else if (t_x >= C4_PX && t_y >= 170 && t_y < 200)
        {
          c4Nueva();
          c4PintaTablero();
          c4TextoTurno();
        }
        else if (t_x >= C4_X0 && t_x < C4_X0 + 7 * C4_CS && t_y >= C4_Y0 && t_y < C4_Y0 + 6 * C4_CS + 8)
        {
          if (c4.fin)
          {
            c4Nueva();
            c4PintaTablero();
            c4TextoTurno();
          }
          else if (c4.turno == 0)
          {
            int c = (t_x - C4_X0) / C4_CS;
            if (!((c4.fich[0] | c4.fich[1]) & (1ULL << (c * 7 + 5))))
              c4Suelta(c, 1);
            else
              ledFlash(true, false, false, 120); // columna llena
          }
        }
      }
      wasDown = down;
    }

    // ---- Le toca a la CPU ----
    if (!salir && c4.fin == 0 && c4.turno == 1)
    {
      unsigned long t0 = millis();
      int col = c4Piensa(c4.fich[1], c4.fich[0], c4.nivel);
      while (millis() - t0 < 450) // que no parezca instantáneo
      {
        webAtender();
        delay(5);
      }
      c4Suelta(col, 2);
      wasDown = toqueXY(t_x, t_y); // lo que tocaras mientras pensaba no vale
    }

    // ---- Parpadeo de la línea ganadora ----
    if (c4.fin == 1 || c4.fin == 2)
    {
      if (now - lastParp >= 350)
      {
        lastParp = now;
        parp = !parp;
        c4Ganadoras(parp);
      }
    }

    // ---- Hora y hashrate, cada segundo ----
    if (now - lastMiningUpdate >= 1000)
    {
      lastMiningUpdate = now;
      mineria = getMiningData(mElapsed);
      snprintf(hashStr, sizeof(hashStr), "%s", mineria.currentHashRate.c_str());
      c4Hora(horaStr);
      c4Cab(hashStr, horaStr);
    }
    webAtender();
    delay(8);
  }

  pongLeds(false);
  tft.setTextPadding(0);
  tft.setTextDatum(TL_DATUM);
  previousTouchMillis = millis(); // que DoLedStuff no se coma un toque justo entre tandas

  cuentita++;
  if (cuentita == 15)
  {
    ajustarZonaHoraria();
    obtenerLocYTemp();
    obtenerVariacionesBTC(variacion1dia, variacion1semana);
  }
}

// ---------------------------------------------------------------------------------------------------------
//  ACUARIO
// ---------------------------------------------------------------------------------------------------------

#define AQ_TOP 28    // arriba quedan 28 px negros para la hora y el hashrate
#define AQ_BOT 170   // el agua llega hasta la barra de precios
#define AQ_SANDY 153 // altura media de la arena
#define AQ_CRABY 162 // por donde pasea el cangrejo
#define AQ_YMIN 48   // los peces nadan entre estas dos alturas
#define AQ_YMAX 126
#define AQ_NFISH 7  // peces ( baja a 5 si quieres aun mas fluidez )
#define AQ_NBUB 9   // burbujas
#define AQ_NPLANT 6 // algas
#define AQ_SLICE_MS 5000
#define AQ_FRAME_MS 33  // ms entre fotogramas de peces y burbujas
#define AQ_PLANT_MS 110 // ms entre fotogramas del vaiven de las algas

#define AQ_RGB(r, g, b) ((uint16_t)((((r) & 0xF8) << 8) | (((g) & 0xFC) << 3) | ((b) >> 3)))

// a,b = semiejes del cuerpo | tl = largo de cola | th = media altura de la cola | fh = alto de aletas
struct AqSp
{
  uint8_t a, b, tl, th, fh;
  uint16_t body, belly, fin, tail;
  uint8_t style;
};
static const AqSp AQ_SP[5] = {
    {13, 8, 7, 5, 4, AQ_RGB(255, 140, 0), AQ_RGB(255, 180, 90), AQ_RGB(230, 100, 0), AQ_RGB(255, 140, 0), 0},  // payaso
    {15, 10, 8, 7, 6, AQ_RGB(0, 90, 255), AQ_RGB(0, 120, 255), AQ_RGB(255, 225, 0), AQ_RGB(255, 225, 0), 1},   // cirujano azul
    {12, 9, 6, 5, 5, AQ_RGB(255, 235, 0), AQ_RGB(255, 250, 190), AQ_RGB(255, 225, 0), AQ_RGB(255, 235, 0), 2}, // mariposa
    {8, 3, 4, 3, 2, AQ_RGB(90, 180, 255), AQ_RGB(190, 190, 190), AQ_RGB(230, 0, 0), AQ_RGB(230, 0, 0), 3},     // neon
    {10, 9, 6, 5, 4, AQ_RGB(255, 210, 0), AQ_RGB(255, 255, 255), AQ_RGB(255, 165, 0), AQ_RGB(255, 165, 0), 4}  // globo
};

struct AqFish
{
  float x, y, vx, ty, ph;
  int8_t dir, wag;
  uint8_t sp;
};
struct AqBub
{
  float x, y, sp, ph;
  int16_t cx, cy;
  uint8_t r;
  bool on;
  uint32_t wait;
};
struct AqCrab
{
  float x;
  int8_t dir;
  uint32_t pausa;
};
struct AqPlant
{
  int16_t x;
  uint8_t h;
  uint16_t c1, c2;
};
struct AqRect
{
  int16_t l, t, r, b;
};

static const AqPlant AQ_PL[AQ_NPLANT] = {
    {16, 56, AQ_RGB(0, 120, 40), AQ_RGB(40, 190, 70)},
    {58, 40, AQ_RGB(20, 140, 60), AQ_RGB(90, 210, 90)},
    {122, 62, AQ_RGB(0, 105, 55), AQ_RGB(30, 170, 100)},
    {196, 46, AQ_RGB(30, 150, 30), AQ_RGB(120, 215, 60)},
    {252, 58, AQ_RGB(0, 120, 40), AQ_RGB(40, 190, 70)},
    {298, 42, AQ_RGB(20, 140, 60), AQ_RGB(90, 210, 90)}};

struct AqState
{
  bool init;
  float ph; // fase del vaiven de las algas
  unsigned long lastPlant;
  AqFish fish[AQ_NFISH];
  AqBub bub[AQ_NBUB];
  AqCrab crab;
};
static AqState aq = {false};

static inline uint16_t aqC(int r, int g, int b)
{
  if (r < 0)
    r = 0;
  else if (r > 255)
    r = 255;
  if (g < 0)
    g = 0;
  else if (g > 255)
    g = 255;
  if (b < 0)
    b = 0;
  else if (b > 255)
    b = 255;
  return AQ_RGB(r, g, b);
}

// ---- Un pixel de un pez, calculado con matematicas ( nada de sprites ): ojo, cuerpo, aletas y cola ----
static bool aqFishPix(const AqFish &f, int fx, int fy, int px, int py, uint16_t &col)
{
  const AqSp &s = AQ_SP[f.sp];
  int dx = px - fx;
  int u = f.dir > 0 ? dx : -dx; // u > 0 hacia la cabeza
  int v = py - fy;
  int a = s.a, b = s.b;

  // Ojo
  int ex = (a * 5) / 9, ey = -(b / 4);
  int d2 = (u - ex) * (u - ex) + (v - ey) * (v - ey);
  if (a >= 10)
  {
    if (d2 <= 1)
    {
      col = 0x0000;
      return true;
    }
    if (d2 <= 5)
    {
      col = 0xFFFF;
      return true;
    }
  }
  else if (d2 <= 1)
  {
    col = 0x0000;
    return true;
  }

  // Cuerpo ( elipse )
  if (u * u * b * b + v * v * a * a <= a * a * b * b)
  {
    uint16_t c = s.body;
    switch (s.style)
    {
    case 0: // payaso: dos bandas blancas con borde negro
    {
      int d = abs(u - a / 4);
      int d3 = abs(u + a / 2);
      if (d3 < d)
        d = d3;
      if (d <= 1)
        c = 0xFFFF;
      else if (d == 2)
        c = 0x0000;
      else if (v > b / 2)
        c = s.belly;
      break;
    }
    case 1: // cirujano azul: franja oscura en el lomo
      if (v >= -(b / 2) - 1 && v <= -(b / 2) + 1 && u > -(a / 2) && u < a / 2 + 3)
        c = AQ_RGB(0, 10, 90);
      else if (v > b / 2)
        c = s.belly;
      break;
    case 2: // mariposa: barra negra que cruza el ojo
      if (abs(u - ex) <= 1 && v > -b + 1)
        c = 0x0000;
      else if (v > b / 2)
        c = s.belly;
      break;
    case 3: // neon: franja celeste arriba, roja abajo
      if (v >= -1 && v <= 0 && u > -a + 2 && u < a - 1)
        c = AQ_RGB(0, 255, 255);
      else if (v >= 1)
        c = AQ_RGB(230, 0, 0);
      break;
    default: // globo: barriga blanca y manchitas
      if (v > b / 3)
        c = s.belly;
      else if ((((u + 24) * 17 + (v + 24) * 29) % 13) == 0)
        c = AQ_RGB(200, 90, 0);
      break;
    }
    col = c;
    return true;
  }

  // Aleta dorsal ( triangulo apoyado en el lomo )
  int hw = a / 2, uc = -(a / 8);
  int du = abs(u - uc);
  if (du <= hw)
  {
    int base = (6 * b) / 10;
    int top = base + (((4 * b) / 10 + s.fh) * (hw - du)) / hw;
    int mv = -v;
    if (mv >= base && mv <= top)
    {
      col = s.fin;
      return true;
    }
  }
  // Aleta de la barriga ( mas pequeña )
  int hw2 = a / 3, uc2 = -(a / 4);
  du = abs(u - uc2);
  if (hw2 > 0 && du <= hw2)
  {
    int base = (6 * b) / 10;
    int top = base + (((4 * b) / 10 + s.fh / 2) * (hw2 - du)) / hw2;
    if (v >= base && v <= top)
    {
      col = s.fin;
      return true;
    }
  }

  // Cola con vaiven y muesca en la punta
  int n = (-a + 2) - u;
  int L = s.tl + 2;
  if (n >= 0 && n <= L)
  {
    int cv = (f.wag * n) / L;
    int hh = 1 + ((s.th - 1) * n) / L;
    int dv = abs(v - cv);
    if (dv <= hh && !(n >= s.tl + 1 && dv <= 1))
    {
      col = s.tail;
      return true;
    }
  }
  return false;
}

// ---- Cangrejo ----
static bool aqCrabPix(int cx, int step, int px, int py, uint16_t &col)
{
  int dx = px - cx, dy = py - AQ_CRABY;
  int ax = abs(dx);
  if (ax == 2 && dy >= -6 && dy <= -5) // ojos
  {
    col = (dy == -6) ? 0xFFFF : AQ_RGB(200, 30, 20);
    return true;
  }
  int sx = dx > 0 ? 8 : -8; // pinzas
  int pdx = dx - sx, pdy = dy + 4;
  if (pdx * pdx + pdy * pdy <= 5)
  {
    col = AQ_RGB(240, 70, 40);
    return true;
  }
  if (dx * dx * 16 + dy * dy * 36 <= 576) // caparazon
  {
    col = (dy < -1) ? AQ_RGB(235, 60, 35) : AQ_RGB(190, 35, 25);
    return true;
  }
  if ((ax == 3 || ax == 5) && dy >= 4 && dy <= 5 + (((ax >> 1) + step) & 1)) // patas
  {
    col = AQ_RGB(170, 30, 20);
    return true;
  }
  return false;
}

// ---- Una linea de w pixeles ya compuesta: fondo + algas + cangrejo + peces + burbujas ----
static void aqRow(int x0, int y, int w, uint16_t *buf)
{
  const int x1 = x0 + w;

  // Fondo: degradado de agua con rayos de luz que se apagan con la profundidad, y arena abajo
  {
    int t = ((y - AQ_TOP) * 256) / (AQ_SANDY - AQ_TOP);
    if (t > 256)
      t = 256;
    int r = 40 - ((40 * t) >> 8), g = 150 - ((115 * t) >> 8), b = 200 - ((100 * t) >> 8);
    uint16_t cn = aqC(r, g, b);
    int amt = 16 - (y - AQ_TOP) / 7;
    uint16_t cr = (amt > 0) ? aqC(r + amt / 2, g + amt, b + amt) : cn;
    for (int i = 0; i < w; i++)
    {
      int px = x0 + i;
      if (y >= AQ_SANDY - 1)
      {
        int tw = px & 63;
        if (tw > 31)
          tw = 63 - tw;
        if (y >= AQ_SANDY - 1 + (tw >> 3)) // arena
        {
          int k = y - (AQ_SANDY - 1);
          int n = (px * 37 + y * 91 + px * y) & 31;
          int sr = 214 - k * 4, sg = 190 - k * 4, sb = 128 - k * 3;
          if (n == 0)
          {
            sr -= 45;
            sg -= 45;
            sb -= 40;
          }
          else if (n == 1)
          {
            sr += 25;
            sg += 25;
            sb += 25;
          }
          buf[i] = aqC(sr, sg, sb);
        }
        else
          buf[i] = cn;
      }
      else
      {
        int ph = (px + ((y * 3) >> 2)) & 127;
        buf[i] = (ph < 20 || (ph >= 64 && ph < 78)) ? cr : cn;
      }
    }
  }

  // Algas ( dos hojas por mata, se mecen con aq.ph )
  for (int p = 0; p < AQ_NPLANT; p++)
  {
    const AqPlant &pl = AQ_PL[p];
    int hh = (AQ_SANDY + 3) - y;
    if (hh < 0 || hh >= pl.h)
      continue;
    if (x1 <= pl.x - 16 || x0 >= pl.x + 20)
      continue;
    for (int k = 0; k < 2; k++)
    {
      int H = k ? (pl.h * 3) / 4 : pl.h;
      if (hh >= H)
        continue;
      float cx = pl.x + (k ? 8 : 0) + sinf(aq.ph + hh * 0.085f + pl.x * 0.05f + k * 1.7f) * (hh * 0.11f);
      int ci = (int)cx;
      int th = 1 + ((H - hh) * 2) / H;
      int xa = ci - th, xb = ci + th;
      if (xa < x0)
        xa = x0;
      if (xb > x1 - 1)
        xb = x1 - 1;
      for (int xx = xa; xx <= xb; xx++)
        buf[xx - x0] = (xx >= ci) ? pl.c1 : pl.c2;
    }
  }

  // Cangrejo
  {
    int cx = (int)aq.crab.x;
    if (y >= AQ_CRABY - 9 && y <= AQ_CRABY + 8 && x1 > cx - 14 && x0 < cx + 15)
    {
      int step = (cx / 3) & 1;
      int xa = cx - 14 < x0 ? x0 : cx - 14, xb = cx + 14 > x1 - 1 ? x1 - 1 : cx + 14;
      uint16_t c;
      for (int xx = xa; xx <= xb; xx++)
        if (aqCrabPix(cx, step, xx, y, c))
          buf[xx - x0] = c;
    }
  }

  // Peces
  for (int i = 0; i < AQ_NFISH; i++)
  {
    const AqFish &f = aq.fish[i];
    const AqSp &s = AQ_SP[f.sp];
    int W = s.a + s.tl + 3, H = s.b + s.fh + 2;
    int fx = (int)f.x, fy = (int)f.y;
    if (y < fy - H || y > fy + H || x1 <= fx - W || x0 > fx + W)
      continue;
    int xa = fx - W < x0 ? x0 : fx - W, xb = fx + W > x1 - 1 ? x1 - 1 : fx + W;
    uint16_t c;
    for (int xx = xa; xx <= xb; xx++)
      if (aqFishPix(f, fx, fy, xx, y, c))
        buf[xx - x0] = c;
  }

  // Burbujas ( por encima de todo )
  for (int i = 0; i < AQ_NBUB; i++)
  {
    const AqBub &q = aq.bub[i];
    if (!q.on)
      continue;
    int r = q.r;
    if (y < q.cy - r - 1 || y > q.cy + r + 1 || x1 <= q.cx - r - 1 || x0 > q.cx + r + 1)
      continue;
    int xa = q.cx - r - 1 < x0 ? x0 : q.cx - r - 1, xb = q.cx + r + 1 > x1 - 1 ? x1 - 1 : q.cx + r + 1;
    for (int xx = xa; xx <= xb; xx++)
    {
      int dx = xx - q.cx, dy = y - q.cy, dd = dx * dx + dy * dy;
      if (r == 1)
      {
        if (dd <= 1)
          buf[xx - x0] = 0xFFFF;
      }
      else if (dx == -1 && dy == -1)
        buf[xx - x0] = 0xFFFF;
      else if (dd <= r * r + 1 && dd >= (r - 1) * (r - 1))
        buf[xx - x0] = AQ_RGB(190, 240, 255);
    }
  }
}

// Repinta un rectangulo [x0,x1) x [y0,y1) linea a linea. Cada pixel se escribe UNA vez con su color final,
// asi que no hay parpadeo y no hace falta ningun sprite: solo un buffer de 128 bytes en la pila.
static void aqRect(int x0, int y0, int x1, int y1)
{
  if (x0 < 0)
    x0 = 0;
  if (x1 > 320)
    x1 = 320;
  if (y0 < AQ_TOP)
    y0 = AQ_TOP;
  if (y1 > AQ_BOT)
    y1 = AQ_BOT;
  if (x0 >= x1 || y0 >= y1)
    return;
  uint16_t buf[64];
  tft.startWrite();
  for (int y = y0; y < y1; y++)
    for (int x = x0; x < x1; x += 64)
    {
      int w = x1 - x;
      if (w > 64)
        w = 64;
      aqRow(x, y, w, buf);
      tft.setAddrWindow(x, y, w, 1);
      tft.pushColors(buf, w, true);
    }
  tft.endWrite();
}

static inline void aqAdd(AqRect &d, int l, int t, int r, int b)
{
  if (d.l >= d.r)
  {
    d.l = l;
    d.t = t;
    d.r = r;
    d.b = b;
    return;
  }
  if (l < d.l)
    d.l = l;
  if (t < d.t)
    d.t = t;
  if (r > d.r)
    d.r = r;
  if (b > d.b)
    d.b = b;
}

static void aqFishBox(const AqFish &f, int &l, int &t, int &r, int &b)
{
  const AqSp &s = AQ_SP[f.sp];
  int W = s.a + s.tl + 3, H = s.b + s.fh + 2;
  l = (int)f.x - W;
  r = (int)f.x + W + 1;
  t = (int)f.y - H;
  b = (int)f.y + H + 1;
}

static void aqFishNew(AqFish &f, uint8_t sp, bool inicial)
{
  const AqSp &s = AQ_SP[sp];
  int W = s.a + s.tl + 3;
  f.sp = sp;
  f.dir = (esp_random() & 1) ? 1 : -1;
  f.vx = (sp == 3 ? 0.9f : 0.35f) + (esp_random() % 100) * (sp == 3 ? 0.007f : 0.008f);
  f.y = AQ_YMIN + esp_random() % (AQ_YMAX - AQ_YMIN);
  f.ty = f.y;
  f.ph = (esp_random() % 628) / 100.0f;
  f.wag = 0;
  if (inicial)
    f.x = esp_random() % 320;
  else
    f.x = f.dir > 0 ? -W - 2 : 320 + W + 2;
}

static void aqInit()
{
  for (int i = 0; i < AQ_NFISH; i++)
    aqFishNew(aq.fish[i], i < 5 ? i : (i == 5 ? 3 : 0), true);
  for (int i = 0; i < AQ_NBUB; i++)
  {
    aq.bub[i].on = false;
    aq.bub[i].wait = millis() + esp_random() % 4000;
  }
  aq.crab.x = 60 + esp_random() % 200;
  aq.crab.dir = (esp_random() & 1) ? 1 : -1;
  aq.crab.pausa = 0;
  aq.ph = 0;
  aq.lastPlant = 0;
  aq.init = true;
}

// Un fotograma: mueve todo y repinta solo lo que ha cambiado ( caja vieja + caja nueva de cada cosa )
static void aqFrame(unsigned long now)
{
  AqRect d[AQ_NFISH + AQ_NBUB + 1 + AQ_NPLANT];
  int nd = 0;

  // Peces
  for (int i = 0; i < AQ_NFISH; i++)
  {
    AqFish &f = aq.fish[i];
    AqRect r = {0, 0, 0, 0};
    int l, t, rr, b;
    aqFishBox(f, l, t, rr, b);
    aqAdd(r, l, t, rr, b);
    f.x += f.dir * f.vx;
    float dy = f.ty - f.y;
    f.y += constrain(dy * 0.04f, -0.3f, 0.3f);
    if (fabsf(dy) < 1.5f && (esp_random() % 400) == 0)
      f.ty = AQ_YMIN + esp_random() % (AQ_YMAX - AQ_YMIN);
    f.ph += 0.22f + f.vx * 0.3f;
    f.wag = (int)(sinf(f.ph) * 3.4f);
    const AqSp &s = AQ_SP[f.sp];
    int W = s.a + s.tl + 3;
    if ((f.dir > 0 && f.x > 320 + W + 6) || (f.dir < 0 && f.x < -W - 6))
      aqFishNew(f, f.sp, false);
    aqFishBox(f, l, t, rr, b);
    aqAdd(r, l, t, rr, b);
    d[nd++] = r;
  }

  // Burbujas
  for (int i = 0; i < AQ_NBUB; i++)
  {
    AqBub &q = aq.bub[i];
    AqRect r = {0, 0, 0, 0};
    if (q.on)
      aqAdd(r, q.cx - q.r - 1, q.cy - q.r - 1, q.cx + q.r + 2, q.cy + q.r + 2);
    if (!q.on)
    {
      if (now >= q.wait)
      {
        q.on = true;
        q.x = 8 + esp_random() % 304;
        q.y = AQ_SANDY - 3;
        q.r = 1 + esp_random() % 3;
        q.sp = 0.5f + (esp_random() % 100) * 0.009f;
        q.ph = (esp_random() % 628) / 100.0f;
        q.cx = (int)q.x;
        q.cy = (int)q.y;
      }
    }
    else
    {
      q.y -= q.sp;
      q.ph += 0.13f;
      q.cx = (int)(q.x + sinf(q.ph) * 3.0f);
      q.cy = (int)q.y;
      if (q.cy < AQ_TOP + q.r + 2)
      {
        q.on = false;
        q.wait = now + esp_random() % 3000;
      }
    }
    if (q.on)
      aqAdd(r, q.cx - q.r - 1, q.cy - q.r - 1, q.cx + q.r + 2, q.cy + q.r + 2);
    d[nd++] = r;
  }

  // Cangrejo
  {
    AqCrab &c = aq.crab;
    AqRect r = {0, 0, 0, 0};
    aqAdd(r, (int)c.x - 15, AQ_CRABY - 10, (int)c.x + 16, AQ_CRABY + 9);
    if (now >= c.pausa)
    {
      c.x += c.dir * 0.45f;
      if (c.x < 18)
      {
        c.x = 18;
        c.dir = 1;
      }
      else if (c.x > 302)
      {
        c.x = 302;
        c.dir = -1;
      }
      if ((esp_random() % 500) == 0)
        c.pausa = now + 800 + esp_random() % 2500;
      if ((esp_random() % 900) == 0)
        c.dir = -c.dir;
    }
    aqAdd(r, (int)c.x - 15, AQ_CRABY - 10, (int)c.x + 16, AQ_CRABY + 9);
    d[nd++] = r;
  }

  // Algas
  if (now - aq.lastPlant >= AQ_PLANT_MS)
  {
    aq.lastPlant = now;
    aq.ph += 0.10f;
    for (int p = 0; p < AQ_NPLANT; p++)
    {
      AqRect r = {0, 0, 0, 0};
      aqAdd(r, AQ_PL[p].x - 16, (AQ_SANDY + 3) - AQ_PL[p].h - 1, AQ_PL[p].x + 20, AQ_SANDY + 4);
      d[nd++] = r;
    }
  }

  // Ya esta todo movido: ahora se pinta
  for (int i = 0; i < nd; i++)
    if (d[i].l < d[i].r)
      aqRect(d[i].l, d[i].t, d[i].r, d[i].b);
}

#define AQ_LED_MS 4000 // duracion de una respiracion completa (sube y baja), en ms
#define AQ_LED_MAX 255 // brillo maximo del rojo (0-255); baja el numero si te molesta de noche

// Respiracion del LED rojo: sube y baja suave. El cuadrado del seno compensa que el ojo ve mal los tonos bajos
static uint8_t aqBrillo(unsigned long now)
{
  float t = (float)(now % AQ_LED_MS) / (float)AQ_LED_MS;
  float s = 0.5f - 0.5f * cosf(t * 6.2831853f);
  return (uint8_t)(s * s * AQ_LED_MAX);
}

void esp32_2432S028R_AcuarioScreen(unsigned long mElapsed)
{
  mineria = getMiningData(mElapsed);
  Serial.printf("M8AX - %s >>> Completados %s Share(s), %s Khashes, Prom. Hashrate %s KH/s %s°\n",
                mineria.currentTime.c_str(), mineria.completedShares.c_str(), mineria.totalKHashes.c_str(), mineria.currentHashRate.c_str(), mineria.temp.c_str());

  bool redraw = hasChangedScreen;
  if (!aq.init)
  {
    aqInit();
    redraw = true;
  }
  if (redraw)
  {
    tft.fillRect(0, 0, 320, 170, TFT_BLACK);
    aqRect(0, AQ_TOP, 320, AQ_BOT);
  }
  printPoolData(); // barra de precios de abajo (solo se repinta cuando toca)
  hasChangedScreen = false;

  char horaStr[9] = "";
  char lastHora[9] = "";
  char hashStr[16];
  char lastHash[16] = "";
  snprintf(hashStr, sizeof(hashStr), "%s", mineria.currentHashRate.c_str());

  unsigned long start = millis();
  unsigned long lastMiningUpdate = 0;
  unsigned long lastTouchChk = 0;
  unsigned long lastFrame = 0;
  const int aqIdx = currentDisplayDriver->current_cyclic_screen;
  unsigned long lastLed = 0;
  ledSet(LED_PIN_G, 2, 0);
  ledSet(LED_PIN_B, 3, 0);

  while (millis() - start < AQ_SLICE_MS)
  {
    unsigned long now = millis();

    // ---- Tactil (igual que Pong / Life / Lluvia) ----
    if (now - lastTouchChk >= 40 && now - previousTouchMillis >= 500)
    {
      lastTouchChk = now;
      int16_t t_x, t_y;
      if (toqueXY(t_x, t_y))
      {
        previousTouchMillis = now;
        if (((t_x > 109) && (t_x < 211)) && ((t_y > 185) && (t_y < 241)))
        {
          bottomScreenBlue ^= true;
          hasChangedScreen = true;
        }
        else if ((t_x > 235) && ((t_y > 0) && (t_y < 16)))
        {
          esp32_2432S028R_AlternateScreenState();
        }
        else if (t_x > 160)
        {
          currentDisplayDriver->current_cyclic_screen = (currentDisplayDriver->current_cyclic_screen + 1) % currentDisplayDriver->num_cyclic_screens;
        }
        else
        {
          currentDisplayDriver->current_cyclic_screen = currentDisplayDriver->current_cyclic_screen - 1;
          if (currentDisplayDriver->current_cyclic_screen < 0)
            currentDisplayDriver->current_cyclic_screen = currentDisplayDriver->num_cyclic_screens - 1;
        }
        break;
      }
    }

    // ---- Peces, burbujas, cangrejo y algas ----
    if (now - lastFrame >= AQ_FRAME_MS)
    {
      lastFrame = now;
      aqFrame(now);
    }

    // ---- LED rojo respirando ----
    if (now - lastLed >= 20)
    {
      lastLed = now;
      ledSet(LED_PIN, 1, aqBrillo(now));
    }

    // ---- Hora (izquierda) y hashrate (derecha), arriba como en las demas ----
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
        pongDrawHud(hashStr, horaStr);
        strcpy(lastHora, horaStr);
        strcpy(lastHash, hashStr);
      }
    }
    webAtender();
    delay(2);
  }

  if (currentDisplayDriver->current_cyclic_screen != aqIdx)
    ledsLibres(); // si nos vamos a otra pantalla, se sueltan y apagan los LEDs; si seguimos aqui, sigue respirando
  tft.setTextPadding(0);
  tft.setTextDatum(TL_DATUM);

  cuentita++;
  if (cuentita == 15)
  {
    ajustarZonaHoraria();
    obtenerLocYTemp();
    obtenerVariacionesBTC(variacion1dia, variacion1semana);
  }
}

void analiCadaSegundo(unsigned long frame)
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

  extern CyclicScreenFunction esp32_2432S028RCyclicScreens[];
  CyclicScreenFunction pantallaActual = esp32_2432S028RCyclicScreens[currentDisplayDriver->current_cyclic_screen];

  if (nbd && pantallaActual != esp32_2432S028R_GalaxiasScreen)
  {
    free(nbd);
    nbd = NULL;
  }

  if (nemM && pantallaActual != esp32_2432S028R_NemesisScreen)
  {
    free(nemM);
    nemM = NULL;
  }

  if (galM && pantallaActual != esp32_2432S028R_GalaxianScreen)
  {
    free(galM);
    galM = NULL;
  }

  if (astM && pantallaActual != esp32_2432S028R_AsteroidesScreen)
  {
    free(astM);
    astM = NULL;
  }

  if (atrM && pantallaActual != esp32_2432S028R_AtractoresScreen)
  {
    free(atrM);
    atrM = NULL;
  }

  if (demM && pantallaActual != esp32_2432S028R_DemoScreen)
  {
    free(demM);
    demM = NULL;
  }

  if (f3M && pantallaActual != esp32_2432S028R_Fractal3DScreen)
  {
    free(f3M);
    f3M = NULL;
  }

  if (snkM && pantallaActual != esp32_2432S028R_SnakeScreen)
  {
    free(snkM);
    snkM = NULL;
  }

  if (banM && pantallaActual != esp32_2432S028R_BannerScreen)
  {
    free(banM);
    banM = NULL;
  }

  if (dmM && pantallaActual != esp32_2432S028R_DoomScreen)
  {
    free(dmM);
    dmM = NULL;
  }

  if (fhM && pantallaActual != esp32_2432S028R_FlappyHashScreen)
  {
    free(fhM);
    fhM = NULL;
  }

  if (chMem && pantallaActual != esp32_2432S028R_AjedrezScreen)
    chMemLibera();

  ecgMuestrea();

  wxHistMuestrea();

  // Felicitar La Navidad O El Año Nuevo ( 20 dic - 6 ene ), tambien nada mas arrancar
  {
    static long navUltima = -1;
    static bool navArranque = false;
    bool horaValida = (epochTime > 1700000000UL);
    bool epocaNavidad = horaValida && ((mes == 12 && dia >= 20) || (mes == 1 && dia <= 6));
    bool toca = false;
    long clave = ((long)mes * 32 + dia) * 24 + horita;

    if (horaValida && !navArranque)
    {
      navArranque = true;
      if (epocaNavidad)
        toca = true;
    }
    else if (epocaNavidad && minutitos == 30 && (horita % 2 == 0) &&
             ((horita >= 8 && horita <= 15) || (horita >= 19 && horita <= 23) || (horita <= 2)) &&
             dia % 2 == 0 && clave != navUltima)
    {
      toca = true;
    }

    if (toca)
    {
      navUltima = clave;
      if (mes == 12)
      {
        Serial.println("M8AX - Felicitando La Navidad...");
        nevarFelicita("FELIZ NAVIDAD", 100, 35);
      }
      else
      {
        Serial.println("M8AX - Felicitando El Año Nuevo...");
        nevarFelicita("HAPPY NEW YEAR", 200, 25);
      }
    }
  }

  // Dando Esperanzas Al Dueño Del NerdminerV2: Lo Conseguirás... Ostia!!! xD
  rndnumero = esp_random();
  if (rndnumero <= 10031977 && segundos <= 20 && segundos % 2 == 0 && dia % 2 == 0)
  {
    Serial.printf("M8AX - >>> NerdMinerV2 Dando Ánimos Y Esperanza Al Usuario...\n");
    mensajeAnimo(rndnumero);
    hasChangedScreen = true; // repinta la pantalla actual y la barra de abajo
  }

  // Bloque minado: aviso parpadeante cada 5 s ( ES / EN ) hasta que se toque la pantalla
  if (bloqueAvisoActivo && millis() - bloqueAvisoUltimo >= BLOQUE_AVISO_CICLO_MS)
  {
    bloqueAvisoUltimo = millis();
    bool tocado = mensajeBloque(bloqueAvisoIngles);
    bloqueAvisoIngles = !bloqueAvisoIngles;
    hasChangedScreen = true; // repinta la pantalla actual y la barra de abajo
    if (tocado)
      bloqueAvisoActivo = false;
  }

  if (startTime == 0)
  {
    BOT_TOKEN = Settings.botTelegram;    // Bot De Telegram
    CHAT_ID = Settings.ChanelIDTelegram; // ID Del Canal De Telegram
    zonilla = Settings.Timezone;
    startTime = (epochTime > 1700000000UL && epochTime < 4102444800UL) ? epochTime : 0; // Guardar el tiempo de inicio cuando el dispositivo arranca
  }

  if (startTime > 0 && uncontadormas > 50)
  {
    float currentHashRate = mineria.currentHashRate.toFloat();
    int currentTemp = mineria.temp.toInt();

    if (currentHashRate > maxkh)
    {
      maxkh = currentHashRate; // Actualiza el máximo de kh/s
    }

    if (currentHashRate < minkh)
    {
      minkh = currentHashRate; // Actualiza el mínimo de kh/s
    }

    if (currentTemp > maxtemp)
    {
      maxtemp = currentTemp; // Actualiza el máximo de temperatura
    }

    if (currentTemp < mintemp)
    {
      mintemp = currentTemp; // Actualiza el mínimo de temperatura
    }
  }

  // Si ya ha pasado el tiempo de arranque mínimo (por ejemplo, 10 minutos) y han pasado 2 horas desde el último mensaje de Telegram

  if (startTime > 0 && epochTime - startTime >= minStartupTime && epochTime - lastTelegramEpochTime >= interval)
  {
    // Ajustar la zona horaria si es necesario
    if ((mes == 3 || mes == 4 || mes == 9 || mes == 10 || mes == 11) && horita >= 0 && horita <= 5)
    {
      ajustarZonaHoraria();
      epochTime = timeClient.getEpochTime();
    }
    obtenerLocYTemp();
    obtenerVariacionesBTC(variacion1dia, variacion1semana);
    // Verificar si los datos de Telegram están configurados
    static int fallosSeguidos = 0;
    // libera la RAM de la pantalla actual antes de la conexión segura
    if (nbd)
    {
      free(nbd);
      nbd = NULL;
    }
    if (nemM)
    {
      free(nemM);
      nemM = NULL;
    }
    if (galM)
    {
      free(galM);
      galM = NULL;
    }
    if (astM)
    {
      free(astM);
      astM = NULL;
    }
    if (atrM)
    {
      free(atrM);
      atrM = NULL;
    }
    if (demM)
    {
      free(demM);
      demM = NULL;
    }
    if (f3M)
    {
      free(f3M);
      f3M = NULL;
    }
    if (snkM)
    {
      free(snkM);
      snkM = NULL;
    }
    if (banM)
    {
      free(banM);
      banM = NULL;
    }
    if (dmM)
    {
      free(dmM);
      dmM = NULL;
    }
    if (fhM)
    {
      free(fhM);
      fhM = NULL;
    }
    if (chMem)
      chMemLibera();
    bool enviado = recopilaTelegram(); // Monta el mensaje, lo envía a Telegram si hay bot y lo guarda en la SD
    if (enviado)
    {
      fallosSeguidos = 0;
      sdGuardadoCiclo = false;
      lastTelegramEpochTime = epochTime; // Actualiza el tiempo de la última ejecución
    }
    else if (++fallosSeguidos < 4)
    {
      lastTelegramEpochTime = epochTime - interval + ((interval > 300UL) ? 300UL : interval);
    }
    else
    {
      fallosSeguidos = 0;
      sdGuardadoCiclo = false;
      lastTelegramEpochTime = epochTime; // 4 fallos seguidos: espera las 2 horas normales
    }
  }

  if (sdColaN && !sdSinTarjeta && millis() - sdColaUltimo >= 60000UL) // tarjeta puesta: cada minuto. Ranura vacía: se prueba una vez en cada ciclo de Telegram
  {
    sdColaUltimo = millis();
    sdVuelcaCola();
  }
}

void esp32_2432S028R_AnimateCurrentScreen(unsigned long frame)
{
  webAtender();
  uncontadormas++;
  if (uncontadormas % 5 == 0)
  {
    analiCadaSegundo(frame);
    if (uncontadormas > 1000000)
      uncontadormas = 50;
  }
}
// Variables para controlar el parpadeo con millis()
unsigned long previousMillis = 0;
unsigned long previousTouchMillis = 0;
char currentScreen = 0;

void esp32_2432S028R_DoLedStuff(unsigned long frame)
{
  unsigned long currentMillis = millis();

  // Check the touch coordinates 110x185 210x240

  if (currentMillis - previousTouchMillis >= 500)
  {
    int16_t t_x, t_y; // To store the touch coordinates
    bool pressed = toqueXY(t_x, t_y);
    if (pressed)
    {
      if (((t_x > 109) && (t_x < 211)) && ((t_y > 185) && (t_y < 241)))
      {
        bottomScreenBlue ^= true;
        hasChangedScreen = true;
      }
      else if ((t_x > 235) && ((t_y > 0) && (t_y < 16)))
      {

        // Touching the top right corner of the screen, roughly in the gray status label.
        // Disabling the screen backlight.

        esp32_2432S028R_AlternateScreenState();
      }
      else if (t_x > 160)
      {

        // next screen
        // Serial.printf("Next Screen Touch( X:%d Y:%d )\n", t_x, t_y);

        currentDisplayDriver->current_cyclic_screen = (currentDisplayDriver->current_cyclic_screen + 1) % currentDisplayDriver->num_cyclic_screens;
      }
      else if (t_x < 160)
      {

        // Previus screen
        // Serial.printf("Previus Screen Touch( X:%d Y:%d )\n", t_x, t_y);
        /* Serial.println(currentDisplayDriver->current_cyclic_screen); */

        currentDisplayDriver->current_cyclic_screen = currentDisplayDriver->current_cyclic_screen - 1;
        if (currentDisplayDriver->current_cyclic_screen < 0)
          currentDisplayDriver->current_cyclic_screen = currentDisplayDriver->num_cyclic_screens - 1;
      }
    }
    previousTouchMillis = currentMillis;
  }
  if (currentScreen != currentDisplayDriver->current_cyclic_screen)
  {
    hasChangedScreen = true, ledsLibres();
    ledUltimoManejo = 0;
  }
  currentScreen = currentDisplayDriver->current_cyclic_screen;
  bool minando = (mMonitor.NerdStatus == NM_hashing) || (ledUltimoManejo != 0 && ledHashRate > 0);
  if (minando)
  {
    actualizaLeds();
  }
  else if (mMonitor.NerdStatus == NM_waitingConfig)
  {
    digitalWrite(LED_PIN, LOW);
  }
  else if (mMonitor.NerdStatus == NM_Connecting)
  {
    if (currentMillis - previousMillis >= 500)
    {
      previousMillis = currentMillis;
      digitalWrite(LED_PIN, HIGH);                      // rojo apagado
      digitalWrite(LED_PIN_B, !digitalRead(LED_PIN_B)); // azul parpadea
    }
  }
}

CyclicScreenFunction esp32_2432S028RCyclicScreens[] = {esp32_2432S028R_MinerScreen, esp32_2432S028R_BTCprice, esp32_2432S028R_ClockScreen, esp32_2432S028R_WeatherScreen, esp32_2432S028R_ECGScreen, tDisplay_granHash, tDisplay_hashGigante, RelojDeNumeros, tDisplay_m8axScreen7, esp32_2432S028R_m8axScreen1, esp32_2432S028R_m8axScreen2, tDisplay_m8axScreenMegaNerd, tDisplay_m8axScreen5, tDisplay_m8axScreen6, esp32_2432S028R_BannerScreen, tDisplay_Primos, esp32_2432S028R_CifrasScreen, esp32_2432S028R_HanoiScreen, esp32_2432S028R_TetrisScreen, esp32_2432S028R_AjedrezScreen, esp32_2432S028R_GalaxianScreen, esp32_2432S028R_NemesisScreen, esp32_2432S028R_AsteroidesScreen, esp32_2432S028R_PongScreen, esp32_2432S028R_KrakoutScreen, esp32_2432S028R_SnakeScreen, esp32_2432S028R_FlappyHashScreen, esp32_2432S028R_DoomScreen, esp32_2432S028R_LifeScreen, esp32_2432S028R_FractalScreen, esp32_2432S028R_Fractal3DScreen, esp32_2432S028R_AtractoresScreen, esp32_2432S028R_GalaxiasScreen, esp32_2432S028R_RainScreen, esp32_2432S028R_AcuarioScreen, esp32_2432S028R_DemoScreen, esp32_2432S028R_monedaYdado, esp32_2432S028R_CalcScreen, esp32_2432S028R_Conecta4Screen, tDisplay_m8axScreen3, tDisplay_m8axScreen4};

DisplayDriver esp32_2432S028RDriver = {
    esp32_2432S028R_Init,
    esp32_2432S028R_AlternateScreenState,
    esp32_2432S028R_AlternateRotation,
    esp32_2432S028R_LoadingScreen,
    esp32_2432S028R_SetupScreen,
    esp32_2432S028RCyclicScreens,
    esp32_2432S028R_AnimateCurrentScreen,
    esp32_2432S028R_DoLedStuff,
    SCREENS_ARRAY_SIZE(esp32_2432S028RCyclicScreens),
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