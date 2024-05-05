#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include "EBYTE.h"

#define PIN_RX 16   // Serial2 RX (connect this to the EBYTE Tx pin)
#define PIN_TX 17   // Serial2 TX pin (connect this to the EBYTE Rx pin)

#define PIN_M0 4    // D4 on the board (possibly pin 24)
#define PIN_M1 22   // D2 on the board (possibly called pin 22)
#define PIN_AX 21   // D15 on the board (possibly called pin 21)

int Chan;

// create the transceiver object, passing in the serial and pins
EBYTE Transceiver(&Serial2, PIN_M0, PIN_M1, PIN_AX);

#define IDENTIFIER_LENGTH 6
#define PAYLOAD_MAXLENGTH 82
#define CHECKSUM_LENGTH 3
#define DATA_LENGTH (IDENTIFIER_LENGTH + PAYLOAD_MAXLENGTH + CHECKSUM_LENGTH)
#define SIX_BITS 6
#define SOTDMA 0
#define ITDMA 1
#define UNUSED '-'
#define RADIO_STATUS 0b1100000000000000110

#define AIVDM "AIVDM"
#define AIVDO "AIVDO"

uint32_t mmsi; // 30
uint16_t altitude; // 12
uint16_t speed_over_ground; // 10
uint8_t position_accuracy; // 1
int longitude; // 28
int latitude; // 27
uint16_t course_over_ground; // 12
uint8_t time_stamp; // 6
uint8_t regional_reserved; // 8
bool dte; // 1
bool assigned; // 1
bool raim_flag; // 1
uint32_t radio_status; // 20
uint16_t true_heading; // 9
bool cs_unit; // 1
bool display_flag; // 1
bool dsc_flag; // 1
bool band_flag; // 1
bool message22_flag; // 1
uint8_t part_number; // 2
char vessel_name[21]; // 120
uint8_t ship_type; // 8
char vendor_id[4]; // 18
uint8_t unit_model_code; // 4
uint32_t serial_number; // 20
char call_sign[8]; // 42
uint16_t dimension_to_bow; // 9
uint16_t dimension_to_stern; // 9
uint8_t dimension_to_port; // 9
uint8_t dimension_to_starboard; // 9
uint32_t mothership_mmsi; // 9

struct AISMessage {
	char payload[PAYLOAD_MAXLENGTH + 1];
	char checksum[CHECKSUM_LENGTH + 1];
	char data[DATA_LENGTH + 1];
	uint16_t fill_bits;
	uint16_t data_length;
};

void InitMessage(struct AISMessage *message, const char *identifier,
		const char fragment_count, const char fragment_num,
		const char message_id, const char radio_channel) {

	memset(message, 0, sizeof(struct AISMessage));
	uint16_t *i = &message->data_length;
	char *data = message->data;
	
	data[(*i)++] = '!';
	data[(*i)++] = identifier[0];
	data[(*i)++] = identifier[1];
	data[(*i)++] = identifier[2];
	data[(*i)++] = identifier[3];
	data[(*i)++] = identifier[4];
	data[(*i)++] = ',';
	data[(*i)++] = fragment_count;
	data[(*i)++] = ',';
	data[(*i)++] = fragment_num;
	data[(*i)++] = ',';

	if (message_id >= '0' && message_id <= '9')
		data[(*i)++] = message_id;

	data[(*i)++] = ',';
	data[(*i)++] = radio_channel;
	data[(*i)++] = ',';
}

void AddData(struct AISMessage *dst, const char *data, const uint8_t length) {
	uint16_t *offset = &dst->data_length;

	for (uint16_t i = 0; i < length; ++i) {
		dst->data[*offset + i] = data[i];
	}

	dst->data_length += length;
}

void AddPayload(struct AISMessage *dst, long int data, uint16_t bit_length) {
	for (uint16_t i = 0; i < bit_length; ++i) {
		uint16_t idx = (dst->fill_bits + i) / SIX_BITS;
		int8_t shift = ((SIX_BITS - 1) - (dst->fill_bits + i) % SIX_BITS);
		dst->payload[idx] |= (1 & (data >> (bit_length - i - 1))) << shift;
	}

	dst->fill_bits += bit_length;
}

void EncodeAISNMEA(struct AISMessage *message) {
		uint16_t data_length = message->fill_bits / SIX_BITS;
		data_length += message->fill_bits % SIX_BITS ? 1 : 0;

    for (uint16_t i = 0; i < data_length; ++i) {
    	if (message->payload[i] > 39)
      	message->payload[i] += 8;

      message->payload[i] += 48;
    }
}

void ToHEX(char *buffer, const uint8_t checksum) {
	sprintf(buffer, "*%2X", checksum);
	
	for (uint8_t i = 1; i < CHECKSUM_LENGTH; ++i) {
		if (buffer[i] == 32)
			buffer[i] = '0';
	}
}

void ComputeChecksum(struct AISMessage *message) {
	uint8_t checksum = 0;
	char result[CHECKSUM_LENGTH + 1];

	char *data = message->data + 1;

	while (*data)
		checksum ^= *data++;

	ToHEX(result, checksum);
	AddData(message, result, CHECKSUM_LENGTH);
}

void EncodeMessage9(struct AISMessage *message) {
	AddPayload(message, 9, 6); // message type
	AddPayload(message, 0, 2);
	AddPayload(message, mmsi, 30);
	AddPayload(message, altitude, 12);
	AddPayload(message, speed_over_ground, 10);
	AddPayload(message, position_accuracy, 1);
	AddPayload(message, longitude, 28);
	AddPayload(message, latitude, 27);
	AddPayload(message, course_over_ground, 12);
	AddPayload(message, time_stamp, 6);
	AddPayload(message, 0, 8); // regional reserved
	AddPayload(message, dte, 1);
	AddPayload(message, 0, 3); // spare
	AddPayload(message, assigned, 1);
	AddPayload(message, raim_flag, 1);
	AddPayload(message, SOTDMA, 1);
	AddPayload(message, 0, 19);
	EncodeAISNMEA(message);
	AddData(message, message->payload, 28);
	AddData(message, ",0", 2);
	ComputeChecksum(message);
}

