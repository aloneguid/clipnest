#include "mini_result_popup.h"
#include "global.h"
#include <Windows.h>
#include "win32/clipboard.h"

using namespace grey;
using namespace std;

namespace clipnest {
    mini_result_popup::mini_result_popup(grey::texture_mgr& mgr) : window{ mgr, ProductName, true } {

        auto cmd_calc = make_button("refresh");
        cmd_calc->tooltip = "recalculate from clipboard";
        cmd_calc->on_pressed = [this](button&) {
            calculate();
        };

        tbl = make_complex_table<operation>({"Operation", "Result" });
        calculate();
    }

    unique_ptr<backend> mini_result_popup::show(bool& is_ui_open) {
        auto be = backend::make_platform_default(ProductName);
        auto w = be->make_window<mini_result_popup>();
        is_ui_open = true;
        w->on_open_changed = [&is_ui_open](bool& is_open) {
            if (!is_open) {
                is_ui_open = false;
            }
        };

        int sw = ::GetSystemMetrics(SM_CXFULLSCREEN);
        int sh = ::GetSystemMetrics(SM_CYFULLSCREEN);

        int pw = sw / 3;
        int ph = sh / 3;

        be->resize(pw, ph);
        be->move(sw - pw, sh - ph);
        be->set_visibility(true);
        be->bring_to_top();
        be->set_is_resizeable(false);
        be->set_theme(colour_theme::corp_grey);
        be->set_default_font(font::proggy_clean);

        return be;
    }

    void mini_result_popup::calculate() {
        string input = win32::clipboard::get_text();
        operation::compute_all(input);

        results.clear();
        for (auto& cvec : operation::cat_to_ops) {
            const std::string& cat = cvec.first;

            for (auto& op : cvec.second) {
                if (op->result.empty() || op->result == input) continue;

                results.push_back(op);
            }
        }

        tbl->clear();
        for (auto& ri : results) {
            auto row = tbl->make_row(ri);
            row.cells[0]->make_selectable(ri->name);
            auto v = row.cells[1]->make_selectable(ri->result);
            string fr = ri->result;
            v->on_click = [this, fr](component&) {
                win32::clipboard::set_text(fr);
                calculate();
            };
        }
    }
}