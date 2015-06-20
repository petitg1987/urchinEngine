################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/scene/GUI/GUIManager.cpp \
../src/scene/GUI/GUISkinService.cpp \
../src/scene/GUI/Widget.cpp 

OBJS += \
./src/scene/GUI/GUIManager.o \
./src/scene/GUI/GUISkinService.o \
./src/scene/GUI/Widget.o 

CPP_DEPS += \
./src/scene/GUI/GUIManager.d \
./src/scene/GUI/GUISkinService.d \
./src/scene/GUI/Widget.d 


# Each subdirectory must supply rules for building sources it contributes
src/scene/GUI/%.o: ../src/scene/GUI/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++11 -fPIC -D_DEBUG -DGLEW_STATIC -DGL_GLEXT_PROTOTYPES -I/usr/include/freetype2 -I"/home/greg/Project/VideoGame/urchinEngine/3dEngine/src" -I"/home/greg/Project/VideoGame/urchinEngine/common/src" -I/usr/include/GL -O0 -g3 -Wall -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


