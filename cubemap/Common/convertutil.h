/* Licensed under the MIT license: http://www.opensource.org/licenses/mit-license.php */
/** @file
 * A collection of inline conversion functions
 * These are common useful arithmetic functions to convert angle values, format duration and bytes values into readable strings.
 */
#ifndef CONVERTUTIL_H
#define CONVERTUTIL_H

#include <QString>


/**
  * Convert an angle value in radians to a value in degrees.
  * @param radians A floating point value of an angle in radians.
  * @return The corresponding value in degrees.
  */
inline float toDegrees(float radians){ return radians*(180.0f/3.14159265f); }

/**
  * Convert an angle value in degrees to a value in radians.
  * @param degrees A floating point value of an angle in degrees.
  * @return The corresponding value in radians.
  */
inline float toRadians(float degrees){ return degrees*(3.14159265f/180.0f); }

/**
  * Format a duration in milliseconds to the format HOURS:MINUTES:SECONDS:MILLISECONDS
  * @param time A duration in integer milliseconds.
  * @return A QString in the format HOURS:MINUTES:SECONDS:MILLISECONDS.
  */
inline QString msToHMSMS(long time){

    long seconds = time / 1000;
    long mseconds = time % 1000;

    long minutes = seconds / 60;
    seconds = seconds % 60;

    long hours = minutes / 60;
    minutes = minutes % 60;

    return QString::number(hours) + "h " + QString::number(minutes) + "m "+ QString::number(seconds) + "s "+ QString::number(mseconds) + "ms ";
}

/**
  * Converts a value in bytes to Kilo or Mega bytes
  * @param bytes A value in integer bytes.
  * @return A QString in the format 'amount kB/MB'.
  */
inline QString bytesToKBMB(long bytes){
    double kB = bytes / 1024.0f;
    if(bytes < 1048576 ){
        return QString::number(kB) + " kB";
    }
    return QString::number(kB / 1024.0f) + " MB";
}

/**
  * Simple arithmetic conversion function used in cube map to fisheye conversion
  * @param x The value to convert.
  * @return The result of (x/2)+0.5.
  */
inline float rc(float x) { return (x/2)+0.5; }

#endif // CONVERTUTIL_H
