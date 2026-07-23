#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

// =====================================================
// 1. WIFI
// =====================================================

const char* WIFI_NAME = "TP-Link_2.4GHz_509B08";
const char* WIFI_PASSWORD = "";

WebServer server(80);

// =====================================================
// 2. OLED
// =====================================================

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDRESS 0x3C

Adafruit_SSD1306 display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &Wire,
  -1
);

// =====================================================
// 3. DHT11
// =====================================================

#define DHT_PIN 2
#define DHT_TYPE DHT11

DHT dht(DHT_PIN, DHT_TYPE);

// =====================================================
// 4. SOIL SENSOR
// =====================================================

#define SOIL_PIN 34

// Προσωρινές τιμές βαθμονόμησης.
// Θα τις προσαρμόσουμε αργότερα στον δικό σου αισθητήρα.
const int SOIL_DRY_VALUE = 3800;
const int SOIL_WET_VALUE = 1500;

const int WATERING_THRESHOLD = 30;

// =====================================================
// 5. RELAY ΚΑΙ ΑΝΤΛΙΑ
// =====================================================

#define RELAY_PIN 26

// Τα περισσότερα relay modules λειτουργούν με active LOW.
#define RELAY_ON LOW
#define RELAY_OFF HIGH

// Χρόνος λειτουργίας αντλίας: 3 δευτερόλεπτα
const unsigned long PUMP_DURATION = 3000;

// Χρόνος αναμονής ανάμεσα σε αυτόματα ποτίσματα.
// 3 600 000 ms = 1 ώρα
const unsigned long WATERING_COOLDOWN = 3600000;

// =====================================================
// 6. ΧΡΟΝΙΣΜΟΙ
// =====================================================

const unsigned long SENSOR_INTERVAL = 2000;
const unsigned long OLED_INTERVAL = 500;

unsigned long lastSensorReading = 0;
unsigned long lastOLEDUpdate = 0;
unsigned long pumpStartTime = 0;
unsigned long lastWateringTime = 0;

// =====================================================
// 7. ΜΕΤΑΒΛΗΤΕΣ ΑΙΣΘΗΤΗΡΩΝ
// =====================================================

float temperature = 0.0;
float airHumidity = 0.0;

int soilRawValue = 0;
int soilPercentage = 0;

bool dhtError = true;
bool sensorDataReady = false;

// =====================================================
// 8. ΜΕΤΑΒΛΗΤΕΣ ΑΝΤΛΙΑΣ
// =====================================================

bool pumpIsRunning = false;
bool plantHasBeenWatered = false;
bool lastWateringWasManual = false;

// =====================================================
// 9. ΔΗΛΩΣΕΙΣ ΣΥΝΑΡΤΗΣΕΩΝ
// =====================================================

void connectToWiFi();

void readSensors();
void controlPump();
void startPump(bool manualRequest);
void stopPump();

void updateOLED();

void handleHomePage();
void handleSensorData();
void handleWaterRequest();
void handleNotFound();

const char* getPlantStatus();
const char* getPlantMessage();
const char* getStatusClass();

// =====================================================
// 10. ΙΣΤΟΣΕΛΙΔΑ
// =====================================================

const char WEBPAGE[] PROGMEM = R"HTML(
<!DOCTYPE html>
<html lang="en">

