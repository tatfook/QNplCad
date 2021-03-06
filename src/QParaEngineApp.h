#pragma once
#include "PEtypes.h"
#include "IParaEngineApp.h"
#include "IParaEngineCore.h"
#include "INPL.h"
#include "INPLRuntime.h"
#include "INPLRuntimeState.h"
#include "INPLAcitvationFile.h"
#include "NPLInterface.hpp"
#include "PluginLoader.hpp"
#include <functional>
#include <QJsonObject>
#include <string>
namespace QNplCad
{
	class QParaEngineApp
	{
	public:
		QParaEngineApp();
		~QParaEngineApp();

		/** load the ParaEngine plug in dll if not.
		* @return true if loaded.
		*/
		bool CheckLoad();
		void RegisterNPL_API();
		// start ParaEngine through a command line.
		int Run(const char* lpCmdLine);
		// start the main loop of ParaEngine in other thread.
		void StartLoop();
		// terminate the main loop.
		void StopLoop();
		void Update();
		// Call npl 
		void Call(std::string filename, std::string msg);
	public:
		ParaEngine::CPluginLoader m_ParaEngine_plugin;
		ParaEngine::IParaEngineApp * m_pParaEngineApp;
		ParaEngine::IParaEngineCore * m_pParaEngine;
		typedef std::function<void(const QJsonObject&  msg)> NplCallback;
		NplCallback callback;
	private:
		bool m_is_looping;
	};
}
