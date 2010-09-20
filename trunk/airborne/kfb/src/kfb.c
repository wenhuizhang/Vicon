/**
 * @file   kfb.c
 * @author Liam O'Sullivan
 *
 * $Author: liamosullivan $
 * $Date: 2010-08-28 11:26:11 +1000 (Sat, 28 Aug 2010) $
 * $Rev: 324 $
 * $Id: kfb.c 324 2010-08-28 01:26:11Z liamosullivan $
 *
 * Queensland University of Technology
 *
 * @section DESCRIPTION
 * Kalman filter library (revision b) which implements:
 * - Initialising the Kalman filter
 * - Performs the attitude filtering
 * - Low pass filtering for read IMU data
 */

#include "kfb.h"

// phi axis struct
axis phi_axis;
// theta axis struct
axis theta_axis;
// accelerometer value storage (used for filtering)
double acc_previous[3];
double acc_current[3];
// euler agnle storage (calculation of filtered rate)
double angle_previous[3];

// function to initialise the values in the axis structures
int attitudeFilterInitialiseB(double *accXr, double *accYr, double *accZr)
{
  /* phi axis */
  // initialise phi angle to coarse roll angle
  phi_axis.X[0] = coarseRollAngle(accXr,accYr,accZr);
  // no initial bias
  phi_axis.X[1] = 0.0;
  // initialise covariance matrix to 1
  phi_axis.P[0][0] = 1.0;
  phi_axis.P[0][1] = 1.0;
  phi_axis.P[1][0] = 1.0;
  phi_axis.P[1][1] = 1.0;
  // initialise S term
  phi_axis.S = 0.0;
  // initialise measurement term
  phi_axis.Y = phi_axis.X[0];
  // initialise correction term
  phi_axis.err = 0.0;
  // initialise the kalman gain
  phi_axis.L[0] = 0.0;
  phi_axis.L[1] = 0.0;
  // initialise the Q terms
  phi_axis.Q[0] = PHI_ANGLE_Q;
  phi_axis.Q[1] = PHI_GYRO_Q;
  // initialise the R term
  phi_axis.R = PHI_R;
  // initialise the direction term
  phi_axis.direction = PHI_DIRECTION;


  /* theta axis */
  // initialise theta angle to coarse pitch angle
  theta_axis.X[0] = coarsePitchAngle(accXr,accYr,accZr);
  // no initial bias
  theta_axis.X[1] = 0.0;
  // initialise covariance matrix to 1
  theta_axis.P[0][0] = 1.0;
  theta_axis.P[0][1] = 1.0;
  theta_axis.P[1][0] = 1.0;
  theta_axis.P[1][1] = 1.0;
  // initialise S term
  theta_axis.S = 0.0;
  // initialise measurement term
  theta_axis.Y = theta_axis.X[0];
  // initialise correction term
  theta_axis.err = 0.0;
  // initialise the kalman gain
  theta_axis.L[0] = 0.0;
  theta_axis.L[1] = 0.0;
  // initialise the Q terms
  theta_axis.Q[0] = THETA_ANGLE_Q;
  theta_axis.Q[1] = THETA_GYRO_Q;
  // initialise the R term
  theta_axis.R = THETA_R;
  // initialise the direction term
  theta_axis.direction = THETA_DIRECTION;

  /* accelerometer previous values */
  acc_previous[0] = *accXr;
  acc_previous[1] = *accYr;
  acc_previous[2] = *accZr;

  /* euler angle previous values */
  angle_previous[0] = phi_axis.X[0]*M_PI/180;
  angle_previous[1] = theta_axis.X[0]*M_PI/180;

  return 1;
}
int attitudeFilterB(double *rateXr, double *rateYr, double *rateZr, double *accXr, double *accYr, double *accZr, double *rateXf, double *rateYf, double *rateZf, double *phif, double *thetaf, double *psif, double dT)
{
  // LPF the accelerometer values
  accLPF(accXr,accYr,accZr,dT); 
  // time update for phi axis
  kFilterTimeUpdate(&phi_axis,rateXr,dT);
  // time update for theta axis
  kFilterTimeUpdate(&theta_axis,rateYr,dT);
  // calculate the coarse angle for phi from the sensors
  phi_axis.Y = coarseRollAngle(accXr,accYr,accZr);
  // measurement update for phi axis
  kFilterMeasureUpdate(&phi_axis);
  // calculate the coarse angle for theta from the sensors
  theta_axis.Y = coarsePitchAngle(accXr,accYr,accZr);
  // measurement update for phi axis
  kFilterMeasureUpdate(&theta_axis);
  // assign new phi angle (radians)
  *phif = phi_axis.X[0]*M_PI/180;
  // assign new theta angle (radians)
  *thetaf = theta_axis.X[0]*M_PI/180;
  // assign new phi rate
  *rateXf = (*phif - angle_previous[0])/dT;
  // assign new theta rate
  *rateYf = (*thetaf - angle_previous[1])/dT;
  // update the previous angles
  angle_previous[0] = *phif;
  angle_previous[1] = *thetaf;

  return 1;
}