<head>
  <meta charset="UTF-8">

  <meta
    name="viewport"
    content="width=device-width, initial-scale=1"
  >

  <meta
    name="theme-color"
    content="#07150f"
  >

  <title>Smart Plant</title>

  <style>
    :root {
      --background: #07150f;
      --background-light: #0b1d15;
      --card: #11271d;
      --card-border: #254336;
      --text: #f1fff6;
      --muted: #9bb5a7;
      --green: #00e887;
      --green-dark: #009f5c;
      --yellow: #f4b942;
      --red: #ff6262;
      --bar-background: #31483d;
    }

    * {
      box-sizing: border-box;
    }

    body {
      min-height: 100vh;
      margin: 0;
      padding: 24px 16px;
      font-family: Arial, Helvetica, sans-serif;
      color: var(--text);
      background:
        radial-gradient(
          circle at top,
          var(--background-light),
          var(--background) 55%
        );
    }

    .container {
      width: 100%;
      max-width: 760px;
      margin: auto;
    }

    .header {
      margin-bottom: 22px;
      text-align: center;
    }

    .header h1 {
      margin: 0 0 10px;
      font-size: 36px;
    }

    .connection {
      display: inline-flex;
      align-items: center;
      gap: 8px;
      padding: 8px 14px;
      border: 1px solid #176d49;
      border-radius: 999px;
      background: #0b3b29;
      color: var(--green);
      font-size: 14px;
      font-weight: bold;
    }

    .connection.offline {
      border-color: #7a3434;
      background: #3b1c1c;
      color: var(--red);
    }

    .connection-dot {
      width: 9px;
      height: 9px;
      border-radius: 50%;
      background: currentColor;
      box-shadow: 0 0 10px currentColor;
    }

    .grid {
      display: grid;
      grid-template-columns: repeat(2, 1fr);
      gap: 16px;
    }

    .card {
      padding: 22px;
      border: 1px solid var(--card-border);
      border-radius: 18px;
      background: var(--card);
      box-shadow: 0 12px 30px rgba(0, 0, 0, 0.25);
    }

    .full-width {
      grid-column: 1 / -1;
    }

    .card-title {
      margin-bottom: 14px;
      color: var(--muted);
      font-size: 15px;
      font-weight: bold;
    }

    .value {
      font-size: 36px;
      font-weight: bold;
    }

    .unit {
      margin-left: 5px;
      color: var(--muted);
      font-size: 19px;
    }

    .soil-row {
      display: flex;
      align-items: center;
      justify-content: space-between;
      margin-bottom: 16px;
    }

    .soil-value {
      font-size: 35px;
      font-weight: bold;
    }

    .raw-value {
      color: var(--muted);
      font-size: 13px;
    }

    .progress-background {
      width: 100%;
      height: 18px;
      overflow: hidden;
      border-radius: 999px;
      background: var(--bar-background);
    }

    .progress-bar {
      width: 0;
      height: 100%;
      border-radius: inherit;
      background: var(--green);
      transition:
        width 0.5s ease,
        background-color 0.4s ease;
    }

    .status-card {
      border-left: 6px solid var(--green);
      transition: border-color 0.4s ease;
    }

    .status-title {
      margin-bottom: 8px;
      font-size: 27px;
      font-weight: bold;
    }

    .status-message {
      margin: 0;
      color: #c0d0c7;
      line-height: 1.5;
    }

    .healthy {
      color: var(--green);
    }

    .attention {
      color: var(--yellow);
    }

    .dry,
    .error {
      color: var(--red);
    }

    .pump-value {
      font-size: 29px;
      font-weight: bold;
    }

    .pump-off {
      color: var(--muted);
    }

    .pump-on {
      color: var(--green);
      text-shadow: 0 0 10px rgba(0, 232, 135, 0.35);
    }

    .mode-value {
      color: var(--green);
      font-size: 27px;
      font-weight: bold;
    }

    .water-button {
      width: 100%;
      margin-top: 18px;
      padding: 18px;
      border: 1px solid #00d47b;
      border-radius: 15px;
      background: var(--green-dark);
      color: white;
      font-size: 18px;
      font-weight: bold;
      cursor: pointer;
      transition:
        transform 0.2s ease,
        background 0.2s ease,
        box-shadow 0.2s ease;
    }

    .water-button:hover {
      transform: translateY(-2px);
      background: #00b96b;
      box-shadow: 0 8px 24px rgba(0, 232, 135, 0.18);
    }

    .water-button:disabled {
      transform: none;
      border-color: #52695d;
      background: #3b5045;
      color: #a6b5ad;
      cursor: not-allowed;
      box-shadow: none;
    }

    .error-box {
      display: none;
      margin-top: 16px;
      padding: 14px;
      border: 1px solid #8c3f3f;
      border-radius: 12px;
      background: #351a1a;
      color: #ff9a9a;
      text-align: center;
    }

    .footer {
      margin-top: 20px;
      color: #81978b;
      font-size: 14px;
      text-align: center;
    }

    @media (max-width: 600px) {
      body {
        padding: 16px 12px;
      }

      .grid {
        grid-template-columns: 1fr;
      }

      .full-width {
        grid-column: auto;
      }

      .header h1 {
        font-size: 30px;
      }

      .value {
        font-size: 32px;
      }

      .card {
        padding: 19px;
      }
    }
  </style>
</head>

