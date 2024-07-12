/*
Copyright 2023 - 2024 by Severin
and TrinityCore
*/
#pragma once

#include "Util.h"
#include "SharedDefines.h"
#include "Logger.h"
#include "TranslateText.h"

#include <algorithm>
#include <sstream>
#include <format>
#include <string>
#include <cctype>
#include <cstdarg>
#include <ctime>
#include <windows.h>
#include <cstdint>
#include <limits>
#include <stdexcept>

#include <QSpinBox>

Tokenizer::Tokenizer(const std::string& src, const char sep, std::uint32_t vectorReserve /*= 0*/, bool keepEmptyStrings /*= true*/)
{
	m_str = new char[src.length() + 1];
	memcpy(m_str, src.c_str(), src.length() + 1);

	if (vectorReserve)
		m_storage.reserve(vectorReserve);

	char* posold = m_str;
	char* posnew = m_str;

	for (;;)
	{
		if (*posnew == sep)
		{
			if (keepEmptyStrings || posold != posnew)
				m_storage.push_back(posold);

			posold = posnew + 1;
			*posnew = '\0';
		}
		else if (*posnew == '\0')
		{
			// Hack like, but the old code accepted these kind of broken strings,
			// so changing it would break other things
			if (posold != posnew)
				m_storage.push_back(posold);

			break;
		}

		++posnew;
	}
}

bool Util::QStringContainsOnlyDigits(const QString& string)
{
	for (const QChar& ch : string)
		if (!ch.isDigit())
			return false;

	return true;
}

std::uint32_t Util::ConvertStringToUint32(const std::string& str)
{
	try
	{
		std::istringstream iss(str);
		std::uint32_t value;
		if (!(iss >> value))
			throw std::invalid_argument("Invalid number.");

		if (!iss.eof())
			throw std::invalid_argument("Invalid number.");

		return value;
	}
	catch (const std::invalid_argument& e)
	{
		LOG_MISC("Error when converting the string: {} for string: {}", e.what(), str);
		return 0;
	}
}

std::uint16_t Util::ConvertStringToUint16(const std::string& str)
{
	try
	{
		std::istringstream iss(str);
		std::uint16_t value;
		if (!(iss >> value))
			throw std::invalid_argument("Invalid number.");

		if (!iss.eof())
			throw std::invalid_argument("Invalid number.");

		return value;
	}
	catch (const std::invalid_argument& e)
	{
		LOG_MISC("Error when converting the string: {} for string: {}", e.what(), str);
		return 0;
	}
}

std::uint32_t Util::ConvertVariantToUInt32(const QVariant& variant)
{
	if (variant.canConvert<qint32>())
	{
		qint32 value = variant.toInt();
		return static_cast<std::uint32_t>(value);
	}
	else
		return 0;
}

std::string Util::ReverseString(const std::string& str)
{
	std::string reverseString;
	for (auto it = str.rbegin(); it != str.rend(); ++it)
		reverseString.push_back(*it);

	return reverseString;
}

std::string Util::ReverseStringParts(const std::string& str)
{
	std::string reversedStr;
	std::string currentPart;
	for (char c : str)
	{
		if (c == ' ')
		{
			reversedStr += ReverseString(currentPart) + ' ';
			currentPart = "";
		}
		else
		{
			currentPart += c;
		}
	}

	if (!currentPart.empty())
	{
		reversedStr += ReverseString(currentPart);
	}
	return reversedStr;
}

std::string Util::ConvertUint32ToString(const std::uint32_t& value)
{
	try
	{
		std::string result = std::to_string(value);
		return result;
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error converting uint32_t to string: " << e.what() << std::endl;
		return "";
	}
}

std::vector<std::uint16_t> Util::splitStringToVectorUint16(std::string& str, std::string delim)
{
	std::vector<std::uint16_t> tokens;
	std::size_t start = 0, end = 0;

	while ((end = str.find_first_of(delim, start)) != std::string::npos) {
		if (start != end)
		{
			tokens.push_back(ConvertStringToUint16(str.substr(start, end - start)));
		}
		start = end + 1;
	}
	if (start != str.length())
	{
		tokens.push_back(ConvertStringToUint16(str.substr(start)));
	}
	return tokens;
}


