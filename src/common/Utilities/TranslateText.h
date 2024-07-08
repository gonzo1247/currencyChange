#pragma once

#include <String>
#include <Vector>
#include <QTableWidget>
#include <QCoreApplication>

class TranslateText
{
public:
	TranslateText();

	static QString translate(const char* context, const char* sourceText, const char* disambiguation = nullptr, int n = -1);
	//static std::string stringTranslate(const char* context, const char* sourceText, const char* disambiguation = nullptr, int n = -1);

private:
};