<body>

  <main class="container">

    <header class="header">

      <h1>🌱 Smart Plant</h1>

      <div
        id="connectionStatus"
        class="connection"
      >
        <span class="connection-dot"></span>

        <span id="connectionText">
          Connecting...
        </span>
      </div>

    </header>

    <section class="grid">

      <article class="card">

        <div class="card-title">
          🌡 Temperature
        </div>

        <span
          id="temperature"
          class="value"
        >
          --
        </span>

        <span class="unit">
          °C
        </span>

      </article>

      <article class="card">

        <div class="card-title">
          💧 Air Humidity
        </div>

        <span
          id="humidity"
          class="value"
        >
          --
        </span>

        <span class="unit">
          %
        </span>

      </article>

      <article class="card full-width">

        <div class="card-title">
          🌱 Soil Moisture
        </div>

        <div class="soil-row">

          <div
            id="soilPercentage"
            class="soil-value"
          >
            -- %
          </div>

          <div
            id="soilRaw"
            class="raw-value"
          >
            Raw: --
          </div>

        </div>

        <div class="progress-background">

          <div
            id="soilBar"
            class="progress-bar"
          ></div>

        </div>

      </article>

      <article
        id="plantCard"
        class="card full-width status-card"
      >

        <div class="card-title">
          Plant Status
        </div>

        <div
          id="plantStatus"
          class="status-title"
        >
          Loading...
        </div>

        <p
          id="plantMessage"
          class="status-message"
        >
          Waiting for sensor data...
        </p>

      </article>

      <article class="card">

        <div class="card-title">
          🚰 Pump
        </div>

        <div
          id="pumpStatus"
          class="pump-value pump-off"
        >
          OFF
        </div>

      </article>

      <article class="card">

        <div class="card-title">
          ⚙ Watering Mode
        </div>

        <div class="mode-value">
          AUTOMATIC
        </div>

      </article>

    </section>

    <button
      id="waterButton"
      class="water-button"
      type="button"
    >
      💧 WATER NOW
    </button>

    <div
      id="errorBox"
      class="error-box"
    >
      The page cannot receive data from the ESP32.
    </div>

    <footer class="footer">

      Last update:
      <span id="lastUpdate">--</span>

    </footer>

  </main>

  <script>
    const connectionStatus =
      document.getElementById("connectionStatus");

    const connectionText =
      document.getElementById("connectionText");

    const temperatureElement =
      document.getElementById("temperature");

    const humidityElement =
      document.getElementById("humidity");

    const soilPercentageElement =
      document.getElementById("soilPercentage");

    const soilRawElement =
      document.getElementById("soilRaw");

    const soilBar =
      document.getElementById("soilBar");

    const plantCard =
      document.getElementById("plantCard");

    const plantStatus =
      document.getElementById("plantStatus");

    const plantMessage =
      document.getElementById("plantMessage");

    const pumpStatus =
      document.getElementById("pumpStatus");

    const waterButton =
      document.getElementById("waterButton");

    const errorBox =
      document.getElementById("errorBox");

    const lastUpdate =
      document.getElementById("lastUpdate");

    function setConnectionState(isOnline) {
      if (isOnline) {
        connectionStatus.classList.remove("offline");
        connectionText.textContent = "ESP32 Connected";
        errorBox.style.display = "none";
      } else {
        connectionStatus.classList.add("offline");
        connectionText.textContent = "Connection Error";
        errorBox.style.display = "block";
      }
    }

    function getStatusColor(statusClass) {
      if (statusClass === "healthy") {
        return "#00e887";
      }

      if (statusClass === "attention") {
        return "#f4b942";
      }

      return "#ff6262";
    }

    function updateInterface(data) {
      if (data.dhtError) {
        temperatureElement.textContent = "--";
        humidityElement.textContent = "--";
      } else {
        temperatureElement.textContent =
          Number(data.temperature).toFixed(1);

        humidityElement.textContent =
          Number(data.humidity).toFixed(0);
      }

      soilPercentageElement.textContent =
        data.soil + " %";

      soilRawElement.textContent =
        "Raw: " + data.soilRaw;

      soilBar.style.width =
        data.soil + "%";

      const statusColor =
        getStatusColor(data.statusClass);

      soilBar.style.backgroundColor =
        statusColor;

      plantCard.style.borderLeftColor =
        statusColor;

      plantStatus.textContent =
        data.plantStatus;

      plantStatus.className =
        "status-title " + data.statusClass;

      plantMessage.textContent =
        data.plantMessage;

      if (data.pumpOn) {
        pumpStatus.textContent = "WATERING";
        pumpStatus.className =
          "pump-value pump-on";

        waterButton.textContent =
          "💦 WATERING...";

        waterButton.disabled = true;
      } else {
        pumpStatus.textContent = "OFF";
        pumpStatus.className =
          "pump-value pump-off";

        waterButton.textContent =
          "💧 WATER NOW";

        waterButton.disabled = false;
      }

      lastUpdate.textContent =
        new Date().toLocaleTimeString();
    }

    async function requestSensorData() {
      try {
        const response = await fetch(
          "/api/data?t=" + Date.now(),
          {
            method: "GET",
            cache: "no-store"
          }
        );

        if (!response.ok) {
          throw new Error(
            "HTTP error " + response.status
          );
        }

        const data = await response.json();

        updateInterface(data);
        setConnectionState(true);
      } catch (error) {
        console.error(
          "Sensor data error:",
          error
        );

        setConnectionState(false);
      }
    }

    async function waterNow() {
      waterButton.disabled = true;
      waterButton.textContent =
        "Starting pump...";

      try {
        const response = await fetch(
          "/api/water",
          {
            method: "POST",
            cache: "no-store"
          }
        );

        if (!response.ok) {
          throw new Error(
            "HTTP error " + response.status
          );
        }

        await requestSensorData();
      } catch (error) {
        console.error(
          "Pump request error:",
          error
        );

        setConnectionState(false);
        waterButton.disabled = false;
        waterButton.textContent =
          "💧 WATER NOW";
      }
    }

    waterButton.addEventListener(
      "click",
      waterNow
    );

    requestSensorData();

    setInterval(
      requestSensorData,
      2000
    );
  </script>

