#define BLYNK_TEMPLATE_ID "TMPL3xXaLaFM_"
#define BLYNK_TEMPLATE_NAME "Smart Irrigation System with ESP32"
#define BLYNK_AUTH_TOKEN "MAgifsJ32VZovXsdfygeFY6IWfdhFLsLjfgjk7GbpUb"


#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <BlynkSimpleEsp32.h> 

#define SOIL_MOISTURE_PIN 34   // Broche analogique pour capteur d'humidité du sol
#define THRESHOLD_MOISTURE 100 // Ajuster cette valeur en fonction des lectures du capteur d'humidité 
#define PUMP_PIN 2  // Goupille numérique pour contrôler la pompe
#define PUMP_SWITCH V6  // Goupille virtuelle pour le contrôle manuel de la pompe

char auth[] = BLYNK_AUTH_TOKEN;  // On remplace par le jeton d'authentification Blynk
char ssid[] = "WiFi Username";   // On remplace par les identifiants wifi
char pass[] = "WiFi Password";

BlynkTimer timer;

bool isPumpOn = false;  // Variable pour suivre l'état de la pompe

void sendSensorData()
{
  int soilMoisture = analogRead(SOIL_MOISTURE_PIN);
  Serial.print("Soil Moisture ");
  Serial.println(soilMoisture);
  // Correspondre les valeurs du capteur analogique à un pourcentage (0-100)
  int soilMoisturePercentage = map(soilMoisture, 3500, 4095, 100, 0);
  Serial.print("Soil Moisture Percentage ");
  Serial.println(soilMoisturePercentage);
  // Envoyer les données sur l'humidité à Blynk
  Blynk.virtualWrite(V5, soilMoisturePercentage);

  // Vérifier si la pompe doit être en marche en fonction de l'interrupteur manuel ou du niveau d'humidité
  if (isPumpOn || soilMoisturePercentage < THRESHOLD_MOISTURE)
  {
    // Allumer la pompe
    digitalWrite(PUMP_PIN, HIGH);
    // Vérifier si la pompe est mise en marche automatiquement et non manuellement 
    if (!isPumpOn) {
      // Envoyer une notification à Blynk si la pompe est mise en marche automatiquement
      Blynk.logEvent("moisture_alert","Soil moisture is below the threshold!");
      Serial.println("Soil moisture is below the threshold!");
    }
  }
  else
  {
    // N'éteigner la pompe que si elle n'a pas été mise en marche manuellement 
    if (!isPumpOn) {
      digitalWrite(PUMP_PIN, LOW);
    }
  }
}

BLYNK_WRITE(PUMP_SWITCH)
{
  isPumpOn = param.asInt();
  if (isPumpOn) {
    Serial.println("Pump manually turned ON");
  } else {
    Serial.println("Pump manually turned OFF");
  }
}

void setup()
{
  Serial.begin(9600);
  pinMode(PUMP_PIN, OUTPUT); // Définir la goupille de la pompe en tant que sortie

  Blynk.begin(auth, ssid, pass);

  timer.setInterval(3000L, sendSensorData); // Régler l'intervalle de vérification de l'humidité (toutes les 3 secondes)

  // Configurer le widget de commutation
  Blynk.virtualWrite(PUMP_SWITCH, isPumpOn);
  Blynk.syncVirtual(PUMP_SWITCH);
}

void loop()
{
  Blynk.run();
  timer.run();
}
