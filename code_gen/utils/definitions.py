preamble_value = "PCPILOT"


class Type:
    def __init__(self, size, name=None, name_cpp=None, name_kt=None):
        self.size = size
        self.name_cpp = name_cpp if name_cpp is not None else name
        self.name_kt = name_kt if name_kt is not None else name.capitalize()

        if not self.name_cpp or not self.name_kt:
            raise Exception("No type name provided")


class Types:
    Int = Type(4,name='int')
    Long = Type(-1, name='long')
    Float = Type(4, name='float')
    Byte = Type(1, name='Byte')
    Preamble = Type(len(preamble_value), name='Preamble')
    IntArray = Type(-1, name_cpp="std::vector<int>", name_kt="List<Int>")

class Field:
    def __init__(self, field_name, field_type):
        if field_type.size <= 0:
            raise Exception("Type is not meant to be used in Field")

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


class Constant:
    def __init__(self, name, constant_type, value):
        self.name = name
        self.type = constant_type
        self.value = value
