#include <string>
#include <memory>
#include <vector>
#include <Windows.h>

#include "win32/app.h"
#include "win32/shell.h"
#include "win32/shell_notify_icon.h"
#include "win32/popup_menu.h"
#include "win32/clipboard.h"
#include "win32/reg.h"
#include "ext/github.h"

#include "str.h"
#include "fss.h"
#include "fmt/core.h"

#include "global.h"
#include "operation.h"
#include "mini_result_popup.h"

#include "grey.h"

#define OWN_WM_NOTIFY_ICON_MESSAGE WM_APP + 1

const size_t MAX_MI_LENGTH{ 40 };

using namespace std;
using namespace clipnest;

bool is_ui_open{ false };
unique_ptr<grey::backend> active_backend;

void copy_op_result(const std::string& op_id) {
    auto it = operation::all.find(op_id);
    if (it == operation::all.end()) return;
    auto op = (*it).second;
    win32::clipboard::set_text(op->result);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {

    // add autostart entry
    win32::reg::set_value(win32::reg::hive::current_user, "Software\\Microsoft\\Windows\\CurrentVersion\\Run",
        string("\"") + fss::get_current_exec_path() + "\"",
        ProductName);

    // check for latest release
    ext::github gh;
    gh.get_latest_release("aloneguid", "clipnest");

    clipnest::operation::init();

    win32::app win32app;
    //win32app.add_clipboard_listener();

    win32::popup_menu m{ win32app.get_hwnd() };
    m.add("$", "Donate");
    m.add("?", fmt::format("About {} v{}", ProductName, Version));
    m.add("x", "Quit");

    // notification icon
    win32::shell_notify_icon sni{ win32app.get_hwnd(), OWN_WM_NOTIFY_ICON_MESSAGE };
    sni.set_tooptip(fmt::format("{} {}", ProductName, Version));

    win32app.on_any_message = [](MSG& msg) {
        if (active_backend) {
            active_backend->run_one_frame();

            if (!is_ui_open) {
                active_backend.reset();
            }
        }
    };

    win32app.on_app_message = [&m, &win32app, &sni](UINT msg, WPARAM wParam, LPARAM lParam) {
        switch (msg) {
            case OWN_WM_NOTIFY_ICON_MESSAGE:
                switch (lParam) {
                    case WM_LBUTTONUP:
                        if (!active_backend) {
                            active_backend = mini_result_popup::show(is_ui_open);
                        }
                        //m.show();
                        break;
                    case WM_RBUTTONUP:
                        m.show();
                        break;
                }
                break;
            case WM_COMMAND: {
                int loword_wparam = LOWORD(wParam);
                string id = m.id_from_loword_wparam(loword_wparam);
                if (id == "?") {
                    win32::shell::exec("https://github.com/aloneguid/clipnest", "");
                }
                else if (id == "x") {
                    ::PostQuitMessage(0);
                } else if (id == "$") {
                    win32::shell::exec("https://github.com/sponsors/aloneguid", "");
                } else {
                    copy_op_result(id);
                }
            }
                break;
            /*case WM_CLIPBOARDUPDATE:
                calculate(sni, m);
                return 0;*/
        }
        return 0;
    };

    win32app.run();

    return 0;
}