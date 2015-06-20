################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/resources/sound/soundshape/SoundBoxReaderWriter.cpp \
../src/resources/sound/soundshape/SoundShapeReaderWriter.cpp \
../src/resources/sound/soundshape/SoundShapeReaderWriterRetriever.cpp \
../src/resources/sound/soundshape/SoundSphereReaderWriter.cpp 

OBJS += \
./src/resources/sound/soundshape/SoundBoxReaderWriter.o \
./src/resources/sound/soundshape/SoundShapeReaderWriter.o \
./src/resources/sound/soundshape/SoundShapeReaderWriterRetriever.o \
./src/resources/sound/soundshape/SoundSphereReaderWriter.o 

CPP_DEPS += \
./src/resources/sound/soundshape/SoundBoxReaderWriter.d \
./src/resources/sound/soundshape/SoundShapeReaderWriter.d \
./src/resources/sound/soundshape/SoundShapeReaderWriterRetriever.d \
./src/resources/sound/soundshape/SoundSphereReaderWriter.d 


# Each subdirectory must supply rules for building sources it contributes
src/resources/sound/soundshape/%.o: ../src/resources/sound/soundshape/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++11 -fPIC -D_DEBUG -I"/home/greg/Project/VideoGame/urchinEngine/common/src" -I"/home/greg/Project/VideoGame/urchinEngine/physicsEngine/src" -I"/home/greg/Project/VideoGame/urchinEngine/soundEngine/src" -I"/home/greg/Project/VideoGame/urchinEngine/3dEngine/src" -I"/home/greg/Project/VideoGame/urchinEngine/mapHandler/src" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


