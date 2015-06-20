################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/tools/xml/tinyxml/tinyxml.cpp \
../src/tools/xml/tinyxml/tinyxmlerror.cpp \
../src/tools/xml/tinyxml/tinyxmlparser.cpp 

OBJS += \
./src/tools/xml/tinyxml/tinyxml.o \
./src/tools/xml/tinyxml/tinyxmlerror.o \
./src/tools/xml/tinyxml/tinyxmlparser.o 

CPP_DEPS += \
./src/tools/xml/tinyxml/tinyxml.d \
./src/tools/xml/tinyxml/tinyxmlerror.d \
./src/tools/xml/tinyxml/tinyxmlparser.d 


# Each subdirectory must supply rules for building sources it contributes
src/tools/xml/tinyxml/%.o: ../src/tools/xml/tinyxml/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++11 -fPIC -D_DEBUG -I"/home/greg/Project/VideoGame/urchinEngine/common/src" -O0 -g3 -Wall -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