</body>
</html>
)HTML";

// =====================================================
// 11. SETUP
// =====================================================

void setup()
{
  Serial.begin(115200);

  delay(500);

  Serial.println();
  Serial.println("==========================");
  Serial.println("Smart Plant starting...");
  Serial.println("==========================");

  // I2C:
  // SDA -> GPIO 21
  // SCL -> GPIO 22
  Wire.begin(21, 22);

  if (!display.begin(
        SSD1306_SWITCHCAPVCC,
        OLED_ADDRESS))
  {
    Serial.println("OLED initialization failed.");

    while (true)
    {
      delay(100);
    }
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("SMART PLANT");
  display.println();
  display.println("Starting...");
  display.display();

  dht.begin();

  pinMode(RELAY_PIN, OUTPUT);

  // Η αντλία ξεκινά κλειστή.
  digitalWrite(RELAY_PIN, RELAY_OFF);

  connectToWiFi();

  // Κεντρική ιστοσελίδα
  server.on(
    "/",
    HTTP_GET,
    handleHomePage
  );

  // Endpoint αισθητήρων
  server.on(
    "/api/data",
    HTTP_GET,
    handleSensorData
  );

  // Endpoint χειροκίνητου ποτίσματος
  server.on(
    "/api/water",
    HTTP_POST,
    handleWaterRequest
  );

  server.onNotFound(handleNotFound);

  server.begin();

  Serial.println("Web server started.");
  Serial.print("Open: http://");
  Serial.println(WiFi.localIP());

  readSensors();
  updateOLED();
}

// =====================================================
// 12. LOOP
// =====================================================

void loop()
{
  // Πρέπει να εκτελείται συνέχεια ώστε να απαντά
  // το ESP32 στα αιτήματα του browser.
  server.handleClient();

  unsigned long currentTime = millis();

  if (
    currentTime - lastSensorReading
    >= SENSOR_INTERVAL
  )
  {
    lastSensorReading = currentTime;
    readSensors();
  }

  controlPump();

  if (
    currentTime - lastOLEDUpdate
    >= OLED_INTERVAL
  )
  {
    lastOLEDUpdate = currentTime;
    updateOLED();
  }

  // Δίνει λίγο χρόνο στο Wi-Fi stack.
  delay(2);
}

// =====================================================
// 13. WIFI
// =====================================================

void connectToWiFi()
{
  WiFi.mode(WIFI_STA);

  Serial.print("Connecting to Wi-Fi: ");
  Serial.println(WIFI_NAME);

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Connecting WiFi...");
  display.display();

  if (strlen(WIFI_PASSWORD) == 0)
  {
    WiFi.begin(WIFI_NAME);
  }
  else
  {
    WiFi.begin(
      WIFI_NAME,
      WIFI_PASSWORD
    );
  }

  unsigned long connectionStart = millis();

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");

    // Αν δεν συνδεθεί σε 30 δευτερόλεπτα,
    // κάνει restart και προσπαθεί ξανά.
    if (
      millis() - connectionStart
      > 30000
    )
    {
      Serial.println();
      Serial.println(
        "Wi-Fi timeout. Restarting..."
      );

      ESP.restart();
    }
  }

  Serial.println();
  Serial.println("Wi-Fi connected.");

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("WiFi connected");
  display.println();
  display.println("IP address:");
  display.println(WiFi.localIP());
  display.display();

  delay(2000);
}

