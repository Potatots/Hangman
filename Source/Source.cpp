#include <stdio.h>
#include <fstream>
#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <io.h>
#include <string>

using namespace std;

ALLEGRO_DISPLAY *display = NULL;

int files_count = 0;

bool if_exit = false;

int install_allegro(int r_x, int r_y) {
	srand(time(NULL));

	al_init_font_addon();
	al_init_ttf_addon();
	al_init();
	al_install_mouse();
	al_install_keyboard();
	al_init_image_addon();
	al_init_primitives_addon();

	display = al_create_display(r_x, r_y);
	if (!display) {
		al_show_native_message_box(display, "Error!", "DISPLAY_ERROR", "Screen init error", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		return -1;
	}

	return 1;
}

int* high_score_load() {
	int *stats = new int[2];
	string temp = "";
	fstream file;
	file.open("Files/Hangman", ios::in);
	if (file) {
		for (int i = 0; i < 2; i++) {
			file >> temp;
			stats[i] = stoi(temp);
		}
		file.close();
	}
	else {
		stats[0] = 0;
		stats[1] = 0;
	}
	return stats;
}

void high_score_save(int gsd, int notgsd) {
	fstream file;
	file.open("Files/Hangman", ios::out);
	file << gsd << "\n" << notgsd;
	file.close();
}

int menu(int r_x, int r_y) {
	ALLEGRO_EVENT_QUEUE *menu_queue = NULL;
	ALLEGRO_EVENT menu_event;

	ALLEGRO_BITMAP *background = NULL;
	background = al_load_bitmap("Files/img/background.png");
	if (!background) {
		al_show_native_message_box(display, "Error!", "BITMAP_ERROR", "Background bitmap missing", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		return -1;
	}

	ALLEGRO_FONT *menu_font = NULL;
	menu_font = al_load_ttf_font("Files/Chocolate Covered Raindrops.ttf", 28, 0);
	if (!menu_font) {
		al_show_native_message_box(display, "Error!", "FONT_ERROR", "Font missing", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		return -1;
	}

	int hover = 0;
	int button_w = 250, button_h = 50;
	int pos_x = (r_x - button_w) / 2, pos_y1 = 150, pos_y2 = 250;

	menu_queue = al_create_event_queue();

	al_register_event_source(menu_queue, al_get_display_event_source(display));
	al_register_event_source(menu_queue, al_get_keyboard_event_source());
	al_register_event_source(menu_queue, al_get_mouse_event_source());

	while (1) {
		al_wait_for_event(menu_queue, &menu_event);

		if (menu_event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) { //EXIT
			return 2;
		}
		else if (menu_event.type == ALLEGRO_EVENT_KEY_DOWN) {
			switch (menu_event.keyboard.keycode) {
				case ALLEGRO_KEY_UP:
					hover--;
					break;
				case ALLEGRO_KEY_DOWN:
					hover++;
					break;
				case ALLEGRO_KEY_ENTER:
					return hover;
					break;
				case ALLEGRO_KEY_ESCAPE:
					return 2;
					break;
			}
			if (hover < 1)
				hover = 1;
			if (hover > 2)
				hover = 2;
		}
		else if (menu_event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN && hover != 0) {
			return hover;
		}
		else if (menu_event.type == ALLEGRO_EVENT_MOUSE_AXES) {
			if (menu_event.mouse.x > pos_x && menu_event.mouse.x < pos_x + button_w) {
				if (menu_event.mouse.y > pos_y1 && menu_event.mouse.y < pos_y1 + button_h)
					hover = 1;
				else if (menu_event.mouse.y > pos_y2 && menu_event.mouse.y < pos_y2 + button_h)
					hover = 2;
				else
					hover = 0;
			}
			else
				hover = 0;
		}

		if (al_is_event_queue_empty(menu_queue)) {
			al_draw_bitmap(background, 0, 0, 0);

			al_draw_rectangle(pos_x, pos_y1, pos_x + button_w, pos_y1 + button_h, al_map_rgb(0, 0, 0), 1);
			al_draw_text(menu_font, al_map_rgb(0, 0, 0), r_x / 2, pos_y1 + button_h / 4, ALLEGRO_ALIGN_CENTRE, "New Game");

			al_draw_rectangle(pos_x, pos_y2, pos_x + button_w, pos_y2 + button_h, al_map_rgb(0, 0, 0), 1);
			al_draw_text(menu_font, al_map_rgb(0, 0, 0), r_x / 2, pos_y2 + button_h / 4, ALLEGRO_ALIGN_CENTRE, "Exit");


			if (hover == 1) {
				al_draw_filled_rectangle(pos_x, pos_y1, pos_x + button_w, pos_y1 + button_h, al_map_rgb(0, 0, 0));
				al_draw_text(menu_font, al_map_rgb(255, 255, 255), r_x / 2, pos_y1 + button_h / 4, ALLEGRO_ALIGN_CENTRE, "New Game");
			}
			else if (hover == 2) {
				al_draw_filled_rectangle(pos_x, pos_y2, pos_x + button_w, pos_y2 + button_h, al_map_rgb(0, 0, 0));
				al_draw_text(menu_font, al_map_rgb(255, 255, 255), r_x / 2, pos_y2 + button_h / 4, ALLEGRO_ALIGN_CENTRE, "Exit");
			}

			al_flip_display();
		}
	}

	al_destroy_event_queue(menu_queue);
	al_destroy_bitmap(background);
}

string* find_files() {
	struct _finddata_t data;
	intptr_t file;
	string *name = NULL;
	int i = 0;

	files_count = 0;

	if ((file = _findfirst("Files/Words/*.txt", &data)) == -1)
		al_show_native_message_box(display, "Error!", "TXT_ERROR", "No *txt files!", NULL, ALLEGRO_MESSAGEBOX_ERROR);
	else {
		do {
			files_count++;
		} while (_findnext(file, &data) == 0);

		name = new string[files_count];

		file = _findfirst("Files/Words/*.txt", &data);

		do {
			string temp(data.name);
			name[i] = temp;
			i++;
		} while (_findnext(file, &data) == 0);

		_findclose(file);
	}
	return name;
}

string category_menu(int r_x, int r_y) {
	ALLEGRO_EVENT_QUEUE *menu_queue = NULL;

	ALLEGRO_BITMAP *background = al_load_bitmap("Files/img/background.png");
	if (!background) {
		al_show_native_message_box(display, "Error!", "BITMAP_ERROR", "Background bitmap missing", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		return NULL;
	}

	ALLEGRO_FONT *menu_font = al_load_ttf_font("Files/Chocolate Covered Raindrops.ttf", 24, 0);
	if (!menu_font) {
		al_show_native_message_box(display, "Error!", "FONT_ERROR", "Font missing", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		return NULL;
	}

	string return_val = "";

	string *txt_names = new string[files_count];
	string *menu_names = new string[files_count];

	txt_names = find_files();
	menu_names = find_files();

	for (int i = 0; i < files_count; i++)
		menu_names[i] = txt_names[i].substr(0, txt_names[i].length() - 4);

	int hover = 0;
	int button_w = 200, button_h = 30;
	int pos_x = (r_x - button_w) / 2, pos_y1 = 40, pos_y2 = 80, pos_y3 = 120, pos_y4 = 160, pos_y5 = 200, pos_y6 = 240, pos_y7 = 280;
	int pos_y8 = 320, pos_y9 = 360, pos_y10 = 400;

	int pos_x_back = 40, pos_y_back = 40;

	menu_queue = al_create_event_queue();

	al_register_event_source(menu_queue, al_get_display_event_source(display));
	al_register_event_source(menu_queue, al_get_keyboard_event_source());
	al_register_event_source(menu_queue, al_get_mouse_event_source());

	while (1) {
		ALLEGRO_EVENT menu_event;

		al_wait_for_event(menu_queue, &menu_event);

		if (menu_event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) { //EXIT
			if_exit = true;
			return "-1";
		}
		else if (menu_event.type == ALLEGRO_EVENT_KEY_DOWN) {
			switch (menu_event.keyboard.keycode) {
				case ALLEGRO_KEY_UP:
					hover--;
					break;
				case ALLEGRO_KEY_DOWN:
					hover++;
					break;
				case ALLEGRO_KEY_LEFT:
					if (hover == 1)
						hover = 11;
					break;
				case ALLEGRO_KEY_RIGHT:
					if (hover == 11)
						hover = 1;
					break;

				case ALLEGRO_KEY_ENTER:
					if (hover != 0) {
						if (hover == 11)
							return "-1";
						else
							return return_val;
					}
					break;
				case ALLEGRO_KEY_ESCAPE:
					return "-1";
					break;
			}
			if (hover < 1)
				hover = 1;
			if (hover > 11)
				hover = 11;

			if (hover > 0 && hover < files_count + 1)
				return_val = txt_names[hover - 1];
		}
		else if (menu_event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN && hover != 0) {
			if (hover == 11)
				return "-1";
			else
				return return_val;
		}
		else if (menu_event.type == ALLEGRO_EVENT_MOUSE_AXES) {
			if (menu_event.mouse.x > pos_x && menu_event.mouse.x < pos_x + button_w) {
				if (menu_event.mouse.y > pos_y1 && menu_event.mouse.y < pos_y1 + button_h)
					hover = 1;
				else if (menu_event.mouse.y > pos_y2 && menu_event.mouse.y < pos_y2 + button_h)
					hover = 2;
				else if (menu_event.mouse.y > pos_y3 && menu_event.mouse.y < pos_y3 + button_h)
					hover = 3;
				else if (menu_event.mouse.y > pos_y4 && menu_event.mouse.y < pos_y4 + button_h)
					hover = 4;
				else if (menu_event.mouse.y > pos_y5 && menu_event.mouse.y < pos_y5 + button_h)
					hover = 5;
				else if (menu_event.mouse.y > pos_y6 && menu_event.mouse.y < pos_y6 + button_h)
					hover = 6;
				else if (menu_event.mouse.y > pos_y7 && menu_event.mouse.y < pos_y7 + button_h)
					hover = 7;
				else if (menu_event.mouse.y > pos_y8 && menu_event.mouse.y < pos_y8 + button_h)
					hover = 8;
				else if (menu_event.mouse.y > pos_y9 && menu_event.mouse.y < pos_y9 + button_h)
					hover = 9;
				else if (menu_event.mouse.y > pos_y10 && menu_event.mouse.y < pos_y10 + button_h)
					hover = 10;
				else
					hover = 0;
			}
			else if (menu_event.mouse.y > pos_y_back && menu_event.mouse.y < pos_y_back + button_h && menu_event.mouse.x > pos_x_back && menu_event.mouse.x < pos_x_back + button_w * 2 / 3)
				hover = 11;
			else
				hover = 0;

			if (hover > 0 && hover < files_count + 1)
				return_val = txt_names[hover - 1];
		}

		if (al_is_event_queue_empty(menu_queue)) {
			al_draw_bitmap(background, 0, 0, 0);

			al_draw_rectangle(pos_x_back, pos_y_back, pos_x_back + button_w * 2 / 3, pos_y_back + button_h, al_map_rgb(0, 0, 0), 1);
			al_draw_text(menu_font, al_map_rgb(0, 0, 0), (pos_x_back + button_w) / 2.5, pos_y_back + button_h / 4, ALLEGRO_ALIGN_CENTRE, "Back");

			al_draw_rectangle(pos_x, pos_y1, pos_x + button_w, pos_y1 + button_h, al_map_rgb(0, 0, 0), 1);
			al_draw_textf(menu_font, al_map_rgb(0, 0, 0), r_x / 2, pos_y1 + button_h / 4, ALLEGRO_ALIGN_CENTRE, "%s", menu_names[0].c_str());

			al_draw_rectangle(pos_x, pos_y1, pos_x + button_w, pos_y1 + button_h, al_map_rgb(0, 0, 0), 1);
			al_draw_textf(menu_font, al_map_rgb(0, 0, 0), r_x / 2, pos_y1 + button_h / 4, ALLEGRO_ALIGN_CENTRE, "%s", menu_names[0].c_str());

			al_draw_rectangle(pos_x, pos_y2, pos_x + button_w, pos_y2 + button_h, al_map_rgb(0, 0, 0), 1);
			al_draw_textf(menu_font, al_map_rgb(0, 0, 0), r_x / 2, pos_y2 + button_h / 4, ALLEGRO_ALIGN_CENTRE, "%s", menu_names[1].c_str());

			al_draw_rectangle(pos_x, pos_y3, pos_x + button_w, pos_y3 + button_h, al_map_rgb(0, 0, 0), 1);
			al_draw_textf(menu_font, al_map_rgb(0, 0, 0), r_x / 2, pos_y3 + button_h / 4, ALLEGRO_ALIGN_CENTRE, "%s", menu_names[2].c_str());

			al_draw_rectangle(pos_x, pos_y4, pos_x + button_w, pos_y4 + button_h, al_map_rgb(0, 0, 0), 1);
			al_draw_textf(menu_font, al_map_rgb(0, 0, 0), r_x / 2, pos_y4 + button_h / 4, ALLEGRO_ALIGN_CENTRE, "%s", menu_names[3].c_str());

			al_draw_rectangle(pos_x, pos_y5, pos_x + button_w, pos_y5 + button_h, al_map_rgb(0, 0, 0), 1);
			al_draw_textf(menu_font, al_map_rgb(0, 0, 0), r_x / 2, pos_y5 + button_h / 4, ALLEGRO_ALIGN_CENTRE, "%s", menu_names[4].c_str());

			al_draw_rectangle(pos_x, pos_y6, pos_x + button_w, pos_y6 + button_h, al_map_rgb(0, 0, 0), 1);
			al_draw_textf(menu_font, al_map_rgb(0, 0, 0), r_x / 2, pos_y6 + button_h / 4, ALLEGRO_ALIGN_CENTRE, "%s", menu_names[5].c_str());

			al_draw_rectangle(pos_x, pos_y7, pos_x + button_w, pos_y7 + button_h, al_map_rgb(0, 0, 0), 1);
			al_draw_textf(menu_font, al_map_rgb(0, 0, 0), r_x / 2, pos_y7 + button_h / 4, ALLEGRO_ALIGN_CENTRE, "%s", menu_names[6].c_str());

			al_draw_rectangle(pos_x, pos_y8, pos_x + button_w, pos_y8 + button_h, al_map_rgb(0, 0, 0), 1);
			al_draw_textf(menu_font, al_map_rgb(0, 0, 0), r_x / 2, pos_y8 + button_h / 4, ALLEGRO_ALIGN_CENTRE, "%s", menu_names[7].c_str());

			al_draw_rectangle(pos_x, pos_y9, pos_x + button_w, pos_y9 + button_h, al_map_rgb(0, 0, 0), 1);
			al_draw_textf(menu_font, al_map_rgb(0, 0, 0), r_x / 2, pos_y9 + button_h / 4, ALLEGRO_ALIGN_CENTRE, "%s", menu_names[8].c_str());

			al_draw_rectangle(pos_x, pos_y10, pos_x + button_w, pos_y10 + button_h, al_map_rgb(0, 0, 0), 1);
			al_draw_textf(menu_font, al_map_rgb(0, 0, 0), r_x / 2, pos_y10 + button_h / 4, ALLEGRO_ALIGN_CENTRE, "%s", menu_names[9].c_str());

			if (hover == 1) {
				al_draw_filled_rectangle(pos_x, pos_y1, pos_x + button_w, pos_y1 + button_h, al_map_rgb(0, 0, 0));
				al_draw_textf(menu_font, al_map_rgb(255, 255, 255), r_x / 2, pos_y1 + button_h / 4, ALLEGRO_ALIGN_CENTRE, "%s", menu_names[0].c_str());
			}

			if (hover == 2) {
				al_draw_filled_rectangle(pos_x, pos_y2, pos_x + button_w, pos_y2 + button_h, al_map_rgb(0, 0, 0));
				al_draw_textf(menu_font, al_map_rgb(255, 255, 255), r_x / 2, pos_y2 + button_h / 4, ALLEGRO_ALIGN_CENTRE, "%s", menu_names[1].c_str());
			}

			if (hover == 3) {
				al_draw_filled_rectangle(pos_x, pos_y3, pos_x + button_w, pos_y3 + button_h, al_map_rgb(0, 0, 0));
				al_draw_textf(menu_font, al_map_rgb(255, 255, 255), r_x / 2, pos_y3 + button_h / 4, ALLEGRO_ALIGN_CENTRE, "%s", menu_names[2].c_str());
			}
			if (hover == 4) {
				al_draw_filled_rectangle(pos_x, pos_y4, pos_x + button_w, pos_y4 + button_h, al_map_rgb(0, 0, 0));
				al_draw_textf(menu_font, al_map_rgb(255, 255, 255), r_x / 2, pos_y4 + button_h / 4, ALLEGRO_ALIGN_CENTRE, "%s", menu_names[3].c_str());
			}
			if (hover == 5) {
				al_draw_filled_rectangle(pos_x, pos_y5, pos_x + button_w, pos_y5 + button_h, al_map_rgb(0, 0, 0));
				al_draw_textf(menu_font, al_map_rgb(255, 255, 255), r_x / 2, pos_y5 + button_h / 4, ALLEGRO_ALIGN_CENTRE, "%s", menu_names[4].c_str());
			}
			if (hover == 6) {
				al_draw_filled_rectangle(pos_x, pos_y6, pos_x + button_w, pos_y6 + button_h, al_map_rgb(0, 0, 0));
				al_draw_textf(menu_font, al_map_rgb(255, 255, 255), r_x / 2, pos_y6 + button_h / 4, ALLEGRO_ALIGN_CENTRE, "%s", menu_names[5].c_str());
			}
			if (hover == 7) {
				al_draw_filled_rectangle(pos_x, pos_y7, pos_x + button_w, pos_y7 + button_h, al_map_rgb(0, 0, 0));
				al_draw_textf(menu_font, al_map_rgb(255, 255, 255), r_x / 2, pos_y7 + button_h / 4, ALLEGRO_ALIGN_CENTRE, "%s", menu_names[6].c_str());
			}
			if (hover == 8) {
				al_draw_filled_rectangle(pos_x, pos_y8, pos_x + button_w, pos_y8 + button_h, al_map_rgb(0, 0, 0));
				al_draw_textf(menu_font, al_map_rgb(255, 255, 255), r_x / 2, pos_y8 + button_h / 4, ALLEGRO_ALIGN_CENTRE, "%s", menu_names[7].c_str());
			}
			if (hover == 9) {
				al_draw_filled_rectangle(pos_x, pos_y9, pos_x + button_w, pos_y9 + button_h, al_map_rgb(0, 0, 0));
				al_draw_textf(menu_font, al_map_rgb(255, 255, 255), r_x / 2, pos_y9 + button_h / 4, ALLEGRO_ALIGN_CENTRE, "%s", menu_names[8].c_str());
			}
			if (hover == 10) {
				al_draw_filled_rectangle(pos_x, pos_y10, pos_x + button_w, pos_y10 + button_h, al_map_rgb(0, 0, 0));
				al_draw_textf(menu_font, al_map_rgb(255, 255, 255), r_x / 2, pos_y10 + button_h / 4, ALLEGRO_ALIGN_CENTRE, "%s", menu_names[9].c_str());
			}
			if (hover == 11) {
				al_draw_filled_rectangle(pos_x_back, pos_y_back, pos_x_back + button_w * 2 / 3, pos_y_back + button_h, al_map_rgb(0, 0, 0));
				al_draw_text(menu_font, al_map_rgb(255, 255, 255), (pos_x_back + button_w) / 2.5, pos_y_back + button_h / 4, ALLEGRO_ALIGN_CENTRE, "Back");
			}

			al_flip_display();
		}
	}

	al_destroy_event_queue(menu_queue);
	al_destroy_bitmap(background);
}

void draw_keyboard(ALLEGRO_FONT *font, char used_letters[11], string decrypted) {
	char character[26] = { 'q','w','e','r','t','y','u','i','o','p','a','s','d','f','g','h','j','k','l','z','x','c','v','b','n','m' };

	int char_pos_y1 = 250, char_pos_y2 = 300, char_pos_y3 = 350, char_pos_x1 = 325, char_pos_x2 = 345, char_pos_x3 = 375;

	bool white = false;

	for (int i = 0; i < 10; i++) {
		for (int g = 0; g < 11; g++) {
			if (character[i] == used_letters[g]) {
				white = true;
				break;
			}
		}
		for (unsigned int g = 0; g < decrypted.length(); g++) {
			if (character[i] == decrypted[g]) {
				white = true;
				break;
			}
		}
		if (white == true)
			al_draw_textf(font, al_map_rgb(180, 180, 180), char_pos_x1, char_pos_y1, ALLEGRO_ALIGN_CENTRE, "%c", character[i]);
		else
			al_draw_textf(font, al_map_rgb(0, 0, 0), char_pos_x1, char_pos_y1, ALLEGRO_ALIGN_CENTRE, "%c", character[i]);

		char_pos_x1 = char_pos_x1 + 30;
		white = false;
	}
	for (int i = 10; i < 19; i++) {
		for (int g = 0; g < 11; g++) {
			if (character[i] == used_letters[g]) {
				white = true;
				break;
			}
		}
		for (unsigned int g = 0; g < decrypted.length(); g++) {
			if (character[i] == decrypted[g]) {
				white = true;
				break;
			}
		}
		if (white == true)
			al_draw_textf(font, al_map_rgb(180, 180, 180), char_pos_x2, char_pos_y2, ALLEGRO_ALIGN_CENTRE, "%c", character[i]);
		else
			al_draw_textf(font, al_map_rgb(0, 0, 0), char_pos_x2, char_pos_y2, ALLEGRO_ALIGN_CENTRE, "%c", character[i]);

		char_pos_x2 = char_pos_x2 + 30;
		white = false;
	}
	for (int i = 19; i < 26; i++) {
		for (int g = 0; g < 11; g++) {
			if (character[i] == used_letters[g]) {
				white = true;
				break;
			}
		}
		for (unsigned int g = 0; g < decrypted.length(); g++) {
			if (character[i] == decrypted[g]) {
				white = true;
				break;
			}
		}
		if (white == true)
			al_draw_textf(font, al_map_rgb(180, 180, 180), char_pos_x3, char_pos_y3, ALLEGRO_ALIGN_CENTRE, "%c", character[i]);
		else
			al_draw_textf(font, al_map_rgb(0, 0, 0), char_pos_x3, char_pos_y3, ALLEGRO_ALIGN_CENTRE, "%c", character[i]);

		char_pos_x3 = char_pos_x3 + 30;
		white = false;
	}
}

char check_where_clicked(int m_x, int m_y) {
	char character[26] = { 'q','w','e','r','t','y','u','i','o','p','a','s','d','f','g','h','j','k','l','z','x','c','v','b','n','m' };
	char tmp = ' ';
	bool click_on_keyboard = false;

	int char_pos_y1 = 250, char_pos_y2 = 300, char_pos_y3 = 350, char_pos_x1 = 325, char_pos_x2 = 345, char_pos_x3 = 375;

	for (int i = 0; i < 10; i++) {
		if (m_x > char_pos_x1 && m_x < char_pos_x1 + 30 && m_y > char_pos_y1 && m_y < char_pos_y1 + 35) {
			click_on_keyboard = true;
			return character[i];
		}
		char_pos_x1 = char_pos_x1 + 28;
	}
	for (int i = 10; i < 19; i++) {
		if (m_x > char_pos_x2 && m_x < char_pos_x2 + 30 && m_y > char_pos_y2 && m_y < char_pos_y2 + 35) {
			click_on_keyboard = true;
			return character[i];
		}
		char_pos_x2 = char_pos_x2 + 28;
	}
	for (int i = 19; i < 26; i++) {
		if (m_x > char_pos_x3 && m_x < char_pos_x3 + 30 && m_y > char_pos_y3 && m_y < char_pos_y3 + 35) {
			click_on_keyboard = true;
			return character[i];
		}
		char_pos_x3 = char_pos_x3 + 28;
	}
	if (click_on_keyboard == false)
		return ' ';
}

void game(int r_x, int r_y, string file_name) {

	if (file_name == "Files/Words/-1")
		return;

	bool if_guessed_letter = false;
	bool escape = false;
	bool guessed_word = false;
	bool next_round = true;

	string temp = "";
	string word_to_guess = "";
	string decrypted_word = "";

	int chances_left = 11;
	int temp2 = 0;
	int number_of_words = 0;
	int unicode = 0;

	char entered_letter = ' ';

	fstream file_to_open;
	file_to_open.open(file_name);

	ALLEGRO_FONT *keyboard_font = al_load_ttf_font("Files/Chocolate Covered Raindrops.ttf", 45, 0);
	ALLEGRO_FONT *game_font = al_load_ttf_font("Files/Chocolate Covered Raindrops.ttf", 35, 0);
	ALLEGRO_FONT *game_small_font = al_load_ttf_font("Files/Chocolate Covered Raindrops.ttf", 25, 0);

	ALLEGRO_BITMAP *hangman = NULL;
	ALLEGRO_BITMAP *background = NULL;

	ALLEGRO_EVENT_QUEUE *game_queue = NULL;
	ALLEGRO_EVENT game_event;

	game_queue = al_create_event_queue();

	al_register_event_source(game_queue, al_get_display_event_source(display));
	al_register_event_source(game_queue, al_get_keyboard_event_source());
	al_register_event_source(game_queue, al_get_mouse_event_source());

	////////////////////////////////////////////////////////////////////////

	background = al_load_bitmap("Files/img/background.png");
	hangman = al_load_bitmap("Files/img/hangman0.png");

	int *stats = new int[2];
	stats = high_score_load();
	int number_of_guessed_words = stats[0];
	int number_of_missed_words = stats[1];

	while (getline(file_to_open, temp))
		number_of_words++;

	string *words = new string[number_of_words];

	file_to_open.clear();
	file_to_open.seekg(0);

	while (!file_to_open.eof()) {
		getline(file_to_open, words[temp2]);
		temp2++;
	}

	file_to_open.close();

	while (next_round == true) {
		char used_letters[12] = { ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ' };
		next_round = false;
		if_guessed_letter = false;
		guessed_word = false;
		chances_left = 11;
		hangman = al_load_bitmap("Files/img/hangman0.png");

		word_to_guess = words[rand() % number_of_words];

		decrypted_word = word_to_guess;

		for (unsigned int i = 0; i < word_to_guess.length(); i++)
			if (decrypted_word[i] != ' ')
				decrypted_word[i] = '*';

		////////////////////////////////////////////////////////////////////////

		while ((guessed_word != true && chances_left > 0) || next_round == false) {
			al_wait_for_event(game_queue, &game_event);

			if (game_event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
				chances_left = 0;
				if_exit = true;
				next_round = false;
				break;
			}
			else if (game_event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
				if (game_event.mouse.button == 2) {
					next_round = false;
					chances_left = 0;
					break;
				}


				if (guessed_word == true || chances_left <= 0)
					next_round = true;
				else
					next_round = false;

				if (guessed_word == false && chances_left > 0) {
					entered_letter = check_where_clicked(game_event.mouse.x, game_event.mouse.y);

					if (isalpha(entered_letter)) {
						for (unsigned int i = 0; i < word_to_guess.length(); i++) {
							if (entered_letter == tolower(word_to_guess[i])) {
								decrypted_word[i] = entered_letter;
								if_guessed_letter = true;
							}
						}
						if (if_guessed_letter == false) {
							bool if_previously_entered = false;

							for (int i = 0;i<11 - chances_left;i++) {
								if (used_letters[i] == entered_letter)
									if_previously_entered = true;
							}

							if (if_previously_entered == false) {
								used_letters[11 - chances_left] = entered_letter;
								chances_left--;

								if (11 - chances_left == 1)
									hangman = al_load_bitmap("Files/img/hangman1.png");
								else if (11 - chances_left == 2)
									hangman = al_load_bitmap("Files/img/hangman2.png");
								else if (11 - chances_left == 3)
									hangman = al_load_bitmap("Files/img/hangman3.png");
								else if (11 - chances_left == 4)
									hangman = al_load_bitmap("Files/img/hangman4.png");
								else if (11 - chances_left == 5)
									hangman = al_load_bitmap("Files/img/hangman5.png");
								else if (11 - chances_left == 6)
									hangman = al_load_bitmap("Files/img/hangman6.png");
								else if (11 - chances_left == 7)
									hangman = al_load_bitmap("Files/img/hangman7.png");
								else if (11 - chances_left == 8)
									hangman = al_load_bitmap("Files/img/hangman8.png");
								else if (11 - chances_left == 9)
									hangman = al_load_bitmap("Files/img/hangman9.png");
								else if (11 - chances_left == 10)
									hangman = al_load_bitmap("Files/img/hangman10.png");
								else if (11 - chances_left == 11)
									hangman = al_load_bitmap("Files/img/hangman11.png");
							}
						}
					}

					if_guessed_letter = false;

					for (unsigned int i = 0; i < decrypted_word.length(); i++) {
						if (decrypted_word[i] == '*') {
							guessed_word = false;
							break;
						}
						else {
							guessed_word = true;
						}
					}
				}
			}
			else if (game_event.type == ALLEGRO_EVENT_KEY_CHAR) {
				unicode = game_event.keyboard.unichar;

				if (unicode == 27) { //ESC
					next_round = false;
					chances_left = 0;
					break;
				}
				else if (unicode == 13) { //ENTER
					if (guessed_word == true || chances_left <= 0)
						next_round = true;
					else
						next_round = false;
				}
				if (guessed_word == false && chances_left > 0) {
					entered_letter = (char)unicode;

					if (isalpha(entered_letter)) {
						for (unsigned int i = 0; i < word_to_guess.length(); i++) {
							if (tolower(entered_letter) == tolower(word_to_guess[i])) {
								decrypted_word[i] = entered_letter;
								if_guessed_letter = true;
							}
						}

						if (if_guessed_letter == false) {
							bool if_previously_entered = false;

							for (int i = 0;i<11 - chances_left;i++) {
								if (used_letters[i] == entered_letter)
									if_previously_entered = true;
							}

							if (if_previously_entered == false) {
								used_letters[11 - chances_left] = entered_letter;
								chances_left--;

								if (11 - chances_left == 1)
									hangman = al_load_bitmap("Files/img/hangman1.png");
								else if (11 - chances_left == 2)
									hangman = al_load_bitmap("Files/img/hangman2.png");
								else if (11 - chances_left == 3)
									hangman = al_load_bitmap("Files/img/hangman3.png");
								else if (11 - chances_left == 4)
									hangman = al_load_bitmap("Files/img/hangman4.png");
								else if (11 - chances_left == 5)
									hangman = al_load_bitmap("Files/img/hangman5.png");
								else if (11 - chances_left == 6)
									hangman = al_load_bitmap("Files/img/hangman6.png");
								else if (11 - chances_left == 7)
									hangman = al_load_bitmap("Files/img/hangman7.png");
								else if (11 - chances_left == 8)
									hangman = al_load_bitmap("Files/img/hangman8.png");
								else if (11 - chances_left == 9)
									hangman = al_load_bitmap("Files/img/hangman9.png");
								else if (11 - chances_left == 10)
									hangman = al_load_bitmap("Files/img/hangman10.png");
								else if (11 - chances_left == 11)
									hangman = al_load_bitmap("Files/img/hangman11.png");
							}
						}

						if_guessed_letter = false;

						for (unsigned int i = 0; i < decrypted_word.length(); i++) {
							if (decrypted_word[i] == '*') {
								guessed_word = false;
								break;
							}
							else {
								guessed_word = true;
							}
						}
					}
				}
			}
			if (al_is_event_queue_empty(game_queue)) {
				al_draw_bitmap(background, 0, 0, 0);
				al_draw_bitmap(hangman, 0, 0, 0);

				draw_keyboard(keyboard_font, used_letters, decrypted_word);

				al_draw_textf(game_small_font, al_map_rgb(0, 0, 0), (r_x - al_get_bitmap_width(hangman)) + 40, 30, ALLEGRO_ALIGN_CENTRE, "Guessed: %i", number_of_guessed_words);
				al_draw_textf(game_small_font, al_map_rgb(0, 0, 0), (r_x - al_get_bitmap_width(hangman)) + 180, 30, ALLEGRO_ALIGN_CENTRE, "Not guessed: %i", number_of_missed_words);
				al_draw_textf(game_font, al_map_rgb(0, 0, 0), (r_x - al_get_bitmap_width(hangman)) * 4 / 3, 75, ALLEGRO_ALIGN_CENTRE, "Category: %s", file_name.substr(12, file_name.length() - 12 - 4).c_str());
				al_draw_textf(game_font, al_map_rgb(0, 0, 0), (r_x - al_get_bitmap_width(hangman)) * 4 / 3, 135, ALLEGRO_ALIGN_CENTRE, "Word: %s", decrypted_word.c_str());
				if (chances_left <= 0)
					al_draw_textf(game_font, al_map_rgb(0, 0, 0), (r_x - al_get_bitmap_width(hangman)) * 4 / 3, 195, ALLEGRO_ALIGN_CENTRE, "Full Word: %s", word_to_guess.c_str());

				al_flip_display();
			}
		}
		if (next_round == true && chances_left <= 0)
			number_of_missed_words++;
		if (next_round == true && chances_left > 0)
			number_of_guessed_words++;

		high_score_save(number_of_guessed_words, number_of_missed_words);
	}
	delete[]words;
	al_destroy_bitmap(hangman);
	al_destroy_bitmap(background);
	al_destroy_font(keyboard_font);
	al_destroy_font(game_font);
}

int main() {
	const int res_x = 640, res_y = 480;

	srand(time(NULL));

	if (install_allegro(res_x, res_y) == -1)
		return 0;

	while (if_exit == false) {
		int choice = menu(res_x, res_y);

		if (choice == 1) {
			game(res_x, res_y, "Files/Words/" + category_menu(res_x, res_y));
		}
		else if (choice == 2) {
			break;
		}
	}
	al_destroy_display(display);
}