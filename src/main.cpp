#include "syntax_analysis_LR0.hpp"
#include <iostream>
#include "debug.hpp"
#include <FlexLexer.h>
#include "json.hpp"
#include <string>
#include <stdexcept>
#include "maketable.hpp"
int main(int argc, char* argv[]) {
    bool is_json = false;
    for (int i = 0; i < argc; ++i) {
        if (std::string(argv[i]) == "json") {
            is_json = true;
        }
    }
    using namespace syntax_analysis;
    SyntaxAnalizer<Grammar::LR0> An;
    nlohmann::json JSON;
    size_t nlines = 0;

if (!is_json) {
    tableHeaderGenerate();
}
    try {
    do {
        if (is_json) {
            JSON[std::to_string(nlines)] = {{"ACTION", An.last_action()},
                                            {"INPUT",  An.input_dump()},
                                            {"STACK",  An.stack_dump()}};
            ++nlines;
        } else {
            tableLineGenerate(An);
        }
    } while (An.step() == false);
    } catch (std::invalid_argument x) {
        if (is_json)
            JSON["exception"] = x.what();
        else throw;
    } catch (std::logic_error x) {
        if (is_json)
            JSON["exception"] = x.what();
        else throw;
    }
if (is_json) {
    JSON["nlines"] = nlines;
    std::cout << JSON.dump();
} else {
    tableBottomGenerate();
}
}
