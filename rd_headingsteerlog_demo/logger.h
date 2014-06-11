#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include "globals.h"
#include "RobotDevil.h"

#define CS (1 << 2)
#define CS_ENABLE() (PORTB &= ~CS)
#define CS_DISABLE() (PORTB |= CS)

////////////////////////////////////////////////////////////////////////////////
// Logger Constants
#define CMD0 0                // resets the card to idle state
#define CMD8 8                // send interface condition
#define CMD9 9                // indicates card-specific data will be sent
#define CMD24 24              // block write command
#define CMD55 55              // indicates next command is application-specific
#define ACMD41 41             // app specific command: request card's OCR


////////////////////////////////////////////////////////////////////////////////
// Logger Variables
volatile uint8_t logger_enabled;
uint32_t logger_next_block;
uint32_t logger_card_blocks;


////////////////////////////////////////////////////////////////////////////////
// Logger Functions
void serial_write(char *cmd)
{
#define SERIAL_OUTPUT 1
#if SERIAL_OUTPUT
  while (*cmd != 0)
  {
    while(!(UCSR0A & 0b00100000));
    UDR0 = *cmd;
    cmd++;
  }
#endif 
}

uint8_t spi_transfer(uint8_t ch)
{
  SPDR = ch;
  while(!(SPSR & (1<<SPIF))) {}       
  return SPDR;
}

uint8_t sd_command(uint8_t cmd,
                   uint32_t arg,
                   uint8_t crc)
{
  spi_transfer(0b01000000 | (cmd & 0b00111111));
  spi_transfer(arg>>24);
  spi_transfer(arg>>16);
  spi_transfer(arg>>8);
  spi_transfer(arg);
  spi_transfer(crc);

  for (uint8_t i = 0; i < 0xFF; i++)
  {
    uint8_t ch = spi_transfer(0xFF);
    if (!(ch & 0x80)) {
      return ch;
    }
  }
  return 0xFF;
}

/* Reads the SD card capacity.
 *
 * If successful, writes the card size (as number of 512 byte blocks
 * available) to the global variable logger_card_blocks and returns 1.
 *
 * If not successful, return 0.
 */
uint8_t read_card_size(void)
{
  uint16_t i;
  uint8_t ch;
  char msg[100];

  serial_write("reading size\n");
  
  // Read CSD
  ch = sd_command(CMD9, 0x00, 0x00);
  if (ch != 0)
    return 0;
  
  for (i = 0; i < 0xFF; i++)
  {
    ch = spi_transfer(0xFF);
    if (ch == 0xFE)
      break;
  }
  if (ch != 0xFE)
  {
    return 0;
  }

  uint8_t csd_register[16];
  // Read the 16 byte CSD register.
  for (i = 0; i < 16; i++)
    csd_register[i] = (uint8_t)spi_transfer(0xFF);
  
  // Ignore the 16 bit CRC
  spi_transfer(0xFF);
  spi_transfer(0xFF);

  snprintf(msg, 100,
           "%02X %02X %02X %02X %02X %02X %02X %02X\n",
           csd_register[0], csd_register[1], csd_register[2], csd_register[3],
           csd_register[4], csd_register[5], csd_register[6], csd_register[7]);
  serial_write(msg);
  snprintf(msg, 100,
           "%02X %02X %02X %02X %02X %02X %02X %02X\n",
           csd_register[8], csd_register[9], csd_register[10], csd_register[11],
           csd_register[12], csd_register[13], csd_register[14], csd_register[15]);
  serial_write(msg);
  
  uint32_t c_size;
  c_size = csd_register[7];
  c_size = c_size << 8;
  c_size |= csd_register[8];
  c_size = c_size << 8;
  c_size |= csd_register[9];
  
  // Card size is (c_size+1) * 512k [bytes] = c_size*1024 [512 byte blocks]
  logger_card_blocks = (c_size+1) * 1024;

  snprintf(msg, 100,
           "card size: %lu blocks\n", logger_card_blocks);
  serial_write(msg);

  if (logger_card_blocks == 0)
    return 0;
  
  return 1;
}

/* Determines if a block on the SD card has been (fully or partially)
 * filled in by looking at the first 32 bits.
 *
 * Returns 1 if the block starts with GLOBAL_START
 *
 * Returns 0 if the block does not start with GLOBAL_START
 *
 * Returns -1 if there was an error reading the block.
 * 
 */
int8_t check_block(uint32_t block_address)
{  
  uint16_t i;
  uint8_t ch;
  union {
    uint8_t bytes[4];
    uint32_t dword;
  } data;

  char msg[100];
  snprintf(msg, 100, "reading block %lu\n", block_address);
  serial_write(msg);
  if (sd_command(17, block_address, 0) != 0)
    return -1;
  
  for (i = 0; i < 0xFF; i++)
  {
    ch = spi_transfer(0xFF);
      if (ch == 0xFE)
        break;
  }
  if (ch != 0xFE)
    return -1;
  
  // Read the first 32 bits of data
  data.bytes[0] = spi_transfer(0xFF);
  data.bytes[1] = spi_transfer(0xFF);
  data.bytes[2] = spi_transfer(0xFF);
  data.bytes[3] = spi_transfer(0xFF);
  
  // Ignore the remaining data from the block
  for (i = 0; i < 508; i++)
    spi_transfer(0xFF);
  
  // Ignore the 16 bit CRC
  spi_transfer(0xFF);
  spi_transfer(0xFF);
  
  if (data.dword == GLOBAL_START)
    return 1;
  else
    return 0;
}

