def trailing_sign(is_last, between, ending):
    return ending if bool(is_last) else between

def get_args_list_cpp(fields):
    result = ""
    for i, field in enumerate(fields):
        if not is_type_field(field):
            separator = ", " if i != len(fields) - 1 else ""
            result += "{} {}{}".format(field.type, field.name, separator)
    return result

def get_largest_message_size(message_class):
    sizes = (sum((field.size for field in message.fields)) for message in message_class.messages)
    try:
        return max(sizes)
    except ValueError:
        raise RuntimeError("message class with no messages")

def is_type_field(field):
    r = field.name == 'type'
    return field.name == 'type'