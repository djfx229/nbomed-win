#include "platform.h"
#include "nbomed-core/application.h"

int main() {
    PlatformWindow* window = new PlatformWindow();
    NbomedApp *application = new NbomedApp(window);
    window->show(application);
    delete application;
    return 0;
}
