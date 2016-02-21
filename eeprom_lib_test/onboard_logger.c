#include <avr/eeprom.h>
#include <stdint.h>
#include <stdlib.h>
#include "onboard_logger.h"

int chunk_index;
int data_size;
const void * data_source;
int eeprom_capacity;
int frame_index;
int num_chunks;
int num_frames;

//uint8_t onboard_logger_enabled;
static uint8_t set_capacity();
static uint8_t set_chunk_index();
static uint8_t set_data_source(const void * data_ptr);
static uint8_t set_num_chunks();
static uint8_t set_num_frames(int frame_size);

void init_onboard_logger(const void * data, int size) {
    onboard_logger_enabled = 0;

    if (set_capacity() &&
        set_num_chunks() &&
        set_num_frames(size) &&
        set_data_source(data) &&
        set_chunk_index()) {

        onboard_logger_enabled = 1;
    }

    frame_index = 0;

    return;
}

/*void write_next_frame(int frame_index) {
    return;
}

void write_frame(const void * data, int data_size, int frame_index) {
    return;
}

uint8_t onboard_logger_enabled() {
    return 0;
}*/

/* Validates and sets the total EEPROM capacity */
static uint8_t set_capacity() {
    uint8_t success = 0;
    eeprom_capacity = 0;

    // Supporting only Uno and Mega
    if (EEPROM_CAPACITY == 1024 ||
        EEPROM_CAPACITY == 4096) {
        eeprom_capacity = EEPROM_CAPACITY;

        success = 1;
    } 

    return success;
}

// TODO consider using rand() to choose a chunk index; decide what seed
// value to use
static uint8_t set_chunk_index() {
    uint8_t success = 1;
    chunk_index = 0;

    if (num_chunks > 1) {
        //choose a random chunk
    } else if (num_chunks <= 0) {
        success = 0;
    }

    return success;
}

static uint8_t set_data_source(const void * data_ptr) {
    uint8_t success = 0;

    if (data_ptr != NULL) {
        data_source = data_ptr;

        success = 1;
    }

    return success;
}

/* Calculates the whole number of chunks that can be written to */
static uint8_t set_num_chunks() {
    uint8_t success = 0;
    num_chunks = 0;

    if (CHUNK_SIZE > TOTAL_PREFIX_SUFFIX_SIZE) {
        num_chunks = eeprom_capacity / CHUNK_SIZE;

        success = 1;
    }

    return success;
}

static uint8_t set_num_frames(int frame_size) {
    uint8_t success = 0;
    num_frames = 0; 

    if (frame_size > 0) {
        num_frames = CHUNK_SIZE / frame_size;

        success = 1;
    }

    return success;
}


