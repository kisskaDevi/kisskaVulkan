glslc.exe base/base.vert                                        -o ../spv/base/basevert.spv
glslc.exe base/base.frag                                        -o ../spv/base/basefrag.spv
glslc.exe spotLightingPass/spotLighting.vert                    -o ../spv/spotLightingPass/spotLightingVert.spv
glslc.exe spotLightingPass/spotLighting.frag                    -o ../spv/spotLightingPass/spotLightingFrag.spv
glslc.exe ambientLightingPass/ambientLighting.vert              -o ../spv/ambientLightingPass/ambientLightingVert.spv
glslc.exe ambientLightingPass/ambientLighting.frag              -o ../spv/ambientLightingPass/ambientLightingFrag.spv
glslc.exe shadow/shadowMapShader.vert                           -o ../spv/shadow/shad.spv
glslc.exe customFilter/customFilter.vert                        -o ../spv/customFilter/customFilterVert.spv
glslc.exe customFilter/customFilter.frag                        -o ../spv/customFilter/customFilterFrag.spv
glslc.exe postProcessing/postProcessingShader.vert              -o ../spv/postProcessing/postProcessingVert.spv
glslc.exe postProcessing/postProcessingShader.frag              -o ../spv/postProcessing/postProcessingFrag.spv
glslc.exe skybox/skybox.vert                                    -o ../spv/skybox/skyboxVert.spv
glslc.exe skybox/skybox.frag                                    -o ../spv/skybox/skyboxFrag.spv
glslc.exe outlining/outlining.vert                              -o ../spv/outlining/outliningVert.spv
glslc.exe outlining/outlining.frag                              -o ../spv/outlining/outliningFrag.spv
glslc.exe sslr/SSLR.vert                                        -o ../spv/sslr/sslrVert.spv
glslc.exe sslr/SSLR.frag                                        -o ../spv/sslr/sslrFrag.spv
glslc.exe ssao/SSAO.vert                                        -o ../spv/ssao/ssaoVert.spv
glslc.exe ssao/SSAO.frag                                        -o ../spv/ssao/ssaoFrag.spv
glslc.exe layersCombiner/layersCombiner.vert                    -o ../spv/layersCombiner/layersCombinerVert.spv
glslc.exe layersCombiner/layersCombiner.frag                    -o ../spv/layersCombiner/layersCombinerFrag.spv
glslc.exe gaussianBlur/xBlur.vert                               -o ../spv/gaussianBlur/xBlurVert.spv
glslc.exe gaussianBlur/xBlur.frag                               -o ../spv/gaussianBlur/xBlurFrag.spv
glslc.exe gaussianBlur/yBlur.vert                               -o ../spv/gaussianBlur/yBlurVert.spv
glslc.exe gaussianBlur/yBlur.frag                               -o ../spv/gaussianBlur/yBlurFrag.spv

pause
