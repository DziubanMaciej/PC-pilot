from utils.definitions import Message, Field, MessageClass


serverMessage = MessageClass("ServerMessage",
    Message('MoveCursor',
        Field('x', 'float'),
        Field('y', 'float'),
    ),
    Message('LeftPress'),
    Message('LeftRelease'),
)

clientMessage = MessageClass("ClientMessage",
    Message('Connect'),
)

message_classes = [
    serverMessage,
    clientMessage
]