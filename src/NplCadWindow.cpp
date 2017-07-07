#include "NplCadWindow.h"
#include "renderer/Loader.h"
#include "documents/DocumentManager.h"
#include "documents/Document.h"
NplCadWindow::NplCadWindow(QWidget * parent) 
	: QMainWindow(parent) 
	, mDocumentManager(DocumentManager::instance())
{
	ui.setupUi(this);
	setCentralWidget(mDocumentManager->widget());
	//3d view
	QGLFormat format;
	m_GLView = new GLView(format,ui.Dock3D);
	ui.Dock3D->setWidget(m_GLView);
	//files view
	m_FilesView = new FilesView(ui.DockFiles);
	ui.DockFiles->setWidget(m_FilesView);
	//log view
	m_LogView = new LogView(ui.DockLog);
	ui.DockLog->setWidget(m_LogView);

	mQParaEngineApp.Run("bootstrapper=\"test/main.lua\"");
	connect(ui.actionNew, &QAction::triggered, this, &NplCadWindow::newFile);
	openFile();

	mTimer = new QTimer(this);
	connect(mTimer, SIGNAL(timeout()), this, SLOT(update()));
	mTimer->start(200);
}

NplCadWindow::~NplCadWindow() {
	delete mTimer;
}
void NplCadWindow::update()
{
	mQParaEngineApp.Update();
}
bool NplCadWindow::load_stl(const QString& filename, bool is_reload /*= false*/)
{

	

	Loader* loader = new Loader(this, filename, is_reload);
	connect(loader, &Loader::got_mesh,
		m_GLView, &GLView::loadMesh);
	//connect(loader, &Loader::finished,
	//	loader, &Loader::deleteLater);
	//connect(loader, &Loader::finished,
	//	m_GLView, &GLView::clear_status);
	loader->start();
	return true;
}

void NplCadWindow::newFile()
{
	QString error;
	Document *document = Document::load("", &error);
	if (!document) {
		QMessageBox::critical(this, tr("Error Opening Map"), error);
		return;
	}
	mDocumentManager->addDocument(document);
	mQParaEngineApp.Test();
}
void NplCadWindow::openFile()
{
	load_stl(":res/gl/sphere.stl");
}
