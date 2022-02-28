// 2020 Baltazarus

#ifndef APP_CLASS
#define APP_CLASS
#include "Utilities.h"
#include "resource.h"

class Application
{
private:
	class WClass
	{
	private:
		WClass();
		~WClass();
		static WClass WCInstance;
		static constexpr const char* WClassName = LP_CLASS_NAME;
		HINSTANCE w_Inst;

	public:
		static HINSTANCE GetInstance() noexcept;
		static const char* GetWClassName() noexcept;
	};

public:
	Application(HWND w_Parent, const char* Caption, WTransform w_Transform, UINT Id);
	Application(HWND w_Parent, const char* Caption, int X, int Y, int Width, int Height, UINT Id);
	~Application();

	static LRESULT __stdcall WndProcSetup(HWND w_Handle, UINT Msg, WPARAM wParam, LPARAM lParam);
	static LRESULT __stdcall Thunk(HWND w_Handle, UINT Msg, WPARAM wParam, LPARAM lParam);
	LRESULT __stdcall WndProc(HWND w_Handle, UINT Msg, WPARAM wParam, LPARAM lParam);

	static LRESULT __stdcall DlgProc_InsertItem(HWND w_Dlg, UINT Msg, WPARAM wParam, LPARAM lParam);
	static LRESULT __stdcall DlgProc_About(HWND w_Dlg, UINT Msg, WPARAM wParam, LPARAM lParam);
	
	HWND GetHandle() const noexcept;

	WTransform GetWindowTransform() const noexcept;
	std::pair<int, int> GetWindowScale() const noexcept;
	std::pair<int, int> GetWindowPosition() const noexcept;
	
	void RunMessageLoop();

private:
	HWND w_Handle;
	WTransform w_Transform;
};

#endif
