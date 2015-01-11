#pragma once
#include <Config/OSHeaders.h>
#include <memory>

namespace k3d {
	
	class AppBase {
	public:

		AppBase();

		void SetEngine(class Engine * engine) {
			pEngine = engine;
		}

		void StartMessageLooping();

#if K3DPLATFORM_OS_WIN
		struct WinMsg {
			HWND	hwnd;
			UINT32	msg;
			WPARAM	wParam;
			LPARAM	lParam;
		};
		
		static LRESULT CALLBACK AppWndProc(HWND hwnd, UINT32 msg, WPARAM wParam, LPARAM lParam);
		
		static std::unique_ptr<AppBase> & CreateApplication(const HINSTANCE instance, const HICON icon);

	protected:
		
		LRESULT processWinMessage(WinMsg & msg);
		
		AppBase(const HINSTANCE instance, const HICON icon);
		
		bool InitClass(WNDCLASSEX & wndClass);
#endif
		Engine * pEngine;
	
	private:

	};

}