################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/scene/renderer3d/model/displayer/CustomModelUniform.cpp \
../src/scene/renderer3d/model/displayer/CustomUniform.cpp \
../src/scene/renderer3d/model/displayer/MeshParameter.cpp \
../src/scene/renderer3d/model/displayer/ModelDisplayer.cpp 

OBJS += \
./src/scene/renderer3d/model/displayer/CustomModelUniform.o \
./src/scene/renderer3d/model/displayer/CustomUniform.o \
./src/scene/renderer3d/model/displayer/MeshParameter.o \
./src/scene/renderer3d/model/displayer/ModelDisplayer.o 

CPP_DEPS += \
./src/scene/renderer3d/model/displayer/CustomModelUniform.d \
./src/scene/renderer3d/model/displayer/CustomUniform.d \
./src/scene/renderer3d/model/displayer/MeshParameter.d \
./src/scene/renderer3d/model/displayer/ModelDisplayer.d 


# Each subdirectory must supply rules for building sources it contributes
src/scene/renderer3d/model/displayer/%.o: ../src/scene/renderer3d/model/displayer/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++11 -fPIC -D_DEBUG -DGLEW_STATIC -DGL_GLEXT_PROTOTYPES -I/usr/include/freetype2 -I"/home/greg/Project/VideoGame/urchinEngine/3dEngine/src" -I"/home/greg/Project/VideoGame/urchinEngine/common/src" -I/usr/include/GL -O0 -g3 -Wall -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


