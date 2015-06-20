################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/resources/object/bodyshape/CollisionBoxReaderWriter.cpp \
../src/resources/object/bodyshape/CollisionCapsuleReaderWriter.cpp \
../src/resources/object/bodyshape/CollisionCompoundShapeReaderWriter.cpp \
../src/resources/object/bodyshape/CollisionConvexHullReaderWriter.cpp \
../src/resources/object/bodyshape/CollisionCylinderReaderWriter.cpp \
../src/resources/object/bodyshape/CollisionShapeReaderWriter.cpp \
../src/resources/object/bodyshape/CollisionShapeReaderWriterRetriever.cpp \
../src/resources/object/bodyshape/CollisionSphereReaderWriter.cpp 

OBJS += \
./src/resources/object/bodyshape/CollisionBoxReaderWriter.o \
./src/resources/object/bodyshape/CollisionCapsuleReaderWriter.o \
./src/resources/object/bodyshape/CollisionCompoundShapeReaderWriter.o \
./src/resources/object/bodyshape/CollisionConvexHullReaderWriter.o \
./src/resources/object/bodyshape/CollisionCylinderReaderWriter.o \
./src/resources/object/bodyshape/CollisionShapeReaderWriter.o \
./src/resources/object/bodyshape/CollisionShapeReaderWriterRetriever.o \
./src/resources/object/bodyshape/CollisionSphereReaderWriter.o 

CPP_DEPS += \
./src/resources/object/bodyshape/CollisionBoxReaderWriter.d \
./src/resources/object/bodyshape/CollisionCapsuleReaderWriter.d \
./src/resources/object/bodyshape/CollisionCompoundShapeReaderWriter.d \
./src/resources/object/bodyshape/CollisionConvexHullReaderWriter.d \
./src/resources/object/bodyshape/CollisionCylinderReaderWriter.d \
./src/resources/object/bodyshape/CollisionShapeReaderWriter.d \
./src/resources/object/bodyshape/CollisionShapeReaderWriterRetriever.d \
./src/resources/object/bodyshape/CollisionSphereReaderWriter.d 


# Each subdirectory must supply rules for building sources it contributes
src/resources/object/bodyshape/%.o: ../src/resources/object/bodyshape/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++11 -fPIC -D_DEBUG -I"/home/greg/Project/VideoGame/urchinEngine/common/src" -I"/home/greg/Project/VideoGame/urchinEngine/physicsEngine/src" -I"/home/greg/Project/VideoGame/urchinEngine/soundEngine/src" -I"/home/greg/Project/VideoGame/urchinEngine/3dEngine/src" -I"/home/greg/Project/VideoGame/urchinEngine/mapHandler/src" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


