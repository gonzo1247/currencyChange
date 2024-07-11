
#include <QFile>
#include <QScreen>
#include <QThread>
#include <QTableWidget>
#include <QList>
#include <QWidget>
#include <QMovie>
#include <QLabel>
#include <QProgressBar>
#include <QVBoxLayout>

#include "MainFrame.h"
#include "SettingsWindow.h"
#include "WoWCurrencyChangeOrReplace.h"


MainFrame::MainFrame(std::shared_ptr<Settings> settings, std::shared_ptr<MySQLConnection> connection, QApplication& application) : _settings(settings),
_connection(connection), _shouldExit(false), _application(application), _styleMgr(std::make_unique<StyleManager>()), _loadingScreenTimer(std::make_unique<QTimer>())
{
	_translator = nullptr;
}

MainFrame::~MainFrame()
{
	delete _translator;
}

int MainFrame::StartFrameAndProgramm(int /*argc*/, char* /*argv*/[])
{
	std::unique_ptr<QSplashScreen> splash = CreateSplashScreen();

	splash->show();
	QApplication::processEvents();

	LoadTranslation();
	LoadStyle();

	if (!_connection->GetExistConnection())
		return OnlySettingsWindow();

	std::thread connectionThread(std::bind(&MainFrame::CheckMySQLConnection, this));
	std::thread pingThread(std::bind(&MainFrame::SendPing, this));

	std::shared_ptr<WoWCurrencyChangeOrReplace> _mainWindow(nullptr);
	_mainWindow = std::make_unique<WoWCurrencyChangeOrReplace>(_settings, _connection);

	QTimer::singleShot(10, [&]()
		{
			//_mainWindow->LoadConstruktorData();
			QTimer::singleShot(500, [&]()
				{
					_mainWindow->show();
					splash->finish(_mainWindow.get());
				});
		});	

	QCoreApplication::processEvents();		

	int result = _application.exec();

	_shouldExit = true;

	connectionThread.join();
	pingThread.join();

	return result;
}

void MainFrame::AbortProgrammOnMySQLConnectionError()
{
	LOG_SQL("ERROR: MySQL Connection lost and can not restore. Shut down Program.");
	_application.quit();
}

/********************* Private Functions ************************************/
void MainFrame::LoadTranslation()
{
	QTranslator* translator = new QTranslator();
	_translator = translator;
	QLocale locale(QString::fromStdString(_settings->GetLanguage()));
	QLocale::setDefault(locale);

	QString translationsPath = QCoreApplication::applicationDirPath() + QString(":/Translation/Translation/");
	translationsPath += QLocale::system().name();

	qApp->addLibraryPath(translationsPath);

	QString test = locale.name();

	if (translator->load(":/Translation/Translation/Translation_" + locale.name() + ".qm"))
		qApp->installTranslator(translator);
}

void MainFrame::LoadStyle()
{
	std::string filePathString = ":/Styles//StyleTheme/style_";
	filePathString += _settings->GetStyleString();
	filePathString += ".qss";

	QFile styleFile(QString::fromStdString(filePathString));
	styleFile.open(QFile::ReadOnly | QFile::Text);
	QString style = QLatin1String(styleFile.readAll());

	_application.setStyleSheet(style);
}

void MainFrame::CheckMySQLConnection()
{
	std::this_thread::sleep_for(std::chrono::minutes(1));
	while (true)
	{
		_connection->CheckConnectionPeriodically();

		if (_shouldExit)
			break;

		for (int i = 0; i < 5 * 60; ++i)
		{
			std::this_thread::sleep_for(std::chrono::seconds(1));
			if (_shouldExit)
				break;
		}

		if (_shouldExit)
			break;

		// std::this_thread::sleep_for(std::chrono::minutes(5));
	}

	return;
}

