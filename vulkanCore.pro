CONFIG += c++17 console

win32: LIBS += -L$$PWD/libs/Lib/vulkan/x64/ -lvulkan-1

INCLUDEPATH += $$PWD/libs/Lib/vulkan/x64
DEPENDPATH += $$PWD/libs/Lib/vulkan/x64

win32: LIBS += -L$$PWD/libs/glfw-3.3.4.bin.WIN64/lib-static-ucrt/ -lglfw3dll

INCLUDEPATH += $$PWD/libs/glfw-3.3.4.bin.WIN64/lib-static-ucrt
DEPENDPATH += $$PWD/libs/glfw-3.3.4.bin.WIN64/lib-static-ucrt

DISTFILES += \
    core/graphics/shaders/SpotLightingPass/SpotLighting.frag \
    core/graphics/shaders/SpotLightingPass/SpotLighting.vert \
    core/graphics/shaders/SpotLightingPass/SpotLightingAmbient.frag \
    core/graphics/shaders/SpotLightingPass/SpotLightingAmbient.vert \
    core/graphics/shaders/SpotLightingPass/SpotLightingScattering.frag \
    core/graphics/shaders/bloom/bloom.frag \
    core/graphics/shaders/bloom/bloom.vert \
    core/graphics/shaders/compile.bat \
    core/graphics/shaders/base/base.frag \
    core/graphics/shaders/base/base.vert \
    core/graphics/shaders/customFilter/customFilter.frag \
    core/graphics/shaders/customFilter/customFilter.vert \
    core/graphics/shaders/oneColor/oneColor.frag \
    core/graphics/shaders/oneColor/oneColor.vert \
    core/graphics/shaders/postProcessing/firstPostProcessingShader.frag \
    core/graphics/shaders/postProcessing/firstPostProcessingShader.vert \
    core/graphics/shaders/shadow/shadowMapShader.vert \
    core/graphics/shaders/postProcessing/postProcessingShader.frag \
    core/graphics/shaders/postProcessing/postProcessingShader.vert \
    core/graphics/shaders/skybox/skybox.frag \
    core/graphics/shaders/skybox/skybox.vert \
    core/graphics/shaders/ssao/SSAO.frag \
    core/graphics/shaders/ssao/SSAO.vert \
    core/graphics/shaders/sslr/SSLR.frag \
    core/graphics/shaders/sslr/SSLR.vert \
    core/graphics/shaders/stencil/firstStencil.frag \
    core/graphics/shaders/stencil/firstStencil.vert \
    core/graphics/shaders/stencil/secondStencil.frag \
    core/graphics/shaders/stencil/secondStencil.vert \
    model/glTF/Sponza/Sponza.gltf \
    model/glb/Bee.glb \
    model/glb/Box.glb \
    model/glb/Duck.glb \
    model/glb/RetroUFO.glb \
    model/glb/sponza.glb \
    texture/0.png \
    texture/1.png \
    texture/icon.ico \
    texture/skybox/back.jpg \
    texture/skybox/bottom.jpg \
    texture/skybox/front.jpg \
    texture/skybox/left.jpg \
    texture/skybox/right.jpg \
    texture/skybox/top.jpg

SOURCES += \
    core/graphics/attachments.cpp \
    core/graphics/customfilter.cpp \
    core/graphics/source/SpotLighting.cpp \
    core/graphics/source/base.cpp \
    core/graphics/source/extension.cpp \
    core/graphics/source/skybox.cpp \
    core/graphics/graphics.cpp \
    core/graphics/postProcessing.cpp \
    core/graphics/shadowGraphics.cpp \
    core/graphics/ssao.cpp \
    core/graphics/sslr.cpp \
    core/transformational/camera.cpp \
    core/transformational/group.cpp \
    core/transformational/light.cpp \
    core/transformational/object.cpp \
    core/transformational/gltfmodel.cpp \
    core/operations.cpp \
    core/texture.cpp \
    core/vulkanCore.cpp\
    physicalobject.cpp \
    main.cpp \
    scene.cpp \
    scene2.cpp

HEADERS += \
    core/graphics/attachments.h \
    core/graphics/customfilter.h \
    core/graphics/graphics.h \
    core/graphics/postProcessing.h \
    core/graphics/shadowGraphics.h \
    core/graphics/ssao.h \
    core/graphics/sslr.h \
    core/transformational/transformational.h \
    core/transformational/camera.h \
    core/transformational/group.h \
    core/transformational/light.h \
    core/transformational/object.h \
    core/transformational/gltfmodel.h \
    libs/dualQuaternion.h \
    libs/quaternion.h \
    physicalobject.h \
    core/operations.h \
    core/texture.h \
    core/vulkanCore.h \
    scene.h \
    scene2.h

win32:RC_ICONS += texture/icon.ico
