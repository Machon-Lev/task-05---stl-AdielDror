I used multimap data structures for storing the cities data from the file.
Since the printing had to be done in sorted order according to the distance from the given city,
so I chose to save in a multimap where you can store data with identical keys, so that initially,
when reading from the file, the distance of all the cities is 0, and then when searching by a given city and calculating the distance of each city,
the data enters a new map that also It's of type multimap where the key is the distance, so it's stored in sorted order by distance,
and then when we print these data structures, it will print in sorted order.