#ifndef TWL_TCL_TYPES_H_
#define TWL_TCL_TYPES_H_

#include <twl/misc.h>
#include <twl/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TCL_ARCHIVE_NAND "photo:"

#define TCL_DIR_SUFFIX "NIN02"
#define TCL_FILE_PREFIX "HNI_"

#define TCL_ROOT_DIR "DCIM"
#define TCL_PRIVATE_DIR "private/ds/app/484E494A"
#define TCL_FRAME_ROOT_DIR TCL_PRIVATE_DIR "/" TCL_ROOT_DIR

enum {
  TCL_MAX_NUM_NAND_PICTURES = 500,

  TCL_MAX_NUM_DIRS = 900,
  TCL_MAX_NUM_FILES_IN_A_DIR = 100,

  TCL_AVARAGE_SIZE = 80 * 1024, // Average photo size
  TCL_MARGIN_SIZE =
      350 * 1024, // Margin size for calculating the number of saved photos
  TCL_MAX_JPEG_SIZE = 200 * 1024, // Maximum JPEG size sought by the TCL library

  TCL_DIR_OFFSET = 100,
  TCL_FILE_OFFSET = 1,

  TCL_JPEG_WIDTH = 640,  // Image width sought by the TCL library
  TCL_JPEG_HEIGHT = 480, // Image height sought by the TCL library

  TCL_JPEG_DEFAULT_QUALITY = 80, // Recommended quality value for SSP encoding

  TCL_JPEG_QUALITY_STEP = 10,

  TCL_PATH_LEN = 64, // Path length

  TCL_CONST_END
};

typedef enum {
  TCL_TARGET_ARCHIVE_NAND = 0,
  TCL_TARGET_ARCHIVE_SD, // Currently, SD Card cannot be used
  TCL_TARGET_ARCHIVE_MAX
} TCLTargetArchive;

typedef enum {
  TCL_FAVORITE_TYPE_NONE = 0,
  TCL_FAVORITE_TYPE_1,
  TCL_FAVORITE_TYPE_2,
  TCL_FAVORITE_TYPE_3,
  TCL_FAVORITE_TYPE_ALL,
  TCL_FAVORITE_TYPE_MAX = TCL_FAVORITE_TYPE_ALL
} TCLFavoriteType;

typedef enum {
  TCL_IMAGE_TYPE_PICTURE = 0,
  TCL_IMAGE_TYPE_FRAME,
  TCL_IMAGE_TYPE_COMPOSITE,
  TCL_IMAGE_TYPE_UNKNOWN,
  TCL_IMAGE_TYPE_MAX
} TCLImageType;

typedef enum {
  TCL_KEY_TYPE_LAUNCHER = 0,
  TCL_KEY_TYPE_APPLICATION,
  TCL_KEY_TYPE_UNKNOWN,
  TCL_KEY_TYPE_MAX
} TCLKeyType;

typedef enum {
  TCL_SEARCH_CONDITION_NONE = 0,
  TCL_SEARCH_CONDITION_PICTURE = 1 << 0x00,
  TCL_SEARCH_CONDITION_FRAME = 1 << 0x01,
  TCL_SEARCH_CONDITION_FAVORITE_NONE = 1 << 0x07,
  TCL_SEARCH_CONDITION_FAVORITE_1 = 1 << 0x08,
  TCL_SEARCH_CONDITION_FAVORITE_2 = 1 << 0x09,
  TCL_SEARCH_CONDITION_FAVORITE_3 = 1 << 0x0a,
  TCL_SEARCH_CONDITION_FAVORITE_ALL = 1 << 0x0b,
  TCL_SEARCH_CONDITION_LAUNCHER = 1 << 0x0c,
  TCL_SEARCH_CONDITION_APPLICATION = 1 << 0x0d,
  TCL_SEARCH_CONDITION_FREE =
      1 << 0x10, // The returning of a blank space is a somewhat unique
                 // condition and cannot therefore be combined
  TCL_SEARCH_CONDITION_MAX
} TCLSearchCondition;

typedef enum {
  TCL_SORT_TYPE_DATE = 0,
  TCL_SORT_TYPE_FAVORITE_1,
  TCL_SORT_TYPE_FAVORITE_2,
  TCL_SORT_TYPE_FAVORITE_3,
  TCL_SORT_TYPE_FAVORITE_ALL,
  TCL_SORT_TYPE_FRAME,
  TCL_SORT_TYPE_MAX
} TCLSortType;

typedef enum {
  TCL_RESULT_SUCCESS = 0,
  TCL_RESULT_ERROR_FS_OPEN_FILE_EX,
  TCL_RESULT_ERROR_FS_CLOSE_FILE,
  TCL_RESULT_ERROR_FS_CREATE_FILE_AUTO,
  TCL_RESULT_ERROR_FS_GET_FILE_LENGTH,
  TCL_RESULT_ERROR_FS_READ_FILE,
  TCL_RESULT_ERROR_FS_WRITE_FILE,
  TCL_RESULT_ERROR_FS_DELETE_FILE,
  TCL_RESULT_ERROR_FS_OPEN_DIRECTORY,
  TCL_RESULT_ERROR_FS_CLOSE_DIRECTORY,
  TCL_RESULT_ERROR_FS_GET_ARCHIVE_RESOURCE,

  TCL_RESULT_ERROR_OVER_NUM_PICTURES, // The management table's save capacity
                                      // has been exceeded
  TCL_RESULT_ERROR_NO_TABLE_FILE,     // The table was not found
  TCL_RESULT_ERROR_BROKEN_TABLE_FILE, // Management file is corrupted
  TCL_RESULT_ERROR_NO_NEXT_INDEX, // The next save location for a photo was not
                                  // found
  TCL_RESULT_ERROR_EXIST_OTHER_FILE, // Another file was in the next save
                                     // location
  TCL_RESULT_ERROR_ALREADY_MANAGED,  // The specified index is already being
                                     // managed
  TCL_RESULT_ERROR_NO_FIND_PICTURE, // No photo was found in the specified index
                                    // with a path search

  TCL_RESULT_ERROR_SSP_START_JPEG_DECODER,        // Decoding error
  TCL_RESULT_ERROR_SSP_EXTRACT_JPEG_DECODER_EXIF, // EXIF loading error

  TCL_RESULT_ERROR_OTHER, // Other error
  TCL_RESULT_MAX
} TCLResult;

typedef struct {
  u16 dirIdx;
  u16 fileIdx;
} TCLIndexInfo;

typedef struct {
  void *tableBuffer;
  u32 tableBufferSize;
  void *workBuffer;
  u32 workBufferSize;
} TCLAccessor;

typedef struct {
  u32 signature;
  u32 version;
  u16 numElements;
  u8 targetArchive;
  u8 reserved0;
  TCLIndexInfo picIdxInfo;
  TCLIndexInfo frmIdxInfo;
  u16 crc;
  u16 offset;
} TCLTableHeader;

typedef struct {
  s64 second;
  int favoriteOrder;

  union {
    u32 bitField;
    struct {
      u32 validity : 1;
      u32 dirIdx : 10;
      u32 fileIdx : 7;
      u32 favoriteType : 2;
      u32 imageType : 2;
      u32 keyType : 2;
      u32 defaultFrame : 1;
      u32 reserved : 7;
    };
  };
} TCLPictureInfo;

typedef struct {
  union {
    u32 bitField;
    struct {
      u32 imageType : 2;
      u32 reserved0 : 30;
    };
  };
  u32 reserved1;
} TCLMakerNote;

typedef struct {
  int startIdx;
  u32 condition;
} TCLSearchObject;

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
