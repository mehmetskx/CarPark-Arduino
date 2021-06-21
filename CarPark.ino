#include <SoftwareSerial.h>                                   //SoftwareSerial kütüphanimizi ekliyoruz.
#include <dht11.h>                                            //DHT11 sensör kütüphanemizi ekliyoruz.

String agAdi = "my";                 //Ağımızın adını buraya yazıyoruz.    
String agSifresi = "12345678";           //Ağımızın şifresini buraya yazıyoruz.

int rxPin = 10;                                               //ESP8266 RX pini
int txPin = 11;                                               //ESP8266 TX pini
int dht11Pin = 2;

String ip = "184.106.153.149";                                //Thingspeak ip adresi
float sicaklik, nem;
#define DEBUG true   
dht11 DHT11;

SoftwareSerial esp(rxPin, txPin);                             //Seri haberleşme pin ayarlarını yapıyoruz.

void setup() {  
  
  Serial.begin(9600);  //Seri port ile haberleşmemizi başlatıyoruz.
  Serial.println("Started"); 
  //esp.begin(115200);                                          //ESP8266 ile seri haberleşmeyi başlatıyoruz.
   esp.begin(9600);
  esp.println("AT");                                          //AT komutu ile modül kontrolünü yapıyoruz.
  Serial.println("AT Yollandı");
  while(!esp.find("OK")){                                     //Modül hazır olana kadar bekliyoruz.
    esp.println("AT");
    Serial.println("ESP8266 Bulunamadı.");
  }
 //  esp.println("AT+UART_DEF=9600,8,1,0,0"); 
  Serial.println("OK Komutu Alındı");
  esp.println("AT+CWMODE=1");                                 //ESP8266 modülünü client olarak ayarlıyoruz.
  while(!esp.find("OK")){                                     //Ayar yapılana kadar bekliyoruz.
    esp.println("AT+CWMODE=1");
    Serial.println("Ayar Yapılıyor....");
  }
  Serial.println("Client olarak ayarlandı");
  Serial.println("Aga Baglaniliyor...");
  esp.println("AT+CWJAP=\""+agAdi+"\",\""+agSifresi+"\"");    //Ağımıza bağlanıyoruz.
  while(!esp.find("OK"));                                     //Ağa bağlanana kadar bekliyoruz.
  Serial.println("Aga Baglandi.");
  delay(1000);
}
void loop() {
  esp.println("AT+CIPSTART=\"TCP\",\""+ip+"\",80");           //Thingspeak'e bağlanıyoruz.
  if(esp.find("Error")){                                      //Bağlantı hatası kontrolü yapıyoruz.
    Serial.println("AT+CIPSTART Error");
  }
  DHT11.read(dht11Pin);
  sicaklik = (float)DHT11.temperature;
  nem = (float)DHT11.humidity;
  String veri = "GET https://api.thingspeak.com/update?api_key=HMCI7YM1HAHF2WTW";   //Thingspeak komutu. Key kısmına kendi api keyimizi yazıyoruz.                                   //Göndereceğimiz sıcaklık değişkeni
  veri += "&field1=123";
 // veri += String("22");
  veri += "&field2=321";
//  veri += String("3232");                                        //Göndereceğimiz nem değişkeni
  veri += "\r\n\r\n"; 
  esp.print("AT+CIPSEND=");                                   //ESP'ye göndereceğimiz veri uzunluğunu veriyoruz.
  esp.println(veri.length()+2);
  delay(2000);
  if(esp.find(">")){                                          //ESP8266 hazır olduğunda içindeki komutlar çalışıyor.
    esp.print(veri);                                          //Veriyi gönderiyoruz.
    Serial.println(veri);
    Serial.println("Veri gonderildi.");
    delay(1000);
  }
  Serial.println("Baglantı Kapatildi.");
  esp.println("AT+CIPCLOSE");                                //Bağlantıyı kapatıyoruz
  delay(1000);                                               //Yeni veri gönderimi için 1 dakika bekliyoruz.
}
