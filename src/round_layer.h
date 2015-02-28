#pragma once
#include <pebble.h>

typedef struct round_layer RoundLayer;

typedef struct round_data {
    char *str;
} RoundData;

RoundLayer *round_layer_create(GRect frame, int16_t cell_hight, const RoundData *data, uint16_t data_size, GTextAlignment alignment);
void round_layer_destroy(RoundLayer *round);
Layer *round_layer_get_layer(RoundLayer *round);
void round_layer_set_selected_index(RoundLayer *round, uint16_t index);