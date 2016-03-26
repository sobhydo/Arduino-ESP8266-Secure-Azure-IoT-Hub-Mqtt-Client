#include <PubSubClient.h>  // https://github.com/Imroy/pubsubclient


PubSubClient client(tlsClient, "MakerDen.azure-devices.net", 8883);
int mqttMsgCount = 0;

const char clientId[] = "MakerDen.azure-devices.net/MqttDevice";
const char clientPwd[] = "SharedAccessSignature sr=MakerDen.azure-devices.net%2fdevices%2fMqttDevice&sig=1yDo8Nf5xUYk4upcU5%2flh25HGTaWGmc6rk6kSoidkLE%3d&se=1466676592";
const char MQTT_SUBSCRIBE_TOPIC[] = "devices/MqttDevice/messages/devicebound/#";
const int MQTTKEEPALIVE = 1000 * 60 * 5; // 5 minutes
#define BUFFER_SIZE 100

void callback(const MQTT::Publish& pub) {
Serial.print(pub.topic());
  Serial.println(" => ");
  if (pub.has_stream()) {
    uint8_t buf[BUFFER_SIZE];
    int read;
    while (read = pub.payload_stream()->read(buf, BUFFER_SIZE)) {
      Serial.write(buf, read);
    }
    pub.payload_stream()->stop();
    Serial.println("");
  } else {
    Serial.println("not a stream");
    Serial.println(pub.payload_string());
  }
}

void InitMqtt(){
  if (!client.connected()) {      
    Serial.println("Connecting to MQTT server");
    
    if (client.connect(MQTT::Connect("MqttDevice")
        .set_keepalive(MQTTKEEPALIVE)
        .set_clean_session()
        .set_auth(clientId, clientPwd))) {        
        
        Serial.println("Connected to MQTT server");
        
        client.set_callback(callback);

        client.subscribe(MQTT::Subscribe().add_topic(MQTT_SUBSCRIBE_TOPIC));

    } else {
      Serial.println("Could not connect to MQTT server");   
    }
  }    
}

void publishOverMqtt(){
  InitMqtt();
  if (client.connected()){
    client.publish(MQTT::Publish("devices/MqttDevice/messages/events/", serializeData(data))
                    .set_qos(1)
                   );

    Serial.println("Published: " + String(++mqttMsgCount));
    client.loop();
  } else {
    Serial.println("Mqtt not connected");
  }
}

