#include <numeric>

#include <sebib/seblog.hpp>

#include "window.hpp"

Window::Window(vk::Extent2D windowSize, const char* windowName)
{
    vkfw::init();

    vkfw::setWindowHints(vkfw::WindowHints {.clientAPI {vkfw::ClientAPI::eNone}});

    this->window_ptr = vkfw::createWindowUnique(windowSize.width, windowSize.height, windowName);

}

Window::~Window() 
{
    this->window_ptr->destroy();

    vkfw::terminate();
}

vk::Extent2D Window::getSize() const
{
    auto [width, height] = this->window_ptr->getFramebufferSize();

    return vk::Extent2D {
        .width  {static_cast<std::uint32_t>(width)},
        .height {static_cast<std::uint32_t>(height)},
    };
}

auto Window::getFrameTimeS() const
    -> float
{
    return static_cast<float>(this->last_frame_duration.count()) / 1'000'000'000.0f;
}

auto Window::shouldClose() const 
    -> bool
{
    return this->window_ptr->shouldClose();
}

auto Window::createSurface(const vk::Instance& instance) const
    -> vk::UniqueSurfaceKHR
{
    return vkfw::createWindowSurfaceUnique(instance, *this->window_ptr); 
}

void Window::pollEvents() 
{
    vkfw::pollEvents();

    FPSTimePoint currentTime = FPSClock::now();

    this->last_frame_duration = currentTime - this->last_frame_time;

    this->last_frame_time = currentTime;  
}

void Window::blockThisThreadIfMinimized() const
{
    int width = 0, height = 0;
    glfwGetFramebufferSize(
            static_cast<GLFWwindow*>(this->window_ptr.get()), &width, &height);
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(
            static_cast<GLFWwindow*>(this->window_ptr.get()), &width, &height);
        glfwWaitEvents();

        // This lessens the impact of this function busy waiting
        std::this_thread::yield();
    }
}

bool Window::isKeyPressed(vkfw::Key key) const
{
    int keyState = glfwGetKey(
        static_cast<GLFWwindow*>(*this->window_ptr), 
        static_cast<int>(key)
    );

    const char* maybeError;

    glfwGetError(&maybeError);

    if (maybeError == nullptr)
    {
        return keyState == GLFW_PRESS;
    }

    seb::panic("Glfw error: {}", maybeError);
}