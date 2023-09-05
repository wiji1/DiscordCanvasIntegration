#include "include/TrackedCourse.h"

TrackedCourse::TrackedCourse(long course_id, long category_id, long announcements_channel, long forums_channel, long role_id) :
course_id {course_id}, category_id {category_id}, announcements_channel {announcements_channel}, forums_channel {forums_channel},
role_id {role_id} {

}

TrackedCourse::TrackedCourse() : course_id {0}, category_id {0}, announcements_channel {0}, forums_channel {0}, role_id {0} {

}
