/*
Copyright 2023 - 2024 by Severin
and TrinityCore
*/

#pragma once
#include <QString>
#include <qspinbox.h>
#include <QObject>
#include <QAction>
#include <QKeySequence>
#include <QMainWindow>

#include <format>
#include <string>
#include <sstream>
#include <vector>

class TranslationHandler;
enum class ErrorCodes : std::uint64_t;
enum class SuccessfullCodes : std::uint64_t;
enum class AccessRights : std::uint8_t;

class Tokenizer
{
public:
	typedef std::vector<char const*> StorageType;

	typedef StorageType::size_type size_type;

	typedef StorageType::const_iterator const_iterator;
	typedef StorageType::reference reference;
	typedef StorageType::const_reference const_reference;

public:
	Tokenizer(const std::string& src, char const sep, std::uint32_t vectorReserve = 0, bool keepEmptyStrings = true);
	~Tokenizer() { delete[] m_str; }

	const_iterator begin() const { return m_storage.begin(); }
	const_iterator end() const { return m_storage.end(); }

	size_type size() const { return m_storage.size(); }

	reference operator [] (size_type i) { return m_storage[i]; }
	const_reference operator [] (size_type i) const { return m_storage[i]; }


private:
	char* m_str;
	StorageType m_storage;
};

namespace Util
{
	bool QStringContainsOnlyDigits(const QString& string);
	std::uint32_t ConvertStringToUint32(const std::string& str);
	std::uint16_t ConvertStringToUint16(const std::string& str);
	std::uint32_t ConvertVariantToUInt32(const QVariant& variant);
	std::string ReverseString(const std::string& str);
	std::string ReverseStringParts(const std::string& str);
	std::string ConvertUint32ToString(const std::uint32_t& value);
	std::vector<std::uint16_t> splitStringToVectorUint16(std::string& str, std::string delim);

	template<typename... Args>
	std::string FormatMessageUtil(std::string_view message, Args&&... args)
	{
		return std::vformat(message, std::make_format_args(args...));
	}

}
