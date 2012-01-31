#ifndef ACTION_SEQUENCE_CPP
#define ACTION_SEQUENCE_CPP

#include "ActionSequence.hpp"
#include <fstream>

namespace sequence {
    ActionSequence::ActionSequence() {};
    ActionSequence::ActionSequence(const char *filename)  {
        _lastIndex = 0;
        Json::Reader reader;
        std::fstream json_stream;
        json_stream.open(filename);
        bool parsingSuccessful = reader.parse( json_stream, _actions );
        if ( !parsingSuccessful )
        {
            // report to the user the failure and their locations in the document.
            std::cout  << "Failed to parse  sequence input file\n"
                       << reader.getFormattedErrorMessages();
            return;
        }
    }
    
    std::string ActionSequence::nextAction() {
        if (this->_lastIndex < _actions.size()) {
            return _actions[_lastIndex++].asString();
        }
        
        return "stop";
    }

}

#endif