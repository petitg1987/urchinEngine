################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/resources/model/ConstAnimation.cpp \
../src/resources/model/ConstMesh.cpp \
../src/resources/model/ConstMeshes.cpp \
../src/resources/model/MeshService.cpp 

OBJS += \
./src/resources/model/ConstAnimation.o \
./src/resources/model/ConstMesh.o \
./src/resources/model/ConstMeshes.o \
./src/resources/model/MeshService.o 

CPP_DEPS += \
./src/resources/model/ConstAnimation.d \
./src/resources/model/ConstMesh.d \
./src/resources/model/ConstMeshes.d \
./src/resources/model/MeshService.d 


# Each subdirectory must supply rules for building sources it contributes
src/resources/model/%.o: ../src/resources/model/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++11 -fPIC -D_DEBUG -DGLEW_STATIC -DGL_GLEXT_PROTOTYPES -I/usr/include/freetype2 -I"/home/greg/Project/VideoGame/urchinEngine/3dEngine/src" -I"/home/greg/Project/VideoGame/urchinEngine/common/src" -I/usr/include/GL -O0 -g3 -Wall -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


