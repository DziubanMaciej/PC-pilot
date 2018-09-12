from utils.definitions import Types
import itertools

class Utils:
    @classmethod
    def trailing_sign(cls, is_last, between, ending):
        return ending if bool(is_last) else between

    @classmethod
    def is_fixed_field(cls, field):
        return field.name in ['type', 'Preamble']

    @classmethod
    def get_largest_message_size(cls, message_class):
        sizes = (sum((field.type.size for field in message.fields)) for message in message_class.messages)
        try:
            return max(sizes)
        except ValueError:
            raise RuntimeError("message class with no messages")

    @classmethod
    def iter_with_last(cls, arg):
        booleans = itertools.chain(itertools.repeat(False, len(arg)-1), [True])
        return zip(booleans, arg)

    @classmethod
    def get_args_list(cls, fields):
        raise NotImplementedError()

    @classmethod
    def get_constant_value(cls, constant):
        if constant.type != Types.IntArray:
            return constant.value
        return cls._get_constant_value_int_array(constant.value)

    @classmethod
    def _get_constant_value_int_array(cls, constant_value):
        raise NotImplementedError()




class UtilsCpp(Utils):
    @classmethod
    def get_args_list(cls, fields):
        result = "const InetAddress &address" + cls.trailing_sign(len(fields) == 2, ', ', '')
        for i, field in enumerate(fields):
            if not cls.is_fixed_field(field):
                separator = ", " if i != len(fields) - 1 else ""
                result += "{} {}{}".format(field.type.name_cpp, field.name, separator)
        return result

    @classmethod
    def _get_constant_value_int_array(cls, constant_value):
        result = "{"
        for last, element in cls.iter_with_last(constant_value):
            result += str(element)
            if not last:
                result += ", "
        result += "}"
        return result




class UtilsKt(Utils):
    @classmethod
    def get_args_list(cls, fields):
        result = 'address: InetSocketAddress' + cls.trailing_sign(len(fields) == 2, ', ', '')
        for i, field in enumerate(fields):
            if not cls.is_fixed_field(field):
                separator = ", " if i != len(fields) - 1 else ""
                result += "{}: {}{}".format(field.name, field.type.name_kt, separator)
        return result

    @classmethod
    def _get_constant_value_int_array(cls, constant_value):
        result = "listOf("
        for last, element in cls.iter_with_last(constant_value):
            result += str(element)
            if not last:
                result += ", "
        result += ")"
        return result

    @classmethod
    def get_put_call(cls, field_type, value):
        if field_type == 'Preamble':
            return ".putPreamble()"

        method_name = 'put' if field_type == 'Byte' else 'put{}'.format(field_type.name_kt)
        return ".{}({})".format(method_name, value)

    @classmethod
    def get_constant_specifiers(cls, field_type):
        const = "const " if field_type != Types.IntArray else ""
        val = "val "
        return const + val
