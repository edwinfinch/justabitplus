#include <pebble.h>
#include "elements.h"
	
void glance_this(int sentence, bool vibrate, int vibrateNum, int animationLength, bool fullNotify);

#define CIRCLE_RADIUS 12
#define CIRCLE_LINE_THICKNESS 1

void on_animation_stopped(Animation *anim, bool finished, void *context)
{
    //Free the memoery used by the Animation
    property_animation_destroy((PropertyAnimation*) anim);
	animationNumber++;
	
	if(animationNumber > 1 && booted == 1){
		currentlyGlancing = 0;
		animationNumber = 0;
	}
}
 
void animate_layer(Layer *layer, GRect *start, GRect *finish, int duration, int delay)
{
    	//Declare animation
    	PropertyAnimation *anim = property_animation_create_layer_frame(layer, start, finish);
     
    	//Set characteristics
    	animation_set_duration((Animation*) anim, duration);
    	animation_set_delay((Animation*) anim, delay);
     
    	//Set stopped handler to free memory
		AnimationHandlers handlers = {
        	//The reference to the stopped handler is the only one in the array
        .stopped = (AnimationStoppedHandler) on_animation_stopped
        };
    	animation_set_handlers((Animation*) anim, handlers, NULL);
     
    	//Start animation
    	animation_schedule((Animation*) anim);
}

	
static TextLayer* textLayerInit(GRect location, GColor colour, GColor background, GTextAlignment alignment)
{
	TextLayer *layer = text_layer_create(location);
	text_layer_set_text_color(layer, colour);
	text_layer_set_background_color(layer, background);
	text_layer_set_text_alignment(layer, alignment);

	return layer;
}
	
void process_tuple(Tuple *t)
{
	//Get key
	int key = t->key;

	//Get integer value
	int value = t->value->int32;

	//Get string value
	char string_value[32];
	strcpy(string_value, t->value->cstring);
  switch (key) {
	  case THEME_KEY:
		  if(value == 5){
			  layer_set_hidden(inverter_layer_get_layer(theme), false);
			  settings.theme = 1;
		  }
		  else if(value == 4){
			  layer_set_hidden(inverter_layer_get_layer(theme), true);
			  settings.theme = 0;
		  }
	    glance_this(1, 1, 2, 5000, 0);
	  	break;
	  case BTREALERT_KEY:
		  if(value == 4){
			  settings.btReAlert = 0;
		  }
		  else if(value == 5){
			  settings.btReAlert = 1;
		  }
	  	  else if(value == 6){
			  settings.btReAlert = 2;
		  }
	  	break;
	  case BTDISALERT_KEY:
	  	  if(value == 4){
			  settings.btDisAlert = 0;
		  }
		  else if(value == 5){
			  settings.btDisAlert = 1;
		  }
	  	  else if(value == 6){
			  settings.btDisAlert = 2;
		  }
	  	break;
	  case SHOWSEC_KEY:
		  if(value == 4){
			settings.showSec = 0;
			settings.rows = 4;
		  }
		  else if(value == 5){
			 settings.showSec = 1;
			 settings.rows = 6;
		  }
	  	break;
  }
}

//When we get a message from the phone
static void in_received_handler(DictionaryIterator *iter, void *context) 
{
	(void) context;

	//Get data
	Tuple *t = dict_read_first(iter);
	if(t)
	{
		process_tuple(t);
	}
	//Get next
	while(t != NULL)
	{
		t = dict_read_next(iter);
		if(t)
		{
			//Process that data
			process_tuple(t);
		}
	}
}
	
void draw_cell(GContext* ctx, GPoint center, bool filled) {
	  graphics_context_set_fill_color(ctx, GColorWhite);
	
	  graphics_fill_circle(ctx, center, CIRCLE_RADIUS);
	
	  if (!filled) {
		graphics_context_set_fill_color(ctx, GColorBlack);
		graphics_fill_circle(ctx, center, CIRCLE_RADIUS - CIRCLE_LINE_THICKNESS);
	  }
}

GPoint get_center_point_from_cell_location(unsigned short x, unsigned short y) {
	return GPoint(SIDE_PADDING + (CELL_SIZE/2) + (CELL_SIZE * x),
			(CELL_SIZE/2) + (CELL_SIZE * y));
	}
	
	void draw_cell_row_for_digit(GContext* ctx, unsigned short digit, unsigned short max_columns_to_display, unsigned short cell_row) {
	  for (int cell_column_index = 0; cell_column_index < max_columns_to_display; cell_column_index++) {
		draw_cell(ctx, get_center_point_from_cell_location(cell_column_index, cell_row), (digit >> cell_column_index) & 0x1);
	}
}

