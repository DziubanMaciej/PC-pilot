<%! import utils.definitions as definitions%>\
// This file is autogenerated, do not try to change it manually
package com.paijan.pcpilot.utils

object Constants {
% for constant in constants:
    ${utils.get_constant_specifiers(constant.type)}${constant.name} : ${constant.type.name_kt} = ${utils.get_constant_value(constant)}
% endfor
}
