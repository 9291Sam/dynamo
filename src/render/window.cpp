#include <numeric>

#include <sebib/seblog.hpp>

#include "window.hpp"

Window::Window(vk::Extent2D size, std::string name)
{
    vkfw::init();

    vkfw::setWindowHints(vkfw::WindowHints {.clientAPI {vkfw::ClientAPI::eNone}});

    this->window_ptr = vkfw::createWindowUnique(size.width, size.height, name.c_str());

    this->window_ptr->set<vkfw::InputMode::eCursor>(vkfw::CursorMode::eDisabled);
}

Window::~Window() 
{
    this->window_ptr->destroy();

    vkfw::terminate();
}

vk::Extent2D Window::size() const
{
    auto [width, height] = this->window_ptr->getFramebufferSize();

    return vk::Extent2D {
        .width  {static_cast<std::uint32_t>(width)},
        .height {static_cast<std::uint32_t>(height)},
    };
}

float Window::getDeltaTimeSeconds() const
{
    return static_cast<float>(this->last_frame_duration.count()) / 1'000'000'000.0f;
}

bool Window::shouldClose() const
{
    return this->window_ptr->shouldClose();
}

vk::UniqueSurfaceKHR Window::createSurface(vk::Instance instance) const
{
    return vkfw::createWindowSurfaceUnique(instance, *this->window_ptr); 
}

void Window::pollEvents() 
{
    vkfw::pollEvents();

    auto currentTime = std::chrono::steady_clock::now();

    this->last_frame_duration = currentTime - this->last_frame_time;

    this->last_frame_time = currentTime;  
}

void Window::blockThisThreadIfMinimized() const
{
    while (this->size().width == 0 || this->size().height == 0)
    {
        vkfw::waitEvents();
        std::this_thread::yield();
    }
}

bool Window::isKeyPressed(vkfw::Key key) const
{
    return this->window_ptr->getKey(key);
}

std::pair<double, double> Window::getMouseDelta()
{
    auto [x, y] = this->window_ptr->getCursorPos();

    std::pair<double, double> delta {
        x - this->previous_mouse_pos.first,
        y - this->previous_mouse_pos.second
    };

    // moving doubles :eyes:
    this->previous_mouse_pos = std::make_pair<double, double>(std::move(x), std::move(y));

    return delta;
}