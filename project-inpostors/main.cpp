#include "CoreEngine.h"
#include "LoggingMacros.h"

int main(int, char**)
{
    LoggingMacros::InitializeSPDLog();

    std::srand(std::time(0));

    mlg::CoreEngine Engine = mlg::CoreEngine();
    if(Engine.Init() == 0)
    {
        Engine.PrepareScene();
        int32_t ReturnCode = Engine.MainLoop();

        if (ReturnCode != 0) {
            return ReturnCode;
        }
    }

    return 0;
}
