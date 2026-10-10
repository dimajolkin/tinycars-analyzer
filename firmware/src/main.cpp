#include <Arduino.h>
#include <SPI.h>
#include <RF24.h>
#include <string.h>

static const uint8_t PIN_CE = 1;
static const uint8_t PIN_CSN = 2;
static const uint8_t PIN_SCK = 7;
static const uint8_t PIN_MISO = 8;
static const uint8_t PIN_MOSI = 9;

static const uint8_t PAYLOAD = 32;
static const uint8_t PREFIX = 5;
static const uint8_t CHANNELS = 126;
static const uint16_t DWELL_MS = 20;
static const uint8_t STAY_LIMIT = 40;

static const uint8_t ADDR_AA[2] = {0xAA, 0xAA};
static const uint8_t ADDR_55[2] = {0x55, 0x55};

static const rf24_datarate_e RATES[] = {RF24_1MBPS, RF24_2MBPS, RF24_250KBPS};
static const char *RATE_NAME[] = {"1M", "2M", "250K"};

static RF24 radio(PIN_CE, PIN_CSN);
static bool radioOk = false;
static uint8_t channel = 0;
static uint8_t rateIndex = 0;
static uint8_t stay = 0;

static void printPacket(const uint8_t *data) {
	Serial.print("C=");
	Serial.print(channel);
	Serial.print(' ');
	Serial.print(RATE_NAME[rateIndex]);
	for (uint8_t i = 0; i < PAYLOAD; i++) {
		Serial.print(' ');
		if (data[i] < 0x10) {
			Serial.print('0');
		}
		Serial.print(data[i], HEX);
	}
	Serial.println();
}

static void applyListen() {
	radio.stopListening();
	radio.setAutoAck(false);
	radio.setRetries(0, 0);
	radio.disableCRC();
	radio.setAddressWidth(2);
	radio.setPayloadSize(PAYLOAD);
	radio.setDataRate(RATES[rateIndex]);
	radio.setChannel(channel);
	radio.openReadingPipe(0, ADDR_AA);
	radio.openReadingPipe(1, ADDR_55);
	radio.flush_rx();
	radio.startListening();
}

static void hop() {
	channel++;
	if (channel >= CHANNELS) {
		channel = 0;
		rateIndex = (rateIndex + 1) % 3;
	}

	radio.stopListening();
	if (!radio.setDataRate(RATES[rateIndex])) {
		rateIndex = 0;
		radio.setDataRate(RATES[rateIndex]);
	}
	if (channel == 0) {
		Serial.print("scan ");
		Serial.println(RATE_NAME[rateIndex]);
	}
	radio.setChannel(channel);
	radio.flush_rx();
	radio.startListening();
}

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
	applyListen();
	radioOk = true;
	Serial.println("scan 1M");
}

void loop() {
	if (!radioOk) {
		return;
	}

	uint8_t prefix[PREFIX] = {};
	uint8_t hits = 0;
	uint8_t shown = 0;
	uint32_t until = millis() + DWELL_MS;

	while ((int32_t)(until - millis()) > 0 && shown < 3) {
		if (!radio.available()) {
			continue;
		}

		uint8_t buf[PAYLOAD];
		radio.read(buf, PAYLOAD);
		if (hits == 0 || memcmp(buf, prefix, PREFIX) != 0) {
			memcpy(prefix, buf, PREFIX);
			hits = 1;
			continue;
		}

		hits++;
		printPacket(buf);
		shown++;
	}

	while (radio.available()) {
		uint8_t drain[PAYLOAD];
		radio.read(drain, PAYLOAD);
	}

	if (shown > 0 && stay < STAY_LIMIT) {
		stay++;
		return;
	}
	stay = 0;
	hop();
}
