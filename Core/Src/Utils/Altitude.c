#include "Utils/Altitude.h"

float CalculateKelvinFromCelsius(float TemperatureC) {
    return TemperatureC + 273.15f;
}

float CalculateAltitude(float PressurePa, float Temperature, float ReferencePressurePa, bool ReferencePressurePaValid) {
    if (PressurePa <= 0.0f || ReferencePressurePa <= 0.0f || !ReferencePressurePaValid) {
        return 0.0f;
    }

    float TemperatureK = CalculateKelvinFromCelsius(Temperature);

    return (GAS_CONSTANT * TemperatureK / GRAV_CONSTANT) * logf(ReferencePressurePa / PressurePa);
}

float CalculateFilteredAltitude(SystemContext_t *SystemContext, float RawAltitude) {
    static float FilteredAltitude = 0.0f;

    if (!SystemContext->ReferencePressurePaValid) {
        FilteredAltitude = 0.0f;
        SystemContext->AltitudeFilterInitialized = false;
        return 0.0f;
    }

    if (!SystemContext->AltitudeFilterInitialized) {
        FilteredAltitude = RawAltitude;
        SystemContext->AltitudeFilterInitialized = true;
        return FilteredAltitude;
    }

    FilteredAltitude = FilteredAltitude + ALTITUDE_IIR_FILTER_ALPHA * (RawAltitude - FilteredAltitude);
    return FilteredAltitude;
}
