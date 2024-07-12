/*
Copyright 2023 - 2024 by Severin
and TrinityCore
*/
#pragma once

#include "MySQLConnection.h"

#include <QDateTimeEdit>
#include <QMessageBox>

#include <string>
#include <chrono>
#include <thread>
#include <stdexcept>
#include <sstream>
#include <cstdlib>
#include <cstdio>
#include <memory>
#include <locale>
#include <codecvt>

#include "MainFrame.h"

// Ensure linking against Ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

/*
MySQLConnectionInfo::MySQLConnectionInfo(std::string const& infoString)
{
	Tokenizer tokens(infoString, ';');
	if (tokens.size() != 5)
		return;

	std::uint8_t i = 0;
	host.assign(tokens[i++]);
	port_or_socket.assign(tokens[i++]);
	user.assign(tokens[i++]);
	password.assign(tokens[i++]);
	database.assign(tokens[i++]);
}*/

MySQLConnection::MySQLConnection(sql::SQLString hostname, sql::SQLString username, sql::SQLString password, sql::SQLString databaseName) :
	_hostname(hostname), _username(username), _password(password), _databaseName(databaseName), tunnelSock_(0)
{
	_log = std::make_shared<Logger>(LoggerTypes::LOG_SQL);
	DoPrepareStatements();
	_settings = std::make_shared<Settings>();
	_timeMgr = std::make_unique<TimeMgr>();
}

/*
MySQLConnection::MySQLConnection(MySQLConnectionInfo& connInfo) :
	m_reconnecting(false),
	m_prepareError(false),
//	m_queue(nullptr),
	m_Mysql(nullptr),
	m_connectionInfo(connInfo) 
{

}*/

MySQLConnection::MySQLConnection(bool _isTest, bool isSSH) : _isTestConnection(_isTest), tunnelSock_(0), _isSSHConnection(isSSH), _connection(nullptr)
{
	_log = std::make_shared<Logger>(LoggerTypes::LOG_SQL);
	DoPrepareStatements();
	_settings = std::make_shared<Settings>();
	ReadDataFromConfig();

	localPort_ = _settings->GetSSHLocalPort();

	if (!_isTest)
		if (!isSSH)
			OpenMySQLConnection();
		else
			TestNewSSH(); // tunnelSock_
}

MySQLConnection::~MySQLConnection()
{
	if (!_isTestConnection)
		CloseMySQLConnection();

	ShutDownSSHConnection();
	delete pipe_;

	if (monitorThread_.joinable())
		monitorThread_.join();
}

void MySQLConnection::OpenMySQLConnection()
{
	try
	{
		sql::mysql::MySQL_Driver* driver;
		driver = sql::mysql::get_mysql_driver_instance();

		_connection = driver->connect(_hostname + ":" + _port, _username, _password);

		// Select the database
		_connection->setSchema(_databaseName);

		SetConnectionExist(true);
		LOG_SQL("Connection to the MySQL database established.");
	}
	catch (sql::SQLException& e)
	{
		std::string errorMessage = "MySQL-Error: ";
		errorMessage += e.what();
		LOG_SQL(errorMessage);
		SetConnectionExist(false);
	}
	catch (std::exception& e)
	{
		std::string errorMessage = "MySQL - Error: : ";
		errorMessage += e.what();
		LOG_SQL(errorMessage);
		SetConnectionExist(false);
	}
	catch (...)
	{
		std::string errorMessage = "MySQL - Error: OpenMySQLConnection - Unknown exception caught.";
		LOG_SQL(errorMessage);
		SetConnectionExist(false);
	}

}

void MySQLConnection::ConfigTestMySQLConnection(sql::SQLString hostname, sql::SQLString port, sql::SQLString username, sql::SQLString password, sql::SQLString databaseName)
{
	try
	{
		sql::mysql::MySQL_Driver* driver;
		sql::Connection* con;

		driver = sql::mysql::get_mysql_driver_instance();
		con = driver->connect(hostname + ":" + port, username, password);

		// Select the database
		con->setSchema(databaseName);		

		std::string text = "Verbindung zur Datenbank erfolgreich";
		std::string windowText = "MySQL Verbindungs Test";
		ShowTestMessage(text, windowText);

		delete con;
	}
	catch (sql::SQLException& e)
	{
		std::string errorMessage = "MySQL-Error: ";
		errorMessage += e.what();
		LOG_SQL(errorMessage);

		std::string text = "Verbindung zur Datenbank war nicht erfolgreich";
		std::string windowText = "MySQL Verbindungs Test";
		ShowTestMessage(text, windowText);
	}
	catch (std::exception& e)
	{
		std::string errorMessage = "Exception ExecuteUpdateOrInsert: ";
		errorMessage += e.what();
		LOG_SQL(errorMessage);
	}
}

