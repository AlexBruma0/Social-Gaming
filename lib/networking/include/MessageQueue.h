#include <vector>
#include <string>
#include <string_view>

class MessageQueue{
    private:
    std::vector<std::string> messages;
    public:
    MessageQueue& operator=(const MessageQueue& mq);
    int add(std::string s);
    std::string remove();
    std::string getMessageFromID(int id);
};