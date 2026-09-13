#include <nitro.h>
#include <nitro/mi/secureUncompress.h>

#include <nitro/code32.h>

void MIi_UncompressBLZ(void *bottom);

s32 MI_SecureUncompressAny(const void *srcp, u32 srcSize, void *destp,
                           u32 dstSize) {
  switch (MI_GetCompressionType(srcp)) {

  case MI_COMPRESSION_RL:
    return MI_SecureUncompressRL(srcp, srcSize, destp, dstSize);

  case MI_COMPRESSION_LZ:
    return MI_SecureUncompressLZ(srcp, srcSize, destp, dstSize);

  case MI_COMPRESSION_HUFFMAN:
    return MI_SecureUncompressHuffman(srcp, srcSize, destp, dstSize);

  case MI_COMPRESSION_DIFF:
    return MI_SecureUnfilterDiff(srcp, srcSize, destp, dstSize);
  default:
    return MI_ERR_UNSUPPORTED;
  }
}

s32 MI_SecureUncompressRL(const void *srcp, u32 srcSize, void *destp,
                          u32 dstSize) {
  const u8 *pSrc = srcp;
  u8 *pDst = destp;
  u8 compType = (u8)(MI_HToLE32(*(u32 *)pSrc) & 0xFF);
  u32 destCount = MI_HToLE32(*(u32 *)pSrc) >> 8;
  s32 srcCount = (s32)srcSize;

  if ((compType & MI_COMPRESSION_TYPE_MASK) != MI_COMPRESSION_RL) {
    return MI_ERR_UNSUPPORTED;
  }
  if ((compType & 0xF) != 0) {
    return MI_ERR_UNSUPPORTED;
  }
  if (srcSize <= 4) {
    return MI_ERR_SRC_SHORTAGE;
  }

  pSrc += 4;
  srcCount -= 4;

  if (destCount == 0) {
    if (srcCount < 4) {
      return MI_ERR_SRC_SHORTAGE;
    }
    destCount = MI_HToLE32(*(u32 *)pSrc);
    pSrc += 4;
    srcCount -= 4;
  }

  if (dstSize < destCount) {
    return MI_ERR_DEST_OVERRUN;
  }

  while (destCount > 0) {
    u8 flags = *pSrc++;
    s32 length = flags & 0x7f;
    if (--srcCount < 0) {
      return MI_ERR_SRC_SHORTAGE;
    }

    if (!(flags & 0x80)) {
      length++;
      if (length > destCount)

      {
        return MI_ERR_DEST_OVERRUN;
      }
      srcCount -= length;
      if (srcCount < 0) {
        return MI_ERR_SRC_SHORTAGE;
      }

      destCount -= length;
      do {
        *pDst++ = *pSrc++;
      } while (--length > 0);
    } else {
      u8 srcTmp;

      length += 3;
      if (length > destCount)

      {
        return MI_ERR_DEST_OVERRUN;
      }

      destCount -= length;
      srcTmp = *pSrc++;
      if (--srcCount < 0) {
        return MI_ERR_SRC_SHORTAGE;
      }
      do {
        *pDst++ = srcTmp;
      } while (--length > 0);
    }
  }

  if (srcCount > 32) {
    return MI_ERR_SRC_REMAINDER;
  }

  return MI_ERR_SUCCESS;
}

