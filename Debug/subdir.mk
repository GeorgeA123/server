################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../getListenSocket.c \
../main.c \
../make_printable_address.c \
../sendResponse.c \
../serviceListenSocketMultiThread.c \
../service_client_socket.c 

OBJS += \
./getListenSocket.o \
./main.o \
./make_printable_address.o \
./sendResponse.o \
./serviceListenSocketMultiThread.o \
./service_client_socket.o 

C_DEPS += \
./getListenSocket.d \
./main.d \
./make_printable_address.d \
./sendResponse.d \
./serviceListenSocketMultiThread.d \
./service_client_socket.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


