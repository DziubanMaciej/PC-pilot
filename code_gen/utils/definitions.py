preamble_value = "PCPILOT"


class Type:
    def __init__(self, name, size, name_cpp=None, name_kt=None):
        self.name = name
        self.size = size
        self.name_cpp = name_cpp if name_cpp is not None else name
        self.name_kt = name_kt if name_kt is not None else name.capitalize()


class Types:
    Int = Type('int', 4)
    Float = Type('float', 4)
    Byte = Type('Byte', 1)
    Preamble = Type('Preamble', len(preamble_value))


class Field:
    def __init__(self, field_name, field_type):
        self.name = field_name
        self.type = field_type
        self.offset = -1


class Message:
    def __init__(self, name, *fields):
        fields = list(fields)
        self.name = name
        self.fields = fields
        self.fixed_fields_count = self.insert_fixed_fields()
        self.set_offsets()

    def insert_fixed_fields(self):
        self.fields.insert(0, Field('type', Types.Byte))
        self.fields.insert(1, Field('Preamble', Types.Preamble))
        return 2

    def set_offsets(self):
        offset = 0
        for field in self.fields:
            field.offset = offset
            offset += field.type.size


class MessageClass:
    def __init__(self, name, *messages):
        self.name = name
        self.messages = list(messages)
