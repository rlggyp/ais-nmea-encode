#include "encode.h"
#include "EBYTE.h"

#define PIN_RX 16   // Serial2 RX on the board GPIO 16 
#define PIN_TX 17   // Serial2 TX on the board GPIO 17

#define PIN_M0 19 // on the board GPIO 24
#define PIN_M1 18 // on the board GPIO 18
#define PIN_AX 4  // on the board GPIO 4
#define SUCCESS true

int channel;

EBYTE Transceiver(&Serial2, PIN_M0, PIN_M1, PIN_AX);

struct AISMessage msg9;
struct AISMessage msg18;
struct AISMessage msg24A;
struct AISMessage msg24B;
struct AISData data;

void setup() {
	// Message 9
	data.mmsi = 366000005;
	data.altitude = 16;
	data.speed_over_ground = 100;
	data.position_accuracy = 1;
	data.longitude = -49749876;
	data.latitude = 17523450;
	data.course_over_ground = 30 * 10;
	data.time_stamp = 11;
	data.dte = 1;
	data.assigned = 0;
	data.raim_flag = 0;
	
	InitMessage(&msg9, AIVDO, '1', '1', UNUSED, 'A');
	EncodeMessage9(&msg9, &data);
	
	// Message 18
	data.mmsi = 212060039;
	data.speed_over_ground = 3.9 * 10;
	data.position_accuracy = 1;
	data.longitude = (53 * 60 * 10000) + (0.6598 * 10000);
	data.latitude = (40 * 60 * 10000) + (0.3170 * 10000);
	data.course_over_ground = 39.4 * 10;
	data.true_heading = 39;
	data.time_stamp = 61;
	data.raim_flag = 0;
	data.assigned = 0;
	
	InitMessage(&msg18, AIVDM, '1', '1', UNUSED, 'A');
	EncodeMessage18(&msg18, &data);
	
	data.mmsi = 112233445;
	
	// Message 24A
	data.part_number = 0;
	strcpy(data.vessel_name, "TEST-AIS");
	
	InitMessage(&msg24A, AIVDO, '1', '1', UNUSED, 'B');
	EncodeMessage24A(&msg24A, &data);
	
	// Message 24B
	data.part_number = 1;
	strcpy(data.call_sign, "CALLSIG");
	
	InitMessage(&msg24B, AIVDO, '1', '1', UNUSED, 'B');
	EncodeMessage24B(&msg24B, &data);

	Serial2.begin(115200);
	Serial.begin(115200);
	Serial.println(Transceiver.init());

	// broadcast
  Transceiver.SetAddressH(0xFF);
  Transceiver.SetAddressL(0xFF);

  channel = (161.975 - 148) / 0.1;
  Transceiver.SetChannel(channel);
  // Transceiver.SaveParameters(PERMANENT);
	Transceiver.PrintParameters();
}

void loop() {
	if (Transceiver.SendStruct(&msg9.data, sizeof(msg9.data)) == SUCCESS){
  	Serial.print("Successfully send msg9: "); 
		Serial.println(msg9.data);
	}
	if (Transceiver.SendStruct(&msg18.data, sizeof(msg18.data)) == SUCCESS){

  	Serial.print("Successfully send msg18: "); 
		Serial.println(msg18.data);
	}

	if (Transceiver.SendStruct(&msg24A.data, sizeof(msg24A.data)) == SUCCESS){
  	Serial.print("Successfully send msg24A: "); 
		Serial.println(msg24A.data);
	}

	if (Transceiver.SendStruct(&msg24B.data, sizeof(msg24B.data)) == SUCCESS){
  	Serial.print("Successfully send msg24B: "); 
		Serial.println(msg24B.data);
	}

	delay(1000);
}
