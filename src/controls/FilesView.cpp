#include "FilesView.h"
using namespace QNplCad;

FilesView::FilesView(QWidget * parent) : QWidget(parent) {
	QPalette pal = palette();
	// set black background
	pal.setColor(QPalette::Background, Qt::darkYellow);
	this->setAutoFillBackground(true);
	this->setPalette(pal);
}

FilesView::~FilesView() {
	
}
