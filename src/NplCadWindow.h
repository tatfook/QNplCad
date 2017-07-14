﻿#pragma once
#include <QMainWindow>
#include <QTimer>
#include <QSettings>
#include "ui_nplcadwindow.h"
#include "controls/FilesView.h"
#include "controls/LogView.h"
#include "QParaEngineApp.h"
#include "renderer/GLView.h"
#include "NPLInterface.hpp"
namespace QNplCad
{
	class DocumentManager;
	class Document;
	class NplCadWindow : public QMainWindow {
		Q_OBJECT

	public:
		NplCadWindow(QWidget * parent = Q_NULLPTR);
		~NplCadWindow();
	private slots:
		void newFile();
		void openFile();
		bool saveFile();
		bool saveFileAs();
		void saveAll();
		void exportFile();
		void exportFileAs();
		void reloadFile();
		void closeFile();
		void closeAllFiles();
		void quit();
		void openRecentFile();
		void clearRecentFiles();
		void updateActions();
		void documentChanged(Document *doc);
		void closeDocument(int index);

		void updateWindowTitle();
		void update();
		void onNplMsg(NPLInterface::NPLObjectProxy msg);
	private:
		void startParaEngineApp(std::string cmdline = "");
		void setRecentFile(const QString &fileName);
		void updateRecentFiles();
		QStringList recentFiles() const;
		void writeSettings();
		void readSettings();
		bool open(const QString &fileName);
		bool save(const QString &fileName);
		QString fileDialogStartLocation() const;
		void addDocument(Document *document);
		bool confirmSave(Document *document);
		bool confirmAllSave();
	private:
		Ui::NplCadWindow ui;

		GLView* m_glView;
		LogView* m_logView;

		QParaEngineApp m_paraEngineApp;
		QSettings* m_settings;
		enum { MaxRecentFiles = 8 };
		QAction* m_recentFiles[MaxRecentFiles];
		Document* m_doc;

		bool load_stl(const QString& filename, bool is_reload = false);
		DocumentManager *m_documentManager;
		QTimer* m_timer;
		
	};
}
