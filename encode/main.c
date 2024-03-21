#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define IDENTIFIER_LENGTH 5
#define PAYLOAD_MAXLENGTH 82
#define CHECKSUM_LENGTH 3
#define DATA_LENGTH (IDENTIFIER_LENGTH + PAYLOAD_MAXLENGTH + CHECKSUM_LENGTH)
#define SIX_BITS 6

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
	memset(message->identifier, 0, sizeof(char) * IDENTIFIER_LENGTH);
	memset(message->payload, 0, sizeof(char) * PAYLOAD_MAXLENGTH);
	memset(message->checksum, 0, sizeof(char) * CHECKSUM_LENGTH);
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

void AddData(struct AISMessage *dst, long data, uint16_t bit_length) {
	for (uint16_t i = 0; i < bit_length; ++i) {
		uint16_t idx = (dst->fill_bits) / SIX_BITS;
		int8_t shift = ((SIX_BITS - 1) - (dst->fill_bits + i) % SIX_BITS);
		dst->payload[idx] |= (1 & (data >> (bit_length - i - 1))) << shift;
	}

	dst->fill_bits += bit_length;
}

void EncodeAISNMEA(struct AISMessage *message) {
		uint16_t data_length = message->fill_bits / 8;
		data_length += message->fill_bits % 8 ? 1 : 0;

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

void EncodeMessage9

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
//    struct Payload payload;
//    payload.data = (char *)malloc(29 * sizeof(char));
//    memset(payload.data, 0, 29);
//    payload.length = 28;
//    payload.bit_length = 0;
//
//    clock_t start, end;
//    double cpu_time_used;
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