void MySQLConnection::ConfigTestMySQLConnectionWithSSH(sql::SQLString hostname, sql::SQLString username, sql::SQLString password, sql::SQLString databaseName, std::string sshHostname, std::string sshUsername, std::string sshPort, std::string sshPassword)
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		// Error handling for the initialization of Winsock
		return;
	}

	int sock;

	LIBSSH2_SESSION* sshSession;
	LIBSSH2_CHANNEL* sshChannel;
	int tunnelSock;  // Local socket for tunneling

	sql::Connection* connection;
	// Create a new SSH session
	sshSession = libssh2_session_init();
	if (!sshSession)
		throw std::runtime_error("Failed to initialize libssh2 session");

	// Set options for the SSH session
	libssh2_session_set_timeout(sshSession, 30000);

#ifdef _WIN32
	sock = socket(AF_INET, SOCK_STREAM, 0);
#else
	sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
#endif
	//	sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == -1)
		throw std::runtime_error("Failed to create socket");

	struct sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(std::stoul(sshPort));

	std::string hostnameIpAdress = resolveDomainToIp(sshHostname);

	// The `inet_pton` function is used to convert the IPv4 address.
	// Note that this expects an IPv4 address in dotted-decimal notation.
	if (inet_pton(AF_INET, hostnameIpAdress.c_str(), &(serverAddress.sin_addr)) <= 0)
		throw std::runtime_error("Invalid IP address or hostname");

	//Establish connection
	if (connect(sock, reinterpret_cast<struct sockaddr*>(&serverAddress), sizeof(serverAddress)) != 0)
		throw std::runtime_error("Failed to connect to SSH server");

	// Perform SSH handshake and authentication
	if (libssh2_session_handshake(sshSession, sock))
	{
		char* errorMsg;
		int errorMsgLen;
		libssh2_session_last_error(sshSession, &errorMsg, &errorMsgLen, 0);
		throw std::runtime_error("SSH handshake failed");
	}

	if (libssh2_userauth_password(sshSession, sshUsername.c_str(), sshPassword.c_str())) {
		throw std::runtime_error("SSH authentication failed");
	}

	// Start a new SSH channel
	sshChannel = libssh2_channel_open_session(sshSession);
	if (!sshChannel) {
		throw std::runtime_error("Failed to open channel");
	}

	// Set up local port forwarding
	libssh2_channel_forward_listen_ex(sshSession, "127.0.0.1", localPort_, &tunnelSock, 1);

	try {
		// Open SSH connection

		// Establish MySQL connection through the SSH tunnel
		sql::mysql::MySQL_Driver* mysqlDriver;
		mysqlDriver = sql::mysql::get_mysql_driver_instance();
		hostname += ":";
		hostname += sql::SQLString(std::to_string(localPort_));

		connection = mysqlDriver->connect(hostname, username, password);

		// Select the database
		connection->setSchema(_databaseName);

		std::string text = "Verbindung zur Datenbank erfolgreich";
		std::string windowText = "MySQL Verbindungs Test";
		ShowTestMessage(text, windowText);

		ShutDownSSHConnection(sshSession, sshChannel, tunnelSock, connection);
	}
	catch (sql::SQLException& e)
	{
		std::string errorMessage = "MySQL-Error: ";
		errorMessage += e.what();
		LOG_SQL(errorMessage);
	}
	catch (const std::exception& e)
	{
		std::string errorMessage = "MySQL-Error: ";
		errorMessage += e.what();
		LOG_SQL(errorMessage);

		std::string text = "Verbindung zur Datenbank war nicht erfolgreich";
		std::string windowText = "MySQL Verbindungs Test";
		ShowTestMessage(text, windowText);
		ShutDownSSHConnection(sshSession, sshChannel, tunnelSock);
		WSACleanup();
	}

	WSACleanup();
}

