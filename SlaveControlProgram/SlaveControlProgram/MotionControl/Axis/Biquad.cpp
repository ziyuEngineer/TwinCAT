#pragma once
#pragma hdrstop
//
//  Biquad.cpp
//
//  Created by Nigel Redmon on 11/24/12
//  EarLevel Engineering: earlevel.com
//  Copyright 2012 Nigel Redmon
//
//  For a complete explanation of the Biquad code:
//  http://www.earlevel.com/main/2012/11/26/biquad-c-source-code/
//
//  License:
//
//  This source code is provided as is, without warranty.
//  You may copy and distribute verbatim copies of this document.
//  You may modify and use this source code to create binary code
//  for your own purposes, free or commercial.
//

#include "Biquad.h"

Biquad::Biquad() {
    type = FilterType::bq_type_lowpass;
    a0 = 1.0;
    a1 = a2 = b1 = b2 = 0.0;
    Fc = 1.0;
    Q = 0.707;
    peakGain = 0.0;
    z1 = z2 = 0.0;
}

Biquad::Biquad(FilterType type, double Fc, double Q, double peakGainDB) {
    setBiquad(type, Fc, Q, peakGainDB);
    z1 = z2 = 0.0;
}

Biquad::~Biquad() {
}

void Biquad::setType(FilterType type) {
    this->type = type;
    calcBiquad();
}

void Biquad::setQ(double Q) {
    this->Q = Q;
    calcBiquad();
}

void Biquad::setFc(double Fc) {
    this->Fc = Fc;
    calcBiquad();
}

void Biquad::setPeakGain(double peakGainDB) {
    this->peakGain = peakGainDB;
    calcBiquad();
}
    
void Biquad::setBiquad(FilterType type, double Fc, double Q, double peakGainDB) {
    this->type = type;
    this->Q = Q;
    this->Fc = Fc;
    setPeakGain(peakGainDB);
}

void Biquad::calcBiquad(void) {
    double norm;
    double V = pow_(10, fabs_(peakGain) / 20.0);
    double K = tan_(PI * Fc);
    switch (this->type) {
        case FilterType::bq_type_lowpass:
            norm = 1 / (1 + K / Q + K * K);
            a0 = K * K * norm;
            a1 = 2 * a0;
            a2 = a0;
            b1 = 2 * (K * K - 1) * norm;
            b2 = (1 - K / Q + K * K) * norm;
            break;
            
        case FilterType::bq_type_highpass:
            norm = 1 / (1 + K / Q + K * K);
            a0 = 1 * norm;
            a1 = -2 * a0;
            a2 = a0;
            b1 = 2 * (K * K - 1) * norm;
            b2 = (1 - K / Q + K * K) * norm;
            break;
            
        case FilterType::bq_type_bandpass:
            norm = 1 / (1 + K / Q + K * K);
            a0 = K / Q * norm;
            a1 = 0;
            a2 = -a0;
            b1 = 2 * (K * K - 1) * norm;
            b2 = (1 - K / Q + K * K) * norm;
            break;
            
        case FilterType::bq_type_notch:
            norm = 1 / (1 + K / Q + K * K);
            a0 = (1 + K * K) * norm;
            a1 = 2 * (K * K - 1) * norm;
            a2 = a0;
            b1 = a1;
            b2 = (1 - K / Q + K * K) * norm;
            break;
            
        case FilterType::bq_type_peak:
            if (peakGain >= 0) {    // boost
                norm = 1 / (1 + 1/Q * K + K * K);
                a0 = (1 + V/Q * K + K * K) * norm;
                a1 = 2 * (K * K - 1) * norm;
                a2 = (1 - V/Q * K + K * K) * norm;
                b1 = a1;
                b2 = (1 - 1/Q * K + K * K) * norm;
            }
            else {    // cut
                norm = 1 / (1 + V/Q * K + K * K);
                a0 = (1 + 1/Q * K + K * K) * norm;
                a1 = 2 * (K * K - 1) * norm;
                a2 = (1 - 1/Q * K + K * K) * norm;
                b1 = a1;
                b2 = (1 - V/Q * K + K * K) * norm;
            }
            break;
        case FilterType::bq_type_lowshelf:
            if (peakGain >= 0) {    // boost
                norm = 1 / (1 + sqrt_(2) * K + K * K);
                a0 = (1 + sqrt_(2*V) * K + V * K * K) * norm;
                a1 = 2 * (V * K * K - 1) * norm;
                a2 = (1 - sqrt_(2*V) * K + V * K * K) * norm;
                b1 = 2 * (K * K - 1) * norm;
                b2 = (1 - sqrt_(2) * K + K * K) * norm;
            }
            else {    // cut
                norm = 1 / (1 + sqrt_(2*V) * K + V * K * K);
                a0 = (1 + sqrt_(2) * K + K * K) * norm;
                a1 = 2 * (K * K - 1) * norm;
                a2 = (1 - sqrt_(2) * K + K * K) * norm;
                b1 = 2 * (V * K * K - 1) * norm;
                b2 = (1 - sqrt_(2*V) * K + V * K * K) * norm;
            }
            break;
        case FilterType::bq_type_highshelf:
            if (peakGain >= 0) {    // boost
                norm = 1 / (1 + sqrt_(2) * K + K * K);
                a0 = (V + sqrt_(2*V) * K + K * K) * norm;
                a1 = 2 * (K * K - V) * norm;
                a2 = (V - sqrt_(2*V) * K + K * K) * norm;
                b1 = 2 * (K * K - 1) * norm;
                b2 = (1 - sqrt_(2) * K + K * K) * norm;
            }
            else {    // cut
                norm = 1 / (V + sqrt_(2*V) * K + K * K);
                a0 = (1 + sqrt_(2) * K + K * K) * norm;
                a1 = 2 * (K * K - 1) * norm;
                a2 = (1 - sqrt_(2) * K + K * K) * norm;
                b1 = 2 * (K * K - V) * norm;
                b2 = (V - sqrt_(2*V) * K + K * K) * norm;
            }
            break;
    }
    
    return;
}
