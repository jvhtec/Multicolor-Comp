#pragma once

#include <cmath>
#include <algorithm>

/**
 * Simple one-pole parameter smoother to prevent zipper noise
 */
class ParameterSmoother
{
public:
    ParameterSmoother() = default;

    void reset(float initialValue = 0.0f)
    {
        current = initialValue;
        target = initialValue;
    }

    void setSampleRate(double sr)
    {
        sampleRate = sr;
        updateCoefficient();
    }

    void setSmoothingTime(float timeMs)
    {
        smoothTimeMs = timeMs;
        updateCoefficient();
    }

    void setTarget(float newTarget)
    {
        target = newTarget;
    }

    float getNext()
    {
        current += (target - current) * coeff;
        return current;
    }

    float getCurrentValue() const { return current; }
    bool isSmoothing() const { return std::abs(current - target) > 0.0001f; }

private:
    void updateCoefficient()
    {
        if (sampleRate > 0.0 && smoothTimeMs > 0.0f)
        {
            coeff = 1.0f - std::exp(-1.0f / (smoothTimeMs * 0.001f * static_cast<float>(sampleRate)));
        }
        else
        {
            coeff = 1.0f;
        }
    }

    double sampleRate = 44100.0;
    float smoothTimeMs = 5.0f;
    float coeff = 1.0f;
    float current = 0.0f;
    float target = 0.0f;
};

/**
 * Exponential envelope follower for compressor attack/release
 */
class EnvelopeFollower
{
public:
    void reset()
    {
        envelope = 0.0f;
    }

    void setSampleRate(double sr)
    {
        sampleRate = sr;
    }

    void setAttackTime(float ms)
    {
        attackCoeff = calculateCoeff(ms);
    }

    void setReleaseTime(float ms)
    {
        releaseCoeff = calculateCoeff(ms);
    }

    float process(float input)
    {
        float coeff = (input > envelope) ? attackCoeff : releaseCoeff;
        envelope += (input - envelope) * coeff;
        return envelope;
    }

    float getCurrentValue() const { return envelope; }

private:
    float calculateCoeff(float timeMs) const
    {
        if (timeMs <= 0.0f || sampleRate <= 0.0)
            return 1.0f;

        return 1.0f - std::exp(-1.0f / (timeMs * 0.001f * static_cast<float>(sampleRate)));
    }

    double sampleRate = 44100.0;
    float envelope = 0.0f;
    float attackCoeff = 1.0f;
    float releaseCoeff = 1.0f;
};
