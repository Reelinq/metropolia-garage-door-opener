#pragma once

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

		DoorState state() const;
		DoorError error() const;
		CalibrationState calibration() const;
		int position() const;
		int total_ticks() const;

	private:
		int pos = 0;
		int total = 0;
		CalibrationState cal = CalibrationState::NotCalibrated;
		DoorError err = DoorError::Normal;
};
