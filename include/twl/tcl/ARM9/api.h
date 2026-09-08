#ifndef TWL_TCL_API_H_
#define TWL_TCL_API_H_

#include <twl/misc.h>
#include <twl/types.h>
#include <nitro/fs.h>
#include <twl/tcl/ARM9/types.h>

#ifdef __cplusplus
extern "C" {
#endif

extern u32 TCL_GetTableBufferSize(void);

extern u32 TCL_GetWorkBufferSize(void);

extern TCLResult TCL_LoadTable(TCLAccessor *pAccessor, void *tableBuffer,
                               u32 tableBufferSize, void *workBuffer,
                               u32 workBufferSize, FSResult *pFSResult);

extern TCLResult TCL_CreateTable(TCLAccessor *pAccessor, void *tableBuffer,
                                 u32 tableBufferSize, void *workBuffer,
                                 u32 workBufferSize, FSResult *pFSResult);

extern TCLResult TCL_RepairTable(TCLAccessor *pAccessor, FSResult *pFSResult);

extern void TCL_SortTable(TCLAccessor *pAccessor, TCLSortType sortType);

extern void TCL_InitSearchObject(TCLSearchObject *pSearchObj, u32 condition);

extern int TCL_GetNumPictures(const TCLAccessor *pAccessor,
                              const TCLSearchObject *pSearchObj);

extern TCLResult TCL_SearchNextPictureInfo(const TCLAccessor *pAccessor,
                                           const TCLPictureInfo **ppPicInfo,
                                           TCLSearchObject *pSearchObj);

extern TCLResult TCL_SearchPictureInfoByIndex(const TCLAccessor *pAccessor,
                                              const TCLPictureInfo **ppPicInfo,
                                              const TCLSearchObject *pSearchObj,
                                              int index);

extern TCLResult TCL_SearchNextPicturePath(const TCLAccessor *pAccessor,
                                           char *path, size_t len,
                                           TCLSearchObject *pSearchObj);

extern TCLResult TCL_SearchPicturePathByIndex(const TCLAccessor *pAccessor,
                                              char *path, size_t len,
                                              const TCLSearchObject *pSearchObj,
                                              int index);

extern int TCL_PrintPicturePath(char *path, size_t len,
                                const TCLPictureInfo *pPicInfo);

extern BOOL TCL_GetPictureInfoFromPath(const TCLAccessor *pAccessor,
                                       TCLPictureInfo **ppPicInfo,
                                       const char *path, size_t len);

extern int TCL_CalcNumEnableToTakePictures(const TCLAccessor *pAccessor);

extern u32 TCL_GetJpegEncoderBufferSize(u32 option);

extern TCLResult TCL_EncodeAndWritePicture(TCLAccessor *pAccessor,
                                           const void *imageBuffer,
                                           u8 *jpegBuffer, u32 jpegBufferSize,
                                           u8 *workBuffer, u32 quality,
                                           u32 option, FSResult *pFSResult);

extern TCLResult
TCL_EncodeAndWritePictureEx(TCLAccessor *pAccessor, const void *imageBuffer,
                            u8 *jpegBuffer, u32 jpegBufferSize, u8 *workBuffer,
                            u32 quality, u32 option, u8 *makerNoteBuffer,
                            u16 makerNoteBufferSize, FSResult *pFSResult);

extern const char *TCL_GetLastWrittenPicturePath(void);

extern const TCLPictureInfo *TCL_GetLastWrittenPictureInfo(void);

extern TCLResult TCL_DecodePicture(u8 *fileBuffer, u32 fileBufferSize,
                                   void *imageBuffer, s16 width, s16 height,
                                   u32 decodeOption);

extern TCLResult TCL_DeletePicture(TCLAccessor *pAccessor,
                                   const TCLPictureInfo *pPicInfo,
                                   FSResult *pFSResult);

extern BOOL TCL_IsSameImageType(const TCLPictureInfo *pPicInfo,
                                const TCLMakerNote *pMakerNote);

extern s64 TCL_GetCurrentSecond(void);

extern TCLResult TCL_ChangePictureFavoriteType(TCLAccessor *pAccessor,
                                               TCLPictureInfo *pPicInfo,
                                               TCLFavoriteType favoriteType,
                                               FSResult *pFSResult);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