#define SRC_COUNT_CHECK
s32 MI_SecureUncompressLZ(const void *srcp, u32 srcSize, void *destp,
                          u32 dstSize) {
  const u8 *pSrc = srcp;
  u8 *pDst = destp;
  u8 compType = (u8)(MI_HToLE32(*(u32 *)pSrc) & 0xFF);
  u32 destCount = MI_HToLE32(*(u32 *)pSrc) >> 8;
  s32 srcCount = (s32)srcSize;
  BOOL exFormat = (*pSrc & 0x0F) ? TRUE : FALSE;

  if ((compType & MI_COMPRESSION_TYPE_MASK) != MI_COMPRESSION_LZ) {
    return MI_ERR_UNSUPPORTED;
  }
  if (((compType & 0xF) != 0x0) && ((compType & 0xF) != 0x1)) {
    return MI_ERR_UNSUPPORTED;
  }
  if (srcSize <= 4) {
    return MI_ERR_SRC_SHORTAGE;
  }

  pSrc += 4;
  srcCount -= 4;

  if (destCount == 0) {
    if (srcCount < 4) {
      return MI_ERR_SRC_SHORTAGE;
    }

    destCount = MI_HToLE32(*(u32 *)pSrc);
    pSrc += 4;
    srcCount -= 4;
  }

  if (dstSize < destCount) {
    return MI_ERR_DEST_OVERRUN;
  }

  while (destCount > 0) {
    u32 i;
#if !defined(SRC_COUNT_CHECK)
    u32 flags = *pSrc++;
#else
    u32 flags;
#endif
    if (--srcCount < 0) {
      return MI_ERR_SRC_SHORTAGE;
    }
#if defined(SRC_COUNT_CHECK)
    flags = *pSrc++;
#endif

    for (i = 0; i < 8; ++i) {
      if (!(flags & 0x80)) {
#if defined(SRC_COUNT_CHECK)
        if (--srcCount < 0) {
          return MI_ERR_SRC_SHORTAGE;
        }
        destCount--; // Should this also be checked?
#endif
        *pDst++ = *pSrc++;
#if !defined(SRC_COUNT_CHECK)
        if (--srcCount < 0) {
          return MI_ERR_SRC_SHORTAGE;
        }
        destCount--;
#endif
      } else {
        s32 length = (*pSrc >> 4);
        s32 offset;

        if (!exFormat) {
          length += 3;
        } else {

          if (length == 1) {
#if defined(SRC_COUNT_CHECK)
            srcCount -= 2;
            if (srcCount < 0) {
              return MI_ERR_SRC_SHORTAGE;
            }
#endif
            length = (*pSrc++ & 0x0F) << 12;
            length |= (*pSrc++) << 4;
            length |= (*pSrc >> 4);
            length += 0xFF + 0xF + 3;
#if !defined(SRC_COUNT_CHECK)
            srcCount -= 2;
#endif
          } else if (length == 0) {
#if defined(SRC_COUNT_CHECK)
            if (--srcCount < 0) {
              return MI_ERR_SRC_SHORTAGE;
            }
#endif
            length = (*pSrc++ & 0x0F) << 4;
            length |= (*pSrc >> 4);
            length += 0xF + 2;
#if !defined(SRC_COUNT_CHECK)
            srcCount -= 1;
#endif
          } else {
            length += 1;
          }
        }

#if defined(SRC_COUNT_CHECK)
        srcCount -= 2;
        if (srcCount < 0) {
          return MI_ERR_SRC_SHORTAGE;
        }
#endif
        offset = (*pSrc++ & 0x0f) << 8;
        offset = (offset | *pSrc++) + 1;

#if !defined(SRC_COUNT_CHECK)
        srcCount -= 2;
        if (srcCount < 0) {
          return MI_ERR_SRC_SHORTAGE;
        }
#endif

        if (length > destCount) {
          return MI_ERR_DEST_OVERRUN;
        }
        if (&pDst[-offset] < destp) {
          return MI_ERR_DEST_OVERRUN;
        }

        destCount -= length;
        do {
          *pDst = pDst[-offset];
          pDst++;
        } while (--length > 0);
      }
      if (destCount <= 0) {
        break;
      }
      flags <<= 1;
    }
  }

  if (srcCount > 32) {
    return MI_ERR_SRC_REMAINDER;
  }

  return MI_ERR_SUCCESS;
}

extern BOOL MIi_VerifyHuffmanTable_(const void *pTable, u8 bit);

