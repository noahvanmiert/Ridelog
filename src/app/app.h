#pragma once

#include <app/workout.h>

class App 
{
public:
    void run();
    int getDasboardChoice();
    void addWorkout();
    void showWorkouts();

private:
    std::vector<Workout> m_Workouts;
};
