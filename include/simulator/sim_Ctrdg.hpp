#ifndef LIBNTR_SIM_CTRDG_HPP
#define LIBNTR_SIM_CTRDG_HPP

// sim_Ctrdg.hpp : C++ class for SIM::CTRDG ( Slot 2 )

#include <nitro/types.h>
#include <nitro/fx/fx.h>

namespace SIM {

class CTRDG {
 public:
  enum class Slot2Type {
    Nothing,      /* Nothing inserted in Slot 2 */
    AgbCartridge, /* GBA Game */
  
    // Below types are not currently supported
    RumblePak,
    MemoryExpansionPak,
    GuitarGrip,
  
    Count
  };

  CTRDG();
  ~CTRDG();

  void Init();
  inline Slot2Type GetSlot2Device() { return mSlot2Device; }
  void SetSlot2Device(Slot2Type device);


 private:
  Slot2Type mSlot2Device = Slot2Type::Nothing;
};

CTRDG& GetCtrdg();

}


#endif