// =====================================================
// 14. ΑΝΑΓΝΩΣΗ ΑΙΣΘΗΤΗΡΩΝ
// =====================================================

void readSensors()
{
  float newTemperature =
    dht.readTemperature();

  float newHumidity =
    dht.readHumidity();

  if (
    isnan(newTemperature)
    ||
    isnan(newHumidity)
  )
  {
    dhtError = true;

    Serial.println(
      "DHT11 reading failed."
    );
  }
  else
  {
    dhtError = false;

    temperature =
      newTemperature;

    airHumidity =
      newHumidity;
  }

  soilRawValue =
    analogRead(SOIL_PIN);

  soilPercentage =
    map(
      soilRawValue,
      SOIL_DRY_VALUE,
      SOIL_WET_VALUE,
      0,
      100
    );

  soilPercentage =
    constrain(
      soilPercentage,
      0,
      100
    );

  sensorDataReady = true;

  Serial.println();
  Serial.println("----- SENSOR DATA -----");

  if (!dhtError)
  {
    Serial.print("Temperature: ");
    Serial.print(temperature, 1);
    Serial.println(" C");

    Serial.print("Air humidity: ");
    Serial.print(airHumidity, 1);
    Serial.println(" %");
  }

  Serial.print("Soil raw: ");
  Serial.println(soilRawValue);

  Serial.print("Soil moisture: ");
  Serial.print(soilPercentage);
  Serial.println(" %");
}

// =====================================================
// 15. ΑΥΤΟΜΑΤΟΣ ΕΛΕΓΧΟΣ ΑΝΤΛΙΑΣ
// =====================================================

void controlPump()
{
  if (pumpIsRunning)
  {
    if (
      millis() - pumpStartTime
      >= PUMP_DURATION
    )
    {
      stopPump();
    }

    return;
  }

  // Δεν ποτίζει πριν υπάρξει πραγματική μέτρηση.
  if (!sensorDataReady)
  {
    return;
  }

  bool soilIsDry =
    soilPercentage < WATERING_THRESHOLD;

  bool cooldownHasFinished =
    !plantHasBeenWatered
    ||
    (
      millis() - lastWateringTime
      >= WATERING_COOLDOWN
    );

  if (
    soilIsDry
    &&
    cooldownHasFinished
  )
  {
    Serial.println(
      "Automatic watering started."
    );

    startPump(false);
  }
}

// =====================================================
// 16. ΕΚΚΙΝΗΣΗ ΑΝΤΛΙΑΣ
// =====================================================

void startPump(bool manualRequest)
{
  if (pumpIsRunning)
  {
    return;
  }

  digitalWrite(
    RELAY_PIN,
    RELAY_ON
  );

  pumpIsRunning = true;
  lastWateringWasManual = manualRequest;
  pumpStartTime = millis();

  if (manualRequest)
  {
    Serial.println(
      "Manual watering: Pump ON"
    );
  }
  else
  {
    Serial.println(
      "Automatic watering: Pump ON"
    );
  }
}

// =====================================================
// 17. ΣΤΑΜΑΤΗΜΑ ΑΝΤΛΙΑΣ
// =====================================================

void stopPump()
{
  digitalWrite(
    RELAY_PIN,
    RELAY_OFF
  );

  pumpIsRunning = false;
  plantHasBeenWatered = true;
  lastWateringTime = millis();

  Serial.println("Pump OFF");
}

// =====================================================
// 18. OLED
// =====================================================

