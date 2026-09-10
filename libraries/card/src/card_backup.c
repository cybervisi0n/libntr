#include <nitro.h>

#include "../include/card_spi.h"
#include "../include/card_common.h"

#if(defined(SDK_ARM9) || defined(SDK_PORT))

#include <nitro/version_begin.h>
SDK_DEFINE_MIDDLEWARE(cardi_backup_assert, "NINTENDO", "BACKUP");
#include <nitro/version_end.h>
#define SDK_USING_BACKUP() SDK_USING_MIDDLEWARE(cardi_backup_assert)

#if SDK_VERSION_MAJOR == 5
static u8 CARDi_backup_cache_page_buf[256] ATTRIBUTE_ALIGN(32);

#ifdef SDK_PORT
void CARDi_OnFifoRecv(PXIFifoTag tag, u64 data, BOOL err)
#else
void CARDi_OnFifoRecv(PXIFifoTag tag, u32 data, BOOL err)
#endif
{
#pragma unused(data)
  if ((tag == PXI_FIFO_TAG_FS) && err) {
    CARDiCommon *const p = &cardi_common;

    SDK_ASSERT(data == CARD_REQ_ACK);
    p->flag &= ~CARD_STAT_WAITFOR7ACK;
    OS_WakeupThreadDirect(p->current_thread_9);
  }
}

static BOOL CARDi_Request(CARDiCommon *p, int req_type, int retry_count)
{
  if ((p->flag & CARD_STAT_INIT_CMD) == 0) {
    p->flag |= CARD_STAT_INIT_CMD;
    while (!PXI_IsCallbackReady(PXI_FIFO_TAG_FS, PXI_PROC_ARM7)) {
      OS_SpinWait(100);
    }

    (void)CARDi_Request(p, CARD_REQ_INIT, 1);
  }

  DC_FlushRange(p->cmd, sizeof(*p->cmd));
  DC_WaitWriteBufferEmpty();

  p->current_thread_9 = OS_GetCurrentThread();

  do {

    p->flag |= CARD_STAT_WAITFOR7ACK;
    CARDi_SendPxi((u32)req_type);

    switch (req_type) {
    case CARD_REQ_INIT:
      CARDi_SendPxi((u32)p->cmd);
      break;
    }
    {

      OSIntrMode bak_psr = OS_DisableInterrupts();
      while ((p->flag & CARD_STAT_WAITFOR7ACK) != 0) {
        OS_SleepThread(NULL);
      }
      (void)OS_RestoreInterrupts(bak_psr);
    }
    DC_InvalidateRange(p->cmd, sizeof(*p->cmd));
  } while ((p->cmd->result == CARD_RESULT_TIMEOUT) && (--retry_count > 0));

  return (p->cmd->result == CARD_RESULT_SUCCESS);
}
#endif

