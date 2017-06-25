#pragma once
#include <QMainWindow>
#include "ui_nplcadwindow.h"
#include "CadGLView.h"
#include "FilesView.h"
#include "LogView.h"
#include "QParaEngineApp.h"
class NplCadWindow : public QMainWindow {
	Q_OBJECT

public:
	NplCadWindow(QWidget * parent = Q_NULLPTR);
	~NplCadWindow();

private:
	Ui::NplCadWindow ui;

	CadGLView* mCadGLView;
	FilesView* mFilesView;
	LogView* mLogView;

	QParaEngineApp mQParaEngineApp;
private slots:
	void newFile();
};
