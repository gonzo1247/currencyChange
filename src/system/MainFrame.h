#pragma once

#include <QApplication>
#include <QTranslator>
#include <QObject>
#include <QSplashScreen>
#include <QTimer>

#include "Settings.h"
#include "MySQLConnection.h"
#include "StyleManager.h"

class MainFrame : public QObject
{
	Q_OBJECT
public:
	MainFrame(std::shared_ptr<Settings> settings, std::shared_ptr<MySQLConnection> connection, QApplication& application);
	~MainFrame();

	int StartFrameAndProgramm(int argc, char* argv[]);

	void AbortProgrammOnMySQLConnectionError();
private:
	void LoadTranslation();
	void LoadStyle();
	void CheckMySQLConnection();
	void SendPing();
	void LoadingScreen();
	std::unique_ptr<QSplashScreen> CreateSplashScreen();

	void setTableWidgetsStyle(const QString& style, Styles styleEnum);
	void UpdateUI();

	int OnlySettingsWindow();

	std::atomic<bool> _shouldExit;

	std::shared_ptr<Settings> _settings;
	std::shared_ptr<MySQLConnection> _connection;

	std::unique_ptr<StyleManager> _styleMgr;

	QApplication& _application;
	QTranslator* _translator;
	std::unique_ptr<QTimer> _loadingScreenTimer;
};

class MainFrameSingelton
{
public:
	static MainFrameSingelton& getInstance()
	{
		static MainFrameSingelton instance;
		return instance;
	}

	std::shared_ptr<MainFrame> getMainFrameSingeltonPointer() { return _mainFrame; }
	void setMainFramePointer(std::shared_ptr<MainFrame> mainFrame) { _mainFrame = mainFrame; }

private:
	MainFrameSingelton() { }

	std::shared_ptr<MainFrame> _mainFrame;

};

inline std::shared_ptr<MainFrame> GetMainFrameClass()
{
	return MainFrameSingelton::getInstance().getMainFrameSingeltonPointer();
}