static void CARDi_RequestStreamCommandCore (CARDiCommon *p)
{
	const int req_type = p->req_type;
	const int req_mode = p->req_mode;
	const int retry_count = p->req_retry;
	u32 size = sizeof(p->backup_cache_page_buf);

	SDK_USING_BACKUP();

	if (req_type == CARD_REQ_ERASE_SECTOR_BACKUP) {
		size = CARD_GetBackupSectorSize();
	} else if (req_type == CARD_REQ_ERASE_SUBSECTOR_BACKUP)   {
		size = cardi_common.cmd->spec.subsect_size;
	}
	do {
		const u32 len = (size < p->len) ? size : p->len;
		p->cmd->len = len;

		if ((p->flag & CARD_STAT_CANCEL) != 0) {
			p->flag &= ~CARD_STAT_CANCEL;
			p->cmd->result = CARD_RESULT_CANCELED;
			break;
		}
		switch (req_mode) {
		case CARD_REQUEST_MODE_RECV:
			DC_InvalidateRange(p->backup_cache_page_buf, len);
			p->cmd->src = (u32)p->src;
			#if SDK_VERSION_MAJOR == 4
			p->cmd->dst = (u32)p->backup_cache_page_buf;
			#elif SDK_VERSION_MAJOR == 5
			p->cmd->dst = (u32)CARDi_backup_cache_page_buf;
			#endif
			break;
		case CARD_REQUEST_MODE_SEND:
		case CARD_REQUEST_MODE_SEND_VERIFY:
			#if SDK_VERSION_MAJOR == 4
			MI_CpuCopy8((const void *)p->src, p->backup_cache_page_buf, len);
			DC_FlushRange(p->backup_cache_page_buf, len);
			#elif SDK_VERSION_MAJOR == 5
      		MI_CpuCopy8((const void *)p->src, CARDi_backup_cache_page_buf, len);
      		DC_FlushRange(CARDi_backup_cache_page_buf, len);
			#endif
			
			DC_WaitWriteBufferEmpty();
			#if SDK_VERSION_MAJOR == 4
			p->cmd->src = (u32)p->backup_cache_page_buf;
			#elif SDK_VERSION_MAJOR == 5
			p->cmd->src = (u32)CARDi_backup_cache_page_buf;
			#endif
			p->cmd->dst = (u32)p->dst;
			break;
		case CARD_REQUEST_MODE_SPECIAL:
			p->cmd->src = (u32)p->src;
			p->cmd->dst = (u32)p->dst;
			break;
		}

		if (!CARDi_Request(p, req_type, retry_count)) {
			break;
		}

		if (req_mode == CARD_REQUEST_MODE_SEND_VERIFY) {
			if (!CARDi_Request(p, CARD_REQ_VERIFY_BACKUP, 1))
				break;
		} else if (req_mode == CARD_REQUEST_MODE_RECV)   {
			#if SDK_VERSION_MAJOR == 4
			MI_CpuCopy8(p->backup_cache_page_buf, (void *)p->dst, len);
			#elif SDK_VERSION_MAJOR == 5
			MI_CpuCopy8(CARDi_backup_cache_page_buf, (void *)p->dst, len);
			#endif
		}
		p->src += len;
		p->dst += len;
		p->len -= len;
	} while (p->len > 0);
	CARDi_EndTask(p, TRUE);
}

static void CARDi_RequestWriteSectorCommandCore (CARDiCommon *p)
{
	const u32 sector_size = CARD_GetBackupSectorSize();
	SDK_USING_BACKUP();

	if ((((u32)p->dst | p->len) & (sector_size - 1)) != 0) {
		p->flag &= ~CARD_STAT_CANCEL;
		p->cmd->result = CARD_RESULT_INVALID_PARAM;
	} else {
		for (; p->len > 0; p->len -= sector_size) {
			u32 len = sector_size;

			if ((p->flag & CARD_STAT_CANCEL) != 0) {
				p->flag &= ~CARD_STAT_CANCEL;
				p->cmd->result = CARD_RESULT_CANCELED;
				break;
			}

			p->cmd->dst = (u32)p->dst;
			p->cmd->len = len;

			if (!CARDi_Request(p, CARD_REQ_ERASE_SECTOR_BACKUP, 1)) {
				break;
			}

			while (len > 0) {
				#if SDK_VERSION_MAJOR == 4
				const u32 page = sizeof(p->backup_cache_page_buf);
				#elif SDK_VERSION_MAJOR == 5
				const u32 page = sizeof(CARDi_backup_cache_page_buf);
				#endif

				if ((p->flag & CARD_STAT_CANCEL) != 0) {
					p->flag &= ~CARD_STAT_CANCEL;
					p->cmd->result = CARD_RESULT_CANCELED;
					break;
				}

				#if SDK_VERSION_MAJOR == 4
				MI_CpuCopy8((const void *)p->src, p->backup_cache_page_buf, page);
				DC_FlushRange(p->backup_cache_page_buf, page);
				#elif SDK_VERSION_MAJOR == 5
				MI_CpuCopy8((const void *)p->src, CARDi_backup_cache_page_buf, page);
				DC_FlushRange(CARDi_backup_cache_page_buf, page);
				#endif
				DC_WaitWriteBufferEmpty();

				#if SDK_VERSION_MAJOR == 4
				p->cmd->src = (u32)p->backup_cache_page_buf;
				#elif SDK_VERSION_MAJOR == 5
				p->cmd->src = (u32)CARDi_backup_cache_page_buf;
				#endif
				p->cmd->dst = (u32)p->dst;
				p->cmd->len = page;

				if (!CARDi_Request(p, CARD_REQ_PROGRAM_BACKUP, CARD_RETRY_COUNT_MAX)) {
					break;
				}

				if (p->req_mode == CARD_REQUEST_MODE_SEND_VERIFY) {
					if (!CARDi_Request(p, CARD_REQ_VERIFY_BACKUP, 1))
						break;
				}
				p->src += page;
				p->dst += page;
				len -= page;
			}
		}
	}
	#if SDK_VERSION_MAJOR == 4
	CARDi_EndTask(p, TRUE);
	#endif
}

