from utils.definitions import Types
import itertools

class Utils:
    @classmethod
    def trailing_sign(cls, is_last, between, ending):
        return ending if bool(is_last) else between

    @classmethod
    def get_comma_separated_list(cls, values):
        values_with_last = list(cls.iter_with_last(values))
        tokens = (str(element) + cls.trailing_sign(last, ', ', '') for last, element in values_with_last)
        return ''.join(tokens)

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
        arg = list(arg)
        booleans = itertools.chain(itertools.repeat(False, len(arg)-1), [True])
        return zip(booleans, arg)

    @classmethod
    def get_constant_value(cls, constant):
        if constant.type != Types.IntArray:
            return constant.value
        return cls._get_constant_value_int_array(constant.value)

    @classmethod
    def _get_constant_value_int_array(cls, constant_value):
        raise NotImplementedError()

    @classmethod
    def get_args_list(cls, fields):
        raise NotImplementedError()

    @classmethod
    def get_constant_specifiers(cls, field_type):
        raise NotImplementedError




class UtilsCpp(Utils):
    @classmethod
    def _get_constant_value_int_array(cls, constant_value):
        return "{{{}}}".format(cls.get_comma_separated_list(constant_value))

    @classmethod
    def get_args_list(cls, fields):
        result = "const InetAddress &address" + cls.trailing_sign(len(fields) == 2, ', ', '')
        params = ('{} {}'.format(field.type.name_cpp, field.name) for field in fields if not cls.is_fixed_field(field))
        return result + cls.get_comma_separated_list(params)

    @classmethod
    def get_constant_specifiers(cls, field_type):
        return "constexpr " if field_type != Types.IntArray else "const "





class UtilsKt(Utils):
    @classmethod
    def _get_constant_value_int_array(cls, constant_value):
        return "listOf({})".format(cls.get_comma_separated_list(constant_value))

    @classmethod
    def get_args_list(cls, fields):
        result = 'address: InetSocketAddress' + cls.trailing_sign(len(fields) == 2, ', ', '')
        params = ('{}: {}'.format(field.name, field.type.name_kt) for field in fields if not cls.is_fixed_field(field))
        return result + cls.get_comma_separated_list(params)

    @classmethod
    def get_constant_specifiers(cls, field_type):
        const = "const " if field_type != Types.IntArray else ""
        val = "val "
        return const + val

    @classmethod
    def get_put_call(cls, field_type, value):
        if field_type == 'Preamble':
            return ".putPreamble()"

        method_name = 'put' if field_type == 'Byte' else 'put{}'.format(field_type.name_kt)
        return ".{}({})".format(method_name, value)
