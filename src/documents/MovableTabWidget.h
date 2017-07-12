#pragma once
#include <QTabWidget>
namespace QNplCad
{
	class MovableTabWidget : public QTabWidget {
		Q_OBJECT

	public:
		MovableTabWidget(QWidget * parent = Q_NULLPTR);
		~MovableTabWidget();
		void moveTab(int from, int to);
	signals:
		/**
		* Emitted when a tab is moved from index position \a from to
		* index position \a to.
		*/
		void tabMoved(int from, int to);

	};
}
