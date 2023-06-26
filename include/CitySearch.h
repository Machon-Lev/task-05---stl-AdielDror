#include <map>
#include <string>
#include "City.h"

class CitySearch {
private:
    std::multimap<double, City> cities;

    double calculateL2Distance(double x0, double y0, double x1, double y1);
    double calculateLInfinityDistance(double x0, double y0, double x1, double y1);
    double calculateL1Distance(double x0, double y0, double x1, double y1);

public:
    CitySearch();

    void readCityDataFromFile(const std::string& filename);
    const std::multimap<double, City>& getCities() const;

    std::multimap<double, City> search(const City& city, float radius, int norm);
    int countNorthernCities(const std::multimap<double, City>& cities, const City& city) const;

};