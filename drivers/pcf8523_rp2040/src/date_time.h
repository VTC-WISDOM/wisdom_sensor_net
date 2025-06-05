//| date_time.h
//| A struct and set of related functions for keeping date/time stamps
//| respecting the limitations of the current RTC I'm working with: PCF8523
#ifndef DATE_TIME_EMO_H
#define DATE_TIME_EMO_H

#include <stdbool.h>
#include <stdint.h>

typedef unsigned uint;

typedef enum _MONTH_E {
	JANUARY = 1,
	FEBRUARY,
	MARCH,
	APRIL,
	MAY,
	JUNE,
	JULY,
	AUGUST,
	SEPTEMBER,
	OCTOBER,
	NOVEMBER,
	DECEMBER
} MONTH_T;

extern const char *month_string_lookup[];

// Weekday is left out for simplicity.
// Might reconsider this later.
struct date_time_s {
	uint8_t minutes; // 0-59
	uint8_t hours;   // 0-23
	uint8_t days;    // 1-31 (month/year dependant)
	uint8_t months;  // 1-12
	uint8_t years;   // 0-99
	// Century flag is to prevent incorrect year comparisons.
	// e.g. 1999 vs 2001 since limitation in PCF8523 hardware would
	// represent those years as 99 and 01.
	//
	// (years = 99, century = false) < (years = 01, century = true)
	bool century;
};

// if return > 0, a after b
// if return < 0, a before b
// if return == 0, a == b
int date_time_cmp(struct date_time_s *dt_a, struct date_time_s *dt_b);
void date_time_add(struct date_time_s *dt, struct date_time_s *add);

void date_time_print(struct date_time_s *dt);

#endif // DATE_TIME_EMO_H
