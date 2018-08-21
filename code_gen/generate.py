from mako.template import Template
import messages
import os

def generate(template_filename, output_dir, output_filename, data):
    template_dir = 'templates'
    template_path = os.path.join(template_dir, template_filename)
    template = Template(filename=template_path)

    os.makedirs(output_dir, exist_ok=True)
    output_path = os.path.join(output_dir, output_filename)
    with open(output_path, 'w') as file:
        file.write(template.render(**data).replace('\r\n', '\n'))

if __name__ == "__main__":
    data = { 'message_classes' : messages.message_classes}
    generate('messages.h.mako', 'out', 'messages.h', data)