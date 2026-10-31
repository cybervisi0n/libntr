#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include "common.h"

namespace fs = std::filesystem;
using std::cout;
using std::endl;
using std::pair;
using std::string;
using std::vector;
using std::exception;


int SIM_ROMReorder(string languagesfolder) {
    string lang = "";
    vector<string> languages = { "spa", "fra", "ita", "ger" };
    fs::path langfolder;

    for (const string& flang : languages) {
        langfolder = languagesfolder + "/" + "resource" + "/" + flang;
        if (fs::exists(langfolder) && fs::is_directory(langfolder)) {
            lang = flang;
            break;
        }
    }

    if (lang == "") {
        cout << DINFO << "No other language folder found. ROM might already be ordered or is US." << endl;
        return 0;
    }

    cout << DINFO << "Language ROM structure detected (" << lang << "). Reordering files..." << endl;

    vector<pair<string, string>> LangList = {
        { "resource/" + lang + "/bag/bag_gra.narc", "graphic/bag_gra.narc" },
        { "resource/" + lang + "/bag/pl_bag_gra.narc", "graphic/pl_bag_gra.narc" },
        { "resource/" + lang + "/battle_graphic/batt_obj.narc", "battle/graphic/batt_obj.narc" },
        { "resource/" + lang + "/battle_graphic/pl_batt_obj.narc", "battle/graphic/pl_batt_obj.narc" },
        { "resource/" + lang + "/b_plist/b_plist_gra.narc", "battle/graphic/b_plist_gra.narc" },
        { "resource/" + lang + "/b_plist/pl_b_plist_gra.narc", "battle/graphic/pl_b_plist_gra.narc" },
        { "resource/" + lang + "/vs_demo/vs_demo_gra.narc", "battle/graphic/vs_demo_gra.narc" },
        { "resource/" + lang + "/contest_graphic/contest_bg.narc", "contest/graphic/contest_bg.narc" },
        { "resource/" + lang + "/contest_graphic/contest_obj.narc", "contest/graphic/contest_obj.narc" },
        { "resource/" + lang + "/nameinput/namein.narc", "data/namein.narc" },
        { "resource/" + lang + "/opening_demo/op_demo.narc", "demo/title/op_demo.narc" },
        { "resource/" + lang + "/slot/slot.narc", "data/slot.narc" },
        { "resource/" + lang + "/title/titledemo.narc", "demo/title/titledemo.narc" },
        { "resource/" + lang + "/pokemon_trade/fld_trade.narc", "fielddata/pokemon_trade/fld_trade.narc" },
        { "resource/" + lang + "/box/box.narc", "graphic/box.narc" },
        { "resource/" + lang + "/dress_up_graphic/imageclip.narc", "graphic/imageclip.narc" },
        { "resource/" + lang + "/mysterycard/mystery.narc", "graphic/mystery.narc" },
        { "resource/" + lang + "/nutmixer/nutmixer.narc", "graphic/nutmixer.narc" },
        { "resource/" + lang + "/pokelist/plist_gra.narc", "graphic/plist_gra.narc" },
        { "resource/" + lang + "/pokelist/pl_plist_gra.narc", "graphic/pl_plist_gra.narc" },
        { "resource/" + lang + "/p_status/pl_pst_gra.narc", "graphic/pl_pst_gra.narc" },
        { "resource/" + lang + "/p_status/pst_gra.narc", "graphic/pst_gra.narc" },
        { "resource/" + lang + "/poketch/poketch.narc", "graphic/poketch.narc" },
        { "resource/" + lang + "/touch_subwindow/touch_subwindow.narc", "graphic/touch_subwindow.narc" },
        { "resource/" + lang + "/trainer_case/trainer_case.narc", "graphic/trainer_case.narc" },

        { "resource/" + lang + "/batt_rec/batt_rec_gra.narc", "resource/eng/batt_rec/batt_rec_gra.narc" },
        { "resource/" + lang + "/frontier_graphic/frontier_bg.narc", "resource/eng/frontier_graphic/frontier_bg.narc" },
        { "resource/" + lang + "/frontier_graphic/frontier_obj.narc", "resource/eng/frontier_graphic/frontier_obj.narc" },
        { "resource/" + lang + "/pms_aikotoba/pms_aikotoba.narc", "resource/eng/pms_aikotoba/pms_aikotoba.narc" },
        { "resource/" + lang + "/scratch/scratch.narc", "resource/eng/scratch/scratch.narc" },
        { "resource/" + lang + "/wifi_lobby_minigame/wlmngm_tool.narc", "resource/eng/wifi_lobby_minigame/wlmngm_tool.narc" },
        { "resource/" + lang + "/zukan/zukan.narc", "resource/eng/zukan/zukan.narc" }
    };

    for (unsigned i = 0; i < LangList.size(); i++) {
        fs::path langsource = languagesfolder + "/" + LangList[i].first;
        fs::path langdest = languagesfolder + "/" + LangList[i].second;

        if (fs::exists(langsource)) {
            fs::path directdest = langdest.parent_path();
            if (!fs::exists(directdest)) {
                try {
                    fs::create_directories(directdest);
                } catch (exception &e) {
                    cout << DERROR << "Failed to create directory " << directdest.string() << "\n"
                         << e.what() << endl;
                    return -1;
                }
            }

            try {
                fs::rename(langsource, langdest);
            } catch (exception &e) {
                cout << DERROR << "Failed to move file " << langsource.string() << " to " << langdest.string() << "\n"
                     << e.what() << endl;
                return -1;
            }
        } else {
            cout << DWARNING << "Source file missing during reorder: " << langsource.string() << endl;
        }
    }

    cout << DINFO << "Cleaning up resource/" << lang << " directory" << endl;
    try {
        fs::remove_all(langfolder);
    } catch (exception &e) {
        cout << DWARNING << "Could not fully remove" << langfolder.string() << "\n"
             << e.what() << endl;
    }

    cout << DINFO << "Reordering complete, initializing the game." << endl;
    return 0;
}
