#include <iostream>
#include <string>
#include <sstream>

int main() {
	std::cout << "Please enter flags separated by space to be active from 0 to 63" << std::endl;

	std::string input;
	std::getline(std::cin, input);

	if (input.empty()) {
		std::cout << "No flags passed" << std::endl;
		return -1;
	}

	std::stringstream ss(input);
	uint64_t data;
	uint64_t num;

	while (ss >> num) {
		data |= (1ll << num);
	}

	std::cout << data << std::endl;
}