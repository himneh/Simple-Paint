// SimplePaint.cpp : Defines the entry point for the application.
//19120583

#include "framework.h"
#include "SimplePaint.h"

#include <Windows.h>
#include <WindowsX.h>
#include <CommCtrl.h>
#include <commdlg.h>
#include <shobjidl.h> 
#include <stdio.h>
#include <MMSystem.h>
#include <WinUser.h>
#include <wingdi.h>
#include <string>
#include <vector>
#include <memory>
#include <fstream>

#include "Ellipse.h"
#include "IShapes.h"
#include "Point.h"
#include "Rectangle.h"
#include "Line.h"
#include "Text.h"
#include "Tokenizer.h"
using namespace std;

#pragma comment(lib, "Comctl32.lib")

#define IMAGE_WIDTH     18
#define IMAGE_HEIGHT    18
#define BUTTON_WIDTH    0
#define BUTTON_HEIGHT   0

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

//Paint's variables:
int fromX;
int fromY;
int toX;
int toY;
int line = 0;
int rec = 0;
int ellipse = 0;
int square = 0;
bool isDrawingTextbox = false;
int shine;
int shineCurrent = 2;
bool isDrawing = false;
int saved = 0;
//Color
CHOOSECOLOR  cc; // Thông tin màu chọn
COLORREF  acrCustClr[16]; // Mảng custom color
DWORD  rgbCurrent = RGB(255, 0, 0); // Red
HBRUSH  hbrush; // Tạo ra brush từ màu đã chọn

//Font
CHOOSEFONT cf;            // common dialog box structure
static LOGFONT lf;        // logical font structure
HFONT hfont, hfontPrev;
DWORD rgbPrev;
HDC hdc;
DWORD fontColor;

OPENFILENAME ofn;       // common dialog box structure
char szFile[260];       // buffer for file name

HANDLE hf;              // file handle

bool recent = false;

//Insert_text's variable:
int buffer_size = 256;
WCHAR* buffer;

//Declaration for Select function - Left Mouse button
int select_x;
int select_y;
int select_tox;
int select_toy;
bool isSelecting;
bool Selected;
int int_Select = -1;
wstring type_Select;
shared_ptr<IShape> Saved;
shared_ptr<IShape> Shape_Saved;
bool isCopy = false; 
bool isCut = false;

//My own variables's for shapes declaration
vector<shared_ptr<IShape>> shapes;
shared_ptr<IShape> input = NULL;
vector<shared_ptr<IShape>> redoShape;
vector<Text> old_text;
vector<Text> redoText;
vector<string> justInput;
vector<string> justPop;
vector<HFONT> text_font;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);
void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
void OnDestroy(HWND hwnd);
void OnPaint(HWND hwnd);
void OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags);
void OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags);
void OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags);

// Open's dialog function: Display Open - Save dialog
wstring openfilename(HWND owner, bool isOpen) {
	OPENFILENAME ofn;
	wchar_t filePath[MAX_PATH] = L"Untitle";
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = owner;
	ofn.lpstrFilter = (LPCWSTR)L"Text (*.txt)\0*.txt\0\0";
	ofn.lpstrFile = (LPTSTR)filePath;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_OVERWRITEPROMPT | OFN_NOVALIDATE | OFN_ENABLETEMPLATEHANDLE | OFN_CREATEPROMPT;
	ofn.lpstrDefExt = (LPCWSTR)L"txt";
	wstring fileNameStr;
	if (isOpen) {
		if (GetOpenFileName(&ofn)) {
			fileNameStr = wstring(filePath);
		}
		else {
			int errorCode = CommDlgExtendedError();
			fileNameStr = L"";
		}
	}
	else {
		if (GetSaveFileName(&ofn)) {
			fileNameStr = wstring(filePath);
		}
		else {
			int errorCode = CommDlgExtendedError();
			fileNameStr = L"";
		}
	}
	return fileNameStr;
}

