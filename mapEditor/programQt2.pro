TEMPLATE = app

TARGET = urchinMapEditor

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
HEADERS += src/support/*.h
HEADERS += src/dialog/*.h
HEADERS += src/scene/controller/*.h
HEADERS += src/scene/controller/objects/*.h
HEADERS += src/scene/controller/objects/dialog/*.h
HEADERS += src/scene/controller/objects/bodyshape/*.h
HEADERS += src/scene/controller/objects/bodyshape/support/*.h
HEADERS += src/scene/controller/lights/*.h
HEADERS += src/scene/controller/lights/dialog/*.h
HEADERS += src/scene/controller/sounds/*.h
HEADERS += src/scene/controller/sounds/dialog/*.h
HEADERS += src/scene/controller/sounds/soundshape/*.h
HEADERS += src/scene/controller/sounds/soundshape/support/*.h
HEADERS += src/scene/displayer/*.h
HEADERS += src/scene/displayer/objects/*.h
HEADERS += src/scene/displayer/lights/*.h
HEADERS += src/scene/displayer/sounds/*.h
    
SOURCES += src/*.cpp
SOURCES += src/support/*.cpp
SOURCES += src/dialog/*.cpp
SOURCES += src/scene/controller/*.cpp
SOURCES += src/scene/controller/objects/*.cpp
SOURCES += src/scene/controller/objects/dialog/*.cpp
SOURCES += src/scene/controller/objects/bodyshape/*.cpp
SOURCES += src/scene/controller/objects/bodyshape/support/*.cpp
SOURCES += src/scene/controller/lights/*.cpp
SOURCES += src/scene/controller/lights/dialog/*.cpp
SOURCES += src/scene/controller/sounds/*.cpp
SOURCES += src/scene/controller/sounds/dialog/*.cpp
SOURCES += src/scene/controller/sounds/soundshape/*.cpp
SOURCES += src/scene/controller/sounds/soundshape/support/*.cpp
SOURCES += src/scene/displayer/*.cpp
SOURCES += src/scene/displayer/objects/*.cpp
SOURCES += src/scene/displayer/lights/*.cpp
SOURCES += src/scene/displayer/sounds/*.cpp
    
FORMS += 

RESOURCES += 

QMAKE_CXXFLAGS += -std=c++14

INCLUDEPATH += ../mapEditor/src \
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