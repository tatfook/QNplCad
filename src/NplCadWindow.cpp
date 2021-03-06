﻿#include "NplCadWindow.h"
#include <QFileDialog>
#include <QVector3D>
#include <QMatrix4x4>

#include "renderer/Loader.h"
#include "documents/DocumentManager.h"
#include "documents/Document.h"
#include "NplCadGlobal.h"
namespace QNplCad
{
	NplCadWindow::NplCadWindow(QWidget * parent)
		: QMainWindow(parent)
		, m_documentManager(DocumentManager::instance())
		, m_doc(nullptr)
		, m_settings(nullptr)
	{
		NplCadGlobal::setAppRoot(QDir::currentPath());

		ui.setupUi(this);
		setCentralWidget(m_documentManager->widget());
		//3d view
		QGLFormat format;
		m_glView = new GLView(format, ui.Dock3D);
		ui.Dock3D->setWidget(m_glView);
		
		//log view
		m_logView = new LogView(ui.DockLog);
		ui.DockLog->setWidget(m_logView);

		m_settings = new QSettings(".qnplcad.ini", QSettings::IniFormat, this);

		// Connect slots.
		connect(ui.actionNew, &QAction::triggered, this, &NplCadWindow::newFile);
		connect(ui.actionOpen, &QAction::triggered, this, &NplCadWindow::openFile);
		connect(ui.actionClear, &QAction::triggered, this, &NplCadWindow::clearRecentFiles);
		connect(ui.actionSave, &QAction::triggered, this, &NplCadWindow::saveFile);
		connect(ui.actionSave_As, &QAction::triggered, this, &NplCadWindow::saveFileAs);
		connect(ui.actionSave_All, &QAction::triggered, this, &NplCadWindow::saveAll);
		connect(ui.actionExport, &QAction::triggered, this, &NplCadWindow::exportFile);
		connect(ui.actionExport_As, &QAction::triggered, this, &NplCadWindow::exportFileAs);
		connect(ui.actionReload, &QAction::triggered, this, &NplCadWindow::reloadFile);
		connect(ui.actionClose, &QAction::triggered, this, &NplCadWindow::closeFile);
		connect(ui.actionClose_All, &QAction::triggered, this, &NplCadWindow::closeAllFiles);
		connect(ui.actionQuit, &QAction::triggered, this, &NplCadWindow::quit);
		connect(ui.actionBuild, &QAction::triggered, this, &NplCadWindow::build);
		// Shortcuts.
		ui.actionNew->setShortcuts(QKeySequence::New);
		ui.actionOpen->setShortcuts(QKeySequence::Open);
		ui.actionSave->setShortcuts(QKeySequence::Save);
		ui.actionSave_As->setShortcuts(QKeySequence::SaveAs);
		ui.actionSave_All->setShortcut(QKeySequence(tr("Ctrl+Shift+S")));
		// Export
		// Export_As
		ui.actionReload->setShortcut(QKeySequence(tr("Ctrl+R")));
		ui.actionClose->setShortcut(QKeySequence(tr("Ctrl+W")));
		// Close_All
		ui.actionQuit->setShortcut(QKeySequence(tr("Ctrl+Q")));
		// Build
		ui.actionBuild->setShortcut(QKeySequence(tr("Ctrl+F5")));

		connect(m_documentManager, SIGNAL(currentDocumentChanged(Document*)),
			SLOT(documentChanged(Document*)));
		connect(m_documentManager, SIGNAL(documentCloseRequested(int)),
			this, SLOT(closeDocument(int)));

		// Add recent file actions to the recent files menu.
		for (int i = 0; i < MaxRecentFiles; ++i)
		{
			m_recentFiles[i] = new QAction(this);ui.menuRecent_Files->insertAction(ui.actionClear,m_recentFiles[i]);
			m_recentFiles[i]->setVisible(false);
			connect(m_recentFiles[i], SIGNAL(triggered()),this, SLOT(openRecentFile()));
		}
		ui.menuRecent_Files->insertSeparator(ui.actionClear);

		// Start paraengine app.
		startParaEngineApp();
		updateActions();

	}

	NplCadWindow::~NplCadWindow() {
		delete m_timer;
		delete m_settings;
	}
	void NplCadWindow::update()
	{
		NplCadGlobal::refresh();
	}

