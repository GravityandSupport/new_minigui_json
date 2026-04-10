#pragma once

#include <cstddef>


namespace ui{

/*
========== 示例类型模板 ==========
struct LangText {
    const char* texts[3];  // ?统一成数组
};

========== 使用模板 ==========
LangText g_texts[] = {
    {{"Pressure", "压力", "壓力"}},
    {{"Temperature", "温度", "溫度"}}
};

LanguageManager<LangText> langMgr;

langMgr.setText(g_texts, 2, 3); // 2条文本，3种语言

========== 全局语言管理 ==========
LanguageManager<LangText>& lang_manager = LanguageManager<LangText>getInstance();

*/

template<typename T>
class LanguageManager {
public:
    const T* texts_ = nullptr;
    size_t texts_size_ = 0;
    size_t lang_count_ = 0;

public:
	static LanguageManager& getInstance(){
		static LanguageManager lang;
		return lang;
	}

    void setText(const T* text, size_t size, size_t lang_count) {
        texts_ = text;
        texts_size_ = size;
        lang_count_ = lang_count;
    }

    const char* getText(size_t id, size_t lang) const {
        if (!texts_ || id >= texts_size_ || lang >= lang_count_) {
            return nullptr;
        }

        return texts_[id].texts[lang];
    }

    operator bool() const {
        return (texts_ != nullptr && texts_size_ > 0);
    }
};


}

