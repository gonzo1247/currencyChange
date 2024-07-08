#include "TranslateText.h"

TranslateText::TranslateText() { }

QString TranslateText::translate(const char* context, const char* sourceText, const char* disambiguation /*= nullptr*/, int n /*= -1*/)
{
	return QCoreApplication::translate(context, sourceText, disambiguation, n).toUtf8().constData();
}

/*
std::string TranslateText::stringTranslate(const char* context, const char* sourceText, const char* disambiguation / *= nullptr* /, int n / *= -1* /)
{
	QString translation = translate(context, sourceText, disambiguation, n);
	return translation.toStdString();
}*/
