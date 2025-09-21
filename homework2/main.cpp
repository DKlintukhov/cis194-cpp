#include <variant>
#include <string>
#include <cassert>
#include <sstream>

struct Info {
    auto operator<=>(const Info&) const = default;
};
struct Warning {
    auto operator<=>(const Warning&) const = default;
};
struct Error { 
    int code;
    auto operator<=>(const Error&) const = default;
};

using MessageType = std::variant<Info, Warning, Error>;
using Timestamp = int;

struct Message {
    MessageType type;
    Timestamp timestamp;
    std::string message;
    auto operator<=>(const Message&) const = default;
};

struct Unknown {
    std::string message;
    auto operator<=>(const Unknown&) const = default;
};

using LogMessage = std::variant<Message, Unknown>;

LogMessage parse_info(std::stringstream& ss) {
    Timestamp ts;
    std::string message;

    ss >> ts;
    ss.get();
    if (ss.fail() || ss.eof())
        return Unknown{ ss.str() };

    if (!std::getline(ss, message))
        return Unknown{ ss.str() };

    return Message{ Info{}, ts, std::move(message) };
}

LogMessage parse_warning(std::stringstream& ss) {
    Timestamp ts;
    std::string message;

    ss >> ts;
    ss.get();
    
    if (ss.fail() || ss.eof())
        return Unknown{ ss.str() };

    if (!std::getline(ss, message))
        return Unknown{ ss.str() };

    return Message{ Warning{}, ts, std::move(message) };
}

LogMessage parse_error(std::stringstream& ss) {
    int code;
    Timestamp ts;
    std::string message;

    ss >> code;
    ss.get();
    
    if (ss.fail() || ss.eof())
        return Unknown{ ss.str() };

    ss >> ts;
    ss.get();
    if (ss.fail() || ss.eof())
        return Unknown{ ss.str() };

    if (!std::getline(ss, message))
        return Unknown{ ss.str() };

    return Message{ Error{code}, ts, std::move(message) };
}

LogMessage parse_message(std::string message) {
    if (message.empty())
        return Unknown{ std::move(message) };

    std::stringstream ss(message);
    char type;
    ss >> type;
    ss.get();

    if (ss.fail() || ss.eof())
        return Unknown{ std::move(message) };

    
    if (type == 'I')
    {
        return parse_info(ss);
    }

    if (type == 'W')
    {
        return parse_warning(ss);
    }

if (type == 'E')
    {
        return parse_error(ss);
    }

    return Unknown{ std::move(message) };
}

int main(int argc, char *argv[]) {
    assert((parse_message("I 29 la la la") ==
      LogMessage{ Message{ Info{}, 29, "la la la" } }));
    assert((parse_message("W 128 warn warn") ==
      LogMessage{ Message{ Warning{}, 128, "warn warn" } }));
    assert((parse_message("E 2 562 help help") ==
      LogMessage{ Message{ Error{2}, 562, "help help" } }));
    assert((parse_message("This is not in the right format") ==
      LogMessage{ Unknown{ "This is not in the right format" } }));

    return 0;
}
