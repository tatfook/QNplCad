#include <QApplication>

#include "PEtypes.h"
#include "IParaEngineApp.h"
#include "IParaEngineCore.h"
#include "INPL.h"
#include "INPLRuntime.h"
#include "INPLRuntimeState.h"
#include "INPLAcitvationFile.h"
#include "NPLInterface.hpp"
#include "PluginLoader.hpp"

#include "mainwindow.h"

#include "MyApp.h"

using namespace ParaEngine;
using namespace NPL;
using namespace MyCompany;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Retina display support for Mac OS, iOS and X11:
    // http://blog.qt.io/blog/2013/04/25/retina-display-support-for-mac-os-ios-and-x11/
    //
    // AA_UseHighDpiPixmaps attribute is off by default in Qt 5.1 but will most
    // likely be on by default in a future release of Qt.
    app.setAttribute(Qt::AA_UseHighDpiPixmaps);

	CMyApp myApp;
	myApp.Run(nullptr,"");
    MainWindow mainWindow;
    mainWindow.show();
    return app.exec();
}
