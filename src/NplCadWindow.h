#pragma once
#include <QMainWindow>
#include "ui_nplcadwindow.h"
#include "controls/FilesView.h"
#include "controls/LogView.h"
#include "QParaEngineApp.h"
#include "renderer/GLView.h"
class DocumentManager;
class NplCadWindow : public QMainWindow {
	Q_OBJECT

public:
	NplCadWindow(QWidget * parent = Q_NULLPTR);
	~NplCadWindow();

private:
	Ui::NplCadWindow ui;

	GLView* m_GLView;
	FilesView* m_FilesView;
	LogView* m_LogView;

	QParaEngineApp mQParaEngineApp;

	bool load_stl(const QString& filename, bool is_reload = false);
	DocumentManager *mDocumentManager;
private slots:
	void newFile();
	void openFile();
};
