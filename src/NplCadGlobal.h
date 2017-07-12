#pragma once
#include <functional>
namespace QNplCad
{
	namespace NplCadGlobal
	{
		void refresh();
		void performFunctionInUIThread(const std::function<void()> &function);
	};
}
