/* 
 * File:   health.c
 * Author: pozpl
 *
 * Created on June 26, 2016, 7:57 PM
 */

#include "health.h"
static TextLayer *s_step_layer;
static int s_step_count = 0, s_step_goal = 0, s_step_average = 0;
static char s_current_steps_buffer[16];

// Is step data available?
bool step_data_is_available() {
  return HealthServiceAccessibilityMaskAvailable &
    health_service_metric_accessible(HealthMetricStepCount,
      time_start_of_today(), time(NULL));
}

// Daily step goal
int get_step_goal() {
  const time_t start = time_start_of_today();
  const time_t end = start + SECONDS_PER_DAY;
  int step_goal = (int)health_service_sum_averaged(HealthMetricStepCount,
    start, end, HealthServiceTimeScopeDaily);
  
  return step_goal;
}

// Todays current step count
int get_step_count() {
  int step_count = (int)health_service_sum_today(HealthMetricStepCount);
  return step_count;
}

// Average daily step count for this time of day
int get_step_average() {
  const time_t start = time_start_of_today();
  const time_t end = time(NULL);
  int step_average = (int)health_service_sum_averaged(HealthMetricStepCount,
    start, end, HealthServiceTimeScopeDaily);
  
  return step_average;
}

int health_handler(HealthEventType event, void *context) {
  if(event == HealthEventSignificantUpdate) {
    s_step_goal = get_step_goal();
  }

  if(event != HealthEventSleepUpdate) {
    s_step_count = get_step_count();
    s_step_average = get_step_average();
    display_step_count();
  }
}

void display_step_count() {
  int thousands = s_step_count / 1000;
  int hundreds = s_step_count % 1000;
  static char s_emoji[5];

  if(s_step_count >= s_step_average) {
    snprintf(s_emoji, sizeof(s_emoji), "\U0001F60C");
  } else {
    snprintf(s_emoji, sizeof(s_emoji), "\U0001F4A9");
  }

  if(thousands > 0) {
    snprintf(s_current_steps_buffer, sizeof(s_current_steps_buffer),
      "%d,%03d %s", thousands, hundreds, s_emoji);
  } else {
    snprintf(s_current_steps_buffer, sizeof(s_current_steps_buffer),
      "%d %s", hundreds, s_emoji);
  }

  text_layer_set_text(s_step_layer, s_current_steps_buffer);
}

void init_health(static TextLayer *step_layer){
    if(step_data_is_available()){
        s_step_layer = step_layer;
        
        health_service_events_subscribe(health_handler, NULL);
        display_step_count();
    }
}

void deinit_health(){
    
}