#include "app.h"

#include <iostream>
#include <cctype>

#include <app/workout.h>

enum class DashboardChoice 
{
    UpdateFromWahoo = 1,
    ManuallyAddWorkout,
    RemoveWorkout,
    ShowWorkouts,
    Quit
};

int App::getDasboardChoice()
{
    std::cout << "-> ";

    std::string inp;
    std::cin.clear(); // Clear any error flags
    std::cin >> inp;

    inp.erase(0, inp.find_first_not_of(' '));
    inp.erase(inp.find_last_not_of(' ') + 1);


    if (std::cin.fail()) 
    {
        std::cout << "  Please enter an option." << std::endl;
        std::cin.clear(); // Clear fail state
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Discard remaining characters
    }

    if (inp == "update")
        return (int) DashboardChoice::UpdateFromWahoo;

    if (inp == "add" || inp == "add workout")
        return (int) DashboardChoice::ManuallyAddWorkout;

    if (inp == "remove" || inp == "remove workout")
        return (int) DashboardChoice::RemoveWorkout;

    if (inp == "show" || inp == "show workout")
        return (int) DashboardChoice::ShowWorkouts;

    if (inp == "quit" || inp == "exit")
        return (int) DashboardChoice::Quit; 

    else 
        std::cout << "  Invalid choice: `" << inp << "`\n";
        
    getDasboardChoice();
}

void App::run() 
{
    std::cout << "Welcome to Ridelog, Noah!\n\n";

    std::cout << "  1. Update from Wahoo API\n";
    std::cout << "  2. Manually add workout\n";
    std::cout << "  3. Remove workout\n";
    std::cout << "  4. Show workouts\n";
    std::cout << "  5. Quit\n\n";

    m_Workouts = Workout::getWorkoutsFromDisk();

    do
    {
        DashboardChoice choice = (DashboardChoice) getDasboardChoice();

        switch (choice)
        {
            case DashboardChoice::UpdateFromWahoo: {
                std::cout << "  Update from Wahoo unimplemented\n";
                break;
            }

            case DashboardChoice::ManuallyAddWorkout: {
                addWorkout();
                break;
            }

            case DashboardChoice::RemoveWorkout: {
                removeWorkout();
                break;
            }

            case DashboardChoice::ShowWorkouts: {
                showWorkouts();
                break;
            }

            case DashboardChoice::Quit: {
                exit(0);
            }
        } 
    } while (true);
}


#define CHECK_MISSING_I(vName, dName)      if (!vName.empty() && std::isdigit(vName[0])) { w.dName = std::stoi(vName); } else { w.addMissing(#dName); }
#define CHECK_MISSING_F(vName, dName)      if (!vName.empty() && std::isdigit(vName[0])) { w.dName = std::stof(vName); } else { w.addMissing(#dName); }
#define CHECK_MISSING_SIZE_T(vName, dName) if (!vName.empty() && std::isdigit(vName[0])) { w.dName = std::stoul(vName); } else { w.addMissing(#dName); }



