################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/resources/object/ModelReaderWriter.cpp \
../src/resources/object/RigidBodyReaderWriter.cpp \
../src/resources/object/SceneObject.cpp 

OBJS += \
./src/resources/object/ModelReaderWriter.o \
./src/resources/object/RigidBodyReaderWriter.o \
./src/resources/object/SceneObject.o 

CPP_DEPS += \
./src/resources/object/ModelReaderWriter.d \
./src/resources/object/RigidBodyReaderWriter.d \
./src/resources/object/SceneObject.d 


# Each subdirectory must supply rules for building sources it contributes
src/resources/object/%.o: ../src/resources/object/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++11 -fPIC -D_DEBUG -I"/home/greg/Project/VideoGame/urchinEngine/common/src" -I"/home/greg/Project/VideoGame/urchinEngine/physicsEngine/src" -I"/home/greg/Project/VideoGame/urchinEngine/soundEngine/src" -I"/home/greg/Project/VideoGame/urchinEngine/3dEngine/src" -I"/home/greg/Project/VideoGame/urchinEngine/mapHandler/src" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


