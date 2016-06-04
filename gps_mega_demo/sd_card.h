/*
 * File: sd_card.h
 *
 * Defines the macros and constants; and declares functions for the Secure Digital
 * High Capacity (SDHC) card. This library is intended to be very lightweight
 * in that all we want to do is write data to an SD card. That's it.
*/
#ifndef _SD_CARD_H_
#define _SD_CARD_H_

#include <avr/io.h>
#include "pins.h"

#define CHIP_SELECT 	(SPI_CS_PORT &= ~(1 << SPI_CS))
#define CHIP_DESELECT	(SPI_CS_PORT |= (1 << SPI_CS))

////////////////////////////////////////////////////////////////////////////////
// SD Card Commands and Responses
// Note:
//  SDCMD == SD Card Command
//  SDARG == SD Card Command Argument
//  SDSFX == SD Card Command Suffix (7-bit CRC and the appended 0x1 end bit)
//  SDRES == SD Card Response
#define SDCMD_GO_IDLE_STATE       0x0    //CMD0; gets R1 response
#define SDARG_GO_IDLE_STATE       0x0
#define SDSFX_GO_IDLE_STATE       0x95
#define SDRES_IN_IDLE_STATE_OK    0x01

#define SDCMD_SEND_IF_COND        0x8  //CMD8; gets R7 response
#define SDARG_SEND_IF_COND        0x1AA //TODO: find out why VHS isn't 0b0001
#define SDSFX_SEND_IF_COND        0x87

#define SDCMD_APP_CMD             0x37 //CMD55; gets R1 response
#define SDARG_APP_CMD             0x0
#define SDSFX_APP_CMD             0x0 //0x1 //CRC doesn't matter, just 0b1

#define SDCMD_SD_SEND_OP_COND     0x29 //ACMD41; gets R1 response
#define SDARG_SD_SEND_OP_COND     0x40000000
#define SDSFX_SD_SEND_OP_COND     0x1 //CRC doesn't matter, just 0b1
#define SDRES_SD_SEND_OP_COND_OK  0x0

#define SDCMD_READ_OCR            0x3A //CMD58; gets R3 response
#define SDARG_READ_OCR            0x0
#define SDSFX_READ_OCR            0x1 //CRC doesn't matter, just 0b1
#define SDRES_READ_OCR            0x0

#define SDCMD_SEND_CSD            0x9 //CMD9; gets R1 response (Table 7-3)
#define SDARG_SEND_CSD            0X0
#define SDSFX_SEND_CSD            0x0

#define SDCMD_READ_SINGLE_BLOCK   0x11 //CMD17; gets R1 response
#define SDSFX_READ_SINGLE_BLOCK   0x0

#define SDCMD_WRITE_BLOCK         0x18 //CMD24; gets R1 response
#define SDSFX_WRITE_BLOCK         0x0

#define SDCMD_SEND_STATUS         0xD //CMD13
#define SDRES_DATA_ACCEPTED       0x2
#define SDRES_DATA_REJECT_CRC     0x5
#define SRES_DATA_REJECT_WRITE    0x6
////////////////////////////////////////////////////////////////////////////////
// Other Constants
#define SDCARD_BYTES_PER_BLOCK    512
#define SDCARD_CMD_MASK_HEAD      0b01000000 // used to shape card commands
#define SDCARD_CMD_MASK_TAIL      0b00111111
#define HIGH_BYTE                 0xFF // used to force MOSI high during init
#define ERROR_BYTE                0xFF // used to indicate and error
#define JUNK_BYTE                 0xFF // used to force data to be exchanged
#define SDCARD_POLL_LIMIT         0xFF // max number of times to poll
#define SDCARD_MAX_RETRIES        0xFF // max retries for send op condition
#define MS_BIT                    0x80
#define LS_BIT                    0x01
#define START_TOKEN               0xFE
#define PADDING_BYTE              0xAA // used to pad a block; just in case

////////////////////////////////////////////////////////////////////////////////
// Functions

/* Initializes the SD card to run in SPI mode. */
uint8_t sdcard_init(void);

/* Returns 1 if logging is enabled; 0 otherwise */
uint8_t sdcard_is_enabled(void);

/* Reads the block at the specified address and writes its contents to
   block_buff. Returns 1 if successful; 0 otherwise.
*/
uint8_t sdcard_read_block(uint32_t block_address, void * block_buff);

/* Writes the next chunk of data to the SD card */
void sdcard_write_data(void);

#endif /* _SD_CARD_H_ */

