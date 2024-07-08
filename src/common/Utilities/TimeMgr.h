#pragma once

#include <string>
#include <QDateTimeEdit>

#include "TranslateText.h"

class TimeMgr
{
public:
	TimeMgr();
	~TimeMgr();

	std::uint64_t GetUnixTime();
	time_t GetUnixTimeAsTime();

	std::uint64_t ReturnPastOfThirtyDays();
	std::uint64_t ReturnPastOfDays(std::uint32_t returnDays);

	std::uint64_t GetThirtyDaysInFutureTimestamp();

	std::uint64_t GetDTETimeAsUint(QDateTimeEdit& timeEdit);
	std::uint64_t GetDTETimeAsUint(const QDateTimeEdit& timeEdit);
	std::string GetTimeEditTimeAsString(QDateTimeEdit& timeEdit);
	std::string GetTimeEditTimeAsString(const QDateTimeEdit& timeEdit);

	std::string ConvertUnixTimeToString(time_t unixTime);
	std::string ConvertUnixTimeToStringWithDefaultValue(time_t unixTime);
	std::pair<std::string, std::string> ConvertUnixTimeToDataTimeEdit(time_t unixTime);

	QDateTime ConvertTimeToQDataTime(std::pair<std::string, std::string> timeToConvert);

private:
	time_t _minOldTime;

};

