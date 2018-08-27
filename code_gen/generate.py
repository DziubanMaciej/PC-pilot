from mako.template import Template
from utils.template_utils import UtilsCpp, UtilsKt
import messages
import constants
import os


def generate(template_filename, output_dir_components, data):
    template_dir = 'templates'
    template_path = os.path.join(template_dir, template_filename)
    template = Template(filename=template_path)

    output_path = os.path.join(*output_dir_components)
    os.makedirs(os.path.dirname(output_path), exist_ok=True)
    with open(output_path, 'w') as file:
        file.write(template.render(**data).replace('\r\n', '\n'))


if __name__ == "__main__":
    cpp_output_base_dir = os.path.join('..', 'server')
    kt_output_base_dir = os.path.join('..', 'client', 'app', 'src', 'main', 'kotlin', 'com', 'paijan', 'pcpilot')

    data = {
        'message_classes' : messages.message_classes,
        'constants': constants.constants,
        'utils': UtilsCpp(),
    }

    generate('messages.h.mako',  [cpp_output_base_dir, 'Communication', 'Messages.h' ], data)
    generate('constants.h.mako', [cpp_output_base_dir, 'Utils',         'Constants.h'], data)

    data['utils'] = UtilsKt()
    generate('messages.kt.mako',  [kt_output_base_dir, 'communication', 'Messages.kt' ], data)
    generate('constants.kt.mako', [kt_output_base_dir,                  'Constants.kt'], data)
