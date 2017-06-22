#include "ParaEngineView.h"

ParaEngineView::ParaEngineView(QWidget * parent) : QWidget(parent) {
	QPalette pal = palette();
	// set black background
	pal.setColor(QPalette::Background, Qt::blue);
	this->setAutoFillBackground(true);
	this->setPalette(pal);
}

ParaEngineView::~ParaEngineView() {
	
}
