##########################################################################################################################
# File automatically-generated by tool: [projectgenerator] version: [3.18.0-B7] date: [Fri Jan 06 11:21:42 GMT+02:00 2023] 
##########################################################################################################################

# ------------------------------------------------
# Generic Makefile (based on gcc)
#
# ChangeLog :
#	2017-02-10 - Several enhancements + project update mode
#   2015-07-22 - first version
# ------------------------------------------------

######################################
# target
######################################
TARGET = malinternshipHUB


######################################
# building variables
######################################
# debug build?
DEBUG = 1
# optimization
OPT = -Og


#######################################
# paths
#######################################
# Build path
BUILD_DIR = build

######################################
# source
######################################
# C sources
C_SOURCES =  \
AZURE_RTOS/App/app_azure_rtos.c \
Core/Src/adc.c \
Core/Src/app_threadx.c \
Core/Src/dma.c \
Core/Src/gpio.c \
Core/Src/iwdg.c \
Core/Src/main.c \
Core/Src/quadspi.c \
Core/Src/spi.c \
Core/Src/stm32f4xx_hal_msp.c \
Core/Src/stm32f4xx_hal_timebase_tim.c \
Core/Src/stm32f4xx_it.c \
Core/Src/system_stm32f4xx.c \
Core/Src/tim.c \
Core/Src/usart.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_adc.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_adc_ex.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma_ex.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_exti.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ex.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ramfunc.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_iwdg.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr_ex.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_qspi.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_spi.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim_ex.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_uart.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_adc.c \
Middlewares/ST/threadx/common/src/tx_initialize_high_level.c \
Middlewares/ST/threadx/common/src/tx_initialize_kernel_enter.c \
Middlewares/ST/threadx/common/src/tx_initialize_kernel_setup.c \
Middlewares/ST/threadx/common/src/tx_block_allocate.c \
Middlewares/ST/threadx/common/src/tx_block_pool_cleanup.c \
Middlewares/ST/threadx/common/src/tx_block_pool_create.c \
Middlewares/ST/threadx/common/src/tx_block_pool_delete.c \
Middlewares/ST/threadx/common/src/tx_block_pool_info_get.c \
Middlewares/ST/threadx/common/src/tx_block_pool_initialize.c \
Middlewares/ST/threadx/common/src/tx_block_pool_prioritize.c \
Middlewares/ST/threadx/common/src/tx_block_release.c \
Middlewares/ST/threadx/common/src/tx_byte_allocate.c \
Middlewares/ST/threadx/common/src/tx_byte_pool_cleanup.c \
Middlewares/ST/threadx/common/src/tx_byte_pool_create.c \
Middlewares/ST/threadx/common/src/tx_byte_pool_delete.c \
Middlewares/ST/threadx/common/src/tx_byte_pool_info_get.c \
Middlewares/ST/threadx/common/src/tx_byte_pool_initialize.c \
Middlewares/ST/threadx/common/src/tx_byte_pool_prioritize.c \
Middlewares/ST/threadx/common/src/tx_byte_pool_search.c \
Middlewares/ST/threadx/common/src/tx_byte_release.c \
Middlewares/ST/threadx/common/src/tx_event_flags_cleanup.c \
Middlewares/ST/threadx/common/src/tx_event_flags_create.c \
Middlewares/ST/threadx/common/src/tx_event_flags_delete.c \
Middlewares/ST/threadx/common/src/tx_event_flags_get.c \
Middlewares/ST/threadx/common/src/tx_event_flags_info_get.c \
Middlewares/ST/threadx/common/src/tx_event_flags_initialize.c \
Middlewares/ST/threadx/common/src/tx_event_flags_set.c \
Middlewares/ST/threadx/common/src/tx_event_flags_set_notify.c \
Middlewares/ST/threadx/common/src/tx_mutex_cleanup.c \
Middlewares/ST/threadx/common/src/tx_mutex_create.c \
Middlewares/ST/threadx/common/src/tx_mutex_delete.c \
Middlewares/ST/threadx/common/src/tx_mutex_get.c \
Middlewares/ST/threadx/common/src/tx_mutex_info_get.c \
Middlewares/ST/threadx/common/src/tx_mutex_initialize.c \
Middlewares/ST/threadx/common/src/tx_mutex_prioritize.c \
Middlewares/ST/threadx/common/src/tx_mutex_priority_change.c \
Middlewares/ST/threadx/common/src/tx_mutex_put.c \
Middlewares/ST/threadx/common/src/tx_queue_cleanup.c \
Middlewares/ST/threadx/common/src/tx_queue_create.c \
Middlewares/ST/threadx/common/src/tx_queue_delete.c \
Middlewares/ST/threadx/common/src/tx_queue_flush.c \
Middlewares/ST/threadx/common/src/tx_queue_front_send.c \
Middlewares/ST/threadx/common/src/tx_queue_info_get.c \
Middlewares/ST/threadx/common/src/tx_queue_initialize.c \
Middlewares/ST/threadx/common/src/tx_queue_prioritize.c \
Middlewares/ST/threadx/common/src/tx_queue_receive.c \
Middlewares/ST/threadx/common/src/tx_queue_send.c \
Middlewares/ST/threadx/common/src/tx_queue_send_notify.c \
Middlewares/ST/threadx/common/src/tx_semaphore_ceiling_put.c \
Middlewares/ST/threadx/common/src/tx_semaphore_cleanup.c \
Middlewares/ST/threadx/common/src/tx_semaphore_create.c \
Middlewares/ST/threadx/common/src/tx_semaphore_delete.c \
Middlewares/ST/threadx/common/src/tx_semaphore_get.c \
Middlewares/ST/threadx/common/src/tx_semaphore_info_get.c \
Middlewares/ST/threadx/common/src/tx_semaphore_initialize.c \
Middlewares/ST/threadx/common/src/tx_semaphore_prioritize.c \
Middlewares/ST/threadx/common/src/tx_semaphore_put.c \
Middlewares/ST/threadx/common/src/tx_semaphore_put_notify.c \
Middlewares/ST/threadx/common/src/tx_thread_create.c \
Middlewares/ST/threadx/common/src/tx_thread_delete.c \
Middlewares/ST/threadx/common/src/tx_thread_entry_exit_notify.c \
Middlewares/ST/threadx/common/src/tx_thread_identify.c \
Middlewares/ST/threadx/common/src/tx_thread_info_get.c \
Middlewares/ST/threadx/common/src/tx_thread_initialize.c \
Middlewares/ST/threadx/common/src/tx_thread_preemption_change.c \
Middlewares/ST/threadx/common/src/tx_thread_priority_change.c \
Middlewares/ST/threadx/common/src/tx_thread_relinquish.c \
Middlewares/ST/threadx/common/src/tx_thread_reset.c \
Middlewares/ST/threadx/common/src/tx_thread_resume.c \
Middlewares/ST/threadx/common/src/tx_thread_shell_entry.c \
Middlewares/ST/threadx/common/src/tx_thread_sleep.c \
Middlewares/ST/threadx/common/src/tx_thread_stack_analyze.c \
Middlewares/ST/threadx/common/src/tx_thread_stack_error_handler.c \
Middlewares/ST/threadx/common/src/tx_thread_stack_error_notify.c \
Middlewares/ST/threadx/common/src/tx_thread_suspend.c \
Middlewares/ST/threadx/common/src/tx_thread_system_preempt_check.c \
Middlewares/ST/threadx/common/src/tx_thread_system_resume.c \
Middlewares/ST/threadx/common/src/tx_thread_system_suspend.c \
Middlewares/ST/threadx/common/src/tx_thread_terminate.c \
Middlewares/ST/threadx/common/src/tx_thread_time_slice.c \
Middlewares/ST/threadx/common/src/tx_thread_time_slice_change.c \
Middlewares/ST/threadx/common/src/tx_thread_timeout.c \
Middlewares/ST/threadx/common/src/tx_thread_wait_abort.c \
Middlewares/ST/threadx/common/src/tx_time_get.c \
Middlewares/ST/threadx/common/src/tx_time_set.c \
Middlewares/ST/threadx/common/src/txe_block_allocate.c \
Middlewares/ST/threadx/common/src/txe_block_pool_create.c \
Middlewares/ST/threadx/common/src/txe_block_pool_delete.c \
Middlewares/ST/threadx/common/src/txe_block_pool_info_get.c \
Middlewares/ST/threadx/common/src/txe_block_pool_prioritize.c \
Middlewares/ST/threadx/common/src/txe_block_release.c \
Middlewares/ST/threadx/common/src/txe_byte_allocate.c \
Middlewares/ST/threadx/common/src/txe_byte_pool_create.c \
Middlewares/ST/threadx/common/src/txe_byte_pool_delete.c \
Middlewares/ST/threadx/common/src/txe_byte_pool_info_get.c \
Middlewares/ST/threadx/common/src/txe_byte_pool_prioritize.c \
Middlewares/ST/threadx/common/src/txe_byte_release.c \
Middlewares/ST/threadx/common/src/txe_event_flags_create.c \
Middlewares/ST/threadx/common/src/txe_event_flags_delete.c \
Middlewares/ST/threadx/common/src/txe_event_flags_get.c \
Middlewares/ST/threadx/common/src/txe_event_flags_info_get.c \
Middlewares/ST/threadx/common/src/txe_event_flags_set.c \
Middlewares/ST/threadx/common/src/txe_event_flags_set_notify.c \
Middlewares/ST/threadx/common/src/txe_mutex_create.c \
Middlewares/ST/threadx/common/src/txe_mutex_delete.c \
Middlewares/ST/threadx/common/src/txe_mutex_get.c \
Middlewares/ST/threadx/common/src/txe_mutex_info_get.c \
Middlewares/ST/threadx/common/src/txe_mutex_prioritize.c \
Middlewares/ST/threadx/common/src/txe_mutex_put.c \
Middlewares/ST/threadx/common/src/txe_queue_create.c \
Middlewares/ST/threadx/common/src/txe_queue_delete.c \
Middlewares/ST/threadx/common/src/txe_queue_flush.c \
Middlewares/ST/threadx/common/src/txe_queue_front_send.c \
Middlewares/ST/threadx/common/src/txe_queue_info_get.c \
Middlewares/ST/threadx/common/src/txe_queue_prioritize.c \
Middlewares/ST/threadx/common/src/txe_queue_receive.c \
Middlewares/ST/threadx/common/src/txe_queue_send.c \
Middlewares/ST/threadx/common/src/txe_queue_send_notify.c \
Middlewares/ST/threadx/common/src/txe_semaphore_ceiling_put.c \
Middlewares/ST/threadx/common/src/txe_semaphore_create.c \
Middlewares/ST/threadx/common/src/txe_semaphore_delete.c \
Middlewares/ST/threadx/common/src/txe_semaphore_get.c \
Middlewares/ST/threadx/common/src/txe_semaphore_info_get.c \
Middlewares/ST/threadx/common/src/txe_semaphore_prioritize.c \
Middlewares/ST/threadx/common/src/txe_semaphore_put.c \
Middlewares/ST/threadx/common/src/txe_semaphore_put_notify.c \
Middlewares/ST/threadx/common/src/txe_thread_create.c \
Middlewares/ST/threadx/common/src/txe_thread_delete.c \
Middlewares/ST/threadx/common/src/txe_thread_entry_exit_notify.c \
Middlewares/ST/threadx/common/src/txe_thread_info_get.c \
Middlewares/ST/threadx/common/src/txe_thread_preemption_change.c \
Middlewares/ST/threadx/common/src/txe_thread_priority_change.c \
Middlewares/ST/threadx/common/src/txe_thread_relinquish.c \
Middlewares/ST/threadx/common/src/txe_thread_reset.c \
Middlewares/ST/threadx/common/src/txe_thread_resume.c \
Middlewares/ST/threadx/common/src/txe_thread_suspend.c \
Middlewares/ST/threadx/common/src/txe_thread_terminate.c \
Middlewares/ST/threadx/common/src/txe_thread_time_slice_change.c \
Middlewares/ST/threadx/common/src/txe_thread_wait_abort.c \
Middlewares/ST/threadx/common/src/tx_timer_activate.c \
Middlewares/ST/threadx/common/src/tx_timer_change.c \
Middlewares/ST/threadx/common/src/tx_timer_create.c \
Middlewares/ST/threadx/common/src/tx_timer_deactivate.c \
Middlewares/ST/threadx/common/src/tx_timer_delete.c \
Middlewares/ST/threadx/common/src/tx_timer_expiration_process.c \
Middlewares/ST/threadx/common/src/tx_timer_info_get.c \
Middlewares/ST/threadx/common/src/tx_timer_initialize.c \
Middlewares/ST/threadx/common/src/tx_timer_system_activate.c \
Middlewares/ST/threadx/common/src/tx_timer_system_deactivate.c \
Middlewares/ST/threadx/common/src/tx_timer_thread_entry.c \
Middlewares/ST/threadx/common/src/txe_timer_activate.c \
Middlewares/ST/threadx/common/src/txe_timer_change.c \
Middlewares/ST/threadx/common/src/txe_timer_create.c \
Middlewares/ST/threadx/common/src/txe_timer_deactivate.c \
Middlewares/ST/threadx/common/src/txe_timer_delete.c \
Middlewares/ST/threadx/common/src/txe_timer_info_get.c \
../../Driver/RFM66/Src/rfm66.c \
HWSpecific/Power/Src/HUBpower.c \
../../Driver/W5500/Src/wiznet_w5500.c \
../../Driver/wizchip/Src/wizchip.c \
../../Module/ThirdParty/ioLibrary_Driver/Ethernet/socket.c \
../../Module/ThirdParty/ioLibrary_Driver/Ethernet/wizchip_conf.c \
../../Module/ThirdParty/ioLibrary_Driver/Ethernet/W5500/w5500.c \
../../Module/ThirdParty/ioLibrary_Driver/Internet/httpServer/httpServer.c \
../../Module/ThirdParty/ioLibrary_Driver/Internet/httpServer/httpParser.c \
../../Module/ThirdParty/ioLibrary_Driver/Internet/httpServer/httpUtil.c \
../../Module/ThirdParty/ioLibrary_Driver/Internet/DNS/dns.c \
../../Module/ThirdParty/ioLibrary_Driver/Internet/DHCP/dhcp.c \
../../Module/Adapter/Ethernet/Src/ethernet_adapter.c \
../../Module/Adapter/Web/Src/web_adapter.c \
../../Module/Logging/Src/loglib.c \
../../Module/Parser/Src/parser.c \
../../Module/Geode/Src/geode.c \
../../Module/LED/Src/LED.c \
../../Module/Power/Src/power.c \
../../Utility/Boot_reason/Src/boot_reason.c

