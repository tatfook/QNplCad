namespace MyCompany
{
	class CMyApp
	{
	public:
		CMyApp(HINSTANCE hInst = NULL);
		~CMyApp();

		/** load the ParaEngine plug in dll if not.
		* @return true if loaded.
		*/
		bool CheckLoad();

		/** run the application */
		int Run(HINSTANCE hInst, const char* lpCmdLine);

		void RegisterNPL_API();

	private:
		ParaEngine::CPluginLoader m_ParaEngine_plugin;
		ParaEngine::IParaEngineApp * m_pParaEngineApp;
		ParaEngine::IParaEngineCore * m_pParaEngine;
		HINSTANCE m_hInst;
	};
}