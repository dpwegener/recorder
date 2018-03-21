#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

enum class EventType {
    MEET,
    INTRODUCE,
    IGNORE
};
// Something interesting
class Event {
public:
    Event(EventType id, std::string details) : id_(id), details_(std::move(details)) {}
    const EventType id_;
    const std::string details_;
};

using NewName = std::function<void (std::string const & name)>;
using Introduction = std::function<void (std::string const & stranger, std::string const & acquantience)>;
// Record event details
class Recorder {
public:
    Recorder() : reuseString_(true), previousDataPtr_(pendingRecords_.data()) {
        logStats();
    }
    void recordNewName(std::string const & name) {
        pendingRecords_.append("I now know ")
                       .append(name)
                       .append("\n");
        logStats();
    }

    void recordIntroduction(std::string const & stranger, std::string const & acquantience) {
        pendingRecords_.append(acquantience)
                       .append(" just met ")
                       .append(stranger)
                       .append("\n");

        logStats();
    }

    void logStats() {
        std::cout << "PendingRecords";
        if (pendingRecords_.data() != previousDataPtr_) {
            std::cout << " data changed from: " 
                      << previousDataPtr_ << " to: " 
                      << static_cast<void const *>(pendingRecords_.data());
            previousDataPtr_ = pendingRecords_.data();
        }
        std::cout << " size: " << pendingRecords_.size();
        std::cout << " capacity: " << pendingRecords_.capacity() << '\n';
    }

    bool hasPendingRecords() {return pendingRecords_.size();}

    void retrieveRecords(std::string & queue) {
        if (reuseString_) {
            queue.append(pendingRecords_);
            pendingRecords_.clear();
            logStats();
        }
        else {
            queue = std::move(pendingRecords_);
            logStats();
        }
    }

    bool reuseString_;
private:
    std::string pendingRecords_;
    void const * previousDataPtr_;
};

struct RecorderCallbacks {
    NewName newName_;
    Introduction introduction_;
};

// Handle events
class Service {
public:
    Service(RecorderCallbacks const & callbacks) : callbacks_(callbacks) {}

    void handleOne(Event const & event) {
        friends_.push_back(event.details_);
        std::cout << "Pleasure to meet you " << event.details_ << '\n';
        //recorder_.recordNewName(event.details_);
        callbacks_.newName_(event.details_);
    }

    void handleTwo(Event const & event) {
        std::cout << event.details_ << ", please meet my friends\n";

        for (auto name: friends_) {
            callbacks_.introduction_(event.details_, name);
        }
    }

    void handleThree(Event const & event) {
        std::cout << "I will ignore " << event.details_ << '\n';
    }


private:
    RecorderCallbacks const & callbacks_;
    std::vector<std::string> friends_;
};

// Invoke Service to process events
// Publish what Recorder captures
class Processor {
public:
    Processor(Recorder & recorder, Service & service) : recorder_(recorder), service_(service) {}

    void process(Event const & event) {
        switch (event.id_) {
            case EventType::MEET :
                service_.handleOne(event);
                break;
            case EventType::INTRODUCE :
                service_.handleTwo(event);
                break;
            case EventType::IGNORE :
                service_.handleThree(event);
                break;
        }
        publishPendingRecords();

    }

    void publishPendingRecords() {
        if (recorder_.hasPendingRecords()) {
            std::string recordsToPublish;
            recorder_.retrieveRecords(recordsToPublish);
            std::cout << "Publishing pending events:\n";
            std::cout << recordsToPublish << '\n';
        }
        else {
            std::cout << "Nothing to publish at this time\n";
        }
    }

private:
    Recorder & recorder_;
    Service & service_;
};

int main(int, char**) {
    std::cout << "Hello, world!\n";
    Recorder theRecorder;
    RecorderCallbacks callbacks;
    callbacks.newName_ =  [&theRecorder] (std::string const & name){theRecorder.recordNewName(name);};
    callbacks.introduction_ = [&theRecorder] (std::string const & stranger, std::string const & acquantience) {
        theRecorder.recordIntroduction(stranger, acquantience);
    };
    Service aService(callbacks);
    Processor theProcessor(theRecorder, aService);

    std::vector<Event> eventList;
    eventList.emplace_back(EventType::MEET, "Bob");
    eventList.emplace_back(EventType::MEET, "Doug");
    eventList.emplace_back(EventType::INTRODUCE, "Hosehead");
    eventList.emplace_back(EventType::IGNORE, "Someone I don't know");

    for (auto event: eventList) {
        theProcessor.process(event);
    }

    theProcessor.process(Event(EventType::MEET, "Friend1"));
    theProcessor.process(Event(EventType::MEET, "Friend2"));
    theProcessor.process(Event(EventType::MEET, "Friend3"));

    Event stranger(EventType::INTRODUCE, "Stranger");
    theProcessor.process(stranger);

    std::cout << "Reusing String \n";
    for(int i = 0; i < 3; i++) {
        theProcessor.process(stranger);
    }

    std::cout << "Don't reuse String\n";
    theRecorder.reuseString_ = false;
    for(int i = 0; i < 3; i++) {
        theProcessor.process(stranger);
    }

    theProcessor.process(Event(EventType::MEET, "Him"));
    theProcessor.process(Event(EventType::MEET, "Them"));
}
