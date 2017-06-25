#include "PEtypes.h"
#include "IParaEngineApp.h"
#include "IParaEngineCore.h"
#include "INPL.h"
#include "INPLRuntime.h"
#include "INPLRuntimeState.h"
#include "INPLAcitvationFile.h"
#include "NPLInterface.hpp"
#include "PluginLoader.hpp"

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
	int Run(const char* lpCmdLine);
	void Test();
	public:
	ParaEngine::CPluginLoader m_ParaEngine_plugin;
	ParaEngine::IParaEngineApp * m_pParaEngineApp;
	ParaEngine::IParaEngineCore * m_pParaEngine;
};