BOOL MIi_VerifyHuffmanTable_(const void *pTable, u8 bit) {
  const u32 FLAGS_ARRAY_NUM = 512 / 8; /* 64Byte */
  u8 *treep = (u8 *)pTable;
  u8 *treeStartp = treep + 1;
  u32 treeSize = *treep;
  u8 *treeEndp = (u8 *)pTable + (treeSize + 1) * 2;
  u32 i;
  u8 end_flags[FLAGS_ARRAY_NUM];
  u32 idx;

  for (i = 0; i < FLAGS_ARRAY_NUM; i++) {
    end_flags[i] = 0;
  }

  if (bit == 4) {
    if (treeSize >= 0x10) {
      return FALSE;
    }
  }

  idx = 1;
  treep = treeStartp;
  while (treep < treeEndp) {
    if ((end_flags[idx / 8] & (1 << (idx % 8))) == 0) {
      u32 offset = (u32)(((*treep & 0x3F) + 1) << 1);
      u8 *nodep = (u8 *)((((u32)treep >> 1) << 1) + offset);

      if (*treep == 0 && idx >= (treeSize * 2)) {
        goto next;
      }

      if (nodep >= treeEndp) {
        return FALSE;
      }
      if (*treep & 0x80) {
        u32 left = (idx & ~0x1) + offset;
        end_flags[left / 8] |= (u8)(1 << (left % 8));
      }
      if (*treep & 0x40) {
        u32 right = (idx & ~0x1) + offset + 1;
        end_flags[right / 8] |= (u8)(1 << (right % 8));
      }
    }
  next:
    ++idx;
    ++treep;
  }
  return TRUE;
}

s32 MI_SecureUncompressHuffman(const void *srcp, u32 srcSize, void *destp,
                               u32 dstSize) {
#define TREE_END 0x80
  const u32 *pSrc = srcp;
  u32 *pDst = destp;
  u8 compType = (u8)(MI_HToLE32(*(u32 *)pSrc) & 0xFF);
  s32 destCount = (s32)(MI_HToLE32(*pSrc) >> 8);
  u8 *treep = (destCount != 0) ? ((u8 *)pSrc + 4) : ((u8 *)pSrc + 8);
  u8 *treeStartp = treep + 1;
  u32 dataBit = *(u8 *)pSrc & 0x0FU;
  u32 destTmp = 0;
  u32 destTmpCount = 0;
  u32 destTmpDataNum = 4 + (dataBit & 0x7);
  s32 srcCount = (s32)srcSize;
  u32 treeSize = (u32)((*treep + 1) << 1);

  if ((compType & MI_COMPRESSION_TYPE_MASK) != MI_COMPRESSION_HUFFMAN) {
    return MI_ERR_UNSUPPORTED;
  }
  if ((dataBit != 4) && (dataBit != 8)) {
    return MI_ERR_UNSUPPORTED;
  }

  if (destCount == 0) {
    if (srcSize < 8 + treeSize) {
      return MI_ERR_SRC_SHORTAGE;
    }
    destCount = (s32)(MI_HToLE32(*(pSrc + 1)));
  } else if (srcSize < 4 + treeSize) {
    return MI_ERR_SRC_SHORTAGE;
  }

  if (!MIi_VerifyHuffmanTable_(treep, (u8)dataBit)) {
    return MI_ERR_ILLEGAL_TABLE;
  }

  pSrc = (u32 *)(treep + treeSize);
  srcCount -= ((u32)pSrc - (u32)srcp);

  if (srcCount < 0) {
    return MI_ERR_SRC_SHORTAGE;
  }

  if (dstSize < destCount) {
    return MI_ERR_DEST_OVERRUN;
  }

  treep = treeStartp;

  while (destCount > 0) {
    s32 srcTmpCount = 32;
    u32 srcTmp = MI_HToLE32(*pSrc++); // Endian strategy
    srcCount -= 4;
    if (srcCount < 0) {
      return MI_ERR_SRC_SHORTAGE;
    }

    while (--srcTmpCount >= 0) {
      u32 treeShift = (srcTmp >> 31) & 0x1;
      u32 treeCheck = *treep;
      treeCheck <<= treeShift;
      treep = (u8 *)((((u32)treep >> 1) << 1) + (((*treep & 0x3f) + 1) << 1) +
                     treeShift);
      if (treeCheck & TREE_END) {
        destTmp >>= dataBit;
        destTmp |= *treep << (32 - dataBit);
        treep = treeStartp;
        ++destTmpCount;

        if (destCount <= (destTmpCount * dataBit) / 8) {
          destTmp >>= (destTmpDataNum - destTmpCount) * dataBit;
          destTmpCount = destTmpDataNum;
        }

        if (destTmpCount == destTmpDataNum) {

          *pDst++ = MI_HToLE32(destTmp); // Endian strategy
          destCount -= 4;
          destTmpCount = 0;
        }
      }
      if (destCount <= 0) {
        break;
      }
      srcTmp <<= 1;
    }
  }
  if (srcCount > 32) {
    return MI_ERR_SRC_REMAINDER;
  }

  return MI_ERR_SUCCESS;
}

