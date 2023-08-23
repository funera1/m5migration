float gx_ofs, gy_ofs, gz_ofs;
float ax_ofs, ay_ofs, az_ofs;

void calibration() {
  float raw_gx, raw_gy, raw_gz;
  float gx_sum = 0, gy_sum = 0, gz_sum = 0;
  float raw_ax, raw_ay, raw_az;
  float ax_sum = 0, ay_sum = 0, az_sum = 0;

  const int count = 1000;

  for (int i = 0; i < count; i++) {
    M5.IMU.getGyroData(&raw_gx, &raw_gy, &raw_gz);
    M5.IMU.getAccelData(&raw_ax, &raw_ay, &raw_az);
    gx_sum += raw_gx;
    gy_sum += raw_gy;
    gz_sum += raw_gz;
    ax_sum += raw_ax;
    ay_sum += raw_ay;
    az_sum += raw_az;
    delay(2);
  }

  gx_ofs = gx / count;
  gy_ofs = gy / count;
  gz_ofs = gz / count;
  
  ax_ofs = ax / count;
  ay_ofs = ay / count;
  az_ofs = az / count - 0.985665; // 重力加速度1G
}

void calcAngle() {
  // M5.update();
  // gyroとaccを取得
  M5.IMU.getGyroData(&gx, &gy, &gz);
  M5.IMU.getAccelData(&ax, &ay, &az);

  // ベースの角速度・加速度を引いておく
  gx -= gx_ofs;
  gy -= gy_ofs;
  gz -= gz_ofs;
  ax -= ax_ofs;
  ay -= ay_ofs;
  az -= az_ofs;

  MadgwickFilter.updateIMU(gx, gy, gz, ax, ay, az);
  pitch = MadgwickFilter.getPitch();
  roll = MadgwickFilter.getRoll();
  yaw = MadgwickFilter.getYaw();
}
