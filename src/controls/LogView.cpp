#include "LogView.h"

LogView::LogView(QWidget * parent) : QWidget(parent) {
	QPalette pal = palette();
	// set black background
	pal.setColor(QPalette::Background, Qt::green);
	this->setAutoFillBackground(true);
	this->setPalette(pal);
}

LogView::~LogView() {
	
}
