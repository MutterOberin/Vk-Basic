#define GLFW_INCLUDE_VULKAN
#include <GLFW\glfw3.h>     // Includes Vulkan
// #include "vulkan\vulkan.h"

#include <vector>
#include <string>
#include <iostream>
#include <fstream>


#define ASSERT_VK(errorcode)\
    if (errorcode != VK_SUCCESS)\
    {\
        __debugbreak();\
    }

VkDevice   device;
VkInstance instance;

VkQueue    queue;

VkSurfaceKHR   surface;
VkSwapchainKHR swapchain;

uint32_t amountOfSwapchainImages;

VkImageView*   imageViews;
VkFramebuffer* framebuffers;

VkCommandPool commandPool;
VkCommandBuffer* commandBuffers;

GLFWwindow* window;

VkPipeline       pipeline;
VkPipelineLayout pipelineLayout;

VkRenderPass renderPass;

VkSemaphore semaphore_1;
VkSemaphore semaphore_2;

VkShaderModule shaderModuleVert;
VkShaderModule shaderModuleFrag;

const uint32_t SIZEW = 800;
const uint32_t SIZEH = 600;

//const VkFormat FORMAT = VK_FORMAT_B8G8R8A8_UNORM;

std::vector<VkQueueFamilyProperties> GetPhysicalDeviceQueueFamilyProperties(VkPhysicalDevice device)
{
    uint32_t amountOfQueueFamilies = 0;
    
    vkGetPhysicalDeviceQueueFamilyProperties(device, &amountOfQueueFamilies, NULL);

    std::vector<VkQueueFamilyProperties> props;
    props.resize(amountOfQueueFamilies);

    vkGetPhysicalDeviceQueueFamilyProperties(device, &amountOfQueueFamilies, props.data());

    std::cout << std::endl;
    std::cout << "Queue Families Count:  " << props.size() << std::endl;

    for (int i = 0; i < props.size(); i++)
    {
        std::cout << std::endl;
        std::cout << "Queue Family #" << i << std::endl;
        std::cout << "--------------------------------------------------------------------------------" << std::endl;
        std::cout << "VK_QUEUE_GRAPHICS_BIT          " << ((props.at(i).queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) << std::endl;
        std::cout << "VK_QUEUE_COMPUTE_BIT           " << ((props.at(i).queueFlags & VK_QUEUE_COMPUTE_BIT) != 0) << std::endl;
        std::cout << "VK_QUEUE_TRANSFER_BIT          " << ((props.at(i).queueFlags & VK_QUEUE_TRANSFER_BIT) != 0) << std::endl;
        std::cout << "VK_QUEUE_SPARSE_BINDING_BIT    " << ((props.at(i).queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) != 0) << std::endl;
        std::cout << "VK_QUEUE_PROTECTED_BIT         " << ((props.at(i).queueFlags & VK_QUEUE_PROTECTED_BIT) != 0) << std::endl;
        std::cout << std::endl;
        std::cout << "Queue Count:                   " << props.at(i).queueCount << std::endl;
        std::cout << "Timestamp Valid Bits           " << props.at(i).timestampValidBits << std::endl;

        uint32_t w = props.at(i).minImageTransferGranularity.width;
        uint32_t h = props.at(i).minImageTransferGranularity.height;
        uint32_t d = props.at(i).minImageTransferGranularity.depth;

        std::cout << "Min Image Transfer Granularity " << w << " x " << h << " x " << d << std::endl;
    }

    return props;
}

std::vector<VkLayerProperties> GetSupportedLayers()
{
    uint32_t amountOfLayers = 0;

    vkEnumerateInstanceLayerProperties(&amountOfLayers, nullptr);

    std::vector<VkLayerProperties> layers;
    layers.resize(amountOfLayers);

    vkEnumerateInstanceLayerProperties(&amountOfLayers, layers.data());

    for (int i = 0; i < layers.size(); i++)
    {
        std::cout << "Instance Layer: " << layers.at(i).layerName << std::endl;
        std::cout << layers.at(i).description << std::endl;
    }

    return layers;
}

std::vector<VkExtensionProperties> GetSupportedExtensions()
{
    uint32_t amountOfExtensions = 0;

    vkEnumerateInstanceExtensionProperties(nullptr, &amountOfExtensions, nullptr);

    std::vector<VkExtensionProperties> extensions;
    extensions.resize(amountOfExtensions);

    vkEnumerateInstanceExtensionProperties(nullptr, &amountOfExtensions, extensions.data());

    for (int i = 0; i < extensions.size(); i++)
    {
        std::cout << "Instance Extension: " << extensions.at(i).extensionName << std::endl;
    }

    return extensions;
}

std::vector<VkLayerProperties> GetSupportedDeviceLayers(VkPhysicalDevice device)
{
    uint32_t amountOfLayers = 0;

    vkEnumerateDeviceLayerProperties(device, &amountOfLayers, nullptr);

    std::vector<VkLayerProperties> layers;
    layers.resize(amountOfLayers);

    vkEnumerateDeviceLayerProperties(device, &amountOfLayers, layers.data());

    for (int i = 0; i < layers.size(); i++)
    {
        std::cout << "Device Layer  : " << layers.at(i).layerName << std::endl;
        std::cout << layers.at(i).description << std::endl;
    }

    return layers;
}

std::vector<VkExtensionProperties> GetSupportedDeviceExtensions(VkPhysicalDevice device)
{
    uint32_t amountOfExtensions = 0;

    vkEnumerateDeviceExtensionProperties(device, nullptr, &amountOfExtensions, nullptr);

    std::vector<VkExtensionProperties> extensions;
    extensions.resize(amountOfExtensions);

    vkEnumerateDeviceExtensionProperties(device, nullptr, &amountOfExtensions, extensions.data());

    for (int i = 0; i < extensions.size(); i++)
    {
        std::cout << "Device Extension  : " << extensions.at(i).extensionName << std::endl;
    }

    return extensions;
}

std::vector<char> ReadFile(const std::string &filename)
{
    std::ifstream file(filename, std::ios::binary | std::ios::ate);

    if (file)
    {
        size_t fileSize = (size_t)file.tellg();

        std::vector<char> fileBuffer(fileSize);

        file.seekg(0);

        file.read(fileBuffer.data(), fileSize);

        file.close();

        return fileBuffer;
    }
    else
    {
        std::cerr << "## Unable to Read File " + filename;

        //throw std::runtime_error("Failed to Read File");
    }
}

void CreateShaderModule(const std::vector<char> &code, VkShaderModule* shaderModule)
{
    VkShaderModuleCreateInfo shaderCreateInfo;

    shaderCreateInfo.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderCreateInfo.pNext    = nullptr;
    shaderCreateInfo.flags    = 0;
    shaderCreateInfo.codeSize = code.size();
    shaderCreateInfo.pCode    = (uint32_t*)code.data();

    ASSERT_VK(vkCreateShaderModule(device, &shaderCreateInfo, nullptr, shaderModule));
}

void StartGLFW()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(SIZEW, SIZEH, "Vulkan GLFW - 1", nullptr, nullptr);
}

