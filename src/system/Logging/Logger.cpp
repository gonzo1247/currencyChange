/*
Copyright 2023 - 2024 by Severin

*/

#include <format>

#include "Logger.h"

Logger::Logger(LoggerTypes types) : _loggerTypes(types), _settings(std::make_unique<Settings>()), _logFile()
{
	//_errorHandler = std::make_unique<UIErrorHandler>();

	OpenOrCreateLogFile();
}

Logger::Logger() : _loggerTypes(LoggerTypes::LOG_MISC), _logFile()
{
	_settings = std::make_unique<Settings>();
}

Logger::~Logger()
{
	CloseLogFile();
}

Logger* Logger::instance()
{
	static Logger instance;
	return &instance;
}

void Logger::OpenOrCreateLogFile()
{
	std::string currentPathStr = _settings->GetLogFilePath();

	currentPathStr = ensurePath(currentPathStr);

	// Get the current time
	std::time_t now = std::time(nullptr);

	// Create a tm struct to hold the local time
	std::tm localTime;

	// Convert time_t to tm as local time using localtime_s
	localtime_s(&localTime, &now);

	// Create a file name with the current date
	std::ostringstream filename;
	filename << currentPathStr;
	filename << "/log_";
	//qDebug() << "Start with filename: " << filename.str();
	if (_loggerTypes == LoggerTypes::LOG_SQL)
		filename << "sql_";
	if (_loggerTypes == LoggerTypes::LOG_MISC)
		filename << "misc_";

	filename << std::put_time(&localTime, "%Y-%m-%d") << ".txt";

	// open or create the log file
	_logFile.open(filename.str(), std::ios::app);

	if (!_logFile.is_open())
	{
	//	_errorHandler->SendWarningDialog(ErrorCodes::ERROR_LOG_FILE_NOT_OPEN_OR_CREATED);
		_logFile.close();
	}
}

std::unique_ptr<std::ofstream> Logger::OpenOrCreateFile(LoggerTypes logger)
{
	std::string currentPathStr = _settings->GetLogFilePath();

	// Get the current time
	std::time_t now = std::time(nullptr);

	// Create a tm struct to hold the local time
	std::tm localTime;

	// Convert time_t to tm as local time using localtime_s
	localtime_s(&localTime, &now);

	currentPathStr = ensurePath(currentPathStr);

	// Create a file name with the current date
	std::ostringstream filename;
	filename << currentPathStr;
	filename << "/log_";
	if (logger == LoggerTypes::LOG_SQL)
		filename << "sql_";
	if (logger == LoggerTypes::LOG_MISC)
		filename << "misc_";

	filename << std::put_time(&localTime, "%Y-%m-%d") << ".txt";

	auto _logTypeFile = std::make_unique<std::ofstream>();

	// open or create the log file
	_logTypeFile->open(filename.str(), std::ios::app);

	if (!_logTypeFile->is_open())
	{
		//_errorHandler->SendWarningDialog(ErrorCodes::ERROR_LOG_FILE_NOT_OPEN_OR_CREATED);
		return nullptr;
	}

	return _logTypeFile;
}

void Logger::WriteLogFile(std::string_view format_str)
{
	if (!_logFile.is_open())
	{
		//_errorHandler->SendWarningDialog(ErrorCodes::ERROR_LOG_FILE_NOT_OPEN_OR_CREATED);
		return;
	}

	// Get the current time
	std::time_t now = std::time(nullptr);

	// Create a tm struct to hold the local time
	std::tm localTime;

	// Convert time_t to tm as local time using localtime_s
	localtime_s(&localTime, &now);

	std::stringstream ss;
	ss << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S");
	std::string formattedLogMessage = std::format("{}: {}", ss.str(), format_str);

	_logFile << formattedLogMessage << std::endl;
	_logFile.flush();
}

void Logger::WriteLogFile(LoggerTypes type, std::string_view format_str, std::format_args args)
{
	std::unique_ptr<std::ofstream> writeLogFile = OpenOrCreateFile(type);
	if (!writeLogFile)
	{
		//_errorHandler->SendWarningDialog(ErrorCodes::ERROR_LOG_FILE_NOT_OPEN_OR_CREATED);
		return;
	}

	// Get the current time
	std::time_t now = std::time(nullptr);

	// Create a tm struct to hold the local time
	std::tm localTime;

	// Convert time_t to tm as local time using localtime_s
	localtime_s(&localTime, &now);

	try
	{
		std::stringstream ss;
		ss << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S");
		std::string formattedLogMessage = std::vformat(format_str, args);
		(*writeLogFile) << ss.str() << ": " << formattedLogMessage << "\n";
	}
	catch (const std::exception& e)
	{
		// Handle exception, e.g., log the error
		std::cerr << "Exception in vlog_error: " << e.what() << std::endl;
		return;
	}	
}

void Logger::WriteLogFile(LoggerTypes const level, std::string_view format_str)
{
	std::unique_ptr<std::ofstream> writeLogFile = OpenOrCreateFile(level);
	if (!writeLogFile)
	{
	//	_errorHandler->SendWarningDialog(ErrorCodes::ERROR_LOG_FILE_NOT_OPEN_OR_CREATED);
		return;
	}

	// Get the current time
	std::time_t now = std::time(nullptr);

	// Create a tm struct to hold the local time
	std::tm localTime;

	// Convert time_t to tm as local time using localtime_s
	localtime_s(&localTime, &now);

	std::stringstream ss;
	ss << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S");
	std::string formattedLogMessage = std::format("{}: {}", ss.str(), format_str);

	(*writeLogFile) << formattedLogMessage << "\n";
}

void Logger::CloseLogFile()
{
	_logFile.close();
}

std::string Logger::ensurePath(const std::string& desiredPath)
{
	// Check whether the desired path exists
	if (!fs::exists(desiredPath))
	{
		// If not, access the program path
		fs::path programPath = fs::current_path();

		// Create additional directory if required
		fs::path fullPath = programPath / fs::path(desiredPath);
		fs::create_directories(fullPath);
		return fullPath.string();
	}

	return fs::absolute(desiredPath).string();
}

void Logger::OutMessageImpl(LoggerTypes level, std::string message)
{
	// WriteLogFile(level, format_str, std::make_format_args(args...));
}
