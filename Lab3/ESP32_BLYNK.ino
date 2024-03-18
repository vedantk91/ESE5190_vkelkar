
#define BLYNK_PRINT Serial

#define BLYNK_TEMPLATE_ID "TMPL2V9EvYR9x"
#define BLYNK_TEMPLATE_NAME "vkelkar5190"
#define BLYNK_AUTH_TOKEN "xFYaI7zUFqxRwL1nV-Jgcl_jabVAZevi"



#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>


char auth[] = BLYNK_AUTH_TOKEN;

char ssid[] = "Unknown";
char pass[] = "iforgotthepassword";


BLYNK_CONNECTED() {
  Blynk.setProperty(V3, "offImageUrl", "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations.png");
  Blynk.setProperty(V3, "onImageUrl", "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations_pressed.png");
  Blynk.setProperty(V3, "url", "https://docs.blynk.io/en/getting-started/what-do-i-need-to-blynk/how-quickstart-device-was-made");
}

BLYNK_WRITE(V0) {
  int up = param.asInt();
  digitalWrite(12, up);
}

BLYNK_WRITE(V1) {
  int down = param.asInt();
  digitalWrite(13, down);
}

void setup() {
  Serial.begin(115200);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);

  Blynk.begin(auth, ssid, pass);
}

void loop() {
  Blynk.run();
}
