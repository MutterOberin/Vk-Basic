// Vk-Basic.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>


#include "vulkan\vulkan.h"



#define ASSERT_VK(errorcode)\
    if (errorcode != VK_SUCCESS)\
    {\
        __debugbreak();\
    }

VkDevice   device;
VkInstance instance;

void PrintPhysicalDevice(VkPhysicalDevice device)
{
    VkPhysicalDeviceProperties props;

    vkGetPhysicalDeviceProperties(device, &props);

    std::cout << std::endl;
    std::cout << "Device Name:           " << props.deviceName << std::endl;
    std::cout << "================================================================================" << std::endl;
    std::cout << "Device API    Version: " << VK_VERSION_MAJOR(props.apiVersion) << "." << VK_VERSION_MINOR(props.apiVersion) << "." << VK_VERSION_PATCH(props.apiVersion) << std::endl;
    std::cout << "Device Driver Version: " << VK_VERSION_MAJOR(props.driverVersion) << "." << VK_VERSION_MINOR(props.driverVersion) << "." << VK_VERSION_PATCH(props.driverVersion) << std::endl;

    std::cout << "Device Type:           ";

    if (props.deviceType == 0) std::cout << "Other";

    if (props.deviceType == 1) std::cout << "GPU (Integrated)";

    if (props.deviceType == 2) std::cout << "GPU (Discrete)";

    if (props.deviceType == 3) std::cout << "GPU (Virtual)";

    if (props.deviceType == 4) std::cout << "CPU";

    std::cout << std::endl;

    //uint32_t                            apiVersion;
    //uint32_t                            driverVersion;
    //uint32_t                            vendorID;
    //uint32_t                            deviceID;
    //VkPhysicalDeviceType                deviceType;
    //char                                deviceName[VK_MAX_PHYSICAL_DEVICE_NAME_SIZE];
    //uint8_t                             pipelineCacheUUID[VK_UUID_SIZE];
    //VkPhysicalDeviceLimits              limits;
    //VkPhysicalDeviceSparseProperties    sparseProperties;

    uint32_t amountOfQueueFamilies = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &amountOfQueueFamilies, NULL);
    VkQueueFamilyProperties* familyProperties = new VkQueueFamilyProperties[amountOfQueueFamilies];
    vkGetPhysicalDeviceQueueFamilyProperties(device, &amountOfQueueFamilies, familyProperties);

    std::cout << std::endl;
    std::cout << "Queue Families Count:  " << amountOfQueueFamilies << std::endl;

    for (int i = 0; i < amountOfQueueFamilies; i++)
    {
        std::cout << std::endl;
        std::cout << "Queue Family #" << i << std::endl;
        std::cout << "--------------------------------------------------------------------------------" << std::endl;
        std::cout << "VK_QUEUE_GRAPHICS_BIT          " << ((familyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)       != 0) << std::endl;
        std::cout << "VK_QUEUE_COMPUTE_BIT           " << ((familyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT)        != 0) << std::endl;
        std::cout << "VK_QUEUE_TRANSFER_BIT          " << ((familyProperties[i].queueFlags & VK_QUEUE_TRANSFER_BIT)       != 0) << std::endl;
        std::cout << "VK_QUEUE_SPARSE_BINDING_BIT    " << ((familyProperties[i].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) != 0) << std::endl;
        std::cout << "VK_QUEUE_PROTECTED_BIT         " << ((familyProperties[i].queueFlags & VK_QUEUE_PROTECTED_BIT)      != 0) << std::endl;
        std::cout << std::endl;
        std::cout << "Queue Count:                   " << familyProperties[i].queueCount << std::endl;
        std::cout << "Timestamp Valid Bits           " << familyProperties[i].timestampValidBits << std::endl;

        uint32_t w = familyProperties[i].minImageTransferGranularity.width;
        uint32_t h = familyProperties[i].minImageTransferGranularity.height;
        uint32_t d = familyProperties[i].minImageTransferGranularity.depth;

        std::cout << "Min Image Transfer Granularity " << w << " x " << h << " x " << d << std::endl;
    }
}

