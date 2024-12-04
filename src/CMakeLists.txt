add_library(${PROJECT_NAME}
    api.c
    list.c
    vertex.c
    init.c
    matrix.c
    texture.c
    misc.c
    clear.c
    light.c
    clip.c
    select.c
    get.c
    error.c
    zbuffer.c
    zline.c
    zdither.c
    ztriangle.c
    zmath.c
    image_util.c
    oscontext.c
    msghandling.c
    arrays.c
    specbuf.c
    memory.c
)

target_include_directories(${PROJECT_NAME}
    PUBLIC ../include
)

