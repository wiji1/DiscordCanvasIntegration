#ifndef DISCORDCANVASINTEGRATION_TRACKEDCOURSE_H
#define DISCORDCANVASINTEGRATION_TRACKEDCOURSE_H


class TrackedCourse {
public:
    long course_id;
    long announcements_channel;
    long forums_channel;
    long role_id;

    explicit TrackedCourse(long course_id);
};


#endif