static u32 MIi_HuffImportTree(u16 *pTable, const u8 *srcp, u8 bitSize,
                              u32 srcRemainSize) {
  u32 tableSize;
  u32 idx = 1;
  u32 data = 0;
  u32 bitNum = 0;
  u32 bitMask = (1 << bitSize) - 1U;
  u32 srcCnt = 0;
  const u32 MAX_IDX = (u32)((1 << bitSize) * 2);

  if (bitSize > 8) {
    tableSize = MI_HToLE16(*(u16 *)srcp);
    srcp += 2;
    srcCnt += 2;
  } else {
    tableSize = *srcp;
    srcp += 1;
    srcCnt += 1;
  }
  tableSize = (tableSize + 1) * 4;
  if (srcRemainSize < tableSize) {
    return tableSize;
  }

  while (srcCnt < tableSize) {
    while (bitNum < bitSize) {
      data <<= 8;
      data |= *srcp++;
      ++srcCnt;
      bitNum += 8;
    }
    if (idx < MAX_IDX) {
      pTable[idx++] = (u16)((data >> (bitNum - bitSize)) & bitMask);
    }
    bitNum -= bitSize;
  }
  return tableSize;
}

s32 MI_SecureUnfilterDiff(register const void *srcp, u32 srcSize,
                          register void *destp, u32 dstSize) {
  const u8 *pSrc = srcp;
  u8 *pDst = destp;
  u32 bitSize = *pSrc & 0xFU;
  u8 compType = (u8)(MI_HToLE32(*(u32 *)pSrc) & 0xFF);
  s32 destCount = (s32)(MI_HToLE32(*(u32 *)pSrc) >> 8);
  u32 sum = 0;
  s32 srcCount = (s32)srcSize;

  if ((compType & MI_COMPRESSION_TYPE_MASK) != MI_COMPRESSION_DIFF) {
    return MI_ERR_UNSUPPORTED;
  }
  if ((bitSize != 0) && (bitSize != 1)) {
    return MI_ERR_UNSUPPORTED;
  }
  if (srcSize <= 4) {
    return MI_ERR_SRC_SHORTAGE;
  }
  if (dstSize < destCount) {
    return MI_ERR_DEST_OVERRUN;
  }

  pSrc += 4;
  srcCount -= 4;

  if (bitSize != 1) {

    do {
      u8 tmp = *(pSrc++);
      if (--srcCount < 0) {
        return MI_ERR_SRC_SHORTAGE;
      }
      destCount--;
      sum += tmp;
      *(pDst++) = (u8)sum;
    } while (destCount > 0);
  } else {

    do {
      u16 tmp = MI_HToLE16(*(u16 *)pSrc);
      pSrc += 2;
      srcCount -= 2;
      if (srcCount < 0) {
        return MI_ERR_SRC_SHORTAGE;
      }
      destCount -= 2;
      sum += tmp;
      *(u16 *)pDst = MI_HToLE16((u16)sum);
      pDst += 2;
    } while (destCount > 0);
  }
  if (srcCount > 32) {
    return MI_ERR_SRC_REMAINDER;
  }

  return MI_ERR_SUCCESS;
}

