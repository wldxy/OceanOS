#ifndef _TYPES_H_
#define _TYPES_H_

#ifndef NULL
    #define NULL 0
#endif

#ifndef TRUE
    #define TRUE  1
    #define FALSE 0
#endif

typedef unsigned int   uint32_t;
typedef          int   int32_t;
typedef unsigned short uint16_t;
typedef          short int16_t;
typedef unsigned char  uint8_t;
typedef          char  int8_t;

typedef          int   pid_t;

typedef unsigned int   pgd_t;
typedef unsigned int   pte_t;

typedef
enum real_color {
    rc_black = 0,
    rc_blue = 1,
    rc_green = 2,
    rc_cyan = 3,
    rc_red = 4,
    rc_magenta = 5,
    rc_brown = 6,
    rc_light_grey = 7,
    rc_dark_grey = 8,
    rc_light_blue = 9,
    rc_light_green = 10,
    rc_light_cyan = 11,
    rc_light_red = 12,
    rc_light_magenta = 13,
    rc_light_brown  = 14,
    rc_white = 15
} real_color_t;

#endif  // INCLUDE_TYPES_H_
