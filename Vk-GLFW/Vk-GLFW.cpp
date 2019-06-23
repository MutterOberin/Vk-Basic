#define GLFW_INCLUDE_VULKAN
#include <GLFW\glfw3.h>     // Includes Vulkan
// #include "vulkan\vulkan.h"

#include <vector>
#include <string>
#include <iostream>


#define ASSERT_VK(errorcode)\
    if (errorcode != VK_SUCCESS)\
    {\
        __debugbreak();\
    }

VkDevice   device;
VkInstance instance;

VkSurfaceKHR   surface;
VkSwapchainKHR swapchain;

GLFWwindow* window;

const uint32_t SIZEW = 800;
const uint32_t SIZEH = 600;

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

std::vector<VkExtensionProperties> GetSupportedExtensions()
{
    uint32_t amountOfExtensions = 0;

    vkEnumerateInstanceExtensionProperties(nullptr, &amountOfExtensions, nullptr);

    std::vector<VkExtensionProperties> extensions;
    extensions.resize(amountOfExtensions);

    vkEnumerateInstanceExtensionProperties(nullptr, &amountOfExtensions, extensions.data());

    for (int i = 0; i < extensions.size(); i++)
    {
        std::cout << "Extension: " << extensions.at(i).extensionName << std::endl;
    }

    return extensions;
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
        std::cout << "Layer: " << layers.at(i).layerName << std::endl;
        std::cout << layers.at(i).description << std::endl;
        std::cout << std::endl;
    }

    return layers;
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

    vkGetPhysicalDeviceProperties(physicalDevices[0], &props_device);

    //std::cout << "Using " << props_device.deviceName << std::endl;

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
    const std::vector<const char*> enabledDeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

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

    VkQueue queue;
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

    // 

    uint32_t amountOfSwapchainImages = 0;

    ASSERT_VK(vkGetSwapchainImagesKHR(device, swapchain, &amountOfSwapchainImages, nullptr));

    std::vector<VkImage> swapchainImages;
    swapchainImages.resize(amountOfSwapchainImages);

    ASSERT_VK(vkGetSwapchainImagesKHR(device, swapchain, &amountOfSwapchainImages, swapchainImages.data()));

    for (int i = 0; i < amountOfSwapchainImages; i++)
    {
        std::cout << "Image: " << swapchainImages.at(i) << std::endl;
    }

}

void RunLoop()
{
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
    }
}

void ShutdownVulkan()
{
    // Wait until all work is done (glFlush)
    vkDeviceWaitIdle(device);

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
