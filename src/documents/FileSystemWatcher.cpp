#include "FileSystemWatcher.h"

#include <QDebug>
#include <QFile>
#include <QFileSystemWatcher>
#include <QStringList>
using namespace QNplCad;


FileSystemWatcher::FileSystemWatcher(QObject *parent) :
	QObject(parent),
	mWatcher(new QFileSystemWatcher(this))
{
	connect(mWatcher, SIGNAL(fileChanged(QString)),
		SLOT(onFileChanged(QString)));
	connect(mWatcher, SIGNAL(directoryChanged(QString)),
		SLOT(onDirectoryChanged(QString)));
}

void FileSystemWatcher::addPath(const QString &path)
{
	// Just silently ignore the request when the file doesn't exist
	if (!QFile::exists(path))
		return;

	QMap<QString, int>::iterator entry = mWatchCount.find(path);
	if (entry == mWatchCount.end()) {
		mWatcher->addPath(path);
		mWatchCount.insert(path, 1);
	}
	else {
		// Path is already being watched, increment watch count
		++entry.value();
	}
}

void FileSystemWatcher::removePath(const QString &path)
{
	QMap<QString, int>::iterator entry = mWatchCount.find(path);
	if (entry == mWatchCount.end()) {
		if (QFile::exists(path))
			qWarning() << "FileSystemWatcher: Path was never added:" << path;
		return;
	}

	// Decrement watch count
	--entry.value();

	if (entry.value() == 0) {
		mWatchCount.erase(entry);
		mWatcher->removePath(path);
	}
}

void FileSystemWatcher::onFileChanged(const QString &path)
{
	// If the file was replaced, the watcher is automatically removed and needs
	// to be re-added to keep watching it for changes. This happens commonly
	// with applications that do atomic saving.
	if (!mWatcher->files().contains(path))
		if (QFile::exists(path))
			mWatcher->addPath(path);

	emit fileChanged(path);
}

void FileSystemWatcher::onDirectoryChanged(const QString &path)
{
	emit directoryChanged(path);
}
