#include "app.h"
using namespace Napi;

Zoin *currentZoin = nullptr;

Zoin::Zoin(const Napi::CallbackInfo &info) : ObjectWrap(info)
{
    Napi::Env env = info.Env();
}

Napi::Value Zoin::BrowserWindow(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    if (info.Length() < 1 || !info[0].IsString())
    {
        Napi::TypeError::New(env, "First argument must be a string")
            .ThrowAsJavaScriptException();
        return env.Null();
    }

    if (info.Length() < 2 || !info[1].IsString())
    {
        Napi::TypeError::New(env, "Second argument must be a string")
            .ThrowAsJavaScriptException();
        return env.Null();
    }

    Napi::String title = info[0].As<Napi::String>();
    std::string titleStr = title.Utf8Value();

    std::string url = info[1].As<Napi::String>().Utf8Value();

    HINSTANCE hInstance = GetModuleHandle(nullptr);

    WNDCLASSW wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"WebView2WindowClass";

    if (!RegisterClassW(&wc))
    {
        Napi::TypeError::New(env, "Error registering window class")
            .ThrowAsJavaScriptException();
        return env.Null();
    }

    HWND hWnd = CreateWindowExW(
        0,
        L"WebView2WindowClass",
        std::wstring(titleStr.begin(), titleStr.end()).c_str(),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        800, 600,
        nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        Napi::TypeError::New(env, "Error creating window")
            .ThrowAsJavaScriptException();
        return env.Null();
    }

    currentZoin = this;

    ShowWindow(hWnd, SW_SHOWNORMAL);

    HRESULT hr = CreateCoreWebView2EnvironmentWithOptions(
        nullptr, L"C:\\Temp\\Zoin", nullptr, 
        Microsoft::WRL::Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(
            [this, hWnd, url](HRESULT result, ICoreWebView2Environment *env) -> HRESULT
            {
                if (FAILED(result))
                    return result;

                env->CreateCoreWebView2Controller(
                    hWnd,
                    Microsoft::WRL::Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
                        [this, hWnd, url](HRESULT result, ICoreWebView2Controller *controller) -> HRESULT
                        {
                            if (controller != nullptr)
                            {
                                _webviewController = controller;
                                _webviewController->get_CoreWebView2(&_webview);
                            }

                            RECT bounds;
                            GetClientRect(hWnd, &bounds);
                            _webviewController->put_Bounds(bounds);

                            _webview->Navigate(std::wstring(url.begin(), url.end()).c_str());

                            return S_OK;
                        })
                        .Get());
                return S_OK;
            })
            .Get());

    if (FAILED(hr))
    {
        std::string errorMessage = "Failed to initialize WebView2. HRESULT: " + std::to_string(hr);
        Napi::TypeError::New(env, errorMessage)
            .ThrowAsJavaScriptException();
        return env.Null();
    }

    // Bucle de mensajes
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return Napi::Number::New(env, reinterpret_cast<intptr_t>(hWnd));
}

Napi::Value Zoin::LoadUrl(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    if (info.Length() < 1 || !info[0].IsString())
    {
        Napi::TypeError::New(env, "First argument must be a string URL")
            .ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!_webview)
    {
        Napi::Error::New(env, "WebView2 is not initialized")
            .ThrowAsJavaScriptException();
        return env.Null();
    }

    std::string url = info[0].As<Napi::String>().Utf8Value();
    _webview->Navigate(std::wstring(url.begin(), url.end()).c_str());

    return env.Undefined();
}

Napi::Function Zoin::GetClass(Napi::Env env)
{
    return DefineClass(
        env,
        "Zoin",
        {
            Zoin::InstanceMethod("create_window", &Zoin::BrowserWindow),
            Zoin::InstanceMethod("load_url", &Zoin::LoadUrl),

        });
}

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
    Napi::String name = Napi::String::New(env, "Zoin");
    exports.Set(name, Zoin::GetClass(env));
    return exports;
}

LRESULT CALLBACK Zoin::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (currentZoin == nullptr)
    {
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    switch (message)
    {
    case WM_SIZE:
        if (currentZoin->_webviewController != nullptr)
        {
            RECT bounds;
            GetClientRect(hWnd, &bounds);

            currentZoin->_webviewController->put_Bounds(bounds);
        }

        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

NODE_API_MODULE(Zoin, Init)