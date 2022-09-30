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

//#include "shell/sys/linux/vsf_linux_cfg.h"
//
//#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
//#   include "shell/sys/linux/include/unistd.h"
//#   include "shell/sys/linux/include/libusb.h"
//#else
//#   include <unistd.h>
//#   include <libusb.h>
//#endif
//
//#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED && VSF_LINUX_USE_SIMPLE_STDIO == ENABLED
//#   include "shell/sys/linux/include/simple_libc/stdio.h"
//#else
//#   include <stdio.h>
//#endif
//
//#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED && VSF_LINUX_USE_SIMPLE_STRING == ENABLED
//#   include "shell/sys/linux/include/simple_libc/string.h"
//#else
//#   include <string.h>
//#endif


#include "vsf.h"

#if 1 //APP_USE_HAL_DEMO == ENABLED && APP_USE_HAL_FLASH_DEMO == ENABLED && VSF_HAL_USE_FLASH == ENABLED



/*============================ MACROS ========================================*/
/*============================ IMPLEMENTATION ================================*/

#define FLASH_MAL_TEST      0
#define MEM_MAL_TEST        1

#if VSF_MAL_USE_HW_FLASH_MAL == ENABLED
#   define MAL_TEST     FLASH_MAL_TEST
#else
#   define MAL_TEST     MEM_MAL_TEST
#endif

#define __MAL_DEMO_BLOCK_SIZE   (4096)

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
        return VSF_ERR_FAIL;
    }

    off_t real_offset = fseek(fd, offset, SEEK_SET);
    if (real_offset != offset) {
        return VSF_ERR_FAIL;
    }

    printf("mal_write_file, size: %d\n", size);


    if (fwrite(buf, size, 1, fd) != size) {
        ret = -1;
    }

    fclose(fd);

    return ret;
}

vsf_err_t mal_read_file(char *file_name, off_t offset, uint8_t * buf, size_t buffer_size)
{
    vsf_err_t ret = VSF_ERR_NONE;

    FILE *fd = fopen(file_name, "r+");
    if (NULL == fd) {
        return VSF_ERR_FAIL;
    }

    off_t real_offset = fseek(fd, offset, SEEK_SET);
    if (real_offset != offset) {
        return VSF_ERR_FAIL;
    }

    size_t size = fread(buf, 1, buffer_size, fd);
    fclose(fd);

    return ret;
}


vsf_err_t __mal_write_then_read_test(char *path, uint8_t *write_buffer, uint8_t *read_buffer, size_t size)
{
    vsf_err_t ret = VSF_ERR_NONE;

    printf("write then read test, size: %d\n", size);

    for (int i = 0; i < __MAL_DEMO_BLOCK_SIZE; i++) {
        write_buffer[i] = i;
    }

    ret = mal_write_file(path, 0, write_buffer, size);
    if (ret != VSF_ERR_NONE) {
        printf("mal write faild: %d\n" , ret);
        return VSF_ERR_FAIL;
    }

    ret = mal_read_file(path, 0, read_buffer, size);
    if (ret != VSF_ERR_NONE) {
        printf("mal read faild: %d\n", ret);
        return VSF_ERR_FAIL;
    }

    for (int i = 0; i < size; i++) {
        if (write_buffer[i] != read_buffer[i]) {
            printf("mal compare: %d\n", ret);
            return VSF_ERR_FAIL;
        }
    }

    printf("mal read/write pass\n");

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
    __mal_write_then_read_test(bind_path, __flash_mal_demo.test_buffer.write, __flash_mal_demo.test_buffer.read, sizeof(__flash_mal_demo.test_buffer.read));
#else
    vk_mal_init(&__flash_mal_demo.flash_mal.use_as__vk_mal_t);
    vsf_linux_fs_bind_mal(bind_path, &__flash_mal_demo.flash_mal.use_as__vk_mal_t);
    __mal_write_then_read_test(bind_path, __flash_mal_demo.test_buffer.write, __flash_mal_demo.test_buffer.read, sizeof(__flash_mal_demo.test_buffer.read));
#endif

    return 0;
}

#endif
