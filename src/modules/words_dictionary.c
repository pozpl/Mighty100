#include "words_dictionary.h"

static uint16_t *words_index;
static size_t res_size;
static uint16_t current_index;

void init_words_index(){
    if (!words_index) {
        // Get resource handle
        ResHandle handle = resource_get_handle(RESOURCE_ID_INDEX_DATA_FILE);
        // Get size of the resource in bytes
        res_size = resource_size(handle);
        // Create a buffer the exact size of the raw resource
        words_index = (uint16_t *) malloc(res_size);
        // Copy all bytes to a buffer
        resource_load(handle, (uint8_t *)words_index, res_size);
    }  
    
    current_index = 0;
}

void deinit_words_index() {
    if (words_index) {
        free(words_index);
        words_index = NULL;
    }
}

uint16_t count_words(){
    if(words_index){
       return res_size/8; //index made from  2 bytes integers and each word and translation have 4 integer to determine offset and shift
    }else{
        return 0;
    }
}

void print_next_word_and_translation(TextLayer *s_word_text_layer, TextLayer *s_translation_text_layer){
    if(current_index > count_words() - 1){
        current_index = 0;
    }
    print_word_and_translation(current_index, s_word_text_layer, s_translation_text_layer);
    current_index++;
}

void print_word_and_translation(uint16_t index, TextLayer *s_word_text_layer, TextLayer *s_translation_text_layer){
    uint32_t real_index = index * 4;
    uint16_t begin_word = words_index[real_index];
    uint16_t end_word = (words_index[real_index + 1] + 1);
    uint16_t begin_translation = words_index[real_index + 2];
    uint16_t end_translation = (words_index[real_index + 3] + 1);
    
    APP_LOG(APP_LOG_LEVEL_INFO, "WORDS INDEXCES %d %d  %d %d",  begin_word, end_word, begin_translation, end_translation);
    
    uint8_t* word_buffer = (uint8_t *)malloc(end_word - begin_word);
    
    ResHandle words_resource_handle = resource_get_handle(RESOURCE_ID_FR_ENG_FILE);
    resource_load_byte_range(words_resource_handle, begin_word, word_buffer, end_word - begin_word);
    
    text_layer_set_text(s_word_text_layer, (char*)word_buffer);
    
    uint8_t* translation_buffer = (uint8_t *)malloc(end_translation - begin_translation);
    resource_load_byte_range(words_resource_handle, begin_translation, translation_buffer, end_translation - begin_translation);
    
    text_layer_set_text(s_translation_text_layer, (char*)translation_buffer);
    
    free(word_buffer);
    free(translation_buffer);
}