void MainFrame::SendPing()
{
	std::this_thread::sleep_for(std::chrono::minutes(1));
	while (true)
	{
		_connection->KeepConnectionAlive();

		if (_shouldExit)
			break;

		for (int i = 0; i < 2 * 60; ++i)
		{
			std::this_thread::sleep_for(std::chrono::seconds(1));
			if (_shouldExit)
				break;
		}

		if (_shouldExit)
			break;

		//std::this_thread::sleep_for(std::chrono::minutes(2));
	}

	return;
}

void MainFrame::LoadingScreen()
{

}

std::unique_ptr<QSplashScreen> MainFrame::CreateSplashScreen()
{
	std::unique_ptr<QSplashScreen> splash = std::make_unique<QSplashScreen>(QPixmap(500, 70));
	splash->setStyleSheet("background-color: black;");
	splash->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, splash->size(),
		QApplication::primaryScreen()->geometry()));

	QProgressBar* progressBar = new QProgressBar(splash.get());
	progressBar->setRange(0, 100);
	progressBar->setTextVisible(false);
	progressBar->setGeometry(0, splash->size().height() - 20, splash->size().width(), 20);
	progressBar->setStyleSheet("QProgressBar {border: 2px solid grey; border-radius: 5px; background-color: #FFFFFF;}"
		"QProgressBar::chunk {background-color: #C0C0C0;}");
	progressBar->setValue(0);

	QLabel* messageLabel = new QLabel(splash.get());
	messageLabel->setStyleSheet("color: white;");  // with textcolor
	messageLabel->setText(QObject::tr("Bitte warten..."));
	messageLabel->setAlignment(Qt::AlignBottom | Qt::AlignCenter);
	messageLabel->setGeometry(0, 0, splash->size().width(), splash->size().height() - 25);

	QObject::connect(_loadingScreenTimer.get(), &QTimer::timeout, [progressBar]() {
		int value = progressBar->value();
		progressBar->setValue((value + 1) % progressBar->maximum());
		});
	_loadingScreenTimer->start(30);

	return splash;
}

void MainFrame::setTableWidgetsStyle(const QString& style, Styles styleEnum)
{
	QList<QWidget*> topLevelWidgets = _application.topLevelWidgets();

	for (QWidget* topLevelWidget : topLevelWidgets)
	{
		QList<QTableWidget*> tableWidgets = topLevelWidget->findChildren<QTableWidget*>();

		for (QTableWidget* tableWidget : tableWidgets)
		{
			tableWidget->setStyleSheet(style);
			_styleMgr->SetTableHeaderFormatAndTableColor(*tableWidget, styleEnum);
		}
	}
}

void MainFrame::UpdateUI()
{
	QList<QWidget*> allWidgets = _application.allWidgets();
	for (auto widget : allWidgets) 
	{
		
		if (auto label = qobject_cast<QLabel*>(widget)) {
			
			label->setText(tr(label->text().toStdString().c_str()));
		}
		else if (auto button = qobject_cast<QPushButton*>(widget)) {
			
			button->setText(tr(button->text().toStdString().c_str()));
		}
		else if (auto checkBox = qobject_cast<QCheckBox*>(widget)) {
			
			checkBox->setText(tr(checkBox->text().toStdString().c_str()));
		}
		else if (auto comboBox = qobject_cast<QComboBox*>(widget)) {
			
			for (int i = 0; i < comboBox->count(); ++i) {
				comboBox->setItemText(i, tr(comboBox->itemText(i).toStdString().c_str()));
			}
		}		
	}
}

int MainFrame::OnlySettingsWindow()
{
	QThread::sleep(2);

	std::unique_ptr<SettingsWindow> _settingsWindow = std::make_unique<SettingsWindow>(_settings);
	_settingsWindow->show();
//	std::unique_ptr<UIErrorHandler> _errorMgr = std::make_unique<UIErrorHandler>();
	//_errorMgr->SendBasicWarningDialog(ErrorCodes::ERROR_NO_MYSQL_CONNECTION);

	QCoreApplication::processEvents();

	return _application.exec();
}
