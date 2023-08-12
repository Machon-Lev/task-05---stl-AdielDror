#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <functional>
#include "CitySearch.h"


CitySearch::CitySearch() {}

/**
 * Calculates the Euclidean distance (L2 norm) between two points in a 2D space.
 *
 * @param x0 The x-coordinate of the first point.
 * @param y0 The y-coordinate of the first point.
 * @param x1 The x-coordinate of the second point.
 * @param y1 The y-coordinate of the second point.
 * @return The Euclidean distance between the two points.
 */
double CitySearch::calculateL2Distance(double x0, double y0, double x1, double y1) {
	return std::sqrt(std::pow(x0 - x1, 2) + std::pow(y0 - y1, 2));
}

/**
 * Calculates the Chebyshev distance (L-infinity norm) between two points in a 2D space.
 *
 * @param x0 The x-coordinate of the first point.
 * @param y0 The y-coordinate of the first point.
 * @param x1 The x-coordinate of the second point.
 * @param y1 The y-coordinate of the second point.
 * @return The Chebyshev distance between the two points.
 */
double CitySearch::calculateLInfinityDistance(double x0, double y0, double x1, double y1) {
	return std::max(std::abs(x0 - x1), std::abs(y0 - y1));
}

/**
 * Calculates the Manhattan distance (L1 norm) between two points in a 2D space.
 *
 * @param x0 The x-coordinate of the first point.
 * @param y0 The y-coordinate of the first point.
 * @param x1 The x-coordinate of the second point.
 * @param y1 The y-coordinate of the second point.
 * @return The Manhattan distance between the two points.
 */
double CitySearch::calculateL1Distance(double x0, double y0, double x1, double y1) {
	return std::abs(x0 - x1) + std::abs(y0 - y1);
}


/**
 * Reads city data from a file and populates the CitySearch object with the extracted information.
 * The file should contain city information in the format:
 * City Name
 * x-coordinate - y-coordinate
 *
 * @param filename The name of the file to read city data from.
 * @throws std::runtime_error if the file fails to open, encounters incorrect information, or errors occur during conversion.
 */
void CitySearch::readCityDataFromFile(const std::string& filename) {
	std::ifstream file(filename);
	if (!file.is_open()) {
		throw std::runtime_error("Failed to open the file.");

	}

	std::string line;
	std::string city_name;


	while (std::getline(file, line)) {
		if (city_name.empty()) {
			city_name = line;
		}
		else {
			std::string coordinate_x;
			std::string coordinate_y;
			std::stringstream ss(line);
			std::getline(ss, coordinate_x, '-');
			std::getline(ss, coordinate_y, '\n');

			double check_x, check_y;
			try {
				check_x = std::stod(coordinate_x);
				check_y = std::stod(coordinate_y);
			}
			catch (const std::exception& e) {
				throw std::runtime_error("Error converting coordinates to double: " + std::string(e.what()));
			}

			if (ss.fail() || !ss.eof()) {
				throw std::runtime_error("Incorrect information encountered while reading the file.");
			}


			cities.insert({ 0, {city_name, {check_x, check_y} } });
			city_name.clear();
		}
	}

	file.close();
}

/**
 * Retrieves a constant reference to the multimap of cities stored in the CitySearch object.
 *
 * @return A constant reference to the multimap of cities.
 */
const std::multimap<double, City>& CitySearch::getCities() const{

	return cities;
}

/**
 * Performs a search for cities within a specified radius from a given city, based on the selected distance norm.
 * The search uses the coordinates of the cities and calculates the distance using the specified norm function.
 * The resulting cities are sorted in ascending order by their distance from the given city.
 *
 * @param city The reference city to search around.
 * @param radius The radius within which to search for cities.
 * @param norm The norm to use for distance calculation (0 - L2, 1 - Linf, 2 - L1).
 * @return A multimap of nearby cities sorted by their distance from the given city.
 */
std::multimap<double, City> CitySearch::search(const City& city, float radius, int norm) {
	std::multimap<double, City> nearby_cities;
	std::multimap<double, City> nearby_cities_by_distance;
	double x_coordinate = city._coordinates._x_coordinate;
	double y_coordinate = city._coordinates._y_coordinate;

	// Define distance calculation function based on the selected norm
	using DistanceFunc = std::function<double(double, double, double, double)>;
	DistanceFunc distanceFuncs[3] = {
		[this](double x0, double y0, double x1, double y1) { return calculateL2Distance(x0, y0, x1, y1); },
		[this](double x0, double y0, double x1, double y1) { return calculateLInfinityDistance(x0, y0, x1, y1); },
		[this](double x0, double y0, double x1, double y1) { return calculateL1Distance(x0, y0, x1, y1); }
	};

	DistanceFunc distanceFunc = distanceFuncs[norm];

	// Copy cities within the radius to the nearby_cities multimap
	std::copy_if(cities.cbegin(), cities.cend(), std::inserter(nearby_cities, nearby_cities.begin()), [x_coordinate, y_coordinate, radius, distanceFunc](const auto& pair) {
		double distance = distanceFunc(x_coordinate, y_coordinate, pair.second._coordinates._x_coordinate, pair.second._coordinates._y_coordinate);
		return distance <= radius;
		});

	// Create a new multimap with cities sorted by their distance from the given city
	std::transform(nearby_cities.begin(), nearby_cities.end(), std::inserter(nearby_cities_by_distance, nearby_cities_by_distance.begin()), [x_coordinate, y_coordinate, radius, distanceFunc](const auto& pair) {
		double distance = distanceFunc(x_coordinate, y_coordinate, pair.second._coordinates._x_coordinate, pair.second._coordinates._y_coordinate);
		if (distance <= radius) {
			City cityCopy = pair.second; // Create a copy of the city
			return std::make_pair(distance, cityCopy);
		}
		});


	return nearby_cities_by_distance;
}

/**
 * Counts the number of cities in the given multimap that are located to the north of the specified city.
 * The determination is based on the y-coordinate of each city's coordinates.
 *
 * @param cities The multimap of cities to search.
 * @param city The reference city used as a point of comparison.
 * @return The count of cities that are to the north of the specified city.
 */
int CitySearch::countNorthernCities(const std::multimap<double, City>& cities, const City& city) const{
	int count = std::count_if(cities.begin(), cities.end(), [city](const auto& pair) {
		return  pair.second._coordinates._y_coordinate < city._coordinates._y_coordinate;
		});

	return count;
}