#include <nitro/types.h>
#include <nitro/mi/memory.h>
#if SDK_VERSION_MAJOR == 5
#include <nitro/platform.h>
#include <nitro/math/math.h>
#endif

#ifdef SDK_PORT
#include <nitro/hw/X86/mmap_main.h>
#include "simulator/assert.h"
#endif

#define HALFW_CONDAL  0xe0000000
#define HALFW_CONDNE  0x10000000
#define HALFW_CONDEQ  0x00000000

#define HALFW_OFF_PL  0x00800000
#define HALFW_OFF_MI  0x00000000
#define HALFW_LOAD    0x00100000
#define HALFW_STORE   0x00000000
#define HALFW_RN(n)   ((n) << 16)
#define HALFW_RD(n)   ((n) << 12)

#define HALFW_DEF1    0x004000B0
#define HALFW_DEF2    0x014000B0

#define HALFW_IMM(n)   (((n) & 0xf) | (((n) & 0xf0) << 4))

#define HALFW_DCD(cond, d, n, offset, sign, ldst, def) \
    dcd (def) | (cond) | (sign) | (ldst) | HALFW_RN(n) | HALFW_RD(d) | HALFW_IMM(offset)

#define LDRH_AD1(cond, d, n, offset) \
    HALFW_DCD(cond, d, n, offset, HALFW_OFF_PL, HALFW_LOAD, HALFW_DEF1)

#define LDRH_AD2(cond, d, n, offset) \
    HALFW_DCD(cond, d, n, offset, HALFW_OFF_PL, HALFW_LOAD, HALFW_DEF2)

#define LDRH_AD3(cond, d, n, offset) \
    HALFW_DCD(cond, d, n, offset, HALFW_OFF_MI, HALFW_LOAD, HALFW_DEF1)

#define LDRH_AD4(cond, d, n, offset) \
    HALFW_DCD(cond, d, n, offset, HALFW_OFF_MI, HALFW_LOAD, HALFW_DEF2)

#define STRH_AD1(cond, d, n, offset) \
    HALFW_DCD(cond, d, n, offset, HALFW_OFF_PL, HALFW_STORE, HALFW_DEF1)

#define STRH_AD2(cond, d, n, offset) \
    HALFW_DCD(cond, d, n, offset, HALFW_OFF_PL, HALFW_STORE, HALFW_DEF2)

#define STRH_AD3(cond, d, n, offset) \
    HALFW_DCD(cond, d, n, offset, HALFW_OFF_MI, HALFW_STORE, HALFW_DEF1)

#define STRH_AD4(cond, d, n, offset) \
    HALFW_DCD(cond, d, n, offset, HALFW_OFF_MI, HALFW_STORE, HALFW_DEF2)

#ifdef SDK_PORT
#include <string.h>
#include <nitro/fx/fx.h>

void    MI_CpuCopy8(const void *src, void *dest, u32 size)
{
    SIM_assert_msg(size < HW_MAIN_MEM_MAIN_SIZE, "Attempt to call MI_CpuCopy8 with a size of %d bytes, which is larger than the DS memory.", size);
    memcpy( dest, src, size );
}

void MI_CpuFill8( void *dstp, u8 data, u32 size )
{
    SIM_assert_msg(size < HW_MAIN_MEM_MAIN_SIZE, "Attempt to call MI_CpuFill8 with a size of %d bytes, which is larger than the DS memory.", size);
    u32 myData;
    myData = data | (data << 8) | (data << 16) | (data << 24);
    memset( dstp, myData, size );
}

void MIi_CpuClear16( u16 data, void* destp, u32 size )
{
    SIM_assert_msg(size < HW_MAIN_MEM_MAIN_SIZE, "Attempt to call MIi_CpuClear16 with a size of %d bytes, which is larger than the DS memory.", size);
    //memset( destp, (u32)data | (u32)data << 16, size );
    u16* destp16;
    destp16 = (u16*)destp;
    for( int i=0; i < size >> 1; i ++ )
    {
        *destp16 = data;
        destp16++;
    }
}

void MIi_CpuCopy16( const void *srcp, void *destp, u32 size )
{
    SIM_assert_msg(size < HW_MAIN_MEM_MAIN_SIZE, "Attempt to call MIi_CpuCopy16 with a size of %d bytes, which is larger than the DS memory.", size);
    memcpy( destp, srcp, size );
}

void MI_Copy16B(const void* pSrc, void* pDest)
{
    memcpy( pDest, pSrc, 16);
}

void MIi_CpuClear32( u32 data, void *destp, u32 size )
{
    SIM_assert_msg(size < HW_MAIN_MEM_MAIN_SIZE, "Attempt to call MIi_CpuClear32 with a size of %d bytes, which is larger than the DS memory.", size);
    memset( destp, data, size );
}

void MIi_CpuCopy32( const void *srcp, void *destp, u32 size )
{
    SIM_assert_msg(size < HW_MAIN_MEM_MAIN_SIZE, "Attempt to call MIi_CpuCopy32 with a size of %d bytes, which is larger than the DS memory.", size);
    memcpy( destp, srcp, size );
}

void MI_Copy32B(const void* pSrc, void* pDest)
{
    memcpy( pDest, pSrc, 32);
}

void MI_Copy36B(const void* pSrc, void* pDest)
{
    memcpy( pDest, pSrc, 36);
}

void MIi_CpuSend32( const void *srcp, volatile void *destp, u32 size )
{
    SIM_assert_msg(size < HW_MAIN_MEM_MAIN_SIZE, "Attempt to call MIi_CpuSend32 with a size of %d bytes, which is larger than the DS memory.", size);
    memcpy( destp, srcp, size );
}

void MIi_CpuCopyFast( const void *srcp, void *destp, u32 size )
{
    SIM_assert_msg(size < HW_MAIN_MEM_MAIN_SIZE, "Attempt to call MIi_CpuCopyFast with a size of %d bytes, which is larger than the DS memory.", size);
    memcpy( destp, srcp, size );
}

void MI_Copy48B(const void* pSrc, void* pDest)
{
    memcpy( pDest, pSrc, 48 );
}

void MI_Copy64B(const void* pSrc, void* pDest)
{
    memcpy( pDest, pSrc, 64 );
}

void MI_Zero36B(void* pDest)
{
    memset(pDest, 0, 36);
}

void MI_Copy128B(const void* pSrc, void* pDest)
{
    memcpy( pDest, pSrc, 128 );
}

#if SDK_VERSION_MAJOR == 5
void MI_CpuMove(register const void *srcp, register void *destp,
                    register u32 size) {
    memmove(destp, srcp, size);
}

void MI_CpuFill(void *dest, u8 data, u32 size) {
    memset(dest, data, size);
}
#endif
#else
#include <nitro/code32.h>

asm void MIi_CpuClear16 (register u16 data, register void * destp, register u32 size)
{
    mov r3, #0
@00:
    cmp r3, r2
#ifdef SP1P3_BUG_FOR_CONDITIONAL_ASM_INSTRUCTIONS
    blt @strlth1
    b @strlth2
@strlth1:
    strh  r0, [r1, r3]
@strlth2:
    blt @addlt1
    b @addlt2
@addlt1:
    add r3, r3, #2
@addlt2:
#else
    strlth r0, [r1, r3]
    addlt r3, r3, #2
#endif
    blt @00
    bx lr
}

