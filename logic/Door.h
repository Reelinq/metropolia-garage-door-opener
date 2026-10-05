#pragma once

// Calibration: if the encoder gives no movement for this long while the motor
// runs and the limit switch has not closed, the door is stuck (watchdog resets the chip).
#define CALIBRATION_WD_MS 2000
 
// Time for the belt to settle after the motor is switched off, before reading
// the last encoder ticks.
#define SETTLE_MS 100
 
class LimitSwitch;
class Stepper;
class RotaryEncoder;

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
		// Runs the motor in direction dir (+1 opening, -1 closing) until the given
		// limit switch closes. The motor is stopped as soon as the switch closes, before
		// the block hits the body of the switch. Encoder ticks are added to the door
		// position. If the door gets stuck, the encoder watchdog resets the chip.
		void run_until_pressed(int dir, const LimitSwitch& sw, Stepper& stepper, RotaryEncoder& encoder);
 
		// Calibrates the door: closes it until closed_sw (position 0), then opens it
		// until open_sw, counting encoder ticks. Stores the result in the door.
		// Returns true on success; false if the encoder counted the wrong way
		// (the door stays not calibrated).
		bool calibrate(const LimitSwitch& closed_sw, const LimitSwitch& open_sw,
					   Stepper& stepper, RotaryEncoder& encoder);
			// Local button (SW1) logic. current_dir: +1 opening, -1 closing, 0 stopped.
		// Returns the new motor direction (+1, -1 or 0 = stop):
		//   not calibrated -> 0 (door may not be moved)
		//   moving         -> 0 (stop, remembers the direction it was moving)
		//   closed         -> +1 (open)
		//   open           -> -1 (close)
		//   stopped midway -> opposite of the direction it was moving before
		int next_direction(int current_dir);

		DoorState state() const;
		DoorError error() const;
		CalibrationState calibration() const;
		int position() const;
		int total_ticks() const;

	private:
		int pos = 0;
		int total = 0;
		int last_dir = 0; // last direction the door was moving (+1 opening, -1 closing)
		CalibrationState cal = CalibrationState::NotCalibrated;
		DoorError err = DoorError::Normal;
};