// Kalman filter time update for axis
int kFilterTimeUpdate(axis *axis_t, double *gyroRate, double dT)
{
  // compute the axis angle by unbiasing the gyro reading and integrating
  axis_t->X[0] += ((*gyroRate*axis_t->direction) - axis_t->X[1])*dT;
  // calculate the covariance matrix and integrate (discrete)
  axis_t->P[0][0] += (axis_t->Q[0] - axis_t->P[0][1] - axis_t->P[1][0])*dT;
  axis_t->P[0][1] += -axis_t->P[1][1]*dT;
  axis_t->P[1][0] += -axis_t->P[1][1]*dT;
  axis_t->P[1][1] += axis_t->Q[1]*dT;
  // end time update
  return 1;
}

// Kalman filter measurement update for axis (assume coarse angle is calculated)
int kFilterMeasureUpdate (axis *axis_t)
{
  // compute the error term
  axis_t->S = axis_t->P[0][0] + axis_t->R;
  // calculate the Kalman gain
  axis_t->L[0] = axis_t->P[0][0] / axis_t->S;
  axis_t->L[1] = axis_t->P[1][0] / axis_t->S;
  // update the covariance matrix (careful of computation order)
  axis_t->P[1][0] -= axis_t->L[1]*axis_t->P[0][0];
  axis_t->P[1][1] -= axis_t->L[1]*axis_t->P[0][1];
  axis_t->P[0][0] -= axis_t->L[0]*axis_t->P[0][0];
  axis_t->P[0][1] -= axis_t->L[0]*axis_t->P[0][1];
  // calculate the difference between prediction and measurement
  axis_t->err = axis_t->Y - axis_t->X[0];
  // update the x states (angle and gyro bias)
  axis_t->X[0]+= axis_t->err*axis_t->L[0];
  axis_t->X[1]+= axis_t->err*axis_t->L[1];
  // end measurement update
  return 1;
}

// function to return the coarse pitch angle based on the accelerometer values (degrees)
double coarsePitchAngle(double *accXr, double *accYr, double *accZr)
{
  return atan2(*accXr,sqrt(*accYr * *accYr + *accZr * *accZr))*180/M_PI;
}

// function to return the coarse roll angle based on the accelerometer values (degrees)
double coarseRollAngle(double *accXr, double *accYr, double *accZr)
{
  return atan2(*accYr,sqrt(*accXr * *accXr + *accZr * *accZr))*180/M_PI;
}

double accLPF (double *accXr, double *accYr, double *accZr, double dT)
{
  acc_current[0] = *accXr;
  acc_current[1] = *accYr;
  acc_current[2] = *accZr;
  // LPF the X accelerometer value
  *accXr = acc_previous[0]*(1-ACCX_ALPHA) + (acc_current[0]*ACCX_ALPHA);
  // LPF the Y accelerometer value
  *accYr = acc_previous[1]*(1-ACCY_ALPHA) + (acc_current[1]*ACCY_ALPHA);  
  // LPF the Z accelerometer value
  *accZr = acc_previous[2]*(1-ACCZ_ALPHA) + (acc_current[2]*ACCZ_ALPHA);
  // store the filtered values
  acc_previous[0] = *accXr;
  acc_previous[1] = *accYr;
  acc_previous[2] = *accZr;
}
