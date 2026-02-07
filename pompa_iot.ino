#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>

/* ================= PIN ================= */
#define POMPA_PIN 2
#define BTN_PIN   0

/* ================= MQTT (Cloud Shiftr Public) ================= */
const char* mqttServer = "public.cloud.shiftr.io";
const int   mqttPort   = 1883;
const char* mqttUser   = "public";
const char* mqttPass   = "public";

/* ================= TOPIC ================= */
const char* topicControl = "iot/esp32/pompa/control";
const char* topicStatus  = "iot/esp32/pompa/status";

/* ================= OBJECT ================= */
WebServer server(80);
Preferences prefs;
WiFiClient espClient;
PubSubClient mqttClient(espClient);

/* ================= GLOBAL ================= */
String ssid, password;

bool pompaState = false;


/* ================= INTERRUPT ================= */
void IRAM_ATTR togglePompa() {

  pompaState = !pompaState;
  digitalWrite(POMPA_PIN, pompaState);

  if (pompaState) {
    mqttClient.publish(topicControl, "ON");
    mqttClient.publish(topicStatus, "ON");
  } else {
    mqttClient.publish(topicControl, "OFF");
    mqttClient.publish(topicStatus, "OFF");
  }

  Serial.println("[BTN] Toggle Pompa");
}


/* ================= MQTT CALLBACK ================= */
void mqttCallback(char* topic, byte* payload, unsigned int length) {

  String msg;

  for (int i = 0; i < length; i++)
    msg += (char)payload[i];

  Serial.println("[MQTT] " + String(topic) + " => " + msg);


  if (String(topic) == topicControl) {

    if (msg == "ON") {

      pompaState = true;
      digitalWrite(POMPA_PIN, HIGH);

      mqttClient.publish(topicStatus, "ON");

      Serial.println("[POMPA] ON dari MQTT");
    }


    if (msg == "OFF") {

      pompaState = false;
      digitalWrite(POMPA_PIN, LOW);

      mqttClient.publish(topicStatus, "OFF");

      Serial.println("[POMPA] OFF dari MQTT");
    }

  }
}


/* ================= MQTT CONNECT ================= */
void connectMQTT() {

  while (!mqttClient.connected()) {

    Serial.println("[MQTT] Connecting...");

    String clientID = "ESP32-" + String(random(1000, 9999));

    if (mqttClient.connect(clientID.c_str(), mqttUser, mqttPass)) {

      Serial.println("[MQTT] Connected");

      mqttClient.subscribe(topicControl);

    } else {

      delay(2000);

    }
  }
}


/* ================= TASK MQTT ================= */
void taskMQTT(void *pv) {

  for (;;) {

    if (WiFi.status() == WL_CONNECTED) {

      if (!mqttClient.connected())
        connectMQTT();

      mqttClient.loop();
    }

    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}


/* ================= WEB PAGE ================= */
const char webpage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<title>ESP32 Smart Control</title>
<meta name="viewport" content="width=device-width, initial-scale=1">

<style>
body{
  font-family:Arial;
  background:#0f172a;
  color:#e5e7eb;
  padding:20px;
}

.card{
  max-width:420px;
  margin:auto;
  background:#020617;
  padding:20px;
  border-radius:16px;
  box-shadow:0 0 20px rgba(0,0,0,.6);
}

.header{
  text-align:center;
  font-size:22px;
  font-weight:bold;
  margin-bottom:20px;
  color:#38bdf8;
}

.box{
  border:1px solid #1e293b;
  border-radius:12px;
  padding:15px;
  margin-bottom:15px;
}

button,input{
  width:100%;
  padding:12px;
  margin:6px 0;
  border:none;
  border-radius:8px;
  font-size:15px;
}

.on{
  background:#22c55e;
  color:#020617;
  font-weight:bold;
}

.off{
  background:#ef4444;
  color:#fff;
  font-weight:bold;
}

.save{
  background:#475569;
  color:#fff;
}
</style>
</head>

<body>

<div class="card">

<div class="header">
ESP32 SMART CONTROL
</div>


<div class="box">

<h3>Kontrol Pompa Air</h3>

<button class="on" onclick="fetch('/pompa/on',{method:'POST'})">
POMPA ON
</button>

<button class="off" onclick="fetch('/pompa/off',{method:'POST'})">
POMPA OFF
</button>

<p>Status Pompa : <b id="pompa">-</b></p>

</div>


<div class="box">

<h3>Status Sistem</h3>

<p>WiFi : <b id="wifi">-</b></p>
<p>IP : <b id="ip">-</b></p>
<p>SSID : <b id="ssid">-</b></p>

</div>


<div class="box">

<h3>Pengaturan WiFi</h3>

<input id="ssidInput" placeholder="Nama WiFi">
<input id="password" type="password" placeholder="Password">

<button class="save" onclick="saveWiFi()">
SIMPAN WIFI
</button>

</div>


</div>


<script>

function refresh(){

 fetch('/status')
 .then(r=>r.json())
 .then(d=>{

  wifi.innerText = d.wifi;
  ip.innerText   = d.ip;
  ssid.innerText = d.ssid;

  pompa.innerText = d.pompa ? 'ON' : 'OFF';

 });

}


function saveWiFi(){

 fetch('/wifi',{
  method:'POST',
  headers:{'Content-Type':'application/json'},
  body:JSON.stringify({
   ssid:ssidInput.value,
   password:password.value
  })
 });

 alert('WiFi disimpan');

}


setInterval(refresh,2000);
refresh();

</script>

</body>
</html>
)rawliteral";


