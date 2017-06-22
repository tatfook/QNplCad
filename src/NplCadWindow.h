#pragma once
#include <QMainWindow>
#include "ui_nplcadwindow.h"
#include "ParaEngineView.h"
#include "FilesView.h"
#include "LogView.h"

class NplCadWindow : public QMainWindow {
	Q_OBJECT

public:
	NplCadWindow(QWidget * parent = Q_NULLPTR);
	~NplCadWindow();

private:
	Ui::NplCadWindow ui;

	ParaEngineView* mParaEngineView;
	FilesView* mFilesView;
	LogView* mLogView;
};
