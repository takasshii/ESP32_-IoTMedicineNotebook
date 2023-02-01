#include "Network.h"
#include "addons/TokenHelper.h"

#define WIFI_SSID //YOUR SSID
#define WIFI_PASSWORD //YOUR PASSWORD

#define API_KEY //YOUR APIKEY
#define FIREBASE_PROJECT_ID //YOUR PROJECTID 

static Network *instance = NULL;

Network::Network(){}

Network::Network(FuncPtrInt f){
  instance = this;
  callBackEvent = f;
}

void WiFiEventConnected(WiFiEvent_t event, WiFiEventInfo_t info){
  Serial.println("WiFi Connected. Waiting For Connecting Local IP Adress...");
  delay(1);
}

void WiFiEventGotIP(WiFiEvent_t event, WiFiEventInfo_t info){
  Serial.print("Local IP Adress is");
  Serial.println(WiFi.localIP());
  instance->callBackEvent(NETWORK_CONNECTED);
  instance->firebaseInit();
  delay(1);
}

void WiFiEventDisconnected(WiFiEvent_t event, WiFiEventInfo_t info){
  Serial.println("WiFi Disconnected.");
  instance->callBackEvent(NETWORK_DISCONNECTED);
  WiFi.disconnect();
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  delay(10);
}

void Network::initWiFi(){
  WiFi.onEvent(WiFiEventConnected, ARDUINO_EVENT_WIFI_STA_CONNECTED);
  WiFi.onEvent(WiFiEventGotIP, ARDUINO_EVENT_WIFI_STA_GOT_IP);
  WiFi.onEvent(WiFiEventDisconnected, ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
  WiFi.disconnect();
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  delay(1);
}

void Network::firebaseInit(){
  config.api_key = API_KEY;

  config.signer.test_mode = true;

  config.token_status_callback = tokenStatusCallback;
  
  fbdo.setResponseSize(2048);

  Firebase.begin(&config, &auth);

  Firebase.reconnectWiFi(true);

  delay(1);

  if(Firebase.ready()) {
    instance->callBackEvent(FIREBASE_CONNECTED);
  } else {
    instance->callBackEvent(FIREBASE_DISCONNECTED);
  }
}

void Network::firestoreDataUpdate(double weight, String medicine) {
  if(WiFi.status() == WL_CONNECTED && Firebase.ready()) {
    String uid = String(auth.token.uid.c_str());
    String timestamp = String(fbdo.to<int>());

    String documentPath = "users/" + uid + "/";

    FirebaseJson content;

    content.set("fields/medicine/stringValue", String(medicine).c_str());
    content.set("fields/weight/doubleValue", String(weight).c_str());

    if(Firebase.Firestore.createDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath.c_str(), content.raw())){
      Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
      instance->callBackEvent(FIREBASE_SEND_COMPLETED);
      return;
    } else {
      Serial.print("timeStamp :");
      Serial.println(timestamp);
      Serial.println(fbdo.errorReason());
    }
  }
}
