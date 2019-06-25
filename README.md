# MQTT-Protokolu-ile-RaspberryPi-Uygulamasi

## DHT22 İle MQTT protokolü'nün  Raspberry Pi üzerinde kullanılması

  Bu projede, bir DHT22 sensörü ile sıcaklık ve nem değerlerini gösteren bir Raspberry Pi ile bağımsız bir web sunucusu oluşturacağız. Ayrıca MQTT protokolünü kullanarak bir ESP8266'dan iki çıkışı kontrol edeceğiz.
 
  Yapacağım işlemler için Raspberry Pi’de Raspbian işletim sistemini kurdum.Sonrasında Raspberry Pi’yi uzaktan kontrol etmek için Putty’i kurarak,Raspberry Pi’nin terminalini kullandım.
  
  Terminal üzerinde ilk yaptığım işlem ESP8266 ile MQTT protokolünü uygulayabilmek Mosquitto broker kurdum.Bunun için farklı brokerlar kullanılabilir.Ben daha çok tercih edilmesi ve kaynak açısından Eclipse’in Mosquittosunu tercih ettim.
 
Kurmak için terminal’e yazdığım kod:

### pi@raspberry:~ $ mosquitto -d

  Raspberry Pi'yi web sunucusuna dönüştürmek için Flask adlı bir Python Mikroframework kullandım.
Flask'ı yüklemek için pip yüklememiz gerekir. Pi'mizi güncellemek ve pip güncellemek için aşağıdaki komutları çalıştırdım:

### pi@raspberrypi ~ $ sudo apt-get update
### pi@raspberrypi ~ $ sudo apt-get upgrade
### pi@raspberrypi ~ $ sudo apt-get install python-pip python-flask git-core


  Bu proje, Python Flask uygulamamız tarafından eşzamansız olarak güncellenebilen bir Python Flask web sayfası oluşturmamıza olanak veren SocketIO'yu kullanacağız. Bu, en son okumaları görmek için web sayfasını yenilememiz gerekmediği anlamına gelir, anında güncellenir. Flask SocketIO Python paketini kurmak için aşağıdaki kodu çalıştırdım.
  
 ### pi@raspberrypi ~ $ sudo pip install flask-socketio

  Sonrasında sıcaklık ve nem değerlerini kontrol etmek için bir tane Pyhton dosyası oluşturacağız.Bu dosya ile Web sunucusunu ayarlayıp ve bu düğmelere basıldığında ESP8266'dan bir MQTT mesajı yayınlar. Ayrıca okumaları almak için (sıcaklık ve nem  değerleri )MQTT ile abone oluşturacağız.

Bunun için bir klasör oluşturdum:

### pi@raspberrypi ~ $ mkdir web-server
### pi@raspberrypi ~ $ cd web-server
### pi@raspberrypi:~/web-server $

Sonrasında kodları app.py adında bir dosya oluşturdum:

### pi@raspberrypi:~/web-server $ nano app.py