void StartVulkan()
{
    // Application Info

    VkApplicationInfo appInfo;

    appInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext              = nullptr;
    appInfo.pApplicationName   = "Vulkan GLFW-1";
    appInfo.pEngineName        = "Vulkan GLFW-1";
    appInfo.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
    appInfo.engineVersion      = VK_MAKE_VERSION(0, 1, 0);
    appInfo.apiVersion         = VK_API_VERSION_1_1; // Only Options: VK_API_VERSION_1_0 and VK_API_VERSION_1_1;


    //GetSupportedLayers();
    //GetSupportedExtensions();

    // Layers and Extensions

    const std::vector<const char*> enabledInstanceLayers = { "VK_LAYER_LUNARG_standard_validation" };
    
    uint32_t amountOfGLFWExtensions = 0;
    auto enabledInstanceExtensions = glfwGetRequiredInstanceExtensions(&amountOfGLFWExtensions);

    // Instance Instance Create Info

    VkInstanceCreateInfo instanceInfo;

    instanceInfo.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.pNext                   = nullptr;
    instanceInfo.flags                   = 0;
    instanceInfo.pApplicationInfo        = &appInfo;
    instanceInfo.enabledLayerCount       = enabledInstanceLayers.size();
    instanceInfo.ppEnabledLayerNames     = enabledInstanceLayers.data();
    instanceInfo.enabledExtensionCount   = amountOfGLFWExtensions;
    instanceInfo.ppEnabledExtensionNames = enabledInstanceExtensions;

    // Instance and Surface

    ASSERT_VK(vkCreateInstance(&instanceInfo, nullptr, &instance));

    ASSERT_VK(glfwCreateWindowSurface(instance, window, nullptr, &surface));

    // Physical Devices

    uint32_t amountOfPhysicalDevices = 0;

    ASSERT_VK(vkEnumeratePhysicalDevices(instance, &amountOfPhysicalDevices, nullptr));

    VkPhysicalDevice* physicalDevices = new VkPhysicalDevice[amountOfPhysicalDevices];

    ASSERT_VK(vkEnumeratePhysicalDevices(instance, &amountOfPhysicalDevices, physicalDevices));

    VkPhysicalDeviceProperties props_device;

    //GetSupportedDeviceLayers(physicalDevices[0])
    //GetSupportedDeviceExtensions(physicalDevices[0]);

    //GetPhysicalDeviceQueueFamilyProperties(physicalDevices[0]);

    vkGetPhysicalDeviceProperties(physicalDevices[0], &props_device);
    
    glfwSetWindowTitle(window, &props_device.deviceName[0]);

    // Physical Device Surface Capabilities

    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevices[0], surface, &surfaceCapabilities);

    //std::cout << "minImageCount           :" << surfaceCapabilities.minImageCount << std::endl;
    //std::cout << "maxImageCount           :" << surfaceCapabilities.maxImageCount << std::endl;
    //std::cout << "currentExtent           :" << surfaceCapabilities.currentExtent.width  << " x " << surfaceCapabilities.currentExtent.height  << std::endl;
    //std::cout << "minImageExtent          :" << surfaceCapabilities.minImageExtent.width << " x " << surfaceCapabilities.minImageExtent.height << std::endl;
    //std::cout << "maxImageExtent          :" << surfaceCapabilities.maxImageExtent.width << " x " << surfaceCapabilities.maxImageExtent.height << std::endl;
    //std::cout << "maxImageArrayLayers     :" << surfaceCapabilities.maxImageArrayLayers << std::endl;
    //std::cout << "supportedTransforms     :" << surfaceCapabilities.supportedTransforms << std::endl;
    //std::cout << "currentTransform        :" << surfaceCapabilities.currentTransform << std::endl;
    //std::cout << "supportedCompositeAlpha :" << surfaceCapabilities.supportedCompositeAlpha << std::endl;
    //std::cout << "supportedUsageFlags     :" << surfaceCapabilities.supportedUsageFlags << std::endl;

    uint32_t amountOfFormats = 0;

    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevices[0], surface, &amountOfFormats, nullptr);

    std::vector<VkSurfaceFormatKHR> surfaceFormats;
    surfaceFormats.resize(amountOfFormats);

    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevices[0], surface, &amountOfFormats, surfaceFormats.data());

    // VK_FORMAT_B8G8R8A8_UNORM = 44,
    // VK_FORMAT_B8G8R8A8_SRGB  = 50,

    //for (int i = 0; i < amountOfFormats; i++)
    //{
    //    std::cout << "Format: " << surfaceFormats.at(i).format << "Colorspace: " << surfaceFormats.at(i).colorSpace << std::endl;
    //}

    uint32_t amountOfPresentationModes = 0;

    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevices[0], surface, &amountOfPresentationModes, nullptr);

    std::vector<VkPresentModeKHR> surfacePresentationModes;
    surfacePresentationModes.resize(amountOfPresentationModes);

    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevices[0], surface, &amountOfPresentationModes, surfacePresentationModes.data());

    // VK_PRESENT_MODE_MAILBOX_KHR      = 1,
    // VK_PRESENT_MODE_FIFO_KHR         = 2,
    // VK_PRESENT_MODE_FIFO_RELAXED_KHR = 3,

    //for (int i = 0; i < amountOfPresentationModes; i++)
    //{
    //    std::cout << "PresentMode: " << surfacePresentationModes.at(i) << std::endl;
    //}

    // VkQueueFamilyProperties 

    uint32_t amountOfQueueFamilies = 0;

    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevices[0], &amountOfQueueFamilies, nullptr);

    std::vector<VkQueueFamilyProperties> props_queue;
    props_queue.resize(amountOfQueueFamilies);

    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevices[0], &amountOfQueueFamilies, props_queue.data());


    // Device Queue Create Info

    float prio[] = { 1.0f, 1.0f, 1.0f, 1.0f };

    VkDeviceQueueCreateInfo deviceQueueCreateInfo;

    deviceQueueCreateInfo.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    deviceQueueCreateInfo.pNext            = nullptr;
    deviceQueueCreateInfo.flags            = 0;
    deviceQueueCreateInfo.queueFamilyIndex = 0;         // Actually check which queue is the best to use
    deviceQueueCreateInfo.queueCount       = 4;
    deviceQueueCreateInfo.pQueuePriorities = prio;


    // Device Features to use
    VkPhysicalDeviceFeatures enabledDeviceFeatures = {};

    // Device Level Extensions
    const std::vector<const char*> enabledDeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_NV_RAY_TRACING_EXTENSION_NAME };

    // Device Create Info

    VkDeviceCreateInfo deviceCreateInfo;

    deviceCreateInfo.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.pNext                   = nullptr;
    deviceCreateInfo.flags                   = 0;
    deviceCreateInfo.queueCreateInfoCount    = 1;
    deviceCreateInfo.pQueueCreateInfos       = &deviceQueueCreateInfo;
    deviceCreateInfo.enabledLayerCount       = 0;
    deviceCreateInfo.ppEnabledLayerNames     = nullptr;
    deviceCreateInfo.enabledExtensionCount   = enabledDeviceExtensions.size();
    deviceCreateInfo.ppEnabledExtensionNames = enabledDeviceExtensions.data();
    deviceCreateInfo.pEnabledFeatures        = &enabledDeviceFeatures;

    // Actually decide what device to use in a smarter way
    ASSERT_VK(vkCreateDevice(physicalDevices[0], &deviceCreateInfo, nullptr, &device));

    // A Queue to do things
    vkGetDeviceQueue(device, 0, 0, &queue);

    VkBool32 support = VK_FALSE;
    ASSERT_VK(vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevices[0], 0, surface, &support));

    if (!support)
        std::cerr << "Surface not supported" << std::endl;

    // Swapchain

    VkSwapchainCreateInfoKHR swapchainCreateInfo;
    swapchainCreateInfo.sType                 = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainCreateInfo.pNext                 = nullptr;
    swapchainCreateInfo.flags                 = 0;
    swapchainCreateInfo.surface               = surface;
    swapchainCreateInfo.minImageCount         = 3;                                  // CIV    
    swapchainCreateInfo.imageFormat           = VK_FORMAT_B8G8R8A8_UNORM;           // CIV
    swapchainCreateInfo.imageColorSpace       = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;  // CIV
    swapchainCreateInfo.imageExtent           = VkExtent2D{ SIZEW, SIZEH };
    swapchainCreateInfo.imageArrayLayers      = 1;
    swapchainCreateInfo.imageUsage            = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchainCreateInfo.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
    swapchainCreateInfo.queueFamilyIndexCount = 0;
    swapchainCreateInfo.pQueueFamilyIndices   = nullptr;
    swapchainCreateInfo.preTransform          = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    swapchainCreateInfo.compositeAlpha        = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainCreateInfo.presentMode           = VK_PRESENT_MODE_FIFO_KHR;
    swapchainCreateInfo.clipped               = VK_TRUE;
    swapchainCreateInfo.oldSwapchain          = VK_NULL_HANDLE;

    ASSERT_VK(vkCreateSwapchainKHR(device, &swapchainCreateInfo, nullptr, &swapchain));

    // Swapchain Images

    amountOfSwapchainImages = 0;

    ASSERT_VK(vkGetSwapchainImagesKHR(device, swapchain, &amountOfSwapchainImages, nullptr));

    std::vector<VkImage> swapchainImages;
    swapchainImages.resize(amountOfSwapchainImages);

    ASSERT_VK(vkGetSwapchainImagesKHR(device, swapchain, &amountOfSwapchainImages, swapchainImages.data()));

    // Image View Create Info

    imageViews = new VkImageView[amountOfSwapchainImages];

    for (int i = 0; i < amountOfSwapchainImages; i++)
    {
        VkImageViewCreateInfo imageViewCreateInfo;
        imageViewCreateInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewCreateInfo.pNext                           = nullptr;
        imageViewCreateInfo.flags                           = 0;
        imageViewCreateInfo.image                           = swapchainImages[i];           // CIV, Image to Create View on
        imageViewCreateInfo.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
        imageViewCreateInfo.format                          = VK_FORMAT_B8G8R8A8_UNORM;     // CIV
        imageViewCreateInfo.components.r                    = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.g                    = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.b                    = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.a                    = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        imageViewCreateInfo.subresourceRange.baseMipLevel   = 0;
        imageViewCreateInfo.subresourceRange.levelCount     = 1;
        imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        imageViewCreateInfo.subresourceRange.layerCount     = 1;

        ASSERT_VK(vkCreateImageView(device, &imageViewCreateInfo, nullptr, &imageViews[i]));
    }

    // Read Shader Files

    auto shaderCodeVert = ReadFile("vk_simple_vert.spv");
    auto shaderCodeFrag = ReadFile("vk_simple_frag.spv");

    // Create Modules (see Above)

    CreateShaderModule(shaderCodeVert, &shaderModuleVert);
    CreateShaderModule(shaderCodeFrag, &shaderModuleFrag);

    // Create ShaderStageCreateInfo

    VkPipelineShaderStageCreateInfo shaderStageCreateInfoVert;

    shaderStageCreateInfoVert.sType               = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStageCreateInfoVert.pNext               = nullptr;
    shaderStageCreateInfoVert.flags               = 0;
    shaderStageCreateInfoVert.stage               = VK_SHADER_STAGE_VERTEX_BIT;
    shaderStageCreateInfoVert.module              = shaderModuleVert;
    shaderStageCreateInfoVert.pName               = "main";
    shaderStageCreateInfoVert.pSpecializationInfo = nullptr;

    VkPipelineShaderStageCreateInfo shaderStageCreateInfoFrag;

    shaderStageCreateInfoFrag.sType               = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStageCreateInfoFrag.pNext               = nullptr;
    shaderStageCreateInfoFrag.flags               = 0;
    shaderStageCreateInfoFrag.stage               = VK_SHADER_STAGE_FRAGMENT_BIT;
    shaderStageCreateInfoFrag.module              = shaderModuleFrag;
    shaderStageCreateInfoFrag.pName               = "main";
    shaderStageCreateInfoFrag.pSpecializationInfo = nullptr;

    VkPipelineShaderStageCreateInfo shaderStages[] =
    {
        shaderStageCreateInfoVert,
        shaderStageCreateInfoFrag
    };

    VkPipelineVertexInputStateCreateInfo pipelineVertexInputCreateInfo;
    pipelineVertexInputCreateInfo.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    pipelineVertexInputCreateInfo.pNext                           = nullptr;
    pipelineVertexInputCreateInfo.flags                           = 0;
    pipelineVertexInputCreateInfo.vertexBindingDescriptionCount   = 0;
    pipelineVertexInputCreateInfo.pVertexBindingDescriptions      = nullptr;
    pipelineVertexInputCreateInfo.vertexAttributeDescriptionCount = 0;
    pipelineVertexInputCreateInfo.pVertexAttributeDescriptions    = nullptr;

    VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyCreateInfo;
    pipelineInputAssemblyCreateInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    pipelineInputAssemblyCreateInfo.pNext                  = nullptr;
    pipelineInputAssemblyCreateInfo.flags                  = 0;
    pipelineInputAssemblyCreateInfo.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    pipelineInputAssemblyCreateInfo.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport;
    viewport.x        = 0;
    viewport.y        = 0;
    viewport.width    = SIZEW;
    viewport.height   = SIZEH;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor;
    scissor.offset = { 0, 0 };
    scissor.extent = { SIZEW, SIZEH };

    VkPipelineViewportStateCreateInfo pipelineViewportCreateInfo;
    pipelineViewportCreateInfo.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    pipelineViewportCreateInfo.pNext         = nullptr;
    pipelineViewportCreateInfo.flags         = 0;
    pipelineViewportCreateInfo.viewportCount = 1;
    pipelineViewportCreateInfo.scissorCount  = 1;
    pipelineViewportCreateInfo.pViewports    = &viewport;
    pipelineViewportCreateInfo.pScissors     = &scissor;

    VkPipelineRasterizationStateCreateInfo pipelineRasterizationCreateInfo;
    pipelineRasterizationCreateInfo.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    pipelineRasterizationCreateInfo.pNext                   = nullptr;
    pipelineRasterizationCreateInfo.flags                   = 0;
    pipelineRasterizationCreateInfo.depthClampEnable        = VK_FALSE;
    pipelineRasterizationCreateInfo.rasterizerDiscardEnable = VK_FALSE;
    pipelineRasterizationCreateInfo.polygonMode             = VK_POLYGON_MODE_FILL;
    pipelineRasterizationCreateInfo.cullMode                = VK_CULL_MODE_NONE;
    pipelineRasterizationCreateInfo.frontFace               = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    pipelineRasterizationCreateInfo.depthBiasEnable         = VK_FALSE;
    pipelineRasterizationCreateInfo.depthBiasConstantFactor = 0.0f;
    pipelineRasterizationCreateInfo.depthBiasClamp          = 0.0f;
    pipelineRasterizationCreateInfo.depthBiasSlopeFactor    = 0.0f;
    pipelineRasterizationCreateInfo.lineWidth               = 1.0f;

    VkPipelineMultisampleStateCreateInfo pipelineMultisampleCreateInfo;
    pipelineMultisampleCreateInfo.sType                 = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    pipelineMultisampleCreateInfo.pNext                 = nullptr;
    pipelineMultisampleCreateInfo.flags                 = 0;
    pipelineMultisampleCreateInfo.rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT; // 1, 2, 4, 8, 16, 32, 64 is possible
    pipelineMultisampleCreateInfo.sampleShadingEnable   = VK_FALSE;
    pipelineMultisampleCreateInfo.minSampleShading      = 1.0f;
    pipelineMultisampleCreateInfo.pSampleMask           = nullptr;
    pipelineMultisampleCreateInfo.alphaToCoverageEnable = VK_FALSE;
    pipelineMultisampleCreateInfo.alphaToOneEnable      = VK_FALSE;

    VkPipelineColorBlendAttachmentState pipelineColorBlendAttachment;
    pipelineColorBlendAttachment.blendEnable         = VK_TRUE;
    pipelineColorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    pipelineColorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    pipelineColorBlendAttachment.colorBlendOp        = VK_BLEND_OP_ADD;
    pipelineColorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    pipelineColorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    pipelineColorBlendAttachment.alphaBlendOp        = VK_BLEND_OP_ADD;
    pipelineColorBlendAttachment.colorWriteMask      = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

    VkPipelineColorBlendStateCreateInfo pipelineColorBlendCreateInfo;
    pipelineColorBlendCreateInfo.sType             = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    pipelineColorBlendCreateInfo.pNext             = nullptr;
    pipelineColorBlendCreateInfo.flags             = 0;
    pipelineColorBlendCreateInfo.logicOpEnable     = VK_FALSE;
    pipelineColorBlendCreateInfo.logicOp           = VK_LOGIC_OP_NO_OP;
    pipelineColorBlendCreateInfo.attachmentCount   = 1;
    pipelineColorBlendCreateInfo.pAttachments      = &pipelineColorBlendAttachment;
    pipelineColorBlendCreateInfo.blendConstants[0] = 0.0f;
    pipelineColorBlendCreateInfo.blendConstants[1] = 0.0f;
    pipelineColorBlendCreateInfo.blendConstants[2] = 0.0f;
    pipelineColorBlendCreateInfo.blendConstants[3] = 0.0f;

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo;
    pipelineLayoutCreateInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.pNext                  = nullptr;
    pipelineLayoutCreateInfo.flags                  = 0;
    pipelineLayoutCreateInfo.setLayoutCount         = 0;
    pipelineLayoutCreateInfo.pSetLayouts            = nullptr;
    pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
    pipelineLayoutCreateInfo.pPushConstantRanges    = nullptr;

    ASSERT_VK(vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout));

    VkAttachmentDescription attachmentDescription;
    attachmentDescription.flags          = VK_ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT;
    attachmentDescription.format         = VK_FORMAT_B8G8R8A8_UNORM;
    attachmentDescription.samples        = VK_SAMPLE_COUNT_1_BIT;
    attachmentDescription.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachmentDescription.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
    attachmentDescription.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachmentDescription.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
    attachmentDescription.finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference attachmentReference;
    attachmentReference.attachment = 0;
    attachmentReference.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpassDescription;
    subpassDescription.flags                   = 0;
    subpassDescription.pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS; // COMPUTE and VK_PIPELINE_BIND_POINT_RAY_TRACING_NV are options here
    subpassDescription.inputAttachmentCount    = 0;
    subpassDescription.pInputAttachments       = nullptr;
    subpassDescription.colorAttachmentCount    = 1;
    subpassDescription.pColorAttachments       = &attachmentReference;
    subpassDescription.pResolveAttachments     = nullptr;
    subpassDescription.pDepthStencilAttachment = nullptr;
    subpassDescription.preserveAttachmentCount = 0;
    subpassDescription.pPreserveAttachments    = nullptr;

    VkSubpassDependency subpassDependency;
    subpassDependency.srcSubpass      = VK_SUBPASS_EXTERNAL;    // we have two subpasses automatically: 1) Initial Layout to Layout, 2) Layout to Final Layout
    subpassDependency.dstSubpass      = 0;
    subpassDependency.srcStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpassDependency.dstStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpassDependency.srcAccessMask   = 0;
    subpassDependency.dstAccessMask   = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    subpassDependency.dependencyFlags = 0;

    VkRenderPassCreateInfo renderPassCreateInfo;
    renderPassCreateInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassCreateInfo.pNext           = nullptr;
    renderPassCreateInfo.flags           = 0;
    renderPassCreateInfo.attachmentCount = 1;
    renderPassCreateInfo.pAttachments    = &attachmentDescription;
    renderPassCreateInfo.subpassCount    = 1;
    renderPassCreateInfo.pSubpasses      = &subpassDescription;
    renderPassCreateInfo.dependencyCount = 1;
    renderPassCreateInfo.pDependencies   = &subpassDependency;

    ASSERT_VK(vkCreateRenderPass(device, &renderPassCreateInfo, nullptr, &renderPass));

    VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo;
    graphicsPipelineCreateInfo.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    graphicsPipelineCreateInfo.pNext               = nullptr;
    graphicsPipelineCreateInfo.flags               = 0;
    graphicsPipelineCreateInfo.stageCount          = 2;
    graphicsPipelineCreateInfo.pStages             = shaderStages;
    graphicsPipelineCreateInfo.pVertexInputState   = &pipelineVertexInputCreateInfo;
    graphicsPipelineCreateInfo.pInputAssemblyState = &pipelineInputAssemblyCreateInfo;
    graphicsPipelineCreateInfo.pTessellationState  = nullptr;
    graphicsPipelineCreateInfo.pViewportState      = &pipelineViewportCreateInfo;
    graphicsPipelineCreateInfo.pRasterizationState = &pipelineRasterizationCreateInfo;
    graphicsPipelineCreateInfo.pMultisampleState   = &pipelineMultisampleCreateInfo;
    graphicsPipelineCreateInfo.pDepthStencilState  = nullptr;
    graphicsPipelineCreateInfo.pColorBlendState    = &pipelineColorBlendCreateInfo;
    graphicsPipelineCreateInfo.pDynamicState       = nullptr;
    graphicsPipelineCreateInfo.layout              = pipelineLayout;    // Is Handle -> vkCreatePipelineLayout
    graphicsPipelineCreateInfo.renderPass          = renderPass;        // Is Handle -> vkCreateRenderPass
    graphicsPipelineCreateInfo.subpass             = 0;
    graphicsPipelineCreateInfo.basePipelineHandle  = VK_NULL_HANDLE;
    graphicsPipelineCreateInfo.basePipelineIndex   = -1;

    ASSERT_VK(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfo, nullptr, &pipeline));

    framebuffers = new VkFramebuffer[amountOfSwapchainImages];

    for (int i = 0; i < amountOfSwapchainImages; i++)
    {
        VkFramebufferCreateInfo framebufferCreateInfo;
        framebufferCreateInfo.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferCreateInfo.pNext           = nullptr;
        framebufferCreateInfo.flags           = 0;
        framebufferCreateInfo.renderPass      = renderPass;  // Is Handle -> vkCreateRenderPass
        framebufferCreateInfo.attachmentCount = 1;
        framebufferCreateInfo.pAttachments    = &(imageViews[i]);
        framebufferCreateInfo.width           = SIZEW;
        framebufferCreateInfo.height          = SIZEH;
        framebufferCreateInfo.layers          = 1;

        ASSERT_VK(vkCreateFramebuffer(device, &framebufferCreateInfo, nullptr, &(framebuffers[i])));
    }

    VkCommandPoolCreateInfo commandPoolCreateInfo;
    commandPoolCreateInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolCreateInfo.pNext            = nullptr;
    commandPoolCreateInfo.flags            = 0;
    commandPoolCreateInfo.queueFamilyIndex = 0;     // CIV Queue should have VK QUEUE GRAPHICS BIT

    ASSERT_VK(vkCreateCommandPool(device, &commandPoolCreateInfo, nullptr, &commandPool));

    VkCommandBufferAllocateInfo commandBufferAllocateInfo;
    commandBufferAllocateInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.pNext              = nullptr;
    commandBufferAllocateInfo.commandPool        = commandPool;
    commandBufferAllocateInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAllocateInfo.commandBufferCount = amountOfSwapchainImages;

    commandBuffers = new VkCommandBuffer[amountOfSwapchainImages];

    ASSERT_VK(vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, commandBuffers));

    VkCommandBufferBeginInfo commandBufferBeginInfo;
    commandBufferBeginInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    commandBufferBeginInfo.pNext            = nullptr;
    commandBufferBeginInfo.flags            = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
    commandBufferBeginInfo.pInheritanceInfo = nullptr;

    for (size_t i = 0; i < amountOfSwapchainImages; i++)
    {
        VkRenderPassBeginInfo renderPassBeginInfo;
        renderPassBeginInfo.sType             = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassBeginInfo.pNext             = nullptr;
        renderPassBeginInfo.renderPass        = renderPass;
        renderPassBeginInfo.framebuffer       = framebuffers[i];
        renderPassBeginInfo.renderArea.offset = { 0, 0 };
        renderPassBeginInfo.renderArea.extent = { SIZEW, SIZEH };
        VkClearValue clearValue               = { 0.1f, 0.1f, 0.1f, 1.0f };
        renderPassBeginInfo.clearValueCount   = 1;
        renderPassBeginInfo.pClearValues      = &clearValue;

        ASSERT_VK(vkBeginCommandBuffer(commandBuffers[i], &commandBufferBeginInfo));

        vkCmdBeginRenderPass(commandBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

        vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);

        vkCmdEndRenderPass(commandBuffers[i]);

        ASSERT_VK(vkEndCommandBuffer(commandBuffers[i]));
    }

    VkSemaphoreCreateInfo semaphoreCreateInfo;
    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphoreCreateInfo.pNext = nullptr;
    semaphoreCreateInfo.flags = 0;

    ASSERT_VK(vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &semaphore_1));  // Image Available
    ASSERT_VK(vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &semaphore_2));  // Rendering Done
}

