#include <Arduino.h>
#include <SPI.h>
#include <RF24.h>

static const uint8_t PIN_CE = 1;
static const uint8_t PIN_CSN = 2;
static const uint8_t PIN_SCK = 7;
static const uint8_t PIN_MISO = 8;
static const uint8_t PIN_MOSI = 9;

static RF24 radio(PIN_CE, PIN_CSN);

void setup() {
	Serial.begin(115200);
	delay(200);
	Serial.println("tinycars-analyzer");

	SPI.begin(PIN_SCK, PIN_MISO, PIN_MOSI, PIN_CSN);
	if (!radio.begin()) {
		Serial.println("NRF24: нет ответа по SPI");
		return;
	}

	radio.setPALevel(RF24_PA_LOW);
	radio.setDataRate(RF24_1MBPS);
	radio.startListening();
	Serial.println("NRF24: приём");
}

void loop() {
}