uint8_t find_available_block(void)
{
  uint32_t min_address;
  uint32_t max_address;;
  uint8_t result;
  char msg[100];

  /* Explicitly check the first block.  It is likely to be free since
   * we clear the card every time we read it, and the binary search is
   * simplified by not including the first address since we can always
   * assume there is at least one preceding address.
   */
  
  result = check_block(0);
  if (result == -1)
    return 0;
  if (result == 0)
  {
    logger_next_block = 0;
    return 1;
  }

  min_address = 1;
  max_address = logger_card_blocks-1;
  while (max_address >= min_address)
  {
    uint32_t midpoint = min_address + (max_address - min_address) / 2;

    snprintf(msg, sizeof(msg), "[%lu %lu]: %lu\n", min_address, max_address, midpoint);
    serial_write(msg);
    
    result = check_block(midpoint);
    if (result == -1)
      return -1;
    else if (result == 0)
    {
      /* This block is free.  If the preceding block is used, this is
       * the block we're looking for.
       */
      result = check_block(midpoint-1);
      if (result == -1)
        return -1;
      if (result == 1)
      {
        /* Preceding block is used, so this is the correct block. */
        logger_next_block = midpoint;
        return 1;
      }
      /* The preceding block is also free, so move the search to the
       * first half of the search space.
       */
      max_address = midpoint-1;      
    }
    else
    {
      /* This block is used, move search to the second half of the
       * space.
       */
      min_address = midpoint+1;
    }
  }

  /* Searched entire space and didn't find a free block.  Either the
   * card is full, or the blocks are not contiguous.
   */
  return 0;
}

void init_logger(void)
{
  logger_enabled = 0;
  logger_next_block = 0;
  logger_card_blocks = 0;

  uint8_t i;
  uint8_t response;
 
  // Enable SPI, master, set clock rate fck/128
  SPCR = (1<<SPE) | (1<<MSTR) | (1<<SPR0) | (1<<SPR1);
  SPSR = (0<<SPI2X);

  CS_DISABLE();
  for (i = 0; i < 10; i++)
    spi_transfer(0xFF);
  CS_ENABLE();
  
  response = sd_command(CMD0, 0, 0x95);
  if (response == 0xFF)
    return;
  else if (response != 0x01)
    return;

  response = sd_command(CMD8, 0x1AA, 0x87);
  if (response == 0xFF)
    return;
  else if (response != 0x01)
    return;

  for (i = 0; i < 0xFF; i++)
  {
    sd_command(CMD55, 0, 0);
    response = sd_command(ACMD41, 0x40000000, 0);
    if (response == 0)
      break;
  }
  if (i == 0xFF)
    return;
    
  // Increase SPI speed to 2Mhz
  SPCR = (1<<SPE) | (1<<MSTR);

  if (!read_card_size())
    return;

  if (!find_available_block())
    return;

  char msg[100];
  snprintf(msg, sizeof(msg),
           "Logging enabled starting at block %lu\n", logger_next_block);
  serial_write(msg);
  logger_enabled = 1;
}

void start_log(void)
{
  /* If logging is disabled (error initializing card, card full), just
   * return.
   */
  if (!logger_enabled)
    return;

  /* Filled the SD card, disable logging. */
  if (logger_next_block >= logger_card_blocks)
  {
    logger_enabled = 0;
    return;
  }

  /* Send command to start a block write. */
  uint8_t response;
  response = sd_command(CMD24, logger_next_block, 0);
  if (response == 0xFF)
  {
    // Transmit failed, disable SD card.
    logger_enabled = 0;
    return;
  }
  
  logger_next_block += 1;

  spi_transfer(0xFE);

  uint16_t i = 0;
  for(i = 0; i < sizeof(globals_t); i++)
    spi_transfer(((uint8_t*)&globals)[i]);

  /* Fill remaining block with 0xBB.  This should never occur because
   * globals_t is padded to be 512 bytes.
   */
  for(; i < 512; i++)
    spi_transfer(0xBB);

  /* Send ignored 16bit CRC checksum. */
  spi_transfer(0x00);
  spi_transfer(0x00);

  if (spi_transfer(0xFF) != 0xE5)
  {
    // Write failed, disable SD card.
    logger_enabled = 0;
    return;
  }
}

void finish_log(void)
{
  /* Wait for the SD card to finish writing. I'm assuming the SD card
   * continued writing without receiving the SPI clock from us.  If
   * this is true, waiting here instead of in start_log() saves time
   * by allowing us to keep working while the card writes.  If not, we
   * should still be fine by get no advantage by waiting here instead
   * of in start_log().  I suspect the card is fast enough that most
   * of our logging time is spent transmitting the data and not
   * waiting on the card, so this probably doesn't matter much.
   */
  while (spi_transfer(0xFF) != 0xFF);
}
