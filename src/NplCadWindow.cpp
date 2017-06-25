#include "NplCadWindow.h"
NplCadWindow::NplCadWindow(QWidget * parent) : QMainWindow(parent) {
	ui.setupUi(this);

	//3d view
	mCadGLView = new CadGLView(ui.Dock3D);
	ui.Dock3D->setWidget(mCadGLView);
	//files view
	mFilesView = new FilesView(ui.DockFiles);
	ui.DockFiles->setWidget(mFilesView);
	//log view
	mLogView = new LogView(ui.DockLog);
	ui.DockLog->setWidget(mLogView);

	connect(ui.actionNew, &QAction::triggered, this, &NplCadWindow::newFile);
	
}

NplCadWindow::~NplCadWindow() {
	
}

void NplCadWindow::newFile()
{
	mQParaEngineApp.Test();
}
