#pragma once
#include <QWidget>
#include <QTextEdit>
namespace QNplCad
{
	class LogView : public QWidget {
		Q_OBJECT

	public:
		LogView(QWidget * parent = Q_NULLPTR);
		~LogView();
		void append(QString log);
	private:
		QTextEdit* m_textEdit;

	};
}
