################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/utils/display/geometry/GeometryDisplayer.cpp \
../src/utils/display/geometry/GeometryModel.cpp 

OBJS += \
./src/utils/display/geometry/GeometryDisplayer.o \
./src/utils/display/geometry/GeometryModel.o 

CPP_DEPS += \
./src/utils/display/geometry/GeometryDisplayer.d \
./src/utils/display/geometry/GeometryModel.d 


# Each subdirectory must supply rules for building sources it contributes
src/utils/display/geometry/%.o: ../src/utils/display/geometry/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++11 -fPIC -D_DEBUG -DGLEW_STATIC -DGL_GLEXT_PROTOTYPES -I/usr/include/freetype2 -I"/home/greg/Project/VideoGame/urchinEngine/3dEngine/src" -I"/home/greg/Project/VideoGame/urchinEngine/common/src" -I/usr/include/GL -O0 -g3 -Wall -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