void updateOLED()
{
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);

  display.println("SMART PLANT");
  display.println("---------------------");

  if (dhtError)
  {
    display.println("DHT11: ERROR");
  }
  else
  {
    display.print("Temp: ");
    display.print(temperature, 1);
    display.println(" C");

    display.print("Air:  ");
    display.print(airHumidity, 0);
    display.println(" %");
  }

  display.print("Soil: ");
  display.print(soilPercentage);
  display.println(" %");

  display.print("Pump: ");

  if (pumpIsRunning)
  {
    display.println("ON");
  }
  else
  {
    display.println("OFF");
  }

  display.println();

  if (soilPercentage < 30)
  {
    display.println("Needs water");
  }
  else if (soilPercentage < 60)
  {
    display.println("Check plant");
  }
  else
  {
    display.println("Plant healthy");
  }

  display.display();
}

// =====================================================
// 19. ΚΑΤΑΣΤΑΣΗ ΦΥΤΟΥ
// =====================================================

const char* getPlantStatus()
{
  if (!sensorDataReady)
  {
    return "Waiting";
  }

  if (soilPercentage < 30)
  {
    return "Needs Water";
  }

  if (soilPercentage < 60)
  {
    return "Needs Attention";
  }

  return "Healthy";
}

const char* getPlantMessage()
{
  if (!sensorDataReady)
  {
    return "Waiting for the first sensor measurement.";
  }

  if (soilPercentage < 30)
  {
    return
      "The soil is dry. Automatic watering is enabled.";
  }

  if (soilPercentage < 60)
  {
    return
      "The soil moisture is moderate. Check the plant later.";
  }

  return
    "The soil moisture is at a good level.";
}

const char* getStatusClass()
{
  if (!sensorDataReady)
  {
    return "error";
  }

  if (soilPercentage < 30)
  {
    return "dry";
  }

  if (soilPercentage < 60)
  {
    return "attention";
  }

  return "healthy";
}

// =====================================================
// 20. ΑΠΟΣΤΟΛΗ ΙΣΤΟΣΕΛΙΔΑΣ
// =====================================================

void handleHomePage()
{
  server.sendHeader(
    "Cache-Control",
    "no-store, no-cache, must-revalidate"
  );

  server.send_P(
    200,
    "text/html; charset=utf-8",
    WEBPAGE
  );
}

// =====================================================
// 21. ΑΠΟΣΤΟΛΗ JSON
// =====================================================

void handleSensorData()
{
  char json[650];

  char temperatureText[16];
  char humidityText[16];

  if (dhtError)
  {
    strcpy(
      temperatureText,
      "null"
    );

    strcpy(
      humidityText,
      "null"
    );
  }
  else
  {
    snprintf(
      temperatureText,
      sizeof(temperatureText),
      "%.1f",
      temperature
    );

    snprintf(
      humidityText,
      sizeof(humidityText),
      "%.1f",
      airHumidity
    );
  }

  snprintf(
    json,
    sizeof(json),

    "{"
      "\"temperature\":%s,"
      "\"humidity\":%s,"
      "\"dhtError\":%s,"
      "\"soil\":%d,"
      "\"soilRaw\":%d,"
      "\"pumpOn\":%s,"
      "\"plantStatus\":\"%s\","
      "\"statusClass\":\"%s\","
      "\"plantMessage\":\"%s\""
    "}",

    temperatureText,
    humidityText,
    dhtError ? "true" : "false",
    soilPercentage,
    soilRawValue,
    pumpIsRunning ? "true" : "false",
    getPlantStatus(),
    getStatusClass(),
    getPlantMessage()
  );

  server.sendHeader(
    "Cache-Control",
    "no-store"
  );

  server.send(
    200,
    "application/json; charset=utf-8",
    json
  );

  Serial.println(
    "Browser requested /api/data"
  );
}

// =====================================================
// 22. WATER NOW
// =====================================================

void handleWaterRequest()
{
  if (!pumpIsRunning)
  {
    startPump(true);
  }

  server.sendHeader(
    "Cache-Control",
    "no-store"
  );

  server.send(
    200,
    "application/json",
    "{\"success\":true}"
  );
}

// =====================================================
// 23. ΑΓΝΩΣΤΗ ΔΙΕΥΘΥΝΣΗ
// =====================================================

void handleNotFound()
{
  server.send(
    404,
    "text/plain",
    "Page not found"
  );
}