#include "desktop_window.hpp"

DesktopWindow::DesktopWindow(const std::string& str)
    : BaseWindow(str){
    
	if (!this->name.empty()) {  // 注意是 !name.empty()
    	auto& desktop_window = getDesktopWindow();
        desktop_window[this->name] = this;
        std::cout << "✓ 桌面窗口已注册: " << this->name << " (地址: " << this << ")\n";
    } else {
        std::cout << "✗ 桌面窗口跳过注册: 空名字对象 (地址: " << this << ")\n";
    }
}

void DesktopWindow::winProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam){
	int x,y;

	switch(message){
		case MSG_CREATE:
			loadResources(); // 桌面窗口的资源加载在放在init中去做了
			this->hWnd = hWnd;
			registry_open_window[this->name] = this;
			this->msg_init(wParam, lParam);
			break;
		case MSG_COMMAND:
			this->msg_command(wParam, lParam);
			break;
		case MSG_LBUTTONDOWN:
			x=LOWORD(lParam);
			y=HIWORD(lParam);
			this->msg_lbutton_down(x, y);
			break;
		case MSG_LBUTTONUP:
			x=LOWORD(lParam);
			y=HIWORD(lParam);
			this->msg_lbutton_up(x, y);
			break;
		case MSG_MOUSEMOVE:
			x=LOWORD(lParam);
			y=HIWORD(lParam);
			this->msg_mousemove(x, y);
			break;
		case MSG_KEYUP:
			this->msg_keyup(wParam, lParam);
			break;
		case MSG_KEYDOWN:
			this->msg_keydown(wParam, lParam);
			break;
		case MSG_KEYLONGPRESS:
			this->msg_key_long_press(wParam, lParam);
			break;
		case MSG_KEYUP_LONG:
			this->msg_keyup_long(wParam, lParam);
			break;
		case MSG_TIMER:
			this->msg_timer(wParam, lParam);
			break;
		default:
			break;
	}
}

void DesktopWindow::winProcPaint(HWND hWnd, HDC hdc){
	SetBkMode(hdc, BM_TRANSPARENT);
	this->cache_hdc = hdc;
	this->msg_paint(hdc);
	SetBkMode(hdc, BM_OPAQUE);
	this->dirty_rc_list.clear(); // 清除脏区域缓冲
}


void DesktopWindow::allWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam){
	auto& desktop_window = getDesktopWindow();
	for (const auto& pair : desktop_window){
        pair.second->winProc(hWnd, message, wParam, lParam);
    }
}
void DesktopWindow::allWinProcPaint(HWND hWnd, HDC hdc){
	auto& desktop_window = getDesktopWindow();
	for (const auto& pair : desktop_window){
        pair.second->winProcPaint(hWnd, hdc);
    }
}