//ClearScreen function: Clear all shapes and texts, back to white background.
void clearScreen(HWND hwnd)
{
	RECT rc;
	GetClientRect(hwnd, &rc);
	rc.top += IMAGE_HEIGHT * 2;
	rc.left -= 5;
	rc.right += 5;
	rc.bottom += 5;
	InvalidateRect(hwnd, &rc, TRUE);
}

//Save function: Save all shapes and text to file .txt
void Save(HWND hwnd) {
	wstring fileDirection = openfilename(hwnd, 0);
	if (fileDirection.size() > 0) {
		wofstream writer(fileDirection);

		//Write shapes
		writer << shapes.size() << "\n";
		for (int i = 0; i < shapes.size(); i++) {
			writer << shapes[i]->Type() << L": " << shapes[i]->toString() << "\n";
		}
						
		//Write texts
		writer << old_text.size() << "\n";
		for (int i = 0; i < old_text.size(); i++) {
			writer << old_text[i].toString() << "\n";
		}

		writer.close();
		
		//Display Success Message
		wstring success = L"Saved at ";
		success += fileDirection;
		MessageBox(hwnd, &success[0], L"Save Image", MB_OK | MB_ICONINFORMATION);
	}
}

//rePaint function: Paint (aka Save shapes) all the shapes to the screen from the vector
void rePaint(HDC hDc)
{
	HPEN hPen = CreatePen(PS_DASHDOT, shine, rgbCurrent);
	SelectObject(hDc, hPen);
	int sizee = shapes.size();
	DWORD color_tmp;
	if (sizee > 0)
	{
		for (int i = 0; i < sizee; i++) {

			if (shapes[i]->Type() == L"Line") {
				color_tmp = shapes[i]->getColor();
				shine = shapes[i]->getSize();
				hPen = CreatePen(PS_DASHDOT, shine, color_tmp);
				SelectObject(hDc, hPen);
				MoveToEx(hDc, shapes[i]->getTop().getX(), shapes[i]->getTop().getY(), NULL);
				LineTo(hDc, shapes[i]->getBottom().getX(), shapes[i]->getBottom().getY());
				DeleteObject(hPen);
			}
			if (shapes[i]->Type() == L"Rectangle") {
				color_tmp = shapes[i]->getColor();
				shine = shapes[i]->getSize();
				hPen = CreatePen(PS_DASHDOT, shine, color_tmp);
				SelectObject(hDc, hPen);
				Rectangle(hDc, shapes[i]->getTop().getX(), shapes[i]->getTop().getY(),
					shapes[i]->getBottom().getX(), shapes[i]->getBottom().getY());
				DeleteObject(hPen);
			}
			if (shapes[i]->Type() == L"Ellipse") {
				color_tmp = shapes[i]->getColor();
				shine = shapes[i]->getSize();
				hPen = CreatePen(PS_DASHDOT, shine, color_tmp);
				SelectObject(hDc, hPen);
				Ellipse(hDc, shapes[i]->getTop().getX(), shapes[i]->getTop().getY(),
					shapes[i]->getBottom().getX(), shapes[i]->getBottom().getY());
				DeleteObject(hPen);
			}
		}
	}
}

