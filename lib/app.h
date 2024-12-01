#pragma once

#include <napi.h>
#include <Windows.h>
#include <WebView2.h>
#include <wrl.h>

class Zoin : public Napi::ObjectWrap<Zoin> {
public:
    Zoin(const Napi::CallbackInfo& info);

    Napi::Value BrowserWindow(const Napi::CallbackInfo& info);
    Napi::Value LoadUrl(const Napi::CallbackInfo& info); 

    static Napi::Function GetClass(Napi::Env env); 

private:
    std::string _windowTitle;
    LPCWSTR _initialUrl;

    static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    Microsoft::WRL::ComPtr<ICoreWebView2Controller> _webviewController;
    Microsoft::WRL::ComPtr<ICoreWebView2> _webview;
};