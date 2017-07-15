#include "Document.h"
#include <QFileInfo>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QTextStream>
#include <QTextDocumentWriter>
#include <QDir>
#include <QMessageBox>

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

	mTextEdit->document()->setModified(false);
	mTextEdit->document()->clearUndoRedoStacks();
	connect(mTextEdit, SIGNAL(textChanged()), SIGNAL(textChanged()));

}

Document::~Document() {
	
}

bool Document::save(QString *error /*= nullptr*/)
{
	return save(fileName(),error);
}

bool Document::save(const QString &fileName, QString *error /*= nullptr*/)
{
	QFile file(fileName);
	if (file.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		QTextStream out(&file);
		out << mTextEdit->toPlainText();
		file.close();

		mTextEdit->document()->setModified(false);
		setFileName(fileName);
		mLastSaved = QFileInfo(fileName).lastModified();

		emit saved();

		return true;

	}
	else
	{
		QMessageBox::critical(this, tr("Could not write to file \"%1\"").arg(QDir::toNativeSeparators(fileName)), *error);
	}

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
	return mTextEdit->document()->isModified();
	/*int undo_steps = mTextEdit->document()->availableUndoSteps();
	return (undo_steps > 0);*/
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
	mTextEdit->document()->setModified(false);
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

