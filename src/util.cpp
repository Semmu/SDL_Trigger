#include "util.h"
#include <sstream>
#include <iomanip>

std::string currentTime() {
    std::stringstream stream;
    std::time_t currentTime = std::time(nullptr);
    std::tm calendarTime = *std::localtime(&currentTime);
    stream << std::put_time(&calendarTime, "%T");
    return stream.str();
}
