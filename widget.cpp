#include "widget.hpp"
#include "base_window.hpp"
#include "common.hpp"

namespace ui{
Widget::Widget(const std::string& str)
    : BaseAttr(str){

}

void Widget::registerWidget(Widget &widget){
	if(parentWindow==nullptr) {LOG_WARN("空指针", parentWindow, "为空，Widget::registerWidget 调用应该在 BaseWindow::registerWidget 之后"); return;}
	widget.parentWindow = parentWindow;
	widget.parentWidget = this;

	widget.getAbsoluteOffset(); // 基于父控件做偏移
	if(widget.rc.left<rc.left || widget.rc.right>rc.right || widget.rc.top<rc.top || widget.rc.bottom>rc.bottom){
		LOG_WARN("区域越界", "子控件区域不在父区域内！", widget.rc.left, widget.rc.top, widget.rc.right, widget.rc.bottom, rc.left, rc.top, rc.right, rc.bottom);return;
	}
	
    registry_widget.emplace_back(&widget);
}

void Widget::getAbsoluteOffset()
{
    int x = 0;
    int y = 0;

    const Widget* current = this;

    // 一直向父控件递归累加
    while(current != nullptr)
    {
        x += current->rc.left;
        y += current->rc.top;
        current = current->parentWidget;
    }

    int _w = RECTW(rc);
	int _h = RECTH(rc);
	rc.left = x;
	rc.top = y;
	rc.right = x+_w;
	rc.bottom = y+_h;
}


void Widget::msg_init(WPARAM wParam, LPARAM lParam)  {
    // getParentOffset(); // 计算对于父部件的坐标偏移
    if(cb_msg_init) {cb_msg_init(wParam, lParam);}
    for(auto& pair : registry_widget){
        pair->hWnd = hWnd;
        pair->msg_init(wParam, lParam);
    }
}
void Widget::msg_command(WPARAM wParam, LPARAM lParam)  {
	if (cb_msg_command) cb_msg_command(wParam, lParam);
    for(auto& pair : registry_widget){
        pair->msg_command(wParam, lParam);
    }
}
void Widget::msg_lbutton_down(int x, int y)  {
	if (cb_msg_lbutton_down) cb_msg_lbutton_down(x, y);
    for(auto& pair : registry_widget){
        pair->msg_lbutton_down(x, y);
    }
}
void Widget::msg_lbutton_up(int x, int y)  {
	if (cb_msg_lbutton_up) cb_msg_lbutton_up(x, y);
    for(auto& pair : registry_widget){
        pair->msg_lbutton_up(x, y);
    }
}
void Widget::msg_mousemove(int x, int y)  {
	if (cb_msg_mousemove) cb_msg_mousemove(x, y);
    for(auto& pair : registry_widget){
        pair->msg_mousemove(x, y);
    }
}
void Widget::msg_keyup(WPARAM wParam, LPARAM lParam)  {
	if (cb_msg_keyup) cb_msg_keyup(wParam, lParam);
    for(auto& pair : registry_widget){
        pair->msg_keyup(wParam, lParam);
    }
}
void Widget::msg_keydown(WPARAM wParam, LPARAM lParam)  {
	if (cb_msg_keydown) cb_msg_keydown(wParam, lParam);
    for(auto& pair : registry_widget){
        pair->msg_keydown(wParam, lParam);
    }
}
void Widget::msg_key_long_press(WPARAM wParam, LPARAM lParam)  {
	if (cb_msg_key_long_press) cb_msg_key_long_press(wParam, lParam);
    for(auto& pair : registry_widget){
        pair->msg_key_long_press(wParam, lParam);
    }
}
void Widget::msg_keyup_long(WPARAM wParam, LPARAM lParam)  {
	if (cb_msg_keyup_long) cb_msg_keyup_long(wParam, lParam);
    for(auto& pair : registry_widget){
        pair->msg_keyup_long(wParam, lParam);
    }
}
void Widget::msg_timer(WPARAM wParam, LPARAM lParam)  {
	if (cb_msg_timer) cb_msg_timer(wParam, lParam);
    for(auto& pair : registry_widget){
        pair->msg_timer(wParam, lParam);
    }
}
void Widget::msg_paint(HDC hdc)  {
	if (cb_msg_paint) cb_msg_paint(hdc);
    for(auto& pair : registry_widget){
        pair->msg_paint(hdc);
    }
}
void Widget::msg_close(WPARAM wParam, LPARAM lParam)  {
	if (cb_msg_close) cb_msg_close(wParam, lParam);
    for(auto& pair : registry_widget){
        pair->msg_close(wParam, lParam);
    }
}
void Widget::msg_nc_lbutton_up(WPARAM wParam, LPARAM lParam)  {
	if (cb_msg_nc_lbutton_up) cb_msg_nc_lbutton_up(wParam, lParam);
    for(auto& pair : registry_widget){
        pair->msg_nc_lbutton_up(wParam, lParam);
    }
}
void Widget::msg_destroy(WPARAM wParam, LPARAM lParam)  {
	if (cb_msg_destroy) cb_msg_destroy(wParam, lParam);
    for(auto& pair : registry_widget){
        pair->msg_destroy(wParam, lParam);
		pair->hWnd = HWND_NULL;
    }
}
void Widget::key_long_press(WPARAM wParam, LPARAM lParam)  {
	if (cb_key_long_press) cb_key_long_press(wParam, lParam);
    for(auto& pair : registry_widget){
        pair->key_long_press(wParam, lParam);
    }
}

void Widget::updateDirtyArea(){
	if(is_can_update==false) return;
	if(hWnd==HWND_NULL) {LOG_WARN("无效窗口句柄", "hWnd 为空，请确认是否调用 init函数初始化");return;}
	if(parentWindow){parentWindow->dirty_rc_list.push_back(rc);}
	PostMessage(hWnd, MSG_COMMAND, __command_update__, 0);
}






int Widget::bvm_LoadBitmap(const char* path){
	if(parentWindow){
		return parentWindow->bvm_LoadBitmap(path);
	}
	return -1;
}


void Widget::bvm_PaintBitmap(HDC hdc, size_t index, const RECT &rc){
	if(parentWindow){
		parentWindow->bvm_PaintBitmap(hdc, index, rc);
	}
}

void Widget::bvm_PaintBitmap(HDC hdc, size_t index, int x, int y){
	if(parentWindow){
		parentWindow->bvm_PaintBitmap(hdc, index, x, y);
	}
}

void Widget::bvm_PaintBitmap(HDC hdc, size_t index, int x, int y, int w, int h){
	if(parentWindow){
		parentWindow->bvm_PaintBitmap(hdc, index, x, y, w, h);
	}
}

void Widget::bvm_UnloadAllBitmaps(){
	if(parentWindow){
		parentWindow->bvm_UnloadAllBitmaps();
	}
}


void Widget::drawText2(size_t id, size_t lang,
							int x, int y, int w, int h, UINT nFormat){
	if(parentWindow){
		parentWindow->drawText2(id, lang, x, y, w, h, nFormat);
	}
}
void Widget::drawText2(const LanguageManager<LangText>& instance, size_t id, size_t lang,
							int x, int y, int w, int h, UINT nFormat){
	if(parentWindow){
		parentWindow->drawText2(instance, id, lang, x, y, w, h, nFormat);
	}
}

void Widget::drawText(const char* pText, int nCount, 
                			int x, int y, int w, int h, UINT nFormat){
	if(parentWindow){
		parentWindow->drawText(pText, nCount, x, y, w, h, nFormat);
	}
}

void Widget::drawText(int pText, int nCount, 
							int x, int y, int w, int h, UINT nFormat){
	if(parentWindow){
		parentWindow->drawText(pText, nCount, x, y, w, h, nFormat);
	}
}

}




