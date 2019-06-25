// ESP8266WiFi kütüphanesini ve PubSubClient kütüphanesini yükleme

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"

// Kullanmakta olduğunuz DHT sensör tipi için aşağıdaki satırlardan birini kullanabiliriz.Bende DHT 22 olduğu için o satırı bıraktım.
//#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT21   // DHT 21 (AM2301)
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321

// Aşağıdaki kimlik bilgilerini değiştirerek,ESP8266
yönlendiricimize bağlanırız.

const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

// Değişkeni Raspberry Pi IP adresimle değiştirdim, böylece MQTT brokerına bağlanabilir.
const char* mqtt_server = "YOUR_RPi_IP_Address";

// EspClient'i başlat.

WiFiClient espClient;
PubSubClient client(espClient);

// ESP8266'mızın her bir GPIO'suna bir LED bağladım.
const int ledGPIO5 = 5;
const int ledGPIO4 = 4;

// DHT Sensör
const int DHTPin = 14;

// DHT sensörünü başlat.
DHT dht(DHTPin, DHTTYPE);

// Değişken yardımcı zamanlayıcılar
long now = millis();
long lastMeasure = 0;

// Aşağıdaki fonksiyon sabit olarak kalacak. Bu fonsiyonlar ESP8266'mizi yönlendiricimize bağlar.
void setup_wifi() {
  delay(10);
// Bir WiFi ağına bağlanarak başlıyoruz
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected - ESP IP address: ");
  Serial.println(WiFi.localIP());}

// Bu fonsiyon, bazı aygıtlar, ESP8266'mızın abone olduğu bir konuya bir mesaj yayınladığında yürütülür.
// Programımıza mantık eklemek için aşağıdaki fonsiyonu değiştirmeliyiz.Böylece bir cihaz bir konuya bir mesaj yayınladığında ESP8266’mız ile abone olabiliriz.

void callback(String topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // MQTT ile daha fazla GPIO'yu kontrol etmek için daha fazla  şey ekleyebiliriz.

   // home / office / esp1 / gpio2 konusuna bir mesaj geldiyse, mesajın 1 veya 0 olup olmadığını kontrol ederiz. ESP GPIO mesaja göre döner.

  if(topic=="esp8266/4"){
      Serial.print("Changing GPIO 4 to ");
      if(messageTemp == "1"){
        digitalWrite(ledGPIO4, HIGH);
        Serial.print("On");
      }
      else if(messageTemp == "0"){
        digitalWrite(ledGPIO4, LOW);
        Serial.print("Off");
      }
  }
  if(topic=="esp8266/5"){
      Serial.print("Changing GPIO 5 to ");
      if(messageTemp == "1"){
        digitalWrite(ledGPIO5, HIGH);
        Serial.print("On");
      }
      else if(messageTemp == "0"){
        digitalWrite(ledGPIO5, LOW);
        Serial.print("Off");
      }
  }
  Serial.println();
}

// Bu işlevler ESP8266'nızı MQTT aracımıza bağlar.
// ESP8266'mızla daha fazla konuya abone olmak istiyorsak aşağıdaki fonsiyonları değiştirmeliyiz.

