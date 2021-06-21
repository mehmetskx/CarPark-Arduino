#include <SoftwareSerial.h>
#include <dht11.h>

String agAdi = "my";                    
String agSifresi = "12345678"; 

int rxPin = 10;                                              
int txPin = 11;  
int dht11Pin = 2;

String ip = "184.106.153.149";

const int trigPin1 = 5, echoPin1 = 6;
long uzaklik1;
long sure1;
const int trigPin2 = 3, echoPin2 = 4;
long uzaklik2;
long sure2;

#define DEBUG true
dht11 DHT11;

SoftwareSerial esp(rxPin, txPin);

void setup() {
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);

  Serial.begin(9600);  //Seri port ile haberleşmemizi başlatıyoruz.
  Serial.println("Started");
  //esp.begin(115200);
  esp.begin(9600);
  esp.println("AT");
  Serial.println("AT Yollandı");
  while (!esp.find("OK")) {
    esp.println("AT");
    Serial.println("ESP8266 Bulunamadı.");
  }
  //  esp.println("AT+UART_DEF=9600,8,1,0,0");
  Serial.println("OK Komutu Alındı");
  esp.println("AT+CWMODE=1");
  while (!esp.find("OK")) {
    esp.println("AT+CWMODE=1");
    Serial.println("Ayar Yapılıyor....");
  }
  Serial.println("Client olarak ayarlandı");
  Serial.println("Aga Baglaniliyor...");
  esp.println("AT+CWJAP=\"" + agAdi + "\",\"" + agSifresi + "\"");
  while (!esp.find("OK"));
  Serial.println("Aga Baglandi.");
  delay(1000);
}
void loop() {
  esp.println("AT+CIPSTART=\"TCP\",\"" + ip + "\",80");
  if (esp.find("Error")) {
    Serial.println("AT+CIPSTART Error");
  }
  DHT11.read(dht11Pin);
  //Park alanı 1
  digitalWrite(trigPin1, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin1, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin1, LOW);
  sure1 = pulseIn(echoPin1, HIGH);
  uzaklik1 = (sure1 / 2 / 29.1);
  //Park alanı 2
  digitalWrite(trigPin2, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin2, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin2, LOW);
  sure2 = pulseIn(echoPin2, HIGH);
  uzaklik2 = (sure2 / 2 / 29.1);

  String veri = "GET https://api.thingspeak.com/update?api_key=HMCI7YM1HAHF2WTW";   //Thingspeak
  if (uzaklik1 > 10)
    veri += "&field1=0";
  else
    veri += "&field1=1";

  if (uzaklik2 > 10)
    veri += "&field2=0";
  else
    veri += "&field2=1";

  veri += "\r\n\r\n";
  esp.print("AT+CIPSEND=");
  esp.println(veri.length() + 2);
  delay(2000);
  if (esp.find(">")) {
    esp.print(veri);
    Serial.println(veri);
    Serial.println("Veri gonderildi.");
    delay(1000);
  }
  Serial.println("Baglantı Kapatildi.");
  esp.println("AT+CIPCLOSE");
  delay(1000);
}
