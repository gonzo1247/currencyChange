/*
Copyright 2023 - 2024 by Severin

*/

#pragma once

#include <string>
#include <map>
#include <cstdint>

#include <QApplication>
#include <QLCDNumber>
#include <QHeaderView>
#include <QDateTimeEdit>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QComboBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QMenu>
#include <QMessageBox>
#include <QTabWidget>

#include "SharedDefines.h"
#include "Settings.h"
#include "Logger.h"

class Settings;
class CustomHeaderView;

class StyleManager
{
public:
	StyleManager();
	~StyleManager();
	void FillStyleMap();
	std::string GetBaseColor();
	std::string GetBaseColor(Styles style);
	std::string GetWindowTextStyle();
	std::string GetWindowTextStyle(Styles style);
	std::string GetHighlight();
	std::string GetHighlight(Styles style);
	std::string GetPushButtonBaseColor();
	std::string GetPushButtonBaseColor(Styles style);

	template<typename... Args>
	void ApplyStyleToChilds(Args... widgets)
	{
		(FillPaletteForMenuChild(widgets), ...);
	}

	QPalette GetPaletteFilled(QPalette pal);
	void GetPaletteFilledWithLabels(QList<QLabel*> labelList);
	void GetPaletteForPushButtons(QList<QPushButton*> pushList);
	void GetPlaetteForMenuChilds(QList<QMenu*> menus);

	void FillPaletteForMenuChild(QList<QLabel*> labelList);
	void FillPaletteForMenuChild(QList<QPushButton*> pushList);
	void FillPaletteForMenuChild(QList<QMenu*> menus);
	void FillPaletteForMenuChild(QList<QCheckBox*> _checkBox);
	void FillPaletteForMenuChild(QList<QMessageBox*> _checkBox);
	void FillPaletteForMenuChild(QList<QTabWidget*> _tabWidget);
	void FillPaletteForMenuChild(QList<QComboBox*> _comboBox);
	void FillPaletteForMenuChild(QList<QDateTimeEdit*> _dateTimeEdit);
	void FillPaletteForMenuChild(QTableWidgetItem* _item);
	void FillPaletteForMenuChild(QList<QTableWidget*> _tableWidget);
	void FillPaletteForMenuChild(QList<QLineEdit*> _lineEdit);
	void FillPaletteForMenuChild(QList<QLCDNumber*> _lcdNumber);
	void FillPaletteForChildOnStyleChange(QTableWidgetItem* _item, Styles style);

	void SetTableHeaderFormatAndTableColor(QTableWidget& table);
	void SetTableHeaderFormatAndTableColor(QTableWidget& table, Styles style);
	void SetTableHeaderFormatAndTableColorForInProcess(QTableWidget& table);

	void SetPaletteForQMessageBox(QMessageBox& _box);
	std::string GetMenuBarStyleString();
	std::string GetTableBackgroundColor();
	std::string GetTableHeaderColor();

	void adjustTableWidget(QTableWidget& table, int fixedColumn);


private:

	std::map<Styles, std::map<std::string, std::string>> styleMap;

	std::string _baseColor	= "BaseColor";
	std::string _windowText = "WindowText";
	std::string _highlight	= "Highlight";
	std::string _pushButtonBaseColor = "PushButtonBaseColor";

	std::string _fallBackBaseColor = "#ebeff7";
	std::string _fallBackWindowText = "Qt::white";
	std::string _fallBackHighlight = "Qt::blue";

	std::unique_ptr<Settings> _settings;

	Styles _style;
};

