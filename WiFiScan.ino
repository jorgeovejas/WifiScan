//Jorge Ovejas
#include "WiFi.h"
//Control del LED
#define LEDC_TIMER_12_BIT 12 
#define LEDC_BASE_FREQ 5000  
#define LED_PIN 5                  
#define LEDC_MAX_DUTY 4095       


const char* targetSSID = "TuSSID";  // Reemplazar "TuSSID" con el SSID de la red WiFi que deseamos scannear

void setup() {
  Serial.begin(115200);
  
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  Serial.println("Setup done");

  ledcAttach(LED_PIN, LEDC_BASE_FREQ, LEDC_TIMER_12_BIT);
}

void loop() {
  Serial.println("Scan start");

  // Funcion de escanear redes
  int n = WiFi.scanNetworks();
  Serial.println("Scan done");
  if (n == 0) {
    Serial.println("no networks found");
  } else {
    Serial.print(n);
    Serial.println(" networks found");
    Serial.println("Nr | SSID                             | RSSI | CH | Encryption");

    // Variable para almacenar el RSSI del SSID
    long targetRSSI = -70;

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

    // Si encontramos la red WiFi deseada, se ajusta el brillo del led de nuestra placa
    if (targetRSSI != -1000) {
      int dutyCycle = map(targetRSSI, -100, 0, 0, LEDC_MAX_DUTY);
      dutyCycle = constrain(dutyCycle, 0, LEDC_MAX_DUTY);

      ledcWrite(LED_PIN, dutyCycle);
      Serial.print("Brillo del LED ajustado a: ");
      Serial.println(dutyCycle);
    } else {
      // Si no se encuentra SSID, apagar el LED
      ledcWrite(LED_PIN, 0);
      Serial.println("SSID no encontrado, LED apagado.");
    }
  }

  Serial.println("");

  // Liberamos memoria del scan de antes
  WiFi.scanDelete();

  // Delay para esperar antes de que vuelva a escanear
  delay(5000);
}
