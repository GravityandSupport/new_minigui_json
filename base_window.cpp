#include "base_window.hpp"

BaseWindow::BaseWindow(const std::string& str)
    : BaseAttr(str){
    
    if (!name.empty()) {  // 注意是 !name.empty()
        registry_window[name] = this;
        std::cout << "✓ 已注册: " << name << " (地址: " << this << ")\n";
    } else {
        std::cout << "✗ 跳过注册: 空名字对象 (地址: " << this << ")\n";
    }
}

BaseWindow::~BaseWindow(){
    if (!name.empty()) {
        auto it = registry_window.find(name);
        if (it != registry_window.end() && it->second == this) {
            registry_window.erase(it);
            std::cout << "✓ 已注销: " << name << " (地址: " << this << ")\n";
        }
    }
}

void BaseWindow::registerWidget(const std::string &name, Widget &widget){
	widget.parentWindow = this;
    registry_widget[name] = &widget;
}

Widget* BaseWindow::findWidget(const std::string& name) const {
    auto it = registry_widget.find(name);
    return (it != registry_widget.end()) ? it->second : nullptr;
}


void BaseWindow::unifiedUpdate(const std::initializer_list<BaseAttr*> &widgets, const std::function<void(void)> &call){
	bool first = true;
	RECT prev_rc;
	if (widgets.size() == 0) {LOG_WARN("空列表", "widgets 是空列表"); return;}
	for (BaseAttr *w : widgets){
		if(hWnd != w->hWnd) {LOG_WARNC("控件父窗口不符", "%s 不是此窗口的子部件", w->name.c_str()); continue;}
		w->is_can_update = false; // 禁止刷新
		if(first==true){
			first = false;
			prev_rc = w->rc;
		}else{
			prev_rc = computBoundBox(prev_rc, w->rc);
		}
	}
	call();
	dirty_rc = prev_rc;
	PostMessage(hWnd, MSG_COMMAND, __UI_WINDOW_COMMON_UPDATE__, 0); // 通过 msg_common 来执行刷新功能
	for (BaseAttr *w : widgets){
		w->is_can_update = true; // 允许刷新
	}
}

void BaseWindow::configDLG(){
    dlg.dwStyle = WS_VISIBLE;
    dlg.dwExStyle = WS_EX_AUTOSECONDARYDC;
    dlg.x = rc.left;
    dlg.y = rc.top;
    dlg.w = rc.right-rc.left;
    dlg.h = rc.bottom-rc.top;
    dlg.caption = "";
    dlg.hIcon = 0;
    dlg.hMenu = 0;
    dlg.controlnr = 0;
    dlg.controls = 0;
    dlg.dwAddData = reinterpret_cast<decltype(dlg.dwAddData)>(this);
}

bool BaseWindow::start(HWND hParent){
    if(name.empty()){ 
        std::clog << "窗口未配置名字，不可以创建，否则容易丢失窗口句柄\n";
        return false;
    }

    loadResources();

    configDLG();

    CreateMainWindowIndirect(&dlg, hParent, winProc);

    std::clog << "创建" << name << "窗口完成\n";
    return true;
}

int BaseWindow::winProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam){
    HDC hdc;
    int x,y;
    BaseWindow* self = reinterpret_cast<BaseWindow*>(GetWindowAdditionalData(hWnd));

    switch(message){
        case MSG_INITDIALOG:
            self->hWnd = hWnd;
            registry_open_window[self->name] = self;
            self->msg_init(wParam, lParam);
            break;
        case MSG_COMMAND:
            self->msg_command(wParam, lParam);
            break;
        case MSG_LBUTTONDOWN:
            x=LOWORD(lParam);
            y=HIWORD(lParam);
            self->msg_lbutton_down(x, y);
            break;
        case MSG_LBUTTONUP:
            x=LOWORD(lParam);
            y=HIWORD(lParam);
            self->msg_lbutton_up(x, y);
            break;
        case MSG_MOUSEMOVE:
            x=LOWORD(lParam);
            y=HIWORD(lParam);
            self->msg_mousemove(x, y);
            break;
        case MSG_KEYUP:
            self->msg_keyup(wParam, lParam);
            break;
        case MSG_KEYDOWN:
            self->msg_keydown(wParam, lParam);
            break;
		case MSG_KEYLONGPRESS:
			self->msg_key_long_press(wParam, lParam);
			break;
		case MSG_KEYUP_LONG:
			self->msg_keyup_long(wParam, lParam);
			break;
        case MSG_TIMER:
            self->msg_timer(wParam, lParam);
            break;
        case MSG_PAINT:
            hdc=BeginPaint(hWnd);
            SetBkMode(hdc, BM_TRANSPARENT);
            self->msg_paint(hdc);
            SetBkMode(hdc, BM_OPAQUE);
            EndPaint(hWnd, hdc);
            break;
        case MSG_CLOSE:
            self->msg_close(wParam, lParam);
            DestroyMainWindowIndirect(hWnd);
            break;
        case MSG_NCLBUTTONUP:
            self->msg_nc_lbutton_up(wParam, lParam);
            break;
        case MSG_DESTROY:{
            self->msg_destroy(wParam, lParam);
            self->uninstallResources();

            auto it = registry_open_window.find(self->name);
            if (it != registry_open_window.end()) {
                std::cout << "[关闭窗口] 对象注销: " << it->first << " (地址: " << it->second << ")\n";
                registry_open_window.erase(it);
            }
            return 0;
        }
        default:
            break;
    }
    return DefaultWindowProc(hWnd,message,wParam,lParam);
}

