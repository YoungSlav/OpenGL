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
	Success,
	Log,
	Warning,
	Error
};

class Log
{
public:
	template<typename... Args>
	static void LogMessage(ELog Type, const std::string& format_str, Args&&... args)
	{
		auto tuple_args = std::make_tuple(Log::to_string(std::forward<Args>(args))...);
		auto format_args = std::apply(
			[&](auto&... args) { return std::make_format_args(args...); },
			tuple_args
		);
		Print(std::vformat(format_str, format_args), Type);
	}
private:

	static void Print(const std::string& Message, ELog Type = ELog::Log);
	static void Print(const char* const Message, ELog Type = ELog::Log);

	
	template<typename T>
	static std::string to_string(const T& val)
	{
		return to_string_internal<T>(val);
	}

	template<typename T=char*>
	static std::string to_string_internal(const char* val) 
	{
		return std::string(val);
	}

	template<typename T=std::string>
	static std::string to_string_internal(const std::string& val) 
	{
		return val;
	}

	//template<typename T, typename U = typename T::value_type, typename Q = glm::qualifier::defaultp >
	template<typename T=glm::qua, typename U = typename T::value_type, glm::qualifier Q = glm::qualifier::defaultp>
	static std::string to_string_internal(const glm::qua<U, Q>& qua)
	{
		return "(" + to_string_internal<glm::vec3>(glm::normalize(glm::vec3(qua.x, qua.y, qua.z))) + " " + to_string_internal<float>(glm::degrees(qua.w)) + ")";
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

	// Templated function to convert non-glm types to string using std::to_string
	template<typename T>
	static std::string to_string_internal(const T& val) 
	{
		return std::to_string(val);
	}
};
