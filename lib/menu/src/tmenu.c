
#include "tmenu.h"
#include "menu_types.h"
#include "menu_util.h"

#include "rtc.h"
#include "utils.h"

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>


/* Prototypes.*/
static void TM_timeShow(char* cmdbuf);
static void TM_timeSet(char* cmdbuf);

static void TM_dateShow(char* cmdbuf);
static void TM_dateSet(char* cmdbuf);

TM_values_t TM_values;

void TM_initMenu(void) {
    //  Currently no state variable
}

TM_values_t* TM_getData(void) {
    return &TM_values;
}

static void TM_timeShow(char* cmdbuf) {
    rtc_time_t t = rtc_get_time();

    printf("\nTime is: %02d:%02d:%02d\n", t.hour, t.minute, t.second);
}

static void TM_dateShow(char* cmdbuf) {
    rtc_date_t d = rtc_get_date();

    printf("\nDate is: %04d/%02d/%02d\n", (int)(2000 + d.year), d.month, d.day);
}

static void TM_timeSet(char* cmdbuf) {
    int length = 0;
    int rc = trim4timeDate(cmdbuf, &length);
    if (rc) {
        return;
    }

    if (length != 6) {
        return;
    }

    rtc_time_t t;

    {
        int hour = 0;
        int minute = 0;
        int second = 0;
        rc = sscanf(cmdbuf, "%2d%2d%2d", &hour, &minute, &second);
        if (3 != rc) {
            return;
        }

        t.hour = (uint8_t)hour;
        t.minute = (uint8_t)minute;
        t.second = (uint8_t)second;
    }

    if (isTimeValid(&t)) {
        return;
    }

    rc = rtc_set_time(&t);
    if (rc) {
        printf("\nUnable to set RTC time\n");
        return;
    }

    printf("\nTime is: %02d:%02d:%02d\n", t.hour, t.minute, t.second);
}

static void TM_dateSet(char* cmdbuf) {
    int length = 0;
    int rc = trim4timeDate(cmdbuf, &length);
    if (rc) {
        return;
    }

    rtc_date_t d;

    {
        uint8_t year = 0;
        uint8_t month = 0;
        uint8_t day = 0;

        if (length == 6) {
            rc = sscanf(cmdbuf, "%2d%2d%2d", &year, &month, &day);
            if (3 != rc) {
                return;
            }
        } else if (length == 8) {
            rc = sscanf(cmdbuf, "20%2u%2u%2u", &year, &month, &day);
            if (3 != rc) {
                return;
            }
        }
        d.year = year;
        d.month = month;
        d.day = day;
    }

    rc = isDateValid(&d);
    if (rc != 0) {
        return;
    }

    rtc_set_date(&d);

    printf("\nDate is: %04d/%02d/%02d\n", (int)d.year + 2000, (int)d.month, (int)d.day);
}

/* clang-format off */
const Cmd TM_menu_table[] = {
    {"S", 0, TM_timeSet,  "System time",  NULL, TM_timeShow },
    {"D", 0, TM_dateSet,  "System date",  NULL, TM_dateShow },
    {"?", 0, 0, "Display help", NULL},
    { 0,  0, 0, 0, NULL}
};
/* clang-format on */
