/* 
 * File:   health.h
 * Author: pozpl
 *
 * Created on June 26, 2016, 7:55 PM
 */

#pragma once
#include <pebble.h> 
#include "storage_keys_constants.h"

void init_health(TextLayer *step_layer);
void deinit_health();