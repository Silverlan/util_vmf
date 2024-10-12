include(${CMAKE_SOURCE_DIR}/cmake/pr_common.cmake)

set(PROJ_NAME util_vmf)
pr_add_library(${PROJ_NAME} STATIC)

pr_add_dependency(${PROJ_NAME} materialsystem TARGET PUBLIC)

pr_init_module(${PROJ_NAME})

pr_finalize(${PROJ_NAME})
