#include "workout.h"

#include <iostream>
#include <string>
#include <iomanip>
#include <fstream>
#include <random>
#include <sstream>


#define SAVE_FILE "ridelog-data.rls"


uuid_t Workout::genUUID()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);

    const char* hexChars = "01234567890abcdef";
    std::string uuid = "xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxx";

    for (char& c : uuid) {
        if (c == 'x')
            c = hexChars[dis(gen)];
        else if (c == 'y')
            c = hexChars[(dis(gen) & 0x3) | 0x8]; // Set high nibble to 4, seconds nible to random value in [8, B, C, D]
    }

    return uuid;
}


Workout::Workout()
{
    m_ID = genUUID();
}


Workout::Workout(WorkoutType type, size_t duration, std::string date)
{
    m_ID = genUUID();
    m_Type = type;
    m_Duration = duration;
    m_Date = date;
}


static std::string typeToStr(WorkoutType type) 
{
    switch (type)
    {
    case WorkoutType::Running: return "Running";
    case WorkoutType::Cycling: return "Cycling";
    case WorkoutType::None:    return "None";
    
    default: return "Unkown";
    }
}


WorkoutType Workout::strToType(std::string str)
{
    if (str == "Running" || str == "running")
        return WorkoutType::Running;

    if (str == "Cycling" || str == "Cycling")
        return WorkoutType::Cycling;

    return WorkoutType::None;
}


std::string Workout::durationToTimeFormat() 
{
    int hours = m_Duration / 3600;
    int minutes = (m_Duration % 3600) / 60;
    int remainingSeconds = m_Duration % 60;

    return std::to_string(hours) + "h " + std::to_string(minutes) + "m " + std::to_string(remainingSeconds) + "s";
}


#define ITALIC "\033[3m"
#define BOLD "\033[1m"
#define ANSI_END "\033[0m"

std::string Workout::getStrDataIfNotMissing(const std::string& varName, std::string value)
{
    if (isMissing(varName)) 
        return "n/a";

    return value;
}


std::string Workout::getStrDataIfNotMissing(const std::string& varName, float value)
{
    if (isMissing(varName)) 
        return "n/a";

    return std::to_string(value);
}


std::string Workout::getStrDataIfNotMissing(const std::string& varName, int value)
{
    if (isMissing(varName)) 
        return "n/a";

    return std::to_string(value);
}


std::string Workout::getStrDataIfNotMissing(const std::string& varName, size_t value)
{
    if (isMissing(varName)) 
        return "n/a";

    return std::to_string(value);
}


#define GET_IF_NOT_MISSING(varName, suffix) Workout::getStrDataIfNotMissing(#varName, varName) << (!isMissing(#varName) ? suffix : "")

void Workout::print()
{
    std::cout << "  Workout " << BOLD << m_Date << " | \033[34;3m" << typeToStr(m_Type) << "\033[0m\n";

    if (!isMissing("m_Duration"))
        std::cout << "      Duration         - " << ITALIC << durationToTimeFormat() << ANSI_END << "\n";
    else
        std::cout << "      Duration         - " << ITALIC << "n/a" << ANSI_END << "\n";

    // Only show if distance needs to be showed
    if (m_Type == WorkoutType::Cycling || m_Type == WorkoutType::Running) {
        if (!isMissing("m_Distance"))
            std::cout << "      Distance         - " << ITALIC << std::fixed << std::setprecision(2) << m_Distance << "km" << ANSI_END << "\n";
        else
            std::cout << "      Distance         - " << ITALIC << "n/a" << ANSI_END << "\n";
    }
           
    std::cout << "      Heart Rate       - " << ITALIC << GET_IF_NOT_MISSING(m_AvgHeartRate, "bpm") << ANSI_END << "\n";
    std::cout << "      Calories Burned  - " << ITALIC << GET_IF_NOT_MISSING(m_BurnedCalories, "kcal") << ANSI_END << "\n\n";

    switch (m_Type) 
    {
        case WorkoutType::Cycling: {
            // When we show a cycling workout we want to show the
            //     - Distance (already shown)
            //     - Speed (avg, max)
            //     - Power data (avg, max, normalized)

            std::cout << "      Average speed    - " << ITALIC << GET_IF_NOT_MISSING(m_AvgCyclingSpeed, "km/h") << ANSI_END << "\n";
            std::cout << "      Max speed        - " << ITALIC << GET_IF_NOT_MISSING(m_MaxCyclingSpeed, "km/h") << ANSI_END << "\n\n";

            std::cout << "      Average power    - " << ITALIC << GET_IF_NOT_MISSING(m_AvgPower, "w") << ANSI_END << "\n";
            std::cout << "      Normalized Power - " << ITALIC << GET_IF_NOT_MISSING(m_NormalizedPower, "w") << ANSI_END << "\n";
            std::cout << "      Max Power        - " << ITALIC << GET_IF_NOT_MISSING(m_MaxPower, "w") << ANSI_END << "\n\n";

            break;
        }

        case WorkoutType::Running: {
            // When we show a running workout we want to show the
            //     - Distance (already shown)
            //     - Average pace (m/km)

            std::cout << "      Averace Pace     - " << ITALIC << GET_IF_NOT_MISSING(m_AvgRunningPace, "/km") << ANSI_END << "\n\n";

            break;
        }

        default: break;
    }

        std::cout << "      Notes            - " << ITALIC << GET_IF_NOT_MISSING(m_Note, "") << ANSI_END << "\n";
}


