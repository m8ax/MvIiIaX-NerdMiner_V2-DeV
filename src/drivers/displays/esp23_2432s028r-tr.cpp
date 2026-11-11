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
 *   1. **Cuando el hash rate es mayor de 300 kH/s**:
 *      - Los LEDs **rojo**, **verde** y **azul** parpadean en alternancia.
 *
 *   2. **Cuando el hash rate es mayor de 0 kH/s pero menor o igual a 300 kH/s**:
 *      - El LED **azul** está encendido de forma constante.
 *      - Los LEDs **rojo** y **verde** parpadean a la vez.
 *
 *   3. **Cuando el hash rate es 0**:
 *      - El LED **azul** está encendido de forma constante.
 *      - El LED **rojo** parpadea.
 *
 *
 *           Un minero de Bitcoin es un dispositivo o software que realiza cálculos
 *           matemáticos complejos para verificar y validar transacciones en la red.
 *           Los mineros compiten para resolver estos problemas y añadir un bloque
 *           a la cadena. A cambio, reciben bitcoins recién creados como recompensa.
 *
 *
 *                     Tmp. De Programación 4H - 6065 Líneas De Código
 *                     -----------------------------------------------
 *
 ****************************************************************************************************************/

// Invocando las poderosas librerías que hacen posible esta obra maestra del minado nerd

#include "displayDriver.h"
#if defined ESP32_2432S028R || ESP32_2432S028_2USB
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
#include <qrcode.h>
#include "rotation.h"
#include <clientntp.h>
#include <WiFiClient.h>
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
    Serial.println("M8AX - Conexión HTTPS fallida");
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

