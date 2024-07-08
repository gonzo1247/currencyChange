/*
Copyright 2023 - 2024 by Severin

*/

#include "StyleManager.h"

StyleManager::StyleManager() : _settings(std::make_unique<Settings>())
{
	FillStyleMap();

	_style = Styles(_settings->GetSelectedStyle());
}

StyleManager::~StyleManager() { }

void StyleManager::FillStyleMap()
{
	styleMap[Styles::STYLE_LIGHT][_baseColor]	= "#ebeff7";
	styleMap[Styles::STYLE_LIGHT][_windowText]	= "#011f20";
	styleMap[Styles::STYLE_LIGHT][_highlight]	= "#0a5399";
	styleMap[Styles::STYLE_LIGHT][_pushButtonBaseColor]	= "#bfcbe3";

	styleMap[Styles::STYLE_LIGHT_DARK][_baseColor]	= "#5d6d91";
	styleMap[Styles::STYLE_LIGHT_DARK][_windowText]	= "#edf0f2";
	styleMap[Styles::STYLE_LIGHT_DARK][_highlight]	= "#0a5399";
	styleMap[Styles::STYLE_LIGHT_DARK][_pushButtonBaseColor] = "#7892cf";

	styleMap[Styles::STYLE_DARK][_baseColor] = "#252526";
	styleMap[Styles::STYLE_DARK][_windowText] = "#d08318";
	styleMap[Styles::STYLE_DARK][_highlight] = "#0a5399";
	styleMap[Styles::STYLE_DARK][_pushButtonBaseColor] = "#4d4c4e";

	styleMap[Styles::STYLE_DARK_FLAT][_baseColor] = "#242526";
	styleMap[Styles::STYLE_DARK_FLAT][_windowText] = "#f0f0f0";
	styleMap[Styles::STYLE_DARK_FLAT][_highlight] = "#0a5399";
	styleMap[Styles::STYLE_DARK_FLAT][_pushButtonBaseColor] = "#607cff";
}

std::string StyleManager::GetBaseColor()
{
	auto _styleMap = styleMap.find(_style);
	if (_styleMap != styleMap.end()) 
	{
		auto colorData = _styleMap->second.find(_baseColor);
		if (colorData != _styleMap->second.end())
			return colorData->second;
	}

	return _fallBackBaseColor;
}

std::string StyleManager::GetBaseColor(Styles style)
{
	auto _styleMap = styleMap.find(style);
	if (_styleMap != styleMap.end())
	{
		auto colorData = _styleMap->second.find(_baseColor);
		if (colorData != _styleMap->second.end())
			return colorData->second;
	}

	return _fallBackBaseColor;
}

std::string StyleManager::GetWindowTextStyle()
{
	auto _styleMap = styleMap.find(_style);
	if (_styleMap != styleMap.end())
	{
		auto colorData = _styleMap->second.find(_windowText);
		if (colorData != _styleMap->second.end())
			return colorData->second;
	}

	return _fallBackWindowText;
}

std::string StyleManager::GetWindowTextStyle(Styles style)
{
	auto _styleMap = styleMap.find(style);
	if (_styleMap != styleMap.end())
	{
		auto colorData = _styleMap->second.find(_windowText);
		if (colorData != _styleMap->second.end())
			return colorData->second;
	}

	return _fallBackWindowText;
}

std::string StyleManager::GetHighlight()
{
	auto _styleMap = styleMap.find(_style);
	if (_styleMap != styleMap.end())
	{
		auto colorData = _styleMap->second.find(_highlight);
		if (colorData != _styleMap->second.end())
			return colorData->second;
	}

	return _fallBackHighlight;
}

std::string StyleManager::GetHighlight(Styles style)
{
	auto _styleMap = styleMap.find(style);
	if (_styleMap != styleMap.end())
	{
		auto colorData = _styleMap->second.find(_highlight);
		if (colorData != _styleMap->second.end())
			return colorData->second;
	}

	return _fallBackHighlight;
}

std::string StyleManager::GetPushButtonBaseColor()
{
	auto _styleMap = styleMap.find(_style);
	if (_styleMap != styleMap.end())
	{
		auto colorData = _styleMap->second.find(_pushButtonBaseColor);
		if (colorData != _styleMap->second.end())
			return colorData->second;
	}

	return _fallBackBaseColor;
}

