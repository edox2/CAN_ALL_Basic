################################################################################
# Automatically-generated file. Do not edit!
################################################################################

export WINEDEBUG=-all

# Keil tools exit code 1 is warnings only, so don't treat as an error in make
RC := test $$? -lt 2

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include subdir.mk
-include objects.mk

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: CAN_ALL_CanObjectDispatcherV5.omf

# Tool invocations
CAN_ALL_CanObjectDispatcherV5.omf: echo_path $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Keil 8051 Linker'
	LX51 "@CAN_ALL_CanObjectDispatcherV5.lnp" || $(RC)
	@echo 'Finished building target: $@'

# Change from all upper case to the expected case
	@-mv "CAN_ALL_CANOBJECTDISPATCHERV5.OMF.CRBUILD" "$@"

# Generate hex file: CAN_ALL_CanObjectDispatcherV5.hex
	@Ohx51 "CAN_ALL_CanObjectDispatcherV5.omf" "HEXFILE (CAN_ALL_CanObjectDispatcherV5.hex)" "H386" 2>&1 >/dev/null

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS) CAN_ALL_CanObjectDispatcherV5.omf
	-@echo ' '

.PHONY: all clean dependents

-include ../makefile.targets
# echo the path
echo_path:
	@echo PATH=$$PATH
	@echo ' '

