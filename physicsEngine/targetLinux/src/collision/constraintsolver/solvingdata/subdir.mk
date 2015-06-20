################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/collision/constraintsolver/solvingdata/AccumulatedSolvingData.cpp \
../src/collision/constraintsolver/solvingdata/CommonSolvingData.cpp \
../src/collision/constraintsolver/solvingdata/ImpulseSolvingData.cpp 

OBJS += \
./src/collision/constraintsolver/solvingdata/AccumulatedSolvingData.o \
./src/collision/constraintsolver/solvingdata/CommonSolvingData.o \
./src/collision/constraintsolver/solvingdata/ImpulseSolvingData.o 

CPP_DEPS += \
./src/collision/constraintsolver/solvingdata/AccumulatedSolvingData.d \
./src/collision/constraintsolver/solvingdata/CommonSolvingData.d \
./src/collision/constraintsolver/solvingdata/ImpulseSolvingData.d 


# Each subdirectory must supply rules for building sources it contributes
src/collision/constraintsolver/solvingdata/%.o: ../src/collision/constraintsolver/solvingdata/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++11 -fPIC -D_DEBUG -I"/home/greg/Project/VideoGame/urchinEngine/common/src" -I"/home/greg/Project/VideoGame/urchinEngine/physicsEngine/src" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


