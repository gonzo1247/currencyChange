
#include <chrono>
#include <cstdint>

#include "TimeMgr.h"

TimeMgr::TimeMgr() : _minOldTime(1704063600)
{ }

TimeMgr::~TimeMgr() { }

std::uint64_t TimeMgr::GetUnixTime()
{
	auto now = std::chrono::system_clock::now();
	auto duration = now.time_since_epoch();
	return std::chrono::duration_cast<std::chrono::seconds>(duration).count();
}

time_t TimeMgr::GetUnixTimeAsTime()
{
	auto now = std::chrono::system_clock::now();
	return std::chrono::system_clock::to_time_t(now);
}

std::uint64_t TimeMgr::ReturnPastOfThirtyDays()
{
	auto now = std::chrono::system_clock::now();

	// Calculate the duration of 30 days in seconds
	auto thirtyDaysAgo = now - std::chrono::hours(24 * 30);

	// Set the time to 00:00:00
	auto startOfDay = std::chrono::time_point_cast<std::chrono::days>(thirtyDaysAgo);

	// Convert the time in seconds since the epoch
	return std::chrono::duration_cast<std::chrono::seconds>(startOfDay.time_since_epoch()).count();
}

std::uint64_t TimeMgr::ReturnPastOfDays(std::uint32_t returnDays)
{
	auto now = std::chrono::system_clock::now();

	// Calculate the duration of 30 days in seconds
	auto thirtyDaysAgo = now - std::chrono::hours(24 * returnDays);

	// Set the time to 00:00:00
	auto startOfDay = std::chrono::time_point_cast<std::chrono::days>(thirtyDaysAgo);

	// Convert the time in seconds since the epoch
	return std::chrono::duration_cast<std::chrono::seconds>(startOfDay.time_since_epoch()).count();
}

std::uint64_t TimeMgr::GetThirtyDaysInFutureTimestamp()
{
	// Call up the current time
	auto now = std::chrono::system_clock::now();

	// Define duration of 30 days
	auto thirty_days = std::chrono::hours(24 * 30);

	// Calculate time 30 days in the future
	auto future_time = now + thirty_days;

	// Conversion to a Unix timestamp
	auto future_timestamp = std::chrono::system_clock::to_time_t(future_time);

	return static_cast<std::uint64_t>(future_timestamp);
}

std::uint64_t TimeMgr::GetDTETimeAsUint(QDateTimeEdit& timeEdit)
{
	std::uint64_t unixTime = 0;
	if (timeEdit.dateTime().toSecsSinceEpoch() >= 0)
		 unixTime = static_cast<std::uint64_t>(timeEdit.dateTime().toSecsSinceEpoch());

	return unixTime;
}

std::uint64_t TimeMgr::GetDTETimeAsUint(const QDateTimeEdit& timeEdit)
{
	std::uint64_t unixTime = 0;
	if (timeEdit.dateTime().toSecsSinceEpoch() >= 0)
		unixTime = static_cast<std::uint64_t>(timeEdit.dateTime().toSecsSinceEpoch());

	return unixTime;
}

std::string TimeMgr::GetTimeEditTimeAsString(QDateTimeEdit& timeEdit)
{
	std::uint64_t unixTime = 0;
	if (timeEdit.dateTime().toSecsSinceEpoch() >= 0)
		unixTime = static_cast<std::uint64_t>(timeEdit.dateTime().toSecsSinceEpoch());

	return std::to_string(unixTime);
}

std::string TimeMgr::GetTimeEditTimeAsString(const QDateTimeEdit& timeEdit)
{
	std::uint64_t unixTime = 0;
	if (timeEdit.dateTime().toSecsSinceEpoch() >= 0)
		unixTime = static_cast<std::uint64_t>(timeEdit.dateTime().toSecsSinceEpoch());

	return std::to_string(unixTime);
}

std::string TimeMgr::ConvertUnixTimeToString(time_t unixTime)
{
	// Create a struct tm object
	struct tm timeinfo;

	// Convert Unix time to struct tm using localtime_s
	if (localtime_s(&timeinfo, &unixTime) != 0)
	{
		// If the conversion fails (e.g., invalid Unix time), return an empty string
		return "";
	}

	// Create a string stream for formatting
	std::stringstream ss;

	// Format the date and time in the desired format (here as an example)
	ss << std::put_time(&timeinfo, "%d-%m-%Y %H:%M:%S");

	// Return the resulting string
	return ss.str();
}

std::string TimeMgr::ConvertUnixTimeToStringWithDefaultValue(time_t unixTime)
{
	if (_minOldTime <= unixTime)
	{
		// Create a struct tm object
		struct tm timeinfo;

		// Convert Unix time to struct tm using localtime_s
		if (localtime_s(&timeinfo, &unixTime) != 0)
		{
			// If the conversion fails (e.g., invalid Unix time), return an empty string
			return "";
		}

		// Create a string stream for formatting
		std::stringstream ss;

		// Format the date and time in the desired format (here as an example)
		ss << std::put_time(&timeinfo, "%d-%m-%Y %H:%M:%S");

		// Return the resulting string
		return ss.str();
	}
	else
	{
		std::unique_ptr<TranslateText> _translate = std::make_unique<TranslateText>();
		return _translate->translate("TimeString", "No Time Available").toStdString();
	}
}

std::pair<std::string, std::string> TimeMgr::ConvertUnixTimeToDataTimeEdit(time_t unixTime)
{
	// Create a struct tm object
	struct tm timeinfo;

	// Convert Unix time to struct tm using localtime_s
	if (localtime_s(&timeinfo, &unixTime) != 0)
	{
		// If the conversion fails (e.g., invalid Unix time), return an empty pair
		return std::make_pair("", "");
	}

	// Create a string stream for formatting
	std::stringstream ss;

	// Format the date and time in the desired format (here as an example)
	ss << std::put_time(&timeinfo, "%d-%m-%Y %H:%M:%S");

	// Return a pair of strings: the formatted date/time and the format
	return std::make_pair(ss.str(), "%d-%m-%Y %H:%M:%S");
}


QDateTime TimeMgr::ConvertTimeToQDataTime(std::pair<std::string, std::string> timeToConvert)
{
	auto& timeResult1 = timeToConvert;
	std::string formattedDateTime = timeResult1.first;
	std::string setDateTimeFormat = timeResult1.second;

	return QDateTime::fromString(formattedDateTime.c_str(), setDateTimeFormat.c_str());
}

