#include <pebble.h>
#include <round_layer.h>

static Window *s_window;

#define MONTH               (0)
#define DAY                 (1)
#define WDAY                (2)
#define HOUR                (3)
#define MIN                 (4)
#define SEC                 (5)
#define MAX_ROUND_LAYER     (6)

static RoundLayer *s_round_layer[MAX_ROUND_LAYER];
static const RoundData s_round_data_month[] = {
    {" 1"}, {" 2"}, {" 3"}, {" 4"}, {" 5"}, {" 6"},
    {" 7"}, {" 8"}, {" 9"}, {"10"}, {"11"}, {"12"}
};
static const RoundData s_round_data_day[] = {
    {"01"}, {"02"}, {"03"}, {"04"}, {"05"},
    {"06"}, {"07"}, {"08"}, {"09"}, {"10"},
    {"11"}, {"12"}, {"13"}, {"14"}, {"15"},
    {"16"}, {"17"}, {"18"}, {"19"}, {"20"},
    {"21"}, {"22"}, {"23"}, {"24"}, {"25"},
    {"26"}, {"27"}, {"28"}, {"29"}, {"30"},
    {"31"}
};
static const RoundData s_round_data_wday[] = {
    {"Sun"}, {"Mon"}, {"Tue"}, {"Wed"}, {"Thu"}, {"Fri"}, {"Sat"}
};
static const RoundData s_round_data_hour[] = {
    {" 0"}, {" 1"}, {" 2"}, {" 3"}, {" 4"}, {" 5"},
    {" 6"}, {" 7"}, {" 8"}, {" 9"}, {"10"}, {"11"},
    {"12"}, {"13"}, {"14"}, {"15"}, {"16"}, {"17"},
    {"18"}, {"19"}, {"20"}, {"21"}, {"22"}, {"23"}
};
static const RoundData s_round_data_min_sec[] = {
    {"00"}, {"01"}, {"02"}, {"03"}, {"04"}, {"05"},
    {"06"}, {"07"}, {"08"}, {"09"}, {"10"}, {"11"},
    {"12"}, {"13"}, {"14"}, {"15"}, {"16"}, {"17"},
    {"18"}, {"19"}, {"20"}, {"21"}, {"22"}, {"23"},
    {"24"}, {"25"}, {"26"}, {"27"}, {"28"}, {"29"},
    {"30"}, {"31"}, {"32"}, {"33"}, {"34"}, {"35"},
    {"36"}, {"37"}, {"38"}, {"39"}, {"40"}, {"41"},
    {"42"}, {"43"}, {"44"}, {"45"}, {"46"}, {"47"},
    {"48"}, {"49"}, {"50"}, {"51"}, {"52"}, {"53"},
    {"54"}, {"55"}, {"56"}, {"57"}, {"58"}, {"59"}
};
struct {
    uint16_t origin_x;
    uint16_t size_w;
    const RoundData *data;
    uint16_t data_size;
    GTextAlignment alignment;
} s_round_data[MAX_ROUND_LAYER] = {
    {0, 22, s_round_data_month, sizeof(s_round_data_month) / sizeof(RoundData), GTextAlignmentRight},
    {22, 22, s_round_data_day, sizeof(s_round_data_day) / sizeof(RoundData), GTextAlignmentRight},
    {44, 28, s_round_data_wday, sizeof(s_round_data_wday) / sizeof(RoundData), GTextAlignmentCenter},
    {72, 24, s_round_data_hour, sizeof(s_round_data_hour) / sizeof(RoundData), GTextAlignmentRight},
    {96, 24, s_round_data_min_sec, sizeof(s_round_data_min_sec) / sizeof(RoundData), GTextAlignmentRight},
    {120, 24, s_round_data_min_sec, sizeof(s_round_data_min_sec) / sizeof(RoundData), GTextAlignmentRight},
};

#define TEXT_SLASH          (0)
#define TEXT_COLON1         (1)
#define TEXT_COLON2         (2)
#define MAX_TEXT_LAYER      (3)
static TextLayer *s_text_layer[MAX_TEXT_LAYER];
struct {
    uint16_t origin_x;
    char *str;
} s_text_data[MAX_TEXT_LAYER] = {
    {20, "/"},
    {96, ":"},
    {120, ":"}
};

static void s_tick_handler(struct tm *tick_time, TimeUnits units_changed) {
    time_t now_time = time(NULL);
    struct tm now_tm;
    memcpy(&now_tm, localtime(&now_time), sizeof(struct tm));

    round_layer_set_selected_index(s_round_layer[MONTH], now_tm.tm_mon);
    round_layer_set_selected_index(s_round_layer[DAY], now_tm.tm_mday - 1);
    round_layer_set_selected_index(s_round_layer[WDAY], now_tm.tm_wday);
    round_layer_set_selected_index(s_round_layer[HOUR], now_tm.tm_hour);
    round_layer_set_selected_index(s_round_layer[MIN], now_tm.tm_min);
    round_layer_set_selected_index(s_round_layer[SEC], now_tm.tm_sec);
}

static void s_window_load(Window *window) {
    Layer *window_layer = window_get_root_layer(window);
    GRect window_bounds = layer_get_frame(window_layer);

    for (int i = 0; i < MAX_ROUND_LAYER; i++) {
        s_round_layer[i] = round_layer_create(
            (GRect){
                .origin = {
                    s_round_data[i].origin_x, // (window_bounds.size.w / MAX_ROUND_LAYER) * i,
                    0
                },
                    .size = {
                    s_round_data[i].size_w, // window_bounds.size.w / MAX_ROUND_LAYER,
                    window_bounds.size.h
                }
            },
            24,
            s_round_data[i].data,
            s_round_data[i].data_size,
            s_round_data[i].alignment
        );
        layer_add_child(window_layer, round_layer_get_layer(s_round_layer[i]));
        APP_LOG(APP_LOG_LEVEL_DEBUG, "x:%d, w:%d", (window_bounds.size.w / MAX_ROUND_LAYER) * i, window_bounds.size.w / MAX_ROUND_LAYER);
    }

    // /
    uint16_t origin_y = (window_bounds.size.h / 2) - 12;
    GSize size = {20, 24};
    
    for (int i = 0; i < MAX_TEXT_LAYER; i++) {
        s_text_layer[i] = text_layer_create((GRect){.origin = {s_text_data[i].origin_x, origin_y}, .size = size});
        text_layer_set_text(s_text_layer[i], s_text_data[i].str);
        text_layer_set_background_color(s_text_layer[i], GColorClear);
        text_layer_set_text_color(s_text_layer[i], GColorWhite);
        text_layer_set_font(s_text_layer[i], fonts_get_system_font(FONT_KEY_GOTHIC_18));
        layer_add_child(window_layer, text_layer_get_layer(s_text_layer[i]));
    }

    tick_timer_service_subscribe(SECOND_UNIT, s_tick_handler);
}

static void s_window_unload(Window *window) {
    for (int i = 0; i < MAX_ROUND_LAYER; i++) {
        round_layer_destroy(s_round_layer[i]);
    }
}

int main(void) {
    s_window = window_create();
    window_set_fullscreen(s_window, true);

    window_set_window_handlers(s_window, (WindowHandlers) {
        .load = s_window_load,
        .unload = s_window_unload,
    });

    window_stack_push(s_window, true /* Animated */);
    app_event_loop();
    window_destroy(s_window);
}