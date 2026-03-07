// include/ActivityRecognizer.h

#pragma once

class ActivityRecognizer {
public:
    enum Activity {
        NONE = 0,
        ACTIVITY = 1
    };

    ActivityRecognizer(int window);
    Activity detect(int motion);

private:
    void push(int value);
    int sum() const;

    int* buf;
    int window;
    int idx;
    int filled;
};
