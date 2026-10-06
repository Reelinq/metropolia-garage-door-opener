#pragma once
#include "header.h"

enum class DoorState { Closed, Open, InBetween };
enum class DoorError { Normal, Stuck };
enum class CalibrationState { NotCalibrated, Calibrated };

// ticks: + = opening, - = closing. ticks: 0 = closed, total = open.
class Door {
	public:
		void on_movement(int ticks); // call with the ticks read from the encoder
		void set_position(int ticks);
		void set_calibrated(bool calibrated, int ticks = 0); // ticks = distance between the two limit switches
		void set_error(DoorError e);
		int next_direction(int current_dir);

		// Puts the door back into a calibrated state from saved data.
		bool restore(int pos_in, int total_in, int dir_in);

		DoorState state() const;
		DoorError error() const;
		CalibrationState calibration() const;
		int position() const;
		int total_ticks() const;
		int last_direction() const;

	private:
		int pos = 0;
		int total = 0;
		int last_dir = 0; // last direction the door was moving (+1 opening, -1 closing)
		CalibrationState cal = CalibrationState::NotCalibrated;
		DoorError err = DoorError::Normal;
};
