#include <avr/eeprom.h>
#include <stdint.h>
#include <stdlib.h>
#include "onboard_logger.h"

uint16_t chunk_index;
const uint8_t * data_ptr;
uint16_t frame_data_size;
uint16_t eeprom_capacity;
uint16_t frame_index;
uint16_t num_chunks;
uint16_t num_frames;

//uint8_t onboard_logger_enabled;
static void advance_chunk_index();
static void advance_frame_index();
static uint8_t current_chunk_is_full();
static uint16_t get_frame_start_address();
static void reset_frame_index();
static uint8_t set_capacity();
static uint8_t set_chunk_index();
static uint8_t set_data_ptr(const void * data_pointer);
static uint8_t set_frame_data_size(int data_size);
static uint8_t set_num_chunks();
static uint8_t set_num_frames();
static void write_data(const uint16_t dest_addr, const uint8_t * data);
static void write_frame_prefix(uint16_t dest);
static void write_frame_suffix(uint16_t dest);

// Initializes supporting variables given the specified data dimensions
void init_onboard_logger(const void * data, int data_size) {
    onboard_logger_enabled = 0;

    if (set_capacity() &&
        set_num_chunks() &&
        set_frame_data_size(data_size) &&
        set_num_frames() &&
        set_data_ptr(data) &&
        set_chunk_index()) {

        onboard_logger_enabled = 1;
    }

    reset_frame_index();

    return;
}

// Writes the next frame to the EEPROM
void write_next_frame() {
    // Don't write to the EEPROM if logging isn't enabled
    if (onboard_logger_enabled != 1) {
        return;
    }

    if (current_chunk_is_full() == 1) {
        // TODO implement the chunk padding feature
        // pad_remaing_chunk_bytes();
        // TODO implement the chunk offload feature
        //mark_current_chunk_for_offloading();

        reset_frame_index();
        advance_chunk_index();
    }

    // The byte index within the EEPROM where writing should start
    uint16_t frame_start_address;

    frame_start_address = get_frame_start_address();
    write_frame_prefix(frame_start_address);

    uint16_t data_start_address = frame_start_address + FRAME_PREFIX_SIZE;
    write_data(data_start_address, data_ptr);

    uint16_t frame_suffix_address = data_start_address + frame_data_size;
    write_frame_suffix(frame_suffix_address);

    advance_frame_index();

    return;
}

static void advance_chunk_index() {
    chunk_index = (chunk_index + 1) % num_chunks;

    return;
}

static void advance_frame_index() {
    frame_index = frame_index + 1;

    return;
}

static uint8_t current_chunk_is_full() {
    return (uint8_t) (frame_index == num_frames);
    //return (uint8_t) ( ((frame_index + 1) % num_frames == 0) );
}

// Calculates the starting address for where the next frame should be written
static uint16_t get_frame_start_address() {
    uint16_t chunk_address = (CHUNK_SIZE * chunk_index) % EEPROM_CAPACITY;
    uint16_t chunk_offset = (frame_data_size * frame_index) % CHUNK_SIZE;
    uint16_t start_address = chunk_address + chunk_offset;

    return start_address;
}

//static void mark_current_chunk_for_offloading() {
//    return;
//}

static void reset_frame_index() {
    frame_index = 0;

    return;
}

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

static uint8_t set_data_ptr(const void * data_pointer) {
    uint8_t success = 0;

    if (data_pointer != NULL) {
        data_ptr = (uint8_t *) data_pointer;

        success = 1;
    }

    return success;
}

static uint8_t set_frame_data_size(int data_size) {
    uint8_t success = 0;
    frame_data_size = 0;

    if (data_size > 0) {
        frame_data_size = data_size;

        success = 1;
    }

    return success;
}

// Calculates the whole number of chunks that can be written to
static uint8_t set_num_chunks() {
    uint8_t success = 0;
    num_chunks = 0;

    if (CHUNK_SIZE > TOTAL_PREFIX_SUFFIX_SIZE) {
        num_chunks = eeprom_capacity / CHUNK_SIZE;

        success = 1;
    }

    return success;
}

static uint8_t set_num_frames() {
    uint16_t frame_size = TOTAL_PREFIX_SUFFIX_SIZE;
    uint8_t success = 0;
    num_frames = 0; 

    if (frame_data_size > 0) {
        frame_size = frame_size + frame_data_size;
        num_frames = CHUNK_SIZE / frame_size;

        success = 1;
    }

    return success;
}

// Writes the data located at value_addr into the specified destination
static void write_data(uint16_t dest, const uint8_t * data) {
    uint16_t byte_index;

    for (byte_index = 0; byte_index < frame_data_size; byte_index++) {
        eeprom_write_byte( (uint8_t *) (dest + byte_index), 
                           data[byte_index] );
    }

    return;
}

// Writes the frame prefix to the specified destination
static void write_frame_prefix(uint16_t dest) {
    uint16_t byte_index;

    for (byte_index = 0; byte_index < FRAME_PREFIX_SIZE; byte_index++) {
        // Write the frame prefix from MSByte to LSByte
        eeprom_write_byte( (uint8_t *) (dest + byte_index), 
            (uint8_t)(FRAME_PREFIX >> ((FRAME_PREFIX_SIZE - (byte_index + 1)) * 8)) );
    }

    return;
}

static void write_frame_suffix(uint16_t dest) {
    uint16_t byte_index;

    for (byte_index = 0; byte_index < FRAME_SUFFIX_SIZE; byte_index++) {
        // Write the frame suffix from MSByte to LSByte
        eeprom_write_byte( (uint8_t *) (dest + byte_index), 
            (uint8_t)(FRAME_SUFFIX >> ((FRAME_SUFFIX_SIZE - (byte_index + 1)) * 8)) );
    }

    return;
}

