#include <vector>
#include <string>
#include <string_view>

class MessageQueue{
    private:
    std::vector<std::string> messages;
    public:
    MessageQueue& operator=(const MessageQueue& mq);    void add(std::string s);
    std::string remove();
};