void DrawFrame()
{
    uint32_t imageIndex;
    
    ASSERT_VK(vkAcquireNextImageKHR(device, swapchain, std::numeric_limits<uint64_t>::max(), semaphore_1, VK_NULL_HANDLE, &imageIndex));    

    VkPipelineStageFlags waitStageMask[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

    VkSubmitInfo submitInfo;
    submitInfo.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pNext                = nullptr;
    submitInfo.waitSemaphoreCount   = 1;
    submitInfo.pWaitSemaphores      = &semaphore_1;
    submitInfo.pWaitDstStageMask    = waitStageMask;
    submitInfo.commandBufferCount   = 1;
    submitInfo.pCommandBuffers      = &(commandBuffers[imageIndex]);
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores    = &semaphore_2;

    ASSERT_VK(vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE));

    VkPresentInfoKHR presentInfo;
    presentInfo.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.pNext              = nullptr;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores    = &semaphore_2;
    presentInfo.swapchainCount     = 1;
    presentInfo.pSwapchains        = &swapchain;
    presentInfo.pImageIndices      = &imageIndex;
    presentInfo.pResults           = nullptr;

    ASSERT_VK(vkQueuePresentKHR(queue, &presentInfo));
}

void RunLoop()
{
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        DrawFrame();
    }
}

