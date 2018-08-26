preamble_value = "PCPILOT"

types = [
    ('int', 4),
    ('float', 4),
    ('bool', 1),
    ('char', 1),
    ('Byte', 1),
    ('Preamble', len(preamble_value)),
]

def get_type_size(type_name):
    try:
        return (x for x in types if x[0] == type_name).__next__()[1]
    except StopIteration:
        return None

class Field:
    def __init__(self, field_name, field_type):
        self.name = field_name
        self.type = field_type
        self.size = get_type_size(field_type)
        self.offset = -1

class Message:
    def __init__(self, name, *fields):
        fields = list(fields)
        self.name = name
        self.fields = fields
        self.insert_fixed_fields()
        self.set_offsets()


    def insert_fixed_fields(self):
        self.fields.insert(0, Field('type', 'Byte'))
        self.fields.insert(1, Field('Preamble', 'Preamble'))

    def set_offsets(self):
        offset = 0
        for field in self.fields:
            field.offset = offset
            offset += field.size

class MessageClass:
    def __init__(self, name, *messages):
        self.name = name
        self.messages = list(messages)
