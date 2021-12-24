#include <sstream>
#include "earthquake.h"


Earthquake::Earthquake() {
}

Earthquake::Earthquake(std::string s) {
    line = s;
}

Date Earthquake::date() {
    int year = ParseInt(line.substr(12,4));
    int month = ParseInt(line.substr(17,2));
    int day = ParseInt(line.substr(20,2));
    int hour = ParseInt(line.substr(24,2));
    int minute = ParseInt(line.substr(27,2));
    double second = ParseFloat(line.substr(30,5));

    return Date(month, day, year, hour, minute, second);
}

double Earthquake::longitude() {
    return ParseFloat(line.substr(44,8));
}

double Earthquake::latitude() {
    return ParseFloat(line.substr(37,7));
}

double Earthquake::magnitude() {
    return ParseFloat(line.substr(66,4));
}

double Earthquake::ParseFloat(std::string s) {
    std::stringstream ss(s);
    double f;
    ss >> f;
    return f;
}

int Earthquake::ParseInt(std::string s) {
    std::stringstream ss(s);
    int i;
    ss >> i;
    return i;
}
