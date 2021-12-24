#include <ctime>
#include "date.h"

using namespace std;

const int SECONDS_PER_4_YEARS=126230400;

Date::Date() {
    year_ = 0;
    month_ = 0;
    day_ = 0;
    hour_ = 0;
    minute_ = 0;
    second_ = 0;
    ctime_ = 0;
    fouryears_ = 0;
}

// for visual studio, disable warning about localtime being unsafe
#pragma warning(push)
#pragma warning(disable: 4996)

Date::Date(double seconds) {
    double tweakedSeconds = seconds;
    fouryears_ = 0;
    while (tweakedSeconds < 0) {
        tweakedSeconds += SECONDS_PER_4_YEARS;
        fouryears_++;
    }
    ctime_ = int(tweakedSeconds);
    tm *t;
    t = localtime(&ctime_);
    year_ = t->tm_year + 1900 - 4 * fouryears_;
    month_ = t->tm_mon + 1;
    day_ = t->tm_mday;
    hour_ = t->tm_hour;
    minute_ = t->tm_min;
    second_ = t->tm_sec + (tweakedSeconds - ctime_);
}

#pragma warning(pop)

Date::Date(int m, int d, int y) {
    month_ = m;
    day_ = d;
    year_ = y;
    hour_ = 0;
    minute_ = 0;
    second_ = 0;
    fouryears_ = 0;
    tm t;
    while (year_ + fouryears_*4 < 1970) {
        fouryears_++;
    }
    t.tm_year = year_ - 1900 + fouryears_ * 4;
    t.tm_mon = month_ - 1;
    t.tm_mday = day_;
    t.tm_hour = 0;
    t.tm_min = 0;
    t.tm_sec = 0;
    ctime_ = mktime(&t);
}

Date::Date(int m, int d, int y, int hr, int min, double sec) {
    month_ = m;
    day_ = d;
    year_ = y;
    hour_ = hr;
    minute_ = min;
    second_ = sec;
    fouryears_ = 0;
    tm t;
    while (year_ + fouryears_*4 < 1970) {
        fouryears_++;
    }
    t.tm_year = year_ - 1900 + fouryears_ * 4;
    t.tm_mon = month_ - 1;
    t.tm_mday = day_;
    t.tm_hour = hour_;
    t.tm_min = minute_;
    t.tm_sec = int(second_);
    ctime_ = mktime(&t);
}

bool Date::operator<(const Date& other) {
    return ToSeconds() < other.ToSeconds();
}

bool Date::operator>(const Date& other) {
    return ToSeconds() > other.ToSeconds();
}

double Date::ToSeconds() const {
    return ctime_ + second_ - int(second_) - double(fouryears_) * SECONDS_PER_4_YEARS;
}

double Date::SecondsUntil(const Date& other) const {
    return ToSeconds() - other.ToSeconds();
}


int Date::year() {
    return year_;
}

int Date::month() {
    return month_;
}

int Date::day() {
    return day_;
}

int Date::hour() {
    return hour_;
}

int Date::minute() {
    return minute_;
}

double Date::second() {
    return second_;
}
