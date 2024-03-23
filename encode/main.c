#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

#define IDENTIFIER_LENGTH 5
#define PAYLOAD_MAXLENGTH 82
#define CHECKSUM_LENGTH 3
#define DATA_LENGTH (IDENTIFIER_LENGTH + PAYLOAD_MAXLENGTH + CHECKSUM_LENGTH)
#define SIX_BITS 6
#define SOTDMA 0
#define ITDMA 1

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
	char identifier[IDENTIFIER_LENGTH + 1];
	char fragment_count;
	char fragment_num;
	char message_id;
	char radio_channel;
	char payload[PAYLOAD_MAXLENGTH + 1];
	char checksum[CHECKSUM_LENGTH + 1];
	char data[DATA_LENGTH + 1];
	uint16_t filled_data;
	uint16_t fill_bits;
};

void InitMessage9(struct AISMessage *message) {
	memset(message, 0, sizeof(struct AISMessage));
	message->identifier[0] = 'A';
	message->identifier[1] = 'I';
	message->identifier[2] = 'V';
	message->identifier[3] = 'D';
	message->identifier[4] = 'M';
	message->fragment_count = '1';
	message->fragment_num = '1';
	message->message_id = 0;
	message->radio_channel = 'B';
	message->filled_data = 0;
	message->fill_bits = 0;
}

