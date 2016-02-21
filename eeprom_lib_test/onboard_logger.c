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
static void set_capacity();
static void set_chunk_index();
static void set_data_source(const void * data_ptr);
static void set_num_chunks();
static void set_num_frames(int frame_size);

// TODO consider changing the static functions so that they return
// 1 on success and 0 otherwise; use an if-conditional chain to
// evaluate the success of all static function calls; set the
// onboard_logging_enabled variable accordingly
// if (func1 && func2 ... && funcN) {onboard_logging enabled = 1;}
void init_onboard_logger(const void * data, int size) {
    set_capacity();
    set_num_chunks();
    set_num_frames(size);
    set_data_source(data);
    set_chunk_index();

    frame_index = 0;
    onboard_logger_enabled = 1;

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
static void set_capacity() {
    eeprom_capacity = 0;

    // Supporting only Uno and Mega
    if (EEPROM_CAPACITY == 1024 ||
        EEPROM_CAPACITY == 4096) {
        eeprom_capacity = EEPROM_CAPACITY;
    } 

    return;
}

// TODO consider using rand() to choose a chunk index; decide what seed
// value to use
static void set_chunk_index() {
    chunk_index = 0;

    if (num_chunks > 1) {
        //choose a random chunk
    }

    return;
}

static void set_data_source(const void * data_ptr) {
    if (data_ptr != NULL) {
        data_source = data_ptr;
    }

    return;
}

/* Calculates the whole number of chunks that can be written to */
static void set_num_chunks() {
    num_chunks = 0;

    if (CHUNK_SIZE > TOTAL_PREFIX_SUFFIX_SIZE) {
        num_chunks = eeprom_capacity / CHUNK_SIZE;
    }

    return;
}

static void set_num_frames(int frame_size) {
    num_frames = 0; 

    if (frame_size > 0) {
        num_frames = CHUNK_SIZE / frame_size;
    }

    return;
}