std::string StyleManager::GetPushButtonBaseColor(Styles style)
{
	auto _styleMap = styleMap.find(style);
	if (_styleMap != styleMap.end())
	{
		auto colorData = _styleMap->second.find(_pushButtonBaseColor);
		if (colorData != _styleMap->second.end())
			return colorData->second;
	}

	return _fallBackBaseColor;
}

QPalette StyleManager::GetPaletteFilled(QPalette pal)
{
	pal.setColor(QPalette::Window, QColor(GetBaseColor().c_str()));
	pal.setColor(QPalette::WindowText, QColor(GetWindowTextStyle().c_str()));  // Window text color
	pal.setColor(QPalette::Highlight, QColor(GetHighlight().c_str()));  // Highlighting color (selection)

	return pal;
}

void StyleManager::GetPaletteFilledWithLabels(QList<QLabel*> labelList)
{
	for (QLabel* label : labelList)
	{
		QPalette labelPalette = label->palette();
		labelPalette.setColor(QPalette::WindowText, QColor(GetWindowTextStyle().c_str()));
		label->setPalette(labelPalette);
		label->setStyleSheet("QLabel { color: " + QString::fromStdString(GetWindowTextStyle()) + "; }");
	}
}

void StyleManager::GetPaletteForPushButtons(QList<QPushButton*> pushList)
{

	for (QPushButton* pushButton : pushList)
	{
		std::string pushButtonStyleString = "color: ";
		pushButtonStyleString += GetWindowTextStyle();
		pushButtonStyleString += "; background-color: ";
		pushButtonStyleString += GetPushButtonBaseColor();
		pushButtonStyleString += ";";

		pushButton->setStyleSheet(QString::fromStdString(pushButtonStyleString));
	}
}

void StyleManager::GetPlaetteForMenuChilds(QList<QMenu*> menus)
{
	for (QMenu* menu : menus)
	{
		std::string menuChildString = "color: ";
		menuChildString += GetWindowTextStyle();
		menuChildString += "; background-color: ";
		menuChildString += GetBaseColor();
		menuChildString += ";";

		menu->setStyleSheet(QString::fromStdString(menuChildString));
	}
}

void StyleManager::FillPaletteForMenuChild(QList<QLabel*> labelList)
{
	for (QLabel* label : labelList)
	{
		QPalette labelPalette = label->palette();
		labelPalette.setColor(QPalette::WindowText, QColor(GetWindowTextStyle().c_str()));
		label->setPalette(labelPalette);
		label->setStyleSheet("QLabel { color: " + QString::fromStdString(GetWindowTextStyle()) + "; }");
	}
}

void StyleManager::FillPaletteForMenuChild(QList<QPushButton*> pushList)
{
	for (QPushButton* pushButton : pushList)
	{
		std::string pushButtonStyleString = "color: ";
		pushButtonStyleString += GetWindowTextStyle();
		pushButtonStyleString += "; background-color: ";
		pushButtonStyleString += GetPushButtonBaseColor();
		pushButtonStyleString += ";";

		pushButton->setStyleSheet(QString::fromStdString(pushButtonStyleString));
	}
}

void StyleManager::FillPaletteForMenuChild(QList<QMenu*> menus)
{
	for (QMenu* menu : menus)
	{
		std::string menuChildString = "color: ";
		menuChildString += GetWindowTextStyle();
		menuChildString += "; background-color: ";
		menuChildString += GetBaseColor();
		menuChildString += ";";

		menu->setStyleSheet(QString::fromStdString(menuChildString));
	}
}

void StyleManager::FillPaletteForMenuChild(QList<QCheckBox*> _checkBox)
{
	for (QCheckBox* checkBox : _checkBox)
	{
		QPalette checkBoxPalette = checkBox->palette();
		checkBoxPalette.setColor(QPalette::WindowText, QColor(GetWindowTextStyle().c_str()));
		checkBox->setPalette(checkBoxPalette);
		checkBox->setStyleSheet("QCheckBox { color: " + QString::fromStdString(GetWindowTextStyle()) + "; }");
	}
}

void StyleManager::FillPaletteForMenuChild(QList<QTabWidget*> _tabWidget)
{
	for (QTabWidget* tabWidget : _tabWidget)
	{
		std::string messageBoxStyleString = "background-color: ";
		messageBoxStyleString += GetBaseColor();
		messageBoxStyleString += ";";

		tabWidget->setStyleSheet(QString::fromStdString(messageBoxStyleString));
		
		std::string hexSelectedColorCode = "#FFA500";  // 
		std::string hexNotSelectedColorCode = "#00FF00";  // 
		QString hexSelectedColorQString = QString::fromStdString(hexSelectedColorCode);
		QString hexNotSelectedColorQString = QString::fromStdString(hexNotSelectedColorCode);

		tabWidget->tabBar()->setStyleSheet(
			"QTabBar::tab:selected { background-color: " + hexSelectedColorQString + "; color: white; }"
			"QTabBar::tab:!selected { background-color: " + hexNotSelectedColorQString + "; color: black; }"
		);
	}
}

