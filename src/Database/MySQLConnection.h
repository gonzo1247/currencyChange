/*
Copyright 2023 - 2024 by Severin

*/

#pragma once

#include "Logger.h"
#include "DatabaseEnvFwd.h"
#include "Util.h"
#include "Settings.h"
#include "TimeMgr.h"

#include <jdbc/mysql_driver.h>
#include <jdbc/mysql_connection.h>
#include <jdbc/cppconn/prepared_statement.h>
#include <jdbc/cppconn/resultset.h>
#include <jdbc/cppconn/statement.h>
#include <libssh2.h>
#include <libssh2_sftp.h>

#include <string>

#include <Windows.h>
#include <DbgHelp.h>
#pragma comment(lib, "DbgHelp.lib")

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#define closeSocket closesocket
#pragma comment(lib, "ws2_32.lib")
#else
#include <arpa/inet.h>
#include <unistd.h>
#endif

/*
struct MySQLConnectionInfo
{
	explicit MySQLConnectionInfo(std::string const& infoString);

	std::string user;
	std::string password;
	std::string database;
	std::string host;
	std::string port_or_socket;
};*/

class Logger;
class Settings;
class TimeMgr;

class MySQLConnection
{
public:
//	MySQLConnection(MySQLConnectionInfo& connInfo);																				//! Constructor for synchronous connections
	MySQLConnection(sql::SQLString hostname, sql::SQLString username, sql::SQLString password, sql::SQLString databaseName);
	MySQLConnection(bool _isTest = false, bool isSSH = false);
	~MySQLConnection();

	sql::Connection* getMySQLConnection() { return _connection; }
	
	void ConfigTestMySQLConnection(sql::SQLString hostname, sql::SQLString port, sql::SQLString username, sql::SQLString password, sql::SQLString databaseName);
	void ConfigTestMySQLConnectionWithSSH(sql::SQLString hostname, sql::SQLString username, sql::SQLString password, sql::SQLString databaseName, std::string sshHostname, std::string sshUsername, std::string Port,
		std::string sshPassword);
	void CloseMySQLConnection();
	void ShowTestMessage(std::string mainText, std::string windowTitel);
	void ReadDataFromConfig();

	std::string GetEPreparedStatement(std::uint32_t index);
	std::string resolveDomainToIp(const std::string& domainName);

	void BindTimeEditToStatement(const QDateTimeEdit& dateTimeEdit, std::unique_ptr<sql::PreparedStatement>& stmt, int parameterIndex);
	void BindTimeEditToStatement(std::uint64_t time, std::unique_ptr<sql::PreparedStatement>& stmt, int parameterIndex);
	std::string EscapeString(const std::string& input);

	bool GetExistConnection() const { return _connectionExist; }

	void CheckConnectionPeriodically();
	void KeepConnectionAlive();

	template <typename... Args>
	void WriteLogMessage(std::string_view format_str, const Args&... args)
	{
		LOG_SQL(format_str, args...);
	}

	void WriteLogMessage(char* message);

	bool isIpAddress(const std::string& str);
	// SQL Executes
	template <typename PointerType>
	void ExecuteDelete(PointerType stmt)
	{
		try
		{
			// Execute the DELETE statement
			stmt->executeUpdate();
		}
		catch (const sql::SQLException& e)
		{
			WriteLogMessage("SQL-Exception Delete: {}", e.what());
		}
		catch (const std::exception& e)
		{
			WriteLogMessage("Exception Delete: {}", e.what());
		}
	}

	template <typename PointerType>
	bool ExecuteUpdateOrInsert(PointerType stmt)
	{
		try
		{
			stmt->executeUpdate();
			return true;
		}
		catch (const sql::SQLException& e)
		{
			WriteLogMessage("SQL-Exception ExecuteUpdateOrInsert: {}", e.what());
			WriteLogMessage("SQL-Status: {}", e.getSQLState());
		}
		catch (const std::exception& e)
		{
			WriteLogMessage("Exception ExecuteUpdateOrInsert: {}", e.what());
		}

		return false;
	}

	template <typename PointerType>
	std::unique_ptr<sql::ResultSet> ExecuteSelect(PointerType stmt)
	{
		try
		{
			// Executing the query
			if (stmt)
			{
				std::unique_ptr<sql::ResultSet> resultSet = std::unique_ptr<sql::ResultSet>(stmt->executeQuery());
				return resultSet;
			}
			else
			{
				WriteLogMessage("Error ExecuteSelect: Statement is nullptr");
				return nullptr;
			}

			return nullptr;

		}
		catch (const sql::SQLException& e)
		{
			WriteLogMessage("SQL-Exception ExecuteSelect: {}", e.what());
			return nullptr;
		}
		catch (const std::exception& e)
		{
			WriteLogMessage("Exception ExecuteSelect: {}", e.what());
			return nullptr;
		}

		return nullptr;
	}

	sql::PreparedStatement* GetPreparedStatement(std::string preparedStatement);
	sql::PreparedStatement* GetPreparedStatement(std::uint32_t index);
	std::unique_ptr<sql::PreparedStatement> GetUniquePreparedStatement(std::uint32_t index);


	void printStackTrace();

protected:
	sql::PreparedStatement* GetPreparedStatement(sql::Connection* con, std::uint32_t index);

	void OpenMySQLConnection();
	void PrepareStatement(std::uint32_t index, std::string const& sql);
	void DoPrepareStatements();

	void ShutDownSSHConnection();
	void ShutDownSSHConnection(LIBSSH2_SESSION* sshSession, LIBSSH2_CHANNEL* sshChannel, int tunnelSock, sql::Connection* connection = nullptr);

	void TestNewSSH();
	void EstablishMySQLConnection(int tunnelSock);
	void HandleException(const std::exception& e);
	void HandleUnknownException();
	int CreateSocket();
	void ConnectToSSHServer(int sock);
	void PerformSSHHandshakeAndAuthentication(LIBSSH2_SESSION* sshSession, int sock);
	void SetConnectionExist(bool exist) { _connectionExist = exist; }

private:
	sql::SQLString _hostname;
	sql::SQLString _port;
	sql::SQLString _username;
	sql::SQLString _password;
	sql::SQLString _databaseName;

	std::string _sshHostname;
	std::string _sshUsername;
	std::string _sshPassword;
	std::string _sshPort;
	std::string _sshMySQLHostname;
	std::string _sshMySQLPort;
	std::string _sshProgramPath;

	const int localPort = 3307;  // Local port for tunneling

	sql::Connection* _connection;

	std::vector<std::pair<std::uint32_t, std::string>> _prepareStatementHolder;

	std::shared_ptr<Logger> _log;
	std::shared_ptr<Settings> _settings;
	std::unique_ptr<TimeMgr> _timeMgr;

	LIBSSH2_SESSION* sshSession_;
	LIBSSH2_CHANNEL* sshChannel_;
	int tunnelSock_;  // Local socket for tunneling

	int localPort_;  // Local port for tunneling

	bool _isTestConnection;
	bool _connectionExist;
	bool _isSSHConnection;

	FILE* pipe_ = nullptr;
	std::thread monitorThread_;
};


enum DatabaseStatements : std::uint32_t
{
	/*  Naming standard for defines:
		{DB}_{SEL/INS/UPD/DEL/REP}_{Summary of data changed}
		When updating more than one field, consider looking at the calling function
		name for a suiting suffix.
	*/

	DB_CHARS_SELECT_CURRENCY,

	MAX_LOGINDATABASE_STATEMENTS,
};
