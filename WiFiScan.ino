//Jorge Ovejas y Claudio Murciano
#include "WiFi.h"
// Configuración para el control del LED
#define LEDC_TIMER_12_BIT 12       // Resolución de 12 bits para el temporizador LEDC
#define LEDC_BASE_FREQ 5000        // Frecuencia base de 5000 Hz
#define LED_PIN 5                  // Pin donde está conectado el LED
#define LEDC_MAX_DUTY 4095         // Valor máximo de duty cycle (4095 para 12 bits)


const char* targetSSID = "iPhone Claudio";  // Reemplaza "TuSSID" con el SSID de la red WiFi que desees

void setup() {
  // Inicializar comunicación serial
  Serial.begin(115200);

  // Configurar el modo WiFi en estación y desconectar cualquier AP previo
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  Serial.println("Setup done");

  // Configurar el temporizador para el LED
  ledcAttach(LED_PIN, LEDC_BASE_FREQ, LEDC_TIMER_12_BIT);
}

void loop() {
  Serial.println("Scan start");

  // Escanear redes WiFi disponibles
  int n = WiFi.scanNetworks();
  Serial.println("Scan done");
  if (n == 0) {
    Serial.println("no networks found");
  } else {
    Serial.print(n);
    Serial.println(" networks found");
    Serial.println("Nr | SSID                             | RSSI | CH | Encryption");

    // Variable para almacenar el RSSI del SSID específico
    long targetRSSI = -70;  // Valor inicial muy bajo

    for (int i = 0; i < n; ++i) {
      Serial.printf("%2d", i + 1);
      Serial.print(" | ");
      Serial.printf("%-32.32s", WiFi.SSID(i).c_str());
      Serial.print(" | ");
      long rssi = WiFi.RSSI(i);
      Serial.printf("%4ld", rssi);
      Serial.print(" | ");
      Serial.printf("%2ld", WiFi.channel(i));
      Serial.print(" | ");
      switch (WiFi.encryptionType(i)) {
        case WIFI_AUTH_OPEN:            Serial.print("open"); break;
        case WIFI_AUTH_WEP:             Serial.print("WEP"); break;
        case WIFI_AUTH_WPA_PSK:         Serial.print("WPA"); break;
        case WIFI_AUTH_WPA2_PSK:        Serial.print("WPA2"); break;
        case WIFI_AUTH_WPA_WPA2_PSK:    Serial.print("WPA+WPA2"); break;
        case WIFI_AUTH_WPA2_ENTERPRISE: Serial.print("WPA2-EAP"); break;
        case WIFI_AUTH_WPA3_PSK:        Serial.print("WPA3"); break;
        case WIFI_AUTH_WPA2_WPA3_PSK:   Serial.print("WPA2+WPA3"); break;
        case WIFI_AUTH_WAPI_PSK:        Serial.print("WAPI"); break;
        default:                        Serial.print("unknown");
      }
      Serial.println();

      // Si el SSID coincide con el objetivo, almacenar el RSSI
      if (WiFi.SSID(i) == targetSSID) {
        targetRSSI = rssi;
      }

      delay(10);
    }

    // Si encontramos la red WiFi deseada, ajustar el brillo del LED
    if (targetRSSI != -1000) {
      // Mapear el RSSI al rango del PWM (de -100 a 0 dBm se mapea a 0 a 4095)
      int dutyCycle = map(targetRSSI, -100, 0, 0, LEDC_MAX_DUTY);
      dutyCycle = constrain(dutyCycle, 0, LEDC_MAX_DUTY);  // Asegurar que esté en el rango válido

      // Ajustar el brillo del LED según la intensidad de la señal del SSID específico
      ledcWrite(LED_PIN, dutyCycle);
      Serial.print("Brillo del LED ajustado a: ");
      Serial.println(dutyCycle);
    } else {
      // Si no se encuentra el SSID, apagar el LED
      ledcWrite(LED_PIN, 0);
      Serial.println("SSID no encontrado, LED apagado.");
    }
  }

  Serial.println("");

  // Liberar memoria de los resultados de escaneo
  WiFi.scanDelete();

  // Esperar antes de escanear de nuevo
  delay(5000);
}
