#include "DocumentManager.h"
#include <QFileInfo>
#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QScrollBar>
#include <QUndoStack>
#include <QVBoxLayout>
#include <QTextEdit>
#include "MovableTabWidget.h"
#include "FileSystemWatcher.h"
#include "Document.h"
class FileChangedWarning : public QWidget
{
	Q_OBJECT

public:
	FileChangedWarning(QWidget *parent = nullptr)
		: QWidget(parent)
		, mLabel(new QLabel(this))
		, mButtons(new QDialogButtonBox(QDialogButtonBox::Yes |
			QDialogButtonBox::No,
			Qt::Horizontal,
			this))
	{
		mLabel->setText(tr("File change detected. Discard changes and reload the map?"));

		QHBoxLayout *layout = new QHBoxLayout;
		layout->addWidget(mLabel);
		layout->addStretch(1);
		layout->addWidget(mButtons);
		setLayout(layout);

		connect(mButtons, SIGNAL(accepted()), SIGNAL(reload()));
		connect(mButtons, SIGNAL(rejected()), SIGNAL(ignore()));
	}

signals:
	void reload();
	void ignore();

private:
	QLabel *mLabel;
	QDialogButtonBox *mButtons;
};
class ViewContainer : public QWidget
{
	Q_OBJECT

public:
	ViewContainer(Document *document,
		QWidget *parent = nullptr)
		: QWidget(parent)
		, mWarning(new FileChangedWarning)
		, mTextEdit(new QTextEdit)
	{
		mWarning->setVisible(false);

		QVBoxLayout *layout = new QVBoxLayout(this);
		layout->setMargin(0);
		layout->setSpacing(0);
		layout->addWidget(mTextEdit);

		connect(mWarning, &FileChangedWarning::reload, this, &ViewContainer::reload);
		connect(mWarning, &FileChangedWarning::ignore, mWarning, &FileChangedWarning::hide);
	}


	void setFileChangedWarningVisible(bool visible)
	{
		mWarning->setVisible(visible);
	}

signals:
	void reload();
private:

	FileChangedWarning *mWarning;
	QTextEdit* mTextEdit;
};
DocumentManager *DocumentManager::mInstance;
DocumentManager *DocumentManager::instance()
{
	if (!mInstance)
		mInstance = new DocumentManager;
	return mInstance;
}

void DocumentManager::deleteInstance()
{
	delete mInstance;
	mInstance = nullptr;
}

DocumentManager::DocumentManager(QObject * parent) 
	: QObject(parent) 
	, mTabWidget(new MovableTabWidget)
	, mFileSystemWatcher(new FileSystemWatcher(this))
{
	mTabWidget->setDocumentMode(true);
	mTabWidget->setTabsClosable(true);

	connect(mTabWidget, SIGNAL(currentChanged(int)),
		SLOT(currentIndexChanged()));
	connect(mTabWidget, SIGNAL(tabCloseRequested(int)),
		SIGNAL(documentCloseRequested(int)));
	connect(mTabWidget, SIGNAL(tabMoved(int, int)),
		SLOT(documentTabMoved(int, int)));

	connect(mFileSystemWatcher, SIGNAL(fileChanged(QString)),
		SLOT(fileChanged(QString)));
}
DocumentManager::~DocumentManager() {
	// All documents should be closed gracefully beforehand
	Q_ASSERT(mDocuments.isEmpty());
	delete mTabWidget;
}
QWidget *DocumentManager::widget() const
{
	return mTabWidget;
}

Document *DocumentManager::currentDocument() const
{
	const int index = mTabWidget->currentIndex();
	if (index == -1)
		return nullptr;

	return mDocuments.at(index);
}
int DocumentManager::findDocument(const QString &fileName) const
{
	const QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath();
	if (canonicalFilePath.isEmpty()) // file doesn't exist
		return -1;

	for (int i = 0; i < mDocuments.size(); ++i) {
		QFileInfo fileInfo(mDocuments.at(i)->fileName());
		if (fileInfo.canonicalFilePath() == canonicalFilePath)
			return i;
	}

	return -1;
}

void DocumentManager::switchToDocument(int index)
{
	mTabWidget->setCurrentIndex(index);
}

void DocumentManager::switchToDocument(Document *document)
{
	const int index = mDocuments.indexOf(document);
	if (index != -1)
		switchToDocument(index);
}

void DocumentManager::switchToLeftDocument()
{
	const int tabCount = mTabWidget->count();
	if (tabCount < 2)
		return;

	const int currentIndex = mTabWidget->currentIndex();
	switchToDocument((currentIndex > 0 ? currentIndex : tabCount) - 1);
}

