#ifndef UI_WINDOWS_H
#define UI_WINDOWS_H

#include "state/editor_state.h"

namespace Ui {
    static constexpr float kTaskbarHeight = 60.0f;
    static constexpr float kTopMargin = 10.0f;
    static constexpr float kRightPanelWidth = 200.0f;
    static constexpr float kRightPanelGap = 14.0f;
    static constexpr float kSelectionWindowHeight = 130.0f;
    static constexpr float kSidebarTopOffset = 210.0f;

    void draw_image_container(EditorContext *ctx);
    void draw_top_toolbar(EditorContext *ctx);
    void draw_selection_container(EditorContext *ctx);
    void draw_sidebar_container(EditorContext *ctx);
}

#endif