import paho.mqtt.client as mqtt
from flask import Flask, render_template, request
from flask_socketio import SocketIO, emit

app = Flask(__name__)
app.config['SECRET_KEY'] = 'secret!'
socketio = SocketIO(app)

# İstemcinin sunucudan bir CONNACK yanıtı aldığı zaman için geri dönüş yapması.

def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))

  # On_connect () 'de abone ol ve bağlantıyı kaybedersek yeniden bağlan, abonelikleri yenile.

  client.subscribe("/esp8266/temperature")
    client.subscribe("/esp8266/humidity")

# Bir PUBLISH mesajının ESP8266'dan alındığı zaman geri dönüş yapması.
def on_message(client, userdata, message):
    #socketio.emit('my variable')
    print("Received message '" + str(message.payload) + "' on topic '"
        + message.topic + "' with QoS " + str(message.qos))
    if message.topic == "/esp8266/temperature":
        print("temperature update")
        socketio.emit('dht_temperature', {'data': message.payload})
    if message.topic == "/esp8266/humidity":
        print("humidity update")
        socketio.emit('dht_humidity', {'data': message.payload})

mqttc=mqtt.Client()
mqttc.on_connect = on_connect
mqttc.on_message = on_message
mqttc.connect("localhost",1883,60)
mqttc.loop_start()

# Pin numarasını, adını ve pin durumunu saklamak için pinler denen bir değişken oluşturdum:
pins = {
   4 : {'name' : 'GPIO 4', 'board' : 'esp8266', 'topic' : 'esp8266/4', 'state' : 'False'},
   5 : {'name' : 'GPIO 5', 'board' : 'esp8266', 'topic' : 'esp8266/5', 'state' : 'False'}
   }

# Pin sözlüğünü templateData değişkenine yerleştirdim:
templateData = {
   'pins' : pins
   }

@app.route("/")
def main():

   # template verileri template main.html'ye aktarın ve kullanıcıya döndürdüm.

   return render_template('main.html', async_mode=socketio.async_mode, **templateData)

# Aşağıdaki fonksiyon, bir kullanıcı pin numarası ve eylemi içeren bir URL istediğinde gerçekleştirilir:


@app.route("/<board>/<changePin>/<action>")
def action(board, changePin, action):

   # Pimi URL'den bir tam sayıya dönüştürdüm:

   changePin = int(changePin)

   # Değiştirilen pin için cihaz adını aldım:

   devicePin = pins[changePin]['name']

   # URL'nin işlem kısmı "1" ise, aşağıda girilen kodu yürüt:

   if action == "1" and board == 'esp8266':
      mqttc.publish(pins[changePin]['topic'],"1")
      pins[changePin]['state'] = 'True'
   if action == "0" and board == 'esp8266':
      mqttc.publish(pins[changePin]['topic'],"0")
      pins[changePin]['state'] = 'False'

   # Pin sözlüğü ile birlikte, mesajı templateData’ya değişkenine yerleştir:

   templateData = {
      'pins' : pins
   }
   return render_template('main.html', **templateData)

@socketio.on('my event')
def handle_my_custom_event(json):
    print('received json data here: ' + str(json))

if __name__ == "__main__":
   socketio.run(app, host='0.0.0.0', port=8181, debug=True)

