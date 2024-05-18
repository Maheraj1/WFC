/**
 NOTE: You are supposed to add "\n" in the log (Helper Functions i.e. Error, Info and Warning as an exeption)
*/

#pragma once

#include <functional>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>

namespace ImLog
{
    /**
     * @brief Enum with number for colored text output
     */
    enum class ImLogColor: int
    {
        None           = 0,
        Black          = 30,
        Red            = 31,
        Green          = 32,
        Yellow         = 33,
        Blue           = 34,
        Magenta        = 35,
        Cyan           = 36,
        White          = 37,
        BrightBlack    = 90,
        BrightRed      = 91,
        BrightGreen    = 92,
        BrightYellow   = 93,
        BrightBlue     = 94,
        BrightMagenta  = 95,
        BrightCyan     = 96,
        BrightWhite    = 97,
    };
    
    /**
     * @brief Enum with number for colored background text output
     */
    enum class ImLogBackGroundColor: int
    {
        None           = 0,
        Black          = 40,
        Red            = 41,
        Green          = 42,
        Yellow         = 43,
        Blue           = 44,
        Magenta        = 45,
        Cyan           = 46,
        White          = 47,
        BrightBlack    = 100,
        BrightRed      = 101,
        BrightGreen    = 102,
        BrightYellow   = 103,
        BrightBlue     = 104,
        BrightMagenta  = 105,
        BrightCyan     = 106,
        BrightWhite    = 107,
    };

	class ImLogOut: public std::ostringstream {
		public:
            ImLogOut() = default;
            ImLogOut(std::string str);

			ImLogOut& operator<<(ImLogColor color) {
				*this << "\e[" << (int)color << 'm';
				return *this;
			}

			ImLogOut& operator<<(ImLogBackGroundColor color) {
				*this << "\e[" << (int)color << 'm';
				return *this;
			}
	};

    /// Api functions

    struct API {
        API() = delete;
        ~API() = delete;
        static std::ostream* stream;
        static std::function<void(bool)> assertFunc;
    };

    
    /// Core Log functions
    
    /**
        * @brief Method for logging formatted text with colors
        * 
        * @param text format to be used
        * @param color color of text (optional)
        * @param bg background color of text (optional)
        */
    void Log(const char* fmt, ...) __attribute__((format(printf, 1, 2)));

    /**
     * @brief Logs Text to console
    */
    void Log(std::string text);

    /**
        * @brief Logs from a DebugLogOut buffer
        * 
        * @param buffer 
    */
    void Log(ImLogOut& buffer);

    /**
        * @brief Method to Assert in Debug Build
        * 
        * @param condition Assertion condition
        * @param text Text to Print
        */
    void Assert(bool condition = false, std::string text = "");

    /// Helper Functions

    /**
        * @brief Method for logging info (In Green)
        * 
        * @param text text to be logged
        * @param from 
        */
    void Info(std::string text, std::string from = "Info");
    
    /**
        * @brief Method for Error Loging
        * 
        * @param text Error text to be logged
        * @param errorType Error name for more debuging information
        */
    void Error(std::string text, std::string errorType = "Error", bool Assert = false);

    /**
        * @brief Method for Loging Warning
        * 
        * @param text Warning text to be logged
        */
    void Warning(std::string text);
}