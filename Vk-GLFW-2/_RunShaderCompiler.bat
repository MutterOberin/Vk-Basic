del vk_simple_vert.spv
del vk_simple_frag.spv

C:\\6_Projects\\Projects_Vulkan\\Vk-SDK-1.1.108\\Bin\\glslangValidator.exe -V110 vk_simple.vert
C:\\6_Projects\\Projects_Vulkan\\Vk-SDK-1.1.108\\Bin\\glslangValidator.exe -V110 vk_simple.frag

ren vert.spv vk_simple_vert.spv
ren frag.spv vk_simple_frag.spv

pause