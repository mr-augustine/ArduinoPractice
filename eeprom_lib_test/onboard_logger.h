#ifndef ONBOARD_LOGGER_H_
#define ONBOARD_LOGGER_H_

// EEPROM Capacities
// Arudino Uno - 1024 bytes
// Arduino Mega - 4096 bytes
#define EEPROM_CAPACITY             1024
#define CHUNK_SIZE                  512

#define FRAME_PREFIX                0xDADAFEED
#define FRAME_PREFIX_SIZE           4
#define FRAME_SUFFIX                0xCAFEBABE
#define FRAME_SUFFIX_SIZE           4
#define TOTAL_PREFIX_SUFFIX_SIZE    (FRAME_PREFIX_SIZE + FRAME_SUFFIX_SIZE)

static uint8_t onboard_logger_enabled;

void init_onboard_logger(const void * data, int data_size);
void write_next_frame();
//void write_next_frame(int frame_index);

//void write_frame(const void * data, int size, int frame_index);

#endif // ONBOARD_LOGGER_H_
