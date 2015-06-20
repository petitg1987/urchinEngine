################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/tools/xml/XmlAttribute.cpp \
../src/tools/xml/XmlChunk.cpp \
../src/tools/xml/XmlParser.cpp \
../src/tools/xml/XmlWriter.cpp 

OBJS += \
./src/tools/xml/XmlAttribute.o \
./src/tools/xml/XmlChunk.o \
./src/tools/xml/XmlParser.o \
./src/tools/xml/XmlWriter.o 

CPP_DEPS += \
./src/tools/xml/XmlAttribute.d \
./src/tools/xml/XmlChunk.d \
./src/tools/xml/XmlParser.d \
./src/tools/xml/XmlWriter.d 


# Each subdirectory must supply rules for building sources it contributes
src/tools/xml/%.o: ../src/tools/xml/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++11 -fPIC -D_DEBUG -I"/home/greg/Project/VideoGame/urchinEngine/common/src" -O0 -g3 -Wall -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