Sonrasında verileri yayınlamak için  gerekli kodları [app.py](https://github.com/fatihawk/MQTT-Protokolu-ile-RaspberryPi-Uygulamasi/blob/master/app.py)’nin içine attım.

  Python scriptimizden ayrılan HTML etiketlerinin tutulması, projemizin nasıl organize edildiğidir. Flask, Python scriptimizden dinamik dosyamıza HTML dosyasını göndermek için kullanabileceğimiz bir Jinja2 adında bir şablon motoru kullanır.

Template adı verilen yeni bir klasör oluşturdum:

### pi@raspberrypi:~/web-server $ mkdir templates
### pi@raspberrypi:~/web-server $ cd templates
### pi@raspberrypi:~/web-server/templates $

Sonrasında main.html adında bir HTML dosyası oluşturdum:

### pi@raspberrypi:~/web-server/templates $ nano main.html

Sonrasında uygulamanın çalışacağı arayüz için gerekli kodları [main.html](https://github.com/fatihawk/MQTT-Protokolu-ile-RaspberryPi-Uygulamasi/blob/master/main.html) içine attım.

  ESP8266'nın Raspberry Pi web sunucusuyla etkileşime girmesi için PubSubClient kütüphanesini kurmamız gerekir. Bu kütüphane, MQTT'yi destekleyen bir sunucu ile basit yayınlama / abone olma mesajlaşma için bir istemci sağlar (temel olarak ESP8266'nızın Python web sunucusu ile konuşmasına izin verir).

PubSubClient kitaplığını yükleme

1) PubSubClient kütüphanesini indirmek için,
 https://github.com/knolleary/pubsubclient/archive/master.zip  adresine girip buradan indirebiliriz. İndirilenler klasörümüzde bir .zip klasörümüz olmalıdır.

2) .zip klasörünü çıkardıktan sonra pubsubclient-master klasörünü alacağız.

3) Klasörün adını pubsubclient-master'dan pubsubclient'e çevireceğiz.Çünkü bu klasör abone-yayıncı işlemi için kullanılan kütüphane.

4) pubsubclient klasörünü Arduino IDE kurulum kütüphaneleri klasörüne yerleştirdim.

Kütüphane bir dizi örnek program ile gelir. Arduino IDE yazılımında Dosya> Örnekler> PubSubClient'e bakmak gerekir.

  DHT sensörü kütüphanesi, ESP8266 veya Arduino kartlarınızla sıcaklık ve nemi okumak için herhangi bir DHT sensörünü kullanmanın kolay bir yolunu sunar.Aşağıdaki adımlarda nasıl indirilebileceğini görebiliriz:

1) DHT sensör kütüphanesini indirmek için DHT sensörü kütüphanesi, ESP8266 veya Arduino kartlarınızla sıcaklık ve nemi okumak için herhangi bir DHT sensörünü kullanmanın kolay bir yolunu sunar.

1) DHT sensör kütüphanesini indirmek için https://github.com/adafruit/DHT-sensor-library girip indirdim.  İndirilenler klasörünüzde bir .zip klasörünüz olmalıdır

2) .zip klasörünü açtıktan sonra DHT-sensor-library-master klasörünü çıkartacağız.

3) Klasörümüzü DHT-sensor-library-master'dan DHT'ye yeniden adlandırdım.

4) DHT klasörünü Arduino IDE kurulum kütüphanelerine taşıdım.

5) Arduino IDE'mizi tekrar açtıktan sonra indirilenler klasörümüzde bir .zip klasörümüz olmalıdır.

Son olarak, ESP8266'mıza tam çizimi yükleyebiliriz (SSID, şifre ve RPi IP adresim ile değiştirdim.).

Bunun için [PubSubClient.ino](https://github.com/fatihawk/MQTT-Protokolu-ile-RaspberryPi-Uygulamasi/blob/master/PubSubClient.ino) adlı bir dosya oluşturup kodları Arduino IDE içerisine attım.

Bu proje için kullandığımız şematik ise şöyle:

![alt text](https://github.com/fatihawk/MQTT-Protokolu-ile-RaspberryPi-Uygulamasi/blob/master/%C5%9EematikBa%C4%9Flant%C4%B1lar.png)

Kullanacağımız malzemeler ise:

* 1x ESP8266 12E 

* 1x DHT22 Sensör 

* 1x 4700 Ohm Direnç

* 2x 470 Ohm Direnç

* 2x adet LED


Raspberry Pi web sunucunuzu başlatmak için app.py dosyasını içeren klasöre geçtim:

### pi@raspberrypi:~/web-server/templates $ cd ..

Ardından, aşağıdaki komutu çalıştırdım:

### pi@raspberrypi:~/web-server $ sudo python app.py

IP adresimi girerek tarayıcımda Raspberry Pi'mi açtım:

![alt text](https://github.com/fatihawk/MQTT-Protokolu-ile-RaspberryPi-Uygulamasi/blob/master/Sonu%C3%A7.jpg)









 




