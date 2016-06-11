#include "words_dictionary.h"


//static uint16_t *words_index;
static size_t res_size;
static uint16_t current_index;

uint16_t get_last_stored_index();

void init_words_index() {
    //    if (!words_index) {
    // Get resource handle
    ResHandle handle = resource_get_handle(RESOURCE_ID_INDEX_DATA_FILE);
    // Get size of the resource in bytes
    res_size = resource_size(handle);
    // Create a buffer the exact size of the raw resource
    //        words_index = (uint16_t *) malloc(res_size);
    // Copy all bytes to a buffer
    //        resource_load(handle, (uint8_t *) words_index, res_size);
    //    }

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

void print_next_word_and_translation(TextLayer *s_word_text_layer, TextLayer *s_translation_text_layer) {

    if (current_index > count_words() - 1) {
        current_index = 0;
    }

    store_last_current_index(current_index);
    print_word_and_translation(current_index, s_word_text_layer, s_translation_text_layer);
    current_index++;
}

void print_word_and_translation(uint16_t index, TextLayer *s_word_text_layer, TextLayer *s_translation_text_layer) {

    // Get resource handle
    ResHandle index_handle = resource_get_handle(RESOURCE_ID_INDEX_DATA_FILE);
    // Get size of the resource in bytes
    res_size = resource_size(index_handle);
    if ( index * 8 < res_size  ) {
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

        ResHandle words_resource_handle = resource_get_handle(RESOURCE_ID_FR_ENG_FILE);
        resource_load_byte_range(words_resource_handle, begin_word, word_buffer, end_word - begin_word);

        word_buffer[end_word - begin_word] = 0;
        text_layer_set_text(s_word_text_layer, (char*) word_buffer);

        uint8_t* translation_buffer = (uint8_t *) malloc(end_translation - begin_translation + 1);
        resource_load_byte_range(words_resource_handle, begin_translation, translation_buffer, end_translation - begin_translation);
        translation_buffer[end_translation - begin_translation] = 0;

        text_layer_set_text(s_translation_text_layer, (char*) translation_buffer);

        //    APP_LOG(APP_LOG_LEVEL_INFO, "WORD %s TRANSLATION %s ", word_buffer, translation_buffer);


        free(word_buffer);
        free(translation_buffer);
    }
}
