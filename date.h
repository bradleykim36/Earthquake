#ifndef DATE_H_
#define DATE_H_

#include <ctime>
#include <iostream>

/** A date class that handles leap years and can convert itself to seconds and
    handle < and > operations.
 */
class Date {
public:
    Date();
    Date(double seconds);
    Date(int m, int d, int y);
    Date(int m, int d, int y, int hr, int min, double sec);

    bool operator<(const Date& other);
    bool operator>(const Date& other);

    double ToSeconds() const;

    double SecondsUntil(const Date& other) const;
    
    int year();
    
    int month();
    
    int day();
    
    int hour();
    
    int minute();
    
    double second();

private:
    std::time_t ctime_;
    int fouryears_;
    int year_;
    int month_;
    int day_;
    int hour_;
    int minute_;
    double second_;
};

#endif
