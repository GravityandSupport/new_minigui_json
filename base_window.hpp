#pragma once

#include "base_attr.hpp"
#include "widget.hpp"

#include "bitmap_manager.hpp"
#include "graphics.hpp"
#include "long_press_detector.hpp"

#include "LanguageManager.hpp"



namespace ui{
class LongPressDetector;

class BaseWindow : public BaseAttr {
protected:
	HDC cache_hdc;
	LongPressDetector long_press_detector; // 长按检测
public:
	std::vector<RECT> dirty_rc_list; // 脏区域，在 msg_common 消息中刷新这个区域

    DLGTEMPLATE dlg;
	HWND hParent=HWND_NULL; // 父窗口句柄

	LanguageManager<LangText> lang_manager;

    virtual bool start(HWND hParent);

    virtual void configDLG();
    virtual void loadResources(){} // 加载资源，比如照片啥的，添加部件也要放在这部分
    virtual void uninstallResources(); // 卸载资源 
    BitmapManager bitmap_manager;
	int bvm_LoadBitmap(const char* path); // 加载位图数据，并加入管理队列，
	void bvm_PaintBitmap(HDC hdc, size_t index, const RECT &rc); // 绘制位图
	void bvm_PaintBitmap(HDC hdc, size_t index, int x, int y);
	void bvm_PaintBitmap(HDC hdc, size_t index, int x, int y, int w, int h);
	void bvm_UnloadAllBitmaps(); // 统一卸载全部管理的位图资源

	void drawText2(size_t id, size_t lang,
							int x, int y, int w, int h, UINT nFormat);
	void drawText(const char* pText, int nCount, 
                			int x, int y, int w, int h, UINT nFormat);
	void drawText(int pText, int nCount, 
                			int x, int y, int w, int h, UINT nFormat);
	
    static int winProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam);
    
    static std::unordered_map<std::string, BaseWindow*> registry_open_window; /*已打开的窗口*/
	static void forEachOpenWindow(std::function<void(BaseWindow*)> callback); // 遍历所有打开的窗口
                                        
    static std::map<std::string, BaseWindow*>& getRegistryWindow(){ /*已注册的窗口*/
		static std::map<std::string, BaseWindow*> registry;
        return registry;
    }
	static void forEachRegistryWindow(std::function<void(BaseWindow*)> callback);// 遍历所有注册的窗口

	static void Create(const std::string &window_name, BaseWindow &hParent);			// 打开窗口
	static void Create(const std::string &window_name, HWND hParent);			// 打开窗口
	static void Close(const std::string &window_name); // 关闭窗口
	virtual void Close();
										
    std::unordered_map<std::string, Widget*> registry_widget; // 子控件

	void forEachRegistryWidget(std::function<void(Widget*)> callback); // 遍历所有已注册的控件

    virtual void registerWidget(const std::string &name, Widget &widget);
	Widget* findWidget(const std::string& name) const; // 通过名字查找已注册的控件，找不到返回nullptr

	virtual void updateDirtyArea() override; // 更新窗口
	virtual void unifiedUpdate(const std::vector<BaseAttr*> &widgets, const std::function<void(void)> &call);
	virtual void unifiedUpdate(const std::vector<BaseAttr*> &widgets);
	virtual void unifiedUpdate(const std::function<void(void)> &call);
	virtual void unifiedUpdate();

	bool registerLongKey(LongPressDetector::Key key, const LongPressConfig& config); // 注册长按消息
	bool registerLongKey(LongPressDetector::Key key, const DoubleClickConfig& config); // 注册双击消息
	bool registerLongKey(LongPressDetector::Key key, const LongPressConfig& lconfig, const DoubleClickConfig& dconfig); // 注册长按+双击消息
	void unregisterLongKey(LongPressDetector::Key key);

	
	
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
	virtual void key_long_press(WPARAM wParam, LPARAM lParam) override;

	BaseWindow() = default;
    BaseWindow(const std::string& str);
    virtual ~BaseWindow();
};

}

