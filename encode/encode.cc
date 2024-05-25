#include "encode.h"

void InitMessage(struct AISMessage *message, char const *identifier,
  char const fragment_count, char const fragment_num,
  char const message_id, char const radio_channel) {

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

void AddData(struct AISMessage *dst, char const *data, uint8_t const length) {
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

void ToHEX(char *buffer, uint8_t const checksum) {
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

  // const char vessel_name[21] = "RLGGYP";

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

  uint32_t vendor_id = 1234567;
  AddPayload(message, ((vendor_id >> 12) & 255) - 64, 6);
  AddPayload(message, ((vendor_id >> 6) & 255) - 64, 6);
  AddPayload(message, (vendor_id & 255) - 64, 6);
  AddPayload(message, 0, 4);
  AddPayload(message, 0, 20);

  // const char call_sign[8] = "CALLSIG";

  for (uint8_t i = 0; i < 7; ++i) {
    AddPayload(message, call_sign[i] - 64, 6);
  }

  AddPayload(message, 0, 9);
  AddPayload(message, 0, 9);
  AddPayload(message, 0, 6);
  AddPayload(message, 0, 6);
  AddPayload(message, 0, 30);

  EncodeAISNMEA(message);
  AddData(message, message->payload, 27);
  AddData(message, ",0", 2);
  ComputeChecksum(message);
}