	void NplCadWindow::onNplMsg(const QJsonObject& msg)
	{
		QString type = msg["type"].toString();
		QJsonArray test = msg["test"].toArray();
		if (type == "qnplcad_output")
		{
			auto data = msg["data"].toObject();
			{
				auto csg_node_values = data["csg_node_values"].toArray();
				auto log = data["log"].toString();
				auto successful = data["successful"].toBool();
				auto compile_error = data["compile_error"].toString();
				m_logView->append(log);
				if (!successful)
				{
					m_logView->append(compile_error);
				}
				for (int i = 0; i< csg_node_values.count(); i++)
				{
					auto node = csg_node_values[i].toObject();
					auto world_matrix = node["world_matrix"].toArray();
					auto vertices = node["vertices"].toArray();
					auto indices = node["indices"].toArray();
					auto normals = node["normals"].toArray();
					auto colors = node["colors"].toArray();
					QMatrix4x4 mt;
					if (!world_matrix.empty())
					{
						toMatrix(mt, &world_matrix);
					}
					std::vector<GLfloat> vertices_list;
					for (int j = 0; j < vertices.count(); j++)
					{
						if (vertices[j].isArray() && normals[j].isArray() && colors[j].isArray())
						{
							auto position_node = vertices[j].toArray();
							auto normal_node = normals[j].toArray();
							auto color_node = colors[j].toArray();

							auto x = position_node[0].toDouble();
							auto y = position_node[1].toDouble();
							auto z = position_node[2].toDouble();
							QVector3D v(x, y, z);
							if (!mt.isIdentity())
							{
								v = v * mt;
							}
							// Position
							vertices_list.push_back(v.x());
							vertices_list.push_back(v.y());
							vertices_list.push_back(v.z());
							// Normal
							vertices_list.push_back(normal_node[0].toDouble());
							vertices_list.push_back(normal_node[1].toDouble());
							vertices_list.push_back(normal_node[2].toDouble());
							// Color
							vertices_list.push_back(color_node[0].toDouble());
							vertices_list.push_back(color_node[1].toDouble());
							vertices_list.push_back(color_node[2].toDouble());
						}
					}
					std::vector<GLuint> indices_list;			
					unionJsonArrayInit(indices_list, indices);

					Mesh* mesh = new Mesh(vertices_list, indices_list);
					m_glView->loadMesh(mesh);
				}
			}
		}
	}

	void NplCadWindow::build()
	{
		Document* doc = m_documentManager->currentDocument();
		if (doc)
		{
			QString filename = doc->fileName();
			filename = filename.replace(NplCadGlobal::getAppRoot(), "");
			QString msg = QString("msg = { build = true, filename = \"%1\"}").arg(filename);
			m_paraEngineApp.Call("", msg.toStdString());
		}
	}

	void NplCadWindow::startParaEngineApp(std::string cmdline)
	{
		if (cmdline.empty())
		{
			cmdline = "bootstrapper=\"script/QNplCad/main.lua\" loadpackage=\"npl_packages/NplCadLibrary/,;npl_packages/STLExporter/\"";
		}
		m_paraEngineApp.Run(cmdline.c_str());

		// Create a timer to receive message from paraengine in ui thread.
		m_timer = new QTimer(this);
		connect(m_timer, SIGNAL(timeout()), this, SLOT(update()));
		m_timer->start(200);
		m_paraEngineApp.StartLoop();
		// The entrance of message from paraengine.
		m_paraEngineApp.callback = std::bind(&NplCadWindow::onNplMsg, this, std::placeholders::_1);
	}

#pragma region Recent files
	void NplCadWindow::setRecentFile(const QString &fileName)
	{
		// Remember the file by its canonical file path
		const QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath();

		if (canonicalFilePath.isEmpty())
			return;

		QStringList files = recentFiles();
		files.removeAll(canonicalFilePath);
		files.prepend(canonicalFilePath);
		while (files.size() > MaxRecentFiles)
			files.removeLast();

		m_settings->beginGroup(QLatin1String("recentFiles"));
		m_settings->setValue(QLatin1String("fileNames"), files);
		m_settings->endGroup();
		updateRecentFiles();
	}

	void NplCadWindow::updateRecentFiles()
	{
		QStringList files = recentFiles();
		const int numRecentFiles = qMin(files.size(), (int)MaxRecentFiles);

		for (int i = 0; i < numRecentFiles; ++i)
		{
			m_recentFiles[i]->setText(QFileInfo(files[i]).fileName());
			m_recentFiles[i]->setData(files[i]);
			m_recentFiles[i]->setVisible(true);
		}
		for (int j = numRecentFiles; j < MaxRecentFiles; ++j)
		{
			m_recentFiles[j]->setVisible(false);
		}
		ui.menuRecent_Files->setEnabled(numRecentFiles > 0);
	}