void MySQLConnection::CloseMySQLConnection()
{
	_connection->close();
	delete _connection;
	_connection = nullptr;
}

void MySQLConnection::ShowTestMessage(std::string mainText, std::string windowTitel)
{
	QMessageBox msgBox;
	msgBox.setText(QString::fromStdString(mainText));
	msgBox.setStandardButtons(QMessageBox::Ok);
	msgBox.setDefaultButton(QMessageBox::Ok);
	msgBox.setWindowTitle(QString::fromStdString(windowTitel));
	msgBox.setWindowIcon(QIcon(":/icons/icon/mysql/database-mysql.svg"));

	// Show the MessageBox and wait for the user's response
	int ret = msgBox.exec();

	if (ret == QMessageBox::Ok) {
		msgBox.close();
	}
}

void MySQLConnection::ReadDataFromConfig()
{
	_hostname = _settings->GetMySQLHostname();
	_port = _settings->GetMySQLPort();
	_username = _settings->GetMySQLUsername();
	_password = _settings->GetMySQLPassword();
	_databaseName = _settings->GetMySQLDatabaseName();

	if (_settings->GetIsSSHActive())
	{
		_sshHostname = _settings->GetSSHHostName();
		_sshPort = _settings->GetSSHPort();
		_sshUsername = _settings->GetSSHUsername();
		_sshPassword = _settings->GetSSHPassword();
		_sshMySQLHostname = _settings->GetMySQLHostname();
		_sshMySQLPort = _settings->GetMySQLPort();
		_sshProgramPath = _settings->GetSSHProgrammPath();
	}
}

std::string MySQLConnection::GetEPreparedStatement(std::uint32_t index)
{
	for (const auto& data : _prepareStatementHolder)
		if (data.first == index)
		{
			return data.second;
			break;
		}

	std::string invalideString = "INVALIDE";
	return invalideString;
}

std::string MySQLConnection::resolveDomainToIp(const std::string& domainName)
{
	if (isIpAddress(domainName))
	{
		std::string errorMessage = "MySQL-Info: ";
		errorMessage += "Provided address is already an IP address: " + domainName + "\n";
		LOG_SQL(errorMessage);
		return domainName;
	}

	sockaddr_in sa;
	sa.sin_family = AF_INET;
	sa.sin_port = htons(0);  // Port is not relevant for IP address resolution

#ifdef _WIN32
	// For Windows
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		std::string errorMessage = "MySQL-Error: ";
		errorMessage += "Error when initializing Winsock\n";
		LOG_SQL(errorMessage);
		return "";
	}
#endif

	// For both Windows and Unix-like systems
	struct addrinfo hints = { 0 };
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	struct addrinfo* result = nullptr;
	if (getaddrinfo(domainName.c_str(), nullptr, &hints, &result) != 0)
	{
		std::string errorMessage = "MySQL-Error: ";
		errorMessage += "Error when resolving the IP address from the domain name\n";
		LOG_SQL(errorMessage);

#ifdef _WIN32
		WSACleanup();
#endif
		return "";
	}

	char ipStr[INET_ADDRSTRLEN] = { 0 };
	if (result != nullptr)
	{
		sockaddr_in* sockaddrIpv4 = reinterpret_cast<sockaddr_in*>(result->ai_addr);
		inet_ntop(AF_INET, &(sockaddrIpv4->sin_addr), ipStr, INET_ADDRSTRLEN);
		freeaddrinfo(result);
	}
	else
	{
		std::string errorMessage = "MySQL-Error: ";
		errorMessage += "No addresses found for domain: " + domainName + "\n";
		LOG_SQL(errorMessage);

#ifdef _WIN32
		WSACleanup();
#endif
		return "";
	}

#ifdef _WIN32
	WSACleanup();
#endif

	return std::string(ipStr);
}

void MySQLConnection::BindTimeEditToStatement(std::uint64_t time, std::unique_ptr<sql::PreparedStatement>& stmt, int parameterIndex)
{
	stmt->setUInt(parameterIndex, time);
}

