#include "words_dictionary.h"


static size_t res_size;
static uint16_t current_index;
static uint16_t dictionnary_index;

uint16_t get_last_stored_index();
void store_last_current_index(uint16_t current_index);

void set_dictionnary(uint16_t new_dictinnary_index) {
    APP_LOG(APP_LOG_LEVEL_INFO, "SET DICT INDEX %d ", dictionnary_index);
    if (dictionnary_index != new_dictinnary_index) {
        current_index = 0;
        dictionnary_index = new_dictinnary_index;

        store_last_current_index(current_index);
        persist_write_int(DICTIONARY_INDEX_KEY, (int) dictionnary_index);

        init_words_index();
    }

}

ResHandle get_index_handler_for_current_dict(uint16_t dictionnary_index) {
    ResHandle handle;
    if (dictionnary_index == DICTIONARY_FRENCH_ENG_100_DICT_ID) {
        handle = resource_get_handle(RESOURCE_ID_INDEX_DATA_FILE);
    } else  if(dictionnary_index == DICTIONARY_SPANISH_ENG_100_DICT_ID){
        handle = resource_get_handle(RESOURCE_ID_INDEX_SP_ENG_100);
    }else  if(dictionnary_index == DICTIONARY_PORTUGUESE_ENG_100_DICT_ID){
        handle = resource_get_handle(RESOURCE_ID_INDEX_PORT_ENG_100);   
    }else{
        handle = resource_get_handle(RESOURCE_ID_INDEX_DATA_FILE);
    }

    return handle;
}

ResHandle get_dictionary_handler_for_current_dict(uint16_t dictionnary_index) {
    ResHandle handle;
    if (dictionnary_index == DICTIONARY_FRENCH_ENG_100_DICT_ID) {
        handle = resource_get_handle(RESOURCE_ID_FR_ENG_FILE);
    } else  if(dictionnary_index == DICTIONARY_SPANISH_ENG_100_DICT_ID){
        handle = resource_get_handle(RESOURCE_ID_SP_ENG_100_FILE);
    }else  if(dictionnary_index == DICTIONARY_PORTUGUESE_ENG_100_DICT_ID){
        handle = resource_get_handle(RESOURCE_ID_PORT_ENG_100_FILE);
    } else{
        handle = resource_get_handle(RESOURCE_ID_FR_ENG_FILE);
    }

    return handle;
}

void init_words_index() {

    if (persist_exists(DICTIONARY_INDEX_KEY)) {
        // Read persisted value
        dictionnary_index = (uint16_t) persist_read_int(DICTIONARY_INDEX_KEY);
        APP_LOG(APP_LOG_LEVEL_INFO, "READ DICTIONARY INDEX  %d FROM PERSIST", (int)persist_read_int(DICTIONARY_INDEX_KEY));
    } else {
        dictionnary_index = DICTIONARY_FRENCH_ENG_100_DICT_ID;
    }
    APP_LOG(APP_LOG_LEVEL_INFO, "DICTIONARY INDEX IN INIT %d ", dictionnary_index);

    ResHandle handle = get_index_handler_for_current_dict(dictionnary_index); //resource_get_handle(RESOURCE_ID_INDEX_DATA_FILE);
    // Get size of the resource in bytes
    res_size = resource_size(handle);


    APP_LOG(APP_LOG_LEVEL_INFO, "WORDS INDEX SIZE IN BYTES %d ", res_size);
    current_index = get_last_stored_index();
    APP_LOG(APP_LOG_LEVEL_INFO, "STORED LAST CURRENT INDEX %d ", current_index);

}


//void deinit_words_index() {
////    if (words_index) {
////        free(words_index);
////        words_index = NULL;
////    }
//}

uint16_t count_words() {
    if (res_size) {
        return res_size / 8; //index made from  2 bytes integers and each word and translation have 4 integer to determine offset and shift
    } else {
        return 0;
    }
}

uint16_t get_last_stored_index() {
    uint16_t stored_index;

    if (persist_exists(CURRENT_WORD_INDEX_KEY)) {
        // Read persisted value
        stored_index = (uint16_t) persist_read_int(CURRENT_WORD_INDEX_KEY);
    } else {
        stored_index = 0;
    }

    return stored_index;
}

void store_last_current_index(uint16_t current_index) {
    persist_write_int(CURRENT_WORD_INDEX_KEY, (int) current_index);
}

void print_next_word_and_translation(TextLayer *word_text_layer, TextLayer *translation_text_layer) {

    if (current_index > count_words() - 1) {
        current_index = 0;
    }

    store_last_current_index(current_index);
    print_word_and_translation(current_index, word_text_layer, translation_text_layer);
    current_index++;
}

void print_word_and_translation(uint16_t index, TextLayer *word_text_layer, TextLayer *translation_text_layer) {

    // Get resource handle
    ResHandle index_handle = get_index_handler_for_current_dict(dictionnary_index); //resource_get_handle(RESOURCE_ID_INDEX_DATA_FILE);
    // Get size of the resource in bytes
    res_size = resource_size(index_handle);
    if (index * 8 < res_size) {
        // Create a buffer the exact size of the raw resource
        uint16_t *word_index_buffer = (uint16_t *) malloc(4 * sizeof (uint16_t));
        // Copy all bytes to a buffer
        resource_load_byte_range(index_handle, index * 8, (uint8_t *) word_index_buffer, 8);

        uint16_t begin_word = word_index_buffer[0];
        uint16_t end_word = (word_index_buffer[1] + 1);
        uint16_t begin_translation = word_index_buffer[2];
        uint16_t end_translation = (word_index_buffer[3] + 1);

        free(word_index_buffer);
        //    APP_LOG(APP_LOG_LEVEL_INFO, "WORDS INDEXCES %d %d  %d %d", begin_word, end_word, begin_translation, end_translation);

        uint8_t* word_buffer = (uint8_t *) malloc(end_word - begin_word + 1);

        ResHandle words_resource_handle = get_dictionary_handler_for_current_dict(dictionnary_index); //resource_get_handle(RESOURCE_ID_FR_ENG_FILE);
        resource_load_byte_range(words_resource_handle, begin_word, word_buffer, end_word - begin_word);

        word_buffer[end_word - begin_word] = 0;
        text_layer_set_text(word_text_layer, (char*) word_buffer);

        uint8_t* translation_buffer = (uint8_t *) malloc(end_translation - begin_translation + 1);
        resource_load_byte_range(words_resource_handle, begin_translation, translation_buffer, end_translation - begin_translation);
        translation_buffer[end_translation - begin_translation] = 0;

        text_layer_set_text(translation_text_layer, (char*) translation_buffer);

        //    APP_LOG(APP_LOG_LEVEL_INFO, "WORD %s TRANSLATION %s ", word_buffer, translation_buffer);


        free(word_buffer);
        free(translation_buffer);
    }
}