//rePaint function: Paint (aka Save shapes) all the shapes to the screen from the vector
void reTexted(HDC hDc) {
	int shizee = old_text.size();
	if (shizee > 0)
	{
		for (int i = 0; i < shizee; i++)
		{
			//HFONT azz = text_font[0];
			DWORD zzo = old_text[i].getColor();
			//SelectObject(hDc, azz);
			SetTextColor(hDc, zzo);
			shine = old_text[i].getSize();
			LPCWSTR str_tmp = &old_text[i].getText()[0]; 
			int lengg = old_text[i].getLength();
			int corX = old_text[i].getX();
			int corY = old_text[i].getY();
			TextOutW(hDc, corX, corY, str_tmp, lengg);
			//DeleteObject(azz);
		}
	}
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.

	// Initialize global strings
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_SIMPLEPAINT, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SIMPLEPAINT));

	MSG msg;

	// Main message loop:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON2));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_SIMPLEPAINT);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON1));

	return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Store instance handle in our global variable

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		HANDLE_MSG(hWnd, WM_CREATE, OnCreate);
		HANDLE_MSG(hWnd, WM_COMMAND, OnCommand);
		HANDLE_MSG(hWnd, WM_DESTROY, OnDestroy);
		HANDLE_MSG(hWnd, WM_PAINT, OnPaint);
		HANDLE_MSG(hWnd, WM_LBUTTONDOWN, OnLButtonDown);
		HANDLE_MSG(hWnd, WM_LBUTTONUP, OnLButtonUp);
		HANDLE_MSG(hWnd, WM_MOUSEMOVE, OnMouseMove);

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

