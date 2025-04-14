/* A simple example showing how to move a cube and print it in H48 format. */

#include "../nissy.h"
#include <iostream>

int main() {
	nissy::cube c;
	if (!c.move("R' U' F").ok()) {
		std::cout << "Error moving the cube!" << std::endl;
		return 1;
	}

	auto string_or_error = c.to_string("H48");
	if (std::holds_alternative<nissy::error>(string_or_error)) {
		std::cout << "Error converting cube!" << std::endl;
		return 1;
	}

	auto str = std::get<std::string>(string_or_error);
	std::cout << "Cube in H48 format after R' U' F:" << std::endl
	    << str << std::endl;

	return 0;
}
