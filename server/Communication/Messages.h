// This file is autogenerated, do not try to change it manually
#pragma once

#include "Communication/InetAddress.h"
#include <vector>

using Byte = unsigned char;

template <typename DerivingClass, unsigned int SIZE>
class Message {
public:
    static void fromBytes(const std::vector<Byte> &bytes, const InetAddress &address, std::vector<DerivingClass> &result) {
        result.clear();

        for (auto i = 0u; i < bytes.size() - SIZE + 1; i += SIZE) {
            if (!validatePreamble(bytes.data(), i)) continue;

            DerivingClass message{ address, bytes.data() + i };
            if (message.validateType()) {
                result.push_back(std::move(message));
            }
        }
    }

    static void toBytes(const std::vector<DerivingClass> &messages, std::vector<Byte> &result) {
        const auto totalSize = messages.size() * SIZE;
        result.resize(totalSize);

        auto index = 0u;
        for (const auto &message : messages) {
            std::memcpy(result.data() + index, message.bytes, SIZE);
            index += SIZE;
        }
    }

    Message(Message&&) = default;
    Message& operator=(Message&&) = default;
protected:
    Byte bytes[SIZE];
    Message() {}
    Message(const Byte *bytes) {
        std::memcpy(this->bytes, bytes, SIZE);
    }
    Message(Message&) = delete;
    Message& operator=(Message&) = delete;

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

    DerivingClass& setPreamble() {
        static const std::string field = "PCPILOT";
        std::memcpy(this->bytes + 0, &field.front(), 7);
        return *static_cast<DerivingClass*>(this);
    }

    static bool validatePreamble(const Byte* bytes, size_t offset) {
        static const char* PREAMBLE = "PCPILOT";
        return std::memcmp(bytes, PREAMBLE, 7) == 0;
    }
};



class ServerMessage : public Message<ServerMessage, 16> {
    friend class Message<ServerMessage, 16>;
    ServerMessage(const InetAddress &address) : address(address) {}
    ServerMessage(const InetAddress &address, const Byte* bytes) : Message(bytes), address(address) {}
//	ServerMessage(ServerMessage&&) = default;
public:
    ServerMessage() = default;
    ServerMessage(ServerMessage&&) = default;
    ServerMessage& operator=(ServerMessage&&) = default;

    InetAddress address;

    enum class Type : Byte {
        ConnectionRequest = 0,
        KeepAlive = 1,
        MoveCursor = 2,
        LeftPress = 3,
        LeftRelease = 4,
        ErrorType = 5
    };

    Type getType() {
        return static_cast<Type>(getField<Byte, 7>());
    }

    bool validateType() {
        return getType() < Type::ErrorType;
    }

    // --- --- --- ConnectionRequest
    static ServerMessage createMessageConnectionRequest(const InetAddress &address) {
        return std::move(ServerMessage(address)
            .setPreamble()
            .setField<Byte, 7>(static_cast<Byte>(Type::ConnectionRequest))
        );
    }

    // --- --- --- KeepAlive
    static ServerMessage createMessageKeepAlive(const InetAddress &address) {
        return std::move(ServerMessage(address)
            .setPreamble()
            .setField<Byte, 7>(static_cast<Byte>(Type::KeepAlive))
        );
    }

    // --- --- --- MoveCursor
    static ServerMessage createMessageMoveCursor(const InetAddress &address, float x, float y) {
        return std::move(ServerMessage(address)
            .setPreamble()
            .setField<Byte, 7>(static_cast<Byte>(Type::MoveCursor))
            .setField<float, 8>(x)
            .setField<float, 12>(y)
        );
    }
    float getMessageMoveCursorX() {
        return getField<float, 8>();
    }
    float getMessageMoveCursorY() {
        return getField<float, 12>();
    }

    // --- --- --- LeftPress
    static ServerMessage createMessageLeftPress(const InetAddress &address) {
        return std::move(ServerMessage(address)
            .setPreamble()
            .setField<Byte, 7>(static_cast<Byte>(Type::LeftPress))
        );
    }

    // --- --- --- LeftRelease
    static ServerMessage createMessageLeftRelease(const InetAddress &address) {
        return std::move(ServerMessage(address)
            .setPreamble()
            .setField<Byte, 7>(static_cast<Byte>(Type::LeftRelease))
        );
    }
};



class ClientMessage : public Message<ClientMessage, 8> {
    friend class Message<ClientMessage, 8>;
    ClientMessage(const InetAddress &address) : address(address) {}
    ClientMessage(const InetAddress &address, const Byte* bytes) : Message(bytes), address(address) {}
public:
    ClientMessage() = default;
    ClientMessage(ClientMessage&&) = default;
    ClientMessage& operator=(ClientMessage&&) = default;

    InetAddress address;

    enum class Type : Byte {
        ConnectionAccept = 0,
        KeepAlive = 1,
        ErrorType = 2
    };

    Type getType() {
        return static_cast<Type>(getField<Byte, 7>());
    }

    bool validateType() {
        return getType() < Type::ErrorType;
    }

    // --- --- --- ConnectionAccept
    static ClientMessage createMessageConnectionAccept(const InetAddress &address) {
        return std::move(ClientMessage(address)
            .setPreamble()
            .setField<Byte, 7>(static_cast<Byte>(Type::ConnectionAccept))
        );
    }

    // --- --- --- KeepAlive
    static ClientMessage createMessageKeepAlive(const InetAddress &address) {
        return std::move(ClientMessage(address)
            .setPreamble()
            .setField<Byte, 7>(static_cast<Byte>(Type::KeepAlive))
        );
    }
};
