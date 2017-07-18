#include "NplCadGlobal.h"
#include <mutex>
#include <vector>
namespace QNplCad
{
	namespace NplCadGlobal
	{
		std::vector<std::function<void()>> _functionsToPerform;
		std::mutex _performMutex;
		QString app_root;
		void refresh()
		{
			if (!_functionsToPerform.empty()) {
				_performMutex.lock();
				// fixed #4123: Save the callback functions, they must be invoked after '_performMutex.unlock()', otherwise if new functions are added in callback, it will cause thread deadlock.
				auto temp = _functionsToPerform;
				_functionsToPerform.clear();
				_performMutex.unlock();
				for (const auto &function : temp) {
					function();
				}
			}
		}

		void performFunctionInUIThread(const std::function<void()> &function)
		{
			_performMutex.lock();

			_functionsToPerform.push_back(function);

			_performMutex.unlock();
		}

		const QString& getAppRoot()
		{
			return app_root;
		}

		void setAppRoot(QString s)
		{
			app_root = s;
		}

	}
}
