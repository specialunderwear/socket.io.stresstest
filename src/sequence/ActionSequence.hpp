#ifndef ACTION_SEQUENCE
#define ACTION_SEQUENCE
#include <string>
#include <json/json.h>

namespace sequence {
    class ActionSequence {
        unsigned int _lastIndex;
        Json::Value _actions;
    public:
        ActionSequence();
        explicit ActionSequence(const char *filename);
        std::string nextAction();
    };
}
#endif