from utils.definitions import Message, Field, MessageClass


serverMessage = MessageClass("ServerMessage",
    Message('ConnectionRequest'),
    Message('KeepAlive'),

    Message('MoveCursor',
        Field('x', 'float'),
        Field('y', 'float'),
    ),
    Message('LeftPress'),
    Message('LeftRelease'),
)

clientMessage = MessageClass("ClientMessage",
    Message('ConnectionAccept'),
    Message('KeepAlive'),
)

message_classes = [
    serverMessage,
    clientMessage
]