#pragma once

#include "base_attr.hpp"

class BaseWindow;

class Widget : public BaseAttr {
public:

	
    // DLGTEMPLATE dlg;

    // virtual bool start(HWND hParent);

    // virtual void configDLG();
    // virtual void loadResources(){} // 加载资源，比如照片啥的
    // virtual void uninstallResources(){} // 卸载资源 

    // static int winProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam);

    virtual void msg_init(WPARAM wParam, LPARAM lParam) override;
    virtual void msg_command(WPARAM wParam, LPARAM lParam) override;
    virtual void msg_lbutton_down(int x, int y) override;
    virtual void msg_lbutton_up(int x, int y) override;
    virtual void msg_mousemove(int x, int y) override;
    virtual void msg_keyup(WPARAM wParam, LPARAM lParam) override;
    virtual void msg_keydown(WPARAM wParam, LPARAM lParam) override;
    virtual void msg_timer(WPARAM wParam, LPARAM lParam) override;
    virtual void msg_paint(HDC hdc) override;
    virtual void msg_close(WPARAM wParam, LPARAM lParam) override;
    virtual void msg_nc_lbutton_up(WPARAM wParam, LPARAM lParam) override;
    virtual void msg_destroy(WPARAM wParam, LPARAM lParam) override;

	BaseWindow* parentWindow = nullptr;
    Widget *parentWidget = nullptr;
    std::vector<Widget*> registry_widget;

    virtual void registerWidget(Widget &widget);

    void getParentOffset();

    Widget(const std::string& str);
    virtual ~Widget() = default;
};
