#pragma once

enum class DoorState { Open, Closed, InBetween };
enum class DoorError { Normal, Stuck };
enum class CalibrationState { Calibrated, NotCalibrated };

class Door {
	public:
		void on_motor_step(int dir); // call after every motor step

		void set_position(int steps);
		void set_calibrated(bool calibrated, int steps = 0);
		void set_error(DoorError e);

		DoorState state() const; // InBetween while not calibrated
		DoorError error() const;
		CalibrationState calibration() const;
		int position() const;
		int total_steps() const;

	private:
		int pos = 0; // current position in motor steps
		int total = 0; // total steps between the two limit switches
		CalibrationState cal = CalibrationState::NotCalibrated;
		DoorError err = DoorError::Normal;
};
