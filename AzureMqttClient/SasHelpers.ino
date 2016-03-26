#include "sha256.h"
#include "Base64.h"

bool azureInitialised = false;

void initialiseAzure(){
  if (azureInitialised) { return; }
  
  switch(cloud.cloudMode){
    case IoTHub:
      initialiseIotHub();
      break;
    case EventHub:
      initialiseEventHub();
      break;
  }

  azureInitialised = true;
}

// Azure IoT Hub Settings
const char* TARGET_URL = "/devices/";
const char* IOT_HUB_END_POINT = "/messages/events?api-version=2015-08-15-preview";

// Azure Event Hub settings
const char* EVENT_HUB_END_POINT = "/ehdevices/publishers/nodemcu/messages";

void initialiseIotHub(){
  String url = urlEncode(cloud.host) + urlEncode(TARGET_URL) + (String)cloud.id;
  cloud.endPoint = (String)TARGET_URL + (String)cloud.id + (String)IOT_HUB_END_POINT;
  cloud.fullSas =  createIotHubSas(cloud.key, url);
}

void initialiseEventHub() {
  String url = urlEncode("https://")  + urlEncode(cloud.host) + urlEncode(EVENT_HUB_END_POINT);
  cloud.endPoint = EVENT_HUB_END_POINT;
  cloud.fullSas = createEventHubSas(cloud.key, url);
}

String createIotHubSas(char *key, String url){  
  String stringToSign = url + "\n" + cloud.sasExpiryDate;

  // START: Create signature
  // https://raw.githubusercontent.com/adamvr/arduino-base64/master/examples/base64/base64.ino
  
  int keyLength = strlen(key);
  
  int decodedKeyLength = base64_dec_len(key, keyLength);
  char decodedKey[decodedKeyLength];  //allocate char array big enough for the base64 decoded key
  
  base64_decode(decodedKey, key, keyLength);  //decode key
  
  Sha256.initHmac((const uint8_t*)decodedKey, decodedKeyLength);
  Sha256.print(stringToSign);  
  char* sign = (char*) Sha256.resultHmac();
  // END: Create signature
  
  // START: Get base64 of signature
  int encodedSignLen = base64_enc_len(HASH_LENGTH);
  char encodedSign[encodedSignLen];
  base64_encode(encodedSign, sign, HASH_LENGTH); 
  
  // SharedAccessSignature
  return "sr=" + url + "&sig="+ urlEncode(encodedSign) + "&se=" + cloud.sasExpiryDate;
  // END: create SAS  
}

String createEventHubSas(char *key, String url){  
  // START: Create SAS  
  // https://azure.microsoft.com/en-us/documentation/articles/service-bus-sas-overview/
  // Where to get seconds since the epoch: local service, SNTP, RTC

  String stringToSign = url + "\n" + cloud.sasExpiryDate;

  // START: Create signature
  Sha256.initHmac((const uint8_t*)key, 44);
  Sha256.print(stringToSign);
  
  char* sign = (char*) Sha256.resultHmac();
  int signLen = 32;
  // END: Create signature

  // START: Get base64 of signature
  int encodedSignLen = base64_enc_len(signLen);
  char encodedSign[encodedSignLen];
  base64_encode(encodedSign, sign, signLen);   
  // END: Get base64 of signature

  // SharedAccessSignature
  return "sr=" + url + "&sig="+ urlEncode(encodedSign) + "&se=" + cloud.sasExpiryDate +"&skn=" + cloud.id;
  // END: create SAS
}
  
