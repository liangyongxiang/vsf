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

/*============================ INCLUDES ======================================*/

#include "vsf.h"

#if 1 //APP_USE_HAL_DEMO == ENABLED && APP_USE_HAL_FLASH_DEMO == ENABLED && VSF_HAL_USE_FLASH == ENABLED


/*============================ MACROS ========================================*/

#ifndef APP_FLASH_MAL_DEMO_CFG_OFFSET
#   define APP_FLASH_MAL_DEMO_CFG_OFFSET    0x001FB000
#endif

#define FLASH_MAL_TEST      0
#define MEM_MAL_TEST        1

#if VSF_MAL_USE_HW_FLASH_MAL == ENABLED
#   define MAL_TEST     FLASH_MAL_TEST
#else
#   define MAL_TEST     MEM_MAL_TEST
#undef APP_FLASH_MAL_DEMO_CFG_OFFSET
#   define APP_FLASH_MAL_DEMO_CFG_OFFSET 4096
#endif

#define __MAL_DEMO_BLOCK_SIZE   (4096)

/*============================ IMPLEMENTATION ================================*/



struct flash_mal_demo_t {
#if MAL_TEST == MEM_MAL_TEST
    vk_mem_mal_t mem_mal;
    uint8_t buffer[__MAL_DEMO_BLOCK_SIZE * __MAL_DEMO_BLOCK_SIZE];
#else
    vk_hw_flash_mal_t flash_mal;
#endif

    struct {
        uint8_t read[__MAL_DEMO_BLOCK_SIZE];
        uint8_t write[__MAL_DEMO_BLOCK_SIZE];
    } test_buffer;

} __flash_mal_demo = {
#if MAL_TEST == MEM_MAL_TEST
    .mem_mal = {
        .mem = {
            .buffer = __flash_mal_demo.buffer,
            .size = sizeof(__flash_mal_demo.buffer),

        },
        .blksz = __MAL_DEMO_BLOCK_SIZE,
        .use_as__vk_mal_t.drv = &vk_mem_mal_drv,
    },
#else
    .flash_mal = {
        .use_as__vk_mal_t.drv = &vk_hw_flash_mal_drv,
        .flash = &vsf_hw_flash0,
    }
#endif
};

vsf_err_t mal_write_file(char * file_name, off_t offset, uint8_t * buf, size_t size)
{
    vsf_err_t ret = VSF_ERR_NONE;

    FILE *fd;
    fd = fopen(file_name, "r+");
    if (NULL == fd) {
        printf("mal_write_file open %s faild\r\n", file_name);
        return VSF_ERR_FAIL;
    }

    int fret = fseek(fd, offset, SEEK_SET);
    if (fret != 0) {
        printf("mal_write_file seek %d faild\r\n", offset);
        ret = VSF_ERR_FAIL;
    } else {
        size_t wsize = fwrite(buf, 1, size, fd);
        if (wsize != size) {
            printf("mal_write_file write faild, wsize: %d\r\n", wsize);
            ret = VSF_ERR_FAIL;
        }
    }

    fclose(fd);

    return ret;
}

vsf_err_t mal_read_file(char *file_name, off_t offset, uint8_t * buf, size_t size)
{
    vsf_err_t ret = VSF_ERR_NONE;
    size_t rsize;

    FILE *fd = fopen(file_name, "r+");
    if (NULL == fd) {
        printf("mal_read_file open %s faild\r\n", file_name);
        return VSF_ERR_FAIL;
    }

    int fret = fseek(fd, offset, SEEK_SET);
    if (fret != 0) {
        printf("mal_read_file seek %d faild\r\n", offset);
        ret = VSF_ERR_FAIL;
    } else {
        rsize = fread(buf, 1, size, fd);
        if (rsize != size) {
            printf("mal_read_file write faild, rsize: %d\r\n", rsize);
            ret = VSF_ERR_FAIL;
        }
    }

    fclose(fd);

    return ret;
}

vsf_err_t __mal_write_then_read_test(char *path, size_t offset, uint8_t *write_buffer, uint8_t *read_buffer, size_t size)
{
    vsf_err_t ret = VSF_ERR_NONE;

    printf("write then read test, size: %d\r\n", size);

    for (int i = 0; i < __MAL_DEMO_BLOCK_SIZE; i++) {
        write_buffer[i] = i;
    }

    ret = mal_write_file(path, offset, write_buffer, size);
    if (ret != VSF_ERR_NONE) {
        printf("mal write faild: %d\r\n" , ret);
        return VSF_ERR_FAIL;
    }

    ret = mal_read_file(path, offset, read_buffer, size);
    if (ret != VSF_ERR_NONE) {
        printf("mal read faild: %d\r\n", ret);
        return VSF_ERR_FAIL;
    }

    for (int i = 0; i < size; i++) {
        if (write_buffer[i] != read_buffer[i]) {
            printf("mal compare faild: %d\r\n", ret);
            return VSF_ERR_FAIL;
        }
    }

    printf("mal read/write pass\r\n");

    return VSF_ERR_NONE;
}

#if APP_USE_LINUX_DEMO == ENABLED
int mal_main(int argc, char *argv[])
{
#else
int VSF_USER_ENTRY(void)
{
#   if VSF_USE_TRACE == ENABLED
    vsf_start_trace();
#   endif
#   if USRAPP_CFG_STDIO_EN == ENABLED
    vsf_stdio_init();
#   endif
#endif

    char *bind_path = "/dev/sda";

#if MAL_TEST == MEM_MAL_TEST
    vk_mal_init(&__flash_mal_demo.mem_mal.use_as__vk_mal_t);
    vsf_linux_fs_bind_mal(bind_path, &__flash_mal_demo.mem_mal.use_as__vk_mal_t);
    __mal_write_then_read_test(bind_path,
                               APP_FLASH_MAL_DEMO_CFG_OFFSET,
                               __flash_mal_demo.test_buffer.write,
                               __flash_mal_demo.test_buffer.read,
                               sizeof(__flash_mal_demo.test_buffer.read));
#else
    static bool is_inited = false;
    if (!is_inited) {
        is_inited = 1;
        vk_mal_init(&__flash_mal_demo.flash_mal.use_as__vk_mal_t);
        vsf_linux_fs_bind_mal(bind_path, &__flash_mal_demo.flash_mal.use_as__vk_mal_t);
    }

    __mal_write_then_read_test(bind_path,
                               APP_FLASH_MAL_DEMO_CFG_OFFSET,
                               __flash_mal_demo.test_buffer.write,
                               __flash_mal_demo.test_buffer.read,
                               sizeof(__flash_mal_demo.test_buffer.read));
#endif

    return 0;
}

#endif