void DocumentManager::switchToRightDocument()
{
	const int tabCount = mTabWidget->count();
	if (tabCount < 2)
		return;

	const int currentIndex = mTabWidget->currentIndex();
	switchToDocument((currentIndex + 1) % tabCount);
}
void DocumentManager::addDocument(Document *document)
{
	Q_ASSERT(document);
	Q_ASSERT(!mDocuments.contains(document));

	mDocuments.append(document);

	if (!document->fileName().isEmpty())
		mFileSystemWatcher->addPath(document->fileName());

	ViewContainer *container = new ViewContainer(document, mTabWidget);

	const int documentIndex = mDocuments.size() - 1;

	mTabWidget->addTab(container, document->displayName());
	mTabWidget->setTabToolTip(documentIndex, document->fileName());
	connect(document, SIGNAL(fileNameChanged(QString, QString)),
		SLOT(fileNameChanged(QString, QString)));
	connect(document, SIGNAL(modifiedChanged()), SLOT(updateDocumentTab()));
	connect(document, SIGNAL(saved()), SLOT(documentSaved()));

	connect(container, SIGNAL(reload()), SLOT(reloadRequested()));

	switchToDocument(documentIndex);
}

void DocumentManager::closeCurrentDocument()
{
	const int index = mTabWidget->currentIndex();
	if (index == -1)
		return;

	closeDocumentAt(index);
}

void DocumentManager::closeDocumentAt(int index)
{
	Document *document = mDocuments.at(index);
	emit documentAboutToClose(document);

	QWidget *mapViewContainer = mTabWidget->widget(index);
	mDocuments.removeAt(index);
	mTabWidget->removeTab(index);
	delete mapViewContainer;

	if (!document->fileName().isEmpty())
		mFileSystemWatcher->removePath(document->fileName());

	delete document;
}

bool DocumentManager::reloadCurrentDocument()
{
	const int index = mTabWidget->currentIndex();
	if (index == -1)
		return false;

	return reloadDocumentAt(index);
}

bool DocumentManager::reloadDocumentAt(int index)
{
	Document *oldDocument = mDocuments.at(index);

	QString error;
	Document *newDocument = Document::load(oldDocument->fileName(),
		&error);
	if (!newDocument) {
		emit reloadError(tr("%1:\n\n%2").arg(oldDocument->fileName(), error));
		return false;
	}
	// Replace old tab
	addDocument(newDocument);
	closeDocumentAt(index);
	mTabWidget->moveTab(mDocuments.size() - 1, index);
	return true;
}

void DocumentManager::closeAllDocuments()
{
	while (!mDocuments.isEmpty())
		closeCurrentDocument();
}

void DocumentManager::currentIndexChanged()
{
	Document *document = currentDocument();
	emit currentDocumentChanged(document);

	
}
void DocumentManager::fileNameChanged(const QString &fileName,
	const QString &oldFileName)
{
	if (!fileName.isEmpty())
		mFileSystemWatcher->addPath(fileName);
	if (!oldFileName.isEmpty())
		mFileSystemWatcher->removePath(oldFileName);

	updateDocumentTab();
}

void DocumentManager::updateDocumentTab()
{
	Document *document = static_cast<Document*>(sender());
	const int index = mDocuments.indexOf(document);

	QString tabText = document->displayName();
	if (document->isModified())
		tabText.prepend(QLatin1Char('*'));

	mTabWidget->setTabText(index, tabText);
	mTabWidget->setTabToolTip(index, document->fileName());
}

void DocumentManager::documentSaved()
{
	Document *document = static_cast<Document*>(sender());
	const int index = mDocuments.indexOf(document);
	Q_ASSERT(index != -1);

	QWidget *widget = mTabWidget->widget(index);
	ViewContainer *container = static_cast<ViewContainer*>(widget);
	container->setFileChangedWarningVisible(false);
}

void DocumentManager::documentTabMoved(int from, int to)
{
	mDocuments.move(from, to);
}

void DocumentManager::fileChanged(const QString &fileName)
{
	const int index = findDocument(fileName);

	// Most likely the file was removed
	if (index == -1)
		return;

	Document *document = mDocuments.at(index);

	// Ignore change event when it seems to be our own save
	if (QFileInfo(fileName).lastModified() == document->lastSaved())
		return;

	// Automatically reload when there are no unsaved changes
	if (!document->isModified()) {
		reloadDocumentAt(index);
		return;
	}

	QWidget *widget = mTabWidget->widget(index);
	ViewContainer *container = static_cast<ViewContainer*>(widget);
	container->setFileChangedWarningVisible(true);
}

void DocumentManager::reloadRequested()
{
	int index = mTabWidget->indexOf(static_cast<ViewContainer*>(sender()));
	Q_ASSERT(index != -1);
	reloadDocumentAt(index);
}
#include "DocumentManager.moc"