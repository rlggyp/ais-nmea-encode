#include <cstring>
#include <cstdio>
#include <cstdint>

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

void InitMessage(struct AISMessage *message, char const *identifier,
  char const fragment_count, char const fragment_num,
  char const message_id, char const radio_channel);

void AddData(struct AISMessage *dst, char const *data, uint8_t const length);
void AddPayload(struct AISMessage *dst, long int data, uint16_t bit_length);
void EncodeAISNMEA(struct AISMessage *message);
void ToHEX(char *buffer, uint8_t const checksum);
void ComputeChecksum(struct AISMessage *message);
void EncodeMessage9(struct AISMessage *message);
void EncodeMessage18(struct AISMessage *message);
void EncodeMessage24A(struct AISMessage *message);
void EncodeMessage24B(struct AISMessage *message);
