#include "Document.h"
#include <QFileInfo>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QTextStream>

using namespace QNplCad;

Document::Document(QWidget* parent)
	: QWidget(parent) 
	, mWarning(new FileChangedWarning)
	, mTextEdit(new QTextEdit)
{
	mWarning->setVisible(false);

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setMargin(0);
	layout->setSpacing(0);
	layout->addWidget(mTextEdit);

	connect(mWarning, &FileChangedWarning::reload, this, &Document::reload);
	connect(mWarning, &FileChangedWarning::ignore, mWarning, &FileChangedWarning::hide);

	
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

QString Document::displayName() const
{
	QString displayName = QFileInfo(mFileName).fileName();
	if (displayName.isEmpty())
		displayName = tr("untitled.lua");

	return displayName;
}

bool Document::isModified() const
{
	int undo_steps = mTextEdit->document()->availableUndoSteps();
	return (undo_steps > 0);
}

bool Document::loadFile(const QString &fileName, QString *error)
{
	QFile file(fileName);

	if (!file.open(QIODevice::ReadWrite | QIODevice::Text))
		return false;

	QTextStream in(&file);
	while (!in.atEnd())
	{
		QString line = in.readLine();
		mTextEdit->append(line);
	}
	setFileName(fileName);
	mTextEdit->document()->clearUndoRedoStacks();
	connect(mTextEdit, SIGNAL(textChanged()), SIGNAL(textChanged()));
	return true;
}

void Document::setFileChangedWarningVisible(bool visible)
{
	mWarning->setVisible(visible);
}

void Document::setFileName(const QString &fileName)
{
	if (mFileName == fileName)
		return;

	QString oldFileName = mFileName;
	mFileName = fileName;
	emit fileNameChanged(fileName, oldFileName);
}