void MySQLConnection::BindTimeEditToStatement(const QDateTimeEdit& dateTimeEdit, std::unique_ptr<sql::PreparedStatement>& stmt, int parameterIndex)
{
	if (!dateTimeEdit.dateTime().isValid())
	{
		return;
	}

	stmt->setUInt(parameterIndex, _timeMgr->GetDTETimeAsUint(dateTimeEdit));
}

std::string MySQLConnection::EscapeString(const std::string& input)
{
	std::ostringstream escaped;
	escaped << std::setprecision(2) << std::fixed;

	for (char c : input)
	{
		switch (c)
		{
		case '\'':
			escaped << "''";  // Double single quotation marks
			break;
			// Further cases for special characters can be added here
		default:
			escaped << c;
		}
	}

	return escaped.str();
}

void MySQLConnection::WriteLogMessage(char* message)
{
	LOG_SQL(message);
}

bool MySQLConnection::isIpAddress(const std::string& str)
{
	struct sockaddr_in sa;
	return inet_pton(AF_INET, str.c_str(), &(sa.sin_addr)) != 0;
}

void MySQLConnection::PrepareStatement(std::uint32_t index, std::string const& sql)
{
	if (sql.length() >= 5)
		_prepareStatementHolder.push_back({ index, sql });
	else
		LOG_SQL("MySQL-Error: SQL PreparedStatement String index {} is to short <= 5 : {}", index, sql);
}

sql::PreparedStatement* MySQLConnection::GetPreparedStatement(std::string preparedStatement)
{
	sql::PreparedStatement* result;
	try 
	{
		result = getMySQLConnection()->prepareStatement(preparedStatement);
	}
	catch (const sql::SQLException& e)
	{
		LOG_SQL("MySQL-Error GetPreparedStatement: {}", e.what());
		LOG_SQL("MySQL-Error GetPreparedStatement: {}", preparedStatement);
		return nullptr;
	}
	catch (const std::exception& e)
	{
		LOG_SQL("Exception GetPreparedStatement: {}", e.what());
		return nullptr;
	}
	catch (...)
	{
		LOG_SQL("Unknown Error in MySQLConnection::GetPreparedStatement(std::string preparedStatement)");
		return nullptr;
	}

	return result;
}

sql::PreparedStatement* MySQLConnection::GetPreparedStatement(sql::Connection* con, std::uint32_t index)
{
	sql::PreparedStatement* result = nullptr;
	for (const auto& data : _prepareStatementHolder)
	{
		if (data.first == index)
		{
			result = con->prepareStatement(data.second.c_str());
			break; // cancel for, index was found
		}
	}

	return result;
}

sql::PreparedStatement* MySQLConnection::GetPreparedStatement(std::uint32_t index)
{
	std::string preparedStatement = GetEPreparedStatement(index);
	sql::PreparedStatement* result;
	try
	{
		result = getMySQLConnection()->prepareStatement(preparedStatement);
	}
	catch (const sql::SQLException& e)
	{
		LOG_SQL("MySQL-Error GetPreparedStatement: {}", e.what());
		LOG_SQL("MySQL-Error GetPreparedStatement: {}", preparedStatement);
		return nullptr;
	}
	catch (const std::exception& e)
	{
		LOG_SQL("Exception GetPreparedStatement: {}", e.what());
		return nullptr;
	}

	return result;
}

std::unique_ptr<sql::PreparedStatement> MySQLConnection::GetUniquePreparedStatement(std::uint32_t index)
{
	std::string preparedStatement = GetEPreparedStatement(index);
	std::unique_ptr<sql::PreparedStatement> result;
	try
	{
		result = std::unique_ptr<sql::PreparedStatement>(getMySQLConnection()->prepareStatement(preparedStatement));
	}
	catch (const sql::SQLException& e)
	{
		LOG_SQL("MySQL-Error GetPreparedStatement: {}", e.what());
		LOG_SQL("MySQL-Error GetPreparedStatement: {}", preparedStatement);
		return nullptr;
	}
	catch (const std::exception& e)
	{
		LOG_SQL("Exception GetPreparedStatement: {}", e.what());
		return nullptr;
	}

	return result;
}

