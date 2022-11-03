#include "mini_result_popup.h"
#include "global.h"
#include <Windows.h>

using namespace grey;
using namespace std;

namespace clipnest {
    mini_result_popup::mini_result_popup(grey::texture_mgr& mgr) : window{ mgr, ProductName, true } {
        
    }

    unique_ptr<backend> mini_result_popup::show() {
        auto be = backend::make_platform_default(ProductName);
        be->make_window<mini_result_popup>();

        int sw = ::GetSystemMetrics(SM_CXFULLSCREEN);
        int sh = ::GetSystemMetrics(SM_CYFULLSCREEN);

        int pw = 300;
        int ph = 100;

        be->resize(pw, ph);
        be->move(sw - pw, sh - ph);
        be->set_visibility(true);
        be->bring_to_top();
        be->set_is_resizeable(false);

        return be;
    }
}