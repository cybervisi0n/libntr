#include "screenquads.h"

static G3SIM_Vertex_t s_VerticalScreenQuad[] = {
    {
        .x = -1.0f,
        .y = -1.0f,
        .z = 1.0f,
        .s = 0.0f,
        .t = 0.0f,
    },
    {
        .x = -1.0f,
        .y = 1.0f,
        .z = 1.0f,
        .s = 0.0f,
        .t = 1.0f,
    },
    {
        .x = 1.0f,
        .y = -1.0f,
        .z = 1.0f,
        .s = 1.0f,
        .t = 0.0f,
    },
    {
        .x = 1.0f,
        .y = -1.0f,
        .z = 1.0f,
        .s = 1.0f,
        .t = 0.0f,
    },
    {
        .x = 1.0f,
        .y = 1.0f,
        .z = 1.0f,
        .s = 1.0f,
        .t = 1.0f,
    },
    {
        .x = -1.0f,
        .y = 1.0f,
        .z = 1.0f,
        .s = 0.0f,
        .t = 1.0f,
    }
};

static G3SIM_Vertex_t s_HorizontalScreenQuad[] = {
    //Top Screen
    {
        .x = -1.0f,
        .y = -1.0f,
        .z = 1.0f,
        .s = 0.0f,
        .t = 0.5f,
    },
    {
        .x = -1.0f,
        .y = 1.0f,
        .z = 1.0f,
        .s = 0.0f,
        .t = 1.0f,
    },
    {
        .x = 0.0f,
        .y = -1.0f,
        .z = 1.0f,
        .s = 1.0f,
        .t = 0.5f,
    },
    {
        .x = 0.0f,
        .y = -1.0f,
        .z = 1.0f,
        .s = 1.0f,
        .t = 0.5f,
    },
    {
        .x = 0.0f,
        .y = 1.0f,
        .z = 1.0f,
        .s = 1.0f,
        .t = 1.0f,
    },
    {
        .x = -1.0f,
        .y = 1.0f,
        .z = 1.0f,
        .s = 0.0f,
        .t = 1.0f,
    },
    // Bottom Screen
    {
        .x = 0.0f,
        .y = -1.0f,
        .z = 1.0f,
        .s = 0.0f,
        .t = 0.0f,
    },
    {
        .x = 0.0f,
        .y = 1.0f,
        .z = 1.0f,
        .s = 0.0f,
        .t = 0.5f,
    },
    {
        .x = 1.0f,
        .y = -1.0f,
        .z = 1.0f,
        .s = 1.0f,
        .t = 0.0f,
    },
    {
        .x = 1.0f,
        .y = -1.0f,
        .z = 1.0f,
        .s = 1.0f,
        .t = 0.0f,
    },
    {
        .x = 1.0f,
        .y = 1.0f,
        .z = 1.0f,
        .s = 1.0f,
        .t = 0.5f,
    },
    {
        .x = 0.0f,
        .y = 1.0f,
        .z = 1.0f,
        .s = 0.0f,
        .t = 0.5f,
    }
};

static G3SIM_Vertex_t s_VerticalScreenQuadSwapped[] = {
    // Bottom Screen
    {
        .x = -1.0f,
        .y = -1.0f,
        .z = 1.0f,
        .s = 0.0f,
        .t = 0.5f,
    },
    {
        .x = -1.0f,
        .y = 0.0f,
        .z = 1.0f,
        .s = 0.0f,
        .t = 1.0f,
    },
    {
        .x = 1.0f,
        .y = -1.0f,
        .z = 1.0f,
        .s = 1.0f,
        .t = 0.5f,
    },
    {
        .x = 1.0f,
        .y = -1.0f,
        .z = 1.0f,
        .s = 1.0f,
        .t = 0.5f,
    },
    {
        .x = 1.0f,
        .y = 0.0f,
        .z = 1.0f,
        .s = 1.0f,
        .t = 1.0f,
    },
    {
        .x = -1.0f,
        .y = 0.0f,
        .z = 1.0f,
        .s = 0.0f,
        .t = 1.0f,
    },
    //Top Screen
    {
        .x = -1.0f,
        .y = 0.0f,
        .z = 1.0f,
        .s = 0.0f,
        .t = 0.0f,
    },
    {
        .x = -1.0f,
        .y = 1.0f,
        .z = 1.0f,
        .s = 0.0f,
        .t = 0.5f,
    },
    {
        .x = 1.0f,
        .y = 0.0f,
        .z = 1.0f,
        .s = 1.0f,
        .t = 0.0f,
    },
    {
        .x = 1.0f,
        .y = 0.0f,
        .z = 1.0f,
        .s = 1.0f,
        .t = 0.0f,
    },
    {
        .x = 1.0f,
        .y = 1.0f,
        .z = 1.0f,
        .s = 1.0f,
        .t = 0.5f,
    },
    {
        .x = -1.0f,
        .y = 1.0f,
        .z = 1.0f,
        .s = 0.0f,
        .t = 0.5f,
    }
};

