#include "simulator/sim_NitroMainThread.hpp"

#include <cstring>
#include <pthread.h>

#include <nitro/gx/gx_load.h>
#include <nitro/ioreg.h>
#include <nitro/hw/X86/mmap_shared.h>

#include "simulator/sim_RenderContextManager.hpp"

extern "C" void NitroMain();

namespace SIM {

// Initializes Nitro memory and runs NitroMain()
int RunNitroMain(void * arg) {
  SIM::AcquireRenderContext();

  SDL_Thread * currentThread = (SDL_Thread *)arg;

  // Initialize Nitro registers
  s_reg_PAD_KEYINPUT = 0x3ff;
  *(vu16 *)HW_BUTTON_XY_BUF = 0b0111110000000000;

  memset(s_HW_OAM, 0, sizeof(s_HW_OAM));
  memset(s_HW_DB_OAM, 0, sizeof(s_HW_DB_OAM));

  s_reg_GX_VRAMCNT_A = 0;
  s_reg_GX_VRAMCNT_B = 0;
  s_reg_GX_VRAMCNT_C = 0;
  s_reg_GX_VRAMCNT_D = 0;
  s_reg_GX_VRAMCNT_E = 0;
  s_reg_GX_VRAMCNT_F = 0;
  s_reg_GX_VRAMCNT_G = 0;
  s_reg_GX_VRAMCNT_H = 0;
  s_reg_GX_VRAMCNT_I = 0;

  WIN_Init_sTexStartAddrTable();
  WIN_Init_sTexPlttStartAddrTable();

  // Zero out some shared memory
  memset(s_HW_BG_PLTT, 0, sizeof(s_HW_BG_PLTT));
  memset(s_HW_OBJ_PLTT, 0, sizeof(s_HW_OBJ_PLTT));
  memset(s_HW_DB_BG_PLTT, 0, sizeof(s_HW_DB_BG_PLTT));
  memset(s_HW_DB_OBJ_PLTT, 0, sizeof(s_HW_DB_OBJ_PLTT));
  memset(s_HW_BG_VRAM, 0, sizeof(s_HW_BG_VRAM));
  memset(s_HW_DB_BG_VRAM, 0, sizeof(s_HW_DB_BG_VRAM));
  memset(s_HW_OBJ_VRAM, 0, sizeof(s_HW_OBJ_VRAM));
  memset(s_HW_DB_OBJ_VRAM, 0, sizeof(s_HW_DB_OBJ_VRAM));

  // Zero out some registers
  s_reg_G2_BG0CNT = 0;
  s_reg_G2_BG1CNT = 0;
  s_reg_G2_BG2CNT = 0;
  s_reg_G2_BG3CNT = 0;

  s_reg_G2S_DB_BG0CNT = 0;
  s_reg_G2S_DB_BG1CNT = 0;
  s_reg_G2S_DB_BG2CNT = 0;
  s_reg_G2S_DB_BG3CNT = 0;

  NitroMain();

  SIM::ReleaseRenderContext();
}

}

// C APIs
void SIM_ExitNitroMain() {
    SIM::ReleaseRenderContext();

    pthread_exit(nullptr);
}
