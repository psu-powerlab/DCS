#ifndef LOGGER_H_INCLUDED
#define LOGGER_H_INCLUDED

// INCLUDES
#include <string>
#include <sstream>

// Logger
// - simple create inline and load with context and path.
// - then use "<<" to add arguments to log.
//
// Example: 
// Logger("INFO") << "Data\t" << "More Data";
class Logger {
public:
	// Constructor/Destructor
	Logger (std::string context, std::string path);
	virtual ~Logger ();

	// Operator Overloads
	template <typename T>
	Logger& operator << (T rhs) {
		// string stream is a simple way to convert any data type into a string
	    std::ostringstream ss;
	    ss << rhs;
		msg_ += ss.str();
		return *this;
	};

private:
	std::string msg_;
	std::string context_;
	std::string path_;

private:
	void ToFile ();
	std::string GetDate ();
	std::string GetDateTime ();
};

#endif // LOGGER_H_INCLUDED
