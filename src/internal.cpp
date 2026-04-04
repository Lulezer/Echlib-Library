#include "internal.hpp"

#ifdef _WIN32
#include <Windows.h>
#include <timeapi.h>
#pragma comment(lib, "winmm.lib")
#endif

namespace ech {

    // This "Janitor" class handles the OS-specific setup
    class InternalJanitor {
    public:
        InternalJanitor() {
#ifdef _WIN32
            // Automatically set 1ms precision when the library loads
            timeBeginPeriod(1);
#endif
        }

        ~InternalJanitor() {
#ifdef _WIN32
            // Automatically clean up when the program exits
            timeEndPeriod(1);
#endif
        }
    };

    // This triggers the constructor before main() starts!
    static InternalJanitor s_Janitor;

    static ech::Window* g_DefaultWindow = nullptr;

    Window*& GetDefaultWindow() {
        return g_DefaultWindow;
    }
}
