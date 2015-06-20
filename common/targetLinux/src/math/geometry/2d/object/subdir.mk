################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/math/geometry/2d/object/ConvexHull2D.cpp \
../src/math/geometry/2d/object/ConvexObject2D.cpp \
../src/math/geometry/2d/object/LineSegment2D.cpp \
../src/math/geometry/2d/object/Rectangle.cpp 

OBJS += \
./src/math/geometry/2d/object/ConvexHull2D.o \
./src/math/geometry/2d/object/ConvexObject2D.o \
./src/math/geometry/2d/object/LineSegment2D.o \
./src/math/geometry/2d/object/Rectangle.o 

CPP_DEPS += \
./src/math/geometry/2d/object/ConvexHull2D.d \
./src/math/geometry/2d/object/ConvexObject2D.d \
./src/math/geometry/2d/object/LineSegment2D.d \
./src/math/geometry/2d/object/Rectangle.d 


# Each subdirectory must supply rules for building sources it contributes
src/math/geometry/2d/object/%.o: ../src/math/geometry/2d/object/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++11 -fPIC -D_DEBUG -I"/home/greg/Project/VideoGame/urchinEngine/common/src" -O0 -g3 -Wall -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