#if SDK_VERSION_MAJOR == 5
static void CARDi_AccessStatusCore(CARDiCommon *p)
{
  CARDRequest command = (CARDRequest)CARDi_backup_cache_page_buf[1];
  DC_FlushRange(CARDi_backup_cache_page_buf, 1);
  p->cmd->src = (u32)CARDi_backup_cache_page_buf;
  p->cmd->dst = (u32)CARDi_backup_cache_page_buf;
  (void)CARDi_Request(p, command, 1);
  DC_InvalidateRange(CARDi_backup_cache_page_buf, 1);
}

static void CARDi_IdentifyBackupCore2(CARDBackupType type)
{
  {
    CARDiCommandArg *const p = cardi_common.cmd;

    MI_CpuFill8(&p->spec, 0, sizeof(p->spec));
    p->type = type;
    p->spec.caps = (CARD_BACKUP_CAPS_AVAILABLE | CARD_BACKUP_CAPS_READ_STATUS);
    if (type != CARD_BACKUP_TYPE_NOT_USE) {

      const u32 size = (u32)(1 << ((type >> CARD_BACKUP_TYPE_SIZEBIT_SHIFT) &
                                   CARD_BACKUP_TYPE_SIZEBIT_MASK));
      const int device = ((type >> CARD_BACKUP_TYPE_DEVICE_SHIFT) &
                          CARD_BACKUP_TYPE_DEVICE_MASK);
      const int vender = ((type >> CARD_BACKUP_TYPE_VENDER_SHIFT) &
                          CARD_BACKUP_TYPE_VENDER_MASK);

      p->spec.total_size = size;

      p->spec.initial_status = 0xFF;
      if (device == CARD_BACKUP_TYPE_DEVICE_EEPROM) {
        switch (size) {
        default:
          goto invalid_type;
        case 0x000200: // CARD_BACKUP_TYPE_EEPROM_4KBITS
          p->spec.page_size = 0x10;
          p->spec.addr_width = 1;
          p->spec.program_page = 5;
          p->spec.initial_status = 0xF0;
          break;
        case 0x002000: // CARD_BACKUP_TYPE_EEPROM_64KBITS
          p->spec.page_size = 0x0020;
          p->spec.addr_width = 2;
          p->spec.program_page = 5;
          p->spec.initial_status = 0x00;
          break;
        case 0x010000: // CARD_BACKUP_TYPE_EEPROM_512KBITS
          p->spec.page_size = 0x0080;
          p->spec.addr_width = 2;
          p->spec.program_page = 10;
          p->spec.initial_status = 0x00;
          break;
        case 0x020000: // CARD_BACKUP_TYPE_EEPROM_1MBITS
          p->spec.page_size = 0x0100;
          p->spec.addr_width = 3;
          p->spec.program_page = 5;
          p->spec.initial_status = 0x00;
          break;
        }
        p->spec.sect_size = p->spec.page_size;
        p->spec.caps |= CARD_BACKUP_CAPS_READ;
        p->spec.caps |= CARD_BACKUP_CAPS_PROGRAM;
        p->spec.caps |= CARD_BACKUP_CAPS_VERIFY;
        p->spec.caps |= CARD_BACKUP_CAPS_WRITE_STATUS;
      } else if (device == CARD_BACKUP_TYPE_DEVICE_FLASH) {
        switch (size) {
        default:
          goto invalid_type;
        case 0x040000: // CARD_BACKUP_TYPE_FLASH_2MBITS
        case 0x080000: // CARD_BACKUP_TYPE_FLASH_4MBITS
        case 0x100000: // CARD_BACKUP_TYPE_FLASH_8MBITS
          p->spec.write_page = 25;
          p->spec.write_page_total = 300;
          p->spec.erase_page = 300;
          p->spec.erase_sector = 5000;
          p->spec.caps |= CARD_BACKUP_CAPS_WRITE;
          p->spec.caps |= CARD_BACKUP_CAPS_ERASE_PAGE;
          break;
        case 0x200000: // CARD_BACKUP_TYPE_FLASH_16MBITS
          p->spec.write_page = 23;
          p->spec.write_page_total = 300;
          p->spec.erase_sector = 500;
          p->spec.erase_sector_total = 5000;
          p->spec.erase_chip = 10000;
          p->spec.erase_chip_total = 60000;
          p->spec.initial_status = 0x00;
          p->spec.caps |= CARD_BACKUP_CAPS_WRITE;
          p->spec.caps |= CARD_BACKUP_CAPS_ERASE_PAGE;
          p->spec.caps |= CARD_BACKUP_CAPS_ERASE_CHIP;
          p->spec.caps |= CARD_BACKUP_CAPS_WRITE_STATUS;
          break;
        case 0x400000: // CARD_BACKUP_TYPE_FLASH_32MBITS
          p->spec.erase_sector = 600;
          p->spec.erase_sector_total = 3000;
          p->spec.erase_subsector = 70;
          p->spec.erase_subsector_total = 150;
          p->spec.erase_chip = 23000;
          p->spec.erase_chip_total = 800000;
          p->spec.initial_status = 0x00;
          p->spec.subsect_size = 0x1000;
          p->spec.caps |= CARD_BACKUP_CAPS_ERASE_SUBSECTOR;
          p->spec.caps |= CARD_BACKUP_CAPS_ERASE_CHIP;
          p->spec.caps |= CARD_BACKUP_CAPS_WRITE_STATUS;
          break;
        case 0x800000:
          if (vender == 0) // CARD_BACKUP_TYPE_FLASH_64MBITS
          {
            p->spec.erase_sector = 1000;
            p->spec.erase_sector_total = 3000;
            p->spec.erase_chip = 68000;
            p->spec.erase_chip_total = 160000;
            p->spec.initial_status = 0x00;
            p->spec.caps |= CARD_BACKUP_CAPS_ERASE_CHIP;
            p->spec.caps |= CARD_BACKUP_CAPS_WRITE_STATUS;
          } else if (vender == 1) // CARD_BACKUP_TYPE_FLASH_64MBITS_EX
          {
            p->spec.erase_sector = 1000;
            p->spec.erase_sector_total = 3000;
            p->spec.erase_chip = 68000;
            p->spec.erase_chip_total = 160000;
            p->spec.initial_status = 0x84;
            p->spec.caps |= CARD_BACKUP_CAPS_ERASE_CHIP;
            p->spec.caps |= CARD_BACKUP_CAPS_WRITE_STATUS;
          }
          break;
        }
        p->spec.sect_size = 0x010000;
        p->spec.page_size = 0x0100;
        p->spec.addr_width = 3;
        p->spec.program_page = 5;
        p->spec.caps |= CARD_BACKUP_CAPS_READ;
        p->spec.caps |= CARD_BACKUP_CAPS_PROGRAM;
        p->spec.caps |= CARD_BACKUP_CAPS_VERIFY;
        p->spec.caps |= CARD_BACKUP_CAPS_ERASE_SECTOR;
      } else if (device == CARD_BACKUP_TYPE_DEVICE_FRAM) {
        switch (size) {
        default:
          goto invalid_type;
        case 0x002000: // #CARD_BACKUP_TYPE_FRAM_64KBITS
        case 0x008000: // #CARD_BACKUP_TYPE_FRAM_256KBITS
          break;
        }
        p->spec.page_size = size;
        p->spec.sect_size = size;
        p->spec.addr_width = 2;
        p->spec.initial_status = 0x00;
        p->spec.caps |= CARD_BACKUP_CAPS_READ;
        p->spec.caps |= CARD_BACKUP_CAPS_PROGRAM;
        p->spec.caps |= CARD_BACKUP_CAPS_VERIFY;
        p->spec.caps |= CARD_BACKUP_CAPS_WRITE_STATUS;
      } else {
      invalid_type:
        p->type = CARD_BACKUP_TYPE_NOT_USE;
        p->spec.total_size = 0;
        cardi_common.cmd->result = CARD_RESULT_UNSUPPORTED;
        return;
      }
    }
  }
}

