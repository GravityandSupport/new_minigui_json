#pragma once

#include "base_window.hpp"


class DesktopWindow : public BaseWindow {
public:
	virtual bool start(HWND hParent) override {return false;} // 桌面窗口不允许创建
	using BaseWindow::Close;
	virtual void Close() override {}; // 不允许关闭桌面窗口

	void winProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam);
	void winProcPaint(HWND hWnd, HDC hdc); /// paint比较特殊，需要特殊处理单独在主窗口的 MSG_PAINT消息中调用
	
	DesktopWindow() = default;
    DesktopWindow(const std::string& str);
    virtual ~DesktopWindow() = default;;
};
