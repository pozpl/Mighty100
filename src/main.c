#include <pebble.h>

static Window *window;
static TextLayer *word_layer;
static TextLayer *time_layer;
static TextLayer *translation_layer;

static GFont dict_font;


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

    
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
    update_time();
}

void init_word_layer(GRect bounds) {
    word_layer = text_layer_create(GRect(0, 22, bounds.size.w, 30));
    text_layer_set_background_color(word_layer, GColorBlack);
    text_layer_set_text_color(word_layer, GColorWhite);
    text_layer_set_text(word_layer, "word");
    text_layer_set_font(word_layer, dict_font);
    text_layer_set_text_alignment(word_layer, GTextAlignmentCenter);
}

void init_time_layer(GRect bounds) {
    // Create the TextLayer with specific bounds
    time_layer = text_layer_create(GRect(0, PBL_IF_ROUND_ELSE(58, 52), bounds.size.w, 50));
    text_layer_set_background_color(time_layer, GColorBlack);
    text_layer_set_text_color(time_layer, GColorWhite);
    text_layer_set_text(time_layer, "00:00");
    text_layer_set_font(time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
    text_layer_set_text_alignment(time_layer, GTextAlignmentCenter);
}

void init_translations_layer(GRect bounds){
    translation_layer = text_layer_create(
            GRect(0, PBL_IF_ROUND_ELSE(108, 102), bounds.size.w, 30));
    text_layer_set_background_color(translation_layer, GColorBlack);
    text_layer_set_text_color(translation_layer, GColorWhite);
    text_layer_set_text(translation_layer, "translation");
    text_layer_set_font(translation_layer, dict_font);
    text_layer_set_text_alignment(translation_layer, GTextAlignmentCenter);
}

static void window_load(Window *window) {
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);
    
    dict_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_OPEN_SANS_20));

    init_word_layer(bounds);
    init_time_layer(bounds);
    init_translations_layer(bounds);
    
    layer_add_child(window_layer, text_layer_get_layer(word_layer));
    layer_add_child(window_layer, text_layer_get_layer(time_layer));
    layer_add_child(window_layer, text_layer_get_layer(translation_layer));
}

static void window_unload(Window *window) {
    // Unload GFont
    fonts_unload_custom_font(dict_font);
    
    // Destroy TextLayer
    text_layer_destroy(word_layer);
    text_layer_destroy(time_layer);
    text_layer_destroy(translation_layer);
    
}

static void init(void) {
    window = window_create();
    
    window_set_window_handlers(window, (WindowHandlers) {
        .load = window_load,
        .unload = window_unload,
    });
    
    window_set_background_color(window, GColorClear);
//
//    // Show the Window on the watch, with animated=true
    window_stack_push(window, true);
//
//    // Make sure the time is displayed from the start
//    update_time();
//
//    // Register with TickTimerService
//    tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit(void) {
    window_destroy(window);
}

int main(void) {
    init();
    app_event_loop();
    deinit();
}