static void CARDi_IdentifyBackupCore(CARDiCommon * p)
{
  (void)CARDi_Request(p, CARD_REQ_IDENTIFY, 1);

  p->cmd->src = 0;
  p->cmd->dst = (u32)CARDi_backup_cache_page_buf;
  p->cmd->len = 1;
  (void)CARDi_Request(p, CARD_REQ_READ_BACKUP, 1);
}

static void CARDi_BeginBackupCommand(CARDAccessLevel level, MIDmaCallback callback, void *arg)
{
  SDK_USING_BACKUP();
  SDK_ASSERT(CARD_IsAvailable());
  SDK_TASSERTMSG(CARDi_GetTargetMode() == CARD_TARGET_BACKUP,
                 "[CARD] current locking target is not backup.");
  CARD_CheckEnabled();
  if ((CARDi_GetAccessLevel() & level) != level) {
    OS_TPanic("this program cannot %s%s CARD-backup!",
              (level & CARD_ACCESS_LEVEL_BACKUP_R) ? "READ" : "",
              (level & CARD_ACCESS_LEVEL_BACKUP_W) ? "WRITE" : "");
  }
  (void)CARDi_WaitForTask(&cardi_common, TRUE, callback, arg);
}
#endif


BOOL CARDi_RequestStreamCommand (u32 src, u32 dst, u32 len,
                                 MIDmaCallback callback, void *arg, BOOL is_async,
                                 CARDRequest req_type, int req_retry, CARDRequestMode req_mode)
{
	#if SDK_VERSION_MAJOR == 4
	CARDiCommon *const p = &cardi_common;

	SDK_USING_BACKUP();

	SDK_ASSERT(CARD_IsAvailable());
	SDK_ASSERT(CARD_GetCurrentBackupType() != CARD_BACKUP_TYPE_NOT_USE);
	SDK_ASSERTMSG(CARDi_GetTargetMode() == CARD_TARGET_BACKUP,
	              "[CARD] current locking target is not backup.");

	CARDi_WaitTask(p, callback, arg);
	p->src = src;
	p->dst = dst;
	p->len = len;
	p->req_type = req_type;
	p->req_retry = req_retry;
	p->req_mode = req_mode;
	if (is_async) {
		CARDi_SetTask(CARDi_RequestStreamCommandCore);
		return TRUE;
	} else {
		cardi_common.cur_th = OS_GetCurrentThread();
		CARDi_RequestStreamCommandCore(p);
		return (p->cmd->result == CARD_RESULT_SUCCESS);
	}
	#elif SDK_VERSION_MAJOR == 5
  	CARDAccessLevel level = (req_mode == CARD_REQUEST_MODE_RECV)
  	                            ? CARD_ACCESS_LEVEL_BACKUP_R
  	                            : CARD_ACCESS_LEVEL_BACKUP_W;
  	SDK_ASSERT(CARD_GetCurrentBackupType() != CARD_BACKUP_TYPE_NOT_USE);

  	CARDi_BeginBackupCommand(level, callback, arg);

  	{
  	  CARDiCommon *p = &cardi_common;
  	  p->src = src;
  	  p->dst = dst;
  	  p->len = len;
  	  p->req_type = req_type;
  	  p->req_retry = req_retry;
  	  p->req_mode = req_mode;
  	}

  	return CARDi_ExecuteOldTypeTask(CARDi_RequestStreamCommandCore, is_async);
	#endif
}

