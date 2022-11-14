#include "mini_result_popup.h"
#include "global.h"
#include <Windows.h>
#include <fmt/core.h>
#include "win32/clipboard.h"
#include "win32/shell.h"
#include "str.h"

using namespace grey;
using namespace std;

namespace clipnest {
    mini_result_popup::mini_result_popup(grey::texture_mgr& mgr, alg::tracker& t) 
        : t{ t }, window { mgr, ProductName, true } {

        auto cmd_calc = make_button("refresh");
        cmd_calc->set_emphasis(emphasis::primary);
        cmd_calc->tooltip = "recalculate from clipboard";
        cmd_calc->on_pressed = [this](button&) {
            calculate();
        };

        if (!LatestVersion.empty() && LatestVersion != Version) {
            same_line();
            auto cmd_upgrade = make_button(fmt::format("update to {}", LatestVersion));
            cmd_upgrade->tooltip = "new version available!";
            cmd_upgrade->set_emphasis(emphasis::warning);
            cmd_upgrade->on_pressed = [](button&) {
                win32::shell::exec(LatestVersionUrl, "");
            };
        }

        auto h1 = make_label("input: ");
        h1->set_emphasis(emphasis::primary);
        same_line();
        lbl_input = make_label("");

        tbl = make_complex_table<operation>({"Operation", "Result" });
        calculate();
    }

    unique_ptr<backend> mini_result_popup::show(bool& is_ui_open, alg::tracker& t) {
        auto be = backend::make_platform_default(ProductName);
        auto w = be->make_window<mini_result_popup>(t);
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
        // get best available text
        {
            string lis = win32::clipboard::get_text();
            if (lis.empty()) lis = win32::clipboard::get_filename();
            last_input = operation_input{ lis };
        }

        t.track(map<string, string> {
            { "event", "calc" },
            { "len", std::to_string(last_input.value.size()) }
        }, false);

        if (last_input.value.empty()) {
            lbl_input->set_value("empty clipboard");
            lbl_input->set_emphasis(emphasis::warning);
            lbl_input->is_enabled = true;
        } else {
            lbl_input->set_value(ellipse(last_input.value));
            lbl_input->set_emphasis(emphasis::none);
            lbl_input->is_enabled = false;
        }

        operation::compute_all(last_input);

        results.clear();
        for (auto& cvec : operation::cat_to_ops) {
            const std::string& cat = cvec.first;

            for (auto& op : cvec.second) {
                if ((!op->is_dirty && op->result.empty()) || op->result == last_input.value) continue;

                results.push_back(op);
            }
        }

        tbl->clear();
        for (auto ri : results) {
            auto row = tbl->make_row(ri);
            row.cells[0]->make_label(ri->name);
            make_result_cell(row.cells[1], ri);
        }
    }

    std::string mini_result_popup::ellipse(const std::string& s, size_t max) {
        string r = s;
        str::replace_all(r, "\r", "");
        str::replace_all(r, "\n", "");
        str::trim(r);

        if (r.length() < max) return r;

        return fmt::format("{}... ({})", r.substr(0, max), s.length());
    }

    void mini_result_popup::make_result_cell(std::shared_ptr<grey::table_cell> cell, operation::sop op) {
        if (op->is_dirty) {
            auto cc = cell->make_button("calc", true);
            cc->set_emphasis(emphasis::primary);
            cc->on_pressed = [this, op, cell](button&) {
                if (!cell->empty()) {
                    cell->get_child(0)->is_visible = false;
                }
                try {
                    op->compute(last_input);
                    op->is_dirty = false;
                }
                catch (...) {

                }
                make_result_cell(cell, op);
            };
        } else {
            auto v = cell->make_selectable(op->result);
            v->on_click = [this, op](component&) {
                win32::clipboard::set_ascii_text(op->result);

                t.track(map<string, string> {
                    { "event", "copy_result" },
                    { "len", std::to_string(op->result.size()) },
                    { "op", op->id }
                }, false);

                calculate();
            };
        }
    }
}