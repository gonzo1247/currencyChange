#pragma once

#include <QMainWindow>
#include <QApplication>

#include "Crypto.h"
#include "MySQLConnection.h"
#include "Settings.h"
#include "ui_SettingsWindow.h"

QT_BEGIN_NAMESPACE
namespace Ui { class SettingsWindowClass; };
QT_END_NAMESPACE

class SettingsWindow : public QMainWindow
{
	Q_OBJECT

public:
	SettingsWindow(std::shared_ptr<MySQLConnection> connection, std::shared_ptr<Settings> settings, QWidget* parent = nullptr);
	SettingsWindow(std::shared_ptr<Settings> settings, QWidget* parent = nullptr);
	~SettingsWindow();

private:
	void LoadSettingsData();
	void SelectLogPath();
	void SaveSettings();

	void CrypthPassword(std::string password, bool isMySQLP /*= true*/);

	SuccessfullCodes mergeValues(const std::vector<SuccessfullCodes>& values);

	std::string _newLogPath;

	bool _overrideVisibility;

	std::string SavePathString		= "SavePath";
	std::string styleThema			= "StyleThema";
	std::string firstStartString	= "FirstStart";
	std::string MYSQLHostName		= "MYSQLHostname";
	std::string MYSQLPort			= "MYSQLPort";
	std::string MYSQLUsername		= "MYSQLUsername";
	std::string MYSQLPassword		= "MYSQLPassword";
	std::string MYSQLDatabaseName	= "MYSQLDatabaseName";
	std::string MYSQLIsActive		= "MYSQLIsActive";
	std::string MYSQLPasswordLength = "MySQLPasswordLenght";
	std::string SSHHostName			= "SSH_Hostname";
	std::string SSHPort				= "SSH_Port";
	std::string SSHUsername			= "SSH_Username";
	std::string SSHPassword			= "SSH_Password";
	std::string SSHPasswordLength	= "SSH_PasswordLength";
	std::string SSHProgrammPath		= "SSH_ProgrammPath";
	std::string SSHIsActive			= "SSH_IsActive";
	std::string SSHLocalPort		= "SSHLocalPort";
	std::string SoundVolume			= "SoundVolume";
	std::string SoundEnabled		= "SoundEnabled";
	std::string CurrentLanguage		= "Language";
	std::string isFullscreen		= "FullScreen";
	std::string PersonalizedStyle = "PersonalStyle";
	std::string PersonalizedLanguage = "PersonalLanguage";

	std::unique_ptr<Ui::SettingsWindowClass> ui;
	std::shared_ptr<MySQLConnection> _connection;
	std::shared_ptr<Settings> _settings;
	std::unique_ptr<Crypto> _crypto;

	std::uint8_t editUserPage = 0;
	std::uint8_t newUserPage = 1;
	std::uint8_t MoreSettingsPage = 2;

private slots:
	void closeSettingsWindow();
	void onPushSelectLogPath();
	void onPushSaveSettings();

protected:
	void showEvent(QShowEvent* event) override;
	void changeEvent(QEvent* event) override;
};