void BaseWindow::msg_init(WPARAM wParam, LPARAM lParam)  {
    LOG_DEBUG("msg_init");
    for(auto& pair : registry_widget){
        pair.second->hWnd = hWnd;
        pair.second->msg_init(wParam, lParam);
    }
}
void BaseWindow::msg_command(WPARAM wParam, LPARAM lParam)  {
	switch (wParam){
		case __UI_WINDOW_COMMON_UPDATE__:{
//			LOG_DEBUG("刷新区域", dirty_rc.left, dirty_rc.top, dirty_rc.right, dirty_rc.bottom);
			InvalidateRect(hWnd, &dirty_rc, true);
			break;
		}
		default:break;
	}
    for(auto& pair : registry_widget){
        pair.second->msg_command(wParam, lParam);
    }
}
void BaseWindow::msg_lbutton_down(int x, int y)  {
    for(auto& pair : registry_widget){
        pair.second->msg_lbutton_down(x, y);
    }
}
void BaseWindow::msg_lbutton_up(int x, int y)  {
    for(auto& pair : registry_widget){
        pair.second->msg_lbutton_up(x, y);
    }
}
void BaseWindow::msg_mousemove(int x, int y)  {
    for(auto& pair : registry_widget){
        pair.second->msg_mousemove(x, y);
    }
}
void BaseWindow::msg_keyup(WPARAM wParam, LPARAM lParam)  {
    for(auto& pair : registry_widget){
        pair.second->msg_keyup(wParam, lParam);
    }
}
void BaseWindow::msg_keydown(WPARAM wParam, LPARAM lParam)  {
    for(auto& pair : registry_widget){
        pair.second->msg_keydown(wParam, lParam);
    }
}
void BaseWindow::msg_key_long_press(WPARAM wParam, LPARAM lParam)  {
    for(auto& pair : registry_widget){
        pair.second->msg_key_long_press(wParam, lParam);
    }
}
void BaseWindow::msg_keyup_long(WPARAM wParam, LPARAM lParam)  {
    for(auto& pair : registry_widget){
        pair.second->msg_keyup_long(wParam, lParam);
    }
}
void BaseWindow::msg_timer(WPARAM wParam, LPARAM lParam)  {
    for(auto& pair : registry_widget){
        pair.second->msg_timer(wParam, lParam);
    }
}
void BaseWindow::msg_paint(HDC hdc)  {
    for(auto& pair : registry_widget){
        pair.second->msg_paint(hdc);
    }
}
void BaseWindow::msg_close(WPARAM wParam, LPARAM lParam)  {
    for(auto& pair : registry_widget){
        pair.second->msg_close(wParam, lParam);
    }
}
void BaseWindow::msg_nc_lbutton_up(WPARAM wParam, LPARAM lParam)  {
    for(auto& pair : registry_widget){
        pair.second->msg_nc_lbutton_up(wParam, lParam);
    }
}
void BaseWindow::msg_destroy(WPARAM wParam, LPARAM lParam)  {
    for(auto& pair : registry_widget){
        pair.second->msg_destroy(wParam, lParam);
    }
}


std::unordered_map<std::string, BaseWindow*> 
            BaseWindow::registry_window,
            BaseWindow::registry_open_window;

//////////////////////////////////////////////////////////////////////////////

void BaseWindow::Create(const std::string &window_name, BaseWindow &hParent){
	Create(window_name, hParent.hWnd);
}

void BaseWindow::Create(const std::string &window_name, HWND hParent){
	if(hParent==HWND_NULL) {LOG_INFO("父窗口句柄为空"); return ;}
	if(window_name.empty()) {LOG_INFO("窗口名为空"); return ;}
	if(registry_open_window.find(window_name) != registry_open_window.end()){
		LOG_INFOC("窗口已打开", "%s 窗口已打开", window_name.c_str());
		return ;
	}

	auto it = registry_window.find(window_name);
	if(it == registry_window.end()) {LOG_INFO("该窗口没有注册过，找不到对应句柄"); return ;}
	it->second->start(hParent);
}

void BaseWindow::Close(const std::string &window_name){
	auto it = registry_open_window.find(window_name);
    if (it != registry_open_window.end()) {
        PostMessage(it->second->hWnd, MSG_CLOSE, 0, 0); 
    }else{
		LOG_INFOC("窗口未打开", "%s 窗口未打开", window_name.c_str());
    }
}

void BaseWindow::Close(){
	PostMessage(hWnd, MSG_CLOSE, 0, 0); 
}




