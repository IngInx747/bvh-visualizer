#pragma once
#ifndef UI_H

class UIOption
{
public:
	enum Option
	{
		SELECT_NONE,
		SELECT_VERT,
		SELECT_EDGE,
		SELECT_FACE
	};

public:
	static int select_mode;
	static bool accel_mode;
	static bool show_bvh_bbox;
};

class UI
{
public:
	static void initialize();
	static void shutdown();
	static void render();

	static void reshape(int w, int h);
	static void keyboard(unsigned char key, int x, int y);
	static void mouse_move(int x, int y);
	static void mouse_click(int button, int state, int x, int y);

private:
	static void _display_func();
};

#endif // !UI_H