	QStringList NplCadWindow::recentFiles() const
	{
		QVariant v = m_settings->value(QLatin1String("recentFiles/fileNames"));
		return v.toStringList();
	}

	void NplCadWindow::writeSettings()
	{
		
	}

	void NplCadWindow::readSettings()
	{

	}
#pragma endregion
#pragma region slots of files
	void NplCadWindow::newFile()
	{
		Document *document = new Document();
		addDocument(document);
	}
	void NplCadWindow::openFile()
	{
		//load_stl(":res/gl/sphere.stl");
		QString filter = tr("All Files (*)");
		filter += QLatin1String(";;");

		QString selectedFilter = tr("npl files (*.lua *.npl)");
		filter += selectedFilter;

		filter += QLatin1String(";;");
		filter += tr("lua files (*.lua)");

		filter += QLatin1String(";;");
		filter += tr("npl files (*.npl)");


		selectedFilter = m_settings->value(QLatin1String("lastUsedOpenFilter"),
			selectedFilter).toString();

		QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("Open"),
			fileDialogStartLocation(),
			filter, &selectedFilter);
		if (fileNames.isEmpty())
			return;

		m_settings->setValue(QLatin1String("lastUsedOpenFilter"), selectedFilter);
		foreach(const QString &fileName, fileNames)
			open(fileName);
	}

	bool NplCadWindow::open(const QString &fileName)
	{
		if (fileName.isEmpty() || fileName.isNull())
		{
			return false;
		}
		if (m_documentManager->findDocument(fileName) != -1)
		{
			return false;
		}
		QString error;
		Document *document = new Document();
		bool result = document->loadFile(fileName, &error);
		if (!result) {
			QMessageBox::critical(this, tr("Error Opening File"), error);
			return false;
		}
		addDocument(document);
		setRecentFile(fileName);
		return true;
	}

	bool NplCadWindow::saveFile()
	{
		if (!m_doc)
			return false;

		const QString currentFileName = m_doc->fileName();

		if (!save(currentFileName))
			return saveFileAs();
		return false;
	}

	bool NplCadWindow::save(const QString &fileName)
	{
		if (!m_doc)
			return false;

		if (fileName.isEmpty())
			return false;

		QString error;
		if (!m_doc->save(fileName, &error)) {
			QMessageBox::critical(this, tr("Error Saving File"), error);
			return false;
		}
		setRecentFile(fileName);
		return true;
	}

	QString NplCadWindow::fileDialogStartLocation() const
	{
		QStringList files = recentFiles();
		return (!files.isEmpty()) ? QFileInfo(files.first()).path() : QString();
	}

	void NplCadWindow::addDocument(Document *document)
	{
		m_documentManager->addDocument(document);
	}

	bool NplCadWindow::confirmSave(Document *document)
	{
		if (!document || !document->isModified())
			return true;

		m_documentManager->switchToDocument(document);

		int ret = QMessageBox::warning(
			this, tr("Unsaved Changes"),
			tr("There are unsaved changes. Do you want to save now?"),
			QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

		switch (ret) {
		case QMessageBox::Save:    return saveFile();
		case QMessageBox::Discard: return true;
		case QMessageBox::Cancel:
		default:
			return false;
		}
	}

	bool NplCadWindow::confirmAllSave()
	{
		for (int i = 0; i < m_documentManager->documentCount(); ++i) {
			if (!confirmSave(m_documentManager->documents().at(i)))
				return false;
		}

		return true;
	}

	void NplCadWindow::unionJsonArray(std::vector<GLfloat>& output, QJsonArray& input, QMatrix4x4* matrix)
	{
		for (int i = 0; i< input.count(); i++)
		{
			if (input[i].isArray())
			{
				auto obj = input[i].toArray();
				auto x = obj[0].toDouble();
				auto y = obj[1].toDouble();
				auto z = obj[2].toDouble();
				QVector3D v(x, y, z);
				if (matrix)
				{
					v = v * (*matrix);
				}
				output.push_back(v.x());
				output.push_back(v.y());
				output.push_back(v.z());
			}
			
		}
	}

	void NplCadWindow::unionJsonArrayInit(std::vector<GLuint>& output, QJsonArray& input)
	{
		for (int i = 0; i < input.count(); i++)
		{
			output.push_back(input[i].toInt() - 1);
		}
	}

	void NplCadWindow::toMatrix(QMatrix4x4& output, QJsonArray* input)
	{
		if (input)
		{
			auto m11 = input->at(0).toDouble();
			auto m12 = input->at(1).toDouble();
			auto m13 = input->at(2).toDouble();
			auto m14 = input->at(3).toDouble();

			auto m21 = input->at(4).toDouble();
			auto m22 = input->at(5).toDouble();
			auto m23 = input->at(6).toDouble();
			auto m24 = input->at(7).toDouble();

			auto m31 = input->at(8).toDouble();
			auto m32 = input->at(9).toDouble();
			auto m33 = input->at(10).toDouble();
			auto m34 = input->at(11).toDouble();

			auto m41 = input->at(12).toDouble();
			auto m42 = input->at(13).toDouble();
			auto m43 = input->at(14).toDouble();
			auto m44 = input->at(15).toDouble();

			QMatrix4x4 matrix(
				m11, m12, m13, m14,
				m21, m22, m23, m24,
				m31, m32, m33, m34,
				m41, m42, m43, m44
			);
			output = matrix;
		}
	}

	void NplCadWindow::updateWindowTitle()
	{
		if (m_doc) {
		setWindowTitle(tr("[*]%1").arg(m_doc->displayName()));
		setWindowFilePath(m_doc->fileName());
		bool modified = m_doc->isModified();
		setWindowModified(modified);
		}
		else {
		setWindowTitle(QApplication::applicationName());
		setWindowFilePath(QString());
		setWindowModified(false);
		}
	}

	bool NplCadWindow::saveFileAs()
	{
		const QString mcmlfilter = tr("npl files (*.lua *.npl)");
		QString filter = QString(mcmlfilter);


		QString selectedFilter = mcmlfilter;


		QString suggestedFileName;
		if (m_doc && !m_doc->fileName().isEmpty()) {
			suggestedFileName = m_doc->fileName();
		}
		else {
			suggestedFileName = fileDialogStartLocation();
			suggestedFileName += QLatin1Char('/');
			suggestedFileName += tr("untitled.lua");
		}

		const QString fileName =
			QFileDialog::getSaveFileName(this, QString(), suggestedFileName,
				filter, &selectedFilter);

		if (fileName.isEmpty())
			return false;
		if (m_documentManager->findDocument(fileName) != -1)
		{
			QMessageBox::warning(this, tr("Save Error"),tr("This document has been opened.\n""Please change another location to save."),QMessageBox::Ok);
			return false;
		}
		return save(fileName);
	}

	void NplCadWindow::saveAll()
	{

	}

	void NplCadWindow::exportFile()
	{

	}

	void NplCadWindow::exportFileAs()
	{

	}

	void NplCadWindow::reloadFile()
	{

	}

	void NplCadWindow::closeFile()
	{
		if (confirmSave(m_documentManager->currentDocument()))
			m_documentManager->closeCurrentDocument();
	}

	void NplCadWindow::closeAllFiles()
	{
		if (confirmAllSave())
			m_documentManager->closeAllDocuments();
	}

	void NplCadWindow::quit()
	{
		if (confirmAllSave())
		{
			m_documentManager->closeAllDocuments();
			close();
		}
	}

	void NplCadWindow::openRecentFile()
	{
		QAction *action = qobject_cast<QAction *>(sender());
		if (action)
			open(action->data().toString());
	}

	void NplCadWindow::clearRecentFiles()
	{
		m_settings->beginGroup(QLatin1String("recentFiles"));
		m_settings->setValue(QLatin1String("fileNames"), QStringList());
		m_settings->endGroup();
		updateRecentFiles();
	}

	void NplCadWindow::updateActions()
	{
		ui.actionSave->setEnabled(m_doc);
		ui.actionSave_As->setEnabled(m_doc);
		ui.actionSave_All->setEnabled(m_doc);
		ui.actionExport->setEnabled(m_doc);
		ui.actionExport_As->setEnabled(m_doc);
		ui.actionReload->setEnabled(m_doc);
		ui.actionClose->setEnabled(m_doc);
		ui.actionClose_All->setEnabled(m_doc);
		ui.actionBuild->setEnabled(m_doc);

		updateRecentFiles();
	}

	void NplCadWindow::documentChanged(Document *doc)
	{
		if (doc)
			doc->disconnect(this);
		m_doc = doc;
		if (m_doc)
		{
			connect(m_doc, SIGNAL(fileNameChanged(QString, QString)), SLOT(updateWindowTitle()));
			connect(m_doc, SIGNAL(textChanged()), SLOT(updateWindowTitle()));
			connect(m_doc, SIGNAL(saved()), SLOT(updateWindowTitle()));
		}
		updateWindowTitle();
		updateActions();
	}

	void NplCadWindow::closeDocument(int index)
	{
		if (confirmSave(m_documentManager->documents().at(index)))
			m_documentManager->closeDocumentAt(index);
	}

#pragma endregion

}
