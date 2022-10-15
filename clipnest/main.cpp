#include <string>
#include <Windows.h>

#include "win32/app.h"
#include "win32/shell_notify_icon.h"
#include "win32/popup_menu.h"
#include "win32/clipboard.h"

#include "str.h"
#include "fmt/core.h"

#define OWN_WM_NOTIFY_ICON_MESSAGE WM_APP + 1

using namespace std;

void calculate(win32::shell_notify_icon& sni, win32::popup_menu& m) {
    string input = win32::clipboard::get_text();

    m.clear();

    size_t wc = str::word_count(input);
    //sni.set_tooptip(fmt::format("{} word(s)", wc));
    m.add("ln", fmt::format("length: {}", input.length()));
    m.add("wc", fmt::format("words: {}", wc));
    m.add("64", fmt::format("base64: {}",
        str::base64_encode((const unsigned char*)input.c_str(), input.length())));
    m.separator();
    m.add("x", "&Exit");
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {

    win32::app win32app;
    win32app.add_clipboard_listener();

    win32::popup_menu m{ win32app.get_hwnd() };
    m.add("x", "&Exit");

    // notification icon
    win32::shell_notify_icon sni{ win32app.get_hwnd(), OWN_WM_NOTIFY_ICON_MESSAGE };

    win32app.on_app_message = [&m, &win32app, &sni](UINT msg, WPARAM wParam, LPARAM lParam) {
        switch (msg) {
            case OWN_WM_NOTIFY_ICON_MESSAGE:
                switch (lParam) {
                    case WM_LBUTTONUP:
                    case WM_RBUTTONUP:
                        calculate(sni, m);
                        m.show();
                        break;
                }
                break;
            case WM_COMMAND: {
                int loword_wparam = LOWORD(wParam);
                string id = m.id_from_loword_wparam(loword_wparam);
                if (id == "x") {
                    ::PostQuitMessage(0);
                }
            }
                break;
            case WM_CLIPBOARDUPDATE:
                calculate(sni, m);
                return 0;
        }
        return 0;
    };

    win32app.run();

    return 0;
}