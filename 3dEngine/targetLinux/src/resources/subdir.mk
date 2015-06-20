################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/resources/MediaManager.cpp \
../src/resources/Resource.cpp \
../src/resources/ResourceManager.cpp 

OBJS += \
./src/resources/MediaManager.o \
./src/resources/Resource.o \
./src/resources/ResourceManager.o 

CPP_DEPS += \
./src/resources/MediaManager.d \
./src/resources/Resource.d \
./src/resources/ResourceManager.d 


# Each subdirectory must supply rules for building sources it contributes
src/resources/%.o: ../src/resources/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++11 -fPIC -D_DEBUG -DGLEW_STATIC -DGL_GLEXT_PROTOTYPES -I/usr/include/freetype2 -I"/home/greg/Project/VideoGame/urchinEngine/3dEngine/src" -I"/home/greg/Project/VideoGame/urchinEngine/common/src" -I/usr/include/GL -O0 -g3 -Wall -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


