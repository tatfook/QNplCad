#pragma once
#include <QObject>
#include <QDateTime>
namespace QNplCad
{
	class Document : public QObject {
		Q_OBJECT
	public:
		Document(const QString &fileName = QString());
		~Document();
		bool save(QString *error = nullptr);
		bool save(const QString &fileName, QString *error = nullptr);
		static Document *load(const QString &fileName, QString *error = nullptr);
		QString fileName() const { return mFileName; }
		QString displayName() const;
		bool isModified() const;
		QDateTime lastSaved() const { return mLastSaved; }
	signals:
		void fileNameChanged(const QString &fileName, const QString &oldFileName);
		void modifiedChanged();
		void saved();
	private:
		QString mFileName;
		QDateTime mLastSaved;
	};
}
