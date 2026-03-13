#pragma once

#include "base_attr.hpp"

class BaseWindow;

class Widget : public BaseAttr {
public:

	
    // DLGTEMPLATE dlg;

    // virtual bool start(HWND hParent);

    // virtual void configDLG();
    // virtual void loadResources(){} // 鍔犺浇璧勬簮锛屾瘮濡傜収鐗囧暐鐨?
    // virtual void uninstallResources(){} // 鍗歌浇璧勬簮 

    // static int winProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam);

	int bvm_LoadBitmap(const char* path);
	void bvm_PaintBitmap(HDC hdc, size_t index, const RECT &rc); // 绘制位图
	void bvm_PaintBitmap(HDC hdc, size_t index, int x, int y);
	void bvm_PaintBitmap(HDC hdc, size_t index, int x, int y, int w, int h);
	void bvm_UnloadAllBitmaps(); // 统一卸载全部管理的位图资源
	
	void drawText(const char* pText, int nCount, 
                			int x, int y, int w, int h, UINT nFormat);
	void drawText(int pText, int nCount, 
                			int x, int y, int w, int h, UINT nFormat);
	
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

	BaseWindow* parentWindow = nullptr;
    Widget *parentWidget = nullptr;
    std::vector<Widget*> registry_widget;

    virtual void registerWidget(Widget &widget);

	Point getAbsoluteOffset(); // 浼氭妸褰撳墠widget::rc鐨勫潗鏍囧綋鍋氬鐖剁獥鍙ｇ殑鍋忕Щ鍧愭爣锛岃繑鍥炴帶浠剁殑鍧愭爣鍩轰簬鐖剁獥鍙ｇ殑鍋忕Щ鍧愭爣

	Widget() = default;
    Widget(const std::string& str);
    virtual ~Widget() = default;
};
