// 2020 Baltazarus

#include "Application.h"

#define ID_LISTBOX_MAIN_LIST		2011
#define ID_EDIT_MAIN_INPUT_EDIT		2012
#define ID_BUTTON_SELECT_RANDOM		2013
#define ID_BUTTON_INSERT_ITEM		2014
#define ID_BUTTON_DELETE_ITEM		2015

Application::WClass Application::WClass::WCInstance;

void InitUI(HWND w_Handle, HINSTANCE w_Inst);
std::vector<std::string> GetItemsFromFile(const char* path, HWND w_Handle);

static HWND w_MainList = nullptr;
static HWND w_MainInputEdit = nullptr;
static HWND w_ButtonRandomSelect = nullptr;
static HWND w_ButtonInsertItem = nullptr;
static HWND w_ButtonDeleteItem = nullptr;

Application::WClass::WClass()
{
	WNDCLASSEXA wcex = { 0 };

	memset(&wcex, 0, sizeof(wcex));
	wcex.cbSize = sizeof(wcex);
	wcex.style = CS_OWNDC;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.lpfnWndProc = &Application::WndProcSetup;
	wcex.hInstance = Application::WClass::GetInstance();
	wcex.hCursor = LoadCursor(Application::WClass::GetInstance(), IDC_ARROW);
	wcex.hIcon = LoadIcon(Application::WClass::GetInstance(), IDI_APPLICATION);
	wcex.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
	wcex.lpszClassName = Application::WClass::GetWClassName();
	wcex.lpszMenuName = MAKEINTRESOURCEA(IDR_MENUBAR);
	wcex.hIconSm = LoadIcon(this->GetInstance(), IDI_APPLICATION);

	if (!RegisterClassExA(&wcex))
		MessageBoxA(0, "Cannot Register Window Class!", "Error!", MB_OK | MB_ICONEXCLAMATION);
}

Application::WClass::~WClass()
{
	UnregisterClassA(this->GetWClassName(), this->GetInstance());
}

HINSTANCE Application::WClass::GetInstance() noexcept
{
	return WCInstance.w_Inst;
}

const char* Application::WClass::GetWClassName() noexcept
{
	return WCInstance.WClassName;
}

Application::Application(HWND w_Parent, const char* Caption, WTransform w_Transform, UINT Id = 10U) : w_Transform(w_Transform)
{
	w_Handle = CreateWindowExA(
		WS_EX_CLIENTEDGE,
		Application::WClass::GetWClassName(),
		Caption,
		WS_OVERLAPPEDWINDOW,
		w_Transform.X, w_Transform.Y, w_Transform.Width, w_Transform.Height,
		w_Parent, nullptr, Application::WClass::GetInstance(), this
	);

	ShowWindow(this->GetHandle(), SW_SHOWDEFAULT);
}

Application::Application(HWND w_Parent, const char* Caption, int X, int Y, int Width, int Height, UINT Id = 10U)
	: Application(w_Parent, Caption, { X, Y, Width, Height }, Id)
{ }

Application::~Application()
{
	DestroyWindow(this->GetHandle());
}

LRESULT __stdcall Application::WndProcSetup(HWND w_Handle, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	if (Msg == WM_NCCREATE)
	{
		const CREATESTRUCTA* const w_Create = reinterpret_cast<CREATESTRUCTA*>(lParam);
		Application* const w_App = reinterpret_cast<Application*>(w_Create->lpCreateParams);
		SetWindowLongPtrA(w_Handle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(w_App));
		SetWindowLongPtrA(w_Handle, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Application::Thunk));
		return w_App->WndProc(w_Handle, Msg, wParam, lParam);
	}
	return DefWindowProcA(w_Handle, Msg, wParam, lParam);
}

LRESULT __stdcall Application::Thunk(HWND w_Handle, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	Application* const w_App = reinterpret_cast<Application*>(GetWindowLongPtrA(w_Handle, GWLP_WNDPROC));
	return w_App->WndProc(w_Handle, Msg, wParam, lParam);
}