void manejandoLeds(float currentHashRate)
{
  // Si el hash rate es mayor que 300
  if (currentHashRate > 300)
  {
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));     // Toggle el LED
    digitalWrite(LED_PIN_B, !digitalRead(LED_PIN_B)); // Toggle otro LED
    digitalWrite(LED_PIN_G, !digitalRead(LED_PIN_G)); // Toggle otro LED
  }
  // Si el hash rate es mayor que 0 y menor o igual a 300
  else if (currentHashRate > 0 && currentHashRate <= 300)
  {
    digitalWrite(LED_PIN, LOW);
    digitalWrite(LED_PIN_G, LOW);
    vTaskDelay(pdMS_TO_TICKS((cuentita % 2 == 0) ? 25 : 50)); // Delay según si cuentita es par o impar
    digitalWrite(LED_PIN, HIGH);                              // Enciende el LED
    digitalWrite(LED_PIN_G, HIGH);                            // Enciende el otro LED
  }
  // Si el hash rate es 0 o menor
  else
  {
    digitalWrite(LED_PIN, HIGH);   // Enciende el LED
    digitalWrite(LED_PIN_G, HIGH); // Enciende el otro LED
    digitalWrite(LED_PIN_B, HIGH); // Enciende otro LED
    digitalWrite(LED_PIN, LOW);    // Apaga el LED
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
    return "- Número Fuera De Rango -";

  String unidades[] = {"cero", "uno", "dos", "tres", "cuatro", "cinco", "seis", "siete", "ocho", "nueve"};
  String especiales[] = {"Diez", "Once", "Doce", "Trece", "Catorce", "Quince",
                         "Dieciséis", "Diecisiete", "Dieciocho", "Diecinueve"};
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
    resultado += capitalizar(centenas[num / 100]);
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
      // Para números del 20 al 29, usamos la forma correcta "Veinti" + unidades

      if (num == 20)
      {
        resultado += "Veinte";
      }
      else if (num < 30)
      {
        resultado += "Veinti";
        if (num % 10 > 0)
          resultado += unidades[num % 10]; // "VeintiUno", "VeintiTres", etc.
      }
      else
      {
        resultado += capitalizar(decenas[num / 10]);
        if (num % 10 > 0)
          resultado += " Y " + capitalizar(unidades[num % 10]); // "Treinta Y Uno", "Cuarenta Y Cinco"
      }
    }
    else if (num > 0)
    {
      resultado += capitalizar(unidades[num]); // Unidades solas
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
  return timeinfo.tm_hour + map((timeinfo.tm_min * 60) + timeinfo.tm_sec, 0, 3600, 0.0, 1.0);
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

std::pair<String, String> obtenerCiudadYTemperatura(const String &ip)
{
  String ciudad = "ERROR", temperatura = "ERROR";
  String latitud = "", longitud = "";
  HTTPClient http;
  String urlGeo = "http://ip-api.com/json/" + ip + "?fields=city,lat,lon";
  http.begin(urlGeo);
  int httpCode = http.GET();
  if (httpCode > 0)
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
    if (httpCode > 0)
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
  tft.fillScreen(TFT_BLACK);      // Fondo negro (puedes cambiarlo)
  const int NUM_COPOS = 500;      // Número de copos de nieve en pantalla
  int x[NUM_COPOS], y[NUM_COPOS]; // Coordenadas de los copos

  // Inicializa copos en posiciones aleatorias
  for (int i = 0; i < NUM_COPOS; i++)
  {
    x[i] = esp_random() % 536;
    y[i] = esp_random() % 240;
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
      if (y[i] > 240)
      { // Si sale de la pantalla, reaparece arriba
        y[i] = 0;
        x[i] = esp_random() % 536;
      }
    }
    int nnumeroAleatorio = esp_random() % 20 + 1;
    delay(nnumeroAleatorio); // Controla la velocidad de la animación
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
  const int NUM_COPOS = 400;      // Número de copos de nieve en pantalla
  int x[NUM_COPOS], y[NUM_COPOS]; // Coordenadas de los copos
  // Inicializa copos en posiciones aleatorias
  for (int i = 0; i < NUM_COPOS; i++)
  {
    x[i] = (esp_random() % 536); // Genera un número entre 0 y 319
    y[i] = (esp_random() % 240); // Genera un número entre 0 y 169
  }

  unsigned long startTime = millis();

  while (millis() - startTime < 5000)
  {                            // 5 segundos
    tft.fillScreen(TFT_BLACK); // Borra pantalla

    for (int i = 0; i < NUM_COPOS; i++)
    {
      tft.drawPixel(x[i], y[i], colors[colorI]); // Dibuja copo de nieve
      y[i] += (esp_random() % 4) + 1;            // Baja la posición del copo

      if (y[i] > 240)
      { // Si sale de la pantalla, reaparece arriba
        y[i] = 0;
        x[i] = esp_random() % 536;
      }
    }

    delay(35); // Controla la velocidad de la animación
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
  const int NUM_COPOS = 600;      // Número de copos de nieve en pantalla
  int x[NUM_COPOS], y[NUM_COPOS]; // Coordenadas de los copos

  // Inicializa copos en posiciones aleatorias
  for (int i = 0; i < NUM_COPOS; i++)
  {
    x[i] = esp_random() % 536;
    y[i] = esp_random() % 240;
  }

  unsigned long startTime = millis();

  while (millis() - startTime < 5000)
  {                            // 5 segundos
    tft.fillScreen(TFT_BLACK); // Borra pantalla

    for (int i = 0; i < NUM_COPOS; i++)
    {
      tft.drawPixel(x[i], y[i], colors[colorI]); // Dibuja copo de nieve
      y[i] += (esp_random() % 5) + 1;            // Baja Posición Del Copo
      if (y[i] > 240)
      { // Si sale de la pantalla, reaparece arriba
        y[i] = 0;
        x[i] = esp_random() % 536;
      }
    }

    delay(25); // Controla la velocidad de la animación
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
    return "Mes inválido";
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

float calcularDiferenciaDias()
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
  float diferenciaDias = (float)(currentEpoch - compiledEpoch) / (60 * 60 * 24);
  return diferenciaDias;
}

void enviarMensajeATelegram(String mensaje)
{
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
      return;
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
    client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: api.telegram.org\r\n" + "Connection: close\r\n\r\n");
    delay(1500); // Espera para dar tiempo a la respuesta
    Serial.println("M8AX - Mensaje Enviado A Telegram...");
    client.flush();
    client.stop();
    sumatele += 1;
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
  const char *diasem[] = {"Dom", "Lun", "Mar", "Mie", "Jue", "Vie", "Sab"};

  int dia = std::stoi(fecha.substr(0, 2));
  int mes = std::stoi(fecha.substr(3, 2)) - 1;
  int anio = std::stoi(fecha.substr(6, 4));

  std::tm timeStruct = {};
  timeStruct.tm_mday = dia;
  timeStruct.tm_mon = mes;
  timeStruct.tm_year = anio - 1900;

  std::mktime(&timeStruct);
  int diaSemana = timeStruct.tm_wday; // 0 = domingo, 1 = lunes, ...

  return diasem[diaSemana];
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

void recopilaTelegram()
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
  quediase.clear();
  quediase.shrink_to_fit();
  if (sumatele <= 3999)
  {
    cadenaEnvio += "Mensaje Número - " + convertirARomanos(sumatele) + " | F.C.FW - " + String(__DATE__) + " A Las " + String(__TIME__) + " | Hace - " + String(convertirTiempo(floatToUint32(calcularDiferenciaDias() * 86400))).c_str() + "\n";
  }
  else
  {
    cadenaEnvio += "Mensaje Número - " + String(sumatele) + " | F.C.FW - " + String(__DATE__) + " A Las " + String(__TIME__) + " | Hace - " + String(convertirTiempo(floatToUint32(calcularDiferenciaDias() * 86400))).c_str() + "\n";
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
  cadenaEnvio += quitarAcentos(Tresultado.first) + ", " + Tresultado.second + " | UTC " + ((zonilla >= 0) ? "+" : "") + String(zonilla) + " | " + LUNAP + "\n";
  cadenaEnvio += "Tiempo Minando - " + (mineria.timeMining.substring(0, mineria.timeMining.indexOf(" ")).length() == 1 ? "0" + mineria.timeMining.substring(0, mineria.timeMining.indexOf(" ")) : mineria.timeMining.substring(0, mineria.timeMining.indexOf(" "))) + " Días" + mineria.timeMining.substring(mineria.timeMining.indexOf(" ") + 1) + "\n";
  cadenaEnvio += "HR Actual - " + mineria.currentHashRate + " KH/s ( MAX - " + String(maxkh) + " | MIN - " + String(minkh) + " )\n";
  cadenaEnvio += "Temp. De CPU - " + mineria.temp + "° ( MAX - " + String(maxtemp) + "° | MIN - " + String(mintemp) + "° ) | " + obtenerEstadoTemperatura(mineria.temp.toInt()) + "\n";
  cadenaEnvio += "RAM Libre - " + String(ESP.getFreeHeap()) + " Bytes\n";
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
  if (mineria.valids.toInt() == 1)
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
  cadenaEnvio += "Factorización De Número - " + String(rndnumero) + " -> " + factorize(rndnumero) + "\n";
  cadenaEnvio += "------------------------------------------------------------------------------------------------\n                                       By M8AX Corp. " + convertirARomanos(anio);
  cadenaEnvio += F("\n------------------------------------------------------------------------------------------------");
  enviarMensajeATelegram(cadenaEnvio);
  cadenaEnvio = "";
  cadenaEnvio.reserve(0);
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
  Serial.printf("M8AX - $$ Free Heap: %u  Max bloque: %u\n", ESP.getFreeHeap(), ESP.getMaxAllocHeap());
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
    Serial.printf("M8AX - Size w:%d h:%d \n", wdt, hgt);
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
    http.setTimeout(4000);
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

      // 2) Sprite solo con la zona de los recuadros, para escribir sin parpadeo
      background.createSprite(320, BARRA_SPR_H);
      if (!background.created())
      {
        Serial.println("M8AX - ###### POOL SPRITE ERROR ######");
        printheap();
      }
      else
      {
        background.setSwapBytes(true);
        if (bottomScreenBlue)
          background.pushImage(0, -(BARRA_SPR_Y - 170), 320, 70, bottonPoolScreen);
        else
          background.pushImage(0, -(BARRA_SPR_Y - 170), 320, 70, bottonPoolScreen_g);

        render.setDrawer(background);
        render.setLineSpaceRatio(1);
        render.setAlignment(Align::TopLeft);
        render.setFontSize(BARRA_PRECIO_TAM);
        background.setFreeFont(NULL);
        background.setTextFont(1);
        background.setTextSize(1);

        for (int col = 0; col < 3; col++)
        {
          int cx = BARRA_CX[col];
          for (int fila = 0; fila < 2; fila++)
          {
            Cripto &c = criptos[paginaCriptos * 6 + col * 2 + fila];
            int y = BARRA_FILA_Y[fila] - BARRA_SPR_Y;
            int xl = cx - BARRA_MEDIO_ANCHO; // borde izquierdo útil
            int xr = cx + BARRA_MEDIO_ANCHO; // borde derecho útil
            int ini = xl + strlen(c.simbolo) * 6 + 1;
            int fin = xr;
            // Símbolo a la izquierda
            background.setTextDatum(TL_DATUM);
            background.setTextColor(TFT_YELLOW);
            background.drawString(c.simbolo, xl, y + BARRA_ETIQ_DY);
            // USDT a la derecha
            if (BARRA_MOSTRAR_USDT)
            {
              background.setTextDatum(TR_DATUM);
              background.setTextColor(TFT_CYAN);
              background.drawString("USDT", xr, y + BARRA_ETIQ_DY);
              fin = xr - 24 - 1;
            }
            // Precio digital centrado entre los dos
            uint16_t colorPrecio = TFT_WHITE;
            if (c.tendencia > 0)
              colorPrecio = BARRA_COLOR_SUBE;
            else if (c.tendencia < 0)
              colorPrecio = BARRA_COLOR_BAJA;
            render.cdrawString(formatoPrecio(c.precio).c_str(), (ini + fin) / 2, y, colorPrecio);
          }
          // Línea de abajo del recuadro (web / By M8AX)
          background.setTextDatum(TC_DATUM);
          colorM8 = esp_random() % (sizeof(colors) / sizeof(colors[0]));
          background.setTextColor(colors[colorM8]);
          background.drawString(BARRA_PIE[col], cx, BARRA_PIE_Y - BARRA_SPR_Y);
        }
        background.pushSprite(0, BARRA_SPR_Y);
        background.deleteSprite();
      }
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
  longitud_pintada = (longitud_total * mineria.currentHashRate.toInt()) / 360;
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
  longitud_pintada = (longitud_total * segundosDelAno) / segundosTotalesDelAno;
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
                mineria.currentTime, mineria.completedShares.c_str(), mineria.totalKHashes.c_str(), mineria.currentHashRate.c_str(), mineria.temp.c_str());
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
                mineria.currentTime, mineria.completedShares.c_str(), mineria.totalKHashes.c_str(), mineria.currentHashRate.c_str(), mineria.temp.c_str());
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
  int longitud_pintada = (longitud_total * segundosDelAno) / segundosTotalesDelAno;
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
  // Recreate sprite to the right side of the screen
  createBackgroundSprite(WIDTH - 5, HEIGHT - 7);
  // Print background screen
  background.pushImage(-190, 0, ImagenM8AXWidth, ImagenM8AXHeight, M8AXQuote1);
  Serial.printf("M8AX - %s >>> Completados %s Share(s), %s Khashes, Prom. Hashrate %s KH/s %s°\n",
                mineria.currentTime, mineria.completedShares.c_str(), mineria.totalKHashes.c_str(), mineria.currentHashRate.c_str(), mineria.temp.c_str());
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
  // Recreate sprite to the right side of the screen
  createBackgroundSprite(WIDTH - 5, HEIGHT - 7);
  // Print background screen
  background.pushImage(-190, 0, ImagenM8AXWidth, ImagenM8AXHeight, ImagenM8AX);
  Serial.printf("M8AX - %s >>> Completados %s Share(s), %s Khashes, Prom. Hashrate %s KH/s %s°\n",
                mineria.currentTime, mineria.completedShares.c_str(), mineria.totalKHashes.c_str(), mineria.currentHashRate.c_str(), mineria.temp.c_str());
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
  tft.print("360 KH/s");
  int grosor = 5;
  int X_INICIO = 5;                                                                // Columna fija
  int X_FINAL = 242;                                                               // Base de la barra
  int Y_POS = 43;                                                                  // Punto más alto
  int longitud_total = X_FINAL - X_INICIO;                                         // Longitud total de la barra (80 píxeles)
  int longitud_pintada = (longitud_total * mineria.currentHashRate.toInt()) / 360; // Porcentaje de la barra según los segundos
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
                mineria.currentTime, mineria.completedShares.c_str(), mineria.totalKHashes.c_str(), mineria.currentHashRate.c_str(), mineria.temp.c_str());
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
  tft.print("paypal.me/m8ax/2");
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
  // Recreate sprite to the right side of the screen
  createBackgroundSprite(WIDTH - 5, HEIGHT - 7);
  // Print background screen
  if (refresca > 0)
  {
    refresca = 0;
    tft.fillRect(0, 0, 320, 170, TFT_BLACK);
  }
  Serial.printf("M8AX - %s >>> Completados %s Share(s), %s Khashes, Prom. Hashrate %s KH/s %s°\n",
                mineria.currentTime, mineria.completedShares.c_str(), mineria.totalKHashes.c_str(), mineria.currentHashRate.c_str(), mineria.temp.c_str());
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
  tft.print("https://paypal.me/m8ax/2");
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
                mineria.currentTime, mineria.completedShares.c_str(), mineria.totalKHashes.c_str(), mineria.currentHashRate.c_str(), mineria.temp.c_str());
  if (hasChangedScreen)
    tft.pushImage(0, 0, initWidth, initHeight, M8AXRelojLunar);
  printPoolData();
  refresca++;
  hasChangedScreen = false;
  int wdtOffset = 190;
  // Recreate sprite to the right side of the screen
  createBackgroundSprite(WIDTH - 5, HEIGHT - 7);
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
                mineria.currentTime, mineria.completedShares.c_str(), mineria.totalKHashes.c_str(), mineria.currentHashRate.c_str(), mineria.temp.c_str());
  if (hasChangedScreen)
    tft.pushImage(0, 0, initWidth, initHeight, M8AXRelojLunar);
  printPoolData();
  refresca++;
  hasChangedScreen = false;
  int wdtOffset = 190;
  // Recreate sprite to the right side of the screen
  createBackgroundSprite(WIDTH - 5, HEIGHT - 7);
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
                mineria.currentTime, mineria.completedShares.c_str(), mineria.totalKHashes.c_str(), mineria.currentHashRate.c_str(), mineria.temp.c_str());
  if (hasChangedScreen)
    tft.pushImage(0, 0, initWidth, initHeight, M8AXRelojLunar);
  printPoolData();
  refresca++;
  hasChangedScreen = false;
  int wdtOffset = 190;
  // Recreate sprite to the right side of the screen
  createBackgroundSprite(WIDTH - 5, HEIGHT - 7);
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
    tft.print("Blockchain No Pregunta Quién Eres, Solo Si Tienes La Llave.");
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
                mineria.currentTime, mineria.completedShares.c_str(), mineria.totalKHashes.c_str(), mineria.currentHashRate.c_str(), mineria.temp.c_str());
  if (hasChangedScreen)
    tft.pushImage(0, 0, initWidth, initHeight, M8AXRelojLunar);
  printPoolData();
  refresca++;
  hasChangedScreen = false;
  int wdtOffset = 190;
  // Recreate sprite to the right side of the screen
  createBackgroundSprite(WIDTH - 5, HEIGHT - 7);
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
                mineria.currentTime, mineria.completedShares.c_str(), mineria.totalKHashes.c_str(), mineria.currentHashRate.c_str(), mineria.temp.c_str());
  if (hasChangedScreen)
    tft.pushImage(0, 0, initWidth, initHeight, M8AXRelojLunar);
  printPoolData();
  refresca++;
  if (refresca > 0)
  {
    tft.pushImage(0, 0, initWidth, initHeight, M8AXRelojLunar);
    refresca = 0;
  }
  hasChangedScreen = false;
  int wdtOffset = 190;
  // Recreate sprite to the right side of the screen
  createBackgroundSprite(WIDTH - 5, HEIGHT - 7);
  // Print background screen
  background.pushImage(-190, 0, ImagenM8AXWidth, ImagenM8AXHeight, M8AXRelojLunar);
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
                mineria.currentTime, mineria.completedShares.c_str(), mineria.totalKHashes.c_str(), mineria.currentHashRate.c_str(), mineria.temp.c_str());
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
                mineria.currentTime, mineria.completedShares.c_str(), mineria.totalKHashes.c_str(), mineria.currentHashRate.c_str(), mineria.temp.c_str());
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
  // Recreate sprite to the right side of the screen
  createBackgroundSprite(WIDTH - 5, HEIGHT - 7);
  // Print background screen
  background.pushImage(-190, 0, ImagenM8AXWidth, ImagenM8AXHeight, M8AXQuote2);
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
  tft.print("          BARRA DE HASH/S 0-360");
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
  longitud_pintada = (longitud_total * segundosDelAno) / segundosTotalesDelAno;
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.fillRect(X_INICIO, Y_POS - 1, longitud_total + 1, 7, TFT_DARKGREY);
  tft.fillRect(X_INICIO, Y_POS - 1, longitud_pintada, 7, colors[colorI]);
  X_INICIO = 87;
  X_FINAL = 213;
  Y_POS = 135;
  longitud_total = X_FINAL - X_INICIO;
  longitud_pintada = (longitud_total * mineria.currentHashRate.toDouble()) / 360.00;
  colorI = esp_random() % (sizeof(colors) / sizeof(colors[0]));
  tft.fillRect(X_INICIO, Y_POS - 1, longitud_total + 1, 3, TFT_DARKGREY);
  tft.drawLine(X_INICIO, Y_POS, X_INICIO + longitud_pintada, Y_POS, colors[colorI]);
  manejandoLeds(mineria.currentHashRate.toFloat());
