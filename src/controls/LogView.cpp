#include "LogView.h"
#include <QVBoxLayout>

using namespace QNplCad;

LogView::LogView(QWidget * parent) 
	: QWidget(parent) 
	, m_textEdit(new QTextEdit)
{
	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setMargin(0);
	layout->setSpacing(0);
	layout->addWidget(m_textEdit);
}

LogView::~LogView() {
	delete m_textEdit;
}
void LogView::append(QString log)
{
	if (log.isEmpty())
	{
		return;
	}
	m_textEdit->append(log);
}
