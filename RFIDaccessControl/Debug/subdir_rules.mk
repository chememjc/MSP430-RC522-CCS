################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
globals.obj: ../globals.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP430 Compiler'
	"/opt/ti/ccsv6/tools/compiler/ti-cgt-msp430_4.4.7/bin/cl430" -vmspx --abi=eabi --data_model=restricted --use_hw_mpy=16 --include_path="/opt/ti/ccsv6/ccs_base/msp430/include" --include_path="/opt/ti/ccsv6/tools/compiler/ti-cgt-msp430_4.4.7/include" --advice:power=all -g --define=__MSP430F2419__ --display_error_number --diag_warning=225 --diag_wrap=off --printf_support=minimal --preproc_with_compile --preproc_dependency="globals.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

i2c.obj: ../i2c.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP430 Compiler'
	"/opt/ti/ccsv6/tools/compiler/ti-cgt-msp430_4.4.7/bin/cl430" -vmspx --abi=eabi --data_model=restricted --use_hw_mpy=16 --include_path="/opt/ti/ccsv6/ccs_base/msp430/include" --include_path="/opt/ti/ccsv6/tools/compiler/ti-cgt-msp430_4.4.7/include" --advice:power=all -g --define=__MSP430F2419__ --display_error_number --diag_warning=225 --diag_wrap=off --printf_support=minimal --preproc_with_compile --preproc_dependency="i2c.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

main.obj: ../main.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP430 Compiler'
	"/opt/ti/ccsv6/tools/compiler/ti-cgt-msp430_4.4.7/bin/cl430" -vmspx --abi=eabi --data_model=restricted --use_hw_mpy=16 --include_path="/opt/ti/ccsv6/ccs_base/msp430/include" --include_path="/opt/ti/ccsv6/tools/compiler/ti-cgt-msp430_4.4.7/include" --advice:power=all -g --define=__MSP430F2419__ --display_error_number --diag_warning=225 --diag_wrap=off --printf_support=minimal --preproc_with_compile --preproc_dependency="main.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

pcf8574lcd.obj: ../pcf8574lcd.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP430 Compiler'
	"/opt/ti/ccsv6/tools/compiler/ti-cgt-msp430_4.4.7/bin/cl430" -vmspx --abi=eabi --data_model=restricted --use_hw_mpy=16 --include_path="/opt/ti/ccsv6/ccs_base/msp430/include" --include_path="/opt/ti/ccsv6/tools/compiler/ti-cgt-msp430_4.4.7/include" --advice:power=all -g --define=__MSP430F2419__ --display_error_number --diag_warning=225 --diag_wrap=off --printf_support=minimal --preproc_with_compile --preproc_dependency="pcf8574lcd.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

rc522.obj: ../rc522.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP430 Compiler'
	"/opt/ti/ccsv6/tools/compiler/ti-cgt-msp430_4.4.7/bin/cl430" -vmspx --abi=eabi --data_model=restricted --use_hw_mpy=16 --include_path="/opt/ti/ccsv6/ccs_base/msp430/include" --include_path="/opt/ti/ccsv6/tools/compiler/ti-cgt-msp430_4.4.7/include" --advice:power=all -g --define=__MSP430F2419__ --display_error_number --diag_warning=225 --diag_wrap=off --printf_support=minimal --preproc_with_compile --preproc_dependency="rc522.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

rfid.obj: ../rfid.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP430 Compiler'
	"/opt/ti/ccsv6/tools/compiler/ti-cgt-msp430_4.4.7/bin/cl430" -vmspx --abi=eabi --data_model=restricted --use_hw_mpy=16 --include_path="/opt/ti/ccsv6/ccs_base/msp430/include" --include_path="/opt/ti/ccsv6/tools/compiler/ti-cgt-msp430_4.4.7/include" --advice:power=all -g --define=__MSP430F2419__ --display_error_number --diag_warning=225 --diag_wrap=off --printf_support=minimal --preproc_with_compile --preproc_dependency="rfid.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

rs232shell.obj: ../rs232shell.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP430 Compiler'
	"/opt/ti/ccsv6/tools/compiler/ti-cgt-msp430_4.4.7/bin/cl430" -vmspx --abi=eabi --data_model=restricted --use_hw_mpy=16 --include_path="/opt/ti/ccsv6/ccs_base/msp430/include" --include_path="/opt/ti/ccsv6/tools/compiler/ti-cgt-msp430_4.4.7/include" --advice:power=all -g --define=__MSP430F2419__ --display_error_number --diag_warning=225 --diag_wrap=off --printf_support=minimal --preproc_with_compile --preproc_dependency="rs232shell.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

spi.obj: ../spi.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP430 Compiler'
	"/opt/ti/ccsv6/tools/compiler/ti-cgt-msp430_4.4.7/bin/cl430" -vmspx --abi=eabi --data_model=restricted --use_hw_mpy=16 --include_path="/opt/ti/ccsv6/ccs_base/msp430/include" --include_path="/opt/ti/ccsv6/tools/compiler/ti-cgt-msp430_4.4.7/include" --advice:power=all -g --define=__MSP430F2419__ --display_error_number --diag_warning=225 --diag_wrap=off --printf_support=minimal --preproc_with_compile --preproc_dependency="spi.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '


