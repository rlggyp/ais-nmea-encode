#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

#define IDENTIFIER_LENGTH 6
#define PAYLOAD_MAXLENGTH 82
#define CHECKSUM_LENGTH 3
#define DATA_LENGTH (IDENTIFIER_LENGTH + PAYLOAD_MAXLENGTH + CHECKSUM_LENGTH)
#define SIX_BITS 6
#define SOTDMA 0
#define ITDMA 1
#define UNUSED '-'

#define AIVDM "AIVDM"
#define AIVDO "AIVDO"

uint32_t mmsi; // 30
uint16_t altitude; // 12
uint16_t speed_over_ground; // 10
bool position_accuracy; // 1
uint32_t longitude; // 28
uint32_t latitude; // 27
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

void InitMessage9(struct AISMessage *message, const char *identifier,
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
}

int main() {
  clock_t start, end;
  double cpu_time_used;

	start = clock();
  struct AISMessage msg9;
	InitMessage9(&msg9, AIVDM, '1', '1', UNUSED, 'A');
	
	AddPayload(&msg9, 18, 6); // 1
	AddPayload(&msg9, 0, 2); // 2
	AddPayload(&msg9, 423302100, 30); // 3
	AddPayload(&msg9, 15, 8); // 4
	AddPayload(&msg9, 14, 10); // 5
	AddPayload(&msg9, 1, 1); // 6
	AddPayload(&msg9, (53 * 60 * 10000 + 0.6598 * 10000), 28); // 7
	AddPayload(&msg9, (40 * 60 * 10000 + 0.3170 * 10000), 27); // 8
	AddPayload(&msg9, 177 * 10, 12); // 9
	AddPayload(&msg9, 177, 9); // 10
	AddPayload(&msg9, 34, 6); // 11
	AddPayload(&msg9, 0, 2); // 12
	AddPayload(&msg9, 1, 1); // 13
	AddPayload(&msg9, 1, 1); // 14
	AddPayload(&msg9, 1, 1); // 15
	AddPayload(&msg9, 1, 1); // 16
	AddPayload(&msg9, 1, 1); // 17
	AddPayload(&msg9, 0, 1); // 18
	AddPayload(&msg9, 0, 1); // 19
	AddPayload(&msg9, 1, 1); // 20
	AddPayload(&msg9, 393222, 19); // 21
	EncodeAISNMEA(&msg9);

	AddData(&msg9, msg9.payload, 28);
	AddData(&msg9, ",0", 2);

	ComputeChecksum(&msg9);

	printf("%s\n", msg9.data);
	end = clock();
	cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
	printf("Time taken: %f seconds\n", cpu_time_used);

  return 0;
}
