#pragma once

#include <Windows.h>

class AppWindow;

//Define OGLApplication as a singleton
class TestApplication
{
	private:
		AppWindow*					m_appwnd;
		
		BOOL						m_terminate;
		static TestApplication*		s_oglapp;

									TestApplication();
		virtual						~TestApplication();
		void						CreateApplicationWindow(int width, int height );
		BOOL						MyRegisterClass(HINSTANCE hinst);

	public:

		HINSTANCE					m_hInst;

		static TestApplication*		CreateApplication(HINSTANCE hInst);
		static void					DestroyApplication();
		static TestApplication*		GetApplication();

		
		int							Run();
		void						Kill();
		

		inline AppWindow*			GetApplicationWindow()
		{
			return m_appwnd;
		}

		static	LRESULT CALLBACK	WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
};
