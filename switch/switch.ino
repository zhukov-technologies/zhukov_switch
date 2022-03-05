#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include <espnow.h>


int32_t channel;

bool butt1, butt2;
uint8_t broadcastAddress[6];

bool new1_flag, new2_flag;
bool new1_flag_answer, new2_flag_answer;
const char* WIFI_SSID = "MYEWWT";
uint8_t Address[2][6]; //mac-адрес прибора

struct {
  byte ID;
  char WIFI_ROUTER_SSID[35]; //ssid роутера для передачи устройствам (чтобы они могли искать основную сеть и настраивать свой канал)
  uint8_t Address[6];
} from_MYEWWT;


struct {
  byte ID;
  byte value;
} to_MYEWWT;


struct {
  byte ID;
  byte value;
} to_relay;



void setup() {

  pinMode(0, OUTPUT);
  digitalWrite(0, HIGH);
  pinMode(14, INPUT);
  pinMode(4, INPUT);

  butt1 = digitalRead(14);
  butt2 = !butt1;

  EEPROM.begin(512);
  if (EEPROM.read(1) != 4) //проверка на первый запуск
  {
    EEPROM.write(1, 4); //если запуск первый, записываем ключ
    EEPROM.commit();
    set_channel();
  }




  EEPROM.get(2, channel);
  wifi_promiscuous_enable(1);
  wifi_set_channel(channel);
  wifi_promiscuous_enable(0);


 

  EEPROM.get(10, Address);
  if (esp_now_init() != 0) {
    return;
  }

  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {


  if (butt1) {
  
    butt1 = 0;
    to_relay.ID = 4;
    to_relay.value = 2;
    WiFi.mode(WIFI_STA);
    esp_now_send(Address[0], (uint8_t *) &to_relay, sizeof(to_relay));
    yield();
  }

  if (butt2) {

    butt2 = 0;
    to_relay.ID = 4;
    to_relay.value = 2;
    WiFi.mode(WIFI_STA);
    esp_now_send(Address[1], (uint8_t *) &to_relay, sizeof(to_relay));
    yield();
  }


  if (!new2_flag && !new1_flag) {
    digitalWrite(0, LOW);
  }


 
  if (millis() > 9000) {
    digitalWrite(0, LOW);
    ESP.deepSleepInstant(0);
  }

  if (digitalRead(14) && (millis() > 6000)) {

    digitalWrite(0, HIGH);
    new1_flag = true;
    to_MYEWWT.ID = 2;
    uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    esp_now_send(broadcastAddress, (uint8_t *) &to_MYEWWT, sizeof(to_MYEWWT));
    delay(1000);
  }

  else if (digitalRead(4) && (millis() > 6000)) {

    digitalWrite(0, HIGH);
    new2_flag = true;
    to_MYEWWT.ID = 2;
    uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    esp_now_send(broadcastAddress, (uint8_t *) &to_MYEWWT, sizeof(to_MYEWWT));
    delay(1000);
  }

  if (new1_flag_answer) {
    for (int i = 0; i < 6; i++) {
      Address[0][i] = from_MYEWWT.Address[i];
    }
    EEPROM.put(10, Address);
    EEPROM.commit();
    to_MYEWWT.ID = 3;
    esp_now_send(broadcastAddress, (uint8_t *) &to_MYEWWT, sizeof(to_MYEWWT));
    yield();
    new1_flag_answer = false;
    digitalWrite(0, LOW);
    ESP.deepSleepInstant(0); //not necessary
  }

  if (new2_flag_answer) {
    for (int i = 0; i < 6; i++) {
      Address[1][i] = from_MYEWWT.Address[i];
    }
    EEPROM.put(10, Address);
    EEPROM.commit();
    to_MYEWWT.ID = 3;
    esp_now_send(broadcastAddress, (uint8_t *) &to_MYEWWT, sizeof(to_MYEWWT));
    yield();
    new2_flag_answer = false;
    digitalWrite(0, LOW);
    ESP.deepSleepInstant(0); //not necessary
  }
}


void OnDataRecv(uint8_t * mac_addr, uint8_t *incomingData, uint8_t len) {
  memcpy(&from_MYEWWT, incomingData, sizeof(from_MYEWWT));
 
  if (from_MYEWWT.ID == 5) {
    for (int i = 0; i < 6; i++) {
      broadcastAddress[i] = mac_addr[i];
    }

    EEPROM.put(400, from_MYEWWT.WIFI_ROUTER_SSID); //если значение не изменено, перезапись не выполняется
    EEPROM.commit();

    if (new1_flag) new1_flag_answer = true;

    if (new2_flag) new2_flag_answer = true;
  }
}


void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  if (sendStatus != 0) {
 
      EEPROM.begin(512);
    EEPROM.write(1, 0);
    EEPROM.commit();
  }

}


int32_t getWiFiChannel(const char *ssid) {
  if (int32_t n = WiFi.scanNetworks()) {
    for (uint8_t i = 0; i < n; i++) {
      if (!strcmp(ssid, WiFi.SSID(i).c_str())) {
        return WiFi.channel(i);
      }
    }
  }
  return 0;
}

void set_channel() {
  
  channel = getWiFiChannel(WIFI_SSID);
  if (channel >= 1) {
    EEPROM.put(2, channel);
    EEPROM.commit();
    wifi_promiscuous_enable(1);
    wifi_set_channel(channel);
    wifi_promiscuous_enable(0);
  }
  else {
    EEPROM.begin(512);

    EEPROM.get(400, from_MYEWWT.WIFI_ROUTER_SSID);
    
    channel = getWiFiChannel(from_MYEWWT.WIFI_ROUTER_SSID);
    if (channel >= 1) {
      EEPROM.put(2, channel);
      EEPROM.commit();
      wifi_promiscuous_enable(1);
      wifi_set_channel(channel);
      wifi_promiscuous_enable(0);
    }
  }
}
