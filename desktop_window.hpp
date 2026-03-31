#pragma once

#include "base_window.hpp"

namespace ui{
class DesktopWindow : public BaseWindow {
private:
	void winProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam);
	void winProcPaint(HWND hWnd, HDC hdc); /// paint比较特殊，需要特殊处理单独在主窗口的 MSG_PAINT消息中调用
public:
	virtual bool start(HWND hParent) override {return false;} // 桌面窗口不允许创建
	using BaseWindow::Close;
	virtual void Close() override {}; // 不允许关闭桌面窗口

	static std::map<std::string, DesktopWindow*>& getDesktopWindow(){ /*所有桌面窗口*/
		static std::map<std::string, DesktopWindow*> registry;return registry;}

	static void allWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam);
	static void allWinProcPaint(HWND hWnd, HDC hdc);

	virtual void updateDirtyArea() override; // 更新窗口
	
	DesktopWindow() = default;
    DesktopWindow(const std::string& str);
    virtual ~DesktopWindow() = default;;
};
}