unsigned short get_display_hour(unsigned short hour) {
	if (clock_is_24h_style()) {
		return hour;
	}
	
	unsigned short display_hour = hour % 12;
	return display_hour ? display_hour : 12;
}

void display_layer_update_callback(Layer *me, GContext* ctx) {
	time_t now = time(NULL);
	struct tm *t = localtime(&now);
	
	unsigned short display_hour = get_display_hour(t->tm_hour);
	if(settings.showSec == 1){
		draw_cell_row_for_digit(ctx, display_hour / 10, HOURS_FIRST_DIGIT_MAX_COLS, HOURS_FIRST_DIGIT_ROW);
		draw_cell_row_for_digit(ctx, display_hour % 10, DEFAULT_MAX_COLS, HOURS_SECOND_DIGIT_ROW);
		
		draw_cell_row_for_digit(ctx, t->tm_min / 10, MINUTES_FIRST_DIGIT_MAX_COLS, MINUTES_FIRST_DIGIT_ROW);
		draw_cell_row_for_digit(ctx, t->tm_min % 10, DEFAULT_MAX_COLS, MINUTES_SECOND_DIGIT_ROW);
		
		draw_cell_row_for_digit(ctx, t->tm_sec / 10, SECONDS_FIRST_DIGIT_MAX_COLS, SECONDS_FIRST_DIGIT_ROW);
		draw_cell_row_for_digit(ctx, t->tm_sec % 10, DEFAULT_MAX_COLS, SECONDS_SECOND_DIGIT_ROW);
	}
	else{
		draw_cell_row_for_digit(ctx, display_hour / 10, HOURS_FIRST_DIGIT_MAX_COLS, HOURS_SECOND_DIGIT_ROW);
		draw_cell_row_for_digit(ctx, display_hour % 10, DEFAULT_MAX_COLS, MINUTES_FIRST_DIGIT_ROW);
		
		draw_cell_row_for_digit(ctx, t->tm_min / 10, MINUTES_FIRST_DIGIT_MAX_COLS, MINUTES_SECOND_DIGIT_ROW);
		draw_cell_row_for_digit(ctx, t->tm_min % 10, DEFAULT_MAX_COLS, SECONDS_FIRST_DIGIT_ROW);
	}
}

void handle_second_tick(struct tm *tick_time, TimeUnits units_changed) {
  	layer_mark_dirty(display_layer);
}

void add_children(){
	Layer *window_layer = window_get_root_layer(window);
	layer_add_child(window_layer, display_layer);
	layer_add_child(window_layer, (Layer*) update_at_a_glance);
	layer_add_child(window_layer, (Layer*) theme);
}

void window_load(Window *window){
	Layer *root_layer = window_get_root_layer(window);
	GRect frame = layer_get_frame(root_layer);		
	update_at_a_glance = textLayerInit(GRect(0, 300, 144, 168), GColorBlack, GColorWhite, GTextAlignmentCenter);
	text_layer_set_font(update_at_a_glance, fonts_get_system_font(FONT_KEY_GOTHIC_18));
	theme = inverter_layer_create(GRect(0, 0, 144, 168));
	display_layer = layer_create(frame);
	layer_set_update_proc(display_layer, &display_layer_update_callback);
	add_children();
	if(settings.theme == 0){
		layer_set_hidden(inverter_layer_get_layer(theme), true);
		APP_LOG(APP_LOG_LEVEL_WARNING, "False!");
	}
}

void window_unload(Window *window){
	layer_destroy(display_layer);
	inverter_layer_destroy(theme);
	text_layer_destroy(update_at_a_glance);
}

void bluetooth_handler(bool connected){
	if(connected == 1){
		if(settings.btReAlert == 1){
			vibes_double_pulse();
		}
		else if(settings.btReAlert == 2){
			glance_this(3, 1, 2, 5000, 0);
		}
	}
	else{
		if(settings.btDisAlert == 1){
			vibes_long_pulse();
		}
		else if(settings.btDisAlert == 2){
			glance_this(2, 1, 3, 5000, 0);
		}
	}
}