#../../App/Weather/Src/weather_app.c \
# ../../Driver/ESP32/Src/esp32.c \
# ../../Utility/AT_Utilities/Src/at_utilities.c
#../../App/BattleShip/Src/bs_creator.c \
#../../Module/Adapter/Modem/Src/modem_adapter.c \
#../../App/BattleShip/Src/bs_main.c \
#../../App/BattleShip/Src/bs_shooting.c \
#../../App/BattleShip/Src/bs_utility.c \
#../../Driver/EC21/Src/modem.c \
#../../Driver/EC21/Src/modem_cmd.c \
#../../Driver/EC21/Src/modem_command_gen.c \
#../../Driver/EC21/Src/modem_urc_parser.c \
#../../Driver/IS25LP032D/Src/is25l.c \
#../../Module/FLASH/Src/flash.c \
#../../Module/Indication/Src/indication.c \

# ASM sources
ASM_SOURCES =  \
startup_stm32f413xx.s \
Core/Src/tx_initialize_low_level.s

# ASM_S sources
ASM_S_SOURCES =  \
Middlewares/ST/threadx/ports/cortex_m4/gnu/src/tx_thread_context_restore.S \
Middlewares/ST/threadx/ports/cortex_m4/gnu/src/tx_thread_context_save.S \
Middlewares/ST/threadx/ports/cortex_m4/gnu/src/tx_thread_interrupt_control.S \
Middlewares/ST/threadx/ports/cortex_m4/gnu/src/tx_thread_schedule.S \
Middlewares/ST/threadx/ports/cortex_m4/gnu/src/tx_thread_stack_build.S \
Middlewares/ST/threadx/ports/cortex_m4/gnu/src/tx_thread_system_return.S \
Middlewares/ST/threadx/ports/cortex_m4/gnu/src/tx_timer_interrupt.S

