#include "include/TrackedCourse.h"

TrackedCourse::TrackedCourse(long course_id) : course_id {course_id} {
    announcements_channel = {0};
    forums_channel = {0};
    role_id = {0};
}