#ifdef DEBUG_MEMORY
  // Print heap
  printheap();
#endif
}

const int HASHG_ANCHO_MAX = 308;        // ancho máximo del texto (pantalla = 320)
const int HASHG_ALTO_MAX = 150;         // alto máximo del texto (zona de arriba = 170)
const int HASHG_TAM_MAX = 130;          // tamaño de letra máximo a probar
const bool HASHG_MOSTRAR_UNIDAD = true; // false = quita el "KH/s" pequeñito
String hashgUltimo = "";

void tDisplay_hashGigante(unsigned long mElapsed)
{
  mineria = getMiningData(mElapsed);
  detectar = 0;

  // Barra de criptos de abajo (ella sola decide cuándo repintarse)
  printPoolData();

  bool repintar = hasChangedScreen;
  hasChangedScreen = false;

  String hr = mineria.currentHashRate;

  // Solo repinta si cambió el valor o si acabamos de entrar en la pantalla (evita parpadeo)
  if (repintar || hr != hashgUltimo)
  {
    hashgUltimo = hr;

    tft.fillRect(0, 0, 320, 170, TFT_BLACK);

    render.setDrawer(tft); // dibuja directo en la pantalla, sin sprite (ahorra RAM)
    render.setAlignment(Align::TopCenter);

    // Busca el tamaño de letra más grande que quepa
    int tam = HASHG_TAM_MAX;
    while (tam > 10)
    {
      render.setFontSize(tam);
      if (render.getTextWidth("%s", hr.c_str()) <= HASHG_ANCHO_MAX &&
          render.getTextHeight("%s", hr.c_str()) <= HASHG_ALTO_MAX)
        break;
      tam -= 2;
    }

    int alto = render.getTextHeight("%s", hr.c_str());
    int y = (170 - alto) / 2 - 13;
    if (y < 0)
      y = 0;
    render.drawString(hr.c_str(), 160, y, TFT_WHITE);

    if (HASHG_MOSTRAR_UNIDAD)
    {
      tft.setTextFont(1);
      tft.setTextSize(1);
      tft.setTextColor(TFT_DARKGREY, TFT_BLACK);
      tft.setCursor(290, 160);
      tft.print("KH/s");
    }
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

void esp32_2432S028R_PongScreen(unsigned long mElapsed)
{
  mineria = getMiningData(mElapsed);
  Serial.printf("M8AX - %s >>> Completados %s Share(s), %s Khashes, Prom. Hashrate %s KH/s %s°\n",
                mineria.currentTime, mineria.completedShares.c_str(), mineria.totalKHashes.c_str(), mineria.currentHashRate.c_str(), mineria.temp.c_str());

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
      if (touch.getXY(t_x, t_y))
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
                mineria.currentTime, mineria.completedShares.c_str(), mineria.totalKHashes.c_str(), mineria.currentHashRate.c_str(), mineria.temp.c_str());

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
  char lastIzq[24] = "", lastDer[24] = "";

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
      if (touch.getXY(t_x, t_y))
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
                mineria.currentTime, mineria.completedShares.c_str(), mineria.totalKHashes.c_str(), mineria.currentHashRate.c_str(), mineria.temp.c_str());

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
      if (touch.getXY(t_x, t_y))
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
          if (x == 1 || y == 1)
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
          if (millis() > c.deadline)
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
  if (!touch.getXY(t_x, t_y))
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
                mineria.currentTime, mineria.completedShares.c_str(), mineria.totalKHashes.c_str(), mineria.currentHashRate.c_str(), mineria.temp.c_str());

  bool redraw = hasChangedScreen;
  if (!cf.init)
  {
    cf.init = true;
    cf.tiene = false;
    cf.ac = cf.fa = 0;
    cf.nextAt = 0;
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
      cifNuevaCuenta();
      cifDrawAll();
      cifDrawFoot();
      Serial.printf("M8AX - Cifras %d %d %d %d %d %d -> %d | Resultado %d (%s)\n",
                    cf.nums[0], cf.nums[1], cf.nums[2], cf.nums[3], cf.nums[4], cf.nums[5],
                    cf.target, cf.valor, cf.exacto ? "EXACTO" : "DIF");
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
      manejandoLeds(mineria.currentHashRate.toFloat());
    }

    delay(10);
  }

  pongLeds(false);
  tft.setTextPadding(0);
  tft.setTextDatum(TL_DATUM);

  cuentita++;
  if (cuentita == 15)
  {
    ajustarZonaHoraria();
    obtenerLocYTemp(), obtenerVariacionesBTC(variacion1dia, variacion1semana);
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

  if (startTime == 0)
  {
    BOT_TOKEN = Settings.botTelegram;    // Bot De Telegram
    CHAT_ID = Settings.ChanelIDTelegram; // ID Del Canal De Telegram
    startTime = epochTime;               // Guardar el tiempo de inicio cuando el dispositivo arranca
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
  if (epochTime - startTime >= minStartupTime && epochTime - lastTelegramEpochTime >= interval)
  {
    // Ajustar la zona horaria si es necesario
    if ((mes == 3 || mes == 4 || mes == 10 || mes == 11) && horita >= 0 && horita <= 5)
      ajustarZonaHoraria();
    obtenerLocYTemp();
    obtenerVariacionesBTC(variacion1dia, variacion1semana);
    // Verificar si los datos de Telegram están configurados
    if (BOT_TOKEN != "NO CONFIGURADO" && CHAT_ID != "NO CONFIGURADO")
    {
      recopilaTelegram(); // Envia el mensaje a Telegram
    }
    lastTelegramEpochTime = epochTime; // Actualiza el tiempo de la última ejecución
  }
}

void esp32_2432S028R_AnimateCurrentScreen(unsigned long frame)
{
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
    bool pressed = touch.getXY(t_x, t_y);
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
        // Serial.printf("Next screen touch( x:%d y:%d )\n", t_x, t_y);
        currentDisplayDriver->current_cyclic_screen = (currentDisplayDriver->current_cyclic_screen + 1) % currentDisplayDriver->num_cyclic_screens;
      }
      else if (t_x < 160)
      {
        // Previus screen
        // Serial.printf("Previus screen touch( x:%d y:%d )\n", t_x, t_y);
        /* Serial.println(currentDisplayDriver->current_cyclic_screen); */
        currentDisplayDriver->current_cyclic_screen = currentDisplayDriver->current_cyclic_screen - 1;
        if (currentDisplayDriver->current_cyclic_screen < 0)
          currentDisplayDriver->current_cyclic_screen = currentDisplayDriver->num_cyclic_screens - 1;
      }
    }
    previousTouchMillis = currentMillis;
  }
  if (currentScreen != currentDisplayDriver->current_cyclic_screen)
    hasChangedScreen ^= true;
  currentScreen = currentDisplayDriver->current_cyclic_screen;
  switch (mMonitor.NerdStatus)
  {
  case NM_waitingConfig:
    digitalWrite(LED_PIN, LOW); // LED encendido de forma continua
    break;
  case NM_Connecting:
    if (currentMillis - previousMillis >= 500)
    { // 0.5sec blink
      previousMillis = currentMillis;
      // Serial.print("C");
      digitalWrite(LED_PIN, HIGH);
      digitalWrite(LED_PIN_B, !digitalRead(LED_PIN)); // Cambia el estado del LED
    }
    break;
  case NM_hashing:
    if (currentMillis - previousMillis >= 500)
    { // 0.1sec blink
      previousMillis = currentMillis;
      digitalWrite(LED_PIN_B, HIGH);
      digitalWrite(LED_PIN, !digitalRead(LED_PIN)); // Cambia el estado del LED
    }
    break;
  }
}

CyclicScreenFunction esp32_2432S028RCyclicScreens[] = {esp32_2432S028R_MinerScreen, tDisplay_granHash, tDisplay_hashGigante, esp32_2432S028R_BTCprice, esp32_2432S028R_ClockScreen, RelojDeNumeros, tDisplay_m8axScreen7, esp32_2432S028R_m8axScreen1, esp32_2432S028R_m8axScreen2, tDisplay_m8axScreenMegaNerd, tDisplay_m8axScreen5, tDisplay_m8axScreen6, esp32_2432S028R_CifrasScreen, esp32_2432S028R_PongScreen, esp32_2432S028R_KrakoutScreen, esp32_2432S028R_LifeScreen, tDisplay_m8axScreen3, tDisplay_m8axScreen4};

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