static G3SIM_Vertex_t s_HorizontalScreenQuadSwapped[] = {
    // Bottom Screen
    {
        .x = -1.0f,
        .y = -1.0f,
        .z = 1.0f,
        .s = 0.0f,
        .t = 0.0f,
    },
    {
        .x = -1.0f,
        .y = 1.0f,
        .z = 1.0f,
        .s = 0.0f,
        .t = 0.5f,
    },
    {
        .x = 0.0f,
        .y = -1.0f,
        .z = 1.0f,
        .s = 1.0f,
        .t = 0.0f,
    },
    {
        .x = 0.0f,
        .y = -1.0f,
        .z = 1.0f,
        .s = 1.0f,
        .t = 0.0f,
    },
    {
        .x = 0.0f,
        .y = 1.0f,
        .z = 1.0f,
        .s = 1.0f,
        .t = 0.5f,
    },
    {
        .x = -1.0f,
        .y = 1.0f,
        .z = 1.0f,
        .s = 0.0f,
        .t = 0.5f,
    },
    //Top Screen
    {
        .x = 0.0f,
        .y = -1.0f,
        .z = 1.0f,
        .s = 0.0f,
        .t = 0.5f,
    },
    {
        .x = 0.0f,
        .y = 1.0f,
        .z = 1.0f,
        .s = 0.0f,
        .t = 1.0f,
    },
    {
        .x = 1.0f,
        .y = -1.0f,
        .z = 1.0f,
        .s = 1.0f,
        .t = 0.5f,
    },
    {
        .x = 1.0f,
        .y = -1.0f,
        .z = 1.0f,
        .s = 1.0f,
        .t = 0.5f,
    },
    {
        .x = 1.0f,
        .y = 1.0f,
        .z = 1.0f,
        .s = 1.0f,
        .t = 1.0f,
    },
    {
        .x = 0.0f,
        .y = 1.0f,
        .z = 1.0f,
        .s = 0.0f,
        .t = 1.0f,
    }
};

static G3SIM_Vertex_t s_LargeScreenQuad[] = {
    //Top Screen
    {
        .x = -1.0f,
        .y = -1.0f,
        .z = 1.0f,
        .s = 0.0f,
        .t = 0.5f,
    },
    {
        .x = -1.0f,
        .y = 1.0f,
        .z = 1.0f,
        .s = 0.0f,
        .t = 1.0f,
    },
    {
        .x = 0.33f,
        .y = -1.0f,
        .z = 1.0f,
        .s = 1.0f,
        .t = 0.5f,
    },
    {
        .x = 0.33f,
        .y = -1.0f,
        .z = 1.0f,
        .s = 1.0f,
        .t = 0.5f,
    },
    {
        .x = 0.33f,
        .y = 1.0f,
        .z = 1.0f,
        .s = 1.0f,
        .t = 1.0f,
    },
    {
        .x = -1.0f,
        .y = 1.0f,
        .z = 1.0f,
        .s = 0.0f,
        .t = 1.0f,
    },
    // Bottom Screen
    {
        .x = 0.33f,
        .y = -1.0f,
        .z = 1.0f,
        .s = 0.0f,
        .t = 0.0f,
    },
    {
        .x = 0.33f,
        .y = 0.0f,
        .z = 1.0f,
        .s = 0.0f,
        .t = 0.5f,
    },
    {
        .x = 1.0f,
        .y = -1.0f,
        .z = 1.0f,
        .s = 1.0f,
        .t = 0.0f,
    },
    {
        .x = 1.0f,
        .y = -1.0f,
        .z = 1.0f,
        .s = 1.0f,
        .t = 0.0f,
    },
    {
        .x = 1.0f,
        .y = 0.0f,
        .z = 1.0f,
        .s = 1.0f,
        .t = 0.5f,
    },
    {
        .x = 0.33f,
        .y = 0.0f,
        .z = 1.0f,
        .s = 0.0f,
        .t = 0.5f,
    }
};

