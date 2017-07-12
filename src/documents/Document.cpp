#include "Document.h"
#include <QFileInfo>
using namespace QNplCad;

Document::Document(const QString &fileName) 
	: mFileName(fileName) 
{
	
}

Document::~Document() {
	
}

bool Document::save(QString *error /*= nullptr*/)
{
	return false;
}

bool Document::save(const QString &fileName, QString *error /*= nullptr*/)
{
	return false;
}

Document * Document::load(const QString &fileName, QString *error /*= nullptr*/)
{
	Document* doc = new Document(fileName);
	return doc;
}

QString Document::displayName() const
{
	QString displayName = QFileInfo(mFileName).fileName();
	if (displayName.isEmpty())
		displayName = tr("untitled.lua");

	return displayName;
}

bool Document::isModified() const
{
	return false;
}

