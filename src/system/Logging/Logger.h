/*
Copyright 2023 - 2024 by Severin

*/
#pragma once

#include "Settings.h"

#include <fstream>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <codecvt>
#include <string>
#include <format>


namespace fs = std::filesystem;

class Settings;

enum class LoggerTypes : std::uint32_t
{
	LOG_SQL						= 1,
	LOG_MISC					= 2
};

enum LogLevel : std::uint8_t
{
	LOG_LEVEL_SQL				= 1,
	LOG_LEVEL_MISC				= 2,
};


class Logger
{
public:
	Logger();
	Logger(LoggerTypes types);
	~Logger();

	Logger(Logger const&) = delete;
	Logger(Logger&&) = delete;
	Logger& operator=(Logger const&) = delete;
	Logger& operator=(Logger&&) = delete;

	static Logger* instance();

	void OpenOrCreateLogFile();
	std::unique_ptr<std::ofstream> OpenOrCreateFile(LoggerTypes logger);
	void WriteLogFile(LoggerTypes type, std::string_view format_str, std::format_args args);
	void WriteLogFile(std::string_view format_str);
	void WriteLogFile(LoggerTypes const level, std::string_view format_str);
	void CloseLogFile();
	std::string ensurePath(const std::string& desiredPath);

	template <typename... Args>
	typename std::enable_if<(sizeof...(Args) > 0), void>::type OutMessage(LoggerTypes const level, std::string_view format_str, const Args&... args)
	{
		WriteLogFile(level, std::vformat(format_str, std::make_format_args(args...)));
	}

	void OutMessage(LoggerTypes const level, std::string_view message)
	{
		WriteLogFile(level, message);
	}

	std::unique_ptr<Settings> _settings;

private:	
	//std::unique_ptr<UIErrorHandler> _errorHandler;

	std::ofstream _logFile;

	LoggerTypes _loggerTypes;

	void OutMessageImpl(LoggerTypes level, std::string message);
};


#define sLog Logger::instance()

#define TC_LOG_MESSAGE_BODY(level__, ...)         \
    do{                                        \
        sLog->OutMessage(level__, ##__VA_ARGS__);  \
    } while (0)

#define LOG_MISC(...) \
    TC_LOG_MESSAGE_BODY(LoggerTypes::LOG_MISC, __VA_ARGS__)

#define LOG_SQL(...) \
    TC_LOG_MESSAGE_BODY(LoggerTypes::LOG_SQL, __VA_ARGS__)