int CARDi_AccessStatus (CARDRequest command, u8 value)
{
	#if SDK_VERSION_MAJOR == 4
	CARDiCommon *const p = &cardi_common;

	SDK_USING_BACKUP();

	SDK_ASSERT(CARD_IsAvailable());
	SDK_ASSERT(CARD_GetCurrentBackupType() != CARD_BACKUP_TYPE_NOT_USE);
	SDK_ASSERTMSG(CARDi_GetTargetMode() == CARD_TARGET_BACKUP,
	              "[CARD] current locking target is not backup.");

	CARDi_WaitTask(p, NULL, NULL);
	cardi_common.cur_th = OS_GetCurrentThread();
	p->backup_cache_page_buf[0] = value;
	DC_FlushRange(p->backup_cache_page_buf, 1);
	p->cmd->src = (u32)p->backup_cache_page_buf;
	p->cmd->dst = (u32)p->backup_cache_page_buf;
	(void)CARDi_Request(p, command, 1);
	DC_InvalidateRange(p->backup_cache_page_buf, 1);
	CARDi_EndTask(p, TRUE);
	return (p->cmd->result == CARD_RESULT_SUCCESS) ? p->backup_cache_page_buf[0] : -1;
	#elif SDK_VERSION_MAJOR == 5
	SDK_ASSERT(CARD_GetCurrentBackupType() != CARD_BACKUP_TYPE_NOT_USE);

	CARDi_BeginBackupCommand(CARD_ACCESS_LEVEL_NONE, NULL, NULL);

	CARDi_backup_cache_page_buf[0] = value;
	CARDi_backup_cache_page_buf[1] = (u8)command;

	return CARDi_ExecuteOldTypeTask(CARDi_AccessStatusCore, FALSE)
	           ? CARDi_backup_cache_page_buf[0]
	           : -1;

	#endif
}

