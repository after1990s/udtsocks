################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/autocritical.cpp \
../src/socks5protocol.cpp \
../src/socks5protocol_test.cpp \
../src/udtconfig.cpp \
../src/udtforwardclient.cpp \
../src/udtforwardclient_test.cpp \
../src/udtsocks.cpp \
../src/udtsocksserver.cpp 

OBJS += \
./src/autocritical.o \
./src/socks5protocol.o \
./src/socks5protocol_test.o \
./src/udtconfig.o \
./src/udtforwardclient.o \
./src/udtforwardclient_test.o \
./src/udtsocks.o \
./src/udtsocksserver.o 

CPP_DEPS += \
./src/autocritical.d \
./src/socks5protocol.d \
./src/socks5protocol_test.d \
./src/udtconfig.d \
./src/udtforwardclient.d \
./src/udtforwardclient_test.d \
./src/udtsocks.d \
./src/udtsocksserver.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -Ipthread -Iudt -O0 -g3 -Wall -c -fmessage-length=0 -std=c++0x -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


