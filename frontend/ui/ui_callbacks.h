#ifndef UI_CALLBACKS_H
#define UI_CALLBACKS_H

#include "state/editor_state.h"
#include "../rendering/rendering_engine.h"

namespace Ui {
    void load_button_logic(EditorContext *ctx);
    void save_button_logic(EditorContext *ctx);
    void fetch_dog_button_logic(EditorContext *ctx);
    void draw_sidebar_menu(EditorContext *ctx);
    void undo_button_logic(EditorContext *ctx);
	void crop_button_logic(EditorContext *ctx);
	void rotate_left_button_logic(EditorContext *ctx);
	void rotate_right_button_logic(EditorContext *ctx);

    
    void save_state_for_undo(EditorContext *ctx);
	void handle_select_all_action(EditorContext *ctx);
	bool isImageLoaded(EditorContext *ctx);

    template <typename Func, typename... Args>
    void apply_image_operation(EditorContext *ctx, Func&& op, Args&&... args)
    {
        Ui::save_state_for_undo(ctx);
        std::forward<Func>(op)(std::forward<Args>(args)...);
        Graphics::refresh_image_render(ctx->img_state->image, ctx->t_state);
    }
}

#endif
