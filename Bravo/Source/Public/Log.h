#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <iomanip> // for std::setw, std::setprecision
#include <type_traits> // for std::is_floating_point
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <memory>
#include <string>
#include <stdexcept>



enum ELog
{
	Log,
	Warning,
	Error
};

class Log
{
public:
	static void LogMessage(const std::string& Message, ELog Type = ELog::Log);
	static void LogMessage(const char* const Message, ELog Type = ELog::Log);


	template<typename T>
	static std::string to_string(const T& val)
	{
		return to_string_internal<T>(val);
	}

private:

	// Templated function to convert non-glm types to string using std::to_string
	template<typename T>
	static std::string to_string_internal(const T& val) 
	{
		return std::to_string(val);
	}
	
	// Templated function to convert glm vector types to string
	template<typename T, glm::length_t L, typename U = typename T::value_type>
	static std::string to_string_internal(const glm::vec<L, U>& vec) 
	{
		// Create a stringstream to build the string
		std::stringstream ss;

		// Set the width for each component
		const int componentWidth = 6;

		// Write each component of the vector to the stringstream
		ss << "(";
		for (int i = 0; i < L; ++i) 
		{
			if (i > 0) 
			{
				ss << " ";
			}
			// Set precision for floating-point types
			if constexpr (std::is_floating_point<U>::value) 
			{
				ss << std::setw(componentWidth) << std::fixed << std::setprecision(3) << std::left << vec[i];
			} 
			else 
			{
				ss << std::setw(componentWidth) << std::left << vec[i];
			}
		}
		ss << ")";

		// Convert the stringstream to a string and return it
		return ss.str();
	}

	// Overload for glm::quat to construct a string representation
	template<typename T, glm::qualifier Q = glm::defaultp>
	static std::string to_string_internal(const glm::qua<T, Q>& qua)
	{
		return "(" + to_string_internal<glm::vec>(glm::vec3(qua.x, qua.y, qua.z)) + " " + to_string(qua.w) + ")";
	}

	// Templated function to convert glm matrix types to string
	template<typename T, glm::length_t C, glm::length_t R, typename U = typename T::value_type>
	static std::string to_string_internal(const glm::mat<C, R, U>& mat) 
	{
		std::string str = "(";
		for (int i = 0; i < C; ++i) 
		{
			if (i > 0) 
			{
				str += ", ";
			}
			str += to_string_internal<T>(mat[i]);
		}
		str += ")";
		return str;
	}
};
