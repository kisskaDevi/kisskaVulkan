TEMPLATE = app
CONFIG += c++17 console
WARNINGS += -Wall

Release:DESTDIR = release
Debug:DESTDIR = debug

Release:GLFW_BIN_DIR = Release
Debug:GLFW_BIN_DIR = Debug

DEFINES += TESTPOS

LIBS += \
    -L$$OUT_PWD/../../core/graphicsManager/$$DESTDIR \
    -L$$OUT_PWD/../../core/deferredGraphics/$$DESTDIR \
    -L$$OUT_PWD/../../core/imguiGraphics/$$DESTDIR \
    -L$$OUT_PWD/../../core/models/$$DESTDIR \
    -L$$OUT_PWD/../../core/transformational/$$DESTDIR \
    -L$$OUT_PWD/../../core/interfaces/$$DESTDIR \
    -L$$OUT_PWD/../../core/utils/$$DESTDIR \
    -L$$OUT_PWD/../../core/math/$$DESTDIR \
    -L$$OUT_PWD/../../core/deferredGraphics/workflows/$$DESTDIR \
    -L$$PWD/../../dependences/libs/vulkan_tools/x64 \
    -L$$PWD/../../dependences/libs/glfw/build/src/$$GLFW_BIN_DIR \
    -lgraphicsManager \
    -limguiGraphics\
    -ldeferredGraphics \
    -lworkflows \
    -lmodels \
    -ltransformational \
    -linterfaces \
    -lutils \
    -lmath
win32: LIBS += -lvulkan-1 -lglfw3dll
unix: LIBS += -lvulkan -lglfw

INCLUDEPATH += \
    $$PWD/../../dependences\libs \
    $$PWD/../../dependences/libs/vulkan/include/vulkan \
    $$PWD/../../dependences/libs/glfw/include/GLFW \
    $$PWD/../../dependences/libs/glfw/include \
    $$PWD/../../dependences/libs/stb \
    $$PWD/../../dependences/libs/tinygltf \
    $$PWD/../../dependences/libs/tinyply/source \
    $$PWD/../../dependences/libs/imgui \
    $$PWD/../../dependences/libs/imgui/backends \
    $$PWD/../../core/graphicsManager \
    $$PWD/../../core/deferredGraphics \
    $$PWD/../../core/deferredGraphics/renderStages \
    $$PWD/../../core/deferredGraphics/workflows \
    $$PWD/../../core/imguiGraphics \
    $$PWD/../../core/utils \
    $$PWD/../../core/transformational \
    $$PWD/../../core/interfaces \
    $$PWD/../../core/models \
    $$PWD/../../core/math \
    $$PWD/../common \
    $$PWD

SOURCES += \
    ../common/controller.cpp \
    ../common/main.cpp \
    testPos.cpp

HEADERS += \
    ../common/controller.h \
    ../common/scene.h \
    testPos.h

DISTFILES += \
    $$PWD/CMakelists.txt