static G3SIM_Vertex_t s_LargeScreenQuadSwapped[] = {
    // Bottom Screen
    {
        .x = -1.0f,
        .y = -1.0f,
        .z = 1.0f,
        .s = 0.0f,
        .t = 0.0f,
    },
    {
        .x = -1.0f,
        .y = 1.0f,
        .z = 1.0f,
        .s = 0.0f,
        .t = 0.5f,
    },
    {
        .x = 0.33f,
        .y = -1.0f,
        .z = 1.0f,
        .s = 1.0f,
        .t = 0.0f,
    },
    {
        .x = 0.33f,
        .y = -1.0f,
        .z = 1.0f,
        .s = 1.0f,
        .t = 0.0f,
    },
    {
        .x = 0.33f,
        .y = 1.0f,
        .z = 1.0f,
        .s = 1.0f,
        .t = 0.5f,
    },
    {
        .x = -1.0f,
        .y = 1.0f,
        .z = 1.0f,
        .s = 0.0f,
        .t = 0.5f,
    },
    //Top Screen
    {
        .x = 0.33f,
        .y = -1.0f,
        .z = 1.0f,
        .s = 0.0f,
        .t = 0.5f,
    },
    {
        .x = 0.33f,
        .y = 0.0f,
        .z = 1.0f,
        .s = 0.0f,
        .t = 1.0f,
    },
    {
        .x = 1.0f,
        .y = -1.0f,
        .z = 1.0f,
        .s = 1.0f,
        .t = 0.5f,
    },
    {
        .x = 1.0f,
        .y = -1.0f,
        .z = 1.0f,
        .s = 1.0f,
        .t = 0.5f,
    },
    {
        .x = 1.0f,
        .y = 0.0f,
        .z = 1.0f,
        .s = 1.0f,
        .t = 1.0f,
    },
    {
        .x = 0.33f,
        .y = 0.0f,
        .z = 1.0f,
        .s = 0.0f,
        .t = 1.0f,
    }
};

G3SIM_Vertex_t * sim_GetScreenQuadArray(SIM_config_screen_layout_type layout, BOOL isSwapped)
{
    switch(layout) {
        default:
        case SIM_CONFIG_SCREEN_LAYOUT_VERTICAL:
            return isSwapped ? s_VerticalScreenQuadSwapped : s_VerticalScreenQuad;
        case SIM_CONFIG_SCREEN_LAYOUT_HORIZONTAL:
            return isSwapped ? s_HorizontalScreenQuadSwapped : s_HorizontalScreenQuad;
        case SIM_CONFIG_SCREEN_LAYOUT_LARGE:
            return isSwapped ? s_LargeScreenQuadSwapped : s_LargeScreenQuad;
    }
}

int sim_GetScreenQuadVertexCount(SIM_config_screen_layout_type layout, BOOL isSwapped)
{
    switch(layout) {
        default:
        case SIM_CONFIG_SCREEN_LAYOUT_VERTICAL:
            return isSwapped ? 12 : 6;
        case SIM_CONFIG_SCREEN_LAYOUT_HORIZONTAL:
        case SIM_CONFIG_SCREEN_LAYOUT_LARGE:
            return 12;
    }
}