LRESULT __stdcall Application::WndProc(HWND w_Handle, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch (Msg)
	{
		case WM_CREATE:
		{
			InitUI(w_Handle, Application::WClass::GetInstance());
			break;
		}
		case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
				case ID_BUTTON_INSERT_ITEM:
				{
					try
					{
						wchar_t* c_buffer = nullptr;
						int text_length = SendMessageW(w_MainInputEdit, WM_GETTEXTLENGTH, 0u, 0u) + 1;
						
						// (text_length < 1), but text_length must have value + 1 because of buffer.
						if (text_length < 2)
						{
							MessageBoxA(w_Handle, "Specify at least 1 character.", "Note", MB_OK);
							break;
						}

						c_buffer = new wchar_t[text_length];
						if (c_buffer == nullptr)
							throw "[0x0002] Cannot allocate needed memory buffer!";

						SendMessageW(
							w_MainInputEdit, WM_GETTEXT, 
							static_cast<WPARAM>(text_length), 
							reinterpret_cast<LPARAM>(c_buffer)
						);
						SendMessageW(w_MainList, LB_ADDSTRING, 0u, reinterpret_cast<LPARAM>(c_buffer));
						SendMessageW(w_MainInputEdit, WM_SETTEXT, 0u, 0u);
						delete[] c_buffer;
					}
					catch (const std::bad_alloc& e)
					{
						MessageBoxA(w_Handle, e.what(), "Exception Thrown!", MB_OK | MB_ICONERROR);
						return -1;
					}
					catch (const char* e)
					{
						MessageBoxA(w_Handle, e, "Exception Thrown!", MB_OK | MB_ICONERROR);
						return -1;
					}
					break;
				}
				case ID_BUTTON_SELECT_RANDOM:
				{
					srand(time(nullptr));
					try
					{
						wchar_t* c_buffer = nullptr;
						int item_count = SendMessageW(w_MainList, LB_GETCOUNT, 0u, 0u);
						if (item_count < 1)
						{
							MessageBoxA(w_Handle, "Enter at least 1 item.", "Alert", MB_OK);
							break;
						}
						int random_choice_index = (rand() % item_count);
						int item_tlen = SendMessageW(w_MainList, LB_GETTEXTLEN, static_cast<WPARAM>(random_choice_index), 0u);
						c_buffer = new wchar_t[item_tlen + 1];
						SendMessageW(
							w_MainList, LB_GETTEXT, 
							static_cast<WPARAM>(random_choice_index), 
							reinterpret_cast<LPARAM>(c_buffer)
						);
						MessageBoxW(w_Handle, c_buffer, L"Picked Item!", MB_OK | MB_ICONINFORMATION);
						delete[] c_buffer;
					}
					catch (const std::bad_alloc& e)
					{
						MessageBoxA(w_Handle, e.what(), "Exception Thrown!", MB_OK | MB_ICONERROR);
						return -1;
					}
					catch (const char* e)
					{
						MessageBoxA(w_Handle, e, "Exception Thrown!", MB_OK | MB_ICONERROR);
						return -1;
					}
					break;
				}
				case ID_BUTTON_DELETE_ITEM:
				{
					int item_count = SendMessageW(w_MainList, LB_GETCOUNT, 0u, 0u);
					if (item_count < 1)
					{
						MessageBoxA(w_Handle, "Nothing to select, nothing to delete...", "Alert", MB_OK);
						break;
					}
					
					int selected_count = 0;

					for (int i = 0; i < item_count; ++i)
					{
						int sel_index = SendMessageW(w_MainList, LB_GETSEL, static_cast<WPARAM>(i), 0u);
						selected_count += (sel_index > 0) ? 1 : 0;

						if (sel_index > 0)
							SendMessageW(w_MainList, LB_DELETESTRING, static_cast<WPARAM>(i), 0u);
					}
					if ((selected_count > 0) != true)
						MessageBoxA(w_Handle, "Select at least one item to delete.", "Alert", MB_OK);
					break;
				}
				case ID_ITEM_INSERT:
				{
					DialogBoxW(
						Application::WClass::GetInstance(),
						MAKEINTRESOURCEW(IDD_INSERT_ITEM),
						w_Handle,
						reinterpret_cast<DLGPROC>(&Application::DlgProc_InsertItem)
					);
					break;
				}
				case ID_ITEM_CLEARLIST:
				{
					SendMessageW(w_MainList, LB_RESETCONTENT, 0u, 0u);
					break;
				}
				case ID_ITEM_EXIT:
				{
					DestroyWindow(w_Handle);
					break;
				}
				case ID_ITEM_LOAD_FROM_FILE:
				{
					char* path = new char[MAX_PATH];
					OPENFILENAMEA ofn;
					memset(&ofn, 0, sizeof(ofn));
					ofn.lStructSize = sizeof(ofn);
					ofn.hwndOwner = w_Handle;
					ofn.lpstrFilter = "Text File\0*.txt\0All Files\0*.*";
					ofn.nFilterIndex = 1;
					ofn.lpstrFile = path;
					ofn.lpstrFile[0] = '\0';
					ofn.lpstrTitle = "Open File";
					ofn.nMaxFile = MAX_PATH;
					ofn.Flags = (OFN_EXPLORER | OFN_PATHMUSTEXIST);

					GetOpenFileNameA(&ofn);
					
					std::vector<std::string> lines = ::GetItemsFromFile(path, this->GetHandle());
					delete[] path;

					if(lines.size() < 1ull)
						break;
					for(std::size_t i = 0ull; i < lines.size(); ++i)
						SendMessage(::w_MainList, LB_ADDSTRING, 0u, reinterpret_cast<LPARAM>(lines[i].c_str()));
					break;
				}
			}
			break;
		}
		case WM_SIZE:
		{
			RECT clRect;
			GetClientRect(w_Handle, &clRect);

			RECT lvRect;
			int lvHeight = 0;
			GetWindowRect(GetDlgItem(w_Handle, 2011), &lvRect);

			RECT eRect;
			int eHeight = 0;
			GetWindowRect(GetDlgItem(w_Handle, 2012), &eRect);

			RECT bRect;
			GetWindowRect(GetDlgItem(w_Handle, 2013), &bRect);
			eHeight = eRect.bottom - eRect.top;
			lvHeight = clRect.bottom - eHeight;

			//SendMessageW(w_MainList, WM_SIZE, 0u, 0u);
			MoveWindow(w_MainList, 0, 0, clRect.right, lvHeight, TRUE);
			MoveWindow(w_MainInputEdit, 0, lvHeight - 1, clRect.right - 150, clRect.bottom - lvHeight, TRUE);
			MoveWindow(w_ButtonInsertItem, clRect.right - 150, lvHeight - 2, 50, clRect.bottom - lvHeight + 2, TRUE);
			MoveWindow(w_ButtonRandomSelect, clRect.right - 100, lvHeight - 2, 50, clRect.bottom - lvHeight + 2, TRUE);
			MoveWindow(w_ButtonDeleteItem, clRect.right - 50, lvHeight - 2, 50, clRect.bottom - lvHeight + 2, TRUE);

			for(int i = 2011; i <= 2015; ++i)
				RedrawWindow(GetDlgItem(w_Handle, i), nullptr, nullptr, RDW_INVALIDATE);
			break;
		}
		case WM_CLOSE:
			DestroyWindow(w_Handle);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProcA(w_Handle, Msg, wParam, lParam);
	}
	return 0;
}

