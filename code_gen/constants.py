from utils.definitions import Types, Constant

constants = [
    Constant('KEEP_ALIVE_SEND_RATE_MS', Types.Long, '500'),
    Constant('KEEP_ALIVE_TIMEOUT_MS', Types.Long, 'KEEP_ALIVE_SEND_RATE_MS * 6'),
    Constant('MANUAL_LOOP_BACK_RATE_MS', Types.Long, '100'),
    Constant('SERVER_LIST_MAX_AGE_MS', Types.Long, '6000'),
    Constant('MAX_MESSAGE_SIZE', Types.Int, '128'),
    Constant('AVAILABLE_CLIENT_PORTS', Types.IntArray, [50156, 51156, 52156, 53156]), # ephemeral ports, picked arbitrarily
]
