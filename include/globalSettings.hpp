#pragma once
enum GlobalSetting // 8 global settings
{
    GLST_CLOCK_EXT,
    GLST_CLOCK_OFFSET,
};

// global settings struct
struct GlobalSettings {
    GlobalSetting setting;
    bool value;
};

// global settings
static GlobalSettings glst[8] = {
    {GLST_CLOCK_EXT, false},
    {GLST_CLOCK_OFFSET, false},
};
