################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/math/geometry/3d/object/AABBox.cpp \
../src/math/geometry/3d/object/Capsule.cpp \
../src/math/geometry/3d/object/ConvexHull3D.cpp \
../src/math/geometry/3d/object/ConvexObject3D.cpp \
../src/math/geometry/3d/object/Cylinder.cpp \
../src/math/geometry/3d/object/Frustum.cpp \
../src/math/geometry/3d/object/LineSegment3D.cpp \
../src/math/geometry/3d/object/OBBox.cpp \
../src/math/geometry/3d/object/Sphere.cpp \
../src/math/geometry/3d/object/Tetrahedron.cpp \
../src/math/geometry/3d/object/Triangle3D.cpp 

OBJS += \
./src/math/geometry/3d/object/AABBox.o \
./src/math/geometry/3d/object/Capsule.o \
./src/math/geometry/3d/object/ConvexHull3D.o \
./src/math/geometry/3d/object/ConvexObject3D.o \
./src/math/geometry/3d/object/Cylinder.o \
./src/math/geometry/3d/object/Frustum.o \
./src/math/geometry/3d/object/LineSegment3D.o \
./src/math/geometry/3d/object/OBBox.o \
./src/math/geometry/3d/object/Sphere.o \
./src/math/geometry/3d/object/Tetrahedron.o \
./src/math/geometry/3d/object/Triangle3D.o 

CPP_DEPS += \
./src/math/geometry/3d/object/AABBox.d \
./src/math/geometry/3d/object/Capsule.d \
./src/math/geometry/3d/object/ConvexHull3D.d \
./src/math/geometry/3d/object/ConvexObject3D.d \
./src/math/geometry/3d/object/Cylinder.d \
./src/math/geometry/3d/object/Frustum.d \
./src/math/geometry/3d/object/LineSegment3D.d \
./src/math/geometry/3d/object/OBBox.d \
./src/math/geometry/3d/object/Sphere.d \
./src/math/geometry/3d/object/Tetrahedron.d \
./src/math/geometry/3d/object/Triangle3D.d 


# Each subdirectory must supply rules for building sources it contributes
src/math/geometry/3d/object/%.o: ../src/math/geometry/3d/object/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++11 -fPIC -D_DEBUG -I"/home/greg/Project/VideoGame/urchinEngine/common/src" -O0 -g3 -Wall -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


