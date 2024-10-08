/*****************************************************************************
 *   Copyright(C)2009-2022 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
 *  You may obtain a copy of the License at                                  *
 *                                                                           *
 *     http://www.apache.org/licenses/LICENSE-2.0                            *
 *                                                                           *
 *  Unless required by applicable law or agreed to in writing, software      *
 *  distributed under the License is distributed on an "AS IS" BASIS,        *
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 *  See the License for the specific language governing permissions and      *
 *  limitations under the License.                                           *
 *                                                                           *
 ****************************************************************************/

#ifndef __VSF_FATFS_H__
#define __VSF_FATFS_H__

/*============================ INCLUDES ======================================*/

#include "../../vsf_fs_cfg.h"

#if VSF_USE_FS == ENABLED && (VSF_FS_USE_FATFS == ENABLED || VSF_FS_USE_EXFATFS == ENABLED)

#include "../malfs/vsf_malfs.h"

#if     defined(__VSF_FATFS_CLASS_IMPLEMENT)
#   undef __VSF_FATFS_CLASS_IMPLEMENT
#   define __VSF_CLASS_IMPLEMENT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#define implement_fatfs_info(__block_size, __cache_num)                         \
    implement(__vk_fatfs_info_t)                                                \
    __implement_malfs_cache(__block_size, __cache_num)

#define init_fatfs_info_ex(__info, __block_size, __cache_num, __fatinfo)        \
    do {                                                                        \
        __fatinfo->block_size = __block_size;                                   \
        __fatinfo->cache.number = __cache_num;                                  \
        __fatinfo->cache.nodes = __info->__cache_nodes;                         \
    } while (0)

#define init_fatfs_info(__info, __block_size, __cache_num)                      \
    .block_size = __block_size,                                                 \
    .cache      = {                                                             \
        .number = __cache_num,                                                  \
        .nodes  = __info.__cache_nodes,                                         \
    },

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

vsf_dcl_class(vk_fatfs_file_t)
vsf_dcl_class(__vk_fatfs_info_t)

#if VSF_FS_USE_EXFATFS == ENABLED
typedef uint64_t vk_fat_sector_type_t;
#else
typedef uint32_t vk_fat_sector_type_t;
#endif

typedef enum vk_fat_type_t {
    VSF_FAT_NONE,
    VSF_FAT_12,
    VSF_FAT_16,
    VSF_FAT_32,
    VSF_FAT_EX,
} vk_fat_type_t;

typedef enum vk_fat_file_attr_t {
    VSF_FAT_FILE_ATTR_VOLUMID = VSF_FILE_ATTR_EXT,
    VSF_FAT_FILE_ATTR_SYSTEM  = VSF_FILE_ATTR_EXT << 1,
    VSF_FAT_FILE_ATTR_ARCHIVE = VSF_FILE_ATTR_EXT << 2,
} vk_fat_file_attr_t;

typedef struct vk_fatfs_dentry_parser_t {
    uint8_t *entry;
    char *filename;
    int16_t entry_num;
    uint16_t node_parsed_num;
    int16_t vital_entry_num;

    // private
    bool is_unicode;
    uint8_t attr;
    uint32_t first_cluster;
    uint64_t size;

    union {
        struct {
            uint8_t lfn;
        } fat;
#if VSF_FS_USE_EXFATFS == ENABLED
        struct {
            uint8_t namelen;
            uint8_t namepos;
            uint8_t entry_num;
            int16_t root_entry_num;
        } exfat;
#endif
        uint32_t zero_before_first_call;
    };
} vk_fatfs_dentry_parser_t;

typedef struct vk_fatfs_file_pos_t {
    uint32_t cluster;
    uint32_t sector_offset_in_cluster;
    uint32_t offset_in_sector;
} vk_fatfs_file_pos_t;

vsf_class(vk_fatfs_file_t) {
    public_member(
        implement(__vk_malfs_file_t)
    )

    private_member(
        uint32_t first_cluster;
        vk_fatfs_file_pos_t cur;
        struct {
            vk_fat_sector_type_t sector0;
            vk_fat_sector_type_t sector1;
            uint8_t entry_num;
            uint8_t entry_offset_in_sector0;

            // vital sector/entry_offset is the entry containing first_cluster/size
            //  for fat, should be the last entry
            //  for exfat, should be the stream entry
            uint8_t vital_sector;
            uint8_t vital_entry_offset;
#if VSF_FS_USE_EXFATFS == ENABLED
            // root 0/entry_offset is the entry for Critical Primary Directory Entries of exfat
            uint8_t root_entry_offset;
#endif
        } dentry;
    )
};

// memory layout:
//  |-----------------------|
//  | __vk_fatfs_info_t:    |<-|
//  | |-------------------| |  |
//  | |      ...          | |  |
//  | |-------------------| |  |
//  | ||__vk_malfs_info_t|| |  |
//  | ||-----------------|| |  |
//  | ||    total_cb     ||-|--- if __vk_fatfs_info_t is dynamically allocated,
//  | ||-----------------|| | total_cb in __vk_malfs_info_t points to the __vk_fatfs_info_t.
//  |-----------------------| so in __vk_malfs_unmount, can free total_cb
//  | vk_malfs_cache_t[num] |
//  |-----------------------|
//  |   cache_buffer[num]   |
//  |-----------------------|
vsf_class(__vk_fatfs_info_t) {
    private_member(
        vk_fatfs_file_t root;
        vk_fat_type_t type;
#if VSF_FS_USE_EXFATFS == ENABLED
        char fat_volume_name[24];
#else
        char fat_volume_name[12];
#endif

        // fat parameters, will not change after mount
#if VSF_FS_USE_EXFATFS == ENABLED
        uint32_t bitmap_sector;
        uint32_t bitmap_size;
#endif
        uint32_t root_sector;
        uint32_t root_size;
        uint32_t fat_sector;
        uint32_t fat_size;
        uint32_t data_sector;
        uint32_t cluster_num;
        uint8_t sector_size_bits;
        uint8_t cluster_size_bits;
        uint8_t fat_num;
    )

    // vk_malfs_info_t must be the last in vk_fatfs_info_t
    public_member(
        implement(__vk_malfs_info_t)
    )
};

/*============================ GLOBAL VARIABLES ==============================*/

extern const vk_fs_op_t vk_fatfs_op;

/*============================ PROTOTYPES ====================================*/

extern bool vk_fatfs_is_lfn(char *name);
extern bool vk_fatfs_parse_dentry_fat(vk_fatfs_dentry_parser_t *parser);

#ifdef __cplusplus
}
#endif

#endif      // VSF_USE_FS && VSF_FS_USE_FATFS
#endif      // __VSF_FATFS_H__
