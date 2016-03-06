#include <stdio.h>
#include "sd_card.h"
#include "uwrite.h"

static uint8_t SPI_exchange_byte(uint8_t byte);
static uint8_t SDCARD_get_response(void);
static uint8_t SDCARD_read_card_size(void);
static void SDCARD_send_command(uint8_t command, uint32_t argument, uint8_t suffix);

void SPI_init(void) {
  SPI_CS_DDR |= (1 << SPI_CS); // set chip select pin as an output
  CHIP_DESELECT; // start the card as not being selected

  SPI_MOSI_DDR |= (1 << SPI_MOSI);  // set MOSI as an output

  SPI_MISO_DDR |= (0 << SPI_MISO);  // ensure MISO is set as an input
  SPI_MISO_PORT |= (1 << SPI_MISO); // mild paranoia; set pull-up resistor to
                                    // avoid spurious signals being sent to
                                    // the card during startup or avoid 
                                    // having a floating pin if the card isn't
                                    // plugged in

  SPI_SCK_DDR |= (1 << SPI_SCK);    // set SCK as an output

  SPCR |= (1 << SPR1) + (1 << SPR0);// set clock rate to f_osc / 128
  SPCR |= (1 << MSTR);              // set Arduino as master device
  SPCR |= (1 << SPE);               // enable SPI operations
}

void SDCARD_init(void) {
  CHIP_DESELECT;

  // Before sending any commands, make sure the SD card is ready by sending
  // clock pulses for 10 cycles while CS is deselected
  uint8_t i;
  for(i = 0; i < 10; i++)
    SPI_exchange_byte(HIGH_BYTE);

  CHIP_SELECT;

  // Send command to go into idle state (CMD0)
  // Response should indicate idle state (1 byte)
  //----- DEBUG
  UWRITE_print_buff("Sending GO_IDLE_STATE     (CMD0) ... got ");
  //----- DEBUG

  SDCARD_send_command(SDCMD_GO_IDLE_STATE,
                      SDARG_GO_IDLE_STATE,
                      SDSFX_GO_IDLE_STATE); 
  uint8_t idle_cmd_response = SDCARD_get_response();

  //----- DEBUG
  UWRITE_print_byte(&idle_cmd_response);
  //----- DEBUG

  if (idle_cmd_response == ERROR_BYTE)
    return;
  else if (idle_cmd_response != SDRES_IN_IDLE_STATE_OK)
    return;

  // Send command to request interface condition (CMD8)
  // Response should indicate idle state (and the condition, another 4 bytes) 
  //----- DEBUG
  UWRITE_print_buff("Sending SEND_IF_COND      (CMD8) ... got ");
  //----- DEBUG

  SDCARD_send_command(SDCMD_SEND_IF_COND,
                      SDARG_SEND_IF_COND,
                      SDSFX_SEND_IF_COND);
  uint8_t cond_response = SDCARD_get_response();

  //----- DEBUG
  UWRITE_print_byte(&cond_response);
  //----- DEBUG

  if (cond_response == ERROR_BYTE)
    return;
  else if (cond_response != SDRES_IN_IDLE_STATE_OK)
    return;
  
  uint8_t op_cond_response;
  uint8_t retry_index;
  for (retry_index = 0; retry_index < SDCARD_MAX_RETRIES; retry_index++) {
    // Before we send the operation condition command (ACMD41)
    // we have to send the Application Specific Command (CMD55)
    // Response should indicate idle state
    //----- DEBUG
    UWRITE_print_buff("Sending APP_CMD          (CMD55) ... got ");
    //----- DEBUG

    SDCARD_send_command(SDCMD_APP_CMD,
                        SDARG_APP_CMD,
                        SDSFX_APP_CMD);
    uint8_t app_response = SDCARD_get_response();

    //----- DEBUG
    UWRITE_print_byte(&app_response);
    //----- DEBUG

    if (app_response == ERROR_BYTE)
      return;
    else if (app_response != SDRES_IN_IDLE_STATE_OK)
      return;
  
    //----- DEBUG
    UWRITE_print_buff("Sending SD_SEND_OP_COND (ACMD41) ... got ");
    //----- DEBUG

    SDCARD_send_command(SDCMD_SD_SEND_OP_COND,
                        SDARG_SD_SEND_OP_COND,
                        SDSFX_SD_SEND_OP_COND);
    op_cond_response = SDCARD_get_response();

    //----- DEBUG
    UWRITE_print_byte(&op_cond_response);
    //----- DEBUG

    if (op_cond_response == SDRES_IN_IDLE_STATE_OK)
      continue;
    else if (op_cond_response == SDRES_SD_SEND_OP_COND_OK)
      break;
  }
  if (op_cond_response == ERROR_BYTE)
    return;

  // Verify that the card is a high capacity card
  //----- DEBUG
  UWRITE_print_buff("Sending READ_OCR         (CMD58) ... got ");
  //----- DEBUG

  SDCARD_send_command(SDCMD_READ_OCR,
                      SDARG_READ_OCR,
                      SDSFX_READ_OCR);
  uint8_t read_ocr_response = SDCARD_get_response();

  //----- DEBUG
  UWRITE_print_byte(&read_ocr_response);
  //----- DEBUG

  if (read_ocr_response != SDRES_READ_OCR)
    return;

  // Turn on DEBUG LED
  PORTD |= (1 << 4);

  // Increase SPI clock to 2 MHz
  SPSR |= (1 << SPI2X);
  SPCR &= ~(1 << SPR1);

  // Read the card size
  SDCARD_read_card_size(); 

  // TODO: Find the next available block for writing
  // TODO: Set an external variable to indicate that the SD card is good to go
}

