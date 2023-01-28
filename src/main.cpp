#include <iostream>

#include <sebib/seblog.hpp>
#include <render/renderer.hpp>

int main()
{
    
    seb::logLog("Dynamo started | Version: {}.{}.{}.{}",
        VERSION_MAJOR,
        VERSION_MINOR,
        VERSION_PATCH,
        VERSION_TWEAK
    );

    try
    {
        render::Renderer();

        seb::panic("end of exeecution!");
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}