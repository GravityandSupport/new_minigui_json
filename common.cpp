#include "common.hpp"

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
	BaseWindow::Create(window_name, hParent);
}

void windowClose(const char* window_name){
	BaseWindow::Close(window_name);
}

}
