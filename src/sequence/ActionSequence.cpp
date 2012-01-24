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
            std::cout  << "Failed to parse configuration\n"
                       << reader.getFormattedErrorMessages();
            return;
        }
        for ( int index = 0; index < _actions.size(); ++index ) {
            std::cout << _actions[index] << std::endl;
        }
    }
    
    std::string ActionSequence::nextAction() {
        if (this->_lastIndex < _actions.size()) {
            return _actions[_lastIndex++].asString();
        }
        
        return "";
    }

}