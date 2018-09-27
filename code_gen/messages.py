from utils.definitions import Message, Field, MessageClass, Types


serverMessage = MessageClass("ServerMessage",
    Message('ConnectionRequest'),
    Message('KeepAlive'),
	Message('Disconnect'),

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
    Message('Disconnect'),
    Message('KeepAlive'),
    Message('Advertise'),
)

message_classes = [
    serverMessage,
    clientMessage
]