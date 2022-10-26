#include "BiquadFilter.hpp"

#define _USE_MATH_DEFINES
#include <math.h>

BiquadFilter::BiquadFilter(float freq, float res)
    : mF(freq),
      mQ(res),
      mDbGain(0),
      mType(BiquadFilter::FilterType::LowPass),
      mSampleRate(48000) {

    clear();
    this->coeff[0].updateFilterCoeff(this->mType, mF, mQ, mDbGain, this->mSampleRate);
    this->coeff[1].updateFilterCoeff(this->mType, mF, mQ, mDbGain, this->mSampleRate);
}

void BiquadFilter::clear()
{
    this->coeff[0].mZ1 = 0;
    this->coeff[0].mZ2 = 0;
    this->coeff[1].mZ1 = 0;
    this->coeff[1].mZ2 = 0;
}

void BiquadFilter::setFilterType (BiquadFilter::FilterType type)
{
    if (type != mType)
    {
        mType = type;
        clear();
    }
}

void BiquadFilter::setFrequency(double f) {

    if (mF != f) {
        mF = static_cast<float>(f);
        this->updateCoefficients();
    }
}

void BiquadFilter::setResonance(double q) {

    if (mQ != q) {
        mQ = static_cast<float>(q);
        this->updateCoefficients();
    }
}

void BiquadFilter::Coefficients::updateFilterCoeff(FilterType type, float freq, float res, float dbGain, double sampleRate)
{
    if (freq <= 0 || freq != freq)
    {
        mA0 = 1;
        mA1 = 0;
        mA2 = 0;
        mB1 = 0;
        mB2 = 0;
        return;
    }

    double norm;
    double V = pow (10, fabs (dbGain) / 20.0);
    double K = tan (M_PI * (freq / sampleRate));

    switch (type)
    {
        case FilterType::LowPass:
            norm = 1 / (1 + K / res + K * K);
            mA0 = K * K * norm;
            mA1 = 2 * mA0;
            mA2 = mA0;
            mB1 = 2 * (K * K - 1) * norm;
            mB2 = (1 - K / res + K * K) * norm;
            break;

        case FilterType::HighPass:
            norm = 1 / (1 + K / res + K * K);
            mA0 = 1 * norm;
            mA1 = -2 * mA0;
            mA2 = mA0;
            mB1 = 2 * (K * K - 1) * norm;
            mB2 = (1 - K / res + K * K) * norm;
            break;

        case FilterType::BandPass:
            norm = 1 / (1 + K / res + K * K);
            mA0 = K / res * norm;
            mA1 = 0;
            mA2 = -mA0;
            mB1 = 2 * (K * K - 1) * norm;
            mB2 = (1 - K / res + K * K) * norm;
            break;

        case FilterType::Notch:
            norm = 1 / (1 + K / res + K * K);
            mA0 = (1 + K * K) * norm;
            mA1 = 2 * (K * K - 1) * norm;
            mA2 = mA0;
            mB1 = mA1;
            mB2 = (1 - K / res + K * K) * norm;
            break;

        case FilterType::Peak:

            if (dbGain >= 0)
            { // boost
                norm = 1 / (1 + K / res + K * K);
                mA0 = (1 + K / res * V + K * K) * norm;
                mA1 = 2 * (K * K - 1) * norm;
                mA2 = (1 - K / res * V + K * K) * norm;
                mB1 = mA1;
                mB2 = (1 - K / res + K * K) * norm;
            }
            else
            { // cut
                norm = 1 / (1 + K / res * V + K * K);
                mA0 = (1 + K / res + K * K) * norm;
                mA1 = 2 * (K * K - 1) * norm;
                mA2 = (1 - K / res + K * K) * norm;
                mB1 = mA1;
                mB2 = (1 - K / res * V + K * K) * norm;
            }
            break;

        case FilterType::LowShelf:
            if (dbGain >= 0)
            { // boost
                norm = 1 / (1 + K / res + K * K);
                mA0 = (1 + sqrt (V) * K / res + V * K * K) * norm;
                mA1 = 2 * (V * K * K - 1) * norm;
                mA2 = (1 - sqrt (V) * K / res + V * K * K) * norm;
                mB1 = 2 * (K * K - 1) * norm;
                mB2 = (1 - K / res + K * K) * norm;
            }
            else
            { // cut
                norm = 1 / (1 + sqrt (V) * K / res + V * K * K);
                mA0 = (1 + K / res + K * K) * norm;
                mA1 = 2 * (K * K - 1) * norm;
                mA2 = (1 - K / res + K * K) * norm;
                mB1 = 2 * (V * K * K - 1) * norm;
                mB2 = (1 - sqrt (V) * K / res + V * K * K) * norm;
            }
            break;

        case FilterType::HighShelf:
            if (dbGain >= 0)
            { // boost
                norm = 1 / (1 + K / res + K * K);
                mA0 = (V + sqrt (V) * K / res + K * K) * norm;
                mA1 = 2 * (K * K - V) * norm;
                mA2 = (V - sqrt (V) * K / res + K * K) * norm;
                mB1 = 2 * (K * K - 1) * norm;
                mB2 = (1 - K / res + K * K) * norm;
            }
            else
            { // cut
                norm = 1 / (V + sqrt (V) * K / res + K * K);
                mA0 = (1 + K / res + K * K) * norm;
                mA1 = 2 * (K * K - 1) * norm;
                mA2 = (1 - K / res + K * K) * norm;
                mB1 = 2 * (K * K - V) * norm;
                mB2 = (V - sqrt (V) * K / res + K * K) * norm;
            }
            break;

        case FilterType::LowShelfNoQ:
            if (dbGain >= 0)
            { // boost
                norm = 1 / (1 + sqrt (2) * K + K * K);
                mA0 = (1 + sqrt (2 * V) * K + V * K * K) * norm;
                mA1 = 2 * (V * K * K - 1) * norm;
                mA2 = (1 - sqrt (2 * V) * K + V * K * K) * norm;
                mB1 = 2 * (K * K - 1) * norm;
                mB2 = (1 - sqrt (2) * K + K * K) * norm;
            }
            else
            { // cut
                norm = 1 / (1 + sqrt (2 * V) * K + V * K * K);
                mA0 = (1 + sqrt (2) * K + K * K) * norm;
                mA1 = 2 * (K * K - 1) * norm;
                mA2 = (1 - sqrt (2) * K + K * K) * norm;
                mB1 = 2 * (V * K * K - 1) * norm;
                mB2 = (1 - sqrt (2 * V) * K + V * K * K) * norm;
            }
            break;

        case FilterType::HighShelfNoQ:
            if (dbGain >= 0)
            { // boost
                norm = 1 / (1 + sqrt (2) * K + K * K);
                mA0 = (V + sqrt (2 * V) * K + K * K) * norm;
                mA1 = 2 * (K * K - V) * norm;
                mA2 = (V - sqrt (2 * V) * K + K * K) * norm;
                mB1 = 2 * (K * K - 1) * norm;
                mB2 = (1 - sqrt (2) * K + K * K) * norm;
            }
            else
            { // cut
                norm = 1 / (V + sqrt (2 * V) * K + K * K);
                mA0 = (1 + sqrt (2) * K + K * K) * norm;
                mA1 = 2 * (K * K - 1) * norm;
                mA2 = (1 - sqrt (2) * K + K * K) * norm;
                mB1 = 2 * (K * K - V) * norm;
                mB2 = (V - sqrt (2 * V) * K + K * K) * norm;
            }
            break;

        case FilterType::AllPass:
            norm = 1 / (1 + K / res + K * K);
            mA0 = (1 - K / res + K * K) * norm;
            mA1 = 2 * (K * K - 1) * norm;
            mA2 = 1;
            mB1 = mA1;
            mB2 = mA0;
            break;

        case FilterType::Off:
            mA0 = 1;
            mA1 = 0;
            mA2 = 0;
            mB1 = 0;
            mB2 = 0;
            break;
    }
}

