#ifndef EARTHQUAKEDATABASE_H_
#define EARTHQUAKEDATABASE_H_

#include <string>
#include <vector>

#include "earthquake.h"
#include "date.h"

/** A database of earthquakes created by loading from a data file.
 */
class EarthquakeDatabase {
public:
    /// Creates an empty EarthquakeDatabase
    EarthquakeDatabase() {}

    /// Creates an EarthquakeDatabase from file
    EarthquakeDatabase(std::string filename);

    /// Returns the index of the most recent earthquake as of given date
    int FindMostRecentQuake(Date d);

    /// Returns Earthquake given index in file
    Earthquake earthquake(int index);

    /// Returns minimum index.  Note that this is not zero!
    int min_index();

    /// Returns maximum valid index.  Running
    /// getByIndex(getMaxIndex()) WILL return the last earthquake in the file
    int max_index();

    /// Returns the magnitude of the earthquake with the largest magnitude
    /// in the database
    float max_magnitude();
    
    /// Returns the magnitude of the earthquake with the smallest magnitude
    /// in the database
    float min_magnitude();

protected:
    std::vector<Earthquake> earthquakes;
    float min_mag_;
    float max_mag_;
};

#endif
