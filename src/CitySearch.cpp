#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <functional>
#include "CitySearch.h"


CitySearch::CitySearch() {}

double CitySearch::calculateL2Distance(double x0, double y0, double x1, double y1) {
	return std::sqrt(std::pow(x0 - x1, 2) + std::pow(y0 - y1, 2));
}

double CitySearch::calculateLInfinityDistance(double x0, double y0, double x1, double y1) {
	return std::max(std::abs(x0 - x1), std::abs(y0 - y1));
}

double CitySearch::calculateL1Distance(double x0, double y0, double x1, double y1) {
	return std::abs(x0 - x1) + std::abs(y0 - y1);
}



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

std::multimap<double, City> CitySearch::getCities() const
{
	return cities;
}

std::multimap<double, City> CitySearch::search(const City& city, float radius, int norm) {
	std::multimap<double, City> nearby_cities;
	std::multimap<double, City> nearby_cities_by_distance;
	double x_coordinate = city._coordinates._x_coordinate;
	double y_coordinate = city._coordinates._y_coordinate;

	using DistanceFunc = std::function<double(double, double, double, double)>;
	DistanceFunc distanceFuncs[3] = {
		[this](double x0, double y0, double x1, double y1) { return calculateL2Distance(x0, y0, x1, y1); },
		[this](double x0, double y0, double x1, double y1) { return calculateLInfinityDistance(x0, y0, x1, y1); },
		[this](double x0, double y0, double x1, double y1) { return calculateL1Distance(x0, y0, x1, y1); }
	};

	DistanceFunc distanceFunc = distanceFuncs[norm];
	std::copy_if(cities.cbegin(), cities.cend(), std::inserter(nearby_cities, nearby_cities.begin()), [x_coordinate, y_coordinate, radius, distanceFunc](const auto& pair) {
		double distance = distanceFunc(x_coordinate, y_coordinate, pair.second._coordinates._x_coordinate, pair.second._coordinates._y_coordinate);
		return distance <= radius;
		});


	std::transform(nearby_cities.begin(), nearby_cities.end(), std::inserter(nearby_cities_by_distance, nearby_cities_by_distance.begin()), [x_coordinate, y_coordinate, radius, distanceFunc](const auto& pair) {
		double distance = distanceFunc(x_coordinate, y_coordinate, pair.second._coordinates._x_coordinate, pair.second._coordinates._y_coordinate);
		if (distance <= radius) {
			City cityCopy = pair.second; // Create a copy of the city
			return std::make_pair(distance, cityCopy);
		}
		});


	return nearby_cities_by_distance;
}


int CitySearch::countNorthernCities(const std::multimap<double, City>& cities, const City& city) {
	int count = std::count_if(cities.begin(), cities.end(), [city](const auto& pair) {
		return  pair.second._coordinates._y_coordinate < city._coordinates._y_coordinate;
		});

	return count;
}