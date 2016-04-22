################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../udtsocks/src/socks5protocol.cpp \
../udtsocks/src/socks5protocol_test.cpp \
../udtsocks/src/udtconfig.cpp \
../udtsocks/src/udtforwardclient.cpp \
../udtsocks/src/udtforwardclient_test.cpp \
../udtsocks/src/udtsocks.cpp \
../udtsocks/src/udtsocksserver.cpp \
../udtsocks/src/udtwrapper.cpp 

OBJS += \
./udtsocks/src/socks5protocol.o \
./udtsocks/src/socks5protocol_test.o \
./udtsocks/src/udtconfig.o \
./udtsocks/src/udtforwardclient.o \
./udtsocks/src/udtforwardclient_test.o \
./udtsocks/src/udtsocks.o \
./udtsocks/src/udtsocksserver.o \
./udtsocks/src/udtwrapper.o 

CPP_DEPS += \
./udtsocks/src/socks5protocol.d \
./udtsocks/src/socks5protocol_test.d \
./udtsocks/src/udtconfig.d \
./udtsocks/src/udtforwardclient.d \
./udtsocks/src/udtforwardclient_test.d \
./udtsocks/src/udtsocks.d \
./udtsocks/src/udtsocksserver.d \
./udtsocks/src/udtwrapper.d 


# Each subdirectory must supply rules for building sources it contributes
udtsocks/src/%.o: ../udtsocks/src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -DDEBUG -Ipthread -Iudt -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