void MySQLConnection::printStackTrace()
{
	const int max_frames = 64;
	void* addrlist[max_frames + 1];
	HANDLE process = GetCurrentProcess();

	// Capture the current stack back trace
	unsigned short frames = CaptureStackBackTrace(0, max_frames, addrlist, NULL);

	if (frames == 0) 
	{
		std::cerr << "Error: Backtrace empty." << std::endl;
		LOG_SQL("Error: Backtrace empty.");
		return;
	}

	// Obtain symbols for each frame
	SYMBOL_INFO* symbol = (SYMBOL_INFO*)calloc(sizeof(SYMBOL_INFO) + 256 * sizeof(char), 1);

	if (symbol != nullptr)
	{
		symbol->MaxNameLen = 255;
		symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
	}
	else
		return;

	for (int i = 0; i < frames; i++) {
		SymFromAddr(process, (DWORD64)(addrlist[i]), 0, symbol);

		std::cerr << i << ": " << symbol->Name << " - 0x" << std::hex << symbol->Address << std::dec << std::endl;
		LOG_SQL("PrintStackTrace - i: {} ; {} - 0x {}", i, symbol->Name, symbol->Address);
	}

	free(symbol);
}

void MySQLConnection::DoPrepareStatements()
{
	PrepareStatement(DB_CHARS_SELECT_CURRENCY, "SELECT CharacterGuid, Currency, Quantity FROM character_currency WHERE Currency = ? OR Currency = ? OR Currency = ? OR Currency =  ?");
}

void MySQLConnection::ShutDownSSHConnection()
{
	if (_connection)
	{
		_connection->close();
		delete _connection;
		_connection = nullptr;
	}

	if (sshChannel_)
	{
		if (libssh2_channel_free(sshChannel_) == 0)
			sshChannel_ = nullptr;
	}

	if (sshSession_)
	{
		if (libssh2_session_disconnect(sshSession_, "Bye bye, thanks for playing") == 0 &&
		libssh2_session_free(sshSession_) == 0)
			sshSession_ = nullptr;
	}

	if (tunnelSock_ != -1)
	{
		closeSocket(tunnelSock_);
		tunnelSock_ = 1;
	}
} 

void MySQLConnection::ShutDownSSHConnection(LIBSSH2_SESSION* sshSession, LIBSSH2_CHANNEL* sshChannel, int tunnelSock, sql::Connection* connection)
{
	if (connection != nullptr)
		if (connection && !connection->isClosed())
		{
			connection->close();
			delete connection;
			connection = nullptr;
		}

	if (sshChannel)
	{
		libssh2_channel_free(sshChannel);
		sshChannel = nullptr;
	}

	if (sshSession)
	{
		libssh2_session_disconnect(sshSession, "Bye bye, thanks for playing");
		libssh2_session_free(sshSession);
		sshSession = nullptr;
	}

	if (tunnelSock != -1)
	{
		closeSocket(tunnelSock);
		tunnelSock = -1;
	}
}

void MySQLConnection::TestNewSSH()
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		_log->WriteLogFile(LoggerTypes::LOG_SQL, "WSAStartup Failed in TestNewSSH");
		return;
	}

	try
	{
		// Initialize SSH session
		sshSession_ = libssh2_session_init();
		if (!sshSession_)
			throw std::runtime_error("Failed to initialize libssh2 session");

		libssh2_session_set_timeout(sshSession_, 30000);

		int sock = CreateSocket();
		ConnectToSSHServer(sock);

		// Perform SSH handshake and authentication
		PerformSSHHandshakeAndAuthentication(sshSession_, sock);

		// Start a new SSH channel
		LIBSSH2_CHANNEL* sshChannel = libssh2_channel_open_session(sshSession_);
		if (!sshChannel)
			throw std::runtime_error("Failed to open channel");

		int tunnelSock;
		libssh2_channel_forward_listen_ex(sshSession_, "127.0.0.1", localPort_, &tunnelSock, 1);

		EstablishMySQLConnection(tunnelSock);

		// libssh2_channel_free(sshChannel);
		// ShutDownSSHConnection();
	}
	catch (const std::exception& e)
	{
		HandleException(e);
	}
	catch (...)
	{
		HandleUnknownException();
	}

	WSACleanup();
}

