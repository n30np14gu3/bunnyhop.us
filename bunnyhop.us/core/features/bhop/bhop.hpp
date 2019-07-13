#pragma once

class c_movement {
public:
	void bunnyhop(c_usercmd* user_cmd) noexcept;
	void autostrafe(c_usercmd* user_cmd) noexcept;
};

extern c_movement movement;