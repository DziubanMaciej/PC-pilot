<%! import utils.definitions as definitions%>\
// This file is autogenerated, do not try to change it manually
package com.paijan.pcpilot.utils

object Constants {
% for constant in constants:
    const val ${constant.name} : ${constant.type.name_kt} = ${constant.value}
% endfor
}
