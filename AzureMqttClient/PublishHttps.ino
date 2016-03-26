int sendCount = 0;
char buffer[256];

void connectToAzure() {
  delay(500); // give network connection a moment to settle
  Serial.print(cloud.id);
  Serial.print(" connecting to ");
  Serial.println(cloud.host);
  if (WiFi.status() != WL_CONNECTED) { return; }
  if (!tlsClient.connect(cloud.host, 443)) {      // Use WiFiClientSecure class to create TLS connection
    Serial.print("Host connection failed.  WiFi IP Address: ");
    Serial.println(WiFi.localIP());

    delay(2000);
  }
  else {
    Serial.println("Host connected");
    yield(); // give firmware some time 
//    delay(250); // give network connection a moment to settle
  }
}

String buildHttpRequest(String data){  
  return "POST " + cloud.endPoint + " HTTP/1.1\r\n" +
    "Host: " + cloud.host + "\r\n" +
    "Authorization: SharedAccessSignature " + cloud.fullSas + "\r\n" +
    "Content-Type: application/atom+xml;type=entry;charset=utf-8\r\n" +
    "Content-Length: " + data.length() + "\r\n\r\n" + data;
}

void publishToAzure() {
  int bytesWritten = 0;

  // https://msdn.microsoft.com/en-us/library/azure/dn790664.aspx  

  initialiseAzure();
  
  if (!tlsClient.connected()) { connectToAzure(); }
  if (!tlsClient.connected()) { return; }
  
  setLedState(Off);

  tlsClient.flush();

  bytesWritten = tlsClient.print(buildHttpRequest(serializeData(data))); 
  
  String response = "";
  String chunk = "";
  int limit = 1;
  
  do {
    if (tlsClient.connected()) { 
      yield();
      chunk = tlsClient.readStringUntil('\n');
      response += chunk;
    }
  } while (chunk.length() > 0 && ++limit < 100);  

  Serial.print("Bytes sent ");
  Serial.print(bytesWritten);
  Serial.print(", Memory ");
  Serial.print(ESP.getFreeHeap());
  Serial.print(" Message ");
  Serial.print(sendCount);
  Serial.print(", Response chunks ");
  Serial.print(limit);
  Serial.print(", Response code: ");
  
  if (response.length() > 12) { Serial.println(response.substring(9, 12)); }
  else { Serial.println("unknown"); }

  setLedState(On);
}