/* Sends the specified byte on the SPI output.
   Assumes that the target device was already selected.
   Returns the byte received in exchange.
*/
static uint8_t SPI_exchange_byte(uint8_t byte) {
  SPDR = byte;

  // Wait until byte exchange is complete. This is indicated when the SPIF
  // flag is set.
  while (!(SPSR & (1 << SPIF))) {}

  return SPDR;
}

/* Sends the specified command to the SD card */
static void SDCARD_send_command(uint8_t command, uint32_t argument, uint8_t suffix) {
  // Send up to 8 high bytes until 0xFF is received
  int i;
  uint8_t response;
  for (i = 0; i < 7; i++) {
    //----- DEBUG
    //UWRITE_print_buff("Sending 0xFF before cmd ... got ");
    //----- DEBUG

    response = SPI_exchange_byte(0xFF);

    //----- DEBUG
    //UWRITE_print_byte(&response);
    //UWRITE_print_buff("\r\n");
    //----- DEBUG

    if (response == 0xFF)
      break;
  }

  // Send the command
  SPI_exchange_byte(SDCARD_CMD_MASK_HEAD | (command & SDCARD_CMD_MASK_TAIL));

  // Then send the argument
  SPI_exchange_byte((uint8_t) (argument >> 24));
  SPI_exchange_byte((uint8_t) (argument >> 16));
  SPI_exchange_byte((uint8_t) (argument >>  8));
  SPI_exchange_byte((uint8_t) argument);

  // Finally, send the suffix
  SPI_exchange_byte(suffix);
}

/* Polls the SD card for a response.
   Returns the response on success; an ERROR_BYTE on failure.
*/
static uint8_t SDCARD_get_response(void) {
  // Poll the card for a response no more than SDCARD_POLL_LIMIT times
  uint8_t poll_index;
  for (poll_index = 0; poll_index < SDCARD_POLL_LIMIT; poll_index++) {
    //----- DEBUG
    //UWRITE_print_buff("Sending 0xFF for response ... got ");
    //----- DEBUG

    uint8_t response = SPI_exchange_byte(JUNK_BYTE);

    //----- DEBUG
    //UWRITE_print_byte(&response);
    //UWRITE_print_buff("\r\n");
    //----- DEBUG


    if (!(response & MS_BIT))
      return response; 
  }

  return ERROR_BYTE;
}

/* Reads the SD card's capacity. */
static uint8_t SDCARD_read_card_size(void) {
  //----- DEBUG
  UWRITE_print_buff("\r\nReading card size\r\n");
  UWRITE_print_buff("Sending SEND_CSD          (CMD9) ... got ");
  //----- DEBUG

  SDCARD_send_command(SDCMD_SEND_CSD,
                      SDARG_SEND_CSD,
                      SDSFX_SEND_CSD);
  uint8_t send_csd_response = SDCARD_get_response();

  //----- DEBUG
  UWRITE_print_byte(&send_csd_response);
  //----- DEBUG

  //----- DEBUG
  UWRITE_print_buff("Sending SEND_CSD (again)  (CMD9) ... got ");
  //----- DEBUG

  SDCARD_send_command(SDCMD_SEND_CSD,
                      SDARG_SEND_CSD,
                      SDSFX_SEND_CSD);
  send_csd_response = SDCARD_get_response();

  //----- DEBUG
  UWRITE_print_byte(&send_csd_response);
  //----- DEBUG

  // Expecting 0x00 reponse
  if (send_csd_response != 0x00) {
    return 0;
  }

  // Expecting start token (0xFE) + 16 bytes of data + 16-bit CRC
  uint16_t grab_index;
  uint8_t response;

  // Send JUNK_BYTE until start token is received, then read the CSD register
  // See Section 7.2.6, Figure 7-3, and Section 7.3.3.2 in SD Card Specs
  for (grab_index = 0; grab_index < 0xFF; grab_index++) {
    response = SPI_exchange_byte(JUNK_BYTE); 

    if (response == START_TOKEN) { 
      UWRITE_print_buff("CSD START_TOKEN received\r\n");
      break;
    }
  }
  if (response != START_TOKEN) {
    return 0;
  }

  uint8_t csd_register[16];

  // Read the 16-byte CSD register
  UWRITE_print_buff("Reading CSD register...\r\n");
  for (grab_index = 0; grab_index < 16; grab_index++) {
    csd_register[grab_index] = SPI_exchange_byte(JUNK_BYTE);    
  }

  // Ignore 16-bit CRC
  //SPI_exchange_byte(JUNK_BYTE);
  //SPI_exchange_byte(JUNK_BYTE);

  // Print the CSD register data
  char msg[100];
  snprintf(msg, 100,
    "%02X %02X %02X %02X %02X %02X %02X %02X ",
    csd_register[0], csd_register[1], csd_register[2], csd_register[3],
    csd_register[4], csd_register[5], csd_register[6], csd_register[7]);
  UWRITE_print_buff(msg);

  snprintf(msg, 100,
    "%02X %02X %02X %02X %02X %02X %02X %02X\r\n",
    csd_register[8], csd_register[9], csd_register[10], csd_register[11],
    csd_register[12], csd_register[13], csd_register[14], csd_register[15]);
  UWRITE_print_buff(msg);

  return 0;
}

/* Writes a byte to the SD card at the specified address */
/*void SDCARD_write_byte(uint32_t address, uint8_t byte) {

}*/
