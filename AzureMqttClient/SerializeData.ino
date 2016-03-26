String serializeData(SensorData data){
  StaticJsonBuffer<JSON_OBJECT_SIZE(16)> jsonBuffer;  //  allow for a few extra json fields that actually being used at the moment
  JsonObject& root = jsonBuffer.createObject();

  root["Dev"] = cloud.id;
  root["Utc"] = GetISODateTime();
  root["Celsius"] = data.temperature;
  root["Humidity"] = data.humidity;
  root["hPa"] = data.pressure;
  root["Light"] = data.light;
  root["Geo"] = cloud.geo;  

  //instrumentation
  root["WiFi"] = device.WiFiConnectAttempts;
  root["Mem"] = ESP.getFreeHeap();
  root["Id"] = ++sendCount;

  root.printTo(buffer, sizeof(buffer));

  return (String)buffer;
}
