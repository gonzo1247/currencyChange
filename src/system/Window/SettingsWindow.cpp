
#include <QFileDialog>

#include "SettingsWindow.h"
#include "WindowModulCustom.h"

SettingsWindow::SettingsWindow(std::shared_ptr<MySQLConnection> connection, std::shared_ptr<Settings> settings, QWidget *parent) : QMainWindow(parent),
ui(std::make_unique<Ui::SettingsWindowClass>()), _connection(connection), _settings(settings), _newLogPath(""), _crypto(std::make_unique<Crypto>()), _overrideVisibility(false)
{
	ui->setupUi(this);

	HWND hWnd = reinterpret_cast<HWND>(winId());
	LONG_PTR style = GetWindowLongPtr(hWnd, GWL_EXSTYLE);
	SetWindowLongPtr(hWnd, GWL_EXSTYLE, style | WS_EX_APPWINDOW);

	connect(ui->pB_SettingsBack, &QPushButton::clicked, this, &SettingsWindow::closeSettingsWindow);
	connect(ui->pB_selectPath, &QPushButton::clicked, this, &SettingsWindow::onPushSelectLogPath);
	connect(ui->pB_SettingsSave, &QPushButton::clicked, this, &SettingsWindow::onPushSaveSettings);

#ifdef Q_OS_WIN
	setDarkTitlebar(reinterpret_cast<HWND>(winId()));
#endif

	if (GetSingeltonSettings()->GetIsFullScreen())
		setWindowState(Qt::WindowFullScreen);
}

SettingsWindow::SettingsWindow(std::shared_ptr<Settings> settings, QWidget* parent /*= nullptr*/) : QMainWindow(parent), ui(std::make_unique<Ui::SettingsWindowClass>()),
_settings(settings), _newLogPath(""), _crypto(std::make_unique<Crypto>()), _overrideVisibility(true)
{
	ui->setupUi(this);

	connect(ui->pB_SettingsBack, &QPushButton::clicked, this, &SettingsWindow::closeSettingsWindow);
	connect(ui->pB_selectPath, &QPushButton::clicked, this, &SettingsWindow::onPushSelectLogPath);
	connect(ui->pB_SettingsSave, &QPushButton::clicked, this, &SettingsWindow::onPushSaveSettings);

#ifdef Q_OS_WIN
	setDarkTitlebar(reinterpret_cast<HWND>(winId()));
#endif
}

SettingsWindow::~SettingsWindow() { }

void SettingsWindow::LoadSettingsData()
{
	ui->le_logFilePath->setText(QString::fromStdString(_settings->GetLogFilePath()));
	ui->le_mySQLusername->setText(QString::fromStdString(_settings->GetMySQLUsername()));
	ui->le_mySQLDatabase->setText(QString::fromStdString(_settings->GetMySQLDatabaseName()));
	ui->le_mySQLHostename->setText(QString::fromStdString(_settings->GetMySQLHostname()));
	ui->le_mySQLPort->setText(QString::fromStdString(_settings->GetMySQLPort()));
	ui->le_mySQLPassword->setText(QString::fromStdString(_settings->GetMySQLPassword()));
	ui->cB_Style->setCurrentIndex(_settings->GetSelectedStyle());
	ui->cB_language->setCurrentIndex(GetSelectLanguageID(_settings->GetLanguage()));
}

void SettingsWindow::SelectLogPath()
{
	// Create an QFileDialog-Object
	QFileDialog folderDialog(this);
	QString windowTitle = tr("Select folder");
	folderDialog.setWindowTitle(windowTitle);
	folderDialog.setFileMode(QFileDialog::Directory); // Only Folders can be selected
	folderDialog.setOption(QFileDialog::ShowDirsOnly, true); // Show only directories

	// Show the folder dialog and wait to action from user
	if (folderDialog.exec() == QDialog::Accepted)
	{
		// Select the selected path
		QString selectedFolderPath = folderDialog.selectedFiles().first();

		// Show the selected Path to user
		ui->le_logFilePath->setText(selectedFolderPath);
		_newLogPath = selectedFolderPath.toStdString();
	}

}

