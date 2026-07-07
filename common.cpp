#include "common.hpp"
#include "widget.hpp"


void notChangeFontSize(HDC hdc, std::function<void(void)> call){
    PLOGFONT curFont = GetCurFont(hdc);
    call();
    SelectFont(hdc, curFont);
}

void notChangeFontColor(HDC hdc, std::function<void(void)> call){
    uint32_t curFontColor = GetTextColor(hdc);
    call();
    SetTextColor(hdc, curFontColor);
}

void notChangeFontSizeColor(HDC hdc, std::function<void(void)> call){
    PLOGFONT curFont = GetCurFont(hdc);
    uint32_t curFontColor = GetTextColor(hdc);
    call();
    SetTextColor(hdc, curFontColor);
    SelectFont(hdc, curFont);
}


//////////////////////////////////////////////////////////////////////////////

extern "C" {

void windowCreate(const char* window_name, HWND hParent){
	ui::BaseWindow::Create(window_name, hParent);
}

void windowClose(const char* window_name){
	ui::BaseWindow::Close(window_name);
}

void desktopWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam){
	ui::DesktopWindow::allWinProc(hWnd, message, wParam, lParam);
}
void desktopWinProcPaint(HWND hWnd, HDC hdc){
	ui::DesktopWindow::allWinProcPaint(hWnd, hdc);
}

void windowStatus(const char* window_name, bool statuss){
	auto& registry_window = ui::BaseWindow::getRegistryWindow();
	auto it = registry_window.find(window_name);
	if(it == registry_window.end()) {LOG_INFO("该窗口没有注册过，找不到对应句柄"); return ;}
	it->second->setStatus(statuss);
}

}