void MySQLConnection::EstablishMySQLConnection(int tunnelSock)
{
	_connection = nullptr;
	sql::mysql::MySQL_Driver* mysqlDriver = sql::mysql::get_mysql_driver_instance();
	_hostname += ":" + sql::SQLString(std::to_string(tunnelSock));

	_connection = mysqlDriver->connect(_hostname, _username, _password);
	_connection->setSchema(_databaseName);
}

void MySQLConnection::HandleException(const std::exception& e)
{
	std::string errorMessage = "MySQL-Error: " + std::string(e.what());
	_log->WriteLogFile(LoggerTypes::LOG_SQL, errorMessage);
	ShutDownSSHConnection();
	WSACleanup();
}

void MySQLConnection::HandleUnknownException()
{
	std::string errorMessage = "MySQL - Error: OpenMySQLConnectionWithSSH - Unknown exception caught.";
	_log->WriteLogFile(LoggerTypes::LOG_SQL, errorMessage);
	ShutDownSSHConnection();
	WSACleanup();
}

int MySQLConnection::CreateSocket()
{
	int sock;
#ifdef _WIN32
	sock = socket(AF_INET, SOCK_STREAM, 0);
#else
	sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
#endif
	if (sock == -1)
		throw std::runtime_error("Failed to create socket");

	return sock;
}

void MySQLConnection::ConnectToSSHServer(int sock)
{
	struct sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(std::stoul(_sshPort));

	std::string hostnameIpAdress = resolveDomainToIp(_sshHostname);

	if (inet_pton(AF_INET, hostnameIpAdress.c_str(), &(serverAddress.sin_addr)) <= 0)
		throw std::runtime_error("Invalid IP address or hostname");

	if (connect(sock, reinterpret_cast<struct sockaddr*>(&serverAddress), sizeof(serverAddress)) != 0)
		throw std::runtime_error("Failed to connect to SSH server");
}

void MySQLConnection::PerformSSHHandshakeAndAuthentication(LIBSSH2_SESSION* sshSession, int sock)
{
	if (libssh2_session_handshake(sshSession, sock))
	{
		char* errorMsg;
		int errorMsgLen;
		libssh2_session_last_error(sshSession, &errorMsg, &errorMsgLen, 0);
		throw std::runtime_error("SSH handshake failed");
	}

	if (libssh2_userauth_password(sshSession, _sshUsername.c_str(), _sshPassword.c_str()))
	{
		_log->WriteLogFile(LoggerTypes::LOG_SQL, "SSH Authentication failed, password or Username wrong!");
		throw std::runtime_error("SSH Authentication failed");
	}
}

void MySQLConnection::CheckConnectionPeriodically()
{
	if (_connection == nullptr || _connection->isClosed())
	{
		LOG_SQL("Lost connection. Reconnecting...");
		if (!_isSSHConnection)
			OpenMySQLConnection();
		else
			TestNewSSH();

		if (_connection == nullptr || _connection->isClosed())
			GetMainFrameClass()->AbortProgrammOnMySQLConnectionError();
	}
}

void MySQLConnection::KeepConnectionAlive()
{
	if (_connection != nullptr && !_connection->isClosed())
	{
		sql::Statement* stmt = nullptr;
		sql::ResultSet* res = nullptr;
		try
		{
			stmt = _connection->createStatement();
			stmt->execute("SELECT 1");
			res = stmt->getResultSet();
			while (stmt->getMoreResults()) {}
			LOG_SQL("Connection is alive");
		}
		catch (sql::SQLException& e)
		{
			std::string errorMessage = "MySQL-Error: ";
			errorMessage += e.what();
			LOG_SQL(errorMessage);
			GetMainFrameClass()->AbortProgrammOnMySQLConnectionError();
		}
		catch (std::exception& e)
		{
			std::string errorMessage = "MySQL - Error: : ";
			errorMessage += e.what();
			LOG_SQL(errorMessage);
			GetMainFrameClass()->AbortProgrammOnMySQLConnectionError();
		}
		catch (...)
		{
			std::string errorMessage = "MySQL - Error: KeepConnectionAlive - Unknown exception caught.";
			LOG_SQL(errorMessage);
			GetMainFrameClass()->AbortProgrammOnMySQLConnectionError();
		}

		if (res != nullptr)
			delete res;

		if (stmt != nullptr)
			delete stmt;
	}
}
