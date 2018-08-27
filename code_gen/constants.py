from utils.definitions import Types, Constant

constants = [
    Constant('KEEP_ALIVE_SEND_RATE_MS', Types.Float, '500.0f'),
    Constant('KEEP_ALIVE_TIMEOUT_MS', Types.Float, 'KEEP_ALIVE_SEND_RATE_MS * 4'),
    Constant('MAX_MESSAGE_SIZE', Types.Int, '128'),
]
