#pragma once
#include <functional>
#include <QString>
namespace QNplCad
{
	namespace NplCadGlobal
	{
		void refresh();
		void performFunctionInUIThread(const std::function<void()> &function);
		const QString& getAppRoot();
		void setAppRoot(QString s);
	};


}