void reconnect() {
  // Yeniden bağlantı kuruluncaya kadar döngü yap
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Bağlanmayı dene
      / *
      MQTT ÇOKLU BAĞLANTILARLA SORUNSUZ OLURSA BU SONRAKİ HATTI DEĞİŞTİRMELİYİZ.
      ESP cihaz kimliğini değiştirmek için ESP8266'ya benzersiz bir isim vermemiz gerekecektir.
      İşte şimdi nasıl görünüyor:
        if (client.connect ("ESP8266Client")) {
      MQTT aracısına bağlı daha fazla cihaz istiyorsak, bunu şu şekilde yapabiliriz:
        if (client.connect ("ESPOffice")) {
      Sonra, diğer ESP için:
        if (client.connect ("ESPGarage")) {
       Bu MQTT çoklu bağlantı sorununuzu çözmek için
      Loop IN loop () işlevi cihazınızın adıyla eşleşmelidir.
     * /   
 if (client.connect("ESP8266Client")) {
      Serial.println("connected"); 
 
      // Bir konuya abone olup veya tekrar kaydolmalıyız.
       // Daha fazla konuya abone olabiliriz (bu örnekte daha fazla LED'yi kontrol etmek için)

      client.subscribe("esp8266/4");
      client.subscribe("esp8266/5");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Yeniden denemeden önce 5 saniye bekle.
      delay(5000);
    }
  }}

// Kurulum işlevi, ESP GPIO'larımızı Çıkışlara ayarlar, 115200 baud hızında seri iletişimi başlatır.
// Mqtt aracısını ayarlar ve geri arama işlevini ayarlar
// Geri arama işlevi, iletileri alan ve LED'leri gerçekten kontrol eden şeydir.
void setup() {
  dht.begin();
  pinMode(ledGPIO4, OUTPUT);
  pinMode(ledGPIO5, OUTPUT);
  
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

// Bu proje için, döngü işlevindeki herhangi bir şeyi değiştirmemize gerek yoktur.
// Temel olarak, ESP'nin brokerimize bağlı olmasını sağlar.

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  if(!client.loop())
     /*
     MQTT ÇOKLU BAĞLANTILARLA SORUNSUZ OLURSA BU SONRAKİ HATTI DEĞİŞTİRMELİYİZ.
      ESP cihaz kimliğini değiştirmek için ESP8266'ya benzersiz bir isim vermemiz gerekecektir.
      İşte şimdi nasıl görünüyor:
        client.connect ( "ESP8266Client");
      MQTT aracısına bağlı daha fazla cihaz istiyorsak, bunu şu şekilde yapabiliriz:
        client.connect ( "ESPOffice");
      Sonra, diğer ESP için:
        client.connect ( "ESPGarage");
       Bu MQTT çoklu bağlantı sorunumuzu çözmek için
      Recionnect () işlevindeki bölüm, aygıt adınızla eşleşmelidir.    */

    client.connect("ESP8266Client");
    
  now = millis();
  // Her 10 saniyede yeni sıcaklık ve nem yayınlar
  if (now - lastMeasure > 10000) {
    lastMeasure = now;
    // Sensör okumaları da 2 saniyeye kadar eski olabilir (çok yavaş bir sensördür)
    float h = dht.readHumidity();
    // Santigrat olarak sıcaklığı okuyoruz. (varsayılan)
    float t = dht.readTemperature();
    // Fahrenhayt olarak sıcaklığı oku (isFahrenheit = true)   
 float f = dht.readTemperature(true);

    // Herhangi bir okuma başarısız olup olmadığını kontrol etmek için (tekrar denemek için).
    if (isnan(h) || isnan(t) || isnan(f)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }

//Bu kısımda pinlerdeki sıcaklık değerleri belirleyerek,istediğimiz sıcaklığın altında bir uyarı vermesini sağlayabliriz.Ben burada 5.pinde,25 derecenin altında led’in yanmasını istedim.


}

    if (t < 25) {
      digitalWrite(ledGPIO5, HIGH);
        Serial.print("On");
 }
    // Celsius'ta sıcaklık değerlerini hesaplar
    float hic = dht.computeHeatIndex(t, h, false);
    static char temperatureTemp[7];
    dtostrf(hic, 6, 2, temperatureTemp);
    
    // Fahrenhayt'ta sıcaklık değerlerini hesaplamak için 
    // float hif = dht.computeHeatIndex(f, h);
    // static char temperatureTemp[7];
    // dtostrf(hic, 6, 2, temperatureTemp);
    
    static char humidityTemp[7];
    dtostrf(h, 6, 2, humidityTemp);

    // Sıcaklık ve Nem değerlerini yayınlar
    client.publish("/esp8266/temperature", temperatureTemp);
    client.publish("/esp8266/humidity", humidityTemp);
    
    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.print(" %\t Temperature: ");
    Serial.print(t);
    Serial.print(" *C ");
    Serial.print(f);
    Serial.print(" *F\t Heat index: ");
    Serial.print(hic);
    Serial.println(" *C ");
    // Serial.print(hif);
    // Serial.println(" *F");
  }
}

