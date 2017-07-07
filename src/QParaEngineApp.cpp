#include "QParaEngineApp.h"
using namespace ParaEngine;
using namespace NPL;

QParaEngineApp::QParaEngineApp() : m_pParaEngine(NULL), m_pParaEngineApp(NULL)
{

}

QParaEngineApp::~QParaEngineApp()
{
	if (m_pParaEngineApp){
		m_pParaEngineApp->StopApp();
	}
}

bool QParaEngineApp::CheckLoad()
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

int QParaEngineApp::Run(const char* lpCmdLine)
{
	if (!CheckLoad())
		return E_FAIL;

	m_pParaEngineApp = m_pParaEngine->CreateApp();
	if (m_pParaEngineApp == 0)
		return E_FAIL;

	if (m_pParaEngineApp->StartApp(lpCmdLine) != S_OK)
		return E_FAIL;

	RegisterNPL_API();
	return 0;
}

void QParaEngineApp::RegisterNPL_API()
{
	class CMyAppAPI : public INPLActivationFile
	{
	public:
		CMyAppAPI(IParaEngineApp* pApp) :m_pApp(pApp){}
		virtual NPLReturnCode OnActivate(INPLRuntimeState* pState)
		{
			auto msg = NPLInterface::NPLHelper::MsgStringToNPLTable(pState->GetCurrentMsg());
			std::string sType = msg["type"];
			if (sType == "nplcad")
			{
				int i = 0;
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

void QParaEngineApp::Test()
{
	auto pNPLRuntime = m_pParaEngineApp->GetNPLRuntime();
	auto pMainState = pNPLRuntime->GetMainState();
	std::string msg = "msg = { start = true}";
	pMainState->activate("test/main.lua",msg.c_str());
}

void QParaEngineApp::Update()
{
	m_pParaEngineApp->DoWork();
}

