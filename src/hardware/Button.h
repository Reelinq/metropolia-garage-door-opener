#pragma once
#include "Config.h"

class Button {
	public:
		Button(uint pin);
		bool pressed();
		bool pressed_with(Button& other);
	private:
		void sample(); 
		uint button_pin;
		bool held = false;   
		bool both_held = false;
		uint32_t last_ms = 0;
};
