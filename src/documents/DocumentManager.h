#pragma once
#include <QObject>
namespace QNplCad
{
	class Document;
	class MovableTabWidget;
	class FileSystemWatcher;
	class DocumentManager : public QObject {
		Q_OBJECT

	public:
		static DocumentManager *instance();
		static void deleteInstance();
		/**
		* Returns the document manager widget. It contains the different map views
		* and a tab bar to switch between them.
		*/
		QWidget *widget() const;
		/**
		* Returns the current map document, or 0 when there is none.
		*/
		Document *currentDocument() const;

		/**
		* Returns the number of map documents.
		*/
		int documentCount() const { return mDocuments.size(); }

		/**
		* Searches for a document with the given \a fileName and returns its
		* index. Returns -1 when the document isn't open.
		*/
		int findDocument(const QString &fileName) const;

		/**
		* Switches to the map document at the given \a index.
		*/
		void switchToDocument(int index);
		void switchToDocument(Document *document);

		/**
		* Adds the new or opened \a mapDocument to the document manager.
		*/
		void addDocument(Document *document);

		/**
		* Closes the current map document. Will not ask the user whether to save
		* any changes!
		*/
		void closeCurrentDocument();

		/**
		* Closes the document at the given \a index. Will not ask the user whether
		* to save any changes!
		*/
		void closeDocumentAt(int index);

		/**
		* Reloads the current document. Will not ask the user whether to save any
		* changes!
		*
		* \sa reloadDocumentAt()
		*/
		bool reloadCurrentDocument();

		/**
		* Reloads the document at the given \a index. It will lose any undo
		* history and current selections. Will not ask the user whether to save
		* any changes!
		*
		* Returns whether the map loaded successfully.
		*/
		bool reloadDocumentAt(int index);

		/**
		* Close all documents. Will not ask the user whether to save any changes!
		*/
		void closeAllDocuments();
		/**
		* Returns all open map documents.
		*/
		QList<Document*> documents() const { return mDocuments; }
	signals:
		/**
		* Emitted when the current displayed map document changed.
		*/
		void currentDocumentChanged(Document *document);

		/**
		* Emitted when the user requested the document at \a index to be closed.
		*/
		void documentCloseRequested(int index);

		/**
		* Emitted when a document is about to be closed.
		*/
		void documentAboutToClose(Document *document);

		/**
		* Emitted when an error occurred while reloading the map.
		*/
		void reloadError(const QString &error);
		public slots:
		void switchToLeftDocument();
		void switchToRightDocument();
		private slots:
		void currentIndexChanged();
		void fileNameChanged(const QString &fileName,
			const QString &oldFileName);
		void updateDocumentTab();
		void documentSaved();
		void documentTabMoved(int from, int to);

		void fileChanged(const QString &fileName);

		void reloadRequested();
	private:
		DocumentManager(QObject *parent = nullptr);
		~DocumentManager();

		QList<Document*> mDocuments;

		MovableTabWidget *mTabWidget;
		FileSystemWatcher *mFileSystemWatcher;

		static DocumentManager *mInstance;

	};
}
