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
using namespace QNplCad;
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


	const int documentIndex = mDocuments.size() - 1;

	mTabWidget->addTab(document, document->displayName());
	mTabWidget->setTabToolTip(documentIndex, document->fileName());
	connect(document, SIGNAL(fileNameChanged(QString, QString)),
		SLOT(fileNameChanged(QString, QString)));
	connect(document, SIGNAL(textChanged()), SLOT(updateDocumentTab()));
	connect(document, SIGNAL(saved()), SLOT(documentSaved()));

	connect(document, SIGNAL(reload()), SLOT(reloadRequested()));
	watchFile(document->fileName());
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

	mDocuments.removeAt(index);
	mTabWidget->removeTab(index);

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
	oldDocument->setFileChangedWarningVisible(false);

	QString error;
	Document *newDocument = new Document();
	bool result = newDocument->loadFile(oldDocument->fileName(),&error);
	if (!result) {
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
	watchFile(fileName);
	unWatchFile(oldFileName);
	updateDocumentTab();
}

void DocumentManager::updateDocumentTab()
{
	Document *document = static_cast<Document*>(sender());
	if (!document)
	{
		return;
	}
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
	Document *doc = static_cast<Document*>(widget);
	doc->setFileChangedWarningVisible(false);
	updateDocumentTab();
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
	Document *doc = static_cast<Document*>(widget);
	doc->setFileChangedWarningVisible(true);
}

void DocumentManager::reloadRequested()
{
	int index = mTabWidget->indexOf(static_cast<Document*>(sender()));
	Q_ASSERT(index != -1);
	reloadDocumentAt(index);
}

void DocumentManager::watchFile(const QString &fileName)
{
	if (!fileName.isEmpty())
		mFileSystemWatcher->addPath(fileName);
}

void DocumentManager::unWatchFile(const QString &fileName)
{
	if (!fileName.isEmpty())
		mFileSystemWatcher->removePath(fileName);
}

#include "DocumentManager.moc"