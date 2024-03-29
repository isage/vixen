cmake_minimum_required(VERSION 3.2)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  if(DEFINED ENV{VITASDK})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake" CACHE PATH "toolchain file")
  else()
    message(FATAL_ERROR "Please define VITASDK to point to your SDK path!")
  endif()
endif()

project(vixen)
include("${VITASDK}/share/vita.cmake" REQUIRED)

add_executable(${PROJECT_NAME}
  src/main.c
  src/devicelist.c
  src/controller.c
  src/controllers/xbox_controller.c
  src/controllers/xbox_360_controller.c
  src/controllers/xbox_360w_controller.c
  src/controllers/ds3_controller.c
  src/controllers/dinput_controller.c
)

target_link_libraries(${PROJECT_NAME}
  SceDebugForDriver_stub
  SceSblAIMgrForDriver_stub
  SceCtrlForDriver_stub
  SceKernelSuspendForDriver_stub
  SceSysclibForDriver_stub
  SceSysmemForDriver_stub
  SceThreadmgrForDriver_stub
  SceUsbdForDriver_stub
  SceUsbServForDriver_stub
  taihenForKernel_stub
)

set_target_properties(${PROJECT_NAME}
  PROPERTIES LINK_FLAGS "-nostdlib"
)

vita_create_self(${PROJECT_NAME}.skprx ${PROJECT_NAME}
  CONFIG vixen.yml
  UNSAFE
)

add_executable(${PROJECT_NAME}_ds3
  src/main.c
  src/devicelist.c
  src/controller.c
  src/controllers/xbox_controller.c
  src/controllers/xbox_360_controller.c
  src/controllers/xbox_360w_controller.c
  src/controllers/ds3_controller.c
  src/controllers/dinput_controller.c
)

target_link_libraries(${PROJECT_NAME}_ds3
  SceDebugForDriver_stub
  SceSblAIMgrForDriver_stub
  SceCtrlForDriver_stub
  SceKernelSuspendForDriver_stub
  SceSysclibForDriver_stub
  SceSysmemForDriver_stub
  SceThreadmgrForDriver_stub
  SceUsbdForDriver_stub
  SceUsbServForDriver_stub
  taihenForKernel_stub
)

set_target_properties(${PROJECT_NAME}_ds3
  PROPERTIES LINK_FLAGS "-nostdlib"
)

target_compile_definitions(${PROJECT_NAME}_ds3
  PRIVATE WITH_DS3
)

vita_create_self(${PROJECT_NAME}_ds3.skprx ${PROJECT_NAME}_ds3
  CONFIG vixen.yml
  UNSAFE
)