asm void MIi_CpuCopy16 (register const void * srcp, register void * destp, register u32 size)
{
    mov r12, #0
@10:
    cmp r12, r2
#ifndef CW_BUG_FOR_LDRH_AND_STRH
    #ifdef SP1P3_BUG_FOR_CONDITIONAL_ASM_INSTRUCTIONS
        blt @ldrlth1
        b @ldrlth2
@ldrlth1:
        ldrh r3, [r0, r12]
@ldrlth2:
    #else
        ldrlth r3, [r0, r12]
    #endif
#else
    dcd 0xb19030bc
#endif
#ifndef CW_BUG_FOR_LDRH_AND_STRH
    #ifdef SP1P3_BUG_FOR_CONDITIONAL_ASM_INSTRUCTIONS
        blt @strlth1
        b @strlth2
@strlth1:
        strh r3, [r1, r12]
@strlth2:
    #else
        strlth r3, [r1, r12]
    #endif
#else
    dcd 0xb18130bc
#endif
#ifdef SP1P3_BUG_FOR_CONDITIONAL_ASM_INSTRUCTIONS
    blt @addlt1
    b @addlt2
@addlt1:
    add r12, r12, #2
@addlt2:
#else
    addlt r12, r12, #2
#endif
    blt @10
    bx lr
}