static void do_init(void) {
	window = window_create();
	window_set_window_handlers(window, (WindowHandlers){
		.load = window_load,
		.unload = window_unload,
	});
	app_message_register_inbox_received(in_received_handler);					 
	app_message_open(512, 512);
	int valueRead;
	valueRead = persist_read_data(SETTINGS_KEY, &settings, sizeof(settings));
	APP_LOG(APP_LOG_LEVEL_INFO, "Read %d byte from settings", valueRead);
	
	tick_timer_service_subscribe(SECOND_UNIT, &handle_second_tick);
	bluetooth_connection_service_subscribe(&bluetooth_handler);
	window_stack_push(window, true);
	window_set_background_color(window, GColorBlack);
}

static void do_deinit(void) {
	int valueWritten;
	valueWritten = persist_write_data(SETTINGS_KEY, &settings, sizeof(settings));
	APP_LOG(APP_LOG_LEVEL_INFO, "Wrote %d bytes into persistent storage", valueWritten);
	app_timer_cancel(glanceTimer);
	window_destroy(window);
}


int main(void) {
	do_init();
	app_event_loop();
	do_deinit();
}

void glanceTimerCallback(void *data){
	if(currentlyGlancing == 1){
		glanceTimer = app_timer_register(500, (AppTimerCallback) glanceTimerCallback, NULL);
	}
	else{
		APP_LOG(APP_LOG_LEVEL_INFO, "Glance_this free, sending request...");
		glance_this(holdUpSentence, holdUpVibrate, holdUpVibrateNum, holdUpAnimationLength, holdUpFullNotify);
	}
	glanceTimerCalled++;
	if(glanceTimerCalled > 2){
		currentlyGlancing = 0;
		glanceTimerCalled = 0;
	}
}

void glance_this(int sentence, bool vibrate, int vibrateNum, int animationLength, bool fullNotify){
	//Update the text layer to the char provided by function call
	if(currentlyGlancing == 1){
		holdUpSentence = sentence;
		holdUpVibrate = vibrate;
		holdUpVibrateNum = vibrateNum;
		holdUpAnimationLength = animationLength;
		holdUpFullNotify = fullNotify;
		glanceTimer = app_timer_register(5000, (AppTimerCallback) glanceTimerCallback, NULL);
	}
	
	else if(currentlyGlancing == 0){
			//if there's a vibration request,
			if(vibrate == true){
					//check what number it is and fufill accordingly.
					if(vibrateNum == 1){
						vibes_short_pulse();
		            }
					else if(vibrateNum == 2){
						vibes_double_pulse();
		            }
					else if(vibrateNum == 3){
						vibes_long_pulse();
		            }
			}
				else{
				//Incase of future need
			    }
			//Update text and animate update_at_a_glance layer for fancy effect
			if(sentence == 1){
				text_layer_set_text(update_at_a_glance, "Settings updated.");
			}
			else if(sentence == 2){
				text_layer_set_text(update_at_a_glance, "Bluetooth disconnected.");
			}
			else if(sentence == 3){
				text_layer_set_text(update_at_a_glance, "Bluetooth reconnected.");
			}
			else if(sentence == 4){
				text_layer_set_text(update_at_a_glance, "Weather updated.");
			}
			currentlyGlancing = 1;
				if(fullNotify == 1){
					APP_LOG(APP_LOG_LEVEL_DEBUG, "Glance_this: Full notification");
					text_layer_set_background_color(update_at_a_glance, GColorWhite);
					GRect start01 = GRect(0, 300, 144, 168);
					GRect finish01 = GRect(0, 70, 144, 168);
					GRect start02 = GRect(0, 70, 144, 168);
					GRect finish02 = GRect(0, 300, 144, 168);
					animate_layer(text_layer_get_layer(update_at_a_glance), &start01, &finish01, 1000, 0);
					animate_layer(text_layer_get_layer(update_at_a_glance), &start02, &finish02, 1000, animationLength);
		        }
				else{
					APP_LOG(APP_LOG_LEVEL_DEBUG, "Glance_this: Not a full notification");
					text_layer_set_background_color(update_at_a_glance, GColorWhite);
					GRect start01 = GRect(0, 300, 144, 168);
					GRect finish01 = GRect(0, 145, 144, 168);
					GRect start02 = GRect(0, 145, 144, 168);
					GRect finish02 = GRect(0, 300, 144, 168);
					animate_layer(text_layer_get_layer(update_at_a_glance), &start01, &finish01, 1000, 0);
					animate_layer(text_layer_get_layer(update_at_a_glance), &start02, &finish02, 1000, animationLength);
		        }
	}
}