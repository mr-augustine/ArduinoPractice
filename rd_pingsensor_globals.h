////////////////////////////////////////////////////////////////////////////////
// Defines
#define LOOP_FREQUENCY_HZ                  40.0

// ping_status bit-field definitions
#define PSTATUS_FLEFT_PING_VALID           (1L << 0)
#define PSTATUS_FCENTER_PING_VALID         (1L << 1)
#define PSTATUS_FRIGHT_PING_VALID          (1L << 2)
#define PSTATUS_RCENTER_PING_VALID         (1L << 3)
#define PSTATUS_DNGRCLOSE_FLEFT            (1L << 4)
#define PSTATUS_DNGRCLOSE_FCENTER          (1L << 5)
#define PSTATUS_DNGRCLOSE_FRIGHT           (1L << 6)
#define PSTATUS_DNGRCLOSE_RCENTER          (1L << 7)
#define PSTATUS_NEARFIELD_FLEFT            (1L << 8)
#define PSTATUS_NEARFIELD_FCENTER          (1L << 9)
#define PSTATUS_NEARFIELD_FRIGHT           (1L << 10)
#define PSTATUS_NEARFIELD_RCENTER          (1L << 11)
#define PSTATUS_FARFIELD_FLEFT             (1L << 12)
#define PSTATUS_FARFIELD_FCENTER           (1L << 13)
#define PSTATUS_FARFIELD_FRIGHT            (1L << 14)
#define PSTATUS_FARFIELD_RCENTER           (1L << 15)
