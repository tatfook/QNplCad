#include "NplCadWindow.h"
NplCadWindow::NplCadWindow(QWidget * parent) : QMainWindow(parent) {
	ui.setupUi(this);

	//3d view
	mParaEngineView = new ParaEngineView(ui.Dock3D);
	ui.Dock3D->setWidget(mParaEngineView);
	//files view
	mFilesView = new FilesView(ui.DockFiles);
	ui.DockFiles->setWidget(mFilesView);
	//log view
	mLogView = new LogView(ui.DockLog);
	ui.DockLog->setWidget(mLogView);
}

NplCadWindow::~NplCadWindow() {
	
}
