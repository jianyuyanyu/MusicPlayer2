#pragma once
#include "UIElement/UIElement.h"
namespace UiElement
{
    class StackElement;

    //导航栏
    class NavigationBar : public Element
    {
    public:
        virtual void Draw() override;
        virtual void LButtonUp(CPoint point) override;
        virtual void MouseMove(CPoint point) override;
        virtual bool RButtunUp(CPoint point) override;
        virtual void MouseLeave() override;

        enum IconType
        {
            ICON_AND_TEXT,
            ICON_ONLY,
            TEXT_ONLY
        };

        enum Orientation
        {
            Horizontal,
            Vertical,
        };

        IconType icon_type{};
        Orientation orientation{ Horizontal };
        int item_space{};
        int item_height{ 28 };
        int font_size{ 9 };
        std::vector<std::string> tab_list;
        std::vector<CRect> item_rects;
        std::vector<std::wstring> labels;
        int SelectedIndex();
        int hover_index{ -1 };
    private:
        void FindStackElement();        //查找StackElement
        bool find_stack_element{};      //如果已经查找过StackElement，则为true
        StackElement* stack_element{};
        int selected_index{};
        int last_hover_index{ -1 };
    };
}

