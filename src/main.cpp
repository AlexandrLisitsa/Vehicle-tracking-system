#include <SoftwareSerial.h>
#include "Arduino.h"

#include <TinyGPS.h>

/* This sample code demonstrates the normal use of a TinyGPS object.
   It requires the use of SoftwareSerial, and assumes that you have a
   4800-baud serial GPS device hooked up on pins 4(rx) and 3(tx).
*/

TinyGPS gps;
SoftwareSerial ss(4, 3);

struct GpsData {
    float x, y;
    float speed;
    char *date;

    ~GpsData() {
        delete[] date;
        date = nullptr;
    }
};

static float getPosition(float value, float invalid);

static char *getDate(TinyGPS &gps);

static GpsData getGpsData();


void setup() {
    Serial.begin(9600);
    ss.begin(9600);
}

void loop() {

    GpsData data = getGpsData();

    Serial.print(data.x, 6);
    Serial.print(" ");
    Serial.print(data.y, 6);
    Serial.print(" ");
    Serial.print((int) data.speed);
    Serial.print(" ");
    Serial.print(data.date);
    Serial.println();

    delay(1000);
}

static GpsData getGpsData() {
    GpsData gpsData;

    float flat, flon;
    unsigned long age = 0;
    static const double LONDON_LAT = 51.508131, LONDON_LON = -0.128002;

    gps.f_get_position(&flat, &flon, &age);
    float parsedLat = getPosition(flat, TinyGPS::GPS_INVALID_F_ANGLE);
    float parsedLong = getPosition(flon, TinyGPS::GPS_INVALID_F_ANGLE);
    float parsedSpeed = getPosition(gps.f_speed_kmph(), TinyGPS::GPS_INVALID_F_ANGLE);


    gpsData.x = parsedLat;
    gpsData.y = parsedLong;
    gpsData.speed = parsedSpeed;
    gpsData.date = getDate(gps);

    return gpsData;
}


static float getPosition(float value, float invalid) {
    if (value == invalid) {
        return -1;
    } else {
        return value;
    }
}

static char *getDate(TinyGPS &gps) {
    int year;
    byte month, day, hour, minute, second, hundredths;
    unsigned long age;
    gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &age);
    if (age == TinyGPS::GPS_INVALID_AGE)
        Serial.print("********** ******** ");
    else {
        char *sz = new char[32];
        sprintf(sz, "%02d/%02d/%02d %02d:%02d:%02d ",
                month, day, year, hour, minute, second);
        return sz;
        //Serial.print(sz);
    }
}