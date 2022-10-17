#include <string>
#include <Windows.h>

#include "win32/app.h"
#include "win32/shell.h"
#include "win32/shell_notify_icon.h"
#include "win32/popup_menu.h"
#include "win32/clipboard.h"

#include "str.h"
#include "fmt/core.h"

#include "global.h"
#include "operation.h"

#define OWN_WM_NOTIFY_ICON_MESSAGE WM_APP + 1

const size_t MAX_MI_LENGTH{ 20 };

using namespace std;
using namespace clipnest;

string mi_txt(const std::string& s) {
    string r = s;
    if (r.length() > MAX_MI_LENGTH) {
        r = fmt::format("{}... ({})", r.substr(0, MAX_MI_LENGTH), r.length());
    }
    return r;
}

void calculate(win32::shell_notify_icon& sni, win32::popup_menu& m) {
    string input = win32::clipboard::get_text();

    m.clear();

    if (input.empty()) {
        m.add("input", "clipboard empty", true);
    } else {
        m.add("input", mi_txt(input), true);

        m.separator();

        operation::compute_all(input);

        for (auto& cvec : operation::cat_to_ops) {
            const std::string& cat = cvec.first;

            if (!cat.empty()) m.enter_submenu(cat);

            for (auto& op : cvec.second) {
                string title = fmt::format("{}: {}", op->name, mi_txt(op->result));
                m.add(op->id, title);
            }

            if (!cat.empty()) m.exit_submenu();
        }
    }

    m.separator();
    m.add("?", fmt::format("&About {} {}", ProductName, Version));
    m.add("x", "&Exit");
}

void copy_op_result(const std::string& op_id) {
    auto it = operation::all.find(op_id);
    if (it == operation::all.end()) return;
    auto op = (*it).second;
    win32::clipboard::set_text(op->result);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {

    clipnest::operation::init();

    win32::app win32app;
    win32app.add_clipboard_listener();

    win32::popup_menu m{ win32app.get_hwnd() };

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
                if (id == "?") {
                    win32::shell::exec("https://www.aloneguid.uk/projects/clipnest/", "");
                }
                if (id == "x") {
                    ::PostQuitMessage(0);
                } else {
                    copy_op_result(id);
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