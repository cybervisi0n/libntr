#ifndef	__WLLIB_H_
#define	__WLLIB_H_

#include <nitro_wl/ARM7/WlBuf.h>
#include <nitro_wl/ARM7/WlFrame.h>
#include <nitro_wl/ARM7/WlCmd.h>
#include <nitro_wl/ARM7/WlCmdLabel.h>
#include <nitro_wl/ARM7/WlStaList.h>
#include <nitro_wl/ARM7/WlParam.h>

typedef	struct
{
	#ifdef SDK_BUILD_ARM
	u32					workingMemAdrs;
	#else
	u64					workingMemAdrs;
	#endif
	void*				stack;
	u32					stacksize;
	u32					priority;
	OSMessageQueue*		sendMsgQueuep;
	OSMessageQueue*		recvMsgQueuep;
	u32					dmaChannel;
	u32					dmaMaxSize;
	u32					heapType;
	union
	{
		struct
		{
			OSArenaId		id;
			OSHeapHandle	heapHandle;
		} os;
		struct
		{
			u32	(*alloc)(u32 size);
			u32	(*free)(void* p);
		} ext;
	} heapFunc;

	void*				camAdrs;
	u32					camSize;
} WlInit;

u32       WL_InitDriver(WlInit* init);
OSThread* WL_GetThreadStruct(void);
void	  WL_Terminate(void);

#ifdef SDK_BUILD_ARM
#define	HW_WIRELESS_INTF0		0x04800000
#define HW_WIRELESS_INTF1       0x04808000
#else
extern u8 s_HW_WIRELESS_INTF0[0x8000];
extern u8 s_HW_WIRELESS_INTF1[0x8000];
#define HW_WIRELESS_INTF0 ((u64)s_HW_WIRELESS_INTF0)
#define HW_WIRELESS_INTF1 ((u64)s_HW_WIRELESS_INTF1)
#endif
#ifndef	MAC_REG_BASE
#define	MAC_REG_BASE			(HW_WIRELESS_INTF1)
#endif

#define	WL_WORKADRS_BUF_ADRS	(HW_PRV_WRAM_SYSRV + 0x34)
#define	WL_VTSF_ADRS			(HW_PRV_WRAM_SYSRV + 0x30)

#define	V_TSF					(WL_VTSF_ADRS)
#define	V_TSF_L					(WL_VTSF_ADRS)
#define	V_TSF_H					(WL_VTSF_ADRS+1)


#endif