LRESULT __stdcall Application::DlgProc_InsertItem(HWND w_Dlg, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	HWND w_TextField = GetDlgItem(w_Dlg, IDC_EDIT_ITEMTEXT);

	switch (Msg)
	{
		case WM_INITDIALOG:
			break;
		case WM_COMMAND:
		{
			if (LOWORD(wParam) == IDOK)
			{
				wchar_t* c_buffer = nullptr;
				int text_length = GetWindowTextLengthW(w_TextField);
				if (text_length < 1)
				{
					MessageBoxA(w_Dlg, "Specify at least 1 character.", "Alert", MB_OK);
					break;
				}
				c_buffer = new wchar_t[text_length + 1];
				GetWindowTextW(w_TextField, c_buffer, text_length + 1);

				SendMessageW(
					GetDlgItem(GetParent(w_Dlg), ID_EDIT_MAIN_INPUT_EDIT), 
					WM_SETTEXT, 0u, 
					reinterpret_cast<LPARAM>(c_buffer)
				);

				delete[] c_buffer;
				EndDialog(w_Dlg, IDOK);

				SendMessageW(
					GetParent(w_Dlg), 
					WM_COMMAND, 
					static_cast<WPARAM>(ID_BUTTON_INSERT_ITEM), 
					reinterpret_cast<LPARAM>(w_ButtonInsertItem)
				);
			}
			if (LOWORD(wParam) == IDCANCEL)
				EndDialog(w_Dlg, IDCANCEL);
			break;
		}
	}
	return 0;
}

