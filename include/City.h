#include <string>
#include "Coordinates.h"

class City {
  
public:
    std::string _name;
    Coordinates _coordinates;

    City(const std::string& name, const Coordinates& coordinate);
};