void AddData(struct AISMessage *dst, long int data, uint16_t bit_length) {
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

void ComputeChecksum(struct AISMessage *message) {
	uint8_t checksum = 0;
	for (uint16_t i = 1; message->data[i] != '\0' && i < DATA_LENGTH; ++i) {
		checksum ^= message->data[i];
	}

	sprintf(message->checksum, "*%2X", checksum);
}

void EncodeMessage9(struct AISMessage *message) {
}

//void AddData(struct Payload *dst, long data, int16_t bit_length) {
//    for (uint16_t i = 0; i < bit_length; ++i) {
//        uint16_t idx = (dst->bit_length + i) / 6;
//        int8_t shift = (5 - (dst->bit_length + i) % 6);
//        dst->data[idx] |= (1 & (data >> (bit_length - i - 1))) << shift;
//    }
//
//    dst->bit_length += bit_length;
//}

//char* encodeAISNMEA(struct Payload *payload) {
//    char *encoded_data = (char *)malloc((payload->length + 1) * sizeof(char));
//
//    for (uint16_t i = 0; i < payload->length; ++i) {
//        if (payload->data[i] > 39)
//            payload->data[i] += 8;
//        payload->data[i] += 48;
//        encoded_data[i] = payload->data[i];
//    }
//
//    encoded_data[payload->length] = '\0';
//    return encoded_data;
//}

int main() {
  clock_t start, end;
  double cpu_time_used;

	start = clock();
  struct AISMessage msg9;
	InitMessage9(&msg9);
	
	AddData(&msg9, 18, 6); // 1
	AddData(&msg9, 0, 2); // 2
	AddData(&msg9, 423302100, 30); // 3
	AddData(&msg9, 15, 8); // 4
	AddData(&msg9, 14, 10); // 5
	AddData(&msg9, 1, 1); // 6
	AddData(&msg9, (53 * 60 * 10000 + 0.6598 * 10000), 28); // 7
	AddData(&msg9, (40 * 60 * 10000 + 0.3170 * 10000), 27); // 8
	AddData(&msg9, 177 * 10, 12); // 9
	AddData(&msg9, 177, 9); // 10
	AddData(&msg9, 34, 6); // 11
	AddData(&msg9, 0, 2); // 12
	AddData(&msg9, 1, 1); // 13
	AddData(&msg9, 1, 1); // 14
	AddData(&msg9, 1, 1); // 15
	AddData(&msg9, 1, 1); // 16
	AddData(&msg9, 1, 1); // 17
	AddData(&msg9, 0, 1); // 18
	AddData(&msg9, 0, 1); // 19
	AddData(&msg9, 1, 1); // 20
	AddData(&msg9, 393222, 19); // 21
	EncodeAISNMEA(&msg9);

	msg9.data[msg9.filled_data++] = '!';
	msg9.data[msg9.filled_data++] = msg9.identifier[0];
	msg9.data[msg9.filled_data++] = msg9.identifier[1];
	msg9.data[msg9.filled_data++] = msg9.identifier[2];
	msg9.data[msg9.filled_data++] = msg9.identifier[3];
	msg9.data[msg9.filled_data++] = msg9.identifier[4];
	msg9.data[msg9.filled_data++] = ',';
	msg9.data[msg9.filled_data++] = '1';
	msg9.data[msg9.filled_data++] = ',';
	msg9.data[msg9.filled_data++] = '1';
	msg9.data[msg9.filled_data++] = ',';
	msg9.data[msg9.filled_data++] = ',';
	msg9.data[msg9.filled_data++] = 'A';
	msg9.data[msg9.filled_data++] = ',';
	msg9.data[msg9.filled_data++] = msg9.payload[0];
	msg9.data[msg9.filled_data++] = msg9.payload[1];
	msg9.data[msg9.filled_data++] = msg9.payload[2];
	msg9.data[msg9.filled_data++] = msg9.payload[3];
	msg9.data[msg9.filled_data++] = msg9.payload[4];
	msg9.data[msg9.filled_data++] = msg9.payload[5];
	msg9.data[msg9.filled_data++] = msg9.payload[6];
	msg9.data[msg9.filled_data++] = msg9.payload[7];
	msg9.data[msg9.filled_data++] = msg9.payload[8];
	msg9.data[msg9.filled_data++] = msg9.payload[9];
	msg9.data[msg9.filled_data++] = msg9.payload[10];
	msg9.data[msg9.filled_data++] = msg9.payload[11];
	msg9.data[msg9.filled_data++] = msg9.payload[12];
	msg9.data[msg9.filled_data++] = msg9.payload[13];
	msg9.data[msg9.filled_data++] = msg9.payload[14];
	msg9.data[msg9.filled_data++] = msg9.payload[15];
	msg9.data[msg9.filled_data++] = msg9.payload[16];
	msg9.data[msg9.filled_data++] = msg9.payload[17];
	msg9.data[msg9.filled_data++] = msg9.payload[18];
	msg9.data[msg9.filled_data++] = msg9.payload[19];
	msg9.data[msg9.filled_data++] = msg9.payload[20];
	msg9.data[msg9.filled_data++] = msg9.payload[21];
	msg9.data[msg9.filled_data++] = msg9.payload[22];
	msg9.data[msg9.filled_data++] = msg9.payload[23];
	msg9.data[msg9.filled_data++] = msg9.payload[24];
	msg9.data[msg9.filled_data++] = msg9.payload[25];
	msg9.data[msg9.filled_data++] = msg9.payload[26];
	msg9.data[msg9.filled_data++] = msg9.payload[27];
	msg9.data[msg9.filled_data++] = ',';
	msg9.data[msg9.filled_data++] = '0';

	ComputeChecksum(&msg9);
	msg9.data[msg9.filled_data++] = msg9.checksum[0];
	msg9.data[msg9.filled_data++] = msg9.checksum[1];
	msg9.data[msg9.filled_data++] = msg9.checksum[2];

	// printf("%s", msg9.checksum);
	printf("%s\n", msg9.data);
	end = clock();
	cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
	printf("Time taken: %f seconds\n", cpu_time_used);

//    struct Payload payload;
//    payload.data = (char *)malloc(29 * sizeof(char));
//    memset(payload.data, 0, 29);
//    payload.length = 28;
//    payload.bit_length = 0;
//
//
//    start = clock();
//
//    AddData(&payload, 18, 6); // 1
//    AddData(&payload, 0, 2); // 2
//    AddData(&payload, 423302100, 30); // 3
//    AddData(&payload, 15, 8); // 4
//    AddData(&payload, 14, 10); // 5
//    AddData(&payload, 1, 1); // 6
//    AddData(&payload, (53 * 60 * 10000 + 0.6598 * 10000), 28); // 7
//    AddData(&payload, (40 * 60 * 10000 + 0.3170 * 10000), 27); // 8
//    AddData(&payload, 177 * 10, 12); // 9
//    AddData(&payload, 177, 9); // 10
//    AddData(&payload, 34, 6); // 11
//    AddData(&payload, 0, 2); // 12
//    AddData(&payload, 1, 1); // 13
//    AddData(&payload, 1, 1); // 14
//    AddData(&payload, 1, 1); // 15
//    AddData(&payload, 1, 1); // 16
//    AddData(&payload, 1, 1); // 17
//    AddData(&payload, 0, 1); // 18
//    AddData(&payload, 0, 1); // 19
//    AddData(&payload, 1, 1); // 20
//    AddData(&payload, 393222, 19); // 21
//
//    end = clock();
//    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
//    printf("Time taken: %f seconds\n", cpu_time_used);
//
//    char *encoded_sentence = encodeAISNMEA(&payload);
//    printf("Encoded AIS NMEA sentence: %s\n", encoded_sentence);
//    printf("Expected sentence: B6CdCm0t3`tba35f@V9faHi7kP06\n");
//    printf("Length: %d\n", payload.length);
//
//    free(encoded_sentence);
//    free(payload.data);

    return 0;
}