//void BiquadFilter::copyCoeffFrom (BiquadFilter& other)
//{
//    mA0 = other.mA0;
//    mA1 = other.mA1;
//    mA2 = other.mA2;
//    mB1 = other.mB1;
//    mB2 = other.mB2;
//}

float BiquadFilter::Coefficients::getMagnitudeResponseAt (float f, double sampleRate) const
{
    auto const piw0 = (f / sampleRate) * M_PI * 2;
    auto const cosw = std::cos (piw0);
    auto const sinw = std::sin (piw0);

    auto square = [] (auto z) {
        return z * z;
    };

    auto const numerator = sqrt (square (mA0 * square (cosw) - mA0 * square (sinw) + mA1 * cosw + mA2) + square (2 * mA0 * cosw * sinw + mA1 * (sinw)));
    auto const denominator = sqrt (square (square (cosw) - square (sinw) + mB1 * cosw + mB2) + square (2 * cosw * sinw + mB1 * (sinw)));

    return static_cast<float> (numerator / denominator);
}

void BiquadFilter::updateCoefficients() {
    this->coeff[0].updateFilterCoeff(this->mType, mF, mQ, mDbGain, this->mSampleRate);
    this->coeff[1].updateFilterCoeff(this->mType, mF, mQ, mDbGain, this->mSampleRate);
}