void PrintPhysicalDevices(VkInstance instance)
{
    VkResult res;

    uint32_t amountOfPhysicalDevices = 0;

    res = vkEnumeratePhysicalDevices(instance, &amountOfPhysicalDevices, NULL);

    ASSERT_VK(res);

    VkPhysicalDevice* physicalDevices = new VkPhysicalDevice[amountOfPhysicalDevices];

    res = vkEnumeratePhysicalDevices(instance, &amountOfPhysicalDevices, physicalDevices);

    ASSERT_VK(res);

    std::cout << "Physical Device Count: " << amountOfPhysicalDevices << std::endl;

    for (int i = 0; i < amountOfPhysicalDevices; i++)
    {
        PrintPhysicalDevice(physicalDevices[i]);
    }
}


int main()
{
    // Example: Application Info

    VkApplicationInfo appInfo;

    appInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext              = NULL;
    appInfo.pApplicationName   = "Vulkan Basic-1";
    appInfo.pEngineName        = "Vulkan Basic-1";
    appInfo.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
    appInfo.engineVersion      = VK_MAKE_VERSION(0, 1, 0);
    appInfo.apiVersion         = VK_API_VERSION_1_1; // Only Options: VK_API_VERSION_1_0 and VK_API_VERSION_1_1;

    
    // Example: Instance Create Info

    VkInstanceCreateInfo instanceInfo;

    instanceInfo.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.pNext                   = NULL;
    instanceInfo.flags                   = 0;
    instanceInfo.pApplicationInfo        = &appInfo;
    instanceInfo.enabledLayerCount       = 0;
    instanceInfo.enabledExtensionCount   = 0;
    instanceInfo.ppEnabledExtensionNames = NULL;
    instanceInfo.ppEnabledExtensionNames = NULL;

    // Example: Instance

    VkResult res;

    res = vkCreateInstance(&instanceInfo, NULL, &instance);

    ASSERT_VK(res);

    // Example: Physical Devices (finally something cool)

    PrintPhysicalDevices(instance);


    uint32_t amountOfPhysicalDevices = 0;

    res = vkEnumeratePhysicalDevices(instance, &amountOfPhysicalDevices, NULL);

    ASSERT_VK(res);

    VkPhysicalDevice* physicalDevices = new VkPhysicalDevice[amountOfPhysicalDevices];

    res = vkEnumeratePhysicalDevices(instance, &amountOfPhysicalDevices, physicalDevices);

    ASSERT_VK(res);

    // Example: Device Queue Create Info

    VkDeviceQueueCreateInfo deviceQueueCreateInfo;

    deviceQueueCreateInfo.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    deviceQueueCreateInfo.pNext            = NULL;
    deviceQueueCreateInfo.flags            = 0;
    deviceQueueCreateInfo.queueFamilyIndex = 0; // Actually check which queue is the best to use
    deviceQueueCreateInfo.queueCount       = 4;
    deviceQueueCreateInfo.pQueuePriorities = NULL;

    // Example: Device Create Info

    VkPhysicalDeviceFeatures usedFeatures = {};

    VkDeviceCreateInfo deviceCreateInfo;

    deviceCreateInfo.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.pNext                   = NULL;
    deviceCreateInfo.flags                   = 0;
    deviceCreateInfo.queueCreateInfoCount    = 1;
    deviceCreateInfo.pQueueCreateInfos       = &deviceQueueCreateInfo;
    deviceCreateInfo.enabledLayerCount       = 0;
    deviceCreateInfo.ppEnabledLayerNames     = NULL;
    deviceCreateInfo.enabledExtensionCount   = 0;
    deviceCreateInfo.ppEnabledExtensionNames = NULL;
    deviceCreateInfo.pEnabledFeatures        = &usedFeatures;

    res = vkCreateDevice(physicalDevices[0], &deviceCreateInfo, NULL, &device); // Actually check which device to use

    ASSERT_VK(res);

    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