s32 MI_SecureUncompressBLZ(const void *srcp, u32 srcSize, u32 dstSize) {

  if ((*(u32 *)((u32)srcp + srcSize - 8) & 0xFFFFFF) > srcSize) {
    return MI_ERR_UNSUPPORTED;
  }

  if (*(u8 *)((u32)srcp + srcSize - 5) < 0x08 &&
      *(u8 *)((u32)srcp + srcSize - 5) > 0x0B) {
    return MI_ERR_UNSUPPORTED;
  }

  if (*(u32 *)((u32)srcp + srcSize - 4) + srcSize > dstSize) {
    return MI_ERR_DEST_OVERRUN;
  }
  MIi_UncompressBLZ((void *)((u32)srcp + srcSize));
  return 0;
}

#ifdef SDK_BUILD_ARM
asm void MIi_UncompressBLZ(register void *bottom) {
#define data r0
#define inp_top r1
#define outp r2
#define inp r3
#define outp_save r4
#define flag r5
#define count8 r6
#define index r7
#define dummy r8
#define len r12
  cmp bottom, #0 beq @exit stmfd sp !, {r4 - r8} ldmdb bottom,
      {r1 - r2} add outp, bottom, outp sub inp, bottom, inp_top,
      LSR #24 bic inp_top, inp_top, #0xff000000 sub inp_top, bottom,
      inp_top mov outp_save,
      outp

      @loop : cmp inp,
              inp_top // exit if inp==inp_top
                  ble @end_loop ldrb flag,
              [ inp, # - 1 ] ! // r4 = compress_flag = *--inp
              mov count8,
              #8 @loop8 : subs count8,
                          count8,
                          #1 blt @loop tst flag,
                          #0x80 bne @blockcopy @bytecopy
      : ldrb data,
        [ inp, # - 1 ] !ldrb dummy,
        [ outp, # - 1 ] // dummy read to use D-Cache efficiently
        strb data,
        [ outp, # - 1 ] ! // Copy 1 byte
        b @joinhere @blockcopy : ldrb len,
                                 [ inp, # - 1 ] !ldrb index,
                                 [ inp, # - 1 ] !orr index,
                                 index,
                                 len,
                                 LSL #8 bic index,
                                 index,
                                 #0xf000 add index,
                                 index,
                                 #0x0002 add len,
                                 len,
                                 #0x0020 @patterncopy
      : ldrb data,
        [ outp, index ] ldrb dummy,
        [ outp, # - 1 ] // dummy read to use D-Cache efficiently
        strb data,
        [ outp, # - 1 ] !subs len,
        len,
        #0x0010 bge @patterncopy

        @joinhere : cmp inp,
                    inp_top mov flag,
                    flag,
                    LSL #1 bgt @loop8 @end_loop :

      mov r0,
      #0 bic inp,
      inp_top,
      #HW_CACHE_LINE_SIZE - 1 @cacheflush : mcr p15,
      0,
      r0,
      c7,
      c10,
      4 // wait writebuffer empty
      mcr p15,
      0,
      inp,
      c7,
      c5,
      1 // ICache
      mcr p15,
      0,
      inp,
      c7,
      c14,
      1 // DCache
      add inp,
      inp,
      #HW_CACHE_LINE_SIZE cmp inp,
      outp_save blt @cacheflush

          ldmfd sp !,
  {
    r4 - r8
  }
  @exit bx lr
}
#endif
