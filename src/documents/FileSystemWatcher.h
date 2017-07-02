#pragma once
#include <QObject>
#include <QMap>
class QFileSystemWatcher;

class FileSystemWatcher : public QObject {
	Q_OBJECT

public:
	FileSystemWatcher(QObject * parent = Q_NULLPTR);
	void addPath(const QString &path);
	void removePath(const QString &path);

signals:
	void fileChanged(const QString &path);
	void directoryChanged(const QString &path);

	private slots:
	void onFileChanged(const QString &path);
	void onDirectoryChanged(const QString &path);

private:
	QFileSystemWatcher *mWatcher;
	QMap<QString, int> mWatchCount;
	
};
