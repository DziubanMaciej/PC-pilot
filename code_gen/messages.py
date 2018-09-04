from utils.definitions import Message, Field, MessageClass, Types


serverMessage = MessageClass("ServerMessage",
    Message('ConnectionRequest'),
    Message('KeepAlive'),

    Message('MoveCursor',
        Field('x', Types.Float),
        Field('y', Types.Float),
    ),
    Message('LeftPress'),
    Message('LeftRelease'),
    Message('RightPress'),
    Message('RightRelease'),
)

clientMessage = MessageClass("ClientMessage",
    Message('ConnectionAccept'),
    Message('KeepAlive'),
)

message_classes = [
    serverMessage,
    clientMessage
]