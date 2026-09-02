#include <nitro/demo/DEMOKey.h>

DEMOKeyWork gKeyWork;

void DEMOReadKey() {
  u16 readData = PAD_Read();
  gKeyWork.trigger = (u16)(readData & (readData ^ gKeyWork.press));
  gKeyWork.press = readData;
}
