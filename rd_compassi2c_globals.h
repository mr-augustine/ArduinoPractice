////////////////////////////////////////////////////////////////////////////////
// Compass Constants
#define COMPASS_ADDR                0x60
#define COMPASS_READING_REG         2
#define COMPASS_OFFSET_RAW          0       // if the compass is not oriented
                                            // north-up, use this to reorient
                                            // the readings
#define DEG_TO_RAD                 0.0174532925199  // 180/pi; multiply degree
                                                    // value by this to get rad

// The values below would normally appear in the twi.h header file.
// Just for the sake of this demo, they're being defined here.
// They were pulled from the following site:
// http://www.nongnu.org/avr-libc/user-manual/group__util__twi.html
// The TW_STATUS_MASK was removed and the TW_STATUS constant was redefined.

////////////////////////////////////////////////////////////////////////////////
// Two-Wire Interface Constants
#define TW_START                   0x08
#define TW_REP_START               0x10
#define TW_MT_SLA_ACK              0x18
#define TW_MT_SLA_NACK             0x20
#define TW_MT_DATA_ACK             0x28
#define TW_MT_DATA_NACK            0x30
#define TW_MT_ARB_LOST             0x38
#define TW_MR_ARB_LOST             0x38
#define TW_MR_SLA_ACK              0x40
#define TW_MR_SLA_NACK             0x48
#define TW_MR_DATA_ACK             0x50
#define TW_MR_DATA_NACK            0x58
#define TW_ST_SLA_ACK              0xA8
#define TW_ST_ARB_LOST_SLA_ACK     0xB0
#define TW_ST_DATA_ACK             0xB8
#define TW_ST_DATA_NACK            0xC0
#define TW_ST_LAST_DATA            0xC8
#define TW_SR_SLA_ACK              0x60
#define TW_SR_ARB_LOST_SLA_ACK     0x68
#define TW_SR_GCALL_ACK            0x70
#define TW_SR_ARB_LOST_GCALL_ACK   0x78
#define TW_SR_DATA_ACK             0x80
#define TW_SR_DATA_NACK            0x88
#define TW_SR_GCALL_DATA_ACK       0x90
#define TW_SR_GCALL_DATA_NACK      0x98
#define TW_SR_STOP                 0xA0
#define TW_NO_INFO                 0xF8
#define TW_BUS_ERROR               0x00
#define TW_STATUS                  (TWSR & TW_NO_INFO)
#define TW_READ                    1
#define TW_WRITE                   0
//#define TW_STATUS_MASK             0b11111000
//#define TW_STATUS                  (TWSR & TW_STATUS_MASK)
