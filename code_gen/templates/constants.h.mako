<%! import utils.definitions as definitions%>\
// This file is autogenerated, do not try to change it manually
#pragma once

namespace Constants {
% for constant in constants:
    const ${constant.type.name_cpp} ${constant.name} = ${constant.value};
% endfor
}