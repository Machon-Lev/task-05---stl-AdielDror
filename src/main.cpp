#include <iostream>
#include <string>
#include "CitySearch.h"
#include <vector>

void printCity(const std::multimap<double, City>& cities, std::map<double, City>::const_iterator it) {
	if (it != cities.end()) {
		std::cout << it->second._name << std::endl;
		printCity(cities, std::next(it));
	}
}

void printCities(const std::multimap<double, City>& cities) {
	std::cout << "City list:" << std::endl;
	printCity(cities, cities.begin());
}

int getIntegerInput(const std::string& prompt) {
	
		std::cout << prompt;
		std::string input;
		std::getline(std::cin, input);

		try {
			size_t pos;
			int value = std::stoi(input, &pos);

			if (pos == input.size()) {
				return value;
			}
		}
		catch (const std::exception& e) {
			throw std::runtime_error("Invalid input.");
		}
	
}

float getFloatInput(const std::string& prompt) {

	std::cout << prompt;
	std::string input;
	std::getline(std::cin, input);

	try {
		size_t pos;

		float value = std::stof(input, &pos);

		if (pos == input.size()) {
			return value;
		}
	}
	catch (const std::exception& e) {
		throw std::runtime_error("Invalid input.");
	}
}

int main() {
	CitySearch city_search;
	try {
		city_search.readCityDataFromFile("data.txt");
	}
	catch (const std::exception& e) {
		std::cout << "Error while reading the file: " << e.what() << std::endl;
		return 1;
	}


	std::string city_name;
	float radius;
	int norm;

	while (true) {
		std::cout << "Please enter selected city name (with line break after it): ";
		std::getline(std::cin, city_name);

		if (city_name == "0") {
			std::cout << "Bye";
			break; // Exit the loop if '0' is entered
		}

		std::multimap<double, City> cities = city_search.getCities();
		std::map<double, City>::iterator it = std::find_if(
			cities.begin(), cities.end(),
			[&](const std::pair<double, City>& pair) {
				return pair.second._name == city_name;
			}
		);



		if (it != cities.end()) {
			try {
				radius = getFloatInput("Please enter the wanted radius: ");
			}
			catch (const std::exception& e) {
				std::cout << e.what() << std::endl;
				return 1;
			}

			if (radius < 0) {
				std::cout << "Invalid radius. Please enter a non-negative value." << std::endl;
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				std::cout << std::endl;
				continue;
			}

			try {
				norm = getIntegerInput("Please enter the wanted norm (0 - L2, Euclidean distance, 1 - Linf, Chebyshev distance, 2 - L1, Manhattan distance): ");
			}
			catch (const std::exception& e) {
				std::cout << e.what() << std::endl;
				return 1;
			}

			if (norm < 0 || norm > 2) {
				std::cout << "Invalid norm. Please enter a valid norm number (0, 1, or 2)." << std::endl;
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				std::cout << std::endl;
				continue;

			}

			std::cout << "Search result:" << std::endl;

			City city(city_name, it->second._coordinates);
	
			std::multimap<double, City> nearby_cities = city_search.search(city, radius, norm);
			
			int number_of_cities = nearby_cities.size();
			std::cout << number_of_cities << " city/cities found within the given radius." << std::endl;

			int count_northern_cities = city_search.countNorthernCities(nearby_cities, city);
			std::cout << count_northern_cities << " cities are to the north of the selected city." << std::endl;

			printCities(nearby_cities);

		}
		else {
			// City not found in the map
			std::cout << "ERROR: \"" << city_name << "\" isn't found in the city list. Please try again." << std::endl;
		}

		std::cout << std::endl;

	}

	return 0;
}