void StyleManager::FillPaletteForMenuChild(QList<QComboBox*> _comboBox)
{
	for (QComboBox* comboBox : _comboBox)
	{
		QPalette comboBoxPalette = comboBox->palette();
		comboBoxPalette.setColor(QPalette::WindowText, QColor(GetWindowTextStyle().c_str()));
		comboBox->setPalette(comboBoxPalette);
		comboBox->setStyleSheet("QComboBox { background-color: white }"
		"QComboBox QAbstractItemView { background-color: white; }");
	}
}

void StyleManager::FillPaletteForMenuChild(QList<QDateTimeEdit*> _dateTimeEdit)
{
	for (QDateTimeEdit* dateTimeEdit : _dateTimeEdit)
	{
		QPalette dateTimeEditPalette = dateTimeEdit->palette();
		dateTimeEditPalette.setColor(QPalette::WindowText, QColor(GetWindowTextStyle().c_str()));  
		dateTimeEdit->setPalette(dateTimeEditPalette);
		dateTimeEdit->setStyleSheet("QDateTimeEdit { background-color: white; } "
			"QDateTimeEdit::up-button, QDateTimeEdit::down-button { subcontrol-origin: border; subcontrol-position: center; background-color: blue; color: white; }");
	}
}

void StyleManager::FillPaletteForMenuChild(QTableWidgetItem* _item)
{
	_item->setBackground(QBrush(QColor(QString::fromStdString(GetBaseColor()))));
	_item->setData(Qt::ForegroundRole, QColor(QString::fromStdString(GetWindowTextStyle())));

}

void StyleManager::FillPaletteForMenuChild(QList<QTableWidget*> _tableWidget)
{
	std::string horizontalHeaderStyle = "QHeaderView::section { background-color: " + GetBaseColor() + "; color: " + GetWindowTextStyle() + "; }";

	std::string tableMainStyle = "QTableWidget::item:selected { background-color: red; color: white; } "
		"QTableCornerButton::section { background-color: " + GetBaseColor() + "; }";


	for (QTableWidget* tableWidget : _tableWidget)
	{
		tableWidget->horizontalHeader()->setStyleSheet(QString::fromStdString(horizontalHeaderStyle));
		tableWidget->verticalHeader()->setStyleSheet(QString::fromStdString(horizontalHeaderStyle));
		tableWidget->setStyleSheet(QString::fromStdString(tableMainStyle));
	}
}

void StyleManager::FillPaletteForMenuChild(QList<QLineEdit*> _lineEdit)
{
	for (QLineEdit* edits : _lineEdit)
	{
		edits->setStyleSheet("QLineEdit { background-color: white; }");
	}
}

void StyleManager::FillPaletteForMenuChild(QList<QLCDNumber*> _lcdNumber)
{
	for (QLCDNumber* numbers : _lcdNumber)
		numbers->setStyleSheet("QLCDNumber { background-color: " + QString::fromStdString(GetBaseColor()) + "; }");
}

void StyleManager::FillPaletteForChildOnStyleChange(QTableWidgetItem* _item, Styles style)
{
	_item->setBackground(QBrush(QColor(QString::fromStdString(GetBaseColor(style)))));
	_item->setData(Qt::ForegroundRole, QColor(QString::fromStdString(GetWindowTextStyle(style))));
}

void StyleManager::SetTableHeaderFormatAndTableColor(QTableWidget& table)
{
	for (int row = 0; row < table.rowCount(); ++row)
	{
		for (int col = 0; col < table.columnCount(); ++col)
		{
			QTableWidgetItem* item = table.item(row, col);
			if (item != nullptr)
			{
				item->setTextAlignment(Qt::AlignCenter);
				
				FillPaletteForMenuChild(item);
			}
		}
	}

	table.resizeRowsToContents();
	table.resizeColumnsToContents();
	table.horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
	table.verticalHeader()->setDefaultAlignment(Qt::AlignCenter);
}

