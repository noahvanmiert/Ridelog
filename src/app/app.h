#pragma once

#include <app/workout.h>

class App 
{
public:
    void run();
    int getDasboardChoice();
    void addWorkout();
    void removeWorkout();
    void showWorkouts();
    Workout* getWorkoutFromList(uuid_t id);
    void removeWorkoutFromList(uuid_t id);

private:
    std::vector<Workout> m_Workouts;
};
