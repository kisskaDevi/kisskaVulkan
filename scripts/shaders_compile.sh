full_path=$(realpath $0)
dir_path=$(dirname $full_path)

python3 $dir_path/shaders_compile.py -d $dir_path/../core/deferredGraphics/shaders -o $dir_path/../core/deferredGraphics/spv -c $dir_path/../dependences/libs/vulkan_tools/glslc