void App::addWorkout()
{
    Workout w;

    std::cin.clear(); 
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Discard remaining characters


    /// ---- Type ----
    std::string type;
    std::cout << "  Type: ";
    std::getline(std::cin, type);

    w.m_Type = Workout::strToType(type);

    /// ---- Duration ----
    std::string duration;
    std::cout << "  Duration (in seconds): ";
    std::getline(std::cin, duration);

    CHECK_MISSING_SIZE_T(duration, m_Duration);

    /// ---- Date ----
    std::string date;
    std::cout << "  Date (dd/mm/yyyy): ";
    std::getline(std::cin, date);

    if (date.empty())
        w.addMissing("m_Date");

    w.m_Date = date;

    /// ---- Distance (Only for cycling or running) ----
    if (w.m_Type == WorkoutType::Cycling || w.m_Type == WorkoutType::Running) {
        std::string distance;
        std::cout << "  Distance: ";
        std::getline(std::cin, distance);

        CHECK_MISSING_F(distance, m_Distance);
    }

    /// ---- Average Heart Rate ----
    std::string hr;
    std::cout << "  Average Heart Rate: ";
    std::getline(std::cin, hr);

    CHECK_MISSING_I(hr, m_AvgHeartRate);

    /// ---- Burned Calories ----
    std::string burnedCalories;
    std::cout << "  Burned Calories: ";
    std::getline(std::cin, burnedCalories);

    CHECK_MISSING_I(burnedCalories, m_BurnedCalories);


    /// ---- Cycling speed (Only for cycling workouts) ----
    if (w.m_Type == WorkoutType::Cycling) {
        std::string avgSpeed;
        std::string maxSpeed;

        std::cout << "  Average Speed: ";
        std::getline(std::cin, avgSpeed);
        CHECK_MISSING_F(avgSpeed, m_AvgCyclingSpeed);

        std::cout << "  Max Speed: ";
        std::getline(std::cin, maxSpeed);
        CHECK_MISSING_F(maxSpeed, m_MaxCyclingSpeed);

    }

    /// ---- Cycling Power Data (Only for cycling workouts) ----
    if (w.m_Type == WorkoutType::Cycling) {
        std::string avgPower;
        std::string norPower;
        std::string maxPower;

        std::cout << "  Average Power: ";
        std::getline(std::cin, avgPower);
        CHECK_MISSING_I(avgPower, m_AvgPower);


        std::cout << "  Normalized Power: ";
        std::getline(std::cin, norPower);
        CHECK_MISSING_I(norPower, m_NormalizedPower);

        
        std::cout << "  Maximal Power: ";
        std::getline(std::cin, maxPower);
        CHECK_MISSING_I(maxPower, m_MaxPower);

    }

    /// ---- Running Pace Date (Only for running workouts) ----
    if (w.m_Type == WorkoutType::Running) {
        std::string avgPace;

        std::cout << "  Average Pace (0'0\"): ";
        std::getline(std::cin, avgPace);

        if (!avgPace.empty())
            w.m_AvgRunningPace = avgPace;
        else
            w.addMissing("m_AvgRunningPace");

    }

    /// ---- Notes ----
    std::string note;
    std::cout << "  Notes on the training: ";
    std::getline(std::cin, note);

    if (note.empty())
        w.addMissing("m_Note");
    
    w.m_Note = note;
    m_Workouts.push_back(w);

    std::cout << "\n";

    w.saveToDisk();
}

void App::showWorkouts()
{
    std::cout << "\n";

    if (m_Workouts.empty()) {
        std::cout << "  There are no workouts to be shown.\n\n";
        return;
    } 

    for (auto it = m_Workouts.rbegin(); it != m_Workouts.rend(); ++it) {
        it->print();

        // We dont want this line at the bottom, only between workouts
        if (it != m_Workouts.rend() - 1)
            std::cout << "\n  --------------------------\n\n";
        else 
            std::cout << "\n\n";
    }
}


Workout* App::getWorkoutFromList(uuid_t id)
{
    for (auto& w : m_Workouts) {
        if (w.m_ID == id)
            return &w;
    }

    return nullptr;
}


void App::removeWorkoutFromList(uuid_t id)
{
    for (auto it = m_Workouts.begin(); it != m_Workouts.end(); ++it) {
        if (it->m_ID == id) {
            m_Workouts.erase(it);
            break; // Assuming IDs are unique, we can break after removing one
        }
    }
}


void App::removeWorkout() 
{
    std::cin.clear(); 
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Discard remaining characters

    std::cout << "  Enter workout ID: ";
    std::string id;
    std::getline(std::cin, id);

    Workout* workout = getWorkoutFromList(id);
    removeWorkoutFromList(id);

    if (!workout) {
        std::cout << "  Could not remove workout from disk, because it doesn't exist!\n";
        return;
    }

    if (workout->removeWorkoutFromDisk())
        std::cout << "  Succesfully removed workout from disk!\n";
    else
        std::cout << "  Could not remove workout from disk!\n";
}