void Workout::addMissing(std::string name)
{
    m_MissingData[name] = true;
}


bool Workout::isMissing(std::string name)
{
    auto it = m_MissingData.find(name);
    return it != m_MissingData.end(); // Returns true if name is found
}


void Workout::saveToDisk()
{
    std::ofstream savefile;

    savefile.open(SAVE_FILE, std::ios_base::app);

    savefile << "[workout]\n";

    savefile << "    m_ID : " << m_ID << "\n";
    savefile << "    m_Type : " << (int) m_Type << "\n";
    savefile << "    m_Duration : " << m_Duration << "\n";
    savefile << "    m_Date : " << m_Date << "\n";
    
    if (m_Type == WorkoutType::Cycling || m_Type == WorkoutType::Running)
        savefile << "    m_Distance : " << m_Distance << "\n";

    savefile << "    m_AvgHeartRate : " << m_AvgHeartRate << "\n";
    savefile << "    m_BurnedCalories : " << m_BurnedCalories << "\n";

    if (m_Type == WorkoutType::Cycling) {
        savefile << "    m_AvgCyclingSpeed : " << m_AvgCyclingSpeed << "\n";
        savefile << "    m_MaxCyclingSpeed : " << m_MaxCyclingSpeed << "\n";

        savefile << "    m_AvgPower : " << m_AvgPower << "\n";
        savefile << "    m_NormalizedPower : " << m_NormalizedPower << "\n";
        savefile << "    m_MaxPower : " << m_MaxPower << "\n";
    }

    else if (m_Type == WorkoutType::Running) {
        savefile << "    m_AvgRunningPace : " << m_AvgRunningPace << "\n";
    }

    savefile << "    m_Note : \"" << m_Note << "\"\n";
    savefile << "[endworkout]\n";
}


static void setValueFromString(Workout& w, std::string key, std::string value)
{
    if (key == "m_ID") w.m_ID = value;
    else if (key == "m_Type") w.m_Type = (WorkoutType) std::stoi(value);
    else if (key == "m_Duration") w.m_Duration = std::stoul(value);
    else if (key == "m_Date") w.m_Date = value;
    else if (key == "m_Distance") w.m_Distance = std::stof(value);
    else if (key == "m_AvgHeartRate") w.m_AvgHeartRate = std::stoi(value);
    else if (key == "m_BurnedCalories") w.m_BurnedCalories = std::stoi(value);
    else if (key == "m_AvgCyclingSpeed") w.m_AvgCyclingSpeed = std::stof(value);
    else if (key == "m_MaxCyclingSpeed") w.m_MaxCyclingSpeed = std::stof(value);
    else if (key == "m_AvgPower") w.m_AvgPower = std::stoi(value);
    else if (key == "m_MaxPower") w.m_MaxPower = std::stoi(value);
    else if (key == "m_NormalizedPower") w.m_NormalizedPower = std::stoi(value);
    else if (key == "m_AvgRunningPace") w.m_AvgRunningPace = value;
    else if (key == "m_Note") w.m_Note = value;

    else {
        std::cerr << "ERROR: unkown key in `" << SAVE_FILE << "`: `" << key << "`" << std::endl;
        exit(0);
    }
}


std::vector<Workout> Workout::getWorkoutsFromDisk()
{
    std::ifstream savefile(SAVE_FILE);
    std::string lineContent;

    std::string key, value;
    bool isInWorkout = false;

    Workout currentWorkout;
    std::vector<Workout> workouts;

    while (std::getline(savefile, lineContent)) {
        if (lineContent == "[workout]") {
            currentWorkout.m_BeginPos = savefile.tellg() - (std::streampos) 10;
            isInWorkout = true;
        } else if (lineContent == "[endworkout]") {
            currentWorkout.m_EndPos = savefile.tellg();
            workouts.push_back(currentWorkout);
            currentWorkout = Workout();
            isInWorkout = false;
        } else if (isInWorkout) {
            std::stringstream ss(lineContent);
            std::string token;

            std::getline(ss, key, ':');
            std::getline(ss, value);

            key.erase(0, key.find_first_not_of(' '));
            key.erase(key.find_last_not_of(' ') + 1);

            value.erase(0, value.find_first_not_of(' '));
            value.erase(value.find_last_not_of(' ') + 1);

            // Handle m_Note field separately
            if (key == "m_Note") {
                // Find the position of the first and last double quotes
                size_t start = value.find('"');
                size_t end = value.find_last_of('"');

                if (start != std::string::npos && end != std::string::npos && start < end) {
                    // Extract the value between the double quotes
                    value = value.substr(start + 1, end - start - 1);
                }
            }

            setValueFromString(currentWorkout, key, value);
        } else {
            std::cerr << "ERROR: unkown tag in `" << SAVE_FILE << "`: `" << lineContent << "`\n";
            exit(1);
        }
    }

    return workouts;
}


bool Workout::removeWorkoutFromDisk()
{
    std::fstream savefile(SAVE_FILE, std::ios::in | std::ios::out);
    savefile.seekp(m_BeginPos, std::ios::beg);

    if (!savefile.is_open())
        return false;

    std::cout << "beginpos: " << m_BeginPos << "\n";
    std::cout << "endpos: " << m_EndPos << "\n";

    for (int i = 0; i < m_EndPos - m_BeginPos; i++)
        savefile << ' ';

    return true;
}