// Message handler for insert text dialog.
INT_PTR CALLBACK insertDialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;
	
	case WM_CLOSE:
	{
		EndDialog(hDlg, LOWORD(wParam));
		isDrawingTextbox = true;
		return (INT_PTR)TRUE;
		break;
	}
	case WM_COMMAND:
		if (LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			isDrawingTextbox = false;
			return (INT_PTR)TRUE;
		}
		else if (LOWORD(wParam) == IDOK)
		{
			HWND hwndOutput = GetDlgItem(hDlg, IDC_EDIT1);
			int len = GetWindowTextLength(hwndOutput);
			WCHAR* buff = new WCHAR[len + 1];
			
			GetWindowText(hwndOutput, buff, len + 1);
			
			buffer = buff;
			buffer_size = len + 1;
			
			isDrawingTextbox = true;
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
	// Lấy font hệ thống
	LOGFONT lf;
	GetObject(GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
	HFONT hFont = CreateFont(lf.lfHeight, lf.lfWidth,
		lf.lfEscapement, lf.lfOrientation, lf.lfWeight,
		lf.lfItalic, lf.lfUnderline, lf.lfStrikeOut, lf.lfCharSet,
		lf.lfOutPrecision, lf.lfClipPrecision, lf.lfQuality,
		lf.lfPitchAndFamily, lf.lfFaceName);

	//Tạo toolbar
	InitCommonControls();
	TBBUTTON tbButtons[] =
	{
		{ STD_FILENEW,	ID_FILE_NEW, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 },
		{ STD_FILEOPEN,	ID_FILE_OPEN, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 },
		{ STD_FILESAVE,	ID_FILE_SAVE, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 },
		{ STD_UNDO,	ID_FILE_UNDO, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 },
		{ STD_REDOW, ID_FILE_REDO, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 }
	};
	HWND hToolBarWnd = CreateToolbarEx(hwnd,
		WS_CHILD | WS_VISIBLE | CCS_ADJUSTABLE | TBSTYLE_TOOLTIPS,
		ID_TOOLBAR, sizeof(tbButtons) / sizeof(TBBUTTON), HINST_COMMCTRL,
		0, tbButtons, sizeof(tbButtons) / sizeof(TBBUTTON),
		BUTTON_WIDTH, BUTTON_HEIGHT, IMAGE_WIDTH, IMAGE_HEIGHT,
		sizeof(TBBUTTON));

	//Chèn hình cá nhân
	TBBUTTON userButtons[] =
	{
		{ 0, 0,	TBSTATE_ENABLED, TBSTYLE_SEP, 0, 0 },
		{ 0, ID_DRAW_ELLIPSE,	TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 },
		{ 1, ID_DRAW_RECTANGLE,	TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 },
		{ 2, ID_DRAW_LINE,	TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 },
		{ 3, ID_DRAW_TEXT,	TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 }
	};
	TBADDBITMAP	tbBitmap = { hInst, IDB_BITMAP1 };
	// Thêm danh sách ảnh vào toolbar
	int idx = SendMessage(hToolBarWnd, TB_ADDBITMAP, (WPARAM)sizeof(tbBitmap) / sizeof(TBADDBITMAP),
		(LPARAM)(LPTBADDBITMAP)&tbBitmap);
	// Xác định chỉ mục hình ảnh c ủa mỗi button từ ảnh bự liên hoàn nhiều tấm
	userButtons[1].iBitmap += idx;
	userButtons[2].iBitmap += idx;
	userButtons[3].iBitmap += idx;
	userButtons[4].iBitmap += idx;
	// Thêm nút bấm vào toolbar
	SendMessage(hToolBarWnd, TB_ADDBUTTONS, (WPARAM)sizeof(userButtons) / sizeof(TBBUTTON),
		(LPARAM)(LPTBBUTTON)&userButtons);

	return TRUE;
}

void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch (id) {
	case ID_CHOOSE_COLOR: 
	{
		ZeroMemory(&cc, sizeof(CHOOSECOLOR));
		cc.lStructSize = sizeof(CHOOSECOLOR);
		cc.hwndOwner = hwnd;
		cc.lpCustColors = (LPDWORD)acrCustClr;
		cc.rgbResult = rgbCurrent;
		cc.Flags = CC_FULLOPEN | CC_RGBINIT;
		if (ChooseColor(&cc))
		{
			hbrush = CreateSolidBrush(cc.rgbResult);
			rgbCurrent = cc.rgbResult;
		}
		break;
	}
	case ID_CHOOSE_FONT:
	{
		// Initialize CHOOSEFONT
		ZeroMemory(&cf, sizeof(cf));
		cf.lStructSize = sizeof(cf);
		cf.hwndOwner = hwnd;
		cf.lpLogFont = &lf;
		cf.rgbColors = rgbCurrent;
		cf.Flags = CF_SCREENFONTS | CF_EFFECTS;

		if (ChooseFont(&cf) == TRUE)
		{
			hfont = CreateFontIndirect(cf.lpLogFont);
			hfontPrev = (HFONT)SelectObject(hdc, hfont);
			fontColor = cf.rgbColors;
			rgbPrev = SetTextColor(hdc, rgbCurrent);
		}
		break;
	}
	case IDM_EXIT:
	{
		if (MessageBox(hwnd, L"Save Image?", L"Save Image", MB_YESNO | MB_ICONQUESTION) == IDYES)
		{
			Save(hwnd);
		}
		DestroyWindow(hwnd);
		break;
	}

	case IDM_ABOUT:
	{
		DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG1), hwnd, About);
		break;
	}

	case ID_FILE_NEW:
	{
		//Ask user for save file
		if (shapes.size() > 0 && saved != shapes.size())
		{
			if (MessageBox(hwnd, L"Save Image?", L"Save Image", MB_YESNO | MB_ICONQUESTION) == IDYES)
			{
				Save(hwnd);
			}
		}
		rec = 0;
		line = 0;
		ellipse = 0;
		isDrawingTextbox = false;
		//Clear all vectors
		shapes.clear();
		old_text.clear();
		text_font.clear();

		clearScreen(hwnd);
		InvalidateRect(hwnd, NULL, TRUE);

		break;
	}

	case ID_FILE_OPEN: //Read file
	{
		wstring fileDirection = openfilename(hwnd, 1);
		wifstream reader(fileDirection);
		if (!reader.is_open()) {
			MessageBox(NULL, __T("Load File Failed!"), __T("Error"), MB_OK | MB_ICONERROR);
		}
		else
		{
			wstring buffer;
			getline(reader, buffer);
			int count = stoi(buffer);

			for (int i = 0; i < count; i++) {
				getline(reader, buffer);
				auto tokens = Tokenizer::split(buffer, L": ");

				wstring kind = tokens[0];
				if (kind == L"Rectangle") {
					Rectanglee myrect = Rectanglee::parse(tokens[1]);
					shapes.push_back(make_shared <Rectanglee>(myrect));
				}
				if (kind == L"Ellipse") {
					Ellipsee myell = Ellipsee::parse(tokens[1]);
					shapes.push_back(make_shared <Ellipsee>(myell));
				}
				if (kind == L"Line") {
					Line myline = Line::parse(tokens[1]);
					shapes.push_back(make_shared <Line>(myline));
				}
			}
					
			getline(reader, buffer);
			int kount = stoi(buffer);
		
			for (int i = 0; i < kount; i++) {
				getline(reader, buffer);
				Text TxT = Text::parse(buffer);
				old_text.push_back(TxT);

			}
			reader.close();
			PAINTSTRUCT ps;
			HDC hDc = BeginPaint(hwnd, &ps);
			rePaint(hDc);
			reTexted(hDc);
			EndPaint(hwnd, &ps);
			clearScreen(hwnd);
			InvalidateRect(hwnd, NULL, TRUE);		

		}
		break;
	}

	case ID_LINESIZE_ZOOMIN:
	{
		if (shineCurrent > 50)
			MessageBox(hwnd, L"Biggest Size!", L"Biggest Size!", MB_OK | MB_ICONWARNING);
		else 
			shineCurrent *= 2;
		break;
	}

	case ID_LINESIZE_ZOOMOUT:
	{
		if(shineCurrent < 2)
			MessageBox(hwnd, L"Smallest Size!", L"Smallest Size!", MB_OK | MB_ICONWARNING);
		else
			shineCurrent /= 2;
		break;
	}

	case ID_FILE_SAVE:
	{
		Save(hwnd);
		saved = shapes.size();
		break;
	}

	case ID_CHILLING_LOFI:
	{
		PlaySound(TEXT("chill.wav"), NULL, SND_FILENAME | SND_ASYNC);
		break;
	}
	
	case ID_DRAW_ELLIPSE:
	{
		ellipse = 1; 
		line = 0;
		rec = 0;
		isDrawingTextbox = false;
		break;
	}

	case ID_DRAW_LINE:
	{
		line = 1;
		rec = 0;
		ellipse = 0;
		isDrawingTextbox = false;
		break;
	}
	
	case ID_DRAW_RECTANGLE:
	{
		rec = 1;
		line = 0;
		ellipse = 0;
		isDrawingTextbox = false;
		break;
	}

	case ID_DRAW_TEXT:
	{
		rec = 0;
		line = 0;
		ellipse = 0;
		SetTextColor(hdc, rgbCurrent);
		SelectObject(hdc, hfontPrev);
		DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG2), hwnd, insertDialog);
		
		break;
	}
	case ID_FILE_UNDO:
	{
		if (shapes.size() == 0 && old_text.size() == 0) {
			clearScreen(hwnd);
			shapes.clear();
			old_text.clear();
			/*rec = 0;
			line = 0;
			ellipse = 0;*/
			InvalidateRect(hwnd, NULL, TRUE);
			MessageBox(hwnd, L"CANNOT UNDO!", L"Error!", MB_OK | MB_ICONWARNING);
		}
		else {
			if (justInput[justInput.size() - 1] == "Shape") {
				if (shapes.size() > 0) {
					redoShape.push_back(shapes[shapes.size() - 1]);
					shapes.pop_back();
					rec = 0;
					line = 0;
					ellipse = 0;
					clearScreen(hwnd);
					rePaint(hdc);
					InvalidateRect(hwnd, NULL, TRUE);
				}

			}
			if (justInput[justInput.size() - 1] == "Text") {
				if (old_text.size() > 0) {
					redoText.push_back(old_text[old_text.size() - 1]);
					old_text.pop_back();
					isDrawingTextbox = false;
					clearScreen(hwnd);
					reTexted(hdc);
				}
			}
			justPop.push_back(justInput[justInput.size() - 1]);
			justInput.pop_back();
		}
		break;
	}
	case ID_FILE_REDO:
	{
		if (redoShape.size() == 0 && redoText.size() == 0) {
			MessageBox(hwnd, L"CANNOT REDO!", L"Error!", MB_OK | MB_ICONWARNING);
		}
		else {
			if (justPop[justPop.size() - 1] == "Shape") {
				if (redoShape.size() > 0) {
					shapes.push_back(redoShape[redoShape.size() - 1]);
					redoShape.pop_back();
					clearScreen(hwnd);
					rePaint(hdc);
				}
			}
			else {
				if (redoText.size() > 0) {
					old_text.push_back(redoText[redoText.size() - 1]);
					redoText.pop_back();
					clearScreen(hwnd);
					reTexted(hdc);
				}
			}

			justInput.push_back(justPop[justPop.size() - 1]);
			justPop.pop_back();
		}
		break;

		}
	}
}