BOOL CARDi_RequestWriteSectorCommand (u32 src, u32 dst, u32 len, BOOL verify, MIDmaCallback callback, void *arg, BOOL is_async)
{
	#if SDK_VERSION_MAJOR == 4
	CARDiCommon *const p = &cardi_common;

	SDK_USING_BACKUP();

	SDK_ASSERT(CARD_IsAvailable());
	SDK_ASSERT(CARD_GetCurrentBackupType() != CARD_BACKUP_TYPE_NOT_USE);
	SDK_ASSERTMSG(CARDi_GetTargetMode() == CARD_TARGET_BACKUP,
	              "[CARD] current locking target is not backup.");

	CARDi_WaitTask(p, callback, arg);
	p->src = src;
	p->dst = dst;
	p->len = len;
	p->req_mode = verify ? CARD_REQUEST_MODE_SEND_VERIFY : CARD_REQUEST_MODE_SEND;
	if (is_async) {
		CARDi_SetTask(CARDi_RequestWriteSectorCommandCore);
		return TRUE;
	} else {
		cardi_common.cur_th = OS_GetCurrentThread();
		CARDi_RequestWriteSectorCommandCore(p);
		return (p->cmd->result == CARD_RESULT_SUCCESS);
	}
	#elif SDK_VERSION_MAJOR == 5
  	SDK_ASSERT(CARD_GetCurrentBackupType() != CARD_BACKUP_TYPE_NOT_USE);

  	CARDi_BeginBackupCommand(CARD_ACCESS_LEVEL_BACKUP_W, callback, arg);

  	{
  	  CARDiCommon *p = &cardi_common;
  	  p->src = src;
  	  p->dst = dst;
  	  p->len = len;
  	  p->req_mode =
  	      verify ? CARD_REQUEST_MODE_SEND_VERIFY : CARD_REQUEST_MODE_SEND;
  	}

  	return CARDi_ExecuteOldTypeTask(CARDi_RequestWriteSectorCommandCore,
  	                                is_async);
	#endif
}



