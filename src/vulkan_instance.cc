#include "vulkan_instance.h"

#include <iostream>
#include <cstring>
#include <vector>

#if defined(SCIN_VALIDATE_VULKAN)
namespace {

VkResult CreateDebugUtilsMessengerEXT(
  VkInstance instance,
  const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
  const VkAllocationCallbacks* pAllocator,
  VkDebugUtilsMessengerEXT* pDebugMessenger) {
  auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
      instance, "vkCreateDebugUtilsMessengerEXT");
  if (func != nullptr) {
      return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
  } else {
      return VK_ERROR_EXTENSION_NOT_PRESENT;
  }
}

void DestroyDebugUtilsMessengerEXT(
  VkInstance instance,
  VkDebugUtilsMessengerEXT debugMessenger,
  const VkAllocationCallbacks* pAllocator) {
  auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
      instance, "vkDestroyDebugUtilsMessengerEXT");
  if (func != nullptr) {
      func(instance, debugMessenger, pAllocator);
  }
}

const std::vector<const char*> validation_layers = {
    "VK_LAYER_LUNARG_standard_validation"
};

bool CheckValidationLayerSupport() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : validation_layers) {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) {
            return false;
        }
    }

    return true;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) {
  std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
  return VK_FALSE;
}

bool SetupDebugMessenger(VkInstance instance,
    VkDebugUtilsMessengerEXT* debugMessenger) {
  VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  createInfo.messageSeverity =
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  createInfo.messageType =
      VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  createInfo.pfnUserCallback = DebugCallback;

  if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr,
      debugMessenger) != VK_SUCCESS) {
      return false;
    }

  return true;
}

}  // namespace

#endif  // SCIN_VALIDATE_VULKAN

namespace scin {

VulkanInstance::VulkanInstance()
    : instance_(VK_NULL_HANDLE) {
}

VulkanInstance::~VulkanInstance() {
  if (instance_ !=  VK_NULL_HANDLE) {
    Destroy();
  }
}

bool VulkanInstance::Create() {
  VkApplicationInfo app_info = {};
  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info.pApplicationName = "scinsynth";
  app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  app_info.pEngineName = "Scintillator";
  app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  app_info.apiVersion = VK_API_VERSION_1_0;

  VkInstanceCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  create_info.pApplicationInfo = &app_info;

  uint32_t glfw_extension_count = 0;
  const char** glfw_extensions;
  glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

  std::vector<const char*> extensions(glfw_extensions,
      glfw_extensions + glfw_extension_count);

#if defined(SCIN_VALIDATE_VULKAN)
  extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

  create_info.enabledLayerCount = static_cast<uint32_t>(
      validation_layers.size());
  create_info.ppEnabledLayerNames = validation_layers.data();
#else
  create_info.enabledLayerCount = 0;
#endif

  create_info.enabledExtensionCount = static_cast<uint32_t>(
      extensions.size());
  create_info.ppEnabledExtensionNames = extensions.data();

  if (vkCreateInstance(&create_info, nullptr, &instance_)
      != VK_SUCCESS) {
    std::cerr << "failed to create instance." << std::endl;
    return false;
  }

#if defined(SCIN_VALIDATE_VULKAN)
  if (!SetupDebugMessenger(instance_, &debug_messenger_)) {
    std::cerr << "failed to create debug messenger" << std::endl;
    return false;
  }
#endif

  return true;
}

void VulkanInstance::Destroy() {
#if defined(SCIN_VALIDATE_VULKAN)
  DestroyDebugUtilsMessengerEXT(instance_, debug_messenger_, nullptr);
#endif
  vkDestroyInstance(instance_, nullptr);
  instance_ = VK_NULL_HANDLE;
}

}  // namespace scin