void OnDestroy(HWND hwnd)
{
	PostQuitMessage(0);
}

void OnPaint(HWND hwnd)
{
	PAINTSTRUCT ps;
	HDC hDc = BeginPaint(hwnd, &ps);
	
	shine = shineCurrent;
	HPEN hPen = CreatePen(PS_DASHDOT, shine, rgbCurrent);
	SelectObject(hDc, hPen);

	rePaint(hDc);

	reTexted(hDc);
	
	shine = shineCurrent;
	//Vẽ mới
	hPen = CreatePen(PS_DASHDOT, shine, rgbCurrent);
	SelectObject(hDc, hPen);
	MoveToEx(hDc, fromX, fromY, NULL);
	
	if (line == 1)//Paint Line
	{
		LineTo(hDc, toX, toY);
	}

	if(rec == 1)//Paint rectangle
	{
		Rectangle(hDc, fromX, fromY, toX, toY);
	}

	if (ellipse == 1) //Paint ellipse
	{
		Ellipse(hDc, fromX, fromY, toX, toY);

	}

	if (isDrawingTextbox) { //Insert Text
		recent = false;
		SelectObject(hDc, hfont);
		SetTextColor(hDc, fontColor);
		TextOutW(hDc, fromX, fromY, &buffer[0], buffer_size);
	}
	EndPaint(hwnd, &ps);
}

void OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
	isDrawing = true;
	fromX = x;
	fromY = y;
	HDC hdc = GetDC(hwnd);
	MoveToEx(hdc, x, y, NULL);
}

void OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags)
{
	isDrawing = false;
	// Báo hiệu cần xóa đi toàn bộ màn hình & vẽ lại
	InvalidateRect(hwnd, NULL, TRUE);
	toX = x;
	toY = y;
	if (line == 1)
	{
		Line line(Point(fromX, fromY), Point(toX, toY), rgbCurrent, shine);
		shapes.push_back(make_shared<Line>(line));
		justInput.push_back("Shape");
	}

	if (rec == 1)
	{
		Rectanglee rect(Point(fromX, fromY), Point(toX, toY), rgbCurrent, shine);
		shapes.push_back(make_shared<Rectanglee>(rect));
		justInput.push_back("Shape");
	}

	if (ellipse == 1)
	{
		Ellipsee ell(Point(fromX, fromY), Point(toX, toY), rgbCurrent, shine);
		shapes.push_back(make_shared <Ellipsee>(ell));
		justInput.push_back("Shape");
	}
	if (isDrawingTextbox)
	{
		Text TXT(buffer, buffer_size, fromX, fromY, fontColor, shine);
		old_text.push_back(TXT);
		text_font.push_back(hfontPrev);
		justInput.push_back("Text");
	}
}

void OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags)
{
	if (isDrawing) {
		toX = x;
		toY = y;
	
		// Báo hiệu cần xóa đi toàn bộ màn hình
		InvalidateRect(hwnd, NULL, TRUE);
	}
}
