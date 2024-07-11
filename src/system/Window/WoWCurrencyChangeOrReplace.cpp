
#include <QIntValidator>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QString>
#include <QRegularExpression>

#include "WoWCurrencyChangeOrReplace.h"

WoWCurrencyChangeOrReplace::WoWCurrencyChangeOrReplace(std::shared_ptr<Settings> settings, std::shared_ptr<MySQLConnection> connection, QWidget *parent) : QMainWindow(parent),
_settings(settings), _connection(connection), ui(std::make_unique<Ui::WoWCurrencyChangeOrReplaceClass>()), _settingWindow(nullptr)
{
    ui->setupUi(this);

    QIntValidator* validator = new QIntValidator(this);
    validator->setRange(0, 100);
    ui->le_goldPvE->setValidator(validator);
    ui->le_silverPvE->setValidator(validator);
    ui->le_copperPvE->setValidator(validator);
	ui->le_goldPvP->setValidator(validator);
	ui->le_silverPvP->setValidator(validator);
	ui->le_copperPvP->setValidator(validator);

    QIntValidator* validatorMail = new QIntValidator(this);
    validatorMail->setRange(0, 2147483647);
    ui->le_mailDatabaseID->setValidator(validatorMail);

    QList<QLineEdit*> lines = findChildren<QLineEdit*>();

    for (QLineEdit* lineEdit : lines)
		if (lineEdit->objectName() != "le_pveMailSubject" && lineEdit->objectName() != "le_pvpMailSubject")
			lineEdit->setText("0");

    connect(ui->pB_startConvert, &QPushButton::clicked, this, &WoWCurrencyChangeOrReplace::OnPushStart);
    connect(ui->actionSettings, &QAction::triggered, this, &WoWCurrencyChangeOrReplace::OnPushSettings);

    // Set Default values
    MoneyPvE _default;
    ui->le_goldPvE->setText(QString::number(_default._gold));
    ui->le_silverPvE->setText(QString::number(_default._silver));
    ui->le_copperPvE->setText(QString::number(_default._copper));

	MoneyPvP _defaultPvP;
	ui->le_goldPvP->setText(QString::number(_defaultPvP._gold));
	ui->le_silverPvP->setText(QString::number(_defaultPvP._silver));
	ui->le_copperPvP->setText(QString::number(_defaultPvP._copper));
}

WoWCurrencyChangeOrReplace::~WoWCurrencyChangeOrReplace()
{
}

void WoWCurrencyChangeOrReplace::StartConverting()
{
    MoneyPvE moneyPvE = CollectPvEMoneyData();
    MoneyPvP moneyPvP = CollectPvPMoneyData();

    std::uint32_t _mailID = ui->le_mailDatabaseID->text().toInt();

    if (_mailID != 0)
    {

        std::unique_ptr<CurrencyChange> _currencyChange = std::make_unique<CurrencyChange>(_connection, moneyPvE, moneyPvP);

        LoadCurrency selectCurrency = static_cast<LoadCurrency>(ui->cB_currency->currentIndex());

        ui->lb_dataLoading->setText(QString::number(_currencyChange->LoadCurrencyDataFromDatabase(selectCurrency)));

        ConvertData _data = _currencyChange->ConvertCurrency();
        ui->lb_dataPvE->setText(QString::number(_data._pveConvert));
        ui->lb_dataPvP->setText(QString::number(_data._pvpConvert));
        ui->lb_dataMoney->setText(QString::number(_data._moneyConvert));

        _currencyChange->CreateSQLUpdateOutput(_mailID);
    }
    else
        QMessageBox::warning(this, tr("Warning"), tr("No Mail Database ID set. Set Mail ID first!"));

}

MoneyPvE WoWCurrencyChangeOrReplace::CollectPvEMoneyData()
{
    MoneyPvE money;
    money._gold = ui->le_goldPvE->text().toInt();
    money._silver = ui->le_silverPvE->text().toInt();
    money._copper = ui->le_copperPvE->text().toInt();
    money._MailSubject = ui->le_pveMailSubject->text().toStdString();
    money._MailBoddy = ui->pTE_pveMailMessage->toPlainText().toStdString();

    return money;
}

MoneyPvP WoWCurrencyChangeOrReplace::CollectPvPMoneyData()
{
	MoneyPvP money;
	money._gold = ui->le_goldPvP->text().toInt();
	money._silver = ui->le_silverPvP->text().toInt();
	money._copper = ui->le_copperPvP->text().toInt();
	money._MailSubject = ui->le_pvpMailSubject->text().toStdString();
    money._MailBoddy = ui->pTE_pvPMailMessage->toPlainText().toStdString();

	return money;
}

void WoWCurrencyChangeOrReplace::OnPushStart()
{
    StartConverting();
}

void WoWCurrencyChangeOrReplace::OnPushSettings()
{
    if (!_settingWindow)
        _settingWindow = std::make_shared<SettingsWindow>(_connection, _settings);

    _settingWindow->show();
}
