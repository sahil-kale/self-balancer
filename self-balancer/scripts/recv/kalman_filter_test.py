import numpy as np
from generated.messages.imu import imu_pb2

class KalmanFilter:
    def __init__(self):
        # Initialize matrices
        self.H = np.array([
            [1, 0, 0, 0, 0, 0],
            [0, 1, 0, 0, 0, 0],
            [0, 0, 0, 0, 0, 0],
        ])

        self.F_raw = [
            [1, 0, 0, "-dt", 0, 0],
            [0, 1, 0, 0, "-dt", 0],
            [0, 0, 1, 0, 0, "-dt"],
            [0, 0, 0, 1, 0, 0],
            [0, 0, 0, 0, 1, 0],
            [0, 0, 0, 0, 0, 1]
        ]

        self.B_raw = [
            ["dt", 0, 0],
            [0, "dt", 0],
            [0, 0, 0],
            [0, 0, 0],
            [0, 0, 0],
            [0, 0, 0]
        ]

        self.P = np.array([
            [9999, 0, 0, 0, 0, 0],
            [0, 9999, 0, 0, 0, 0],
            [0, 0, 9999, 0, 0, 0],
            [0, 0, 0, 9999, 0, 0],
            [0, 0, 0, 0, 9999, 0],
            [0, 0, 0, 0, 0, 9999]
        ])

        self.Q_raw = np.array([
            [1, 1, 1, 1, 1, 1]
        ])

        self.R_raw = np.array([
            [1, 0, 0],
            [0, 1, 0],
            [0, 0, 1]
        ])

        self.X = np.array([0, 0, 0, 0, 0, 0])

    def compute_matrix_with_dt(self, matrix, dt):
        # Replace "-dt" with -dt and "dt" with dt
        matrix_modified = []
        for row in matrix:
            new_row = []
            for element in row:
                if element == "-dt":
                    new_row.append(-dt)
                elif element == "dt":
                    new_row.append(dt)
                else:
                    new_row.append(element)
            matrix_modified.append(new_row)

        # Convert to a numpy array
        matrix_array = np.array(matrix_modified)
        return matrix_array

    def kalman_filter_predict(self, X, F, B, U, P, Q):
        # Predict the state
        X = np.matmul(F, X) + np.matmul(B, U)
        # Predict the error covariance
        P = np.matmul(F, np.matmul(P, F.T)) + Q
        return X, P

    def kalman_filter_update(self, X, P, Z, R, H):
        # Compute the innovation
        Y = Z - np.matmul(H, X)
        # Compute the innovation covariance
        S = np.matmul(H, np.matmul(P, H.T)) + R
        # Compute the Kalman gain
        K = np.matmul(P, np.matmul(H.T, np.linalg.inv(S)))
        # Update the state estimate
        X = X + np.matmul(K, Y)
        # Update the error covariance
        P = np.matmul((np.eye(6) - np.matmul(K, H)), P)
        return X, P

    def run(self, msg: imu_pb2):
        dt = 1 / 1000
        F = self.compute_matrix_with_dt(self.F_raw, dt)
        B = self.compute_matrix_with_dt(self.B_raw, dt)
        # Define the process vector as a 1x6 matrix where the first 3 elements are the gyro measurements
        U = np.array([msg.Gyro.x_dps, msg.Gyro.y_dps, msg.Gyro.z_dps])
        Q = self.Q_raw

        self.X, self.P = self.kalman_filter_predict(self.X, F, B, U, self.P, Q)

        # Compute the euler angles from the accelerometer measurements
        theta_x = np.arctan2(msg.Accel.y_m_per_s_squared,
                             (np.sqrt(msg.Accel.x_m_per_s_squared ** 2 + msg.Accel.z_m_per_s_squared ** 2)))
        theta_y = np.arctan2(-msg.Accel.x_m_per_s_squared, msg.Accel.z_m_per_s_squared)
        theta_z = 0

        # convert to degrees
        theta_x = np.degrees(theta_x)
        theta_y = np.degrees(theta_y)
        theta_z = np.degrees(theta_z)

        Z = np.array([theta_x, theta_y, theta_z])

        self.X, self.P = self.kalman_filter_update(self.X, self.P, Z, self.R_raw, self.H)
        
    def plot(self):
        # in real time, plot the data
        
