#pragma once

#include "base_attr.hpp"
#include "widget.hpp"

#define __UI_WINDOW_COMMON_UPDATE__ 10

class BaseWindow : public BaseAttr {
public:
	RECT dirty_rc; // 脏区域，在 msg_common 消息中刷新这个区域
	
    DLGTEMPLATE dlg;

    virtual bool start(HWND hParent);

    virtual void configDLG();
    virtual void loadResources(){} // 加载资源，比如照片啥的，添加部件也要放在这部分
    virtual void uninstallResources(){} // 卸载资源 

    static int winProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam);
    
    static std::unordered_map<std::string, BaseWindow*> 
                                        registry_window, /*已注册的窗口*/
                                        registry_open_window; /*已打开的窗口*/

	static void Create(const std::string &window_name, BaseWindow &hParent);			// 打开窗口
	static void Create(const std::string &window_name, HWND hParent);			// 打开窗口
	static void Close(const std::string &window_name); // 关闭窗口
	void Close();
										
    std::unordered_map<std::string, Widget*> registry_widget; // 子控件

    virtual void registerWidget(const std::string &name, Widget &widget);
	Widget* findWidget(const std::string& name) const; // 通过名字查找已注册的控件，找不到返回nullptr

	virtual void unifiedUpdate(const std::initializer_list<BaseAttr*> &widgets, const std::function<void(void)> &call);
	
    virtual void msg_init(WPARAM wParam, LPARAM lParam) override;
    virtual void msg_command(WPARAM wParam, LPARAM lParam) override;
    virtual void msg_lbutton_down(int x, int y) override;
    virtual void msg_lbutton_up(int x, int y) override;
    virtual void msg_mousemove(int x, int y) override;
    virtual void msg_keyup(WPARAM wParam, LPARAM lParam) override;
    virtual void msg_keydown(WPARAM wParam, LPARAM lParam) override;
	virtual void msg_key_long_press(WPARAM wParam, LPARAM lParam) override;
	virtual void msg_keyup_long(WPARAM wParam, LPARAM lParam) override;
    virtual void msg_timer(WPARAM wParam, LPARAM lParam) override;
    virtual void msg_paint(HDC hdc) override;
    virtual void msg_close(WPARAM wParam, LPARAM lParam) override;
    virtual void msg_nc_lbutton_up(WPARAM wParam, LPARAM lParam) override;
    virtual void msg_destroy(WPARAM wParam, LPARAM lParam) override;

    BaseWindow(const std::string& str);
    virtual ~BaseWindow();
};