void SettingsWindow::SaveSettings()
{
	bool _isChanged = false;

	if (!_newLogPath.empty() && _newLogPath != _settings->GetLogFilePath())
	{
		_settings->UpdateSettingFile(_settings->GetConfigKeyValues(SettingData::SETTING_DATA_LOG_FILE_PATH), _newLogPath);
		_isChanged = true;
	}

	if (ui->le_mySQLusername->isModified())
	{
		_settings->UpdateSettingFile(MYSQLUsername, ui->le_mySQLusername->text().toStdString());
		_isChanged = true;
	}

	if (ui->le_mySQLDatabase->isModified())
	{
		_settings->UpdateSettingFile(MYSQLDatabaseName, ui->le_mySQLDatabase->text().toStdString());
		_isChanged = true;
	}

	if (ui->le_mySQLHostename->isModified())
	{
		_settings->UpdateSettingFile(MYSQLHostName, ui->le_mySQLHostename->text().toStdString());
		_isChanged = true;
	}

	if (ui->le_mySQLPort->isModified())
	{
		_settings->UpdateSettingFile(MYSQLPort, ui->le_mySQLPort->text().toStdString());
		_isChanged = true;
	}

	if (ui->le_mySQLPassword->isModified())
	{
		CrypthPassword(ui->le_mySQLPassword->text().toStdString(), true);
		_isChanged = true;
	}

	if (static_cast<uint32_t>(ui->cB_Style->currentIndex()) != _settings->GetSelectedStyle())
	{
		std::uint32_t styleIndex = ui->cB_Style->currentIndex();
		_settings->UpdateSettingFile(styleThema, std::to_string(styleIndex));
		_isChanged = true;
	}

	if (ui->cB_language->currentIndex() != GetSelectLanguageID(_settings->GetLanguage()))
	{
		_settings->UpdateSettingFile(CurrentLanguage, GetSelectLangaugeString(SelectLanguage(ui->cB_language->currentIndex())));
		_isChanged = true;
	}

/*
	if (_isChanged && !_overrideVisibility)
		_errorMgr->SendSuccessfullyDialog(SuccessfullCodes::MESSAGE_SETTINGS_SUCCESSFULLY_SAVED);*/
}

void SettingsWindow::CrypthPassword(std::string password, bool isMySQLP /*= true*/)
{
	std::vector<unsigned char> passwordData(password.begin(), password.end());

	std::vector<unsigned char> key, iv;
	_crypto->GenerateRandomKeyAndIV(key, iv, EVP_CIPHER_key_length(EVP_aes_256_cbc()), EVP_CIPHER_iv_length(EVP_aes_256_cbc()));

	_crypto->SaveKeyAndIV(isMySQLP ? _settings->GetMySQLKFP() : _settings->GetSSHKFP(), key, iv);

	std::vector<unsigned char> encryptData = _crypto->EncryptData(passwordData, key, iv);

	_settings->UpdatePasswordInFile(encryptData, isMySQLP);

}

SuccessfullCodes SettingsWindow::mergeValues(const std::vector<SuccessfullCodes>& values)
{
	SuccessfullCodes lastNonNoData = SuccessfullCodes::MESSAGE_CODE_NO_DATA;

	for (auto it = values.rbegin(); it != values.rend(); ++it)
	{
		if (*it != SuccessfullCodes::MESSAGE_CODE_NO_DATA)
		{
			lastNonNoData = *it;
			break;
		}
	}

	return lastNonNoData;
}

void SettingsWindow::closeSettingsWindow() 
{
	close();
}

void SettingsWindow::onPushSelectLogPath()
{
	SelectLogPath();
 }

void SettingsWindow::onPushSaveSettings()
{
	SaveSettings();
}


void SettingsWindow::showEvent(QShowEvent* event)
{
	LoadSettingsData();

	QMainWindow::showEvent(event);
}

void SettingsWindow::changeEvent(QEvent* event)
{
	if (event->type() == QEvent::LanguageChange)
	{
		ui->retranslateUi(this);
	}

	QMainWindow::changeEvent(event);
}