#pragma once

#include <cstddef>
#include <string>
#include <map>
#include <vector>

using uuid_t = std::string;

enum class WorkoutType 
{
    Running,
    Cycling,
    None
};


class Workout 
{
public:
    Workout();
    Workout(WorkoutType type, size_t duration, std::string date);

    static uuid_t genUUID();

    void print();

    std::string durationToTimeFormat();
    static WorkoutType strToType(std::string str);

    void addMissing(std::string name);
    bool isMissing(std::string name);

    std::string getStrDataIfNotMissing(const std::string& varName, std::string data);
    std::string getStrDataIfNotMissing(const std::string& varName, float data);
    std::string getStrDataIfNotMissing(const std::string& varName, int data);
    std::string getStrDataIfNotMissing(const std::string& varName, size_t data);

    void saveToDisk();
    static std::vector<Workout> getWorkoutsFromDisk();
    bool removeWorkoutFromDisk();


public:
    uuid_t m_ID = "";
    std::streampos m_BeginPos = 0;
    std::streampos m_EndPos = 0;
    WorkoutType m_Type = WorkoutType::None;
    size_t m_Duration = 0;
    std::string m_Date = "n/a";
    float m_Distance = 0.0f;

    // Heart rate data
    int m_AvgHeartRate = 0;
    
    // Calories
    int m_BurnedCalories = 0;

    // Cycling speed
    float m_AvgCyclingSpeed = 0.0f;
    float m_MaxCyclingSpeed = 0.0f;

    // Cycling power data
    int m_AvgPower = 0;
    int m_MaxPower = 0;
    int m_NormalizedPower = 0;

    // Running pace data
    std::string m_AvgRunningPace = "00:00";

    // Note (how did the training go? What have you learned? etc.)
    std::string m_Note = "";

    std::map<std::string, bool> m_MissingData;
};