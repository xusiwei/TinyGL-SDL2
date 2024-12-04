find_package(SDL2 REQUIRED)

set(UI_SOURCES
    ui.h
    ui_sdl.c
)

set(GLU_SOURCES
    glu.c
    glu.h
)

# TinyGL_UI 目标，方便其他可执行程序使用
add_library(TinyGL_UI OBJECT
    ${UI_SOURCES}
    ${GLU_SOURCES}
)

target_include_directories(TinyGL_UI
    PUBLIC ${CMAKE_CURRENT_LIST_DIR}
    PUBLIC ../src
)

target_link_libraries(TinyGL_UI
    PUBLIC ${PROJECT_NAME}
    PUBLIC SDL2::SDL2
)

# gears
add_executable(gears
    gears.c
)

target_link_libraries(gears
    PRIVATE TinyGL_UI
)

# 拷贝 SDL2.dll 到可执行文件目录
add_custom_command(TARGET gears POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:SDL2::SDL2> $<TARGET_FILE_DIR:gears>
)

# spin
add_executable(spin
    spin.c
)

target_link_libraries(spin
    PRIVATE TinyGL_UI
)

# mech
add_executable(mech
    mech.c
)

target_link_libraries(mech
    PRIVATE TinyGL_UI
)

# texobj
add_executable(texobj
    texobj.c
)

target_link_libraries(texobj
    PRIVATE TinyGL_UI
)

# triangle
add_executable(triangle
    triangle.c
)

target_link_libraries(triangle
    PRIVATE TinyGL_UI
)