void ShutdownVulkan()
{
    // Wait until all work is done (glFlush)
    vkDeviceWaitIdle(device);

    vkDestroySemaphore(device, semaphore_1, nullptr);
    vkDestroySemaphore(device, semaphore_2, nullptr);

    vkFreeCommandBuffers(device, commandPool, amountOfSwapchainImages, commandBuffers);

    delete[] commandBuffers;

    vkDestroyCommandPool(device, commandPool, nullptr);

    // Remark:
    // Images of a Swapchain do not need destroy
    // Image Views have to be destroyed

    for (int i = 0; i < amountOfSwapchainImages; i++)
    {
        vkDestroyFramebuffer(device, framebuffers[i], nullptr);
        vkDestroyImageView(device, imageViews[i], nullptr);
    }

    delete[] imageViews;
    delete[] framebuffers;
    

    vkDestroyPipeline(device, pipeline, nullptr);
    vkDestroyPipelineLayout(device, pipelineLayout, nullptr);

    vkDestroyRenderPass(device, renderPass, nullptr);

    // Destroy Shadermodules
    vkDestroyShaderModule(device, shaderModuleVert, nullptr);
    vkDestroyShaderModule(device, shaderModuleFrag, nullptr);

    // Destroy Swapchain
    vkDestroySwapchainKHR(device, swapchain, nullptr);

    vkDestroyDevice(device, nullptr);

    vkDestroySurfaceKHR(instance, surface, nullptr);

    vkDestroyInstance(instance, nullptr);
}

void ShutdownGLFW()
{
    glfwDestroyWindow(window);
}


int main()
{
    StartGLFW();
    StartVulkan();

    RunLoop();

    ShutdownVulkan();
    ShutdownGLFW();
    
    return 0;
}