#######################################
# binaries
#######################################
PREFIX = arm-none-eabi-
# The gcc compiler bin path can be either defined in make command via GCC_PATH variable (> make GCC_PATH=xxx)
# either it can be added to the PATH environment variable.
ifdef GCC_PATH
CC = $(GCC_PATH)/$(PREFIX)gcc
AS = $(GCC_PATH)/$(PREFIX)gcc -x assembler-with-cpp
CP = $(GCC_PATH)/$(PREFIX)objcopy
SZ = $(GCC_PATH)/$(PREFIX)size
else
CC = $(PREFIX)gcc
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size
endif
HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S
 
#######################################
# CFLAGS
#######################################
# cpu
CPU = -mcpu=cortex-m4

# fpu
FPU = -mfpu=fpv4-sp-d16

# float-abi
FLOAT-ABI = -mfloat-abi=hard

# mcu
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

# macros for gcc
# AS defines
AS_DEFS = 

# C defines
C_DEFS =  \
-DUSE_HAL_DRIVER \
-DSTM32F413xx \
-DTX_INCLUDE_USER_DEFINE_FILE \
-DTARGET_$(BUILD_TARGET)


# AS includes
AS_INCLUDES = 

# C includes
C_INCLUDES =  \
-IAZURE_RTOS/App \
-ICore/Inc \
-IDrivers/CMSIS/Include \
-IDrivers/CMSIS/Device/ST/STM32F4xx/Include \
-IDrivers/STM32F4xx_HAL_Driver/Inc \
-IDrivers/STM32F4xx_HAL_Driver/Inc/Legacy \
-IMiddlewares/ST/threadx/common/inc/ \
-IMiddlewares/ST/threadx/ports/cortex_m4/gnu/inc/ \
-IHWSpecific/Power/Inc \
-I../../App/BattleShip/Inc \
-I../../App/Weather/Inc \
-I../../Driver/EC21/Inc \
-I../../Driver/ESP32/Inc \
-I../../Driver/IS25LP032D/Inc \
-I../../Driver/RFM66/Inc \
-I../../Driver/wizchip/Inc \
-I../../Driver/W5500/Inc \
-I../../Module/Adapter/Ethernet/Inc \
-I../../Module/Adapter/Modem/Inc \
-I../../Module/Adapter/Web/Inc \
-I../../Module/FLASH/Inc \
-I../../Module/Indication/Inc \
-I../../Module/LED/Inc \
-I../../Module/Logging/Inc \
-I../../Module/Parser/Inc \
-I../../Module/Power/Inc \
-I../../Module/ThirdParty/ioLibrary_Driver \
-I../../Module/ThirdParty/ioLibrary_Driver/Application/loopback \
-I../../Module/ThirdParty/ioLibrary_Driver/Application/multicast \
-I../../Module/ThirdParty/ioLibrary_Driver/Ethernet \
-I../../Module/ThirdParty/ioLibrary_Driver/Ethernet/W5500 \
-I../../Module/Extm/Inc \
-I../../Module/Geode/Inc \
-I../../Driver/EC21/Inc \
-I../../Driver/IS25LP032D/Inc \
-I../../Driver/wizchip/Inc \
-I../../Driver/W5500/Inc \
-I../../Utility/Boot_reason/Inc \
-IAZURE_RTOS/App \
-IMiddlewares/ST/threadx/common/inc/ \
-IMiddlewares/ST/threadx/ports/cortex_m4/gnu/inc/


