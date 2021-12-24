#include <fstream>
#include <algorithm>

#include "earthquake_database.h"


EarthquakeDatabase::EarthquakeDatabase(std::string filename) {
    
    std::ifstream in(filename.c_str());
    std::string line;
    while (getline(in, line)) {
        if (line.size() > 30) {
            Earthquake e(line);
            if (earthquakes.size() == 0) {
                min_mag_ = (float)e.magnitude();
                max_mag_ = (float)e.magnitude();
            }
            if (e.magnitude() > max_mag_) {
                max_mag_ = (float)e.magnitude();
            }
            if (e.magnitude() < min_mag_) {
                min_mag_ = (float)e.magnitude();
            }
            earthquakes.push_back(e);
        }
    }
}

Earthquake EarthquakeDatabase::earthquake(int index) {
    return earthquakes[index];
}

int EarthquakeDatabase::min_index() {
    return 250;
}

int EarthquakeDatabase::max_index() {
    return (int)earthquakes.size() - 1;
}

int EarthquakeDatabase::FindMostRecentQuake(Date d) {
    double targetSeconds = d.ToSeconds();
    int start = min_index();
    int end = max_index();
    while (start < end-1) {
        int half = (start + end) / 2;
        if (earthquakes[half].date().ToSeconds() > targetSeconds) {
            end = half - 1;
        }
        else {
            start = half;
        }
    }
    if (start == end) {
        return start;
    }
    else {
        if (earthquakes[end].date().ToSeconds() > targetSeconds) {
            return start;
        }
        else {
            return end;
        }
    }
}

float EarthquakeDatabase::min_magnitude() {
    return min_mag_;
}

float EarthquakeDatabase::max_magnitude() {
    return max_mag_;
}
