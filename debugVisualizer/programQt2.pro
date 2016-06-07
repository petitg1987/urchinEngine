TEMPLATE = app

TARGET = urchinDebugVisualizer

shaderCopy.target = shaderCopy
shaderCopy.commands = rsync -r ../3dEngine/resources/shaders/* ./resources/shaders/
QMAKE_EXTRA_TARGETS += shaderCopy

QT += core \
	opengl \
	gui
	
CONFIG += debug

CONFIG(debug, debug){
	DEFINES += _DEBUG
}

HEADERS += src/*.h
HEADERS += src/scene/displayer/*.h

SOURCES += src/*.cpp
SOURCES += src/scene/displayer/*.cpp
    
FORMS += 

RESOURCES += 

QMAKE_CXXFLAGS += -std=c++14

INCLUDEPATH += ../debugVisualizer/src \
	../common/src \
	../3dEngine/src \
	../physicsEngine/src \
	../AIEngine/src \
	../soundEngine/src \
	../mapHandler/src \

unix{
	LIBS += -L../common/targetLinux/ -lurchinCommon \
	    -L../3dEngine/targetLinux/ -lurchin3dEngine \
	    -L../AIEngine/targetLinux/ -lurchinAIEngine \
	    -L../physicsEngine/targetLinux/ -lurchinPhysicsEngine \
	    -L../soundEngine/targetLinux/ -lurchinSoundEngine \
	    -L../mapHandler/targetLinux/ -lurchinMapHandler \
	    
	OBJECTS_DIR = targetLinux
	MOC_DIR = targetLinux
	RCC_DIR = targetLinux
	UI_DIR = targetLinux
}

win32{
	LIBS += -L../common/targetWin/ -lliburchinCommon \
	    -L../3dEngine/targetWin/ -lliburchin3dEngine \
	    -L../AIEngine/targetWin/ -lliburchinAIEngine \
	    -L../physicsEngine/targetWin/ -lliburchinPhysicsEngine \
	    -L../soundEngine/targetWin/ -lliburchinSoundEngine \
	    -L../mapHandler/targetWin/ -lliburchinMapHandler \
	
	OBJECTS_DIR = targetWin
	MOC_DIR = targetWin
	RCC_DIR = targetWin
	UI_DIR = targetWin
}