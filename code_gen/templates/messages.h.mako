<%! import utils.template_utils as utils %>\

template <typename DerivingClass, unsigned int SIZE>
class Message {
public:
    using Byte = unsigned char;

protected:
    Byte bytes[SIZE];
    Message() {}
    Message(Byte *bytes) {
        std::memcpy(this->bytes, bytes, SIZE);
    }

    template <typename T, int offset>
    T getField() {
        static_assert(offset + sizeof(T) - 1 < SIZE, "invalid offset");
        T result;
        std::memcpy(&result, this->bytes + offset, sizeof(T));
        return result;
    }

    template <typename T, int offset>
    DerivingClass& setField(T field) {
        static_assert(offset + sizeof(T) - 1 < SIZE, "invalid offset");
        std::memcpy(this->bytes + offset, &field, sizeof(T));
        return *static_cast<DerivingClass*>(this);
    }
};

%for message_class in message_classes:

class ${message_class.name} : public Message<${message_class.name}, ${utils.get_largest_message_size(message_class)}> {
public:
    enum class Type : Byte {
    % for index, message in enumerate(message_class.messages):
        ${message.name} = ${index}${utils.trailing_sign(index + 1 == len(message_class.messages), ',', '')}
    % endfor
    };

    Type getType() {
        return static_cast<Type>(getField<Byte, 0>());
    }
    % for message in message_class.messages:

    // --- --- --- ${message.name}
    static ${message_class.name} createMessage${message.name}(${utils.get_args_list_cpp(message.fields)}) {
        return ${message_class.name}()
            .setField<Byte, 0>(static_cast<Byte>(Type::${message.name}))${';' if len(message.fields) == 1 else ''}
        % for field in message.fields:
        % if not utils.is_type_field(field):
            .setField<${field.type}, ${field.offset}>(${field.name})${utils.trailing_sign(loop.last, '', ';')}
        % endif
        % endfor
    }
    %for field in message.fields:
    % if not utils.is_type_field(field):
    ${field.type} getMessage${message.name}${field.name.title()}() {
        return getField<${field.type}, ${field.offset}>();
    }
    % endif
    % endfor
    % endfor
};


%endfor
