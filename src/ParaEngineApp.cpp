//-----------------------------------------------------------------------------
// Class:	ParaEngineApp 
// Authors:	LiXizhi
// Emails:	LiXizhi@yeah.net
// Company: ParaEngine Co.
// Date:	2015.5.12
// Desc: This exe must be run in the working directory of ParaEngineClient.dll
//-----------------------------------------------------------------------------
#include "PEtypes.h"
#include "IParaEngineApp.h"
#include "IParaEngineCore.h"
#include "INPL.h"
#include "INPLRuntime.h"
#include "INPLRuntimeState.h"
#include "INPLAcitvationFile.h"
#include "NPLInterface.hpp"
#include "PluginLoader.hpp"

#include "ParaEngineApp.h"

using namespace ParaEngine;
using namespace NPL;

ParaEngineApp::ParaEngineApp(HINSTANCE hInst /*= NULL*/) : m_pParaEngine(NULL), m_pParaEngineApp(NULL), m_hInst(hInst)
{

}

ParaEngineApp::~ParaEngineApp()
{
	if (m_pParaEngineApp){
		m_pParaEngineApp->StopApp();
	}
}

bool ParaEngineApp::CheckLoad()
{
	if (m_ParaEngine_plugin.IsValid())
	{
		return true;
	}

#ifdef _DEBUG
	// post_fix with _d
	m_ParaEngine_plugin.Init("ParaEngineClient_d.dll");
#else
	m_ParaEngine_plugin.Init("ParaEngineClient.dll");
#endif

	int nClassCount = m_ParaEngine_plugin.GetNumberOfClasses();
	for (int i = 0; i < nClassCount; ++i)
	{
		ClassDescriptor* pDesc = m_ParaEngine_plugin.GetClassDescriptor(i);
		if (pDesc)
		{
			if (strcmp(pDesc->ClassName(), "ParaEngine") == 0)
			{
				m_pParaEngine = (ParaEngine::IParaEngineCore*)(pDesc->Create());
			}
		}
	}
	return m_ParaEngine_plugin.IsValid();
}

int ParaEngineApp::Run(HINSTANCE hInst, const char* lpCmdLine)
{
	if (!CheckLoad())
		return E_FAIL;

	m_pParaEngineApp = m_pParaEngine->CreateApp();
	if (m_pParaEngineApp == 0)
		return E_FAIL;

	if (m_pParaEngineApp->StartApp(lpCmdLine) != S_OK)
		return E_FAIL;

	RegisterNPL_API();

	// Set Frame Rate
	//m_pParaEngineApp->SetRefreshTimer(1/45.f, 0);
	m_pParaEngineApp->SetRefreshTimer(1 / 30.f, 0);

	//m_pParaEngineApp->Enable3DRendering(false);
	// Run to end
	return m_pParaEngineApp->Run(hInst);
}

void ParaEngineApp::RegisterNPL_API()
{
	/* example of registering C++ file. In NPL script, call
	   NPL.activate("MyApp.cpp", {type="SetIcon"});
	*/
	class CMyAppAPI : public INPLActivationFile
	{
	public:
		CMyAppAPI(IParaEngineApp* pApp) :m_pApp(pApp){}
		virtual NPLReturnCode OnActivate(INPLRuntimeState* pState)
		{
			auto msg = NPLInterface::NPLHelper::MsgStringToNPLTable(pState->GetCurrentMsg(), pState->GetCurrentMsgLength());
			std::string sType = msg["type"];
			if (sType == "SetIcon")
			{
				// example of changing application icon. 
				HICON hIcon = LoadIcon(NULL, IDI_WARNING);
				SendMessage(m_pApp->GetMainWindow(), WM_SETICON, ICON_BIG, (LPARAM)hIcon);
			}
			return NPLReturnCode::NPL_OK;
		};
	protected:
		IParaEngineApp* m_pApp;
	};

	auto pNPLRuntime = m_pParaEngineApp->GetNPLRuntime();
	if (pNPLRuntime)
	{
		// main NPL thread
		auto pMainState = pNPLRuntime->GetMainState();
		if (pMainState)
		{
			pMainState->RegisterFile("NplCad_ParaEngineApp.cpp", new CMyAppAPI(m_pParaEngineApp));
		}
	}
}


