# CMakeLists head

vsf_add_sources(
    vsf_simple_stream.c
)

add_subdirectory(block_stream)
add_subdirectory(fifo_stream)
add_subdirectory(mem_stream)
add_subdirectory(distbus_stream)
add_subdirectory(queue_stream)