HWND Application::GetHandle() const noexcept
{
	return this->w_Handle;
}

WTransform Application::GetWindowTransform() const noexcept
{
	return this->w_Transform;
}

std::pair<int, int> Application::GetWindowScale() const noexcept
{
	return std::pair<int, int>(GetWindowTransform().Width, GetWindowTransform().Height);
}

std::pair<int, int> Application::GetWindowPosition() const noexcept
{
	return std::pair<int, int>(GetWindowTransform().X, GetWindowTransform().Y);
}

void Application::RunMessageLoop()
{
	MSG Msg = { };
	while (GetMessageA(&Msg, nullptr, 0, 0) > 0)
	{
		TranslateMessage(&Msg);
		DispatchMessageA(&Msg);
	}
	return;
}

void InitUI(HWND w_Handle, HINSTANCE w_Inst)
{
	DWORD wStyles = (WS_VISIBLE | WS_CHILD);

	w_MainList = CreateWindowW(
		WC_LISTBOXW, nullptr,
		wStyles | WS_BORDER | WS_VSCROLL | WS_HSCROLL,
		0, 0, 0, 0,
		w_Handle, reinterpret_cast<HMENU>(ID_LISTBOX_MAIN_LIST),
		nullptr, nullptr
	);

	w_MainInputEdit = CreateWindow(
		WC_EDIT, nullptr,
		wStyles | WS_BORDER,
		0, 0, 0, 30,
		w_Handle, reinterpret_cast<HMENU>(ID_EDIT_MAIN_INPUT_EDIT),
		nullptr, nullptr
	);

	w_ButtonRandomSelect = CreateWindowA(
		WC_BUTTONA, "Pick",
		wStyles,
		0, 0, 0, 30,
		w_Handle, reinterpret_cast<HMENU>(ID_BUTTON_SELECT_RANDOM),
		nullptr, nullptr
	);

	w_ButtonInsertItem = CreateWindowA(
		WC_BUTTONA, "Insert",
		wStyles,
		0, 0, 0, 30,
		w_Handle, reinterpret_cast<HMENU>(ID_BUTTON_INSERT_ITEM),
		nullptr, nullptr
	);

	w_ButtonDeleteItem = CreateWindowA(
		WC_BUTTONA, "Delete",
		wStyles,
		0, 0, 0, 30,
		w_Handle, reinterpret_cast<HMENU>(ID_BUTTON_DELETE_ITEM),
		nullptr, nullptr
	);

	HFONT hFont = CreateFontW(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, L"Arial");
	for (int i = 0; i <= 2015; ++i)
	{
		if((i == 2011) || (i == 2012))
			SendMessage(GetDlgItem(w_Handle, i), WM_SETFONT, reinterpret_cast<WPARAM>(hFont), 1u);
		else
			SendMessage(GetDlgItem(w_Handle, i), WM_SETFONT, reinterpret_cast<WPARAM>(GetStockObject(DEFAULT_GUI_FONT)), 1u);
	}
	return;
}

std::vector<std::string> GetItemsFromFile(const char* path, HWND w_Handle)
{
	std::vector<std::string> linesVec = std::vector<std::string>();

	std::ifstream file;
	file.open(path, std::ios::in);
	if(file.is_open())
	{
		std::string line;
		while(std::getline(file, line))
			linesVec.push_back(line);
		file.close();
	}
	else
		MessageBoxW(w_Handle, L"Cannot open file.", L"Error", MB_OK | MB_ICONERROR);
	
	delete[] path;
	return linesVec;
}
