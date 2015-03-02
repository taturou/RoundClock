#include <pebble.h>
#include "round_layer.h"

#define NUM_SECTIONS    (1)
#define HEADER_HIGHT    (0)
#define MARGIN_CELLS    (5)
#define DELAY_SELECTED0 (500)

typedef struct round_layer {
    MenuLayer *layer;
    int16_t cell_hight;
    RoundData *data;
    uint16_t data_size;
    GTextAlignment alignment;
    AppTimer *timer;
} RoundLayer;

static uint16_t s_get_num_sections_callback(struct MenuLayer *menu_layer, void *callback_context);
static uint16_t s_get_num_rows_callback(struct MenuLayer *menu_layer, uint16_t section_index, void *callback_context);
static int16_t s_get_cell_hight_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context);
static int16_t s_get_header_height_callback(struct MenuLayer *menu_layer, uint16_t section_index, void *callback_context);
static void s_draw_row_callback(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *callback_context);
static void s_draw_separator_callback(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *callback_context);
static void s_set_selected_0(void *data);

RoundLayer *round_layer_create(GRect frame, int16_t cell_hight, const RoundData *data, uint16_t data_size, GTextAlignment alignment) {
    RoundLayer *round = NULL;
    
    round = calloc(sizeof(RoundLayer), 1);
    if (round != NULL) {
        round->layer = menu_layer_create(frame);
        if (round->layer != NULL) {
            round->cell_hight = cell_hight;

            round->data = calloc(sizeof(RoundData), data_size);
            if (round->data != NULL) {
                memcpy(round->data, data, sizeof(RoundData) * data_size);
                round->data_size = data_size;
                round->alignment = alignment;

                menu_layer_set_callbacks(
                    round->layer,
                    (void*)round,
                    (MenuLayerCallbacks){
                        .get_num_sections = s_get_num_sections_callback,
                        .get_num_rows = s_get_num_rows_callback,
                        .get_cell_height = s_get_cell_hight_callback,
                        .get_header_height = s_get_header_height_callback,
                        .draw_row = s_draw_row_callback,
                        .draw_separator = s_draw_separator_callback,
                    });            
            } else {
                menu_layer_destroy(round->layer);
                free(round);
                round = NULL;
            }
        } else {
            free(round);
            round = NULL;
        }
    }
    return round;
}

void round_layer_destroy(RoundLayer *round) {
    if (round != NULL) {
        if (round->data != NULL) {
            free(round->data);
        }
        if (round->layer != NULL) {
            menu_layer_destroy(round->layer);
        }
        free(round);
    }
}

Layer *round_layer_get_layer(RoundLayer *round) {
    return menu_layer_get_layer(round->layer);
}

void round_layer_set_selected_index(RoundLayer *round, uint16_t index, bool animated) {
    index = index % round->data_size;

    // to smooth rounding
    // The index is selected to the max once and will be moved to 0 at 500 milliseconds later.
    if ((index == 0)
        && (animated == true)
        && ((menu_layer_get_selected_index(round->layer).row - MARGIN_CELLS) != index)) {
        index = round->data_size;

        if (round->timer != NULL) {
            app_timer_cancel(round->timer);
        }
        round->timer = app_timer_register(DELAY_SELECTED0, s_set_selected_0, round);
    }

    menu_layer_set_selected_index(round->layer, (MenuIndex){0, index + MARGIN_CELLS}, MenuRowAlignCenter, true);
}

static uint16_t s_get_num_sections_callback(struct MenuLayer *menu_layer, void *callback_context) {
    (void)menu_layer;
    (void)callback_context;
    return NUM_SECTIONS;
}

static uint16_t s_get_num_rows_callback(struct MenuLayer *menu_layer, uint16_t section_index, void *callback_context) {
    (void)menu_layer;
    RoundLayer *round = (RoundLayer*)callback_context;
    
    return round->data_size + (MARGIN_CELLS * 2);
}

static int16_t s_get_cell_hight_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context) {
    (void)menu_layer;
    (void)cell_index;
    RoundLayer *round = (RoundLayer*)callback_context;
    
    return round->cell_hight;
}
    
static int16_t s_get_header_height_callback(struct MenuLayer *menu_layer, uint16_t section_index, void *callback_context) {
    (void)menu_layer;
    (void)section_index;
    (void)callback_context;
    return HEADER_HIGHT;
}

static void s_draw_row_callback(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *callback_context) {
    RoundLayer *round = (RoundLayer*)callback_context;

    int index;
    if (cell_index->row < MARGIN_CELLS) {
        index = (round->data_size - MARGIN_CELLS) + cell_index->row;
    } else if (cell_index->row < (MARGIN_CELLS + round->data_size)) {
        index = cell_index->row - MARGIN_CELLS;
    } else {
        index = cell_index->row - (MARGIN_CELLS + round->data_size);
    }
    
    GRect frame = layer_get_bounds(cell_layer);
    if (round->alignment == GTextAlignmentRight) {
        frame.size.w -= 3; // magic number
    }

    graphics_context_set_text_color(ctx, GColorBlack);
    graphics_draw_text(
        ctx,
        round->data[index].str,
        fonts_get_system_font(FONT_KEY_GOTHIC_18),
        frame,
        GTextOverflowModeWordWrap,
        round->alignment,
        NULL);    
}

static void s_draw_separator_callback(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *callback_context) {
    (void)ctx;
    (void)cell_layer;
    (void)cell_index;
    (void)callback_context;
    /* do nothing to draw */
}

static void s_set_selected_0(void *data) {
    RoundLayer *round = (RoundLayer*)data;

    round->timer = NULL;

    menu_layer_set_selected_index(
        round->layer,
        (MenuIndex){
            0,
            MARGIN_CELLS},
        MenuRowAlignCenter,
        false);
}