#include <pebble.h>
#include "modules/words_dictionary.h"
#include "modules/health.h"

static Window *window;
static TextLayer *word_layer;
static TextLayer *time_layer;
static TextLayer *translation_layer;
static TextLayer *s_battery_layer;
static TextLayer *s_connection_layer;
static TextLayer *date_layer;
static TextLayer *s_steps_layer;

static GFont dict_font;

static void handle_battery(BatteryChargeState charge_state) {
    static char battery_text[] = "100%      ";

    if (charge_state.is_charging) {
        snprintf(battery_text, sizeof (battery_text), "charging");
    } else {
        snprintf(battery_text, sizeof (battery_text), " %d%% ", charge_state.charge_percent);
    }
    text_layer_set_text(s_battery_layer, battery_text);
}

static void handle_bluetooth(bool connected) {
    text_layer_set_text(s_connection_layer, connected ? "" : "disconnected");

    if (!connected) {
        // Issue a vibrating alert
        vibes_double_pulse();
    }
}

static void update_time() {
    // Get a tm structure
    time_t temp = time(NULL);
    struct tm *tick_time = localtime(&temp);

    // Write the current hours and minutes into a buffer
    static char s_buffer[8];
    strftime(s_buffer, sizeof (s_buffer), clock_is_24h_style() ?
            "%H:%M" : "%I:%M", tick_time);

    // Display this time on the TextLayer
    text_layer_set_text(time_layer, s_buffer);

    //    Display date 
    static char date_buffer[16];
    strftime(date_buffer, sizeof (date_buffer), "%a %d %b", tick_time);
    text_layer_set_text(date_layer, date_buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
    update_time();

    // update word and translation every hour for a start
    //        if (tick_time->tm_min % 5 == 0) {
    //            print_next_word_and_translation(word_layer, translation_layer);
    //        }
    //    APP_LOG(APP_LOG_LEVEL_INFO, "TICK HANDLER MINUTE %d ", tick_time->tm_min);
    print_next_word_and_translation(word_layer, translation_layer);

}

void init_battery_level(GRect bounds) {
    s_battery_layer = text_layer_create(GRect(0, 0, PBL_IF_RECT_ELSE(40, bounds.size.w), PBL_IF_RECT_ELSE(22, 10)));
    text_layer_set_text_color(s_battery_layer, GColorWhite);
    text_layer_set_background_color(s_battery_layer, GColorBlack);
    text_layer_set_font(s_battery_layer, fonts_get_system_font(FONT_KEY_GOTHIC_09));
#if defined(PBL_ROUND)
    text_layer_set_text_alignment(s_battery_layer, GTextAlignmentCenter);
#endif

    text_layer_set_text(s_battery_layer, "100%      ");
}

void init_bluetooth_level(GRect bounds) {
#ifdef PBL_ROUND
    s_connection_layer = text_layer_create(GRect( 0,  10, bounds.size.w,  18));
#else
    s_connection_layer = text_layer_create(GRect(40, 0, bounds.size.w - 40, 22));
#endif
    text_layer_set_text_color(s_connection_layer, GColorWhite);
    text_layer_set_background_color(s_connection_layer, GColorBlack);
    text_layer_set_font(s_connection_layer, fonts_get_system_font(FONT_KEY_GOTHIC_09));
#if defined(PBL_ROUND)
    text_layer_set_text_alignment(s_connection_layer, GTextAlignmentCenter);
#else
    text_layer_set_text_alignment(s_connection_layer, GTextAlignmentRight);
#endif
    handle_bluetooth(bluetooth_connection_service_peek());

}

void init_word_layer(GRect bounds) {
#ifdef PBL_ROUND
    word_layer = text_layer_create(GRect(0, 45, bounds.size.w, 30));
#else
    word_layer = text_layer_create(GRect(0, 22, bounds.size.w, 30));
#endif

    text_layer_set_background_color(word_layer, GColorBlack);
    text_layer_set_text_color(word_layer, GColorWhite);
    text_layer_set_text(word_layer, "word");
    text_layer_set_font(word_layer, dict_font);
    text_layer_set_text_alignment(word_layer, GTextAlignmentCenter);
}

void init_time_layer(GRect bounds) {
    // Create the TextLayer with specific bounds
#ifdef PBL_ROUND
    time_layer = text_layer_create(GRect(0, 66, bounds.size.w, 43));
#else
    time_layer = text_layer_create(GRect(0, 52, bounds.size.w, 50));
#endif

    text_layer_set_background_color(time_layer, GColorBlack);
    text_layer_set_text_color(time_layer, GColorWhite);
    text_layer_set_text(time_layer, "00:00");
    text_layer_set_font(time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
    text_layer_set_text_alignment(time_layer, GTextAlignmentCenter);
}

void init_translations_layer(GRect bounds) {
    translation_layer = text_layer_create(
            GRect(0, PBL_IF_RECT_ELSE(102, 108), bounds.size.w, PBL_IF_RECT_ELSE(44, 50)));
    text_layer_set_background_color(translation_layer, GColorBlack);
    text_layer_set_text_color(translation_layer, GColorWhite);
    text_layer_set_text(translation_layer, "translation,translation,translation");
    text_layer_set_font(translation_layer, dict_font);
    text_layer_set_text_alignment(translation_layer, GTextAlignmentCenter);
    text_layer_set_overflow_mode(translation_layer, GTextOverflowModeWordWrap);
}

void init_steps_layer(GRect bounds) {
#ifdef PBL_ROUND
    s_steps_layer = text_layer_create(GRect(0, 28, bounds.size.w, 18));
    text_layer_set_text_alignment(s_steps_layer, GTextAlignmentCenter);
#else
    s_steps_layer = text_layer_create(GRect(bounds.size.w / 2, 146, bounds.size.w / 2, 24));
    text_layer_set_text_alignment(s_steps_layer, GTextAlignmentRight);
#endif

    text_layer_set_text_color(s_steps_layer, GColorWhite);
    text_layer_set_background_color(s_steps_layer, GColorBlack);
    text_layer_set_text(s_steps_layer, "20,000");
    text_layer_set_font(s_steps_layer, fonts_get_system_font(PBL_IF_RECT_ELSE(FONT_KEY_GOTHIC_14, FONT_KEY_GOTHIC_14)));
}

void init_date_level(GRect bounds) {
#ifdef PBL_ROUND
    date_layer = text_layer_create(GRect(0, 158, bounds.size.w, 22));
    text_layer_set_text_alignment(date_layer, GTextAlignmentCenter);
#else
    date_layer = text_layer_create(GRect(0, 146, bounds.size.w / 2, 24));
    text_layer_set_text_alignment(date_layer, GTextAlignmentLeft);
#endif

    text_layer_set_text_color(date_layer, GColorWhite);
    text_layer_set_background_color(date_layer, GColorBlack);

    text_layer_set_font(date_layer, fonts_get_system_font(PBL_IF_RECT_ELSE(FONT_KEY_GOTHIC_14, FONT_KEY_GOTHIC_14)));

}

static void window_load(Window *window) {
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);

    //window_set_background_color(window, GColorBlack);


    dict_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_OPEN_SANS_20));

    init_word_layer(bounds);
    init_time_layer(bounds);
    init_translations_layer(bounds);
    init_battery_level(bounds);
    init_bluetooth_level(bounds);
    init_date_level(bounds);
    init_steps_layer(bounds);

    layer_add_child(window_layer, text_layer_get_layer(time_layer));
    layer_add_child(window_layer, text_layer_get_layer(word_layer));
    layer_add_child(window_layer, text_layer_get_layer(translation_layer));
    layer_add_child(window_layer, text_layer_get_layer(s_battery_layer));
    layer_add_child(window_layer, text_layer_get_layer(s_connection_layer));
    layer_add_child(window_layer, text_layer_get_layer(date_layer));
    layer_add_child(window_layer, text_layer_get_layer(s_steps_layer));

    handle_battery(battery_state_service_peek());
    handle_bluetooth(connection_service_peek_pebble_app_connection());
    print_next_word_and_translation(word_layer, translation_layer);

    init_health(s_steps_layer);
}

