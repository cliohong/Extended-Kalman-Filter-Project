#include "FusionEKF.h"
#include "tools.h"
#include "Eigen/Dense"
#include <iostream>

using namespace std;
using Eigen::MatrixXd;
using Eigen::VectorXd;
using std::vector;

/*
 * Constructor.
 */
FusionEKF::FusionEKF() {
  is_initialized_ = false;
  
  previous_timestamp_ = 0;
  
  // initializing matrices
  R_laser_ = MatrixXd(2, 2);
  R_laser_<<0.001,0,
  0,0.001;


  R_radar_ = MatrixXd(3, 3);
  R_radar_<<0.0005,0,0,
  0,0.0005,0,
  0,0,0.0005;

  H_laser_ = MatrixXd(2, 4);
  H_laser_<<1,0,0,0,
  0,1,0,0;
  //  Hj_ = MatrixXd(3, 4);
  
  /**
   TODO:
   * Finish initializing the FusionEKF.
   */
  //set the acceleration noise components
  noise_ax = 5;
  noise_ay = 5;
}

/**
 * Destructor.
 */
FusionEKF::~FusionEKF() {}

void FusionEKF::ProcessMeasurement(const MeasurementPackage &measurement_pack) {
  /*****************************************************************************
   *  Initialization
   ****************************************************************************/
  if (!is_initialized_) {
    /**
     TODO:
     * Initialize the state ekf_.x_ with the first measurement.
     * Create the covariance matrix.
     * Remember: you'll need to convert radar from polar to cartesian coordinates.
     */
    
    // first measurement
    cout << "EKF: " << endl;
    float px=0;
    float py=0;
    ekf_.x_ = VectorXd(4);
    ekf_.x_ << 1, 1, 1, 1;
    //intialized jacobin matrix
    //    Hj_=tools.CalculateJacobian(ekf_.x_);
    //state covariance matrix P
    ekf_.P_ = MatrixXd(4, 4);
    ekf_.P_ << 1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1000, 0,
    0, 0, 0, 1000;
    
    
    if (measurement_pack.sensor_type_ == MeasurementPackage::RADAR) {
      
      /**
       Convert radar from polar to cartesian coordinates and initialize state.
       */
      
      float rho=measurement_pack.raw_measurements_[0];
      float phi=measurement_pack.raw_measurements_[1];
      px=rho*cos(phi);
      py=rho*sin(phi);
      
      
    }
    else if (measurement_pack.sensor_type_ == MeasurementPackage::LASER) {
      /**
       Initialize state.
       */
      px=measurement_pack.raw_measurements_[0];
      py=measurement_pack.raw_measurements_[1];
    }
    ekf_.x_<<px,py,0,0;
    // done initializing, no need to predict or update
    previous_timestamp_=measurement_pack.timestamp_;
    is_initialized_ = true;
    return;
  }
  
  /*****************************************************************************
   *  Prediction
   ****************************************************************************/
  
  /**
   TODO:
   * Update the state transition matrix F according to the new elapsed time.
   - Time is measured in seconds.
   * Update the process noise covariance matrix.
   */
  //the initial transition F matrix
  ekf_.F_=MatrixXd(4,4);
  ekf_.F_<<1,0,1,0,
  0,1,0,1,
  0,0,1,0,
  0,0,0,1;
  
  float dt=(measurement_pack.timestamp_-previous_timestamp_)/1000000.0;
  cout<<dt<<endl;
  previous_timestamp_=measurement_pack.timestamp_;
  float dt2=dt*dt;
  float dt3=dt2*dt;
  float dt4=dt3*dt;
  ekf_.F_(0,2)=dt;
  ekf_.F_(1,3)=dt;
  ekf_.Q_=MatrixXd(4,4);
  ekf_.Q_<<dt4/4*noise_ax,0,dt3/2*noise_ax,0,
  0,dt4/4*noise_ay,0,dt3/2*noise_ay,
  dt3/2*noise_ax,0,dt2*noise_ax,0,
  0,dt3/2*noise_ay,0,dt2*noise_ay;
  
  
  ekf_.Predict();
  
  /*****************************************************************************
   *  Update
   ****************************************************************************/
  
  /**
   TODO:
   * Use the sensor type to perform the update step.
   * Update the state and covariance matrices.
   */
  
  if (measurement_pack.sensor_type_ == MeasurementPackage::RADAR) {
    // Radar updates
    ekf_.R_=R_radar_;
    ekf_.H_=tools.CalculateJacobian(ekf_.x_);
    ekf_.UpdateEKF(measurement_pack.raw_measurements_);
    
  } else {
    // Laser updates
    ekf_.R_=R_laser_;
    ekf_.H_=H_laser_;
    ekf_.Update(measurement_pack.raw_measurements_);
  }
  
  // print the output
  cout << "x_ = " << ekf_.x_ << endl;
  cout << "P_ = " << ekf_.P_ << endl;
}