void EncodeMessage18(struct AISMessage *message) {
	AddPayload(message, 18, 6); // message type
	AddPayload(message, 0, 2);
	AddPayload(message, mmsi, 30);
	AddPayload(message, 15, 8); // regional reserved
	AddPayload(message, speed_over_ground, 10);
	AddPayload(message, position_accuracy, 1);
	AddPayload(message, longitude, 28);
	AddPayload(message, latitude, 27);
	AddPayload(message, course_over_ground, 12);
	AddPayload(message, true_heading, 9);
	AddPayload(message, time_stamp, 6);
	AddPayload(message, 0, 2); // regional reserved
	AddPayload(message, 1, 1);
	AddPayload(message, 1, 1); // display flag
	AddPayload(message, 1, 1); // dsc flag
	AddPayload(message, 1, 1); // band flag
	AddPayload(message, 1, 1); // message22 flag
	AddPayload(message, assigned, 1); // message22 flag
	AddPayload(message, raim_flag, 1);
	AddPayload(message, ITDMA, 1);
	AddPayload(message, RADIO_STATUS, 19);
	EncodeAISNMEA(message);
	AddData(message, message->payload, 28);
	AddData(message, ",0", 2);
	ComputeChecksum(message);
}

void EncodeMessage24A(struct AISMessage *message) {
	AddPayload(message, 24, 6); // message type
	AddPayload(message, 0, 2); // repeat indicator
	AddPayload(message, mmsi, 30);
	AddPayload(message, part_number, 2); // part number

	const char vessel_name[21] = "LORAIS-PPNS";

	for (uint8_t i = 0; i < 20; ++i) {
		AddPayload(message, vessel_name[i] - 64, 6);
	}

	EncodeAISNMEA(message);
	AddData(message, message->payload, 27);
	AddData(message, ",2", 2);
	ComputeChecksum(message);
}

void EncodeMessage24B(struct AISMessage *message) {
	AddPayload(message, 24, 6); // message type
	AddPayload(message, 0, 2); // repeat indicator
	AddPayload(message, mmsi, 30);
	AddPayload(message, part_number, 2); // part number
	AddPayload(message, ship_type, 8);

	// const char vendor_id[4] = "";
	uint32_t vendor_id = 1234567;
	AddPayload(message, ((vendor_id >> 12) & 255) - 64, 6);
	AddPayload(message, ((vendor_id >> 6) & 255) - 64, 6);
	AddPayload(message, (vendor_id & 255) - 64, 6);
	AddPayload(message, 0, 4);
	AddPayload(message, 0, 20);

	const char call_sign[8] = "CALLSIG";

	for (uint8_t i = 0; i < 7; ++i) {
		AddPayload(message, call_sign[i] - 64, 6);
	}

	AddPayload(message, 0, 9);
	AddPayload(message, 0, 9);
	AddPayload(message, 0, 6);
	AddPayload(message, 0, 6);
	AddPayload(message, 0, 30);

	// AddPayload(message, unit_model_code, 4);
	EncodeAISNMEA(message);
	AddData(message, message->payload, 27);
	AddData(message, ",0", 2);
	ComputeChecksum(message);
}

struct AISMessage msg9;
struct AISMessage msg18;
struct AISMessage msg24A;
struct AISMessage msg24B;

void setup() {
  Serial.begin(9600);
  Serial2.begin(9600);

  Transceiver.init();
  Transceiver.SetAddressH(0xFF); // broadcast
  Transceiver.SetAddressL(0xFF);
	float channel = (161.975 - 148) / 0.1;
  Chan = channel;

  Transceiver.SetChannel(Chan);
  Transceiver.PrintParameters();
}

void loop() {
	mmsi = 366000005;
	altitude = 16;
	speed_over_ground = 100;
	position_accuracy = 1;
	longitude = -49749876;
	latitude = 17523450;
	course_over_ground = 30 * 10;
	time_stamp = 11;
	dte = 1;
	assigned = 0;
	raim_flag = 0;

	InitMessage(&msg9, AIVDO, '1', '1', UNUSED, 'A');
	EncodeMessage9(&msg9);
  Transceiver.SendStruct(&msg9, sizeof(msg9));

	mmsi = 423302100;
	speed_over_ground = 14;
	position_accuracy = 1;
	longitude = (53 * 60 * 10000) + (0.6598 * 10000);
	latitude = (40 * 60 * 10000) + (0.3170 * 10000);
	course_over_ground = 177 * 10;
	true_heading = 177;
	time_stamp = 34;
	raim_flag = 0;
	assigned = 0;
	InitMessage(&msg18, AIVDM, '1', '1', UNUSED, 'A');
	EncodeMessage18(&msg18);
  Transceiver.SendStruct(&msg18, sizeof(msg18));

	mmsi = 112233445;
	part_number = 0;

	InitMessage(&msg24A, AIVDO, '1', '1', UNUSED, 'B');
	EncodeMessage24A(&msg24A);
  Transceiver.SendStruct(&msg24A, sizeof(msg24A));

	part_number = 1;

	InitMessage(&msg24B, AIVDO, '1', '1', UNUSED, 'B');
	EncodeMessage24B(&msg24B);
  Transceiver.SendStruct(&msg24B, sizeof(msg24B));

  delay(1000); // 1 Second
}