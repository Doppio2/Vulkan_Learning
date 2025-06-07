#include <windows.h>

#include <stdint.h>
#include <cstring>
#include <malloc.h>
#include <assert.h>        // Later implement own asssert.

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_win32.h>

// For dynamic arrays and hash table's i need a stb lib here.

#define func static
#define global_var static

#define ArrayCount(Array)(sizeof(Array)/sizeof(Array[0]))

typedef uint32_t u32;

// Global for now. 

bool global_var GlobalRunning;

#ifdef _DEBUG
    global_var const int   ValidationLayerCount = 1;
    global_var const char *ValidationLayers[ValidationLayerCount] = {"VK_LAYER_KHRONOS_validation"};
    global_var bool EnableValidationLayers = true;
#else
    global_var bool EnableValidationLayers = false;
#endif

#ifdef _DEBUG
bool CheckValidationLayerSupport()
{
    u32 AvailableLayerCount;
    vkEnumerateInstanceLayerProperties(&AvailableLayerCount, nullptr);

    // TODO: for now it's a malloc.
    VkLayerProperties *AvailableLayers = (VkLayerProperties*)malloc(sizeof(VkLayerProperties)*AvailableLayerCount);
    vkEnumerateInstanceLayerProperties(&AvailableLayerCount, AvailableLayers);

    for(u32 ValidationLayerIndex = 0; ValidationLayerIndex < ValidationLayerCount; ValidationLayerIndex++) 
    {
        bool LayerFound = false;

        for(u32 AvailableLayerIndex = 0; AvailableLayerIndex < AvailableLayerCount; AvailableLayerIndex++)
        {
            if(strcmp(ValidationLayers[ValidationLayerIndex], AvailableLayers[AvailableLayerIndex].layerName) == 0)
            {
                LayerFound = true;
                break;
            }
        }

        if(!LayerFound)
        {
            return false;
        }
    }

    free(AvailableLayers);
    return true;
}
#endif

VkInstance CreateVulkanInstance()
{
    if(EnableValidationLayers && !CheckValidationLayerSupport())
    {
        OutputDebugStringA("Validation layers requested, but not available!\n");
    }

    VkInstance VulkanInstance = {};

    // This struct will be used for creating vulkan instance.
    // It's countain the info about application.
    VkApplicationInfo ApplicationInfo{};

    ApplicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    ApplicationInfo.pApplicationName = "Hello Triangle";
    ApplicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    ApplicationInfo.pEngineName = "No Engine";
    ApplicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    ApplicationInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo CreateInfo{};

    CreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    CreateInfo.pApplicationInfo = &ApplicationInfo;

    // Then we need to pass a global extensions to vkInstanceCreateInfo struct. 
    // This will give us opportuinty to connect a Win32 window and the vulkan.
    // We need create a array(or vector) and fill it with needeble extensions.
    // In our case we need a VK_KHR_SURFACE_EXTENSION_NAME(it's default for all applications).
    // And VK_KHR_WIN32_SURFACE_EXTENSION_NAME for windows.
    // Forwhile for learning i will use a standart array for it with lenght 2.
    const char *EnabledExtensions[2] = 
    {
        VK_KHR_SURFACE_EXTENSION_NAME,
        VK_KHR_WIN32_SURFACE_EXTENSION_NAME
    };

    CreateInfo.enabledExtensionCount = ArrayCount(EnabledExtensions);
    CreateInfo.ppEnabledExtensionNames = EnabledExtensions;

    if(vkCreateInstance(&CreateInfo, nullptr, &VulkanInstance) == VK_SUCCESS)
    {
        OutputDebugStringA("Vulkan instance was created succesfully.\n");
    }
    else
    {
        assert(false && "Cannot create a Vulkan instance");
    }

    return VulkanInstance;
}

void VulkanCleanup(VkInstance VulkanInstance)
{
    vkDestroyInstance(VulkanInstance, NULL); 
    VulkanInstance = VK_NULL_HANDLE;
}

func LRESULT CALLBACK
Win32MainWindowCallback(HWND Window,
                        UINT Message,
                        WPARAM WParam,
                        LPARAM LParam)
{
    LRESULT Result = 0;

    switch(Message)
    {
        case WM_CLOSE:
        {
            OutputDebugStringA("App has been closed");
        }
        case WM_ACTIVATEAPP:
        {
            OutputDebugStringA("App is active now");
        }
        case WM_DESTROY:
        {
            GlobalRunning = false;
            OutputDebugStringA("App has been destroyed");
        }
        default:
        {
            Result = DefWindowProcA(Window, Message, WParam, LParam);
        }
    }

    return Result;
}

int CALLBACK 
WinMain(HINSTANCE Instance, 
                  HINSTANCE PrevInstance,
                  PSTR CommandLine, 
                  int ShowCode)
{
    WNDCLASSA WindowClass = {};

    WindowClass.style = CS_HREDRAW|CS_VREDRAW|CS_OWNDC;
    WindowClass.lpfnWndProc = Win32MainWindowCallback;
    WindowClass.hInstance = Instance;
    WindowClass.lpszClassName = "VulkanWindowClass!";

    if(RegisterClassA(&WindowClass))
    {
        HWND Window = CreateWindowExA(0,
                                      WindowClass.lpszClassName,
                                      "Hellow Vulkan!",
                                      WS_OVERLAPPEDWINDOW|WS_VISIBLE,
                                      CW_USEDEFAULT,
                                      CW_USEDEFAULT,
                                      CW_USEDEFAULT,
                                      CW_USEDEFAULT,
                                      0, 
                                      0,
                                      Instance,
                                      0);
        if(Window)
        {
            GlobalRunning = true;

            VkInstance VulkanInstance = CreateVulkanInstance();

            while(GlobalRunning)
            {
                MSG Message;
                while(PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
                {
                    // TODO: Check it later.
                    if(Message.message == WM_QUIT)
                    {
                        GlobalRunning = false;
                    }

                    TranslateMessage(&Message);
                    DispatchMessageA(&Message);
                    OutputDebugStringA("Hellllo\n");
                }
            }

            // Maybe i should put it into WM_DESTROY case in Win32MainWindowCallback function.
            if(VulkanInstance != VK_NULL_HANDLE)
            {
                VulkanCleanup(VulkanInstance);
            }
        }
    }

    return 0;
}
