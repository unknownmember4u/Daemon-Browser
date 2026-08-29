#include "request_handler.h"
#include "include/cef_app.h"
#include "include/wrapper/cef_helpers.h"
#include <iostream>

DaemonRequestHandler::DaemonRequestHandler() {}

void DaemonRequestHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser) {
    CEF_REQUIRE_UI_THREAD();
    // Quit the message loop when the main window is closed.
    CefQuitMessageLoop();
}

cef_return_value_t DaemonRequestHandler::OnBeforeResourceLoad(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefRequest> request,
    CefRefPtr<CefCallback> callback) {

    // REQUEST OBSERVATION
    std::string method = request->GetMethod();
    std::string url = request->GetURL();
    int resource_type = request->GetResourceType(); // RT_MAIN_FRAME = 0, etc.

    std::cout << "REQUEST" << std::endl;
    std::cout << "METHOD: " << method << std::endl;
    std::cout << "URL: " << url << std::endl;
    std::cout << "RESOURCE_TYPE: " << resource_type << std::endl;
    std::cout << "---------------------------------" << std::endl;

    return RV_CONTINUE;
}