# compile gcc flags
ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

CFLAGS += $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections
CFLAGS += -Werror
CFLAGS += -Wno-missing-braces	# Fix compiler bug

ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
endif


# Generate dependency information
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"


#######################################
# LDFLAGS
#######################################
# link script
LDSCRIPT = STM32F413VGTx_FLASH.ld

# libraries
LIBS = -lc -lm -lnosys 
LIBDIR = 
LDFLAGS = $(MCU) -specs=nano.specs -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections
LDFLAGS += -Wl,--print-memory-usage

# default action: build all
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin


#######################################
# build the application
#######################################
# list of objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
# list of ASM program objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))
# list of ASM_S program objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_S_SOURCES:.S=.o)))
vpath %.S $(sort $(dir $(ASM_S_SOURCES)))

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR) 
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	$(AS) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/%.o: %.S Makefile | $(BUILD_DIR)
	$(AS) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SZ) $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(HEX) $< $@
	
$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(BIN) $< $@	
	
$(BUILD_DIR):
	mkdir $@		

#######################################
# clean up
#######################################
clean:
	-rm -fR $(BUILD_DIR)

#######################################
# Flash
#######################################

OPENOCD_CONFIG_FILE := openocd.cfg
PROG = openocd

OPENOCD_LOAD_COMMAND = \
-c "init"\
-c "halt"\
-c "flash protect 0 0 7 off"\
-c "stm32f4x unlock 0"\
-c "reset init"\
-c "flash write_image erase unlock $(abspath $(BUILD_DIR)/$(TARGET).hex)"\
-c "reset run"\
-c "shutdown"\

OPENOCD_RESET_COMMAND := \
-c "init"\
-c "reset run"\
-c "shutdown"\

OPENOCD_ERASE_COMMAND := \
-c "init"\
-c "halt"\
-c "wait_halt"\
-c "flash protect 0 0 7 off"\
-c "stm32f4x unlock 0"\
-c "stm32f4x mass_erase 0"\
-c "reset run"\
-c "shutdown"\

flash: $(BUILD_DIR)/$(TARGET).hex
	$(PROG) -f $(OPENOCD_CONFIG_FILE) $(OPENOCD_HLA_SERIAL_STR) $(OPENOCD_LOAD_COMMAND)

reset: $(OPENOCD_CONFIG_FILE)
	$(PROG) -f $(OPENOCD_CONFIG_FILE) $(OPENOCD_HLA_SERIAL_STR) $(OPENOCD_RESET_COMMAND)

erase: $(OPENOCD_CONFIG_FILE)
	$(PROG) -f $(OPENOCD_CONFIG_FILE) $(OPENOCD_HLA_SERIAL_STR) $(OPENOCD_ERASE_COMMAND)


#######################################
# dependencies
#######################################
-include $(wildcard $(BUILD_DIR)/*.d)

# *** EOF ***
