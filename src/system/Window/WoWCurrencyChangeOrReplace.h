#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_WoWCurrencyChangeOrReplace.h"
#include "Settings.h"
#include "MySQLConnection.h"
#include "CurrencyChange.h"
#include "SettingsWindow.h"

QT_BEGIN_NAMESPACE
namespace Ui { class WoWCurrencyChangeOrReplaceClass; };
QT_END_NAMESPACE

class WoWCurrencyChangeOrReplace : public QMainWindow
{
    Q_OBJECT

public:
    WoWCurrencyChangeOrReplace(std::shared_ptr<Settings> settings, std::shared_ptr<MySQLConnection> connection, QWidget *parent = nullptr);
    ~WoWCurrencyChangeOrReplace();

private:
    void StartConverting();

    MoneyPvE CollectPvEMoneyData();
    MoneyPvP CollectPvPMoneyData();

    std::unique_ptr<Ui::WoWCurrencyChangeOrReplaceClass> ui;
    std::shared_ptr<Settings> _settings;
    std::shared_ptr<MySQLConnection> _connection;
    std::shared_ptr<SettingsWindow> _settingWindow;

private slots:
    void OnPushStart();
    void OnPushSettings();
};
