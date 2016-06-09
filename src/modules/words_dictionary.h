/* 
 * File:   words_dictionary.h
 * Author: pozpl
 *
 * Created on June 7, 2016, 9:51 PM
 */

#pragma once
#include <pebble.h> 

typedef struct {
    uint8_t *word;
    uint8_t *translation;
} WordWithTranslation;

void init_words_index();
void deinit_words_index();
uint16_t count_words();

void print_word_and_translation(uint16_t index, TextLayer *s_word_text_layer, 
        TextLayer *s_translation_text_layer);

void print_next_word_and_translation(TextLayer *s_word_text_layer, TextLayer *s_translation_text_layer);