CARDBackupType CARD_GetCurrentBackupType (void)
{
	SDK_ASSERT(CARD_IsAvailable());
	return cardi_common.cmd->type;
}

u32 CARD_GetBackupTotalSize (void)
{
	SDK_ASSERT(CARD_IsAvailable());
	return cardi_common.cmd->spec.total_size;
}

u32 CARD_GetBackupSectorSize (void)
{
	SDK_ASSERT(CARD_IsAvailable());
	return cardi_common.cmd->spec.sect_size;
}

u32 CARD_GetBackupPageSize (void)
{
	SDK_ASSERT(CARD_IsAvailable());
	return cardi_common.cmd->spec.page_size;
}

BOOL CARD_IdentifyBackup (CARDBackupType type)
{
	#if SDK_VERSION_MAJOR == 4
	CARDiCommon *const p = &cardi_common;

	SDK_USING_BACKUP();
	if (type == CARD_BACKUP_TYPE_NOT_USE) {
		OS_TPanic("cannot specify CARD_BACKUP_TYPE_NOT_USE.");
	}

	SDK_ASSERT(CARD_IsAvailable());

	SDK_ASSERTMSG((CARDi_GetTargetMode() == CARD_TARGET_BACKUP),
	              "CARD_IdentifyBackup() must be called with CARD locked by CARD_LockBackup()!");

	CARD_CheckEnabled();

	CARDi_WaitTask(p, NULL, NULL);
	CARDi_IdentifyBackupCore(type);
	cardi_common.cur_th = OS_GetCurrentThread();
	(void)CARDi_Request(p, CARD_REQ_IDENTIFY, 1);

	p->cmd->src = 0;
	p->cmd->dst = (u32)p->backup_cache_page_buf;
	p->cmd->len = 1;
	(void)CARDi_Request(p, CARD_REQ_READ_BACKUP, 1);
	CARDi_EndTask(p, TRUE);
	return (p->cmd->result == CARD_RESULT_SUCCESS);
	#elif SDK_VERSION_MAJOR == 5
 	if (type == CARD_BACKUP_TYPE_NOT_USE) {
 	  OS_TPanic("cannot specify CARD_BACKUP_TYPE_NOT_USE.");
 	}

 	CARDi_BeginBackupCommand(CARD_ACCESS_LEVEL_NONE, NULL, NULL);

 	CARDi_IdentifyBackupCore2(type);

 	return CARDi_ExecuteOldTypeTask(CARDi_IdentifyBackupCore, FALSE);
	#endif
}

BOOL CARD_WaitBackupAsync (void)
{
	return CARDi_WaitAsync();
}

BOOL CARD_TryWaitBackupAsync (void)
{
	return CARDi_TryWaitAsync();
}

void CARD_CancelBackupAsync (void)
{
	OSIntrMode bak_cpsr = OS_DisableInterrupts();
	cardi_common.flag |= CARD_STAT_CANCEL;
	(void)OS_RestoreInterrupts(bak_cpsr);
}

#endif