static void window_unload(Window *window) {
    // Unload GFont
    fonts_unload_custom_font(dict_font);

    // Destroy TextLayer
    text_layer_destroy(word_layer);
    text_layer_destroy(time_layer);
    text_layer_destroy(translation_layer);
    text_layer_destroy(s_battery_layer);
    text_layer_destroy(s_connection_layer);
    text_layer_destroy(date_layer);
    text_layer_destroy(s_steps_layer);

}

static void inbox_received_handler(DictionaryIterator *iter, void *context) {
    Tuple *language_dict_t = dict_find(iter, MESSAGE_KEY_DICTIONARY);
    if (language_dict_t && language_dict_t->value->int32) { // Read boolean as an integer
        set_dictionnary((uint16_t) language_dict_t->value->int32);
        print_next_word_and_translation(word_layer, translation_layer);
    }
}

static void init(void) {
    window = window_create();

    window_set_window_handlers(window, (WindowHandlers) {
        .load = window_load,
        .unload = window_unload,
    });

    init_words_index();


    window_set_background_color(window, GColorClear);

    //    // Show the Window on the watch, with animated=true
    window_stack_push(window, true);

    // Make sure the time is displayed from the start
    update_time();

    // Register with TickTimerService
    tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

    // Register for battery level updates
    battery_state_service_subscribe(handle_battery);

    // Register for Bluetooth connection updates

    connection_service_subscribe((ConnectionHandlers) {
        .pebble_app_connection_handler = handle_bluetooth
    });

    app_message_register_inbox_received(inbox_received_handler);
    const int inbox_size = 128;
    const int outbox_size = 128;
    app_message_open(inbox_size, outbox_size);
}

static void deinit(void) {
    window_destroy(window);
    //    deinit_words_index();

    tick_timer_service_unsubscribe();
    battery_state_service_unsubscribe();
    bluetooth_connection_service_unsubscribe();
}

int main(void) {
    init();
    app_event_loop();
    deinit();
}
