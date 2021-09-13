struct Engine
{
    int temp1;        // Engine Temperature 1
    int temp2;        // Engine Temperature 2
    int temp3;        // Engine Temperature 3
    int temp4_vacuum; // Engine Vacuum Pump Temperature 4
    int rpm;
    int running_hours;
    int lube_oil_pressure;
};
struct Fuel
{
    int fuel_volume;            //- Measurement of exact volume in real time
    int fuel_percentage;        //- Measurement of fuel level in real time
    int fuel_filling_quantity;  //Volume of fuel filled in the tank
    int fuel_draining_quantity; // Volume of fuel drained from the tank
    bool communication_error;   // Fuel level sensor connection tampering / no communication of LLS data
};
/*
struct water with propertites :
Water presence in pump - Identify and monitor water flow during the pump ON period 
No water flow / Dry run - No water flowing in the pump body
Water flow - Continuous measurement of water flow 
Water flow volume - Total run hours for a duration or user selected period
*/
struct Water
{
    int water_presence;    //- Identify and monitor water flow during the pump ON period
    int dry_run;           //- No water flowing in the pump body
    int water_flow;        //- Continuous measurement of water flow
    int water_flow_volume; //- Total run hours for a duration or user selected period
};
/*
Control 
emote Engine ON - Turn on the engine remotely
Remote Engine OFF - Turn off the engine remotely
Self Engine OFF, dry run > user defined time - Turning off the engine 
when fuel pump is running dry.
Self Engine OFF, low fuel - Turning off the engine 
when fuel is running low.
Self Engine OFF, Engine operated more than a limit - Turning off the engine 
when it runs more that a certain time limit.
Self Engine OFF, Engine temp critical high - Turning off the engine 
when engine running with a high temperature.
*/
struct Control
{
    int engine_on;
    int engine_off;
    int dry_run_time = 10;
    int low_fuel_percent = 10;
    int engine_critical_temp = 200;
    bool isDryRun(Water water) { return water.dry_run; }
    bool isLowFuel(Fuel fuel) { return fuel.fuel_percentage < low_fuel_percent; }
    bool isOperatedMoreThanLimit(Engine engine)
    {
        return engine.running_hours > dry_run_time;
    }
    bool isEngineTempCriticalHigh(Engine engine)
    {
        return engine.temp1 > engine_critical_temp;
    }
    bool shouldEngineBeOn(Water water, Fuel fuel, Engine engine)
    {
        return isDryRun(water) && isLowFuel(fuel) && isOperatedMoreThanLimit(engine);
    }
};