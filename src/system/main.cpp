
#include "WoWCurrencyChangeOrReplace.h"

#include <QtWidgets/QApplication>
#include <QFile>
#include <QDebug>
#include <QTextStream>
#include <QTimer>
#include <QTranslator>
#include <QApplication>

#include <iostream>

#include "Settings.h"
#include "SharedDefines.h"
#include "MySQLConnection.h"
#include "SettingsWindow.h"
#include "MainFrame.h"

static void customMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
	QFile file("qtlog.txt");
	file.open(QIODevice::WriteOnly | QIODevice::Append);
	QTextStream stream(&file);

	switch (type)
	{
		case QtDebugMsg:
			stream << "Debug: " << msg.toUtf8().constData() << '\n';
			break;
		case QtWarningMsg:
			stream << "Warning: " << msg.toUtf8().constData() << '\n';
			break;
		case QtCriticalMsg:
			stream << "Critical: " << msg.toUtf8().constData() << '\n';
			break;
		case QtFatalMsg:
			stream << "Fatal: " << msg.toUtf8().constData() << '\n';
			abort();
	}
}

int main(int argc, char *argv[])
{
	QApplication mApplication(argc, argv);

#ifdef _DEBUG
	qInstallMessageHandler(customMessageHandler);
#endif 

	std::shared_ptr<Settings> _settings = std::make_shared<Settings>(true);
	std::shared_ptr<MySQLConnection> _connection = std::make_shared<MySQLConnection>(false, _settings->GetIsSSHActive());
	std::shared_ptr<MainFrame> _mainFrame = std::make_shared<MainFrame>(_settings, _connection, mApplication);

	MainFrameSingelton::getInstance().setMainFramePointer(_mainFrame);

	return _mainFrame->StartFrameAndProgramm(argc, argv);
}
