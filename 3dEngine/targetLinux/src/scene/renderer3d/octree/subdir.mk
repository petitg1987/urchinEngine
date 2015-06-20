################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/scene/renderer3d/octree/Octree.cpp \
../src/scene/renderer3d/octree/OctreeManager.cpp \
../src/scene/renderer3d/octree/Octreeable.cpp 

OBJS += \
./src/scene/renderer3d/octree/Octree.o \
./src/scene/renderer3d/octree/OctreeManager.o \
./src/scene/renderer3d/octree/Octreeable.o 

CPP_DEPS += \
./src/scene/renderer3d/octree/Octree.d \
./src/scene/renderer3d/octree/OctreeManager.d \
./src/scene/renderer3d/octree/Octreeable.d 


# Each subdirectory must supply rules for building sources it contributes
src/scene/renderer3d/octree/%.o: ../src/scene/renderer3d/octree/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++11 -fPIC -D_DEBUG -DGLEW_STATIC -DGL_GLEXT_PROTOTYPES -I/usr/include/freetype2 -I"/home/greg/Project/VideoGame/urchinEngine/3dEngine/src" -I"/home/greg/Project/VideoGame/urchinEngine/common/src" -I/usr/include/GL -O0 -g3 -Wall -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


