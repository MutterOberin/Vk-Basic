#include <iostream>
#include <vector>

#include "vulkan\vulkan.h"



#define ASSERT_VK(errorcode)\
    if (errorcode != VK_SUCCESS)\
    {\
        __debugbreak();\
    }

VkDevice   device;
VkInstance instance;

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
        std::cout << "Layer: " << layers.at(i).layerName << std::endl;
        std::cout << layers.at(i).description << std::endl;
        std::cout << std::endl;
    }

    return layers;
}

int main()
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

    
    // Layer

    //GetSupportedLayers();

    const std::vector<const char*> enabledLayers = { "VK_LAYER_LUNARG_standard_validation" };
    
    // Instance Instance Create Info

    VkInstanceCreateInfo instanceInfo;

    instanceInfo.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.pNext                   = nullptr;
    instanceInfo.flags                   = 0;
    instanceInfo.pApplicationInfo        = &appInfo;
    instanceInfo.enabledLayerCount       = enabledLayers.size();
    instanceInfo.ppEnabledLayerNames     = enabledLayers.data();
    instanceInfo.enabledExtensionCount   = 0;
    instanceInfo.ppEnabledExtensionNames = nullptr;

    // Instance

    ASSERT_VK(vkCreateInstance(&instanceInfo, nullptr, &instance));

    // Physical Devices
    
    uint32_t amountOfPhysicalDevices = 0;

    ASSERT_VK(vkEnumeratePhysicalDevices(instance, &amountOfPhysicalDevices, nullptr));
    
    VkPhysicalDevice* physicalDevices = new VkPhysicalDevice[amountOfPhysicalDevices];

    ASSERT_VK(vkEnumeratePhysicalDevices(instance, &amountOfPhysicalDevices, physicalDevices));

    VkPhysicalDeviceProperties props_device;

    vkGetPhysicalDeviceProperties(physicalDevices[0], &props_device);

    std::cout << "Using " << props_device.deviceName << std::endl;

    // Device Queue 

    uint32_t amountOfQueueFamilies = 0;

    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevices[0], &amountOfQueueFamilies, NULL);

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
    VkPhysicalDeviceFeatures enabledFeatures = {};


    // Device Create Info

    VkDeviceCreateInfo deviceCreateInfo;

    deviceCreateInfo.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.pNext                   = nullptr;
    deviceCreateInfo.flags                   = 0;
    deviceCreateInfo.queueCreateInfoCount    = 1;
    deviceCreateInfo.pQueueCreateInfos       = &deviceQueueCreateInfo;
    deviceCreateInfo.enabledLayerCount       = 0;
    deviceCreateInfo.ppEnabledLayerNames     = nullptr;
    deviceCreateInfo.enabledExtensionCount   = 0;
    deviceCreateInfo.ppEnabledExtensionNames = nullptr;
    deviceCreateInfo.pEnabledFeatures        = &enabledFeatures;

    // Actually decide what device to use in a smarter way
    ASSERT_VK(vkCreateDevice(physicalDevices[0], &deviceCreateInfo, nullptr, &device));
    
    return 0;
}
