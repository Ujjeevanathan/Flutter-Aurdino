#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

// Service name to the broadcasted to outside world
# define LED_BUILTIN 2

#define PERIPHERAL_NAME "IoT Device"
#define SERVICE_UUID "CD9CFC21-0ECC-42E5-BF22-48AA715CA112"
#define CHARACTERISTIC_INPUT_UUID "66E5FFCE-AA96-4DC9-90C3-C62BBCCD29AC"
#define CHARACTERISTIC_OUTPUT_UUID "142F29DD-B1F0-4FA8-8E55-5A2D5F3E2471"
static const char* greeting =  "";
// Output characteristic is used to send the response back to the connected phone
BLECharacteristic *pOutputChar;

// Class defines methods called when a device connects and disconnects from the service
class ServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
        Serial.println("BLE Client Connected");
    }
    void onDisconnect(BLEServer* pServer) {
        BLEDevice::startAdvertising();
        Serial.println("BLE Client Disconnected");
    }
};

class InputReceivedCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharWriteState) {
        std::string inputValue = pCharWriteState->getValue();
        
        if (inputValue.length() > 0) {
          Serial.print("Received Value: ");
          for (int i = 0; i < inputValue.length(); i++) {
            Serial.print(inputValue[i]);
          }
          Serial.println();
          
          // Send data to client
          delay(1000);
          std::string outputData = "Last received: " + inputValue;
          pOutputChar->setValue(outputData);
          pOutputChar->notify();
        }
        
        if(pCharWriteState->getValue() == "off")
        {
          digitalWrite(LED_BUILTIN, LOW);
        }

        if(pCharWriteState->getValue() == "on")
        {
          digitalWrite(LED_BUILTIN, HIGH);
        }

    }
};

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  // Initialize BLE
  BLEDevice::init(PERIPHERAL_NAME);

  // Create the server
  BLEServer *pServer = BLEDevice::createServer();
  
  // Create the service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Handle inputs (sent from app)
  BLECharacteristic *pInputChar 
      = pService->createCharacteristic(
                    CHARACTERISTIC_INPUT_UUID,                                        
                    BLECharacteristic::PROPERTY_WRITE_NR |
                    BLECharacteristic::PROPERTY_WRITE);

  pOutputChar = pService->createCharacteristic(
                        CHARACTERISTIC_OUTPUT_UUID,
                        BLECharacteristic::PROPERTY_READ |
                        BLECharacteristic::PROPERTY_NOTIFY);

  pServer->setCallbacks(new ServerCallbacks());                  
  pInputChar->setCallbacks(new InputReceivedCallbacks());
  
  // Start service
  pService->start();

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  BLEDevice::startAdvertising();
}

void loop() { }