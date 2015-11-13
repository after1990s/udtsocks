################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/autocritical.cpp \
../src/udtconfig.cpp \
../src/udtsocks.cpp \
../src/udtsocksclient.cpp \
../src/udtsocksserver.cpp 

OBJS += \
./src/autocritical.o \
./src/udtconfig.o \
./src/udtsocks.o \
./src/udtsocksclient.o \
./src/udtsocksserver.o 

CPP_DEPS += \
./src/autocritical.d \
./src/udtconfig.d \
./src/udtsocks.d \
./src/udtsocksclient.d \
./src/udtsocksserver.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -Ipthread -Iudt -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


