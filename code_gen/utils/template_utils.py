def trailing_sign(is_last, between, ending):
    return ending if bool(is_last) else between

def get_args_list_cpp(fields):
    result = ""
    for i, field in enumerate(fields):
        if not is_fixed_field(field):
            separator = ", " if i != len(fields) - 1 else ""
            result += "{} {}{}".format(field.type, field.name, separator)
    return result

def get_args_list_kt(fields):
    result = ""
    for i, field in enumerate(fields):
        if not is_fixed_field(field):
            separator = ", " if i != len(fields) - 1 else ""
            result += "{}: {}{}".format(field.name, translate_type_name_kt(field.type), separator)
    return result

def get_put_call_kt(type_name, value):
    if type_name == 'Preamble':
        return ".putPreamble()"

    method_name = 'put' if type_name == 'Byte' else 'put{}'.format(type_name.capitalize())
    return ".{}({})".format(method_name, value)

def translate_type_name_kt(type_name):
    return type_name.capitalize()


def get_largest_message_size(message_class):
    sizes = (sum((field.size for field in message.fields)) for message in message_class.messages)
    try:
        return max(sizes)
    except ValueError:
        raise RuntimeError("message class with no messages")

def is_fixed_field(field):
    return field.name in ['type', 'Preamble']