asm void MIi_CpuSend16 (register const void * srcp, register volatile void * destp, register u32 size)
{
    mov r12, #0
@11:
    cmp r12, r2
#ifndef CW_BUG_FOR_LDRH_AND_STRH
    #ifdef SP1P3_BUG_FOR_CONDITIONAL_ASM_INSTRUCTIONS
        blt @ldrlth1
        b @ldrlth2
@ldrlth1:
        ldrh  r3, [r0, r12]
@ldrlth2:
    #else
        ldrlth r3, [r0, r12]
    #endif
#else
    dcd 0xb19030bc
#endif
#ifdef SP1P3_BUG_FOR_CONDITIONAL_ASM_INSTRUCTIONS
    blt @strlth1
    b @strlth2
@strlth1:
    strh r3, [r1, #0]
@strlth2:
    blt @addlt1
    b @addlt2
@addlt1:
    add r12, r12, #2
@addlt2:
#else
    strlth r3, [r1, #0]
    addlt r12, r12, #2
#endif
    blt @11
    bx lr
}

#if SDK_VERSION_MAJOR == 5
asm void MIi_CpuRecv16(register volatile const void *srcp, register void *destp,
                       register u32 size) {
        mov     r12, #0
@12:
        cmp     r12, r2
        ldrlth  r3, [r0]
        strlth  r3, [r1, r12]
        addlt   r12, r12, #2
        blt     @12
        bx      lr
}

asm void MIi_CpuPipe16( register volatile const void *srcp, register volatile void* destp, register u32 size )
{
        mov     r12, #0
@13:
        cmp     r12, r2
        ldrlth  r3, [r0]
        strlth  r3, [r1]
        addlt   r12, r12, #2
        blt     @13

        bx      lr
}

static asm void CpuCopy16Reverse( register const void *srcp, register void *destp, register u32 size )
{
        mov     r12, r1
        add     r0, r0, r2
        add     r1, r1, r2

@14:
        cmp     r12, r1
        ldrlth  r2, [r0, #-2]!
        strlth  r2, [r1, #-2]!
        blt     @14

        bx      lr
}

// NOTES: These are C implementations so can be consistent between port/nonport
void MIi_CpuMove16(const void *src, void *dest, u32 size)
{
    if( ( (u32)dest <= (u32)src )
     || ( (u32)src + size <= (u32)dest ) )
    {
        MIi_CpuCopy16(src, dest, size);
    }
    else
    {
        CpuCopy16Reverse(src, dest, size);
    }
}

void* MIi_CpuFind16(const void *src, u16 data, u32 size)
{
    const u16* p = src;
    u32 i;

    for( i = 0; i < size; i += 2, ++p )
    {
        if( *p == data )
        {
            return (void*)p;
        }
    }

    return NULL;
}

int MIi_CpuComp16(const void *mem1, const void *mem2, u32 size)
{
    const u16* p1 = mem1;
    const u16* p2 = mem2;
    const u16* p1end = (const u16*)( (const u8*)p1 + size );

    while( p1 < p1end )
    {
        int d = (int)*p1++ - (int)*p2++;

        if( d != 0 )
        {
            return d;
        }
    }

    return 0;
}
#endif

asm void MIi_CpuClear32 (register u32 data, register void * destp, register u32 size)
{
    add r12, r1, r2
@20:
    cmp r1, r12
#ifdef SP1P3_BUG_FOR_CONDITIONAL_ASM_INSTRUCTIONS
    blt @stmltia1
    b @stmltia2
@stmltia1:
    stmia r1!, {r0}
@stmltia2:
#else
    stmltia r1 !, {r0}
#endif
    blt @20
    bx lr
}

asm void MIi_CpuCopy32 (register const void * srcp, register void * destp, register u32 size)
{
    add r12, r1, r2
@30:
    cmp r1, r12
#ifdef SP1P3_BUG_FOR_CONDITIONAL_ASM_INSTRUCTIONS
    blt @ldmltia1
    b @ldmltia2
@ldmltia1:
    ldmia r0!, {r2}
@ldmltia2:
    blt @stmltia1
    b @stmltia2
@stmltia1:
    stmia r1!, {r2}
@stmltia2:
#else
    ldmltia r0 !, {r2}
    stmltia r1 !, {r2}
#endif
    blt @30
    bx lr
}

asm void MIi_CpuSend32 (register const void * srcp, volatile void * destp, u32 size)
{
    add r12, r0, r2
@31:
    cmp r0, r12
#ifdef SP1P3_BUG_FOR_CONDITIONAL_ASM_INSTRUCTIONS
    blt @ldmltia1
    b @ldmltia2
@ldmltia1:
    ldmia r0!, {r2}
@ldmltia2:
    blt @strlt1
    b @strlt2
@strlt1:
    str r2, [r1]
@strlt2:
#else
    ldmltia r0 !, {r2}
    strlt r2, [r1]
#endif
    blt @31
    bx lr
}

#if SDK_VERSION_MAJOR == 5
asm void MIi_CpuRecv32( volatile const void *srcp, register void *destp, register u32 size )
{
        add     r12, r1, r2
@32:
        cmp     r1, r12
        ldrlt   r2, [r0]
        stmltia r1!, {r2}
        blt     @32
        bx      lr
}

asm void MIi_CpuPipe32( volatile const void *srcp, register volatile void *destp, register u32 size )
{
        mov     r12, #0      
@33:
        cmp     r12, r2      
        ldrlt   r3, [r0]     
        strlt   r3, [r1]
        addlt   r12, r12, #4 
        blt     @33
        bx      lr
}

static asm void CpuCopy32Reverse( register const void *srcp, register void *destp, register u32 size )
{
        mov     r12, r1       
        add     r0, r0, r2    
        add     r1, r1, r2    
@34:
        cmp     r12, r1       
        ldrlt   r2, [r0, #-4]!
        strlt   r2, [r1, #-4]!
        blt     @34
        bx      lr
}

void MIi_CpuMove32(const void *src, void *dest, u32 size)
{
    if( ( (u32)dest <= (u32)src )
     || ( (u32)src + size <= (u32)dest ) )
    {
        MIi_CpuCopy32(src, dest, size);
    }
    else
    {
        CpuCopy32Reverse(src, dest, size);
    }
}

void* MIi_CpuFind32(const void *src, u32 data, u32 size)
{
    const u32* p = src;
    u32 i;

    for( i = 0; i < size; i += 4, ++p )
    {
        if( *p == data )
        {
            return (void*)p;
        }
    }

    return NULL;
}

int MIi_CpuComp32(const void *mem1, const void *mem2, u32 size)
{
    const u32* p1 = mem1;
    const u32* p2 = mem2;
    const u32* p1end = (const u32*)( (const u8*)p1 + size );

    for( ; p1 < p1end; ++p1, ++p2 )
    {
        const u32 v1 = *p1;
        const u32 v2 = *p2;

        if( v1 != v2 )
        {
            return (v1 < v2) ? -1: 1;
        }
    }

    return 0;
}
#endif

asm void MIi_CpuClearFast (register u32 data, register void * destp, register u32 size)
{
    stmfd sp !, {r4 - r9}
    add r9, r1, r2
    mov r12, r2, lsr #5
    add r12, r1, r12, lsl #5
    mov r2, r0
    mov r3, r2
    mov r4, r2
    mov r5, r2
    mov r6, r2
    mov r7, r2
    mov r8, r2
@40:
    cmp r1, r12
#ifdef SP1P3_BUG_FOR_CONDITIONAL_ASM_INSTRUCTIONS
    blt @stmltia1
    b @stmltia2
@stmltia1:
    stmia r1!, {r0, r2-r8}
@stmltia2:
#else
    stmltia r1 !, {r0, r2 - r8}
#endif
    blt @40
@41:
    cmp r1, r9
#ifdef SP1P3_BUG_FOR_CONDITIONAL_ASM_INSTRUCTIONS
    blt @stmltia3
    b @stmltia4
@stmltia3:
    stmia r1!, {r0}
@stmltia4:
#else
    stmltia r1 !, {r0}
#endif
    blt @41
    ldmfd sp !, {r4 - r9}
    bx lr
}

asm void MIi_CpuCopyFast (register const void * srcp, register void * destp, register u32 size)
{
    stmfd sp !, {r4 - r10}
    add r10, r1, r2
    mov r12, r2, lsr #5
    add r12, r1, r12, lsl #5
@50:
    cmp r1, r12
#ifdef SP1P3_BUG_FOR_CONDITIONAL_ASM_INSTRUCTIONS
    blt @ldmltia1
    b @ldmltia2
@ldmltia1:
    ldmia r0!, {r2-r9}
@ldmltia2:
    blt @stmltia1
    b @stmltia2
@stmltia1:
    stmia r1!, {r2-r9}
@stmltia2:
#else
    ldmltia r0 !, {r2 - r9}
    stmltia r1 !, {r2 - r9}
#endif
    blt @50
@51:
    cmp r1, r10
#ifdef SP1P3_BUG_FOR_CONDITIONAL_ASM_INSTRUCTIONS
    blt @ldmltia3
    b @ldmltia4
@ldmltia3:
    ldmia r0!, {r2}
@ldmltia4:
    blt @stmltia3
    b @stmltia4
@stmltia3:
    stmia r1!, {r2}
@stmltia4:
#else
    ldmltia r0 !, {r2}
    stmltia r1 !, {r2}
#endif
    blt @51
    ldmfd sp !, {r4 - r10}
    bx lr
}

#if SDK_VERSION_MAJOR == 5
asm void MIi_CpuSendFast( register const void *srcp, register volatile void *destp, register u32 size )
{
        stmfd   sp!, {r4-r10}

        add     r10, r0, r2 
        mov     r12, r2, lsr
        add     r12, r0, r12, lsl #5

@50:
        cmp     r0, r12     
        ldmltia r0!, {r2-r9}
        strlt   r2, [r1]
        strlt   r3, [r1]
        strlt   r4, [r1]
        strlt   r5, [r1]
        strlt   r6, [r1]
        strlt   r7, [r1]
        strlt   r8, [r1]
        strlt   r9, [r1]
        blt     @50
@51:
        cmp     r0, r10     
        ldmltia r0!, {r2}   
        strlt   r2, [r1]
        blt     @51

        ldmfd   sp!, {r4-r10}
        bx      lr
}

asm void MIi_CpuRecvFast(volatile const void *srcp, register void *destp, register u32 size)
{
        stmfd   sp!, {r4-r10}

        add     r10, r1, r2    
        mov     r12, r2, lsr #5
        add     r12, r1, r12, lsl #5

@50:
        cmp     r1, r12        
        ldrlt   r2, [r0]       
        ldrlt   r3, [r0]       
        ldrlt   r4, [r0]       
        ldrlt   r5, [r0]       
        ldrlt   r6, [r0]       
        ldrlt   r7, [r0]       
        ldrlt   r8, [r0]       
        ldrlt   r9, [r0]       
        stmltia r1!, {r2-r9}
        blt     @50
@51:
        cmp     r1, r10        
        ldrlt   r2, [r0]       
        stmltia r1!, {r2}
        blt     @51

        ldmfd   sp!, {r4-r10}
        bx      lr
}

static asm void CpuCopyFastReverse( register const void *srcp, register void *destp, register u32 size )
{
        stmfd   sp!, {r4-r10}

        mov     r10, r1        
        mov     r12, r2, lsr #5
        add     r12, r1, r12, lsl #5
        add     r0, r0, r2     
        add     r1, r1, r2     

@52:
        cmp     r12, r1        
        ldrlt   r2, [r0, #-4]! 
        strlt   r2, [r1, #-4]!
        blt     @52
@53:
        cmp     r10, r1        
        ldmltdb r0!, {r2-r9}
        stmltdb r1!, {r2-r9}
        blt     @53

        ldmfd   sp!, {r4-r10}
        bx      lr
}

void MIi_CpuMoveFast(const void *src, void *dest, u32 size)
{
    if( ( (u32)dest <= (u32)src )
     || ( (u32)src + size <= (u32)dest ) )
    {
        MIi_CpuCopyFast(src, dest, size);
    }
    else
    {
        CpuCopyFastReverse(src, dest, size);
    }
}
#endif

asm void MI_Copy16B (register const void * pSrc, register void * pDest)
{
    ldmia r0 !, {r2, r3, r12}
    stmia r1 !, {r2, r3, r12}
    ldmia r0 !, {r2}
    stmia r1 !, {r2}

    bx lr
}

asm void MI_Copy32B (register const void * pSrc, register void * pDest)
{
    ldmia r0 !, {r2, r3, r12}
    stmia r1 !, {r2, r3, r12}
    ldmia r0 !, {r2, r3, r12}
    stmia r1 !, {r2, r3, r12}
    ldmia r0 !, {r2, r3}
    stmia r1 !, {r2, r3}
    bx lr
}

asm void MI_Copy36B (register const void * pSrc, register void * pDest)
{
    ldmia r0 !, {r2, r3, r12}
    stmia r1 !, {r2, r3, r12}
    ldmia r0 !, {r2, r3, r12}
    stmia r1 !, {r2, r3, r12}
    ldmia r0 !, {r2, r3, r12}
    stmia r1 !, {r2, r3, r12}
    bx lr
}

asm void MI_Copy48B (register const void * pSrc, register void * pDest)
{
    ldmia r0 !, {r2, r3, r12}
    stmia r1 !, {r2, r3, r12}
    ldmia r0 !, {r2, r3, r12}
    stmia r1 !, {r2, r3, r12}
    ldmia r0 !, {r2, r3, r12}
    stmia r1 !, {r2, r3, r12}
    ldmia r0 !, {r2, r3, r12}
    stmia r1 !, {r2, r3, r12}
    bx lr
}

asm void MI_Copy64B (register const void * pSrc, register void * pDest)
{
    ldmia r0 !, {r2, r3, r12}
    stmia r1 !, {r2, r3, r12}
    ldmia r0 !, {r2, r3, r12}
    stmia r1 !, {r2, r3, r12}
    ldmia r0 !, {r2, r3, r12}
    stmia r1 !, {r2, r3, r12}
    ldmia r0 !, {r2, r3, r12}
    stmia r1 !, {r2, r3, r12}
    ldmia r0, {r0, r2, r3, r12}
    stmia r1 !, {r0, r2, r3, r12}
    bx lr
}

asm void MI_Copy128B (register const void * pSrc, register void * pDest)
{
    stmfd sp !, {r4}
    ldmia r0 !, {r2, r3, r4, r12}
    stmia r1 !, {r2, r3, r4, r12}
    ldmia r0 !, {r2, r3, r4, r12}
    stmia r1 !, {r2, r3, r4, r12}
    ldmia r0 !, {r2, r3, r4, r12}
    stmia r1 !, {r2, r3, r4, r12}
    ldmia r0 !, {r2, r3, r4, r12}
    stmia r1 !, {r2, r3, r4, r12}
    ldmia r0 !, {r2, r3, r4, r12}
    stmia r1 !, {r2, r3, r4, r12}
    ldmia r0 !, {r2, r3, r4, r12}
    stmia r1 !, {r2, r3, r4, r12}
    ldmia r0 !, {r2, r3, r4, r12}
    stmia r1 !, {r2, r3, r4, r12}
    ldmia r0 !, {r2, r3, r4, r12}
    stmia r1 !, {r2, r3, r4, r12}
    ldmfd sp !, {r4}
    bx lr
}

#ifdef SDK_SMALL_BUILD
    asm void MI_CpuFill8 (register void * dstp, register u8 data, register u32 size)
    {
        mov r12, #0
    @1:
        cmp r12, r2
        strltb r1, [r0, r12]
        addlt r12, r12, #1
        blt @1
        bx lr
    }
#else
    asm void MI_CpuFill8 (register void * dstp, register u8 data, register u32 size)
    {
        cmp r2, #0
        #ifdef SP1P3_BUG_FOR_CONDITIONAL_ASM_INSTRUCTIONS
            beq @bxeq1
            b @bxeq2
@bxeq1:
            bx lr
@bxeq2:
        #else
            bxeq lr
        #endif
        tst r0, #1
        beq @_1
    #ifndef CW_BUG_FOR_LDRH_AND_STRH
        ldrh r12, [r0, #- 1]
    #else
        LDRH_AD4(HALFW_CONDAL, 12, 0, 1)
    #endif
        and r12, r12, #0x00FF
        orr r3, r12, r1, lsl #8
    #ifndef CW_BUG_FOR_LDRH_AND_STRH
        strh r3, [r0, #- 1]
    #else
        STRH_AD4(HALFW_CONDAL, 3, 0, 1)
    #endif
        add r0, r0, #1
        subs r2, r2, #1
    #ifdef SP1P3_BUG_FOR_CONDITIONAL_ASM_INSTRUCTIONS
        beq @bxeq3
        b @bxeq4
@bxeq3:
        bx lr
@bxeq4:
    #else
        bxeq lr
    #endif
    @_1:
        cmp r2, #2
        bcc @_6
        orr r1, r1, r1, lsl #8
        tst r0, #2
        beq @_8
    #ifndef CW_BUG_FOR_LDRH_AND_STRH
        strh r1, [r0], #2
    #else
        STRH_AD1(HALFW_CONDAL, 1, 0, 2)
    #endif
        subs r2, r2, #2
    #ifdef SP1P3_BUG_FOR_CONDITIONAL_ASM_INSTRUCTIONS
        beq @bxeq5
        b @bxeq6
@bxeq5:
        bx lr
@bxeq6:
    #else
        bxeq lr
    #endif
    @_8:
        orr r1, r1, r1, lsl #16
        bics r3, r2, #3
        beq @_10
        sub r2, r2, r3
        add r12, r3, r0
    @_9:
        str r1, [r0], #4
        cmp r0, r12
        bcc @_9
    @_10:
        tst r2, #2
    #ifndef CW_BUG_FOR_LDRH_AND_STRH
        #ifdef SP1P3_BUG_FOR_CONDITIONAL_ASM_INSTRUCTIONS
            bne @strneh1
            b @strneh2
@strneh1:
            strh r1, [r0], #2
@strneh2:
        #else
            strneh r1, [r0], #2
        #endif
    #else
        STRH_AD1(HALFW_CONDNE, 1, 0, 2)
    #endif
    @_6:
        tst r2, #1
    #ifdef SP1P3_BUG_FOR_CONDITIONAL_ASM_INSTRUCTIONS
        beq @bxeq7
        b @bxeq8
@bxeq7:
        bx lr
@bxeq8:
    #else
        bxeq lr
    #endif
    #ifndef CW_BUG_FOR_LDRH_AND_STRH
        ldrh r3, [r0]
    #else
        LDRH_AD2(HALFW_CONDAL, 3, 0, 0)
    #endif
        and r3, r3, #0xFF00
        and r1, r1, #0x00FF
        orr r1, r1, r3
    #ifndef CW_BUG_FOR_LDRH_AND_STRH
        strh r1, [r0]
    #else
        STRH_AD2(HALFW_CONDAL, 1, 0, 0)
    #endif
        bx lr
    }
#endif

#if SDK_VERSION_MAJOR == 5

asm void MI_CpuFill( register void *dstp, register u8 data, register u32 size )
{
    cmp     r2, #0
    bxeq    lr
    cmp     r2, #8
    bgt	    _fill_and_align
    
_fill1_less_than_equal_8:
    rsb     r3, r2, #0x8
    add     pc, pc, r3, lsl #2
    nop
    strb    r1, [r0], #1
    strb    r1, [r0], #1
    strb    r1, [r0], #1
    strb    r1, [r0], #1
    strb    r1, [r0], #1
    strb    r1, [r0], #1
    strb    r1, [r0], #1
    strb    r1, [r0], #1
    bx      lr

_fill_and_align:
    orr     r1, r1, r1, lsl #8
    orr     r1, r1, r1, lsl #16

    tst     r0, #1
    subne   r2, r2, #1
    strneb  r1, [r0], #1
    
    tst     r0, #2
    subne   r2, r2, #2
    strneh  r1, [r0], #2

    tst     r0, #4
    subne   r2, r2, #4
    strne   r1, [r0], #4

_fill32:
    cmp     r2, #32
    blt	    _fill4

_fill32_pre:
    stmfd   sp!, {r4-r10}
    mov     r4, r1
    mov     r5, r1
    mov     r6, r1
    mov     r7, r1
    mov     r8, r1
    mov     r9, r1
    mov     r10, r1
    subs    r2, r2, #32

_fill32_loop:
    stmgeia r0!, {r1,r4-r10}
    subges  r2, r2, #32
    bge     _fill32_loop
    add     r2, r2, #32
    
_fill32_post:
    ldmfd   sp!, {r4-r10}

_fill4:
    cmp     r2, #4
    blt     _fill1_less_than_4
    subs    r2, r2, #4

_fill4_loop:
    strge   r1, [r0], #4
    subs    r2, r2, #4
    bge     _fill4_loop
    add     r2, r2, #4
    
_fill1_less_than_4:
    subs    r2, r2, #1
    strgeb  r1, [r0], #1
    subges  r2, r2, #1
    strgeb  r1, [r0], #1
    subges  r2, r2, #1
    strgeb  r1, [r0], #1

    bx      lr
}
#endif

#ifdef SDK_SMALL_BUILD
    asm void MI_CpuCopy8 (register const void * srcp, register void * dstp, register u32 size)
    {
        mov r12, #0
    @1:
        cmp r12, r2
        ldrltb r3, [r0, r12]
        strltb r3, [r1, r12]
        addlt r12, r12, #1
        blt @1
        bx lr
    }
#else
    asm void MI_CpuCopy8 (register const void * srcp, register void * dstp, register u32 size)
    {
        cmp r2, #0
    #ifdef SP1P3_BUG_FOR_CONDITIONAL_ASM_INSTRUCTIONS
        beq @bxeq1
        b @bxeq2
@bxeq1:
        bx lr
@bxeq2:
    #else
        bxeq lr
    #endif
        tst r1, #1
        beq @_1
    #ifndef CW_BUG_FOR_LDRH_AND_STRH
        ldrh r12, [r1, #- 1]
    #else
        LDRH_AD4(HALFW_CONDAL, 12, 1, 1)
    #endif
        and r12, r12, #0x00FF
        tst r0, #1
    #ifndef CW_BUG_FOR_LDRH_AND_STRH
        #ifdef SP1P3_BUG_FOR_CONDITIONAL_ASM_INSTRUCTIONS
            bne @ldrneh1
            b @ldrneh2
@ldrneh1:
            ldrh r3, [r0, #-1]
@ldrneh2:
        #else
            ldrneh r3, [r0, #- 1]
        #endif
    #else
        LDRH_AD4(HALFW_CONDNE, 3, 0, 1)
    #endif
    #ifdef SP1P3_BUG_FOR_CONDITIONAL_ASM_INSTRUCTIONS
        bne @movne1
        b @movne2
@movne1:
        mov r3, r3, lsr #8
@movne2:
    #else
        movne r3, r3, lsr #8
    #endif
    #ifndef CW_BUG_FOR_LDRH_AND_STRH
        #ifdef SP1P3_BUG_FOR_CONDITIONAL_ASM_INSTRUCTIONS
            beq @ldreqh1
            b @ldreqh2
@ldreqh1:
            ldrh r3, [r0]
@ldreqh2:
        #else
            ldreqh r3, [r0]
        #endif
    #else
        LDRH_AD2(HALFW_CONDEQ, 3, 0, 0)
    #endif
        orr r3, r12, r3, lsl #8
    #ifndef CW_BUG_FOR_LDRH_AND_STRH
        strh r3, [r1, #- 1]
    #else
        STRH_AD4(HALFW_CONDAL, 3, 1, 1)
    #endif
        add r0, r0, #1
        add r1, r1, #1
        subs r2, r2, #1
    #ifdef SP1P3_BUG_FOR_CONDITIONAL_ASM_INSTRUCTIONS
        beq @bxeq3
        b @bxeq4
@bxeq3:
        bx lr
@bxeq4:
    #else
        bxeq lr
    #endif
    @_1:
        eor r12, r1, r0
        tst r12, #1
        beq @_2
        bic r0, r0, #1
    #ifndef CW_BUG_FOR_LDRH_AND_STRH
        ldrh r12, [r0], #2
    #else
        LDRH_AD1(HALFW_CONDAL, 12, 0, 2)
    #endif
        mov r3, r12, lsr #8
        subs r2, r2, #2
        bcc @_3
    @_4:
    #ifndef CW_BUG_FOR_LDRH_AND_STRH
        ldrh r12, [r0], #2
    #else
        LDRH_AD1(HALFW_CONDAL, 12, 0, 2)
    #endif
        orr r12, r3, r12, lsl #8
    #ifndef CW_BUG_FOR_LDRH_AND_STRH
        strh r12, [r1], #2
    #else
        STRH_AD1(HALFW_CONDAL, 12, 1, 2)
    #endif
        mov r3, r12, lsr #16
        subs r2, r2, #2
        bcs @_4
    @_3:
        tst r2, #1
    #ifdef SP1P3_BUG_FOR_CONDITIONAL_ASM_INSTRUCTIONS
        beq @bxeq5
        b @bxeq6
@bxeq5:
        bx lr
@bxeq6:
    #else
        bxeq lr
    #endif
    #ifndef CW_BUG_FOR_LDRH_AND_STRH
        ldrh r12, [r1]
    #else
        LDRH_AD2(HALFW_CONDAL, 12, 1, 0)
    #endif
        and r12, r12, #0xFF00
        orr r12, r12, r3
    #ifndef CW_BUG_FOR_LDRH_AND_STRH
        strh r12, [r1]
    #else
        STRH_AD2(HALFW_CONDAL, 12, 1, 0)
    #endif
        bx lr
    @_2:
        tst r12, #2
        beq @_5
        bics r3, r2, #1
        beq @_6
        sub r2, r2, r3
        add r12, r3, r1
    @_7:
    #ifndef CW_BUG_FOR_LDRH_AND_STRH
        ldrh r3, [r0], #2
    #else
        LDRH_AD1(HALFW_CONDAL, 3, 0, 2)
    #endif
    #ifndef CW_BUG_FOR_LDRH_AND_STRH
        strh r3, [r1], #2
    #else
        STRH_AD1(HALFW_CONDAL, 3, 1, 2)
    #endif
        cmp r1, r12
        bcc @_7
        b @_6
    @_5:
        cmp r2, #2
        bcc @_6
        tst r1, #2
        beq @_8
    #ifndef CW_BUG_FOR_LDRH_AND_STRH
        ldrh r3, [r0], #2
    #else
        LDRH_AD1(HALFW_CONDAL, 3, 0, 2)
    #endif
    #ifndef CW_BUG_FOR_LDRH_AND_STRH
        strh r3, [r1], #2
    #else
        STRH_AD1(HALFW_CONDAL, 3, 1, 2)
    #endif
        subs r2, r2, #2
    #ifdef SP1P3_BUG_FOR_CONDITIONAL_ASM_INSTRUCTIONS
        beq @bxeq7
        b @bxeq8
@bxeq7:
        bx lr
@bxeq8:
    #else
        bxeq lr
    #endif
    @_8:
        bics r3, r2, #3
        beq @_10
        sub r2, r2, r3
        add r12, r3, r1
    @_9:
        ldr r3, [r0], #4
        str r3, [r1], #4
        cmp r1, r12
        bcc @_9
    @_10:
        tst r2, #2
    #ifndef CW_BUG_FOR_LDRH_AND_STRH
        #ifdef SP1P3_BUG_FOR_CONDITIONAL_ASM_INSTRUCTIONS
            bne @ldrneh3
            b @ldrneh4
@ldrneh3:
            ldrh r3, [r0], #2
@ldrneh4:
            bne @strneh1
            b @strneh2
@strneh1:
            strh r3, [r1], #2
@strneh2:
        #else
            ldrneh r3, [r0], #2
            strneh r3, [r1], #2
        #endif
    #else
        LDRH_AD1(HALFW_CONDNE, 3, 0, 2)
        STRH_AD1(HALFW_CONDNE, 3, 1, 2)
    #endif
    @_6:
        tst r2, #1
    #ifdef SP1P3_BUG_FOR_CONDITIONAL_ASM_INSTRUCTIONS
        beq @bxeq9
        b @bxeq10
@bxeq9:
        bx lr
@bxeq10:
    #else
        bxeq lr
    #endif
    #ifndef CW_BUG_FOR_LDRH_AND_STRH
        ldrh r2, [r1]
        ldrh r0, [r0]
    #else
        LDRH_AD2(HALFW_CONDAL, 2, 1, 0)
        LDRH_AD2(HALFW_CONDAL, 0, 0, 0)
    #endif
        and r2, r2, #0xFF00
        and r0, r0, #0x00FF
        orr r0, r2, r0
    #ifndef CW_BUG_FOR_LDRH_AND_STRH
        strh r0, [r1]
    #else
        STRH_AD2(HALFW_CONDAL, 0, 1, 0)
    #endif
        bx lr
    }
#endif

#if SDK_VERSION_MAJOR == 5
// NOTES: this is a C function so it could be the same for port and nonport
void*   MI_CpuFind8(const void *src, u8 data, u32 size)
{
    const u8* p8 = (const u8*)src;

    if( size == 0 )
    {
        return NULL;
    }

    // If the address is not 2-byte aligned
    // Check only 1 byte and align in 2 bytes
    if( ((u32)p8 & 0x1) != 0 )
    {
        const u16 v = *(u16*)(p8 - 1);

        if( (v >> 8) == data )
        {
            return (void*)p8;
        }

        size--;
        p8++;
    }

    // Check in 2-byte units
    {
        const u16* p16 = (const u16*)p8;
        const u16* p16end = p16 + MATH_ROUNDDOWN(size, 2);

        for( ;  p16 < p16end; ++p16 )
        {
            const u16 v = *p16;

            if( (v & 0xFF) == data )
            {
                return (void*)( (u8*)p16 + 0 );
            }
            if( (v >> 8) == data )
            {
                return (void*)( (u8*)p16 + 1 );
            }
        }
    }

    // At this point the size is an odd number
    // Check remaining 1 byte
    if( (size & 0x1) != 0 )
    {
        const u16 v = *(u16*)(p8 + size - 1);

        if( (v & 0xFF) == data )
        {
            return (void*)(p8 + size - 1);
        }
    }

    return NULL;
}

int     MI_CpuComp8(const void *mem1, const void *mem2, u32 size)
{
    const u8* p1 = mem1;
    const u8* p2 = mem2;
    const u8* p1end = (const u8*)( (const u8*)p1 + size );

    while( p1 < p1end )
    {
        const int d = (int)*p1++ - (int)*p2++;

        if( d != 0 )
        {
            return d;
        }
    }

    return 0;
}

#if PLATFORM_BYTES_ENDIAN == PLATFORM_ENDIAN_LITTLE
/* Little-endian */
#define FORWARD_(n)         lsl #((n) * 8)
#define BACKWARD_(n)        lsr #(32 - (n) * 8)
#define FORWARD_MASK(n)     #((1 << ((n) * 8)) - 1)
#elif PLATFORM_BYTES_ENDIAN == PLATFORM_ENDIAN_BIG
/* Big-endian */
#define FORWARD_(n)         lsr #((n) * 8)
#define BACKWARD_(n)        lsl #(32 - (n) * 8)
#define FORWARD_MASK(n)     #((1 << ((n) * 8)) - 1)
#else
#error
#endif

asm void MI_CpuCopy( register const void *srcp, register void *destp, register u32 size )
{
    cmp     r2, #8
    bgt	    _forward_blt
    rsb     r3, r2, #0x8
    add     pc, pc, r3, lsl #3
    nop
    ldrb    r3, [r0], #1
    strb    r3, [r1], #1
    ldrb    r3, [r0], #1
    strb    r3, [r1], #1
    ldrb    r3, [r0], #1
    strb    r3, [r1], #1
    ldrb    r3, [r0], #1
    strb    r3, [r1], #1
    ldrb    r3, [r0], #1
    strb    r3, [r1], #1
    ldrb    r3, [r0], #1
    strb    r3, [r1], #1
    ldrb    r3, [r0], #1
    strb    r3, [r1], #1
    ldrb    r3, [r0], #1
    strb    r3, [r1], #1
    bx      lr

_forward_blt:
    tst     r0, #1
    subne   r2, r2, #1
    ldrneb  r3, [r0], #1
    strneb  r3, [r1], #1
    tst     r0, #2
    subne   r2, r2, #2
    ldrneb  r3, [r0], #1
    strneb  r3, [r1], #1
    ldrneb  r3, [r0], #1
    strneb  r3, [r1], #1

    and     r3, r1, #3
    bic     r1, r1, #3
    cmp     r3, #0
    beq     _forward_blt_0
    cmp     r3, #1
    beq     _forward_blt_1
    cmp     r3, #2
    beq     _forward_blt_2
    b       _forward_blt_3
    
_forward_blt_0:
    stmfd   sp!, {r4-r10,lr}
    subs    r2, r2, #32
_forward_blt_0_32:
    ldmgeia r0!, {r4-r10,lr}
    stmgeia r1!, {r4-r10,lr}
    subges  r2, r2, #32
    bge     _forward_blt_0_32
    add     r2, r2, #32
    ldmfd   sp!, {r4-r10,lr}

    subs    r2, r2, #4
_forward_blt_0_4:
    ldrge   r3, [r0], #4
    strge   r3, [r1], #4
    subges  r2, r2, #4
    bge     _forward_blt_0_4
    add     r2, r2, #4
    b       _forward_blt_end

_forward_blt_1:
#define SHIFT  1
    ldr     r12, [r1]
    mov     r12, r12, FORWARD_(4 - SHIFT)
    mov     r12, r12, BACKWARD_(SHIFT)
    stmfd   sp!, {r4-r10,lr}
    subs    r2, r2, #32
_forward_blt_1_32:
    ldmgeia r0!, {r4-r10,lr}
    movge   r3, r4, BACKWARD_(SHIFT)
    orrge   r4, r12, r4, FORWARD_(SHIFT)
    movge   r12, r5, BACKWARD_(SHIFT)
    orrge   r5, r3, r5, FORWARD_(SHIFT)
    movge   r3, r6, BACKWARD_(SHIFT)
    orrge   r6, r12, r6, FORWARD_(SHIFT)
    movge   r12, r7, BACKWARD_(SHIFT)
    orrge   r7, r3, r7, FORWARD_(SHIFT)
    movge   r3, r8, BACKWARD_(SHIFT)
    orrge   r8, r12, r8, FORWARD_(SHIFT)
    movge   r12, r9, BACKWARD_(SHIFT)
    orrge   r9, r3, r9, FORWARD_(SHIFT)
    movge   r3, r10, BACKWARD_(SHIFT)
    orrge   r10, r12, r10, FORWARD_(SHIFT)
    movge   r12, lr, BACKWARD_(SHIFT)
    orrge   lr, r3, lr, FORWARD_(SHIFT)
    stmgeia r1!, {r4-r10,lr}
    subges  r2, r2, #32
    bge     _forward_blt_1_32
    add     r2, r2, #32
    ldmfd   sp!, {r4-r10,lr}
    subs    r2, r2, #4
_forward_blt_1_4:
    ldrge   r3, [r0], #4
    orrge   r12, r12, r3, FORWARD_(SHIFT)
    strge   r12, [r1], #4
    movge   r12, r3, BACKWARD_(SHIFT)
    subges  r2, r2, #4
    bge     _forward_blt_1_4
    add     r2, r2, #4
    sub     r0, r0, #SHIFT
    add     r2, r2, #SHIFT
    b       _forward_blt_end
#undef SHIFT

_forward_blt_2:
#define SHIFT  2
    ldr     r12, [r1]
    mov     r12, r12, FORWARD_(4 - SHIFT)
    mov     r12, r12, BACKWARD_(SHIFT)
    stmfd   sp!, {r4-r10,lr}
    subs    r2, r2, #32
_forward_blt_2_32:
    ldmgeia r0!, {r4-r10,lr}
    movge   r3, r4, BACKWARD_(SHIFT)
    orrge   r4, r12, r4, FORWARD_(SHIFT)
    movge   r12, r5, BACKWARD_(SHIFT)
    orrge   r5, r3, r5, FORWARD_(SHIFT)
    movge   r3, r6, BACKWARD_(SHIFT)
    orrge   r6, r12, r6, FORWARD_(SHIFT)
    movge   r12, r7, BACKWARD_(SHIFT)
    orrge   r7, r3, r7, FORWARD_(SHIFT)
    movge   r3, r8, BACKWARD_(SHIFT)
    orrge   r8, r12, r8, FORWARD_(SHIFT)
    movge   r12, r9, BACKWARD_(SHIFT)
    orrge   r9, r3, r9, FORWARD_(SHIFT)
    movge   r3, r10, BACKWARD_(SHIFT)
    orrge   r10, r12, r10, FORWARD_(SHIFT)
    movge   r12, lr, BACKWARD_(SHIFT)
    orrge   lr, r3, lr, FORWARD_(SHIFT)
    stmgeia r1!, {r4-r10,lr}
    subges  r2, r2, #32
    bge     _forward_blt_2_32
    add     r2, r2, #32
    ldmfd   sp!, {r4-r10,lr}
    subs    r2, r2, #4
_forward_blt_2_4:
    ldrge   r3, [r0], #4
    orrge   r12, r12, r3, FORWARD_(SHIFT)
    strge   r12, [r1], #4
    movge   r12, r3, BACKWARD_(SHIFT)
    subges  r2, r2, #4
    bge     _forward_blt_2_4
    add     r2, r2, #4
    sub     r0, r0, #SHIFT
    add     r2, r2, #SHIFT
    b       _forward_blt_end
#undef SHIFT

_forward_blt_3:
#define SHIFT  3
    ldr     r12, [r1]
    mov     r12, r12, FORWARD_(4 - SHIFT)
    mov     r12, r12, BACKWARD_(SHIFT)
    stmfd   sp!, {r4-r10,lr}
    subs    r2, r2, #32
_forward_blt_3_32:
    ldmgeia r0!, {r4-r10,lr}
    movge   r3, r4, BACKWARD_(SHIFT)
    orrge   r4, r12, r4, FORWARD_(SHIFT)
    movge   r12, r5, BACKWARD_(SHIFT)
    orrge   r5, r3, r5, FORWARD_(SHIFT)
    movge   r3, r6, BACKWARD_(SHIFT)
    orrge   r6, r12, r6, FORWARD_(SHIFT)
    movge   r12, r7, BACKWARD_(SHIFT)
    orrge   r7, r3, r7, FORWARD_(SHIFT)
    movge   r3, r8, BACKWARD_(SHIFT)
    orrge   r8, r12, r8, FORWARD_(SHIFT)
    movge   r12, r9, BACKWARD_(SHIFT)
    orrge   r9, r3, r9, FORWARD_(SHIFT)
    movge   r3, r10, BACKWARD_(SHIFT)
    orrge   r10, r12, r10, FORWARD_(SHIFT)
    movge   r12, lr, BACKWARD_(SHIFT)
    orrge   lr, r3, lr, FORWARD_(SHIFT)
    stmgeia r1!, {r4-r10,lr}
    subges  r2, r2, #32
    bge     _forward_blt_3_32
    add     r2, r2, #32
    ldmfd   sp!, {r4-r10,lr}
    subs    r2, r2, #4
_forward_blt_3_4:
    ldrge   r3, [r0], #4
    orrge   r12, r12, r3, FORWARD_(SHIFT)
    strge   r12, [r1], #4
    movge   r12, r3, BACKWARD_(SHIFT)
    subges  r2, r2, #4
    bge     _forward_blt_3_4
    add     r2, r2, #4
    sub     r0, r0, #SHIFT
    add     r2, r2, #SHIFT
    b       _forward_blt_end
#undef SHIFT

_forward_blt_end:
    tst     r2, #4
    ldrneb  r3, [r0], #1
    strneb  r3, [r1], #1
    ldrneb  r3, [r0], #1
    strneb  r3, [r1], #1
    ldrneb  r3, [r0], #1
    strneb  r3, [r1], #1
    ldrneb  r3, [r0], #1
    strneb  r3, [r1], #1
    tst     r2, #2
    ldrneb  r3, [r0], #1
    strneb  r3, [r1], #1
    ldrneb  r3, [r0], #1
    strneb  r3, [r1], #1
    tst     r2, #1
    ldrneb  r3, [r0], #1
    strneb  r3, [r1], #1
    bx      lr
}

asm void MI_CpuMove( register const void *srcp, register void *destp, register u32 size )
{
    cmp     r2, #0
    subnes  r3, r0, r1
    bxeq    lr
    bgt     MI_CpuCopy

_backward:
    add     r1, r1, r2
    add     r0, r0, r2

    cmp     r2, #8
    bgt	    _backward_blt
    rsb     r3, r2, #0x8
    add     pc, pc, r3, lsl #3
    nop
    ldrb    r3, [r0, #-1]!
    strb    r3, [r1, #-1]!
    ldrb    r3, [r0, #-1]!
    strb    r3, [r1, #-1]!
    ldrb    r3, [r0, #-1]!
    strb    r3, [r1, #-1]!
    ldrb    r3, [r0, #-1]!
    strb    r3, [r1, #-1]!
    ldrb    r3, [r0, #-1]!
    strb    r3, [r1, #-1]!
    ldrb    r3, [r0, #-1]!
    strb    r3, [r1, #-1]!
    ldrb    r3, [r0, #-1]!
    strb    r3, [r1, #-1]!
    ldrb    r3, [r0, #-1]!
    strb    r3, [r1, #-1]!
    bx      lr

_backward_blt:
    tst     r0, #2
    subne   r2, r2, #2
    ldrneb  r3, [r0, #-1]!
    strneb  r3, [r1, #-1]!
    ldrneb  r3, [r0, #-1]!
    strneb  r3, [r1, #-1]!
    tst     r0, #1
    subne   r2, r2, #1
    ldrneb  r3, [r0, #-1]!
    strneb  r3, [r1, #-1]!

    and     r3, r1, #3
    bic     r1, r1, #3
    cmp     r3, #0
    beq     _backward_blt_0
    cmp     r3, #1
    beq     _backward_blt_1
    cmp     r3, #2
    beq     _backward_blt_2
    b       _backward_blt_3

_backward_blt_0:
    stmfd   sp!, {r4-r10,lr}
    subs    r2, r2, #32
_backward_blt_0_32:
    ldmgedb r0!, {r4-r10,lr}
    stmgedb r1!, {r4-r10,lr}
    subges  r2, r2, #32
    bge     _backward_blt_0_32
    add     r2, r2, #32
    ldmfd   sp!, {r4-r10,lr}

    subs    r2, r2, #4
_backward_blt_0_4:
    ldrge   r3, [r0, #-4]!
    strge   r3, [r1, #-4]!
    subges  r2, r2, #4
    bge     _backward_blt_0_4
    add     r2, r2, #4
    b       _backward_blt_end

_backward_blt_1:
#define SHIFT  1
    ldr     r12, [r1]
    mov     r12, r12, BACKWARD_(4 - SHIFT)
    mov     r12, r12, FORWARD_(SHIFT)
    stmfd   sp!, {r4-r10,lr}
    subs    r2, r2, #32
_backward_blt_1_32:
    ldmgedb r0!, {r4-r10,lr}
    movge   r3, lr, FORWARD_(SHIFT)
    orrge   lr, r12, lr, BACKWARD_(SHIFT)
    movge   r12, r10, FORWARD_(SHIFT)
    orrge   r10, r3, r10, BACKWARD_(SHIFT)
    movge   r3, r9, FORWARD_(SHIFT)
    orrge   r9, r12, r9, BACKWARD_(SHIFT)
    movge   r12, r8, FORWARD_(SHIFT)
    orrge   r8, r3, r8, BACKWARD_(SHIFT)
    movge   r3, r7, FORWARD_(SHIFT)
    orrge   r7, r12, r7, BACKWARD_(SHIFT)
    movge   r12, r6, FORWARD_(SHIFT)
    orrge   r6, r3, r6, BACKWARD_(SHIFT)
    movge   r3, r5, FORWARD_(SHIFT)
    orrge   r5, r12, r5, BACKWARD_(SHIFT)
    movge   r12, r4, FORWARD_(SHIFT)
    orrge   r4, r3, r4, BACKWARD_(SHIFT)
    stmgeda r1!, {r4-r10,lr}
    subges  r2, r2, #32
    bge     _backward_blt_1_32
    add     r2, r2, #32
    ldmfd   sp!, {r4-r10,lr}
    subs    r2, r2, #4
_backward_blt_1_4:
    ldrge   r3, [r0, #-4]!
    orrge   r12, r12, r3, BACKWARD_(SHIFT)
    strge   r12, [r1], #-4
    movge   r12, r3, FORWARD_(SHIFT)
    subges  r2, r2, #4
    bge     _backward_blt_1_4
    add     r2, r2, #4
    add     r1, r1, #4
    add     r0, r0, #(4 - SHIFT)
    add     r2, r2, #(4 - SHIFT)
    b       _backward_blt_end
#undef SHIFT

_backward_blt_2:
#define SHIFT  2
    ldr     r12, [r1]
    mov     r12, r12, BACKWARD_(4 - SHIFT)
    mov     r12, r12, FORWARD_(SHIFT)
    stmfd   sp!, {r4-r10,lr}
    subs    r2, r2, #32
_backward_blt_2_32:
    ldmgedb r0!, {r4-r10,lr}
    movge   r3, lr, FORWARD_(SHIFT)
    orrge   lr, r12, lr, BACKWARD_(SHIFT)
    movge   r12, r10, FORWARD_(SHIFT)
    orrge   r10, r3, r10, BACKWARD_(SHIFT)
    movge   r3, r9, FORWARD_(SHIFT)
    orrge   r9, r12, r9, BACKWARD_(SHIFT)
    movge   r12, r8, FORWARD_(SHIFT)
    orrge   r8, r3, r8, BACKWARD_(SHIFT)
    movge   r3, r7, FORWARD_(SHIFT)
    orrge   r7, r12, r7, BACKWARD_(SHIFT)
    movge   r12, r6, FORWARD_(SHIFT)
    orrge   r6, r3, r6, BACKWARD_(SHIFT)
    movge   r3, r5, FORWARD_(SHIFT)
    orrge   r5, r12, r5, BACKWARD_(SHIFT)
    movge   r12, r4, FORWARD_(SHIFT)
    orrge   r4, r3, r4, BACKWARD_(SHIFT)
    stmgeda r1!, {r4-r10,lr}
    subges  r2, r2, #32
    bge     _backward_blt_2_32
    add     r2, r2, #32
    ldmfd   sp!, {r4-r10,lr}
    subs    r2, r2, #4
_backward_blt_2_4:
    ldrge   r3, [r0, #-4]!
    orrge   r12, r12, r3, BACKWARD_(SHIFT)
    strge   r12, [r1], #-4
    movge   r12, r3, FORWARD_(SHIFT)
    subges  r2, r2, #4
    bge     _backward_blt_2_4
    add     r2, r2, #4
    add     r1, r1, #4
    add     r0, r0, #(4 - SHIFT)
    add     r2, r2, #(4 - SHIFT)
    b       _backward_blt_end
#undef SHIFT

_backward_blt_3:
#define SHIFT  3
    ldr     r12, [r1]
    mov     r12, r12, BACKWARD_(4 - SHIFT)
    mov     r12, r12, FORWARD_(SHIFT)
    stmfd   sp!, {r4-r10,lr}
    subs    r2, r2, #32
_backward_blt_3_32:
    ldmgedb r0!, {r4-r10,lr}
    movge   r3, lr, FORWARD_(SHIFT)
    orrge   lr, r12, lr, BACKWARD_(SHIFT)
    movge   r12, r10, FORWARD_(SHIFT)
    orrge   r10, r3, r10, BACKWARD_(SHIFT)
    movge   r3, r9, FORWARD_(SHIFT)
    orrge   r9, r12, r9, BACKWARD_(SHIFT)
    movge   r12, r8, FORWARD_(SHIFT)
    orrge   r8, r3, r8, BACKWARD_(SHIFT)
    movge   r3, r7, FORWARD_(SHIFT)
    orrge   r7, r12, r7, BACKWARD_(SHIFT)
    movge   r12, r6, FORWARD_(SHIFT)
    orrge   r6, r3, r6, BACKWARD_(SHIFT)
    movge   r3, r5, FORWARD_(SHIFT)
    orrge   r5, r12, r5, BACKWARD_(SHIFT)
    movge   r12, r4, FORWARD_(SHIFT)
    orrge   r4, r3, r4, BACKWARD_(SHIFT)
    stmgeda r1!, {r4-r10,lr}
    subges  r2, r2, #32
    bge     _backward_blt_3_32
    add     r2, r2, #32
    ldmfd   sp!, {r4-r10,lr}
    subs    r2, r2, #4
_backward_blt_3_4:
    ldrge   r3, [r0, #-4]!
    orrge   r12, r12, r3, BACKWARD_(SHIFT)
    strge   r12, [r1], #-4
    movge   r12, r3, FORWARD_(SHIFT)
    subges  r2, r2, #4
    bge     _backward_blt_3_4
    add     r2, r2, #4
    add     r1, r1, #4
    add     r0, r0, #(4 - SHIFT)
    add     r2, r2, #(4 - SHIFT)
    b       _backward_blt_end
#undef SHIFT

_backward_blt_end:
    tst     r2, #4
    ldrneb  r3, [r0, #-1]!
    strneb  r3, [r1, #-1]!
    ldrneb  r3, [r0, #-1]!
    strneb  r3, [r1, #-1]!
    ldrneb  r3, [r0, #-1]!
    strneb  r3, [r1, #-1]!
    ldrneb  r3, [r0, #-1]!
    strneb  r3, [r1, #-1]!
    tst     r2, #2
    ldrneb  r3, [r0, #-1]!
    strneb  r3, [r1, #-1]!
    ldrneb  r3, [r0, #-1]!
    strneb  r3, [r1, #-1]!
    tst     r2, #1
    ldrneb  r3, [r0, #-1]!
    strneb  r3, [r1, #-1]!
    bx      lr
}

#undef FORWARD_
#undef BACKWARD_
#undef FORWARD_MASK
#endif

#include <nitro/codereset.h>
#include <nitro/code16.h>

asm void MI_Zero32B (register void * pDest)
{
    mov r1, #0
    mov r2, #0
    stmia r0 !, {r1, r2}
    mov r3, #0
    stmia r0 !, {r1, r2, r3}
    stmia r0 !, {r1, r2, r3}
    bx lr
}

asm void MI_Zero36B (register void * pDest)
{
    mov r1, #0
    mov r2, #0
    mov r3, #0
    stmia r0 !, {r1, r2, r3}
    stmia r0 !, {r1, r2, r3}
    stmia r0 !, {r1, r2, r3}
    bx lr
}

asm void MI_Zero48B (register void * pDest)
{
    mov r1, #0
    mov r2, #0
    mov r3, #0
    stmia r0 !, {r1, r2, r3}
    stmia r0 !, {r1, r2, r3}
    stmia r0 !, {r1, r2, r3}
    stmia r0 !, {r1, r2, r3}
    bx lr
}

asm void MI_Zero64B (register void * pDest)
{
    mov r1, #0
    mov r2, #0
    stmia r0 !, {r1, r2}
    mov r3, #0
    stmia r0 !, {r1, r2}
    stmia r0 !, {r1, r2, r3}
    stmia r0 !, {r1, r2, r3}
    stmia r0 !, {r1, r2, r3}
    stmia r0 !, {r1, r2, r3}
    bx lr
}

#include <nitro/codereset.h>
#endif
