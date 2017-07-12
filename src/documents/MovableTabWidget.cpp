#include "MovableTabWidget.h"
#include <QTabBar>
using namespace QNplCad;

MovableTabWidget::MovableTabWidget(QWidget * parent) : QTabWidget(parent) {

	setMovable(true);
	connect(tabBar(), SIGNAL(tabMoved(int, int)),
		SIGNAL(tabMoved(int, int)));
}

MovableTabWidget::~MovableTabWidget() {
	
}
void MovableTabWidget::moveTab(int from, int to)
{
	tabBar()->moveTab(from, to);
}
