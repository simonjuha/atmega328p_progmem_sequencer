#pragma once
enum Mode{
    MODE_ACTIVE,
    MODE_LENGTH,
    MODE_RATE,
    MODE_OFFSET,
    MODE_BANK,
};

enum Command{
    CMD_SET_MODE,
    CMD_PRESS_STEP,
    CMD_SELECT_STEP,
    CMD_SHIFTA,
    CMD_SHIFTB,
    CMD_SHIFTC,
    CMD_SET_GLOBAL_MODE,
};
