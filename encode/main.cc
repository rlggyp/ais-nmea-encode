#include "encode.h"

int main() {
	{
		struct AISMessage msg9;
		struct AISMessage msg18;
		struct AISMessage msg24A;
		struct AISMessage msg24B;
		struct AISData data;

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
		std::printf("msg9: %s\n", msg9.data);

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
		std::printf("msg18: %s\n", msg18.data);

		data.mmsi = 112233445;

		// Message 24A
		data.part_number = 0;
		strcpy(data.vessel_name, "RLGGYP");

		InitMessage(&msg24A, AIVDO, '1', '1', UNUSED, 'B');
		EncodeMessage24A(&msg24A, &data);
		std::printf("msg24A: %s\n", msg24A.data);

		// Message 24B
		data.part_number = 1;
		strcpy(data.call_sign, "CALLSIG");

		InitMessage(&msg24B, AIVDO, '1', '1', UNUSED, 'B');
		EncodeMessage24B(&msg24B, &data);
		std::printf("msg24B: %s\n", msg24B.data);
	}
	return 0;
}