/* ================= SETUP ================= */
void setup() {

  Serial.begin(115200);

  pinMode(POMPA_PIN, OUTPUT);
  pinMode(BTN_PIN, INPUT_PULLUP);

  digitalWrite(POMPA_PIN, LOW);

  attachInterrupt(BTN_PIN, togglePompa, FALLING);


  prefs.begin("wifi", false);

  ssid     = prefs.getString("ssid","");
  password = prefs.getString("password","");


  if (ssid == "")
    startAPMode();
  else
    connectWiFi();


  mqttClient.setServer(mqttServer, mqttPort);
  mqttClient.setCallback(mqttCallback);


  /* ===== ROUTING ===== */

  server.on("/", [](){
    server.send_P(200,"text/html",webpage);
  });


  server.on("/pompa/on", HTTP_POST, [](){

    pompaState = true;
    digitalWrite(POMPA_PIN, HIGH);

    mqttClient.publish(topicControl, "ON");
    mqttClient.publish(topicStatus, "ON");

    server.send(200);

  });


  server.on("/pompa/off", HTTP_POST, [](){

    pompaState = false;
    digitalWrite(POMPA_PIN, LOW);

    mqttClient.publish(topicControl, "OFF");
    mqttClient.publish(topicStatus, "OFF");

    server.send(200);

  });


  server.on("/wifi", HTTP_POST, [](){

    DynamicJsonDocument doc(256);

    deserializeJson(doc, server.arg("plain"));

    ssid     = doc["ssid"].as<String>();
    password = doc["password"].as<String>();

    prefs.putString("ssid",ssid);
    prefs.putString("password",password);

    server.send(200);

    connectWiFi();

  });


  server.on("/status", [](){

    DynamicJsonDocument doc(256);

    doc["wifi"]  = WiFi.status()==WL_CONNECTED?"Connected":"AP Mode";
    doc["ip"]    = WiFi.status()==WL_CONNECTED?WiFi.localIP().toString():WiFi.softAPIP().toString();
    doc["ssid"]  = WiFi.status()==WL_CONNECTED?WiFi.SSID():"ESP32-Config";

    doc["pompa"] = pompaState;

    String res;
    serializeJson(doc,res);

    server.send(200,"application/json",res);

  });


  server.begin();


  xTaskCreatePinnedToCore(
    taskMQTT,
    "MQTT",
    4096,
    NULL,
    1,
    NULL,
    1
  );

}


/* ================= LOOP ================= */
void loop() {

  server.handleClient();

}


/* ================= WIFI ================= */

void connectWiFi(){

  Serial.println("[WIFI] Connecting...");

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid.c_str(),password.c_str());

  unsigned long t = millis();

  while(WiFi.status()!=WL_CONNECTED && millis()-t<15000){
    delay(300);
  }

  if(WiFi.status()==WL_CONNECTED){

    Serial.println("[WIFI] Connected");
    Serial.println(WiFi.localIP());

  } else {

    startAPMode();

  }

}


void startAPMode(){

  WiFi.mode(WIFI_AP);
  WiFi.softAP("ESP32-Config");

  Serial.println("[AP MODE] ESP32-Config");
  Serial.println(WiFi.softAPIP());

}
