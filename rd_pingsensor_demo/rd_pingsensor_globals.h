////////////////////////////////////////////////////////////////////////////////
// Defines

// For the sake of this demo...
// The ping sensor's viewable range is broken up into three distinct regions
// (i.e., danger-close, near-field, and far-field). The danger-close region
// defines the space in front of the sensor that is closest. The far-field
// defines the space in front of the sensor that is farthest; up to the max
// detection range. The near-field region is defined as the space between the 
// danger-close and far-field regions. The limit values for these regions are
// defined by the echo return duration in microseconds. For the Parralax Ping)))
// sensor, echo return durations are between 115 and 18500 microseconds long.

#define LIMIT_DNGRCLOSE                    6243
#define LIMIT_FARFIELD                     12373

// ping_status bit-field definitions
// three front-facing sensors, one rear-facing sensor
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
