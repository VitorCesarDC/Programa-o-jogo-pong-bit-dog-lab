cmake_minimum_required(VERSION 3.13)

# Ajuste o caminho para o seu pico-sdk (ou defina a env var PICO_SDK_PATH)
include(pico_sdk_import.cmake)

project(pong_bitdoglab CXX C ASM)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

pico_sdk_init()

# Fontes compartilhadas entre o jogo real e o menu de teste (evita
# duplicar a compilação da mesma lógica em dois lugares).
set(SHARED_SOURCES
    src/entities/Ball.cpp
    src/entities/Paddle.cpp
    src/hal/CelebrationEffects.cpp
    src/hal/ssd1306.c
)

set(SHARED_INCLUDES
    ${CMAKE_CURRENT_LIST_DIR}/include
    ${CMAKE_CURRENT_LIST_DIR}/src
    # ${CMAKE_CURRENT_LIST_DIR}/libs/ssd1306
)

set(SHARED_LIBS
    pico_stdlib
    hardware_adc
    hardware_pwm
    hardware_i2c
    hardware_pio
)

# ------------------------------------------------------------
# Executável 1: o jogo Pong completo
# ------------------------------------------------------------
add_executable(pong_bitdoglab
    src/main.cpp
    ${SHARED_SOURCES}
    src/game/GameManager.cpp
    # Adicione aqui o(s) arquivo(s) .c/.cpp da lib ssd1306 do curso, ex.:
    # libs/ssd1306/ssd1306.c
)
target_include_directories(pong_bitdoglab PRIVATE ${SHARED_INCLUDES})
target_link_libraries(pong_bitdoglab ${SHARED_LIBS})
pico_generate_pio_header(pong_bitdoglab ${CMAKE_CURRENT_LIST_DIR}/src/hal/ws2812.pio)
pico_enable_stdio_usb(pong_bitdoglab 1)
pico_enable_stdio_uart(pong_bitdoglab 0)
pico_add_extra_outputs(pong_bitdoglab)

# ------------------------------------------------------------
# Executável 2: menu de teste sintético dos efeitos (LED/buzzer/matriz)
# Grave este primeiro na placa pra validar o hardware isoladamente.
# ------------------------------------------------------------
add_executable(pong_test_menu
    src/test/main_test_menu.cpp
    ${SHARED_SOURCES}
    # libs/ssd1306/ssd1306.c
)
target_include_directories(pong_test_menu PRIVATE ${SHARED_INCLUDES})
target_link_libraries(pong_test_menu ${SHARED_LIBS})
pico_generate_pio_header(pong_test_menu ${CMAKE_CURRENT_LIST_DIR}/src/hal/ws2812.pio)
pico_enable_stdio_usb(pong_test_menu 1)
pico_enable_stdio_uart(pong_test_menu 0)
pico_add_extra_outputs(pong_test_menu)
