def trailing_sign(is_last, between, ending):
    return ending if bool(is_last) else between

def get_args_list_cpp(fields):
    result = "const InetAddress &address" + trailing_sign(len(fields) == 2, ', ', '')
    for i, field in enumerate(fields):
        if not is_fixed_field(field):
            separator = ", " if i != len(fields) - 1 else ""
            result += "{} {}{}".format(field.type.name_cpp, field.name, separator)
    return result

def get_args_list_kt(fields):
    result = 'address: InetSocketAddress' + trailing_sign(len(fields) == 2, ', ', '')
    for i, field in enumerate(fields):
        if not is_fixed_field(field):
            separator = ", " if i != len(fields) - 1 else ""
            result += "{}: {}{}".format(field.name, field.type.name_kt, separator)
    return result

def get_put_call_kt(field_type, value):
    if field_type == 'Preamble':
        return ".putPreamble()"

    method_name = 'put' if field_type == 'Byte' else 'put{}'.format(field_type.name_kt)
    return ".{}({})".format(method_name, value)

def get_largest_message_size(message_class):
    sizes = (sum((field.type.size for field in message.fields)) for message in message_class.messages)
    try:
        return max(sizes)
    except ValueError:
        raise RuntimeError("message class with no messages")

def is_fixed_field(field):
    return field.name in ['type', 'Preamble']

def has_additional_fields(message):
    return len(message.fields) == message.fixed_fields_count
