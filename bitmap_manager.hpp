#pragma once

#include "base_attr.hpp"

class BitmapManager{
public:
//	int findIndex(BITMAP* bmp); 
	
	std::vector<BITMAP*> m_loadedBitmaps; // 存储已加载位图指针的容器

	// 加载位图资源 
	int load(BITMAP& bmp, const char* path){
		BITMAP* pBmp = &bmp;
		if (IsManaged(pBmp)) {
			// 找到已存在的索引
	        int index = findIndex(pBmp);
            LOG_DEBUGC("位图加载", "位图 %p 已经加载过了", pBmp);
            return index;
        }

		if (LoadBitmap(HDC_SCREEN, &bmp, path) != 0) {
            LOG_DEBUGC("位图加载", "%s fail", path);
            return -1;
        }

		m_loadedBitmaps.push_back(pBmp);
        int index = static_cast<int>(m_loadedBitmaps.size()) - 1;
	    LOG_DEBUGC("位图加载", "%s success, index: %d, managed count: %zu", path, index, m_loadedBitmaps.size());
	    
	    return index;
	}

	// 卸载全部照片资源
	void unLoadAll() {
        for (auto* bmp : m_loadedBitmaps) {
            if (bmp) {
                UnloadBitmap(bmp);
            }
        }
        m_loadedBitmaps.clear();
        LOG_DEBUG("位图加载", "All managed bitmaps unloaded");
    }

	// 通过索引获取位图数据
	PBITMAP get(size_t index){
		if (index >= m_loadedBitmaps.size() || index < 0) {
            return nullptr;
        }
        return m_loadedBitmaps[index];
	}

	// 通过索引来绘画位图照片
	void paint(HDC hdc, RECT rc, size_t index){
		PBITMAP pbitmap = get(index);
		if(pbitmap==nullptr) {LOG_DEBUG("位图", "位图不存在，无法绘制"); return ;}
		FillBoxWithBitmap(hdc, rc.left, rc.top, RECTW(rc), RECTH(rc), pbitmap);
	}

	void paint(HDC hdc, int x, int y, size_t index){
		PBITMAP pbitmap = get(index);
		if(pbitmap==nullptr) {LOG_DEBUG("位图", "位图不存在，无法绘制"); return ;}
		FillBoxWithBitmap(hdc, x, y, pbitmap->bmWidth, pbitmap->bmHeight, pbitmap);
	}

	// 查找位图指针在 vector 中的索引，未找到返回 -1
	int findIndex(BITMAP* bmp) {
		auto it = std::find(m_loadedBitmaps.begin(), m_loadedBitmaps.end(), bmp);
		if (it != m_loadedBitmaps.end()) {
			return static_cast<int>(it - m_loadedBitmaps.begin());
		}
		return -1;
	}

	
	// 获取当前管理的数量（用于调试）
    size_t getCount() const {
        return m_loadedBitmaps.size();
    }
	
	BitmapManager() = default;
	virtual ~BitmapManager(){
		unLoadAll();
	}
private:
	// 辅助函数：检查指针是否在 vector 中
    bool IsManaged(BITMAP* bmp) {
        return findIndex(bmp) != -1;
    }
};



