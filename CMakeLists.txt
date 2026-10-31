cmake_minimum_required(VERSION 3.5)

set(APPLE FALSE)

# Disable compiler checking
set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_CXX_COMPILER_WORKS 1)

project(stm32_template)

set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS)
set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS)

set(TARGET_NAME ${PROJECT_NAME})

# Принудительно задаем ARM-компилятор
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(OBJCOPY arm-none-eabi-objcopy)
set(STFLASH st-flash)

SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/build)

set(CMAKE_EXECUTABLE_SUFFIX ".elf")

set(CMAKE_C_FLAGS "-mcpu=cortex-m3 -mthumb -std=c99")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fdata-sections -ffunction-sections -mlittle-endian -g3")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-move-loop-invariants -fsigned-char -ffreestanding")

set(LINKER_FLAGS "-nostdlib -nostartfiles")
set(LINKER_FLAGS "${LINKER_FLAGS} -Xlinker -Map -Xlinker ${TARGET_NAME}.map ")
set(LINKER_FLAGS "${LINKER_FLAGS} -Xlinker -T ${CMAKE_CURRENT_SOURCE_DIR}/stm32f103c8.ld")
set(LINKER_FLAGS "${LINKER_FLAGS} -Wl,--gc-sections ")
set(CMAKE_C_LINK_FLAGS ${LINKER_FLAGS})

message("DIRECTORY ${EXECUTABLE_OUTPUT_PATH}")

add_definitions(-DSTM32F103C8)
add_definitions(-DSTM32F10X_MD)

include_directories(cmsis)
include_directories(stm32)

set(SOURCE_FILES
    stm32/startup/startup_stm32f10x_md.c
    stm32/system_stm32f10x.c
    syscalls/syscalls.c
    main.c)

# Копирование .gdbinit (если его нет, CMake не упадет)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/.gdbinit")
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/.gdbinit DESTINATION ${EXECUTABLE_OUTPUT_PATH})
endif()

add_executable(${TARGET_NAME} ${SOURCE_FILES})

set(BIN_TARGET "${EXECUTABLE_OUTPUT_PATH}/${TARGET_NAME}.bin")
set(HEX_TARGET "${EXECUTABLE_OUTPUT_PATH}/${TARGET_NAME}.hex")
set(ELF_TARGET "${EXECUTABLE_OUTPUT_PATH}/${TARGET_NAME}${CMAKE_EXECUTABLE_SUFFIX}")

add_custom_command(OUTPUT ${BIN_TARGET}
                    COMMAND ${OBJCOPY} -O binary ${ELF_TARGET} ${BIN_TARGET}
                    DEPENDS ${ELF_TARGET} ${PROJECT_NAME})

add_custom_target(bin DEPENDS ${BIN_TARGET})

add_custom_command(OUTPUT ${HEX_TARGET}
                    COMMAND ${OBJCOPY} -O ihex ${ELF_TARGET} ${HEX_TARGET}
                    DEPENDS ${ELF_TARGET} ${PROJECT_NAME})

add_custom_target(hex DEPENDS ${HEX_TARGET})

add_custom_command(OUTPUT flash_target
        COMMAND ${STFLASH} write ${BIN_TARGET} 0x8000000
        DEPENDS ${BIN_TARGET})

add_custom_target(flash DEPENDS flash_target)