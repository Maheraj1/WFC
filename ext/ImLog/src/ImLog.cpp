#include "ImLog.hpp"
#include <cassert>
#include <cstdarg>
#include <functional>
#include <iostream>
#include <sstream>

namespace ImLog {

    std::ostream* API::stream = &std::cout;
    std::function<void(bool)> API::assertFunc = [](bool condition) { assert(condition); };

    void Log(const char* fmt, ...) {
        va_list list;
		va_start(list, fmt);
        
        int size = snprintf(NULL, 0, fmt, list);
		char* buffer = new char[size + 1];
        snprintf(buffer, size, fmt, list);
        
        Log((std::string)buffer);
        va_end(list);
    }


    void Log(ImLogOut& buffer) {
        Log(buffer.str());
    }
    
    void Log(std::string text) {
        *API::stream << text;
    }

    void Error(std::string text, std::string errorType, bool Assert) {
        std::string statment = '(' + errorType + "): " + text + "\n";

        if (Assert)
		{
            ImLog::Assert(false, statment);
        } else {
            ImLog::Log("\e[" + std::to_string((int)ImLogColor::Red) + statment);
        }
    }

    void Assert(bool condition, std::string text) {
        if (!text.empty()) {
            ImLogOut out;
            out << ImLogColor::Red << text;
        }

        API::assertFunc(false);
    }

    void Info(std::string text, std::string from) {
        Log("\e[" + std::to_string((int)ImLogColor::Green) + '(' + from + "): " + text + '\n');
    }

    void Warning(std::string text) {
        Log("\e[" + std::to_string((int)ImLogColor::Green) + "(Warning): " + text + '\n');
    }

    ImLogOut::ImLogOut(std::string str) 
    :std::ostringstream(str)
    { }
}