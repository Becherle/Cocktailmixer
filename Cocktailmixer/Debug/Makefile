################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL := cmd.exe
RM := rm -rf

USER_OBJS :=

LIBS := 
PROJ := 

O_SRCS := 
C_SRCS := 
S_SRCS := 
S_UPPER_SRCS := 
OBJ_SRCS := 
ASM_SRCS := 
PREPROCESSING_SRCS := 
OBJS := 
OBJS_AS_ARGS := 
C_DEPS := 
C_DEPS_AS_ARGS := 
EXECUTABLES := 
OUTPUT_FILE_PATH :=
OUTPUT_FILE_PATH_AS_ARGS :=
AVR_APP_PATH :=$$$AVR_APP_PATH$$$
QUOTE := "
ADDITIONAL_DEPENDENCIES:=
OUTPUT_FILE_DEP:=
LIB_DEP:=
LINKER_SCRIPT_DEP:=

# Every subdirectory with source files must be described here
SUBDIRS := 


# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS +=  \
../AnalogDigitalConverter.cpp \
../Bildschirmausgaben.cpp \
../Bitoperationen.cpp \
../BringMe2IdleState.cpp \
../Faderbewegung.cpp \
../Fader_Motor.cpp \
../IC_74HC595.cpp \
../Init_XMEGA.cpp \
../lcd-routines.cpp \
../LED.cpp \
../main.cpp \
../Pin.cpp \
../Potentiometer.cpp \
../Pumpenklasse.cpp \
../Pumpensteuerung.cpp \
../PwmPin.cpp \
../Schieberegister.cpp \
../Schieberegister_Operationen.cpp \
../StringOperationen.cpp \
../UART.cpp \
../uart_string_handling.cpp


PREPROCESSING_SRCS += 


ASM_SRCS += 


OBJS +=  \
AnalogDigitalConverter.o \
Bildschirmausgaben.o \
Bitoperationen.o \
BringMe2IdleState.o \
Faderbewegung.o \
Fader_Motor.o \
IC_74HC595.o \
Init_XMEGA.o \
lcd-routines.o \
LED.o \
main.o \
Pin.o \
Potentiometer.o \
Pumpenklasse.o \
Pumpensteuerung.o \
PwmPin.o \
Schieberegister.o \
Schieberegister_Operationen.o \
StringOperationen.o \
UART.o \
uart_string_handling.o

OBJS_AS_ARGS +=  \
AnalogDigitalConverter.o \
Bildschirmausgaben.o \
Bitoperationen.o \
BringMe2IdleState.o \
Faderbewegung.o \
Fader_Motor.o \
IC_74HC595.o \
Init_XMEGA.o \
lcd-routines.o \
LED.o \
main.o \
Pin.o \
Potentiometer.o \
Pumpenklasse.o \
Pumpensteuerung.o \
PwmPin.o \
Schieberegister.o \
Schieberegister_Operationen.o \
StringOperationen.o \
UART.o \
uart_string_handling.o

C_DEPS +=  \
AnalogDigitalConverter.d \
Bildschirmausgaben.d \
Bitoperationen.d \
BringMe2IdleState.d \
Faderbewegung.d \
Fader_Motor.d \
IC_74HC595.d \
Init_XMEGA.d \
lcd-routines.d \
LED.d \
main.d \
Pin.d \
Potentiometer.d \
Pumpenklasse.d \
Pumpensteuerung.d \
PwmPin.d \
Schieberegister.d \
Schieberegister_Operationen.d \
StringOperationen.d \
UART.d \
uart_string_handling.d

C_DEPS_AS_ARGS +=  \
AnalogDigitalConverter.d \
Bildschirmausgaben.d \
Bitoperationen.d \
BringMe2IdleState.d \
Faderbewegung.d \
Fader_Motor.d \
IC_74HC595.d \
Init_XMEGA.d \
lcd-routines.d \
LED.d \
main.d \
Pin.d \
Potentiometer.d \
Pumpenklasse.d \
Pumpensteuerung.d \
PwmPin.d \
Schieberegister.d \
Schieberegister_Operationen.d \
StringOperationen.d \
UART.d \
uart_string_handling.d

OUTPUT_FILE_PATH +=Cocktailmixer.elf

OUTPUT_FILE_PATH_AS_ARGS +=Cocktailmixer.elf

ADDITIONAL_DEPENDENCIES:=

OUTPUT_FILE_DEP:= ./makedep.mk

LIB_DEP+= 

LINKER_SCRIPT_DEP+= 


# AVR32/GNU C Compiler











































./%.o: .././%.cpp
	@echo Building file: $<
	@echo Invoking: AVR8/GNU C Compiler : 5.4.0
	$(QUOTE)C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-g++.exe$(QUOTE) -funsigned-char -funsigned-bitfields -DDEBUG  -I"C:\Program Files (x86)\Atmel\Studio\7.0\Packs\atmel\XMEGAA_DFP\1.1.68\include"  -O1 -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -g2 -Wall -mmcu=atxmega32a4 -B "C:\Program Files (x86)\Atmel\Studio\7.0\Packs\atmel\XMEGAA_DFP\1.1.68\gcc\dev\atxmega32a4" -c -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 
	@echo Finished building: $<
	



# AVR32/GNU Preprocessing Assembler



# AVR32/GNU Assembler




ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: $(OUTPUT_FILE_PATH) $(ADDITIONAL_DEPENDENCIES)

$(OUTPUT_FILE_PATH): $(OBJS) $(USER_OBJS) $(OUTPUT_FILE_DEP) $(LIB_DEP) $(LINKER_SCRIPT_DEP)
	@echo Building target: $@
	@echo Invoking: AVR8/GNU Linker : 5.4.0
	$(QUOTE)C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-g++.exe$(QUOTE) -o$(OUTPUT_FILE_PATH_AS_ARGS) $(OBJS_AS_ARGS) $(USER_OBJS) $(LIBS) -Wl,-Map="Cocktailmixer.map" -Wl,--start-group -Wl,-lm  -Wl,--end-group -Wl,--gc-sections -mmcu=atxmega32a4 -B "C:\Program Files (x86)\Atmel\Studio\7.0\Packs\atmel\XMEGAA_DFP\1.1.68\gcc\dev\atxmega32a4"  
	@echo Finished building target: $@
	"C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-objcopy.exe" -O ihex -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures  "Cocktailmixer.elf" "Cocktailmixer.hex"
	"C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-objcopy.exe" -j .eeprom  --set-section-flags=.eeprom=alloc,load --change-section-lma .eeprom=0  --no-change-warnings -O ihex "Cocktailmixer.elf" "Cocktailmixer.eep" || exit 0
	"C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-objdump.exe" -h -S "Cocktailmixer.elf" > "Cocktailmixer.lss"
	"C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-objcopy.exe" -O srec -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures "Cocktailmixer.elf" "Cocktailmixer.srec"
	"C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-size.exe" "Cocktailmixer.elf"
	
	





# Other Targets
clean:
	-$(RM) $(OBJS_AS_ARGS) $(EXECUTABLES)  
	-$(RM) $(C_DEPS_AS_ARGS)   
	rm -rf "Cocktailmixer.elf" "Cocktailmixer.a" "Cocktailmixer.hex" "Cocktailmixer.lss" "Cocktailmixer.eep" "Cocktailmixer.map" "Cocktailmixer.srec" "Cocktailmixer.usersignatures"
	