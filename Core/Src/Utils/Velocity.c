#include "Utils/Velocity.h"

typedef struct {
    float Altitude;
    uint32_t TimestampUs;
} AltitudeSample_t;

static AltitudeSample_t VelocitySamples[VELOCITY_Z_WINDOW_SIZE];
static uint8_t VelocitySampleIndex = 0;
static uint8_t VelocitySampleCount = 0;

void ResetVelocityZWindow(void) {
    VelocitySampleIndex = 0;
    VelocitySampleCount = 0;

    for (uint8_t i = 0; i < VELOCITY_Z_WINDOW_SIZE; i++) {
        VelocitySamples[i].Altitude = 0.0f;
        VelocitySamples[i].TimestampUs = 0;
    }
}

static void PushVelocitySample(float Altitude, uint32_t TimestampUs) {
    VelocitySamples[VelocitySampleIndex].Altitude = Altitude;
    VelocitySamples[VelocitySampleIndex].TimestampUs = TimestampUs;

    VelocitySampleIndex = (uint8_t)((VelocitySampleIndex + 1u) % VELOCITY_Z_WINDOW_SIZE);
    if (VelocitySampleCount < VELOCITY_Z_WINDOW_SIZE) {
        VelocitySampleCount++;
    }
}

static float CalculateVelocityZWindowed(void) {
    if (VelocitySampleCount < 2u) {
        return 0.0f;
    }

    const uint8_t newest_index = (uint8_t)((VelocitySampleIndex + VELOCITY_Z_WINDOW_SIZE - 1u) % VELOCITY_Z_WINDOW_SIZE);
    const uint8_t oldest_index = (VelocitySampleCount < VELOCITY_Z_WINDOW_SIZE) ? 0u : VelocitySampleIndex;

    const uint32_t DeltaUs = VelocitySamples[newest_index].TimestampUs - VelocitySamples[oldest_index].TimestampUs;
    if (DeltaUs == 0u) {
        return 0.0f;
    }

    const float DeltaAltitude = VelocitySamples[newest_index].Altitude - VelocitySamples[oldest_index].Altitude;
    return DeltaAltitude / ((float)DeltaUs * 1e-6f);
}

float CalculateVelocityZ(SystemContext_t *SystemContext, TIM_HandleTypeDef *TIM2_Handle, float CurrentAltitude) {
    const uint32_t TimestampUs = __HAL_TIM_GET_COUNTER(TIM2_Handle);

    if (!SystemContext->ReferencePressurePaValid) {
        ResetVelocityZWindow();
        return 0.0f;
    }

    PushVelocitySample(CurrentAltitude, TimestampUs);
    return CalculateVelocityZWindowed();
}