void StyleManager::SetTableHeaderFormatAndTableColor(QTableWidget& table, Styles style)
{
	for (int row = 0; row < table.rowCount(); ++row)
	{
		for (int col = 0; col < table.columnCount(); ++col)
		{
			QTableWidgetItem* item = table.item(row, col);
			if (item != nullptr)
			{
				item->setTextAlignment(Qt::AlignCenter);
				FillPaletteForChildOnStyleChange(item, style);
			}
		}
	}

	table.resizeRowsToContents();
	table.resizeColumnsToContents();
	table.horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
	table.verticalHeader()->setDefaultAlignment(Qt::AlignCenter);
}

void StyleManager::SetTableHeaderFormatAndTableColorForInProcess(QTableWidget& table)
{
	for (int row = 0; row < table.rowCount(); ++row)
	{
		for (int col = 0; col < table.columnCount(); ++col)
		{
			QTableWidgetItem* item = table.item(row, col);
			if (item != nullptr)
			{
				item->setTextAlignment(Qt::AlignCenter);				
				FillPaletteForMenuChild(item);
				// Set color based on the content
				if (item->text() == "Express (Morgen)")
					item->setBackground(QColor(0, 0, 255)); // red
				else if (item->text() == "Kurierfahrt (Heute)")
					item->setBackground(QColor(255, 0, 0)); // blue
			}
		}
	}

	int minColumnWidth = 100;
	for (int col = 0; col < table.columnCount(); ++col)
		table.horizontalHeader()->setMinimumSectionSize(minColumnWidth);

	table.resizeRowsToContents();
	table.resizeColumnsToContents();
	table.horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
	table.verticalHeader()->setDefaultAlignment(Qt::AlignCenter);
}

void StyleManager::SetPaletteForQMessageBox(QMessageBox& _box)
{
	std::string messageBoxStyleString = "background-color: ";
	messageBoxStyleString += GetBaseColor();
	messageBoxStyleString += "; ";
	messageBoxStyleString += "color: ";
	messageBoxStyleString += GetWindowTextStyle();
	messageBoxStyleString += ";";

	_box.setStyleSheet(QString::fromStdString(messageBoxStyleString));
}

std::string StyleManager::GetMenuBarStyleString()
{
	std::string menuBarStyleString = "QMenuBar { background-color: ";
	menuBarStyleString += GetBaseColor();
	menuBarStyleString += " ; color: ";
	menuBarStyleString += GetWindowTextStyle();
	menuBarStyleString += "; }";

	return menuBarStyleString;
}

std::string StyleManager::GetTableBackgroundColor()
{
	std::string tableBackgroundString = "QTableWidget::item { border-bottom: 1px solid ";
	tableBackgroundString += GetWindowTextStyle();
	tableBackgroundString += "; } ";
	tableBackgroundString += "QTableWidget::item:selected { background-color: ";
	tableBackgroundString += GetBaseColor();
	tableBackgroundString += "; color: ";
	tableBackgroundString += GetWindowTextStyle();
	tableBackgroundString += "; }";

	return tableBackgroundString;

	// tableWidget->horizontalHeader()->setStyleSheet("background-color: yellow; color: blue; border: 1px solid black;");
}

std::string StyleManager::GetTableHeaderColor()
{
	/*
	tableWidget->setStyleSheet("QTableWidget::item { border-bottom: 1px solid #d08318; }"
						   "QTableWidget::item:selected { background-color: #252526; color: #d08318; }");
	
	*/

	std::string tableBackgroundString = "background-color: ";
	tableBackgroundString += GetBaseColor();
	tableBackgroundString += "; ";
	tableBackgroundString += "color: ";
	tableBackgroundString += GetWindowTextStyle();
	tableBackgroundString += "; border-bottom: ";
	tableBackgroundString += "1px solid ";
	tableBackgroundString += GetWindowTextStyle();
	tableBackgroundString += ";";

	return tableBackgroundString;
}

void StyleManager::adjustTableWidget(QTableWidget& table, int fixedColumn)
{
	table.resizeColumnsToContents();

	if (fixedColumn >= 0 && fixedColumn < table.columnCount()) 
		table.horizontalHeader()->setSectionResizeMode(fixedColumn, QHeaderView::Stretch);
	else
		LOG_MISC("Error while adjust TableWidget: {}", table.objectName().toStdString());

	for (int i = 0; i < table.columnCount(); ++i) 
		if (i != fixedColumn)
			table.horizontalHeader()->setSectionResizeMode(i, QHeaderView::ResizeToContents);
}
